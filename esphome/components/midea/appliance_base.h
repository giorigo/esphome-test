#pragma once
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/climate/climate.h"
#if 0//USE_REMOTE_TRANSMITTER
#include "esphome/components/remote_base/midea_protocol.h"
#include "esphome/components/remote_transmitter/remote_transmitter.h"
#endif
#include <Appliance/ApplianceBase.h>

namespace esphome {
namespace midea {

using climate::ClimatePreset;
using climate::ClimateTraits;
using climate::ClimateMode;
using climate::ClimateSwingMode;
using climate::ClimateFanMode;
using MideaMode = dudanov::midea::Mode;
using MideaSwingMode = dudanov::midea::SwingMode;
using MideaFanMode = dudanov::midea::FanMode;
using MideaPreset = dudanov::midea::Preset;

template<typename T>
class ApplianceBase : public Component, public uart::UARTDevice, public climate::Climate {
  static_assert(std::is_base_of<dudanov::midea::ApplianceBase, T>::value, "T must derive from dudanov::midea::ApplianceBase class");
 public:
  ApplianceBase() {
    this->base_.setStream(this);
    this->base_.addOnStateCallback(std::bind(&ApplianceBase::on_status_change, this));
  }
  bool can_proceed() override {
    return this->base_.getAutoconfStatus() != dudanov::midea::AutoconfStatus::AUTOCONF_PROGRESS;
  }
  float get_setup_priority() const override { return setup_priority::BEFORE_CONNECTION; }
  void setup() override { this->base_.setup(); }
  void loop() override { this->base_.loop(); }
  //void dump_config() override;
  void set_period(uint32_t ms) { this->base_.setPeriod(ms); }
  void set_response_timeout(uint32_t ms) { this->base_.setTimeout(ms); }
  void set_request_attempts(uint32_t attempts) { this->base_.setNumAttempts(attempts); }
  void set_beeper_feedback(bool state) { this->base_.setBeeper(state); }
  void set_autoconf(bool value) { this->base_.setAutoconf(value); }
  void set_supported_modes(std::set<ClimateMode> modes) { this->supported_modes_ = std::move(modes); }
  void set_supported_swing_modes(std::set<ClimateSwingMode> modes) { this->supported_swing_modes_ = std::move(modes); }
  void set_supported_presets(std::set<ClimatePreset> presets) { this->supported_presets_ = std::move(presets); }
  void set_custom_presets(std::set<std::string> presets) { this->supported_custom_presets_ = std::move(presets); }
  void set_custom_fan_modes(std::set<std::string> modes) { this->supported_custom_fan_modes_ = std::move(modes); }
  virtual void on_status_change() = 0;
#if 0 //USE_REMOTE_TRANSMITTER
  void set_transmitter(remote_transmitter::RemoteTransmitterComponent *transmitter) {
    this->transmitter_ = transmitter;
  }
  void transmit_ir(remote_base::MideaData &data);
#endif

 protected:
  T base_;
  std::set<ClimateMode> supported_modes_{};
  std::set<ClimateSwingMode> supported_swing_modes_{};
  std::set<ClimatePreset> supported_presets_{};
  std::set<std::string> supported_custom_presets_{};
  std::set<std::string> supported_custom_fan_modes_{};
#ifdef USE_REMOTE_TRANSMITTER
  remote_transmitter::RemoteTransmitterComponent *transmitter_{nullptr};
#endif
};

}  // namespace midea
}  // namespace esphome
