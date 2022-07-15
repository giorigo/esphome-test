#include "mitsubishi.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mitsubishi {

static const char *const TAG = "mitsubishi.climate";

const uint32_t MITSUBISHI_OFF = 0x00;

const uint8_t MITSUBISHI_COOL = 0x18;
const uint8_t MITSUBISHI_DRY = 0x10;
const uint8_t MITSUBISHI_AUTO = 0x20;
const uint8_t MITSUBISHI_HEAT = 0x08;

const uint8_t MITSUBISHI_FAN_1 = 0x01;
const uint8_t MITSUBISHI_FAN_2 = 0x02;
const uint8_t MITSUBISHI_FAN_3 = 0x03;
const uint8_t MITSUBISHI_FAN_4 = 0x04;


// Pulse parameters in usec
const uint16_t MITSUBISHI_BIT_MARK = 430;
const uint16_t MITSUBISHI_ONE_SPACE = 1250;
const uint16_t MITSUBISHI_ZERO_SPACE = 390;
const uint16_t MITSUBISHI_HEADER_MARK = 3500;
const uint16_t MITSUBISHI_HEADER_SPACE = 1700;
const uint16_t MITSUBISHI_MIN_GAP = 17500;

void MitsubishiClimate::transmit_state() {
  uint32_t remote_state[18] = {0x23, 0xCB, 0x26, 0x01, 0x00, 0x20, 0x08, 0x00, 0x30,
                               0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  switch (this->mode) {
    case climate::CLIMATE_MODE_COOL:
      remote_state[6] = MITSUBISHI_COOL;
      break;
    case climate::CLIMATE_MODE_HEAT:
      remote_state[6] = MITSUBISHI_HEAT;
      break;
    case climate::CLIMATE_MODE_HEAT_COOL:
      remote_state[6] = MITSUBISHI_AUTO;
      break;
    case climate::CLIMATE_MODE_DRY:
      remote_state[6] = MITSUBISHI_DRY;
      break;
    case climate::CLIMATE_MODE_OFF:
    default:
      remote_state[5] = MITSUBISHI_OFF;
      break;
  }

  //Temp
  if(this->mode == climate::CLIMATE_MODE_DRY) {
    remote_state[7] = 24 - MITSUBISHI_TEMP_MIN; //Remote sends always 24°C if "Dry" mode is selected
  } else {
    remote_state[7] = (uint8_t) roundf(clamp<float>(this->target_temperature, MITSUBISHI_TEMP_MIN, MITSUBISHI_TEMP_MAX) -
                                       MITSUBISHI_TEMP_MIN);
  }

  ESP_LOGV(TAG, "Sending Mitsubishi target temp: %.1f state: %02X mode: %02X temp: %02X", this->target_temperature,
           remote_state[5], remote_state[6], remote_state[7]);

  // Checksum
  for (int i = 0; i < 17; i++) {
    remote_state[17] += remote_state[i];
  }

  auto transmit = this->transmitter_->transmit();
  auto *data = transmit.get_data();

  data->set_carrier_frequency(38000);
  // repeat twice
  for (uint16_t r = 0; r < 2; r++) {
    // Header
    data->mark(MITSUBISHI_HEADER_MARK);
    data->space(MITSUBISHI_HEADER_SPACE);
    // Data
    for (uint8_t i : remote_state) {
      for (uint8_t j = 0; j < 8; j++) {
        data->mark(MITSUBISHI_BIT_MARK);
        bool bit = i & (1 << j);
        data->space(bit ? MITSUBISHI_ONE_SPACE : MITSUBISHI_ZERO_SPACE);
      }
    }
    // Footer
    if (r == 0) {
      data->mark(MITSUBISHI_BIT_MARK);
      data->space(MITSUBISHI_MIN_GAP);  // Pause before repeating
    }
  }
  data->mark(MITSUBISHI_BIT_MARK);

  transmit.perform();
}

bool MitsubishiClimate::parse_state_frame_(const uint8_t frame[]) {
	return false;
}

bool MitsubishiClimate::on_receive(remote_base::RemoteReceiveData data) {
    uint8_t state_frame[18] = {};

    if (!data.expect_item(MITSUBISHI_HEADER_MARK, MITSUBISHI_HEADER_SPACE)) {
    return false;
  }

  for (uint8_t pos = 0; pos < 18; pos++) {
    uint8_t byte = 0;
    for (int8_t bit = 0; bit < 8; bit++) {
      if (data.expect_item(MITSUBISHI_BIT_MARK, MITSUBISHI_ONE_SPACE)) {
        byte |= 1 << bit;
      } else if (!data.expect_item(MITSUBISHI_BIT_MARK, MITSUBISHI_ZERO_SPACE)) {
        return false;
      }
    }
    state_frame[pos] = byte;

    //Check Header
    if (pos == 0 && byte != 0x23) {
        return false;
    } else if (pos == 1 && byte != 0xCB) {
        return false;
    } else if (pos == 2 && byte != 0x26) {
        return false;
    } else if (pos == 3 && byte != 0x01) {
        return false;
    } else if (pos == 4 && byte != 0x00) {
        return false;
    }
    //Check Footer
    else if ((pos == 14 || pos == 15 || pos == 16) && byte != 0x00) {
        return false;
    }
  }

  //On/Off and Mode
  if(state_frame[5] == MITSUBISHI_OFF) {
      this->mode = climate::CLIMATE_MODE_OFF;
  } else {
      switch (state_frame[6]) {
        case MITSUBISHI_HEAT:
          this->mode = climate::CLIMATE_MODE_HEAT;
          break;
        case MITSUBISHI_DRY:
          this->mode = climate::CLIMATE_MODE_DRY;
          break;
        case MITSUBISHI_COOL:
          this->mode = climate::CLIMATE_MODE_COOL;
          break;
        case MITSUBISHI_AUTO:
          this->mode = climate::CLIMATE_MODE_HEAT_COOL;
          break;
      }
  }

  //Temp
  this->target_temperature = state_frame[7] + MITSUBISHI_TEMP_MIN;

  //Fan
  uint8_t fan = state_frame[9] & 0x87; //(Bit 8 = Auto, Bit 1,2,3 = Speed)
  switch (fan) {
    case 0x00: //Fan = Auto but Vane(Swing) in specific mode
    case 0x80: //Fan & Vane = Auto
      this->fan_mode = climate::CLIMATE_FAN_AUTO;
      break;
    case MITSUBISHI_FAN_1: //Lowest modes mapped together, CLIMATE_FAN offers only 3 states
    case MITSUBISHI_FAN_2:
      this->fan_mode = climate::CLIMATE_FAN_LOW;
      break;
    case MITSUBISHI_FAN_3:
      this->fan_mode = climate::CLIMATE_FAN_MEDIUM;
      break;
    case MITSUBISHI_FAN_4:
      this->fan_mode = climate::CLIMATE_FAN_HIGH;
      break;
  }

  //Vane (Swing Vertical) is active if
  // - Byte is 01111000 (Fan (Bit8,3,2,1) in invalid state since not auto (Bit8) but also no speed set (Bit1,2,3))
  // - Byte is 10111000 (Fan (Bit8) in Auto Mode)
  if ((state_frame[9]&0x78) == 0x78 || state_frame[9] == 0xB8) {
      this->swing_mode = climate::CLIMATE_SWING_VERTICAL;
  } else {
      this->swing_mode = climate::CLIMATE_SWING_OFF;
  }

	//ESP_LOGD(TAG, "Data: %02X", state_frame[9]);

	this->publish_state();
    return true;
}

}  // namespace mitsubishi
}  // namespace esphome
