#ifdef USE_ESP32

#include "esp32_touch.h"
#include "esphome/core/application.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace esp32_touch {

static const char *const TAG = "esp32_touch";

void ESP32TouchComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ESP32 Touch Hub...");
  touch_pad_init();

#if defined(USE_ESP32) && !defined(USE_ESP32_VARIANT_ESP32S2)
  if (this->iir_filter_enabled_()) {
    touch_pad_filter_start(this->iir_filter_);
  }
#endif

  touch_pad_set_meas_time(this->sleep_cycle_, this->meas_cycle_);
  touch_pad_set_voltage(this->high_voltage_reference_, this->low_voltage_reference_, this->voltage_attenuation_);

  for (auto *child : this->children_) {
    // Disable interrupt threshold
#if defined(USE_ESP32) && defined(USE_ESP32_VARIANT_ESP32S2)
    touch_pad_config(child->get_touch_pad());
    touch_pad_set_cnt_mode(child->get_touch_pad(), TOUCH_PAD_SLOPE_DEFAULT, TOUCH_PAD_TIE_OPT_DEFAULT);
#else
    touch_pad_config(child->get_touch_pad(), 0);
#endif
  }

#if defined(USE_ESP32) && defined(USE_ESP32_VARIANT_ESP32S2)
  touch_pad_set_meas_time(TOUCH_PAD_SLEEP_CYCLE_DEFAULT, TOUCH_PAD_SLEEP_CYCLE_DEFAULT);
  touch_pad_set_voltage(TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD,
                        TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD);
  touch_pad_set_idle_channel_connect(TOUCH_PAD_IDLE_CH_CONNECT_DEFAULT);

  touch_pad_denoise_t denoise = {
      /* The bits to be cancelled are determined according to the noise level. */
      .grade = TOUCH_PAD_DENOISE_BIT4,
      .cap_level = TOUCH_PAD_DENOISE_CAP_L4,
  };
  touch_pad_denoise_set_config(&denoise);
  touch_pad_denoise_enable();
  touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
  touch_pad_fsm_start();
#endif
}

void ESP32TouchComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Config for ESP32 Touch Hub:");
  ESP_LOGCONFIG(TAG, "  Meas cycle: %.2fms", this->meas_cycle_ / (8000000.0f / 1000.0f));
  ESP_LOGCONFIG(TAG, "  Sleep cycle: %.2fms", this->sleep_cycle_ / (150000.0f / 1000.0f));

  const char *lv_s;
  switch (this->low_voltage_reference_) {
    case TOUCH_LVOLT_0V5:
      lv_s = "0.5V";
      break;
    case TOUCH_LVOLT_0V6:
      lv_s = "0.6V";
      break;
    case TOUCH_LVOLT_0V7:
      lv_s = "0.7V";
      break;
    case TOUCH_LVOLT_0V8:
      lv_s = "0.8V";
      break;
    default:
      lv_s = "UNKNOWN";
      break;
  }
  ESP_LOGCONFIG(TAG, "  Low Voltage Reference: %s", lv_s);

  const char *hv_s;
  switch (this->high_voltage_reference_) {
    case TOUCH_HVOLT_2V4:
      hv_s = "2.4V";
      break;
    case TOUCH_HVOLT_2V5:
      hv_s = "2.5V";
      break;
    case TOUCH_HVOLT_2V6:
      hv_s = "2.6V";
      break;
    case TOUCH_HVOLT_2V7:
      hv_s = "2.7V";
      break;
    default:
      hv_s = "UNKNOWN";
      break;
  }
  ESP_LOGCONFIG(TAG, "  High Voltage Reference: %s", hv_s);

  const char *atten_s;
  switch (this->voltage_attenuation_) {
    case TOUCH_HVOLT_ATTEN_1V5:
      atten_s = "1.5V";
      break;
    case TOUCH_HVOLT_ATTEN_1V:
      atten_s = "1V";
      break;
    case TOUCH_HVOLT_ATTEN_0V5:
      atten_s = "0.5V";
      break;
    case TOUCH_HVOLT_ATTEN_0V:
      atten_s = "0V";
      break;
    default:
      atten_s = "UNKNOWN";
      break;
  }
  ESP_LOGCONFIG(TAG, "  Voltage Attenuation: %s", atten_s);

  if (this->iir_filter_enabled_()) {
    ESP_LOGCONFIG(TAG, "    IIR Filter: %ums", this->iir_filter_);
  } else {
    ESP_LOGCONFIG(TAG, "  IIR Filter DISABLED");
  }
  if (this->setup_mode_) {
    ESP_LOGCONFIG(TAG, "  Setup Mode ENABLED!");
  }

  for (auto *child : this->children_) {
    LOG_BINARY_SENSOR("  ", "Touch Pad", child);
    ESP_LOGCONFIG(TAG, "    Pad: T%d", child->get_touch_pad());
    ESP_LOGCONFIG(TAG, "    Threshold: %u", child->get_threshold());
  }
}

void ESP32TouchComponent::loop() {
  const uint32_t now = millis();
  bool should_print = this->setup_mode_ && now - this->setup_mode_last_log_print_ > 250;
  for (auto *child : this->children_) {
#if defined(USE_ESP32) && defined(USE_ESP32_VARIANT_ESP32S2)
    uint32_t value;
    touch_pad_read_raw_data(child->get_touch_pad(), &value);
#else
    uint16_t value;
    if (this->iir_filter_enabled_()) {
      touch_pad_read_filtered(child->get_touch_pad(), &value);
    } else {
      touch_pad_read(child->get_touch_pad(), &value);
    }
#endif

    child->value_ = value;
#if defined(USE_ESP32) && defined(USE_ESP32_VARIANT_ESP32S2)
    child->publish_state(value >= child->get_threshold());
#else
    child->publish_state(value < child->get_threshold());
#endif

    if (should_print) {
      ESP_LOGD(TAG, "Touch Pad '%s' (T%u): %u", child->get_name().c_str(), child->get_touch_pad(), value);
    }

    App.feed_wdt();
  }

  if (should_print) {
    // Avoid spamming logs
    this->setup_mode_last_log_print_ = now;
  }
}

void ESP32TouchComponent::on_shutdown() {
  bool is_wakeup_source = false;

#if defined(USE_ESP32) && defined(USE_ESP32_VARIANT_ESP32S2)
  touch_pad_fsm_stop();
  touch_pad_filter_disable();
#else
  if (this->iir_filter_enabled_()) {
    touch_pad_filter_stop();
    touch_pad_filter_delete();
  }
#endif

  for (auto *child : this->children_) {
    if (child->get_wakeup_threshold() != 0) {
      if (!is_wakeup_source) {
        is_wakeup_source = true;
        // Touch sensor FSM mode must be 'TOUCH_FSM_MODE_TIMER' to use it to wake-up.
        touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
      }

      // No filter available when using as wake-up source.
#if defined(USE_ESP32) && defined(USE_ESP32_VARIANT_ESP32S2)
      touch_pad_config(child->get_touch_pad());
#else
      touch_pad_config(child->get_touch_pad(), child->get_wakeup_threshold());
#endif
    }
  }

  if (!is_wakeup_source) {
    touch_pad_deinit();
  }
}

ESP32TouchBinarySensor::ESP32TouchBinarySensor(touch_pad_t touch_pad, uint16_t threshold, uint16_t wakeup_threshold)
    : BinarySensor(), touch_pad_(touch_pad), threshold_(threshold), wakeup_threshold_(wakeup_threshold) {}

}  // namespace esp32_touch
}  // namespace esphome

#endif
