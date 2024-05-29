// Based on this datasheet:
// https://www.mouser.ca/datasheet/2/678/AVGO_S_A0002854364_1-2574547.pdf

#include "apds9306.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace apds9306 {

static const char *const TAG = "apds9306";

enum {  // APDS9306 registers
  APDS9306_MAIN_CTRL = 0x00,
  APDS9306_ALS_MEAS_RATE = 0x04,
  APDS9306_ALS_GAIN = 0x05,
  APDS9306_PART_ID = 0x06,
  APDS9306_MAIN_STATUS = 0x07,
  APDS9306_CLEAR_DATA_0 = 0x0A,  // LSB
  APDS9306_CLEAR_DATA_1 = 0x0B,
  APDS9306_CLEAR_DATA_2 = 0x0C,  // MSB
  APDS9306_ALS_DATA_0 = 0x0D,    // LSB
  APDS9306_ALS_DATA_1 = 0x0E,
  APDS9306_ALS_DATA_2 = 0x0F,  // MSB
  APDS9306_INT_CFG = 0x19,
  APDS9306_INT_PERSISTENCE = 0x1A,
  APDS9306_ALS_THRES_UP_0 = 0x21,  // LSB
  APDS9306_ALS_THRES_UP_1 = 0x22,
  APDS9306_ALS_THRES_UP_2 = 0x23,   // MSB
  APDS9306_ALS_THRES_LOW_0 = 0x24,  // LSB
  APDS9306_ALS_THRES_LOW_1 = 0x25,
  APDS9306_ALS_THRES_LOW_2 = 0x26,  // MSB
  APDS9306_ALS_THRES_VAR = 0x27
};

#define APDS9306_ERROR_CHECK(func, error) \
  if (!(func)) { \
    ESP_LOGE(TAG, error); \
    this->mark_failed(); \
    return; \
  }
#define APDS9306_WARNING_CHECK(func, warning) \
  if (!(func)) { \
    ESP_LOGW(TAG, warning); \
    this->status_set_warning(); \
    return; \
  }
#define APDS9306_WRITE_BYTE(reg, value) \
  ESP_LOGV(TAG, "Writing 0x%02x to 0x%02x", value, reg); \
  if (!this->write_byte(reg, value)) { \
    ESP_LOGE(TAG, "Failed writing 0x%02x to 0x%02x", value, reg); \
    this->mark_failed(); \
    return; \
  }

void APDS9306::setup() {
  ESP_LOGCONFIG(TAG, "Setting up APDS9306...");

  this->setup_has_run_++;

  uint8_t id;
  if (!this->read_byte(APDS9306_PART_ID, &id)) {  // Part ID register
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }

  this->setup_has_run_++;

  if (id != 0xB1 && id != 0xB3) {  // 0xB1 for APDS9306 0xB3 for APDS9306-065
    this->error_code_ = WRONG_ID;
    this->mark_failed();
    return;
  }

  this->setup_has_run_++;

  // Trigger software reset
  APDS9306_WRITE_BYTE(APDS9306_MAIN_CTRL, 0x10);

  this->setup_has_run_++;
  // Put in standby mode
  APDS9306_WRITE_BYTE(APDS9306_MAIN_CTRL, 0x00);

  this->setup_has_run_++;

  // ALS resolution and measurement, see datasheet or init.py for options
  uint8_t als_meas_rate = ((this->bit_width_ & 0x07) << 4) | (this->measurement_rate_ & 0x07);
  APDS9306_WRITE_BYTE(APDS9306_ALS_MEAS_RATE, als_meas_rate);
  
  this->setup_has_run_++;

  // ALS gain, see datasheet or init.py for options
  uint8_t als_gain = (this->gain_ & 0x07);
  APDS9306_WRITE_BYTE(APDS9306_ALS_GAIN, als_gain);

  this->setup_has_run_++;

  // Set to Active mode
  APDS9306_WRITE_BYTE(APDS9306_MAIN_CTRL, 0x02);

  this->setup_has_run_++;

  ESP_LOGCONFIG(TAG, "setup complete");
}

