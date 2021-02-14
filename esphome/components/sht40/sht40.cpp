#include "sht40.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sht40 {

static const char *TAG = "sht40";

static const uint8_t MEASURECOMMANDS[] = {0xFD, 0xF6, 0xE0};

void SHT40Component::start_heater_() {
  uint8_t cmd[] = {MEASURECOMMANDS[this->heater_command_]};

  ESP_LOGD(TAG, "Heater turning on");
  this->write_bytes_raw(cmd, 1);
}

void SHT40Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up sht40...");

  uint32_t heater_interval;

  // Calculate time interval for heater
  if (this->duty_cycle_ > 0) {
    heater_interval = (uint32_t)(this->heater_time_ / this->duty_cycle_);
  } else {
    heater_interval = 0;
  }
  ESP_LOGD(TAG, "Heater interval: %i", heater_interval);

  if (this->heater_power_ == SHT40_HEATERPOWER_HIGH) {
    if (this->heater_time_ == SHT40_HEATERTIME_LONG) {
      this->heater_command_ = 0x39;
    } else {
      this->heater_command_ = 0x32;
    }
  } else if (this->heater_power_ == SHT40_HEATERPOWER_MED) {
    if (this->heater_time_ == SHT40_HEATERTIME_LONG) {
      this->heater_command_ = 0x2F;
    } else {
      this->heater_command_ = 0x24;
    }
  } else {
    if (this->heater_time_ == SHT40_HEATERTIME_LONG) {
      this->heater_command_ = 0x1E;
    } else {
      this->heater_command_ = 0x15;
    }
  }
  ESP_LOGD(TAG, "Heater command: %x", this->heater_command_);

  this->set_interval(heater_interval, std::bind(&SHT40Component::start_heater_, this));
}

void SHT40Component::dump_config() { LOG_I2C_DEVICE(this); }

void SHT40Component::update() {
  uint8_t cmd[] = {MEASURECOMMANDS[this->precision_]};

  // Send command
  this->write_bytes_raw(cmd, 1);

  this->set_timeout(10, [this]() {
    const uint8_t num_bytes = 6;
    uint8_t buffer[num_bytes];

    // Read measurement
    bool read_status = this->read_bytes_raw(buffer, num_bytes);

    if (read_status) {
      // Evaluate and publish measurements
      if (this->temp_sensor_ != nullptr) {
        // Temp is contained in the first 16 bits
        float sensor_value_temp = (buffer[0] << 8) + buffer[1];
        float temp = -45 + 175 * sensor_value_temp / 65535;

        this->temp_sensor_->publish_state(temp);
      }

      if (this->humidity_sensor_ != nullptr) {
        // Relative humidity is in the last 16 bits
        float sensor_value_rh = (buffer[3] << 8) + buffer[4];
        float rh = -6 + 125 * sensor_value_rh / 65535;

        this->humidity_sensor_->publish_state(rh);
      }
    } else {
      ESP_LOGD(TAG, "Sensor read failed");
    }
  });
}

}  // namespace sht40
}  // namespace esphome
