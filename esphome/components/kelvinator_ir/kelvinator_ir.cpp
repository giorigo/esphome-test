#include "kelvinator_ir.h"
#include "esphome/components/remote_base/kelvinator_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace kelvinator_ir {

static const char *const TAG = "climate.kelvinator_ir";

uint8_t KelvinatorIR::getMode_() {
  switch (this->mode) {
    case climate::CLIMATE_MODE_COOL:
      return KELVINATOR_MODE_COOL;
    case climate::CLIMATE_MODE_DRY:
      return KELVINATOR_MODE_DRY;
    case climate::CLIMATE_MODE_FAN_ONLY:
      return KELVINATOR_MODE_FAN;
    case climate::CLIMATE_MODE_HEAT:
      return KELVINATOR_MODE_HEAT;
    case climate::CLIMATE_MODE_HEAT_COOL:
      return KELVINATOR_MODE_AUTO;
    default:
      return KELVINATOR_MODE_AUTO;
  }
}

uint8_t KelvinatorIR::getTemperature_() { return this->target_temperature - KELVINATOR_TEMPERATURE_MIN; }

uint8_t KelvinatorIR::getPower_() { return this->mode != climate::CLIMATE_MODE_OFF; }

uint8_t KelvinatorIR::getBasicFan_() {
  if (this->fan_mode.has_value()) {
    switch (this->fan_mode.value()) {
      case climate::CLIMATE_FAN_AUTO:
        return KELVINATOR_BASICFAN_AUTO;
      case climate::CLIMATE_FAN_HIGH:
        return KELVINATOR_BASICFAN_MAX;
      case climate::CLIMATE_FAN_MEDIUM:
        return KELVINATOR_BASICFAN_MEDIUM;
      case climate::CLIMATE_FAN_LOW:
        return KELVINATOR_BASICFAN_MIN;
      default:
        return KELVINATOR_BASICFAN_AUTO;
    }
  }
  return KELVINATOR_BASICFAN_AUTO;
}

void KelvinatorIR::transmit_state() {
  auto command = KelvinatorCommand();

  command.power = this->getPower_();
  command.mode = this->getMode_();
  command.temperature = this->getTemperature_();
  command.light = this->light_;
  command.basicfan = this->getBasicFan_();
  command.swingauto = this->swing_mode == climate::CLIMATE_SWING_VERTICAL;
  command.swingv = this->swing_mode == climate::CLIMATE_SWING_VERTICAL;

  command.raw8[8] = command.raw8[0];
  command.raw8[9] = command.raw8[1];
  command.raw8[10] = command.raw8[2];

  command.raw8[3] = 0x50;
  command.raw8[11] = 0x70;

  remote_base::KelvinatorData kelvinatorData;
  kelvinatorData.data.push_back(command.raw64[0]);
  kelvinatorData.data.push_back(command.raw64[1]);
  kelvinatorData.applyChecksum();

  kelvinatorData.log();

  auto transmit = this->transmitter_->transmit();
  auto *data = transmit.get_data();
  remote_base::KelvinatorProtocol().encode(data, kelvinatorData);
  transmit.perform();
}

bool KelvinatorIR::on_receive(remote_base::RemoteReceiveData data) {
  auto decoded = remote_base::KelvinatorProtocol().decode(data);
  if (!decoded.has_value()) {
    return false;
  }

  auto kelvinatorData = decoded.value();

  if (kelvinatorData.data.size() < 2) {
    return false;
  }

  KelvinatorCommand command;
  command.raw64[0] = kelvinatorData.data[0];
  command.raw64[1] = kelvinatorData.data[1];

  if (command.power != 0) {
    switch (command.mode) {
      case KELVINATOR_MODE_AUTO:
        this->mode = climate::CLIMATE_MODE_HEAT_COOL;
        break;
      case KELVINATOR_MODE_COOL:
        this->mode = climate::CLIMATE_MODE_COOL;
        break;
      case KELVINATOR_MODE_DRY:
        this->mode = climate::CLIMATE_MODE_DRY;
        break;
      case KELVINATOR_MODE_FAN:
        this->mode = climate::CLIMATE_MODE_FAN_ONLY;
        break;
      case KELVINATOR_MODE_HEAT:
        this->mode = climate::CLIMATE_MODE_HEAT;
        break;
    }
  } else {
    this->mode = climate::CLIMATE_MODE_OFF;
  }

  this->target_temperature = command.temperature + KELVINATOR_TEMPERATURE_MIN;

  switch (command.basicfan) {
    case KELVINATOR_BASICFAN_MAX:
      this->fan_mode = climate::CLIMATE_FAN_HIGH;
      break;
    case KELVINATOR_BASICFAN_MEDIUM:
      this->fan_mode = climate::CLIMATE_FAN_MEDIUM;
      break;
    case KELVINATOR_BASICFAN_MIN:
      this->fan_mode = climate::CLIMATE_FAN_LOW;
      break;
    default:  // KELVINATOR_BASICFAN_AUTO
      this->fan_mode = climate::CLIMATE_FAN_AUTO;
      break;
  }

  switch (command.swingv) {
    case KELVINATOR_VSWING_AUTO:
      this->swing_mode = climate::CLIMATE_SWING_VERTICAL;
      break;
    default:  // KELVINATOR_VSWING_OFF
      this->swing_mode = climate::CLIMATE_SWING_OFF;
      break;
  }

  this->publish_state();

  return true;
}

void KelvinatorIR::set_light(const bool enabled) { this->light_ = enabled; }

}  // namespace kelvinator_ir
}  // namespace esphome
