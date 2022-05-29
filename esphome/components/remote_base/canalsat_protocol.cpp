#include "canalsat_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const CANALSAT_TAG = "remote.canalsat";
static const char *const CANALSATLD_TAG = "remote.canalsatld";

static const uint16_t CANALSAT_FREQ = 55500;
static const uint16_t CANALSATLD_FREQ = 56000;
static const uint16_t CANALSAT_UNIT = 250;
static const uint16_t CANALSATLD_UNIT = 320;

CanalSatProtocol::CanalSatProtocol() {
  this->frequency = CANALSAT_FREQ;
  this->unit = CANALSAT_UNIT;
  this->tag = CANALSAT_TAG;
}

CanalSatLDProtocol::CanalSatLDProtocol() {
  this->frequency = CANALSATLD_FREQ;
  this->unit = CANALSATLD_UNIT;
  this->tag = CANALSATLD_TAG;
}

void CanalSatBaseProtocol::encode(RemoteTransmitData *dst, const CanalSatData &data) {
  dst->reserve(48);
  dst->set_carrier_frequency(this->frequency);

  uint32_t raw{
      static_cast<uint32_t>((1 << 23) | (data.device << 16) | (data.address << 10) | (0 << 9) | (data.command << 1))};
  bool wasHigh{true};

  for (uint32_t mask = 0x800000; mask; mask >>= 1) {
    if (raw & mask) {
      if (wasHigh) {
        dst->mark(this->unit);
      }
      wasHigh = true;
      if (raw & mask >> 1) {
        dst->space(this->unit);
      } else {
        dst->space(this->unit * 2);
      }
    } else {
      if (!wasHigh) {
        dst->space(this->unit);
      }
      wasHigh = false;
      if (raw & mask >> 1) {
        dst->mark(this->unit * 2);
      } else {
        dst->mark(this->unit);
      }
    }
  }
}

optional<CanalSatData> CanalSatBaseProtocol::decode(RemoteReceiveData src) {
  CanalSatData data{
      .device = 0,
      .address = 0,
      .repeat = 0,
      .command = 0,
  };

  // Check if initial mark and spaces match
  if (!src.peek_mark(this->unit) || !(src.peek_space(this->unit, 1) || src.peek_space(this->unit * 2, 1))) {
    return {};
  }

  bool bit{1};
  uint8_t offset{1};
  uint32_t bitStream{0};

  while (offset < 24) {
    bitStream = bitStream | (bit << (24 - offset++));
    src.advance();
    if (src.peek_mark(this->unit) || src.peek_space(this->unit)) {
      src.advance();
    } else if (src.peek_mark(this->unit * 2) || src.peek_space(this->unit * 2)) {
      bit = !bit;
    } else if (offset != 24 && bit != 1) {  // If last bit is high, final space is indistinguishable
      return {};
    }
  }

  data.device = (0xFF0000 & bitStream) >> 16;
  data.address = (0x00FF00 & bitStream) >> 10;
  data.repeat = (0x00FF00 & bitStream) >> 9;
  data.command = (0x0000FF & bitStream) >> 1;

  return data;
}

void CanalSatBaseProtocol::dump(const CanalSatData &data) {
  if (this->tag == CANALSATLD_TAG) {
    ESP_LOGD(this->tag, "Received CanalSatLD: device=0x%02X, address=0x%02X, command=0x%02X, repeat=0x%X", data.device,
             data.address, data.command, data.repeat);
  } else {
    ESP_LOGD(this->tag, "Received CanalSat: device=0x%02X, address=0x%02X, command=0x%02X, repeat=0x%X", data.device,
             data.address, data.command, data.repeat);
  }
}

}  // namespace remote_base
}  // namespace esphome
