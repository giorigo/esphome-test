#include "input_datetime.h"

#include "esphome/core/log.h"

#include <cinttypes>

namespace esphome {
namespace input_datetime {

static const char *const TAG = "input_datetime";

InputDatetime::InputDatetime() {}

void InputDatetime::publish_state(std::string state) {
  this->has_state_ = true;
  this->state = state;
  this->has_date = HAS_DATETIME_STRING_DATE_ONLY(state) || HAS_DATETIME_STRING_DATE_AND_TIME(state);
  this->has_time = HAS_DATETIME_STRING_TIME_ONLY(state) || HAS_DATETIME_STRING_DATE_AND_TIME(state);
  if (!ESPTime::strptime(state, this->state_as_time)) {
    ESP_LOGE(TAG, "'%s' Could not convert the dateime string to an ESPTime objekt", this->get_name().c_str());
  }
  ESP_LOGD(TAG, "'%s': Sending state %s", this->get_name().c_str(), this->state.c_str());
  this->state_callback_.call(state);
}

void InputDatetime::add_on_state_callback(std::function<void(std::string)> &&callback) {
  this->state_callback_.add(std::move(callback));
}

}  // namespace input_datetime
}  // namespace esphome
