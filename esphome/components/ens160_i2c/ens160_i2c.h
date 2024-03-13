#pragma once

#include "esphome/components/ens160_base/ens160_base.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace ens160_i2c {

static const char *const TAG = "ens160_i2c.sensor";

class ENS160I2CComponent : public esphome::ens160_base::ENS160Component, public i2c::I2CDevice {
  bool read_byte(uint8_t a_register, uint8_t *data) override;
  bool write_byte(uint8_t a_register, uint8_t data) override;
  bool read_bytes(uint8_t a_register, uint8_t *data, size_t len) override;
  bool write_bytes(uint8_t a_register, uint8_t *data, size_t len) override;
  void dump_config() override;
};

}  // namespace ens160_i2c
}  // namespace esphome
