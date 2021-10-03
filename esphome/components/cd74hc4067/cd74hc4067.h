#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/voltage_sampler/voltage_sampler.h"

namespace esphome {
namespace cd74hc4067 {

class CD74HC4067Component : public Component {
 public:
  /// Set up the internal sensor array.
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  /// set the pin connected to multiplexer control pin 0
  void set_pin_s0(InternalGPIOPin *pin) { this->pin_s0_ = pin; }
  /// set the pin connected to multiplexer control pin 1
  void set_pin_s1(InternalGPIOPin *pin) { this->pin_s1_ = pin; }
  /// set the pin connected to multiplexer control pin 2
  void set_pin_s2(InternalGPIOPin *pin) { this->pin_s2_ = pin; }
  /// set the pin connected to multiplexer control pin 3
  void set_pin_s3(InternalGPIOPin *pin) { this->pin_s3_ = pin; }
  /// set the pin connected to multiplexer output
  void set_pin_adc(InternalGPIOPin *pin) { this->pin_adc_ = pin; }
  /// sets the multiplexer to the desired input and reads the value from the adc pin
  float read_data_(uint8_t pin);

 private:
  /// setting pin active by setting the right combination of the four multiplexer input pins
  void activate_pin(uint8_t pin);
  InternalGPIOPin *pin_s0_;
  InternalGPIOPin *pin_s1_;
  InternalGPIOPin *pin_s2_;
  InternalGPIOPin *pin_s3_;
  InternalGPIOPin *pin_adc_;
  /// the currently active pin
  uint8_t active_pin_;
};

class CD74HC4067Sensor : public sensor::Sensor, public PollingComponent, public voltage_sampler::VoltageSampler {
 public:
  CD74HC4067Sensor(CD74HC4067Component *parent, std::string name, uint8_t pin);

  void setup() override;
  void update() override;

  void dump_config() override;
  /// `HARDWARE_LATE` setup priority.
  float get_setup_priority() const override;
  void set_pin(uint8_t pin) { this->pin_ = pin; }
  float sample() override;

  std::string unique_id() override;

 protected:
  CD74HC4067Component *parent_;
  uint8_t pin_;
};
}  // namespace cd74hc4067
}  // namespace esphome
