#pragma once

#include "ble_characteristic.h"
#include "esphome/components/esp32_ble/ble_uuid.h"

#include <memory>
#include <vector>

#ifdef USE_ESP32

#include <esp_bt_defs.h>
#include <esp_gap_ble_api.h>
#include <esp_gatt_defs.h>
#include <esp_gattc_api.h>
#include <esp_gatts_api.h>

namespace esphome {
namespace esp32_ble_server {

class BLEServer;

using namespace esp32_ble;

class BLEService : public std::enable_shared_from_this<BLEService> {
 public:
  BLEService(ESPBTUUID uuid, uint16_t num_handles, uint8_t inst_id, bool advertise);

  std::shared_ptr<BLECharacteristic> get_characteristic(ESPBTUUID uuid);
  std::shared_ptr<BLECharacteristic> get_characteristic(uint16_t uuid);

  std::shared_ptr<BLECharacteristic> create_characteristic(const char *uuid, size_t len,
                                                           esp_gatt_char_prop_t properties);
  std::shared_ptr<BLECharacteristic> create_characteristic(const std::string &uuid, esp_gatt_char_prop_t properties);
  std::shared_ptr<BLECharacteristic> create_characteristic(uint16_t uuid, esp_gatt_char_prop_t properties);
  std::shared_ptr<BLECharacteristic> create_characteristic(ESPBTUUID uuid, esp_gatt_char_prop_t properties);

  ESPBTUUID get_uuid() { return this->uuid_; }
  std::shared_ptr<BLECharacteristic> get_last_created_characteristic() { return this->last_created_characteristic_; }
  uint16_t get_handle() { return this->handle_; }

  BLEServer *get_server() { return this->server_; }

  void do_create(BLEServer *server);
  void do_delete();
  void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

  void start();
  void stop();

  bool is_created();
  bool is_failed();

  bool is_running() { return this->running_state_ == RUNNING; }
  bool is_starting() { return this->running_state_ == STARTING; }
  bool is_deleted() { return this->init_state_ == DELETED; }

 protected:
  std::vector<std::shared_ptr<BLECharacteristic>> characteristics_;
  std::shared_ptr<BLECharacteristic> last_created_characteristic_{nullptr};
  uint32_t created_characteristic_count_{0};
  BLEServer *server_;
  ESPBTUUID uuid_;
  uint16_t num_handles_;
  uint16_t handle_{0xFFFF};
  uint8_t inst_id_;
  bool advertise_{false};
  bool should_start_{false};

  bool do_create_characteristics_();
  void stop_();

  enum InitState : uint8_t {
    FAILED = 0x00,
    INIT,
    CREATING,
    CREATING_DEPENDENTS,
    CREATED,
    DELETING,
    DELETED,
  } init_state_{INIT};

  enum RunningState : uint8_t {
    STARTING,
    RUNNING,
    STOPPING,
    STOPPED,
  } running_state_{STOPPED};
};

}  // namespace esp32_ble_server
}  // namespace esphome

#endif
