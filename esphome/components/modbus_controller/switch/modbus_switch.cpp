
#include "modbus_switch.h"
#include "esphome/core/log.h"
namespace esphome {
namespace modbus_controller {

static const char *const TAG = "modbus_controller.switch";

void ModbusSwitch::dump_config() { LOG_SWITCH("", "Modbus Controller Switch", this); }

float ModbusSwitch::parse_and_publish(const std::vector<uint8_t> &data) {
  int64_t value = 0;
  switch (this->register_type) {
    case ModbusFunctionCode::READ_DISCRETE_INPUTS:
    case ModbusFunctionCode::READ_COILS:
      // offset for coil is the actual number of the coil not the byte offset
      value = coil_from_vector(this->offset, data);
      break;
    default:
      value = get_data<uint16_t>(data, this->offset) & this->bitmask;
      break;
  }

  float result = static_cast<float>(value);
  ESP_LOGV(TAG, "publish ModbusSwitch '%s': new value = %d  type = %d address = %X offset = %x",
           this->get_name().c_str(), value != 0.0, (int) this->register_type, this->start_address, this->offset);
  this->publish_state(value != 0.0);
  return result;
}

void ModbusSwitch::write_state(bool state) {
  // This will be called every time the user requests a state change.
  ModbusCommandItem cmd;
  ESP_LOGV(TAG, "write_state for ModbusSwitch '%s': new value = %d  type = %d address = %X offset = %x",
           this->get_name().c_str(), state, (int) this->register_type, this->start_address, this->offset);
  switch (this->register_type) {
    case ModbusFunctionCode::READ_COILS:
      // offset for coil and discrete inputs is the coil/register number not bytes
      cmd = ModbusCommandItem::create_write_single_coil(parent_, this->start_address + this->offset, state);
      break;
    case ModbusFunctionCode::READ_DISCRETE_INPUTS:
      cmd = ModbusCommandItem::create_write_single_command(parent_, this->start_address + this->offset, state);
      break;

    default:
      // since offset is in bytes and a register is 16 bits we get the start by adding offset/2
      cmd = ModbusCommandItem::create_write_single_command(parent_, this->start_address + this->offset / 2,
                                                           state ? 0xFFFF & this->bitmask : 0);
      break;
  }
  this->parent_->queue_command(cmd);
  publish_state(state);
}
// ModbusSwitch end
}  // namespace modbus_controller
}  // namespace esphome
