#include "led_strip.h"

#ifdef USE_RP2040

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

#include <pico/stdlib.h>
#include <hardware/pio.h>
#include <hardware/clocks.h>
#include "SK6812.pio.h"
#include "SM16703.pio.h"
#include "WS2812.pio.h"
#include "SK6812B.pio.h"

namespace esphome {
namespace rp2040_pio_led_strip {

static const char *TAG = "rp2040_pio_led_strip";

void RP2040PIOLEDStripLightOutput::setup() {
	ESP_LOGCONFIG(TAG, "Setting up RP2040 LED Strip...");

	size_t buffer_size = this->get_buffer_size_();

	ExternalRAMAllocator<uint8_t> allocator(ExternalRAMAllocator<uint8_t>::ALLOW_FAILURE);
	this->buf_ = allocator.allocate(buffer_size);
	if (this->buf_ == nullptr) {
		ESP_LOGE(TAG, "Failed to allocate buffer of size %u", buffer_size);
		this->mark_failed();
		return;
  	}

	this->write_buf_ = allocator.allocate(buffer_size);
	if (this->write_buf_ == nullptr) {
		ESP_LOGE(TAG, "Failed to allocate write buffer of size %u", buffer_size);
		this->mark_failed();
		return;
  	}

  	this->effect_data_ = allocator.allocate(this->num_leds_);
  	if (this->effect_data_ == nullptr) {
		ESP_LOGE(TAG, "Failed to allocate effect data of size %u", this->num_leds_);
		this->mark_failed();
		return;
  	}

  	// Select PIO instance to use (0 or 1)
  	this->pio_ = pio0;
	if (this->pio_ == nullptr) {
		ESP_LOGE(TAG, "Failed to claim PIO instance");
		this->mark_failed();
		return;
	}

	switch(this->chipset_) {
		case Chipset::WS2812:
			ESP_LOGCONFIG(TAG, "Using WS2812 chipset");
			this->pio_program_ = &rp2040_pio_led_ws2812_driver_program;
			break;
		case Chipset::SK6812:
			ESP_LOGCONFIG(TAG, "Using SK6812 chipset");
			this->pio_program_ = &rp2040_pio_led_sk6812_driver_program;
			break;
		case Chipset::SM16703:
			ESP_LOGCONFIG(TAG, "Using SM16703 chipset");
			this->pio_program_ = &rp2040_pio_led_sm16703_driver_program;
		case Chipset::WS2812B:
			ESP_LOGCONFIG(TAG, "Using WS2812B chipset");
			this->pio_program_ = &rp2040_pio_led_ws2812b_driver_program;
			break;
	}

  	// Load the assembled program into the PIO and get its location in the PIO's instruction memory
  	uint offset = pio_add_program(this->pio_, this->pio_program_);

  	// Configure the state machine's PIO, and start it
  	this->sm_ = pio_claim_unused_sm(this->pio_, true);
  	if (this->sm_ < 0) {
		ESP_LOGE(TAG, "Failed to claim PIO state machine");
		this->mark_failed();
		return;
  	}

	switch (this->chipset_) {
		case Chipset::WS2812:
			rp2040_pio_WS2812_init(this->pio_, this->sm_, offset, this->pin_, 0.0f);
			break;
		case Chipset::SK6812:
			rp2040_pio_SK6812_init(this->pio_, this->sm_, offset, this->pin_, 0.0f);
			break;
		case Chipset::SM16703:
			rp2040_pio_SM16703_init(this->pio_, this->sm_, offset, this->pin_, 0.0f);
			this->pio_program_ = &rp2040_pio_led_sm16703_driver_program;
		case Chipset::WS2812B:
			rp2040_pio_WS2812B_init(this->pio_, this->sm_, offset, this->pin_, 0.0f);
			break;
	}
}

void RP2040PIOLEDStripLightOutput::write_state(light::LightState *state) {
	ESP_LOGVV(TAG, "Writing state...");

	if (this->is_failed()) {
		ESP_LOGW(TAG, "Light is in failed state, not writing state.");
		return;
	}

	if (this->buf_ == nullptr) {
		ESP_LOGW(TAG, "Buffer is null, not writing state.");
		return;
	}

	// Convert the light state in this->buf_ to uint32_t to write to the LED strip
	memcpy(this->write_buf_, this->buf_, this->get_buffer_size_());

	// assemble bits in buffer to 32 bit words with 0bGGGGGGGGRRRRRRRRBBBBBBBB00000000
	for (int i = 0; i < this->num_leds_; i++) {
		uint8_t r = this->write_buf_[(i * 3) + 0];
		uint8_t g = this->write_buf_[(i * 3) + 1];
		uint8_t b = this->write_buf_[(i * 3) + 2];
		uint32_t grb = (g << 24) | (r << 16) | b << 8;
		ESP_LOGVV(TAG, "Writing 0x%08x to LED %d", grb, i);
		pio_sm_put_blocking(this->pio_, this->sm_, grb);
	}
}

light::ESPColorView RP2040PIOLEDStripLightOutput::get_view_internal(int32_t index) const {
	int32_t r = 0, g = 0, b = 0, w = 0;
	switch (this->rgb_order_) {
		case ORDER_RGB:
			r = 0;
			g = 1;
			b = 2;
			break;
		case ORDER_RBG:
			r = 0;
			g = 2;
			b = 1;
			break;
		case ORDER_GRB:
			r = 1;
			g = 0;
			b = 2;
		break;
		case ORDER_GBR:
			r = 2;
			g = 0;
			b = 1;
			break;
		case ORDER_BGR:
			r = 2;
			g = 1;
			b = 0;
			break;
		case ORDER_BRG:
			r = 1;
			g = 2;
			b = 0;
			break;
	}
	uint8_t multiplier = this->is_rgbw_ ? 4 : 3;
	return {
		this->buf_ + (index * multiplier) + r,
        this->buf_ + (index * multiplier) + g,
        this->buf_ + (index * multiplier) + b,
        this->is_rgbw_ ? this->buf_ + (index * multiplier) + 3 : nullptr,
        &this->effect_data_[index],
        &this->correction_
		};
}

void RP2040PIOLEDStripLightOutput::dump_config() {
	ESP_LOGCONFIG(TAG, "RP2040 PIO LED Strip Light Output:");
	ESP_LOGCONFIG(TAG, "  Pin: GPIO%d", this->pin_);
	ESP_LOGCONFIG(TAG, "  Number of LEDs: %d", this->num_leds_);
	ESP_LOGCONFIG(TAG, "  RGBW: %s", YESNO(this->is_rgbw_));
	ESP_LOGCONFIG(TAG, "  RGB Order: %s", rgb_order_to_string(this->rgb_order_));
	ESP_LOGCONFIG(TAG, "  Max Refresh Rate: %f Hz", this->max_refresh_rate_);
}

float RP2040PIOLEDStripLightOutput::get_setup_priority() const {return setup_priority::HARDWARE;}
	


}  // namespace rp2040_led_strip
}  // namespace esphome

#endif