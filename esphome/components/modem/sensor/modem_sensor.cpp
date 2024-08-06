#ifdef USE_ESP_IDF

#include "esphome/core/defines.h"

#ifdef USE_MODEM
#ifdef USE_SENSOR

#include "modem_sensor.h"

#include "esphome/core/log.h"
#include "esphome/core/application.h"

#include "esphome/components/modem/modem_component.h"

#include <string>
#include <vector>
#include <sstream>

#define ESPHL_ERROR_CHECK(err, message) \
  if ((err) != ESP_OK) { \
    ESP_LOGE(TAG, message ": (%d) %s", err, esp_err_to_name(err)); \
    this->mark_failed(); \
    return; \
  }

#define ESPMODEM_ERROR_CHECK(err, message) \
  if ((err) != command_result::OK) { \
    ESP_LOGE(TAG, message ": %s", command_result_to_string(err).c_str()); \
  }

namespace esphome {
namespace modem_sensor {

using namespace esp_modem;

void ModemSensor::setup() { ESP_LOGI(TAG, "Setting up Modem Sensor..."); }

void ModemSensor::update() {
  ESP_LOGD(TAG, "Modem sensor update");
  if (modem::global_modem_component->dce && modem::global_modem_component->modem_ready()) {
    this->update_signal_sensors_();
    App.feed_wdt();
    this->update_gnss_sensors_();
  }
}

void ModemSensor::update_signal_sensors_() {
  float rssi = NAN;
  float ber = NAN;
  if (this->rssi_sensor_ || this->ber_sensor_) {
    int modem_rssi, modem_ber;
    if (modem::global_modem_component->dce && modem::global_modem_component->modem_ready()) {
      modem::global_modem_component->dce->get_signal_quality(modem_rssi, modem_ber);
      if (this->rssi_sensor_ && modem_rssi != 99) {
        rssi = -113 + (modem_rssi * 2);
        this->rssi_sensor_->publish_state(rssi);
      }
      if (this->ber_sensor_ && modem_ber != 99) {
        ber = 0.1f * (modem_ber * modem_ber);
        this->ber_sensor_->publish_state(ber);
      }
      ESP_LOGD(TAG, "Modem sensor rssi: %d", modem_rssi);
    }
  }
}

#ifdef USE_MODEM_GNSS
std::map<std::string, std::string> get_gnssinfo_tokens(const std::string &gnss_info, const std::string &module) {
  // for 7670 (18 tokens):
  //    +CGNSSINFO: 3,12,,04,00,48.6167297,N,4.5600739,W,060824,101218.00,75.7,0.000,234.10,2.52,1.88,1.68,08
  // for 7600 (16 tokens):
  //     +CGNSSINFO: 2,04,03,00,4836.989133,N,00433.611595,W,060824,102247.0,-13.8,0.0,70.4,1.7,1.4,1.0

  std::string data = gnss_info.substr(12);

  std::map<std::string, std::string> gnss_data;

  if (data.find(",,,,,,") != std::string::npos) {
    // no data recieved
    return gnss_data;
  }

  std::vector<std::string> parts;
  char delimiter = ',';
  std::istringstream token_stream(data);
  std::string part;
  while (std::getline(token_stream, part, delimiter)) {
    parts.push_back(part);
  }

  switch (parts.size()) {
    case 16:
      gnss_data["mode"] = parts[0];
      gnss_data["sat_used_count"] = parts[1];
      gnss_data["sat_view_count"] = parts[2];  // Satellites in view
      gnss_data["fix_status"] = parts[3];
      gnss_data["latitude"] = parts[4];
      gnss_data["lat_dir"] = parts[5];
      gnss_data["longitude"] = parts[6];
      gnss_data["lon_dir"] = parts[7];
      gnss_data["date"] = parts[8];
      gnss_data["time"] = parts[9];
      gnss_data["altitude"] = parts[10];
      gnss_data["speed"] = parts[11];
      gnss_data["cog"] = parts[12];
      gnss_data["hdop"] = parts[13];
      gnss_data["vdop"] = parts[14];
      gnss_data["pdop"] = parts[15];
      gnss_data["lon_lat_format"] = "DDMM.MM";  // decimal degrees, float minutes
      break;

    case 18:
      gnss_data["mode"] = parts[0];
      gnss_data["sat_used_count"] = parts[1];
      gnss_data["fix_status"] = parts[2];      // Primary fix status
      gnss_data["sat_view_count"] = parts[3];  // Satellites in view
      gnss_data["fix_status_2"] = parts[4];    // Additional fix status if needed
      gnss_data["latitude"] = parts[5];
      gnss_data["lat_dir"] = parts[6];
      gnss_data["longitude"] = parts[7];
      gnss_data["lon_dir"] = parts[8];
      gnss_data["date"] = parts[9];
      gnss_data["time"] = parts[10];
      gnss_data["altitude"] = parts[11];
      gnss_data["speed"] = parts[12];
      gnss_data["cog"] = parts[13];
      gnss_data["hdop"] = parts[14];
      gnss_data["vdop"] = parts[15];
      gnss_data["pdop"] = parts[16];
      gnss_data["sat_view_count_2"] = parts[17];  // Additional satellites in view if needed
      gnss_data["lon_lat_format"] = "DD.DD";      // 48.34567  (decimal)
      break;

    default:
      ESP_LOGE(TAG, "Unknown gnssinfo len %d", parts.size());
      break;
  }

  for (const auto &pair : gnss_data) {
    ESP_LOGV(TAG, "GNSSINFO token %s: %s", pair.first.c_str(), pair.second.c_str());
  }

  return gnss_data;
}

void ModemSensor::update_gnss_sensors_() {
  if (this->gnss_latitude_sensor_ || this->gnss_longitude_sensor_ || this->gnss_altitude_sensor_) {
    std::string gnss_info = modem::global_modem_component->send_at("AT+CGNSSINFO");

    if (gnss_info != "ERROR") {
      std::map<std::string, std::string> parts = get_gnssinfo_tokens(gnss_info, "SIM7600");

      if (parts["latitude"].empty() || parts["lat_dir"].empty() || parts["longitude"].empty() ||
          parts["lon_dir"].empty()) {
        return;
      }

      float lat = NAN;
      float lon = NAN;

      if (parts["lon_lat_format"] == "DDMM.MM") {
        float lat_deg = std::stof(parts["latitude"].substr(0, 2));
        float lat_min = std::stof(parts["latitude"].substr(2));
        lat = lat_deg + (lat_min / 60.0);
        if (parts["lat_dir"] == "S")
          lat = -lat;

        float lon_deg = std::stof(parts["longitude"].substr(0, 3));
        float lon_min = std::stof(parts["longitude"].substr(3));
        lon = lon_deg + (lon_min / 60.0);
        if (parts["lon_dir"] == "W")
          lon = -lon;
      } else if (parts["lon_lat_format"] == "DD.DD") {
        lat = std::stof(parts["latitude"]);
        if (parts["lat_dir"] == "S")
          lat = -lat;

        lon = std::stof(parts["longitude"]);
        if (parts["lon_dir"] == "W")
          lon = -lon;
      }

      float alt = std::stof(parts["altitude"]);
      float speed_knots = std::stof(parts["speed"]);
      float speed_kmh = speed_knots * 1.852;  // Convert speed from knots to km/h
      float cog = parts["cog"].empty() ? NAN : std::stof(parts["cog"]);
      float pdop = std::stof(parts["pdop"]);
      float hdop = std::stof(parts["hdop"]);
      float vdop = std::stof(parts["vdop"]);
      int mode = std::stoi(parts["mode"]);
      int gps_svs = std::stoi(parts["sat_used_count"]);
      int glonass_svs = std::stoi(parts["sat_view_count"]);
      int beidou_svs = parts["sat_view_count_2"].empty() ? 0 : std::stoi(parts["sat_view_count_2"]);

      // Parsing date
      int day = std::stoi(parts["date"].substr(0, 2));
      int month = std::stoi(parts["date"].substr(2, 2));
      int year = std::stoi(parts["date"].substr(4, 2)) + 2000;

      // Parsing time
      int hour = std::stoi(parts["time"].substr(0, 2));
      int minute = std::stoi(parts["time"].substr(2, 2));
      int second = std::stoi(parts["time"].substr(4, 2));

      ESP_LOGD(TAG, "Latitude: %f, Longitude: %f", lat, lon);
      ESP_LOGD(TAG, "Altitude: %f m", alt);
      ESP_LOGD(TAG, "Speed: %f km/h", speed_kmh);
      ESP_LOGD(TAG, "COG: %f degrees", cog);
      ESP_LOGD(TAG, "PDOP: %f", pdop);
      ESP_LOGD(TAG, "HDOP: %f", hdop);
      ESP_LOGD(TAG, "VDOP: %f", vdop);
      ESP_LOGD(TAG, "GPS SVs: %d", gps_svs);
      ESP_LOGD(TAG, "GLONASS SVs: %d", glonass_svs);
      ESP_LOGD(TAG, "BEIDOU SVs: %d", beidou_svs);
      ESP_LOGD(TAG, "Fix mode: %d", mode);
      ESP_LOGD(TAG, "Date: %04d-%02d-%02d", year, month, day);
      ESP_LOGD(TAG, "Time: %02d:%02d:%02d", hour, minute, second);

      // Sensors update
      if (this->gnss_latitude_sensor_)
        this->gnss_latitude_sensor_->publish_state(lat);
      if (this->gnss_longitude_sensor_)
        this->gnss_longitude_sensor_->publish_state(lon);
      if (this->gnss_altitude_sensor_)
        this->gnss_altitude_sensor_->publish_state(alt);
      if (this->gnss_speed_sensor_)
        this->gnss_speed_sensor_->publish_state(speed_kmh);
      if (this->gnss_cog_sensor_)
        this->gnss_cog_sensor_->publish_state(speed_kmh);
      if (this->gnss_pdop_sensor_)
        this->gnss_pdop_sensor_->publish_state(pdop);
      if (this->gnss_hdop_sensor_)
        this->gnss_hdop_sensor_->publish_state(hdop);
      if (this->gnss_vdop_sensor_)
        this->gnss_vdop_sensor_->publish_state(vdop);
      if (this->gnss_mode_sensor_)
        this->gnss_mode_sensor_->publish_state(mode);
    }
  }
}

#endif  // USE_MODEM_GNSS

}  // namespace modem_sensor
}  // namespace esphome

#endif  // USE_MODEM
#endif  // USE_SENSOR
#endif  // USE_ESP_IDF
