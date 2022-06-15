#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace ezo {

enum EzoCommandType : uint8_t {
  EZO_READ = 0,
  EZO_LED = 1,
  EZO_DEVICE_INFORMATION = 2,
  EZO_SLOPE = 3,
  EZO_CALIBRATION,
  EZO_SLEEP = 4,
  EZO_I2C = 5,
  EZO_T = 6,
  EZO_CUSTOM = 7
};
static const char *const EZO_COMMAND_TYPE_STRINGS[] = {"EZO_READ",  "EZO_LED",         "EZO_DEVICE_INFORMATION",
                                                       "EZO_SLOPE", "EZO_CALIBRATION", "EZO_SLEEP",
                                                       "EZO_I2C",   "EZO_T",           "EZO_CUSTOM"};

/// This class implements support for the EZO circuits in i2c mode
class EZOSensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  void loop() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; };

  // For sending commands (needs to log therefore implementation is in cpp file)
  void send_command(const std::string &payload, EzoCommandType type, uint16_t delay_ms = 300,
                    bool response_expected = true);

  // R
  void get_state() { this->send_command("R", EzoCommandType::EZO_READ, 900); }

  // I2C
  void set_i2c() {
    this->send_command("I2c,100", EzoCommandType::EZO_I2C, 300, false);
  }  // NOLINT otherwise we get set_i2_c

  // Sleep
  void set_sleep() { this->send_command("Sleep", EzoCommandType::EZO_SLEEP, 300, false); }

  // Calibration
  void set_calibration(const std::string &point, const std::string &value) {
    std::string payload = "Cal," + point + "," + value;
    this->send_command(payload, EzoCommandType::EZO_CALIBRATION, 900);
  }
  void get_calibration() { this->send_command("Cal,?", EzoCommandType::EZO_CALIBRATION); }
  void clear_calibration() { this->send_command("Cal,clear", EzoCommandType::EZO_CALIBRATION); }
  void add_calibration_callback(std::function<void(std::string)> &&callback) {
    this->calibration_callback_.add(std::move(callback));
  }

  // Device Information
  void get_device_information() { this->send_command("i", EzoCommandType::EZO_DEVICE_INFORMATION); }
  void add_device_infomation_callback(std::function<void(std::string)> &&callback) {
    this->device_infomation_callback_.add(std::move(callback));
  }

  // Slope
  void get_slope() { this->send_command("Slope,?", EzoCommandType::EZO_SLOPE); }
  void add_slope_callback(std::function<void(std::string)> &&callback) {
    this->slope_callback_.add(std::move(callback));
  }

  // LED
  void set_led_state(bool on) {
    std::string payload = "L,";
    payload += on ? "1" : "0";
    this->send_command(payload, EzoCommandType::EZO_LED);
  }
  void get_led_state() { this->send_command("L,?", EzoCommandType::EZO_LED); }
  void add_led_state_callback(std::function<void(bool)> &&callback) { this->led_callback_.add(std::move(callback)); }

  // T
  void set_t(const std::string &value) {
    std::string to_send = "T," + value;
    this->send_command(to_send, EzoCommandType::EZO_T);
  }
  void set_tempcomp_value(float temp) { this->set_t(to_string(temp)); }  // For backwards compatibility
  void get_t() { this->send_command("T,?", EzoCommandType::EZO_T); }
  void add_t_callback(std::function<void(std::string)> &&callback) { this->t_callback_.add(std::move(callback)); }

  // Custom command
  void send_custom(const std::string &payload, uint16_t delay_ms = 300, bool response_expected = true) { this->send_command(payload, EzoCommandType::EZO_CUSTOM, delay_ms, response_expected); }
  void add_custom_callback(std::function<void(std::string)> &&callback) {
    this->custom_callback_.add(std::move(callback));
  }

 protected:
  CallbackManager<void(std::string)> device_infomation_callback_{};
  CallbackManager<void(std::string)> calibration_callback_{};
  CallbackManager<void(std::string)> slope_callback_{};
  CallbackManager<void(std::string)> t_callback_{};
  CallbackManager<void(std::string)> custom_callback_{};
  CallbackManager<void(bool)> led_callback_{};

  uint32_t start_time_ = 0;
  uint32_t next_command_after_ = 0;
  bool cmd_sent_ = true;
  bool cmd_completed_ = true;
  std::string cmd_payload_;
  EzoCommandType cmd_type_;
  uint16_t cmd_delay_ms_ = 0;
  bool cmd_response_expected_ = true;
};

}  // namespace ezo
}  // namespace esphome
