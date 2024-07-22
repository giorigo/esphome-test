#pragma once
#include "esphome/core/defines.h"
#ifdef USE_LVGL

// required for clang-tidy
#ifndef LV_CONF_H
#define LV_CONF_SKIP 1  // NOLINT
#endif

#include "esphome/components/display/display.h"
#include "esphome/components/key_provider/key_provider.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/components/display/display_color_utils.h"
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "lvgl_hal.h"
#include <lvgl.h>
#include <vector>
#include <map>

#ifdef LVGL_USES_FONT
#include "esphome/components/font/font.h"
#endif
namespace esphome {
namespace lvgl {

#ifdef LVGL_USES_COLOR
static lv_color_t lv_color_from(Color color) { return lv_color_make(color.red, color.green, color.blue); }
#endif
#if LV_COLOR_DEPTH == 16
static const display::ColorBitness LV_BITNESS = display::ColorBitness::COLOR_BITNESS_565;
#elif LV_COLOR_DEPTH == 32
static const display::ColorBitness LV_BITNESS = display::ColorBitness::COLOR_BITNESS_888;
#else
static const display::ColorBitness LV_BITNESS = display::ColorBitness::COLOR_BITNESS_332;
#endif

// Parent class for things that wrap an LVGL object
class LvCompound {
 public:
  virtual void set_obj(lv_obj_t *lv_obj) { this->obj = lv_obj; }
  lv_obj_t *obj{};
};

typedef std::function<void(lv_obj_t *)> LvLambdaType;
typedef std::function<void(float)> set_value_lambda_t;
typedef void(event_callback_t)(_lv_event_t *);
typedef std::function<const char *(void)> text_lambda_t;

#ifdef LVGL_USES_FONT
class FontEngine {
 public:
  FontEngine(font::Font *esp_font);
  const lv_font_t *get_lv_font();

  const font::GlyphData *get_glyph_data(uint32_t unicode_letter);
  uint16_t baseline{};
  uint16_t height{};
  uint8_t bpp{};

 protected:
  font::Font *font_{};
  uint32_t last_letter_{};
  const font::GlyphData *last_data_{};
  lv_font_t lv_font_{};
};
#endif  // LVGL_USES_FONT

class LvglComponent : public PollingComponent {
  constexpr static const char *const TAG = "lvgl";

 public:
  static void static_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    reinterpret_cast<LvglComponent *>(disp_drv->user_data)->flush_cb_(disp_drv, area, color_p);
  }

  float get_setup_priority() const override { return setup_priority::PROCESSOR; }
  static void log_cb(const char *buf) {
    esp_log_printf_(ESPHOME_LOG_LEVEL_INFO, TAG, 0, "%.*s", (int) strlen(buf) - 1, buf);
  }
  static void rounder_cb(lv_disp_drv_t *disp_drv, lv_area_t *area) {
    // make sure all coordinates are even
    if (area->x1 & 1)
      area->x1--;
    if (!(area->x2 & 1))
      area->x2++;
    if (area->y1 & 1)
      area->y1--;
    if (!(area->y2 & 1))
      area->y2++;
  }

  void loop() override { lv_timer_handler_run_in_period(5); }
  void setup() override;

  void update() override {}

  void add_display(display::Display *display) { this->displays_.push_back(display); }
  void add_init_lambda(std::function<void(lv_disp_t *)> lamb) { this->init_lambdas_.push_back(lamb); }
  void dump_config() override { esph_log_config(TAG, "LVGL:"); }
  void set_full_refresh(bool full_refresh) { this->full_refresh_ = full_refresh; }
  void set_buffer_frac(size_t frac) { this->buffer_frac_ = frac; }
  lv_disp_t *get_disp() { return this->disp_; }

 protected:
  void draw_buffer_(const lv_area_t *area, const uint8_t *ptr) {
    for (auto display : this->displays_) {
      display->draw_pixels_at(area->x1, area->y1, lv_area_get_width(area), lv_area_get_height(area), ptr,
                              display::COLOR_ORDER_RGB, LV_BITNESS, LV_COLOR_16_SWAP);
    }
  }

  void flush_cb_(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    auto now = millis();
    this->draw_buffer_(area, (const uint8_t *) color_p);
    esph_log_v(TAG, "flush_cb, area=%d/%d, %d/%d took %dms", area->x1, area->y1, lv_area_get_width(area),
               lv_area_get_height(area), (int) (millis() - now));
    lv_disp_flush_ready(disp_drv);
  }

  std::vector<display::Display *> displays_{};
  lv_disp_draw_buf_t draw_buf_{};
  lv_disp_drv_t disp_drv_{};
  lv_disp_t *disp_{};

  std::vector<std::function<void(lv_disp_t *)>> init_lambdas_;
  size_t buffer_frac_{1};
  bool full_refresh_{};
};

}  // namespace lvgl
}  // namespace esphome

#endif
