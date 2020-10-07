#include "ndef_message.h"

namespace esphome {
namespace nfc {

static const char *TAG = "nfc.ndef_message";

NdefMessage::NdefMessage(std::vector<uint8_t> &data) {
  ESP_LOGV(TAG, "Building NdefMessage with %zu bytes", data.size());
  uint8_t index = 0;
  while (index <= data.size()) {
    uint8_t tnf_byte = data[index++];
    bool me = tnf_byte & 0x40;
    bool sr = tnf_byte & 0x10;
    bool il = tnf_byte & 0x08;
    uint8_t tnf = tnf_byte & 0x07;

    auto record = new NdefRecord(tnf);

    uint8_t type_length = data[index++];
    uint32_t payload_length = 0;
    if (sr) {
      payload_length = data[index++];
    } else {
      payload_length = (static_cast<uint32_t>(data[index]) << 24) | (static_cast<uint32_t>(data[index + 1]) << 16) |
                       (static_cast<uint32_t>(data[index + 2]) << 8) | static_cast<uint32_t>(data[index + 3]);
      index += 4;
    }

    uint8_t id_length = 0;
    if (il) {
      id_length = data[index++];
    }

    std::string type_str(data.begin() + index, data.begin() + index + type_length);
    record->set_type(type_str);
    index += type_length;

    if (il) {
      std::string id_str(data.begin() + index, data.begin() + index + id_length);
      record->set_id(id_str);
      index += id_length;
    }

    std::string payload_str(data.begin() + index, data.begin() + index + payload_length);
    record->set_payload(payload_str);
    index += payload_length;

    this->add_record(record);
    ESP_LOGV(TAG, "Adding record type %s = %s", record->get_type().c_str(), record->get_payload().c_str());

    if (me)
      break;
  }
}

bool NdefMessage::add_record(NdefRecord *record) {
  if (this->records_.size() >= MAX_NDEF_RECORDS) {
    ESP_LOGE(TAG, "Too many records. Max: %d", MAX_NDEF_RECORDS);
    return false;
  }
  this->records_.push_back(record);
  return true;
}

bool NdefMessage::add_text_record(const std::string &text) { return this->add_text_record(text, "en"); };

bool NdefMessage::add_text_record(const std::string &text, const std::string &encoding) {
  std::string payload = to_string(text.length()) + encoding + text;
  auto r = new NdefRecord(TNF_WELL_KNOWN, "T", payload);
  return this->add_record(r);
}

bool NdefMessage::add_uri_record(const std::string &uri) {
  std::string payload = '\0' + uri;
  auto r = new NdefRecord(TNF_WELL_KNOWN, "U", payload);
  return this->add_record(r);
}

bool NdefMessage::add_empty_record() {
  auto r = new NdefRecord(TNF_EMPTY);
  return this->add_record(r);
}

std::vector<uint8_t> NdefMessage::encode() {
  std::vector<uint8_t> data;

  for (uint8_t i = 0; i < this->records_.size(); i++) {
    auto encoded_record = this->records_[i]->encode(i == 0, (i + 1) == this->records_.size());
    data.insert(data.end(), encoded_record.begin(), encoded_record.end());
  }
  return data;
}

}  // namespace nfc
}  // namespace esphome
