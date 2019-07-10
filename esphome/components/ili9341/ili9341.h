#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"
#include "ili9341_defines.h"
#include "ili9341_init.h"

namespace esphome {
namespace ili9341 {

enum ILI9341Model {
  M5STACK = 0,
};

class ili9341 : public PollingComponent,
                public display::DisplayBuffer,
                public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW, spi::CLOCK_PHASE_LEADING,
                                      spi::DATA_RATE_8MHZ> {
 public:
  void set_dc_pin(GPIOPin *dc_pin) { dc_pin_ = dc_pin; }
  float get_setup_priority() const override;
  void set_reset_pin(GPIOPin *reset) { this->reset_pin_ = reset; }
  void set_led_pin(GPIOPin *led) { this->led_pin_ = led; }
  void set_model(ILI9341Model model) { this->model_ = model; }

  void command(uint8_t value);
  void data(uint8_t value);
  void send_command(uint8_t commandByte, const uint8_t *dataBytes, uint8_t numDataBytes);
  uint8_t read_command_(uint8_t commandByte, uint8_t index);
  virtual void initialize() = 0;

  void update() override;

  void fill(int color) override;

  void setup() override {
    this->setup_pins_();
    this->initialize();
  }

 protected:
  void draw_absolute_pixel_internal(int x, int y, int color) override;
  void setup_pins_();

  void init_lcd_(const uint8_t *init_cmd);
  void set_addr_window_(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
  void invert_display_(bool invert);
  void reset_();
  void fill_internal_(int color);

  ILI9341Model model_;
  int16_t width_{320};   ///< Display width as modified by current rotation
  int16_t height_{240};  ///< Display height as modified by current rotation
  uint16_t x_low_{0};
  uint16_t y_low_{0};
  uint16_t x_high_{0};
  uint16_t y_high_{0};

  uint32_t get_buffer_length_();

  void start_command_();
  void end_command_();
  void start_data_();
  void end_data_();

  GPIOPin *reset_pin_{nullptr};
  GPIOPin *led_pin_{nullptr};
  GPIOPin *dc_pin_;
  GPIOPin *busy_pin_{nullptr};
};

class ili9341_M5Stack : public ili9341 {
 public:
  void initialize() override;

  void dump_config() override;
  void set_rotation_();
  int get_width_internal() override;
  int get_height_internal() override;

 protected:
  uint32_t at_update_{0};
};

}  // namespace ili9341
}  // namespace esphome
