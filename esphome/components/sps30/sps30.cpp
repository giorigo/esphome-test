#include "sps30.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sps30 {

static const char *TAG = "sps30";

static const uint16_t SPS30_CMD_GET_ARTICLE_CODE = 0xD025;
static const uint16_t SPS30_CMD_GET_SERIAL_NUMBER = 0xD033;
static const uint16_t SPS30_CMD_GET_FIRMWARE_VERSION = 0xD100;

static const uint16_t SPS30_CMD_START_CONTINUOUS_MEASUREMENTS = 0x0010;
static const uint16_t SPS30_CMD_START_CONTINUOUS_MEASUREMENTS_ARG = 0x0300;
static const uint16_t SPS30_CMD_GET_DATA_READY_STATUS = 0x0202;
static const uint16_t SPS30_CMD_READ_MEASUREMENT = 0x0300;
static const uint16_t SPS30_CMD_STOP_MEASUREMENTS = 0x0104;
static const uint16_t SPS30_CMD_SET_AUTOMATIC_CLEANING_INTERVAL_SECONDS = 0x8004;
static const uint16_t SPS30_CMD_START_FAN_CLEANING = 0x5607;
static const uint16_t SPS30_CMD_SOFT_RESET = 0xD304;
static const size_t SERIAL_NUMBER_LENGTH = 8;

void SPS30Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up sps30...");
  this->write_command_(SPS30_CMD_SOFT_RESET);
  delayMicroseconds(500000);
  /// Firmware version identification
  if (!this->write_command_(SPS30_CMD_GET_FIRMWARE_VERSION)) {
    this->error_code_ = FIRMWARE_VERSION_REQUEST_FAILED;
    this->mark_failed();
    return;
  }

  uint16_t raw_firmware_version[4];
  if (!this->read_data_(raw_firmware_version, 4)) {
    this->error_code_ = FIRMWARE_VERSION_READ_FAILED;
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "  Firmware version v%0d.%02d", (uint16_t(raw_firmware_version[0]) >> 8),
           uint16_t(raw_firmware_version[0] & 0xFF));
  /// Serial number identification
  if (!this->write_command_(SPS30_CMD_GET_SERIAL_NUMBER)) {
    this->error_code_ = SERIAL_NUMBER_REQUEST_FAILED;
    this->mark_failed();
    return;
  }

  uint16_t raw_serial_number[8];
  if (!this->read_data_(raw_serial_number, 8)) {
    this->error_code_ = SERIAL_NUMBER_READ_FAILED;
    this->mark_failed();
    return;
  }

  for(size_t i=0; i<8; ++i) {
    this->serial_number_[i*2] = uint16_t(uint16_t(raw_serial_number[i]) >> 8);
    this->serial_number_[i*2+1] = uint16_t(uint16_t(raw_serial_number[i] & 0xFF));
  }
  ESP_LOGD(TAG, "  Serial Number: '%s'", this->serial_number_);

  this->start_continuous_measurement_();
}

void SPS30Component::dump_config() {
  ESP_LOGCONFIG(TAG, "sps30:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    switch (this->error_code_) {
      case COMMUNICATION_FAILED:
        ESP_LOGW(TAG, "Communication failed! Is the sensor connected?");
        break;
      case MEASUREMENT_INIT_FAILED:
        ESP_LOGW(TAG, "Measurement Initialization failed!");
        break;
      case SERIAL_NUMBER_REQUEST_FAILED:
        ESP_LOGW(TAG, "Unable to request sensor serial number");
        break;
      case SERIAL_NUMBER_READ_FAILED:
        ESP_LOGW(TAG, "Unable to read sensor serial number");
        break;
      case FIRMWARE_VERSION_REQUEST_FAILED:
        ESP_LOGW(TAG, "Unable to request sensor firmware version");
        break;
      case FIRMWARE_VERSION_READ_FAILED:
        ESP_LOGW(TAG, "Unable to read sensor firmware version");
        break;
      default:
        ESP_LOGW(TAG, "Unknown setup error!");
        break;
    }
  }
  LOG_UPDATE_INTERVAL(this);
  ESP_LOGCONFIG(TAG, "  Serial Number: '%s'", this->serial_number_);
  LOG_SENSOR("  ", "PM1.0", this->pm_1_0_sensor_);
  LOG_SENSOR("  ", "PM2.5", this->pm_2_5_sensor_);
  LOG_SENSOR("  ", "PM4", this->pm_4_0_sensor_);
  LOG_SENSOR("  ", "PM10", this->pm_10_0_sensor_);
}

