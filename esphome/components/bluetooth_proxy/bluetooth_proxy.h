#pragma once

#ifdef USE_ESP32

#include <map>

#include "esphome/components/esp32_ble_client/ble_client_base.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/lrucache.hpp"

#include <map>

#ifdef USE_API
#include "esphome/components/api/api_pb2.h"
#endif  // USE_API

namespace esphome {
namespace bluetooth_proxy {

using namespace esp32_ble_client;
using adv_data_t = std::vector<uint8_t>;

class BluetoothProxy : public BLEClientBase {
 public:
  BluetoothProxy();
  bool parse_device(const esp32_ble_tracker::ESPBTDevice &device) override;
  void dump_config() override;
  void loop() override;

  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;

#ifdef USE_API
  void bluetooth_device_request(const api::BluetoothDeviceRequest &msg);
  void bluetooth_gatt_read(const api::BluetoothGATTReadRequest &msg);
  void bluetooth_gatt_write(const api::BluetoothGATTWriteRequest &msg);
  void bluetooth_gatt_read_descriptor(const api::BluetoothGATTReadDescriptorRequest &msg);
  void bluetooth_gatt_write_descriptor(const api::BluetoothGATTWriteDescriptorRequest &msg);
  void bluetooth_gatt_send_services(const api::BluetoothGATTGetServicesRequest &msg);
  void bluetooth_gatt_notify(const api::BluetoothGATTNotifyRequest &msg);
#endif

  int get_bluetooth_connections_free() { return this->state_ == espbt::ClientState::IDLE ? 1 : 0; }
  int get_bluetooth_connections_limit() { return 1; }

  void set_active(bool active) { this->active_ = active; }
  void set_min_rssi(int min_rssi) { this->min_rssi_ = min_rssi; }
  bool has_active() { return this->active_; }

 protected:
  void send_api_packet_(const esp32_ble_tracker::ESPBTDevice &device);

  std::map<uint64_t, esp_ble_addr_type_t> address_type_map_;
  int16_t send_service_{-1};
  bool active_;
  int min_rssi_{-80};
  cache::lru_cache<uint64_t, adv_data_t> prev_data_(25);
};

extern BluetoothProxy *global_bluetooth_proxy;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

}  // namespace bluetooth_proxy
}  // namespace esphome

#endif  // USE_ESP32
