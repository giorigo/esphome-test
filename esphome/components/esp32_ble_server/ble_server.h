#pragma once

#include "ble_service.h"
#include "ble_characteristic.h"

#include "esphome/components/esp32_ble/ble.h"
#include "esphome/components/esp32_ble/ble_advertising.h"
#include "esphome/components/esp32_ble/ble_uuid.h"
#include "esphome/components/esp32_ble/queue.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"

#include <memory>
#include <vector>
#include <unordered_map>

#ifdef USE_ESP32

#include <esp_gap_ble_api.h>
#include <esp_gatts_api.h>

namespace esphome {
namespace esp32_ble_server {

using namespace esp32_ble;

class BLEServerAutomationInterface {
 public:
  static Trigger<std::string> *create_on_write_trigger(BLECharacteristic *characteristic);
  
  template<typename... Ts> class BLECharacteristicSetValueAction : public Action<Ts...> {
  public:
    BLECharacteristicSetValueAction(BLECharacteristic *characteristic) : parent_(characteristic) {}
    TEMPLATABLE_VALUE(std::string, value)
    void play(Ts... x) override {this->parent_->set_value(this->value_.value(x...)); }

  protected:
    BLECharacteristic *parent_;
  };
};


class BLEServer : public Component, public GATTsEventHandler, public BLEStatusEventHandler, public Parented<ESP32BLE> {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;
  bool can_proceed() override;

  void teardown();
  bool is_running();

  void set_manufacturer(const std::string &manufacturer) { this->manufacturer_ = manufacturer; }
  void set_model(const std::string &model) { this->model_ = model; }
  void set_manufacturer_data(const std::vector<uint8_t> &data) {
    this->manufacturer_data_ = data;
    this->restart_advertising_();
  }

  BLEService *create_service(ESPBTUUID uuid, bool advertise = false, uint16_t num_handles = 15);
  void remove_service(ESPBTUUID uuid, uint8_t inst_id = 0);
  BLEService *get_service(ESPBTUUID uuid, uint8_t inst_id = 0);
  void enqueue_start_service(BLEService *service) { this->services_to_start_.push_back(service); }

  esp_gatt_if_t get_gatts_if() { return this->gatts_if_; }
  uint32_t get_connected_client_count() { return this->connected_clients_; }
  const std::unordered_map<uint16_t, void *> &get_clients() { return this->clients_; }

  void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                           esp_ble_gatts_cb_param_t *param) override;

  void ble_before_disabled_event_handler() override;

 protected:
  static std::string get_service_key(ESPBTUUID uuid, uint8_t inst_id);
  bool create_device_characteristics_();
  void restart_advertising_();

  void add_client_(uint16_t conn_id, void *client) { this->clients_.emplace(conn_id, client); }
  bool remove_client_(uint16_t conn_id) { return this->clients_.erase(conn_id) > 0; }

  std::string manufacturer_;
  optional<std::string> model_;
  std::vector<uint8_t> manufacturer_data_;
  esp_gatt_if_t gatts_if_{0};
  bool registered_{false};

  uint32_t connected_clients_{0};
  std::unordered_map<uint16_t, void *> clients_;
  std::unordered_map<std::string, BLEService *> services_;
  std::vector<BLEService *> services_to_start_;
  BLEService *device_information_service_;

  enum State : uint8_t {
    INIT = 0x00,
    REGISTERING,
    STARTING_SERVICE,
    RUNNING,
  } state_{INIT};
};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern BLEServer *global_ble_server;

}  // namespace esp32_ble_server
}  // namespace esphome

#endif
