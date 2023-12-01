//
// Created by Clyde Stubbs on 29/10/2023.
//
#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/panel_driver/panel_driver.h"
#include "esp_lcd_panel_ops.h"

#include "esp_lcd_panel_rgb.h"

namespace esphome {
namespace st7701s {

constexpr static const char *const TAG = "panel_driver.st7701s";
const uint8_t SW_RESET_CMD = 0x01;
const uint8_t SDIR_CMD = 0xC7;
const uint8_t MADCTL_CMD = 0x36;

class ST7701S : public panel_driver::PanelDriver,
                public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW, spi::CLOCK_PHASE_LEADING,
                                      spi::DATA_RATE_1MHZ> {
 public:
  void setup() override {
    this->spi_setup();
    esp_lcd_rgb_panel_config_t config{};
    config.flags.fb_in_psram = 1;
    config.num_fbs = 2;
    config.timings.pclk_hz = 8 * 1000 * 1000;
    config.timings.h_res = this->width_;
    config.timings.v_res = this->height_;
    config.timings.hsync_pulse_width = this->hsync_pulse_width_;
    config.timings.hsync_back_porch = this->hsync_back_porch_;
    config.timings.hsync_front_porch = this->hsync_front_porch_;
    config.timings.vsync_pulse_width = this->vsync_pulse_width_;
    config.timings.vsync_back_porch = this->vsync_back_porch_;
    config.timings.vsync_front_porch = this->vsync_front_porch_;
    config.timings.flags.pclk_active_neg = true;
    config.clk_src = LCD_CLK_SRC_PLL160M;
    config.sram_trans_align = 64;
    config.psram_trans_align = 64;
    size_t data_pin_count = sizeof(this->data_pins_) / sizeof(this->data_pins_[0]);
    for (size_t i = 0; i != data_pin_count; i++) {
      config.data_gpio_nums[i] = this->data_pins_[i]->get_pin();
    }
    config.data_width = data_pin_count;
    config.hsync_gpio_num = this->hsync_pin_->get_pin();
    config.vsync_gpio_num = this->vsync_pin_->get_pin();
    config.de_gpio_num = this->de_pin_->get_pin();
    config.pclk_gpio_num = this->pclk_pin_->get_pin();
    esp_err_t err = esp_lcd_new_rgb_panel(&config, &this->handle_);
    if (err != ESP_OK) {
      esph_log_e(TAG, "lcd_new_rgb_panel failed: %s", esp_err_to_name(err));
    }
    ESP_ERROR_CHECK(esp_lcd_panel_reset(this->handle_));
    ESP_ERROR_CHECK(esp_lcd_panel_init(this->handle_));
    this->write_init_sequence();
  }

  void draw_pixels_at(size_t x, size_t y, size_t width, size_t height, const void *src_ptr) override {
    auto err = esp_lcd_panel_draw_bitmap(this->handle_, x, y, x + width, y + height, src_ptr);
    if (err != ESP_OK)
      esph_log_e(TAG, "lcd_lcd_panel_draw_bitmap failed: %s", esp_err_to_name(err));
  }

  panel_driver::ColorMode get_color_mode() override { return this->color_mode_; }
  void set_color_mode(panel_driver::ColorMode color_mode) { this->color_mode_ = color_mode; }
  void set_invert_colors(bool invert_colors) { this->invert_colors_ = invert_colors; }

  void add_data_pin(InternalGPIOPin *data_pin, size_t index) { this->data_pins_[index] = data_pin; };
  void set_de_pin(InternalGPIOPin *de_pin) { this->de_pin_ = de_pin; }
  void set_pclk_pin(InternalGPIOPin *pclk_pin) { this->pclk_pin_ = pclk_pin; }
  void set_vsync_pin(InternalGPIOPin *vsync_pin) { this->vsync_pin_ = vsync_pin; }
  void set_hsync_pin(InternalGPIOPin *hsync_pin) { this->hsync_pin_ = hsync_pin; }
  void set_dc_pin(GPIOPin *dc_pin) { this->dc_pin_ = dc_pin; }
  void set_reset_pin(GPIOPin *reset_pin) { this->reset_pin_ = reset_pin; }
  void set_width(uint16_t width) { this->width_ = width; }
  void set_height(uint16_t height) { this->height_ = height; }
  void set_hsync_back_porch(uint16_t hsync_back_porch) { this->hsync_back_porch_ = hsync_back_porch; }
  void set_hsync_front_porch(uint16_t hsync_front_porch) { this->hsync_front_porch_ = hsync_front_porch; }
  void set_hsync_pulse_width(uint16_t hsync_pulse_width) { this->hsync_pulse_width_ = hsync_pulse_width; }
  void set_vsync_pulse_width(uint16_t vsync_pulse_width) { this->vsync_pulse_width_ = vsync_pulse_width; }
  void set_vsync_back_porch(uint16_t vsync_back_porch) { this->vsync_back_porch_ = vsync_back_porch; }
  void set_vsync_front_porch(uint16_t vsync_front_porch) { this->vsync_front_porch_ = vsync_front_porch; }
  void set_init_sequence(const std::vector<uint8_t> &init_sequence) { this->init_sequence_ = init_sequence; }
  void dump_config() override;

 protected:
  void write_command_(uint8_t value) {
    this->enable();
    if (this->dc_pin_ == nullptr) {
      this->write(value, 9);
    } else {
      this->dc_pin_->digital_write(false);
      this->write_byte(value);
      this->dc_pin_->digital_write(true);
    }
    this->disable();
  }

  void write_data_(uint8_t value) {
    this->enable();
    if (this->dc_pin_ == nullptr) {
      this->write(value | 0x100, 9);
    } else {
      this->dc_pin_->digital_write(true);
      this->write_byte(value);
    }
    this->disable();
  }

  /**
   * this relies upon the init sequence being well-formed, which is guaranteed by the Python init code.
   */
  void write_init_sequence() {
    for (size_t i = 0; i != this->init_sequence_.size();) {
      uint8_t cmd = this->init_sequence_[i++];
      this->write_command_(cmd);
      size_t len = this->init_sequence_[i++];
      esph_log_v(TAG, "Command %X, %d bytes", cmd, len);
      while (len-- != 0)
        this->write_data_(this->init_sequence_[i++]);
      if (cmd == SW_RESET_CMD)
        delay(6);
    }
    uint8_t val = 0;
    if (this->color_mode_ == panel_driver::COLOR_MODE_BGR)
      val = 0x08;
    if (this->mirror_y_)
      val |= 0x10;
    this->write_command_(MADCTL_CMD);
    this->write_data_(val);
    val = 0;
    if (this->mirror_x_)
      val = 4;
    this->write_command_(SDIR_CMD);
    this->write_data_(val);
    // don't know how to swap the axes at this time.
    this->set_timeout(120, [this] { this->write_command_(0x29); });
  }

  InternalGPIOPin *de_pin_{nullptr};
  InternalGPIOPin *pclk_pin_{nullptr};
  InternalGPIOPin *hsync_pin_{nullptr};
  InternalGPIOPin *vsync_pin_{nullptr};
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *dc_pin_{nullptr};
  InternalGPIOPin *data_pins_[16] = {};
  uint16_t hsync_pulse_width_ = 10;
  uint16_t hsync_back_porch_ = 10;
  uint16_t hsync_front_porch_ = 20;
  uint16_t vsync_pulse_width_ = 10;
  uint16_t vsync_back_porch_ = 10;
  uint16_t vsync_front_porch_ = 10;
  std::vector<uint8_t> init_sequence_;

  bool invert_colors_{};
  panel_driver::ColorMode color_mode_{panel_driver::COLOR_MODE_BGR};

  esp_lcd_panel_handle_t handle_{};
};

}  // namespace st7701s
}  // namespace esphome
