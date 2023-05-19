// Datasheet https://wiki.dfrobot.com/A01NYUB%20Waterproof%20Ultrasonic%20Sensor%20SKU:%20SEN0313

#include "a01nyub.h"
#include "esphome/core/log.h"

namespace esphome {
namespace a01nyub {

static const char *const TAG = "a01nyub.sensor";
static const int MAX_DATA_LENGTH_BYTES = 4;

void A01nyubComponent::loop() {
  uint8_t data;
  while (this->available() > 0) {
    if (this->read_byte(&data)) {
      buffer_ += (char) data;
      this->check_buffer_();
    }
  }
}

void A01nyubComponent::check_buffer_() {
  if (this->buffer_.length() >= MAX_DATA_LENGTH_BYTES) {
    size_t i;
    for (i = 0; i < this->buffer_.length(); i++) {
      // Look for the first packet
      if (this->buffer_[i] == static_cast<char>(0xFF)) {
        if (i + 1 + 3 < this->buffer_.length()) {  // Packet is not complete
          return;                                  // Wait for completion
        }

        int sum = (this->buffer_[i] + this->buffer_[i + 1] + this->buffer_[i + 2]) & 0x00FF;
        if (this->buffer_[i + 3] == sum) {
          float distance = (this->buffer_[i + 1] << 8) + this->buffer_[i + 2];
          if (distance > 280) {
            float meters = distance / 1000.0;
            ESP_LOGV(TAG, "Distance from sensor: %f mm, %f m", distance, meters);
            this->publish_state(meters);
          } else {
            ESP_LOGW(TAG, "Invalid data read from sensor: %s", this->buffer_.c_str());
          }
        }
        break;
      }
    }
    this->buffer_.clear();
  }
}

void A01nyubComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "A01nyub Sensor:");
  LOG_SENSOR("  ", "Distance", this);
  // As specified in the sensor's data sheet
  this->check_uart_settings(9600, 1, esphome::uart::UART_CONFIG_PARITY_NONE, 8);
}

}  // namespace a01nyub
}  // namespace esphome
