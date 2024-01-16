#pragma once

#ifdef USE_ESP32

#include <freertos/FreeRTOS.h>
#include <freertos/stream_buffer.h>

#include <cinttypes>

namespace esphome {

class RingBuffer {
 public:
  size_t read(void *data, size_t size, TickType_t ticks_to_wait = 0);

  size_t write(void *data, size_t len);

  size_t available() const;
  size_t free() const;

  BaseType_t reset();

  static RingBuffer *create(size_t size);

 protected:
  StreamBufferHandle_t handle_;
  StaticStreamBuffer_t structure;
  uint8_t *storage;
};

}  // namespace esphome

#endif
