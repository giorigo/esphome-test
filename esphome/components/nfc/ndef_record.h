#pragma once

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace nfc {

static const uint8_t TNF_EMPTY = 0x00;
static const uint8_t TNF_WELL_KNOWN = 0x01;
static const uint8_t TNF_MIME_MEDIA = 0x02;
static const uint8_t TNF_ABSOLUTE_URI = 0x03;
static const uint8_t TNF_EXTERNAL_TYPE = 0x04;
static const uint8_t TNF_UNKNOWN = 0x05;
static const uint8_t TNF_UNCHANGED = 0x06;
static const uint8_t TNF_RESERVED = 0x07;

class NdefRecord {
 public:
  NdefRecord(){};
  NdefRecord(std::vector<uint8_t> payloadData);
  void set_tnf(uint8_t tnf) { this->tnf_ = tnf; };
  void set_type(const std::string &type) { this->type_ = type; };
  void set_payload(const std::string &payload) { this->payload_ = payload; };
  void set_id(const std::string &id) { this->id_ = id; };

  uint32_t get_encoded_size();

  std::vector<uint8_t> encode(bool first, bool last);

  uint8_t create_flag_byte(bool first, bool last);

  const std::string &get_type() { return this->type_; };
  const std::string &get_id() { return this->id_; };
  virtual const std::string get_payload() { return this->payload_; };

  virtual std::vector<uint8_t> getEncodedPayload() {
    std::vector<uint8_t> emptyPayload;
    return emptyPayload;
  };

 protected:
  uint8_t tnf_;
  std::string type_;
  std::string id_;
  std::string payload_;
};

}  // namespace nfc
}  // namespace esphome
