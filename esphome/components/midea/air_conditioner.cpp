#include "esphome/core/log.h"
#include "esphome/components/midea/air_conditioner.h"
#include "esphome/components/midea/adapter.h"

namespace esphome {
namespace midea {

static void set_sensor(Sensor *sensor, float value) {
  if (sensor != nullptr && (!sensor->has_state() || sensor->get_raw_state() != value))
    sensor->publish_state(value);
}

template<typename T>
void update_property(T &property, const T &value, bool &flag) {
  if (property != value) {
    property = value;
    flag = true;
  }
}

void AirConditioner::on_status_change() {
  bool need_publish = false;
  update_property(this->target_temperature, this->base_.getTargetTemp(), need_publish);
  update_property(this->current_temperature, this->base_.getIndoorTemp(), need_publish);
  auto mode = Converters::to_climate_mode(this->base_.getMode());
  update_property(this->mode, mode, need_publish);
  auto swing_mode = Converters::to_climate_swing_mode(this->base_.getSwingMode());
  update_property(this->swing_mode, swing_mode, need_publish);
  // Preset
  auto preset = this->base_.getPreset();
  if (Converters::is_custom_midea_preset(preset)) {
    if (this->set_custom_preset_(Converters::to_custom_climate_preset(preset)))
      need_publish = true;
  } else if (this->set_preset_(Converters::to_climate_preset(preset))) {
    need_publish = true;
  }
  // Fan mode
  auto fan_mode = this->base_.getFanMode();
  if (Converters::is_custom_midea_fan_mode(fan_mode)) {
    if (this->set_custom_fan_mode_(Converters::to_custom_climate_fan_mode(fan_mode)))
      need_publish = true;
  } else if (this->set_fan_mode_(Converters::to_climate_fan_mode(fan_mode))) {
    need_publish = true;
  }
  if (need_publish)
    this->publish_state();
  set_sensor(this->outdoor_sensor_, this->base_.getOutdoorTemp());
  set_sensor(this->power_sensor_, this->base_.getPowerUsage());
  set_sensor(this->humidity_sensor_, this->base_.getIndoorHum());
}

void AirConditioner::control(const ClimateCall &call) {
  dudanov::midea::ac::Control ctrl{};
  bool update = false;
  if (call.get_mode().has_value() && call.get_mode().value() != this->mode) {
    ctrl.mode = Converters::to_midea_mode(call.get_mode().value());
    update = true;
  }
  if (call.get_target_temperature().has_value() && call.get_target_temperature().value() != this->target_temperature) {
    ctrl.targetTemp = call.get_target_temperature().value();
    update = true;
  }
  if (call.get_fan_mode().has_value() &&
      (!this->fan_mode.has_value() || this->fan_mode.value() != call.get_fan_mode().value())) {
    this->custom_fan_mode.reset();
    ctrl.fanMode = Converters::to_midea_fan_mode(call.get_fan_mode().value());
    update = true;
  }
  if (call.get_custom_fan_mode().has_value() &&
      (!this->custom_fan_mode.has_value() || this->custom_fan_mode.value() != call.get_custom_fan_mode().value())) {
    this->fan_mode.reset();
    ctrl.fanMode = Converters::to_custom_midea_fan_mode(call.get_custom_fan_mode().value());
    update = true;
  }
  if (call.get_swing_mode().has_value() && call.get_swing_mode().value() != this->swing_mode) {
    ctrl.swingMode = Converters::to_midea_swing_mode(call.get_swing_mode().value());
    update = true;
  }
  if (call.get_preset().has_value() &&
      (!this->preset.has_value() || this->preset.value() != call.get_preset().value())) {
    this->custom_preset.reset();
    ctrl.preset = Converters::to_midea_preset(call.get_preset().value());
    update = true;
  }
  if (call.get_custom_preset().has_value() &&
      (!this->custom_preset.has_value() || this->custom_preset.value() != call.get_custom_preset().value())) {
    this->preset.reset();
    ctrl.preset = Converters::to_custom_midea_preset(call.get_custom_preset().value());
    update = true;
  }
  if (update)
    this->base_.control(ctrl);
}

ClimateTraits AirConditioner::traits() {
  auto traits = ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_visual_min_temperature(17);
  traits.set_visual_max_temperature(30);
  traits.set_visual_temperature_step(0.5);
  traits.set_supported_modes(this->supported_modes_);
  traits.set_supported_swing_modes(this->supported_swing_modes_);
  traits.set_supported_presets(this->supported_presets_);
  traits.set_supported_custom_presets(this->supported_custom_presets_);
  traits.set_supported_custom_fan_modes(this->supported_custom_fan_modes_);
  /* + MINIMAL SET OF CAPABILITIES */
  traits.add_supported_mode(ClimateMode::CLIMATE_MODE_OFF);
  traits.add_supported_mode(ClimateMode::CLIMATE_MODE_FAN_ONLY);
  traits.add_supported_fan_mode(ClimateFanMode::CLIMATE_FAN_AUTO);
  traits.add_supported_fan_mode(ClimateFanMode::CLIMATE_FAN_LOW);
  traits.add_supported_fan_mode(ClimateFanMode::CLIMATE_FAN_MEDIUM);
  traits.add_supported_fan_mode(ClimateFanMode::CLIMATE_FAN_HIGH);
  traits.add_supported_swing_mode(ClimateSwingMode::CLIMATE_SWING_OFF);
  traits.add_supported_swing_mode(ClimateSwingMode::CLIMATE_SWING_VERTICAL);
  traits.add_supported_preset(ClimatePreset::CLIMATE_PRESET_NONE);
  traits.add_supported_preset(ClimatePreset::CLIMATE_PRESET_SLEEP);

  //if (this->capabilities_.is_ready())
    //this->capabilities_.to_climate_traits(traits);
  return traits;
}

void AirConditioner::dump_config() {
  //if (this->capabilities_.is_ready()) {
   // this->capabilities_.dump(TAG);
  //} else if (this->use_autoconf_) {
   // ESP_LOGW(TAG, "Failed to get 0xB5 capabilities report. Suggest to disable it in config and manually set your "
    //              "appliance options.");
  //}
  this->dump_traits_(Constants::TAG);
}

/* ACTIONS */

void AirConditioner::do_follow_me(float temperature, bool beeper) {
#ifdef USE_REMOTE_TRANSMITTER
  IrFollowMeData data(static_cast<uint8_t>(lroundf(temperature)), beeper);
  this->transmit_ir_(data);
#else
  //ESP_LOGW(TAG, "Action needs remote_transmitter component");
#endif
}

void AirConditioner::do_swing_step() {
#ifdef USE_REMOTE_TRANSMITTER
  IrSpecialData data(0x01);
  this->transmit_ir_(data);
#else
  //ESP_LOGW(TAG, "Action needs remote_transmitter component");
#endif
}

void AirConditioner::do_display_toggle() {
  //if (this->capabilities_.light_control()) {
    //this->display_toggle_();
  //} else {
#ifdef USE_REMOTE_TRANSMITTER
    IrSpecialData data(0x08);
    this->transmit_ir_(data);
#else
   // ESP_LOGW(TAG, "Action needs remote_transmitter component");
#endif
 // }
}

void AirConditioner::do_beeper_on() {
  //ESP_LOGD(TAG, "Beeper Feedback Turning ON...");
  this->set_beeper_feedback(true);
}

void AirConditioner::do_beeper_off() {
  //ESP_LOGD(TAG, "Beeper Feedback Turning OFF...");
  this->set_beeper_feedback(false);
}

}  // namespace midea_ac
}  // namespace esphome
