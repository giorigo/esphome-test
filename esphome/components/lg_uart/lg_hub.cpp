// For itoa()
#include <stdlib.h>
#include <stdio.h>

#include "lg_hub.h"

#include "lg_uart_child.h"

namespace esphome {
namespace lg_uart {

/* Public */

bool LGUartHub::send_cmd(char cmd_code[2], int data) {
  ESP_LOGD(TAG, "send_cmd(%s). data: '%i' for screen: '%i'", cmd_code, data, this->screen_num_);
  std::string s = str_sprintf("%02s %02x %02x\r", cmd_code, this->screen_num_, data);
  this->parent_->write_array(std::vector<uint8_t>(s.begin(), s.end()));

  uint8_t reply[PACKET_LEN] = {0};
  uint8_t peeked;
  uint8_t idx = 0;

  bool stream_valid = false;

  /*
    After sending a command, we listen for the reply.
    In testing, replies didn't always come back instantly; would send inquire about one setting, have nothing in the
    UART and then exit. A short time later, a new inquiry for a different setting would get fired off and available()
    would return true as the reply to the PREVIOUS inquiry was now in the buffer.
    Rather than drop the received bytes because they don't match the setting we just inquired about, we accept all
    as we get them and - assuming they're valid - dispatch them to the appropriate child for follow up.

    Packets should look like this:

    let's say that we have a packet:
        peeked[0]: 0x61         97      [a]
        peeked[0]: 0x20         32      [ ]
        peeked[0]: 0x30         48      [0]
        peeked[0]: 0x31         49      [1]
        peeked[0]: 0x20         32      [ ]
        peeked[0]: 0x4f         79      [O]
        peeked[0]: 0x4b         75      [K]
        peeked[0]: 0x30         48      [0]
        peeked[0]: 0x31         49      [1]
        peeked[0]: 0x78         120     [x]

    The first byte needs to be one of the chars that indexes this->children_
    The second byte needs to be a space.
    The third and forth byte need to be the screen number that we're configured to listen to.
    Then another space before either OK or NG. NG means "not good"; there is no specific return for "error" vs "not
    supported".
    The last bytes will be the value of the setting we inquired about and an `x`

  */
  // Note: this does not get called when uart debugging turned on!
  while (this->available()) {
    // Get the byte
    this->read_byte(&peeked);

    // ESP_LOGD(TAG, "send_cmd(%s). peeked[%i]: 0x%x \t %u \t [%c]", cmd_code, idx, peeked, peeked, peeked);

    // For the first byte, we need it to be in children
    if (idx == 0) {
      if (this->children_.count((char) peeked) == 0) {
        // ESP_LOGD(TAG, "send_cmd(%s). NOT FOUND: [%c]", cmd_code, peeked);
        continue;
      } else {
        reply[idx] = peeked;
        idx += 1;
        // ESP_LOGD(TAG, "send_cmd(%s). FOUND: [%c]", cmd_code, peeked);
        continue;
      }
      // Space
    } else if ((idx == 1 || idx == 4) && peeked == 0x20) {
      reply[idx] = peeked;
      idx += 1;
      continue;
      // Screen number
    } else if (idx == 2 && peeked == this->screen_num_chars[0]) {
      reply[idx] = peeked;
      idx += 1;
      continue;
    } else if (idx == 3 && peeked == this->screen_num_chars[1]) {
      reply[idx] = peeked;
      idx += 1;
      continue;
      // OK
    } else if (idx == 5 || idx == 6 || idx == 7 || idx == 8) {
      reply[idx] = peeked;
      idx += 1;
      continue;
    } else if (idx == 9) {
      reply[idx] = peeked;
      idx += 1;
    }

    if (idx == PACKET_LEN - 1) {
      // OK
      if (reply[5] == 0x4f && reply[6] == 0x4b) {
        ESP_LOGD(TAG, "send_cmd(%s). Got OK packet! Dispatching to handler for [%c]...", cmd_code, reply[0]);
        this->children_[reply[0]]->on_reply_packet(reply);
        return true;
      } else {
        ESP_LOGW(TAG, "send_cmd(%s). LG replied with 'NG'", cmd_code);
        return false;
      }
    }
    ESP_LOGD(TAG, "send_cmd(%s). Invalid or incomplete packet.", cmd_code);
    return false;
  }
  // If we didn't get a full packet, indicate failure of some sort.
  return false;
}

/* Internal */
void LGUartHub::loop() {}

void LGUartHub::setup() {
  // User gives us a number from 0 to 99; we need the individual characters
  std::string s = str_sprintf("%02d", this->screen_num_);
  this->screen_num_chars[0] = s.at(0);
  this->screen_num_chars[1] = s.at(1);
  // Show sign of life in boot logs
  this->dump_config();
}

void LGUartHub::dump_config() {
  ESP_LOGCONFIG(TAG, "Config for screen_num: [%c,%c]", this->screen_num_chars[0], this->screen_num_chars[1]);
  ESP_LOGCONFIG(TAG, "UART baud rate '%i'", this->parent_->get_baud_rate());
  ESP_LOGCONFIG(TAG, "  Child components (%d):", this->children_.size());

  // Dump the cmd char and the name of the thing responding to it
  std::map<char, LGUartClient *>::iterator it;
  for (it = this->children_.begin(); it != this->children_.end(); it++) {
    ESP_LOGCONFIG(TAG, "    -  [%c] => %s", it->first, it->second->describe().c_str());
  }
}

void LGUartHub::register_child(LGUartClient *obj, std::string cmd_char) {
  this->children_[cmd_char.at(0)] = obj;
  obj->set_parent(this);
}

}  // namespace lg_uart
}  // namespace esphome