void APDS9306::dump_config() {
  LOG_SENSOR("", "apds9306", this);
  LOG_I2C_DEVICE(this);

  if (this->is_failed()) {
    switch (this->error_code_) {
      case COMMUNICATION_FAILED:
        ESP_LOGE(TAG, "Communication with APDS9306 failed!");
        break;
      case WRONG_ID:
        ESP_LOGE(TAG, "APDS9306 has invalid id!");
        break;
      default:
        ESP_LOGE(TAG, "Setting up APDS9306 registers failed!");
        break;
    }
  }

  this->convert_config_variables_();

  ESP_LOGCONFIG(TAG, "  Gain: %f", gain_val_);
  ESP_LOGCONFIG(TAG, "  Measurement rate: %f", measurement_rate_val_);
  ESP_LOGCONFIG(TAG, "  Measurement Resolution/Bit width: %d", bit_width_val_);
  if (this->setup_has_run_) ESP_LOGCONFIG(TAG, "SETUP HAS RUN");
  else ESP_LOGCONFIG(TAG, "SETUP HAS NOT RUN");
  ESP_LOGCONFIG(TAG, "Setup step reached: %d", setup_has_run_);

  LOG_UPDATE_INTERVAL(this);
}

void APDS9306::update() {
  uint8_t status;
  APDS9306_WARNING_CHECK(this->read_byte(APDS9306_MAIN_STATUS, &status), "Reading status bit failed.");

  this->status_clear_warning();

  APDS9306_WRITE_BYTE(APDS9306_MAIN_CTRL, 0x02);

  while (!(status &= 0b00001000)) {APDS9306_WARNING_CHECK(this->read_byte(APDS9306_MAIN_STATUS, &status), "Reading status bit failed.")}  // No new data  
  
  APDS9306_WRITE_BYTE(APDS9306_MAIN_CTRL, 0x00);
    
  this->convert_config_variables_();

  uint8_t als_data[3];
  APDS9306_WARNING_CHECK(this->read_byte(APDS9306_ALS_DATA_0, als_data, 3), "Reading ALS data has failed.");

  uint32_t light_level = 0x00 | encode_uint24(als_data[2], als_data[1], als_data[0]);

  float lux = (light_level / this->gain_val_) * (100.0f / this->measurement_time_);

  ESP_LOGD(TAG, "Got illuminance=%.1flx from", lux);
  this->publish_state(lux);

  // Restart
  
  
}

void APDS9306::convert_config_variables_() {
  switch (this->gain_) {
    case 0:
      gain_val_ = 1;
      break;
    case 1:
      gain_val_ = 3;
      break;
    case 2:
      gain_val_ = 6;
      break;
    case 3:
      gain_val_ = 9;
      break;
    case 4:
      gain_val_ = 18;
      break;
  }

  switch (this->measurement_rate_) {
    case 0:
      measurement_rate_val_ = 25;
      break;
    case 1:
      measurement_rate_val_ = 50;
      break;
    case 2:
      measurement_rate_val_ = 100;
      break;
    case 3:
      measurement_rate_val_ = 200;
      break;
    case 4:
      measurement_rate_val_ = 500;
      break;
    case 5:
      measurement_rate_val_ = 1000;
      break;
    case 6:
      measurement_rate_val_ = 2000;
      break;
  }

  switch (this->bit_width_) {
    case 0:
      bit_width_val_ = 20;
      measurement_time_ = 400;
      break;
    case 1:
      bit_width_val_ = 19;
      measurement_time_ = 200;
      break;
    case 2:
      bit_width_val_ = 18;
      measurement_time_ = 100;
      break;
    case 3:
      bit_width_val_ = 17;
      measurement_time_ = 50;
      break;
    case 4:
      bit_width_val_ = 16;
      measurement_time_ = 25;
      break;
    case 5:
      bit_width_val_ = 13;
      measurement_time_ = 3.125;
      break;
  }
}

}  // namespace apds9306
}  // namespace esphome
