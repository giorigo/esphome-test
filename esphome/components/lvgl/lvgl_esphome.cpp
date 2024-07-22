#include "esphome/core/defines.h"
#ifdef USE_LVGL
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"
#include "lvgl_hal.h"
#include "lvgl_esphome.h"

static const char *const TAG = "lvgl";

namespace esphome {
namespace lvgl {

lv_event_code_t lv_custom_event;
void LvglComponent::setup() {
  esph_log_config(TAG, "LVGL Setup starts");
#if LV_USE_LOG
  lv_log_register_print_cb(log_cb);
#endif
  lv_init();
  lv_custom_event = static_cast<lv_event_code_t>(lv_event_register_id());
  auto *display = this->displays_[0];
  size_t buffer_pixels = display->get_width() * display->get_height() / this->buffer_frac_;
  auto buf_bytes = buffer_pixels * LV_COLOR_DEPTH / 8;
  auto *buf = lv_custom_mem_alloc(buf_bytes);
  if (buf == nullptr) {
    esph_log_e(TAG, "Malloc failed to allocate %zu bytes", buf_bytes);
    this->mark_failed();
    this->status_set_error("Memory allocation failure");
    return;
  }
  lv_disp_draw_buf_init(&this->draw_buf_, buf, nullptr, buffer_pixels);
  lv_disp_drv_init(&this->disp_drv_);
  this->disp_drv_.draw_buf = &this->draw_buf_;
  this->disp_drv_.user_data = this;
  this->disp_drv_.full_refresh = this->full_refresh_;
  this->disp_drv_.flush_cb = static_flush_cb;
  this->disp_drv_.rounder_cb = rounder_cb;
  switch (display->get_rotation()) {
    case display::DISPLAY_ROTATION_0_DEGREES:
      break;
    case display::DISPLAY_ROTATION_90_DEGREES:
      this->disp_drv_.sw_rotate = true;
      this->disp_drv_.rotated = LV_DISP_ROT_90;
      break;
    case display::DISPLAY_ROTATION_180_DEGREES:
      this->disp_drv_.sw_rotate = true;
      this->disp_drv_.rotated = LV_DISP_ROT_180;
      break;
    case display::DISPLAY_ROTATION_270_DEGREES:
      this->disp_drv_.sw_rotate = true;
      this->disp_drv_.rotated = LV_DISP_ROT_270;
      break;
  }
  display->set_rotation(display::DISPLAY_ROTATION_0_DEGREES);
  this->disp_drv_.hor_res = (lv_coord_t) display->get_width();
  this->disp_drv_.ver_res = (lv_coord_t) display->get_height();
  esph_log_v(TAG, "sw_rotate = %d, rotated=%d", this->disp_drv_.sw_rotate, this->disp_drv_.rotated);
  this->disp_ = lv_disp_drv_register(&this->disp_drv_);
  for (const auto &v : this->init_lambdas_)
    v(this->disp_);
  // this->display_->set_writer([](display::Display &d) { lv_timer_handler(); });
  lv_disp_trig_activity(this->disp_);
  esph_log_config(TAG, "LVGL Setup complete");
}
}  // namespace lvgl
}  // namespace esphome

size_t lv_millis(void) { return esphome::millis(); }

#if defined(USE_HOST) || defined(USE_RP2040) || defined(USE_ESP8266)
void *lv_custom_mem_alloc(size_t size) {
  auto *ptr = malloc(size);  // NOLINT
  if (ptr == nullptr) {
    esphome::esph_log_e(TAG, "Failed to allocate %zu bytes", size);
  }
  return ptr;
}
void lv_custom_mem_free(void *ptr) { return free(ptr); }                            // NOLINT
void *lv_custom_mem_realloc(void *ptr, size_t size) { return realloc(ptr, size); }  // NOLINT
#else
static unsigned cap_bits = MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT;  // NOLINT

void *lv_custom_mem_alloc(size_t size) {
  void *ptr;
  ptr = heap_caps_malloc(size, cap_bits);
  if (ptr == nullptr) {
    cap_bits = MALLOC_CAP_8BIT;
    ptr = heap_caps_malloc(size, cap_bits);
  }
  if (ptr == nullptr) {
    esphome::esph_log_e(TAG, "Failed to allocate %zu bytes", size);
    return nullptr;
  }
#ifdef ESPHOME_LOG_HAS_VERBOSE
  esphome::esph_log_v(TAG, "allocate %zu - > %p", size, ptr);
#endif
  return ptr;
}

void lv_custom_mem_free(void *ptr) {
#ifdef ESPHOME_LOG_HAS_VERBOSE
  esphome::esph_log_v(TAG, "free %p", ptr);
#endif
  if (ptr == nullptr)
    return;
  heap_caps_free(ptr);
}

void *lv_custom_mem_realloc(void *ptr, size_t size) {
#ifdef ESPHOME_LOG_HAS_VERBOSE
  esphome::esph_log_v(TAG, "realloc %p: %zu", ptr, size);
#endif
  return heap_caps_realloc(ptr, size, cap_bits);
}
#endif

#endif
