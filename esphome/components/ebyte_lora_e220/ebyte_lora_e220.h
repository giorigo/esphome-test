#pragma once
#include <utility>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/helpers.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ebyte_lora_e220 {

static const char *const TAG = "ebyte_lora_e220";

// the mode the receiver is in
enum MODE_TYPE {
  MODE_0_NORMAL = 0,
  MODE_0_TRANSMISSION = 0,
  MODE_1_WOR_TRANSMITTER = 1,
  MODE_1_WOR = 1,
  MODE_2_WOR_RECEIVER = 2,
  MODE_2_POWER_SAVING = 2,
  MODE_3_CONFIGURATION = 3,
  MODE_3_PROGRAM = 3,
  MODE_3_SLEEP = 3,
  MODE_INIT = 0xFF
};
class EbyteLoraE220 : public PollingComponent, public uart::UARTDevice {
 public:
  void setup() override;
  void update() override;
  void loop() override;
  void dump_config() override;
  // local
  void set_latitude_sensor(sensor::Sensor *latitude_sensor) { latitude_sensor_ = latitude_sensor; }
  void set_longitude_sensor(sensor::Sensor *longitude_sensor) { longitude_sensor_ = longitude_sensor; }
  void set_message_sensor(text_sensor::TextSensor *s) { message_text_sensor = s; }
  void set_rssi_sensor(sensor::Sensor *s) { rssi_sensor = s; }
  void set_pin_aux(GPIOPin *s) { pin_aux = s; }
  void set_pin_m0(GPIOPin *s) { pin_m0 = s; }
  void set_pin_m1(GPIOPin *s) { pin_m1 = s; }

 private:
  MODE_TYPE mode = MODE_0_NORMAL;
  // set WOR mode
  bool setMode(MODE_TYPE type);
  // checks the aux port to see if it is done setting
  bool waitCompleteResponse(unsigned long timeout = 1000, unsigned int waitNoAux = 100);

 protected:
  int rssi_ = 0;
  float latitude_ = -1;
  float longitude_ = -1;
  std::string raw_message_;
  sensor::Sensor *latitude_sensor_{nullptr};
  sensor::Sensor *longitude_sensor_{nullptr};
  text_sensor::TextSensor *message_text_sensor{nullptr};
  sensor::Sensor *rssi_sensor{nullptr};
  GPIOPin *pin_aux{nullptr};
  GPIOPin *pin_m0{nullptr};
  GPIOPin *pin_m1{nullptr};
};

}  // namespace ebyte_lora_e220
}  // namespace esphome
