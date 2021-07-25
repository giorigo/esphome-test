#include "nexa_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const TAG = "remote.nexa";


static const uint8_t NBITS = 32;
static const uint32_t HEADER_HIGH_US = 319;
static const uint32_t HEADER_LOW_US = 2610;
static const uint32_t BIT_HIGH_US = 319;
static const uint32_t BIT_ONE_LOW_US = 1000;
static const uint32_t BIT_ZERO_LOW_US = 140;

static const uint32_t TX_HEADER_HIGH_US = 250;
static const uint32_t TX_HEADER_LOW_US = TX_HEADER_HIGH_US * 10;
static const uint32_t TX_BIT_HIGH_US = 250;
static const uint32_t TX_BIT_ONE_LOW_US = TX_BIT_HIGH_US * 5;
static const uint32_t TX_BIT_ZERO_LOW_US = TX_BIT_HIGH_US * 1;

void NexaProtocol::one(RemoteTransmitData *dst) const {
  // '1' => '10'
  dst->item(TX_BIT_HIGH_US, TX_BIT_ONE_LOW_US);
  dst->item(TX_BIT_HIGH_US, TX_BIT_ZERO_LOW_US);
  // ESP_LOGD(TAG, "1 ");
}

void NexaProtocol::zero(RemoteTransmitData *dst) const {
  // '0' => '01'
  dst->item(TX_BIT_HIGH_US, TX_BIT_ZERO_LOW_US);
  dst->item(TX_BIT_HIGH_US, TX_BIT_ONE_LOW_US);
  // ESP_LOGD(TAG, "0 ");
}

void NexaProtocol::sync(RemoteTransmitData *dst) const {
  dst->item(TX_HEADER_HIGH_US, TX_HEADER_LOW_US);
  // ESP_LOGD(TAG, "S ");
}

void NexaProtocol::encode(RemoteTransmitData *dst, const NexaData &data) {
  dst->set_carrier_frequency(0);
  ESP_LOGD(TAG, "ENCODE NEXA: device=0x%04X group=%d state=%d channel=%d level=%d", data.device, data.group, data.state,
           data.channel, data.level);

  // Send SYNC
  this->sync(dst);

  // ESP_LOGD(TAG, "Device:");
  // Device (26 bits)
  for (int16_t i = 26 - 1; i >= 0; i--) {
    if (data.device & (1 << i))
      this->one(dst);
    else
      this->zero(dst);
  }

  // ESP_LOGD(TAG, "Group:");
  // Group (1 bit)
  if (data.group != 0)
    this->one(dst);
  else
    this->zero(dst);

  // ESP_LOGD(TAG, "State:");
  // State (1 bit)
  if (data.state == 2) {
    // Special case for dimmers...send 00 as state
    dst->item(TX_BIT_HIGH_US, TX_BIT_ZERO_LOW_US);
    dst->item(TX_BIT_HIGH_US, TX_BIT_ZERO_LOW_US);
  } else if (data.state == 1)
    this->one(dst);
  else
    this->zero(dst);

  // ESP_LOGD(TAG, "Channel:");
  // Channel (4 bits)
  for (int16_t i = 4 - 1; i >= 0; i--) {
    if (data.channel & (1 << i))
      this->one(dst);
    else
      this->zero(dst);
  }

  // ESP_LOGD(TAG, "Level:");
  // Level (4 bits)
  if (data.state == 2) {
    for (int16_t i = 4 - 1; i >= 0; i--) {
      if (data.level & (1 << i))
        this->one(dst);
      else
        this->zero(dst);
    }
  }

  // Send finishing Zero
  // this->zero(dst);
  dst->item(TX_BIT_HIGH_US, TX_BIT_ZERO_LOW_US);

  // Send the PAUSE
  // dst->mark(BIT_HIGH_US);
  // dst->space(TX_BIT_HIGH_US*50);

  ESP_LOGD(TAG, "ENCODE NEXA: Done");
}

