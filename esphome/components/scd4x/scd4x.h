#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace scd4x {

enum ERRORCODE { COMMUNICATION_FAILED, SERIAL_NUMBER_IDENTIFICATION_FAILED, MEASUREMENT_INIT_FAILED, UNKNOWN };

class SCD4XComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  float get_setup_priority() const override { return setup_priority::DATA; }
  void setup() override;
  void dump_config() override;
  void update() override;

  void set_automatic_self_calibration(bool asc) { enable_asc_ = asc; }
  void set_altitude_compensation(uint16_t altitude);
  void set_ambient_pressure_compensation(float pressure);
  void set_temperature_offset(float offset) { temperature_offset_ = offset; };

  void set_co2_sensor(sensor::Sensor *co2) { co2_sensor_ = co2; }
  void set_temperature_sensor(sensor::Sensor *temperature) { temperature_sensor_ = temperature; };
  void set_humidity_sensor(sensor::Sensor *humidity) { humidity_sensor_ = humidity; }
  void set_altidute_sensor(sensor::Sensor *altidute) { altidute_sensor_ = altidute; }
  void set_pressure_sensor(sensor::Sensor *pressure) { pressure_sensor_ = pressure; }

 protected:
  uint8_t sht_crc_(uint8_t data1, uint8_t data2);
  bool read_data_(uint16_t *data, uint8_t len);
  bool write_command_(uint16_t command);
  bool write_command_(uint16_t command, uint16_t data);
  bool update_ambient_pressure_compensation_(uint16_t pressure_in_hpa);
  bool update_altitude_compensation_(uint16_t altitude);

  ERRORCODE error_code_;

  bool initialized_{false};

  float temperature_offset_;
  uint16_t altitude_compensation_;
  bool ambient_pressure_compensation_;
  uint16_t ambient_pressure_;
  bool enable_asc_;

  sensor::Sensor *co2_sensor_{nullptr};
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};
  // used for compensation
  sensor::Sensor *altidute_sensor_{nullptr};
  sensor::Sensor *pressure_sensor_{nullptr};
};

}  // namespace scd4x
}  // namespace esphome
