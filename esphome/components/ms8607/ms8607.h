#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace ms8607 {

/**
 Temperature, pressure, and humidity sensor.

 By default, the MS8607 measures sensors at the highest resolution.
 A potential enhancement would be to expose the resolution as a configurable
 setting.  A lower resolution speeds up ADC conversion time & uses less power.
 */
class MS8607Component : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; };

  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_pressure_sensor(sensor::Sensor *pressure_sensor) { pressure_sensor_ = pressure_sensor; }
  void set_humidity_sensor(sensor::Sensor *humidity_sensor) { humidity_sensor_ = humidity_sensor; }
  /// Creates `humidity_i2c_device_` using the provided `address`
  void set_humidity_sensor_address(uint8_t address);

 protected:
  /**
   Read and store the Pressure & Temperature calibration settings from the PROM.
   Intended to be called during setup(), this will set the `failure_reason_`
   */
  bool read_calibration_values_from_prom_();

  /// Start async temperature read
  void request_read_temperature_();
  /// Process async temperature read
  void read_temperature_();
  /// start async pressure read
  void request_read_pressure_(uint32_t raw_temperature);
  /// process async pressure read
  void read_pressure_(uint32_t raw_temperature);
  void read_humidity_();
  /// use raw temperature & pressure to calculate & publish values
  void calculate_values_(uint32_t raw_temperature, uint32_t raw_pressure);

  sensor::Sensor *temperature_sensor_;
  sensor::Sensor *pressure_sensor_;
  sensor::Sensor *humidity_sensor_;

  /** Secondary I2C address for the humidity sensor, and an I2CDevice object to communicate with it.
   *
   * Uses the same I2C bus & settings as `this` object, since the MS8607 only has one set of pins. This
   * I2CDevice object is an implementation detail of the Component. The esphome configuration only
   * cares what the I2C address of the humidity sensor is. (Default is 0x40)
   */
  i2c::I2CDevice *humidity_i2c_device_;
  /// I2C address for the humidity sensor
  uint8_t humidity_sensor_address_;

  /// This device's pressure & temperature calibration values, read from PROM
  struct CalibrationValues {
    /// Pressure sensitivity | SENS-T1. [C1]
    uint16_t pressure_sensitivity;
    /// Temperature coefficient of pressure sensitivity | TCS. [C3]
    uint16_t pressure_sensitivity_temperature_coefficient;
    /// Pressure offset | OFF-T1. [C2]
    uint16_t pressure_offset;
    /// Temperature coefficient of pressure offset | TCO. [C4]
    uint16_t pressure_offset_temperature_coefficient;
    /// Reference temperature | T-REF. [C5]
    uint16_t reference_temperature;
    /// Temperature coefficient of the temperature | TEMPSENS. [C6]
    uint16_t temperature_coefficient_of_temperature;
  } calibration_values_;

  /// Possible failure reasons of this component
  enum class ErrorCode;
  /// Keep track of the reason why this component failed, to augment the dumped config
  ErrorCode error_code_;
};

}  // namespace ms8607
}  // namespace esphome
