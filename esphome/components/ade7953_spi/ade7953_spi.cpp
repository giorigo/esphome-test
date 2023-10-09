#include "ade7953_spi.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ade7953_spi {

static const char *const TAG = "ade7953";

void AdE7953Spi::dump_config() {
  ESP_LOGCONFIG(TAG, "ADE7953_spi:");
  LOG_PIN("  CS Pin: ", this->cs_);
  ade7953_base::ADE7953::dump_config();
}

bool AdE7953Spi::ade_write_8(uint16_t reg, uint8_t value) {
  this->enable();
  this->write_byte16(reg);
  this->transfer_byte(0);
  this->transfer_byte(value);
  this->disable();
  return false;
}

bool AdE7953Spi::ade_write_16(uint16_t reg, uint16_t value) {
  this->enable();
  this->write_byte16(reg);
  this->transfer_byte(0);
  this->write_byte16(value);
  this->disable();
  return false;
}

bool AdE7953Spi::ade_write_32(uint16_t reg, uint32_t value) {
  this->enable();
  this->write_byte16(reg);
  this->transfer_byte(0);
  this->write_byte16(value >> 16);
  this->write_byte16(value & 0xFFFF);
  this->disable();
  return false;
}

bool AdE7953Spi::ade_read_8(uint16_t reg, uint8_t *value) {
  this->enable();
  this->write_byte16(reg);
  this->transfer_byte(0x80);
  *value = this->read_byte();
  this->disable();
  return false;
}

bool AdE7953Spi::ade_read_16(uint16_t reg, uint16_t *value) {
  this->enable();
  this->write_byte16(reg);
  this->transfer_byte(0x80);
  *value = 0;
  *value |= this->read_byte() << 8;
  *value |= this->read_byte();
  this->disable();
  return false;
}

bool AdE7953Spi::ade_read_32(uint16_t reg, uint32_t *value) {
  this->enable();
  this->write_byte16(reg);
  this->transfer_byte(0x80);
  *value = 0;
  *value |= this->read_byte() << 24;
  *value |= this->read_byte() << 16;
  *value |= this->read_byte() << 8;
  *value |= this->read_byte();
  this->disable();
  return false;
}

}  // namespace ade7953_spi
}  // namespace esphome
