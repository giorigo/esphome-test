/// @file wk2168_spi.h
/// @author DrCoolZic
/// @brief  wk_base classes declaration

#pragma once
#include <bitset>
#include "esphome/components/spi/spi.h"
#include "esphome/components/wk_base/wk_base.h"

namespace esphome {
namespace wk2168_spi {

class WKGPIOPinSPI : public wk_base::WKGPIOPin {};
class WKGPIOComponentSPI;

////////////////////////////////////////////////////////////////////////////////////
// class WK2168RegSPI
////////////////////////////////////////////////////////////////////////////////////
class WK2168RegSPI : public wk_base::WKBaseRegister {
 public:
  uint8_t read_reg() const override;
  void write_reg(uint8_t value) override;
  void read_fifo(uint8_t *data, size_t length) const override;
  void write_fifo(uint8_t *data, size_t length) override;

 protected:
  friend WKGPIOComponentSPI;
  WK2168RegSPI(wk_base::WKGPIOComponent *const comp, uint8_t reg, uint8_t channel)
      : WKBaseRegister(comp, reg, channel) {}
};

////////////////////////////////////////////////////////////////////////////////////
// class WKGPIOComponentSPI
////////////////////////////////////////////////////////////////////////////////////

/// @brief WKGPIOComponent using SPI bus
class WKGPIOComponentSPI : public wk_base::WKGPIOComponent,
                           public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                                 spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_1MHZ> {
 public:
  wk_base::WKBaseRegister &reg(uint8_t reg, uint8_t channel) override {
    reg_spi_.register_ = reg;
    reg_spi_.channel_ = channel;
    return reg_spi_;
  }

  void setup() override;
  void dump_config() override;

 protected:
  friend class WK2168RegSPI;
  WK2168RegSPI reg_spi_{this, 0, 0};  ///< store the current register/channel
};

}  // namespace wk2168_spi
}  // namespace esphome
