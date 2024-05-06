#pragma once

#ifdef USE_ESP_IDF

#include "i2c_bus.h"
#include "esphome/core/component.h"
#include <driver/i2c.h>

namespace esphome {
namespace i2c {

class IDFI2CBus : public I2CBus, public Component {
 public:
  void setup() override;
  void dump_config() override;
  ErrorCode readv(uint8_t address, ReadBuffer *buffers, size_t cnt) override;
  ErrorCode writev(uint8_t address, WriteBuffer *buffers, size_t cnt, bool stop) override;
  float get_setup_priority() const override { return setup_priority::BUS; }
  RecoveryCode recover() override;

  void set_scan(bool scan) { scan_ = scan; }
  void set_sda_pin(uint8_t sda_pin) { sda_pin_ = sda_pin; }
  void set_sda_pullup_enabled(bool sda_pullup_enabled) { sda_pullup_enabled_ = sda_pullup_enabled; }
  void set_scl_pin(uint8_t scl_pin) { scl_pin_ = scl_pin; }
  void set_scl_pullup_enabled(bool scl_pullup_enabled) { scl_pullup_enabled_ = scl_pullup_enabled; }
  void set_frequency(uint32_t frequency) { frequency_ = frequency; }
  void set_timeout(uint32_t timeout) { timeout_ = timeout; }

 protected:
  i2c_port_t port_;
  uint8_t sda_pin_;
  bool sda_pullup_enabled_;
  uint8_t scl_pin_;
  bool scl_pullup_enabled_;
  uint32_t frequency_;
  uint32_t timeout_ = 0;
  bool initialized_ = false;
};

}  // namespace i2c
}  // namespace esphome

#endif  // USE_ESP_IDF
