#pragma once
#include "esphome/core/defines.h"
#ifdef USE_ESP_IDF

#include "ota_component.h"
#include "ota_backend.h"
#include <esp_ota_ops.h>
#include "esphome/components/md5/md5.h"

namespace esphome {
namespace ota {

class IDFOTABackend : public OTABackend {
 public:
  OTAResponseTypes begin(OTABinType bin_type, size_t image_size) override;
  void set_update_md5(const char *md5) override;
  OTAResponseTypes write(uint8_t *data, size_t len) override;
  OTAResponseTypes end() override;
  void abort() override;
  bool supports_compression() override { return false; }
#ifdef USE_UNPROTECTED_WRITES
  bool supports_writing_bootloader() override { return false; }
  bool supports_writing_partition_table() override { return false; }
  bool supports_writing_partitions() override { return false; }
#else
  bool supports_writing_bootloader() override { return true; }
  bool supports_writing_partition_table() override { return true; }
  bool supports_writing_partitions() override { return false; }
#endif
 private:
  static void log_partitions();
  void deregister_partitions();

  OTABinType bin_type_;
  esp_ota_handle_t update_handle_{0};
  size_t data_written_;
  const esp_partition_t *partition_;
  const esp_partition_t *bootloader_partition_{0};
  const esp_partition_t *partition_table_partition_{0};
  md5::MD5Digest md5_{};
  char expected_bin_md5_[32];
};

}  // namespace ota
}  // namespace esphome
#endif