optional<NexaData> NexaProtocol::decode(RemoteReceiveData src) {
  NexaData out{
      .device = 0,
      .group = 0,
      .state = 0,
      .channel = 0,
      .level = 0,
  };

  // From: http://tech.jolowe.se/home-automation-rf-protocols/
  // New data: http://tech.jolowe.se/old-home-automation-rf-protocols/
  /*

  SHHHH HHHH HHHH HHHH HHHH HHHH HHGO EE BB DDDD 0 P

  S = Sync bit.
  H = The first 26 bits are transmitter unique codes, and it is this code that the reciever "learns" to recognize.
  G = Group code, set to one for the whole group.
  O = On/Off bit. Set to 1 for on, 0 for off.
  E = Unit to be turned on or off. The code is inverted, i.e. '11' equals 1, '00' equals 4.
  B = Button code. The code is inverted, i.e. '11' equals 1, '00' equals 4.
  D = Dim level bits.
  0 = packet always ends with a zero.
  P = Pause, a 10 ms pause in between re-send.

  Update: First of all the '1' and '0' bit seems to be reversed (and be the same as Jula I protocol below), i.e.

  */

  // Require a SYNC pulse + long gap
  if (!src.expect_pulse_with_gap(HEADER_HIGH_US, HEADER_LOW_US))
    return {};

  // ESP_LOGD(TAG, "DECODE NEXA (good sync pulse): ");
  // ESP_LOGD(TAG, "SYNC");

  // ESP_LOGD(TAG, "DEVICE:");
  // Device
  for (uint8_t i = 0; i < 26; i++) {
    out.device <<= 1UL;
    if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ONE_LOW_US) &&
        (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ZERO_LOW_US))) {
      // '1' => '10'
      out.device |= 0x01;
      // ESP_LOGD(TAG, "1");
    } else if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ZERO_LOW_US) &&
               (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ONE_LOW_US))) {
      // '0' => '01'
      out.device |= 0x00;
      // ESP_LOGD(TAG, "0");
    } else {
      // ESP_LOGD(TAG, "...unknown..");
      // This should not happen...failed command
      return {};
    }
  }

  // ESP_LOGD(TAG, "GROUP:");
  // GROUP
  for (uint8_t i = 0; i < 1; i++) {
    out.group <<= 1UL;
    if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ONE_LOW_US) &&
        (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ZERO_LOW_US))) {
      // '1' => '10'
      out.group |= 0x01;
      // ESP_LOGD(TAG, "1");
    } else if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ZERO_LOW_US) &&
               (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ONE_LOW_US))) {
      // '0' => '01'
      out.group |= 0x00;
      // ESP_LOGD(TAG, "0");
    } else {
      // ESP_LOGD(TAG, "...unknown..");
      // This should not happen...failed command
      return {};
    }
  }

  // ESP_LOGD(TAG, "STATE:");
  // STATE
  for (uint8_t i = 0; i < 1; i++) {
    out.state <<= 1UL;

    // Special treatment as we should handle 01, 10 and 00
    // We need to care for the advance made in the expect functions
    // hence take them one at a time so that we do not get out of sync
    // in decoding

    if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      // Starts with '1'
      if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
        // '10' => 1
        out.state |= 0x01;
        // ESP_LOGD(TAG, "1");
      } else if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ONE_LOW_US)) {
        // '11' => NOT OK
        // This case is here to make sure we advance through the correct index
        // ESP_LOGD(TAG, "- err 11 not ok");
        // This should not happen...failed command
        return {};
      }
    } else if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      // Starts with '0'
      if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ONE_LOW_US)) {
        // '01' => 0
        out.state |= 0x00;
        // ESP_LOGD(TAG, "0");
      } else if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
        // '00' => Special case for dimmer! => 2
        out.state |= 0x02;
        // ESP_LOGD(TAG, "00 => special case for dimmer");
      }
    }
  }

  // ESP_LOGD(TAG, "CHANNEL:");
  // CHANNEL (EE and BB bits)

  for (uint8_t i = 0; i < 4; i++) {
    out.channel <<= 1UL;
    if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ONE_LOW_US) &&
        (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ZERO_LOW_US))) {
      // '1' => '10'
      out.channel |= 0x01;
      // ESP_LOGD(TAG, "1");
    } else if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ZERO_LOW_US) &&
               (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ONE_LOW_US))) {
      // '0' => '01'
      out.channel |= 0x00;
      // ESP_LOGD(TAG, "0");
    } else {
      // ESP_LOGD(TAG, "...unknown..");
      // This should not happen...failed command
      return {};
    }
  }

  // Optional to transmit LEVEL data (8 bits more)
  if (int32_t(src.get_index() + 8) >= src.size()) {
    // ESP_LOGD(TAG, "DECODE NEXA: DONE - No LEVEL DATA");
    // raw_dump(src);
    // ESP_LOGD(TAG, "         NEXA: device=0x%04X group=%d state=%d channel=%d level=%d", out.device, out.group,
    //      out.state, out.channel, out.level);
    return out;
  }

  // ESP_LOGD(TAG, "LEVEL:");
  // LEVEL
  for (uint8_t i = 0; i < 4; i++) {
    out.level <<= 1UL;
    if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ONE_LOW_US) &&
        (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ZERO_LOW_US))) {
      // '1' => '10'
      out.level |= 0x01;
      // ESP_LOGD(TAG, "1");
    } else if (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ZERO_LOW_US) &&
               (src.expect_pulse_with_gap(BIT_HIGH_US, BIT_ONE_LOW_US))) {
      // '0' => '01'
      out.level |= 0x00;
      // ESP_LOGD(TAG, "0");
    } else {
      // ESP_LOGD(TAG, "...unknown..");
      // This should not happen...failed command
      // return {};
      break;
    }
  }

  // raw_dump(src);

  // ESP_LOGD(TAG, "DECODE NEXA: DONE");

  // ESP_LOGD(TAG, "         NEXA: device=0x%04X group=%d state=%d channel=%d level=%d", out.device, out.group,
  //         out.state, out.channel, out.level);

  return out;
}

void NexaProtocol::raw_dump(RemoteReceiveData src) {
  char buffer[256];
  uint32_t buffer_offset = 0;
  buffer_offset += sprintf(buffer, "Timings: ");

  for (int32_t i = 0; i < src.size() - 1; i++) {
    const int32_t value = src[i];
    const uint32_t remaining_length = sizeof(buffer) - buffer_offset;
    int written;

    if (i + 1 < src.size() - 1) {
      written = snprintf(buffer + buffer_offset, remaining_length, "%d, ", value);
    } else {
      written = snprintf(buffer + buffer_offset, remaining_length, "%d", value);
    }

    if (written < 0 || written >= int(remaining_length)) {
      // write failed, flush...
      buffer[buffer_offset] = '\0';
      ESP_LOGD(TAG, "%s", buffer);
      buffer_offset = 0;
      written = sprintf(buffer, "  ");
      if (i + 1 < src.size()) {
        written += sprintf(buffer + written, "%d, ", value);
      } else {
        written += sprintf(buffer + written, "%d", value);
      }
    }

    buffer_offset += written;
  }
  if (buffer_offset != 0) {
    ESP_LOGD(TAG, "%s", buffer);
  }
}

void NexaProtocol::dump(const NexaData &data) {
  ESP_LOGD(TAG, "Received NEXA: device=0x%04X group=%d state=%d channel=%d level=%d", data.device, data.group,
           data.state, data.channel, data.level);
}

}  // namespace remote_base
}  // namespace esphome