void SPS30Component::update() {
  /// Check if warning flag active (sensor reconnected?)
  if (this->status_has_warning()) {
    ESP_LOGD(TAG, "Retrying to reconnect the sensor.");
    if (this->write_command_(SPS30_CMD_SOFT_RESET)) {
      /// Sensor restarted and reading attempt made next cycle
      delayMicroseconds(500000);
      this->start_continuous_measurement_();
      this->status_clear_warning();
      ESP_LOGD(TAG, "Sensor reconnected!");
    } else {
      ESP_LOGD(TAG, "Sensor offline?");
    }
  } else {
    /// Check if measurement is ready before reading the value
    if (!this->write_command_(SPS30_CMD_GET_DATA_READY_STATUS)) {
      this->status_set_warning();
      return;
    }

    uint16_t raw_read_status[1];
    if (!this->read_data_(raw_read_status, 1) || raw_read_status[0] == 0x00) {
      ESP_LOGW(TAG, "Data not ready yet!");
      return;
    }

    if (!this->write_command_(SPS30_CMD_READ_MEASUREMENT)) {
      ESP_LOGW(TAG, "Error reading measurement status!");
      this->status_set_warning();
      return;
    }

    this->set_timeout(50, [this]() {
      uint16_t raw_data[20];
      if (!this->read_data_(raw_data, 20)) {
        ESP_LOGW(TAG, "Error reading measurement data!");
        this->status_set_warning();
        return;
      }

      union uint32_float_t {
        uint32_t uint32;
        float value;
      };

      /// Reading and converting Mass concentration
      uint32_float_t pm_1_0{.uint32 = (((uint32_t(raw_data[0])) << 16) | (uint32_t(raw_data[1])))};
      uint32_float_t pm_2_5{.uint32 = (((uint32_t(raw_data[2])) << 16) | (uint32_t(raw_data[3])))};
      uint32_float_t pm_4_0{.uint32 = (((uint32_t(raw_data[4])) << 16) | (uint32_t(raw_data[5])))};
      uint32_float_t pm_10_0{.uint32 = (((uint32_t(raw_data[6])) << 16) | (uint32_t(raw_data[7])))};

      /// Reading and converting Number concentration
      uint32_float_t pmc_0_5{.uint32 = (((uint32_t(raw_data[8])) << 16) | (uint32_t(raw_data[9])))};
      uint32_float_t pmc_1_0{.uint32 = (((uint32_t(raw_data[10])) << 16) | (uint32_t(raw_data[11])))};
      uint32_float_t pmc_2_5{.uint32 = (((uint32_t(raw_data[12])) << 16) | (uint32_t(raw_data[13])))};
      uint32_float_t pmc_4_0{.uint32 = (((uint32_t(raw_data[14])) << 16) | (uint32_t(raw_data[15])))};
      uint32_float_t pmc_10_0{.uint32 = (((uint32_t(raw_data[16])) << 16) \
      | (uint32_t(raw_data[17])))};

      /// Reading and converting Typical size
      uint32_float_t pm_size{.uint32 = (((uint32_t(raw_data[18])) << 16) | (uint32_t(raw_data[19])))};

      if (this->pm_1_0_sensor_ != nullptr)
        this->pm_1_0_sensor_->publish_state(pm_1_0.value);
      if (this->pm_2_5_sensor_ != nullptr)
        this->pm_2_5_sensor_->publish_state(pm_2_5.value);
      if (this->pm_4_0_sensor_ != nullptr)
        this->pm_4_0_sensor_->publish_state(pm_4_0.value);
      if (this->pm_10_0_sensor_ != nullptr)
        this->pm_10_0_sensor_->publish_state(pm_10_0.value);

      if (this->pmc_0_5_sensor_ != nullptr)
        this->pmc_0_5_sensor_->publish_state(pmc_0_5.value);
      if (this->pmc_1_0_sensor_ != nullptr)
        this->pmc_1_0_sensor_->publish_state(pmc_1_0.value);
      if (this->pmc_2_5_sensor_ != nullptr)
        this->pmc_2_5_sensor_->publish_state(pmc_2_5.value);
      if (this->pmc_4_0_sensor_ != nullptr)
        this->pmc_4_0_sensor_->publish_state(pmc_4_0.value);
      if (this->pmc_10_0_sensor_ != nullptr)
        this->pmc_10_0_sensor_->publish_state(pmc_10_0.value);

      if (this->pm_size_sensor_ != nullptr)
        this->pm_size_sensor_->publish_state(pm_size.value);

      this->status_clear_warning();
    });
  }
}

bool SPS30Component::write_command_(uint16_t command) {
  // Warning ugly, trick the I2Ccomponent base by setting register to the first 8 bit.
  return this->write_byte(command >> 8, command & 0xFF);
}

uint8_t SPS30Component::sht_crc_(uint8_t data1, uint8_t data2) {
  uint8_t bit;
  uint8_t crc = 0xFF;

  crc ^= data1;
  for (bit = 8; bit > 0; --bit) {
    if (crc & 0x80)
      crc = (crc << 1) ^ 0x131;
    else
      crc = (crc << 1);
  }

  crc ^= data2;
  for (bit = 8; bit > 0; --bit) {
    if (crc & 0x80)
      crc = (crc << 1) ^ 0x131;
    else
      crc = (crc << 1);
  }

  return crc;
}

bool SPS30Component::start_continuous_measurement_() {
  uint8_t data[4];
  data[0] = SPS30_CMD_START_CONTINUOUS_MEASUREMENTS & 0xFF;
  data[1] = 0x03;
  data[2] = 0x00;
  data[3] = sht_crc_(0x03, 0x00);
  if (!this->write_bytes(SPS30_CMD_START_CONTINUOUS_MEASUREMENTS >> 8, data, 4)) {
    ESP_LOGE(TAG, "Error initiating measurements");
    return false;
  }
  return true;
}

bool SPS30Component::read_data_(uint16_t *data, uint8_t len) {
  const uint8_t num_bytes = len * 3;
  auto *buf = new uint8_t[num_bytes];

  if (!this->parent_->raw_receive(this->address_, buf, num_bytes)) {
    delete[](buf);
    return false;
  }

  for (uint8_t i = 0; i < len; i++) {
    const uint8_t j = 3 * i;
    uint8_t crc = sht_crc_(buf[j], buf[j + 1]);
    if (crc != buf[j + 2]) {
      ESP_LOGE(TAG, "CRC8 Checksum invalid! 0x%02X != 0x%02X", buf[j + 2], crc);
      delete[](buf);
      return false;
    }
    data[i] = (buf[j] << 8) | buf[j + 1];
  }

  delete[](buf);
  return true;
}

}  // namespace sps30
}  // namespace esphome
