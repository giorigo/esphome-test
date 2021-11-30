#include "button.h"
#include "esphome/core/log.h"

namespace esphome {
namespace button {

static const char *const TAG = "button";

Button::Button(const std::string &name) : EntityBase(name) {}
Button::Button() : Button("") {}

void Button::press() {
  ESP_LOGD(TAG, "'%s' Pressed.", this->get_name().c_str());
  this->press_action();
  this->press_callback_.call();
}
void Button::add_on_press_callback(std::function<void()> &&callback) { this->press_callback_.add(std::move(callback)); }
uint32_t Button::hash_base() { return 1495763804UL; }

void Button::set_device_class(const std::string &device_class) { this->device_class_ = device_class; }
std::string Button::get_device_class() {
  if (this->device_class_.has_value())
    return *this->device_class_;
  else
    return "";
}

}  // namespace button
}  // namespace esphome
