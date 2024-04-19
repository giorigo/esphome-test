#include "generic_humidifier.h"
#include "esphome/core/log.h"

namespace esphome {
namespace generic_humidifier {

static const char *const TAG = "generic_humdifier.humidifier";

void GenericHumidifier::setup() {
  this->sensor_->add_on_state_callback([this](float state) {
    this->current_humidity = state;
    // control may have changed, recompute
    this->compute_state_();
    // current humidity changed, publish state
    this->publish_state();
  });
  this->current_humidity = this->sensor_->state;


  // restore set points
  auto restore = this->restore_state_();
  if (restore.has_value()) {
    restore->to_call(this).perform();
  } else {
    // restore from defaults, change_away handles those for us
    if (supports_level_1_) {
      this->mode = humidifier::HUMIDIFIER_MODE_LEVEL_1;
    } else if (supports_level_2_) {
      this->mode = humidifier::HUMIDIFIER_MODE_LEVEL_2;
    } else if (supports_level_3_) {
      this->mode = humidifier::HUMIDIFIER_MODE_LEVEL_3;
    } else if (supports_preset_) {
      this->mode = humidifier::HUMIDIFIER_MODE_PRESET;
    }
  }
}
void GenericHumidifier::control(const humidifier::HumidifierCall &call) {
  if (call.get_mode().has_value())
    this->mode = *call.get_mode();
  if (call.get_target_humidity().has_value())
    this->target_humidity = *call.get_target_humidity();
  if (call.get_preset().has_value())
    this->preset = *call.get_preset();

  this->compute_state_();
  this->publish_state();
}
humidifier::HumidifierTraits GenericHumidifier::traits() {
  auto traits = humidifier::HumidifierTraits();
  traits.set_supports_current_humidity(true);
  traits.set_supported_modes({
      humidifier::HUMIDIFIER_MODE_OFF,
  });
  if (supports_level_1_)
    traits.add_supported_mode(humidifier::HUMIDIFIER_MODE_LEVEL_1);
  if (supports_level_2_)
    traits.add_supported_mode(humidifier::HUMIDIFIER_MODE_LEVEL_2);
  if (supports_level_3_)
    traits.add_supported_mode(humidifier::HUMIDIFIER_MODE_LEVEL_3);
  if (supports_preset_)
    traits.add_supported_mode(humidifier::HUMIDIFIER_MODE_PRESET);
  traits.set_supports_action(true);
  return traits;
}
void GenericHumidifier::compute_state_() {
  if (this->mode == humidifier::HUMIDIFIER_MODE_OFF) {
    this->switch_to_action_(humidifier::HUMIDIFIER_ACTION_OFF);
    return;
  }
  if (std::isnan(this->current_humidity) || std::isnan(this->target_humidity)) {
    // if any control parameters are nan, go to OFF action (not IDLE!)
    this->switch_to_action_(humidifier::HUMIDIFIER_ACTION_OFF);
    return;
  }

  // #############################################################################
//   const bool too_cold = this->current_temperature < this->target_temperature_low;
//   const bool too_hot = this->current_temperature > this->target_temperature_high;

//   climate::ClimateAction target_action;
//   if (too_cold) {
//     // too cold -> enable heating if possible and enabled, else idle
//     if (this->supports_heat_ &&
//         (this->mode == climate::CLIMATE_MODE_HEAT_COOL || this->mode == climate::CLIMATE_MODE_HEAT)) {
//       target_action = climate::CLIMATE_ACTION_HEATING;
//     } else {
//       target_action = climate::CLIMATE_ACTION_IDLE;
//     }
//   } else if (too_hot) {
//     // too hot -> enable cooling if possible and enabled, else idle
//     if (this->supports_cool_ &&
//         (this->mode == climate::CLIMATE_MODE_HEAT_COOL || this->mode == climate::CLIMATE_MODE_COOL)) {
//       target_action = climate::CLIMATE_ACTION_COOLING;
//     } else {
//       target_action = climate::CLIMATE_ACTION_IDLE;
//     }
//   } else {
//     // neither too hot nor too cold -> in range
//     if (this->supports_cool_ && this->supports_heat_ && this->mode == climate::CLIMATE_MODE_HEAT_COOL) {
//       // if supports both ends and both cooling and heating enabled, go to idle action
//       target_action = climate::CLIMATE_ACTION_IDLE;
//     } else {
//       // else use current mode and don't change (hysteresis)
//       target_action = this->action;
//     }
//   }

//   this->switch_to_action_(target_action);
// #############################################
}


void GenericHumidifier::switch_to_action_(humidifier::HumidifierAction action) {
  if (action == this->action) {
    // already in target mode
    return;
  }
// #############################################
  // if ((action == climate::CLIMATE_ACTION_OFF && this->action == climate::CLIMATE_ACTION_IDLE) ||
  //     (action == climate::CLIMATE_ACTION_IDLE && this->action == climate::CLIMATE_ACTION_OFF)) {
  //   // switching from OFF to IDLE or vice-versa
  //   // these only have visual difference. OFF means user manually disabled,
  //   // IDLE means it's in auto mode but value is in target range.
  //   this->action = action;
  //   this->publish_state();
  //   return;
  // }
// ############################################

  if (this->prev_trigger_ != nullptr) {
    this->prev_trigger_->stop_action();
    this->prev_trigger_ = nullptr;
  }
  Trigger<> *trig;
  switch (action) {
    case humidifier::HUMIDIFIER_ACTION_OFF:
    case humidifier::HUMIDIFIER_ACTION_LEVEL_1:
      trig = this->level_1_trigger_;
      break;
    case humidifier::HUMIDIFIER_ACTION_LEVEL_2:
      trig = this->level_2_trigger_;
      break;
    case humidifier::HUMIDIFIER_ACTION_LEVEL_3:
      trig = this->level_3_trigger_;
      break; 
   case humidifier::HUMIDIFIER_ACTION_PRESET: 
      trig = this->preset_trigger_;
      break;   
    default:
      trig = nullptr;
  }
  assert(trig != nullptr);
  trig->trigger();
  this->action = action;
  this->prev_trigger_ = trig;
  this->publish_state();
}
// void BangBangClimate::change_away_(bool away) {
//   if (!away) {
//     this->target_temperature_low = this->normal_config_.default_temperature_low;
//     this->target_temperature_high = this->normal_config_.default_temperature_high;
//   } else {
//     this->target_temperature_low = this->away_config_.default_temperature_low;
//     this->target_temperature_high = this->away_config_.default_temperature_high;
//   }
//   this->preset = away ? climate::CLIMATE_PRESET_AWAY : climate::CLIMATE_PRESET_HOME;
// }
void GenericHumidifier::set_normal_config(const GenericHumidifierTargetHumidityConfig &normal_config) {
  this->normal_config_ = normal_config;
}

GenericHumidifier::GenericHumidifier()
    : level_1_trigger_(new Trigger<>()), level_2_trigger_(new Trigger<>()), level_3_trigger_(new Trigger<>()), preset_trigger_(new Trigger<>()) {}
void GenericHumidifier::set_sensor(sensor::Sensor *sensor) { this->sensor_ = sensor; }
Trigger<> *GenericHumidifier::get_level_1_trigger() const { return this->level_1_trigger_; }
void GenericHumidifier::set_supports_level_1(bool supports_level_1) { this->supports_level_1_ = supports_level_1; }
Trigger<> *GenericHumidifier::get_level_2_trigger() const { return this->level_2_trigger_; }
void GenericHumidifier::set_supports_level_2(bool supports_level_2) { this->supports_level_2_ = supports_level_2; }
Trigger<> *GenericHumidifier::get_level_3_trigger() const { return this->level_3_trigger_; }
void GenericHumidifier::set_supports_level_3(bool supports_level_3) { this->supports_level_3_ = supports_level_3; }
Trigger<> *GenericHumidifier::get_preset_trigger() const { return this->preset_trigger_; }
void GenericHumidifier::set_supports_preset(bool supports_preset) { this->supports_preset_ = supports_preset; }
void GenericHumidifier::dump_config() {
  LOG_HUMIDIFIER("", "Generic Humidifier", this);
  ESP_LOGCONFIG(TAG, "  Supports LEVEL 1: %s", YESNO(this->supports_level_1_));
  ESP_LOGCONFIG(TAG, "  Supports LEVEL 2: %s", YESNO(this->supports_level_2_));
  ESP_LOGCONFIG(TAG, "  Supports LEVEL 3: %s", YESNO(this->supports_level_3_));
  ESP_LOGCONFIG(TAG, "  Supports PRESET: %s", YESNO(this->supports_preset_));
  ESP_LOGCONFIG(TAG, "  Default Target Humidity: %.2f%%", this->normal_config_.default_humidity);
}

GenericHumidifierTargetHumidityConfig::GenericHumidifierTargetHumidityConfig() = default;
GenericHumidifierTargetHumidityConfig::GenericHumidifierTargetHumidityConfig(float default_humidity)
    : default_humidity(default_humidity) {}

}  // namespace generic_humidifier
}  // namespace esphome
