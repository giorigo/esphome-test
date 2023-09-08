/// @file wk2132.cpp
/// @author DrCoolzic
/// @brief wk2132 classes implementation

#include "wk2132.h"

namespace esphome {
namespace wk2132 {

/*! @mainpage WK2132 Documentation
 This page gives some information about the details of the implementation of
 the WK2132Component in ESPHome. This component uses two main classes:
 WK2132Component and  WK2132Channel classes as well as the  RingBuffer helper
 class. Below you will find a short description of these three classes.

 @n But before that let see how the different classes interact with ESPHome.
 We have the following UML class diagram:

 <img src="WK2132 Class diagram.png" align="left" width="1024">
 <div style="clear: both"></div>

 As you can see, the WK2132Component class derives from the two ESPHome classes:
 esphome::Component and the i2c::I2CDevice. Therefore this class has to implement all the
 virtual methods from these two classes.
 @n @n Another important class is the WK2132Channel class. The relationship between
 WK2132Component and WK2132Channel is an aggregation. The WK2132Component is
 a container that contains one or two WK2132Channel instances. However there
 is not a strong dependency as the WK2132Channel instances continue to live
 if the WK2132Component it belongs to is destroyed (which never happen in ESPHome).
 The list of associated WK2132Channel instances in a WK2132Component is kept in
 the WK2132Component::children_ attribute.
 @n The WK2132Channel derives from the ESPHome uart::UARTComponent class.
 Therefore it has to implement all the virtual methods from this class.
 There is a composition relationship between the WK2132Channel class and the
 RingBuffer helper class. Therefore destroying a WK2132Channel instance
 also destroys the associated RingBuffer instance.
 @n @n The last class defined in the WK2132 component is a helper class
 called RingBuffer. It is a very simple container that implements the
 functionality of a FIFO : bytes are pushed into the container on one side
 and they are popped from the other side in the same order as they entered.

 @section WK2132Component The WK2132Component class
 TODO

 @section WK2132Channel_ The WK2132Channel class
 We have one instance of this class per UART channel. This class implements
 all the virtual methods of the ESPHome uart::UARTComponent class.
 Unfortunately as far as I know there is **no documentation** about the uart::UARTDevice and
 uart::UARTComponent classes of @ref ESPHome. However it seems that both of them are based
 on equivalent Arduino classes.\n
 This help a little bit in terms of documentation as we can look for the methods counterpart
 in the Arduino library. However the interfaces provided by the Arduino Serial library
 are **poorly defined** and have even \b changed over time!\n
 Note that for compatibility reason (?) many helper methods are made available in ESPHome
 to read and write. Unfortunately in many cases these helpers are missing the critical
 status information and therefore are even more unsafe to use than the original...\n
 You can review each of these function in the list provided below.
 But first lets describe the optimum usage of these methods to get the best performance.

 Usually UART are used to communicate with remote devices following this protocol:
   -# the master (usually the microcontroller) asks the remote slave device for information
   by sending a specific command.
   -# in response the slave send back the information requested by the master as a frame
   of many bytes
   -# the master process the information received and prepare the next request.
   -# and these steps repeat forever

 With such protocol and the available methods, the optimum would look like this:
  @code
  constexpr size_t CMD_SIZE = 23;
  uint8_t command_buffer[CMD_SIZE];
  auto uart = new uart::UARTDevice();

  // infinite loop to do requests and get responses
  while (true) {
    //
    // prepare the command buffer
    // ...
    uart->flush();  // we get rid of bytes in UART FIFO if needed
    uart->write_array(command_buffer, CMD_SIZE); // we send all bytes
    int available = 0;
    // loop until all bytes processed
    while ((available = uart->available())) {
      uint8_t rec_buffer[available];
      uart->read_array(rec_buffer, available);
      // here we loop for all received bytes
      for (auto byte : rec_buffer) {
        //
        // here we process each byte received
        // ...
      }
    }
  }
  @endcode

 But unfortunately all the code I have reviewed are not written like that.
 They actually look more like this:
  @code
  constexpr size_t CMD_SIZE = 23;
  uint8_t command_buffer[CMD_SIZE];
  auto uart = new uart::UARTDevice();

  // infinite loop to do requests and get responses
  while (true) {
    //
    // prepare the command buffer
    // ...
    uart->write_array(command_buffer, CMD_SIZE);
    uart->flush();  // we get rid of bytes still in UART FIFO just after write!
    while (uart->available()) {
      uint8_t byte = uart->read();
      //
      // here we process each byte received
      // ...
    }
  }
  @endcode

 So what is wrong about this last code ?
  -# using write_array() followed immediately by a flush() method is
  asking the UART to wait for all bytes in the fifo to be gone. And
  this can take a lot of time especially if the line speed is low.
    - In the optimum code, we reverse the call: we do the flush() first
    and the write_array() after. In this case the flush() should return
    immediately because there is plenty of time spend in receiving and
    processing the bytes before we do the flush() in the next loop.
  -# using available() followed by reading one byte with read() in a
  loop is very inefficient because it causes a lot of useless calls.
  If you are using an UART directly located on the micro-processor
  this  is not too bad as the response time is quick. However if the
  UART is located remotely and communicating through an I²C bus then
  it becomes very problematic because we will need a lot of transactions
  on the bus before getting just one byte.
    - In the optimum code, we ask how many bytes are available and then
    we read them all in one shot. This alone could improve the speed
    by a factor of more than six.

  So what can be done to make the remote device works efficiently
  without asking the WK2132's client to rewrite its code?
  I have been testing a lot of different strategies measuring
  timings and I finally came up with solutions for the transmission
  problem as well as for the reception problem:
    - For transmission if there is a flush() immediately after the
    write_array() what I do is to defer the blocking until the
    next write_array(). As I said that gives usually plenty of time,
    so flush() should almost never block.
    - For reception what I do is to buffer the received bytes into
    a ring buffer. Therefore even if the client is reading bytes
    one by one as I have the bytes locally I do not need to do
    transactions on the bus for each byte.

 @subsection _x_ The WK2132Channel methods
  - bool WK2132Channel::flush()
  - void WK2132Channel::write_array(const uint8_t *buffer, size_t len);
  - int WK2132Channel::available()
  - bool WK2132Channel::read_array(uint8_t *buffer, size_t len);
  - bool WK2132Channel::peek_byte(uint8_t *buffer);

*/

static const char *const TAG = "wk2132";

static const char *const REG_TO_STR_P0[] = {"GENA", "GRST", "GMUT",  "SPAGE", "SCR", "LCR", "FCR",
                                            "SIER", "SIFR", "TFCNT", "RFCNT", "FSR", "LSR", "FDAT"};
static const char *const REG_TO_STR_P1[] = {"GENA", "GRST", "GMUT",  "SPAGE", "BAUD1", "BAUD0", "PRES",
                                            "RFTL", "TFTL", "_INV_", "_INV_", "_INV_", "_INV_"};

// convert an int to binary string
inline std::string i2s(uint8_t val) { return std::bitset<8>(val).to_string(); }
#define I2CS(val) (i2s(val).c_str())

/// @brief Computes the I²C Address to access the component
/// @param base_address the base address of the component as set by the A1 A0 pins
/// @param channel (0-3) the UART channel
/// @param fifo (0-1) if 0 access to internal register, if 1 direct access to fifo
/// @return the i2c address to use
inline uint8_t i2c_address(uint8_t base_address, uint8_t channel, uint8_t fifo) {
  // the address of the device is:
  // +----+----+----+----+----+----+----+----+
  // |  0 | A1 | A0 |  1 |  0 | C1 | C0 |  F |
  // +----+----+----+----+----+----+----+----+
  // where:
  // - A1,A0 is the address read from A1,A0 switch
  // - C1,C0 is the channel number (in practice only 00 or 01)
  // - F is: 0 when accessing register, one when accessing FIFO
  uint8_t const addr = base_address | channel << 1 | fifo;
  return addr;
}

/// @brief Converts the parity enum to a string
/// @param parity enum
/// @return the string
const char *parity2string(uart::UARTParityOptions parity) {
  using namespace uart;
  switch (parity) {
    case UART_CONFIG_PARITY_NONE:
      return "NONE";
    case UART_CONFIG_PARITY_EVEN:
      return "EVEN";
    case UART_CONFIG_PARITY_ODD:
      return "ODD";
    default:
      return "UNKNOWN";
  }
}

///////////////////////////////////////////////////////////////////////////////
// The WK2132Component methods
///////////////////////////////////////////////////////////////////////////////

// method to print registers as text: used in log messages ...
const char *WK2132Component::reg_to_str_(int val) { return this->page1_ ? REG_TO_STR_P1[val] : REG_TO_STR_P0[val]; }

void WK2132Component::write_wk2132_register_(uint8_t reg_number, uint8_t channel, const uint8_t *buffer, size_t len) {
  this->address_ = i2c_address(this->base_address_, channel, 0);  // update the I²C address
  auto error = this->write_register(reg_number, buffer, len);     // write to the register
  if (error == i2c::ERROR_OK) {
    this->status_clear_warning();
    ESP_LOGVV(TAG, "write_wk2132_register_(@%02X %s, ch=%d b=%02X, len=%d): I2C code %d", this->address_,
              this->reg_to_str_(reg_number), channel, *buffer, len, (int) error);
  } else {  // error
    this->status_set_warning();
    ESP_LOGE(TAG, "write_wk2132_register_(@%02X %s, ch=%d b=%02X, len=%d): I2C code %d", this->address_,
             this->reg_to_str_(reg_number), channel, *buffer, len, (int) error);
  }
}

uint8_t WK2132Component::read_wk2132_register_(uint8_t reg_number, uint8_t channel, uint8_t *buffer, size_t len) {
  this->address_ = i2c_address(this->base_address_, channel, 0);  // update the i2c address
  auto error = this->read_register(reg_number, buffer, len);
  if (error == i2c::ERROR_OK) {
    this->status_clear_warning();
    ESP_LOGVV(TAG, "read_wk2132_register_(@%02X %s, ch=%d b=%02X, len=%d): I2C code %d", this->address_,
              this->reg_to_str_(reg_number), channel, *buffer, len, (int) error);
  } else {  // error
    this->status_set_warning();
    ESP_LOGE(TAG, "read_wk2132_register_(@%02X %s, ch=%d b=%02X, len=%d): I2C code %d", this->address_,
             this->reg_to_str_(reg_number), channel, *buffer, len, (int) error);
  }
  return *buffer;
}

//
// overloaded methods from Component
//

void WK2132Component::setup() {
  this->base_address_ = this->address_;  // TODO should not be necessary done in the ctor
  ESP_LOGCONFIG(TAG, "Setting up WK2132:@%02X with %d UARTs...", this->get_base_addr_(), (int) this->children_.size());
  // we setup our children
  for (auto *child : this->children_)
    child->setup_channel_();
}

void WK2132Component::dump_config() {
  ESP_LOGCONFIG(TAG, "Initialization of configuration WK2132:@%02X with %d UARTs completed", this->get_base_addr_(),
                (int) this->children_.size());
  ESP_LOGCONFIG(TAG, "  crystal %d", this->crystal_);
  int tm = this->test_mode_.to_ulong();
  ESP_LOGCONFIG(TAG, "  test_mode %d", tm);
  LOG_I2C_DEVICE(this);

  for (auto i = 0; i < this->children_.size(); i++) {
    ESP_LOGCONFIG(TAG, "  UART @%02X:%d...", this->get_base_addr_(), i);
    ESP_LOGCONFIG(TAG, "    baudrate %d Bd", this->children_[i]->baud_rate_);
    ESP_LOGCONFIG(TAG, "    data_bits %d", this->children_[i]->data_bits_);
    ESP_LOGCONFIG(TAG, "    stop_bits %d", this->children_[i]->stop_bits_);
    ESP_LOGCONFIG(TAG, "    parity %s", parity2string(this->children_[i]->parity_));

    // here we reset the buffers and the fifos
    children_[i]->receive_buffer_.clear();
    children_[i]->flush_requested = false;
    children_[i]->reset_fifo_();
  }
  this->initialized_ = true;  // we can safely use the wk2132
}

///////////////////////////////////////////////////////////////////////////////
// The WK2132Channel methods
///////////////////////////////////////////////////////////////////////////////

void WK2132Channel::setup_channel_() {
  ESP_LOGCONFIG(TAG, "  Setting up UART @%02X:%d...", this->parent_->get_base_addr_(), this->channel_);

  //
  // we first do the global register (common to both channel)
  //
  uint8_t gena;
  this->parent_->read_wk2132_register_(REG_WK2132_GENA, 0, &gena, 1);
  (this->channel_ == 0) ? gena |= 0x01 : gena |= 0x02;  // enable channel 1 or 2
  this->parent_->write_wk2132_register_(REG_WK2132_GENA, 0, &gena, 1);
  // software reset UART channels
  uint8_t grst = 0;
  this->parent_->read_wk2132_register_(REG_WK2132_GRST, 0, &gena, 1);
  (this->channel_ == 0) ? grst |= 0x01 : grst |= 0x02;  // reset channel 1 or 2
  this->parent_->write_wk2132_register_(REG_WK2132_GRST, 0, &grst, 1);

  //
  // now we do the register linked to a specific channel
  //
  // initialize the spage register to page 0
  uint8_t const page = 0;
  this->parent_->page1_ = false;
  this->parent_->write_wk2132_register_(REG_WK2132_SPAGE, channel_, &page, 1);
  // we enable both channel
  uint8_t const scr = 0x3;  // 0000 0011 enable receive and transmit
  this->parent_->write_wk2132_register_(REG_WK2132_SCR, this->channel_, &scr, 1);

  this->set_baudrate_();
  this->set_line_param_();
  this->reset_fifo_();
}

void WK2132Channel::reset_fifo_() {
  // we reset and enable all fifo ... FCR => 0000 1111
  uint8_t const fcr = 0b00001111;
  this->parent_->write_wk2132_register_(REG_WK2132_FCR, this->channel_, &fcr, 1);
}

void WK2132Channel::set_baudrate_() {
  uint16_t const val_int = this->parent_->crystal_ / (this->baud_rate_ * 16) - 1;
  uint16_t val_dec = (this->parent_->crystal_ % (this->baud_rate_ * 16)) / (this->baud_rate_ * 16);
  uint8_t const baud_high = (uint8_t) (val_int >> 8);
  uint8_t const baud_low = (uint8_t) (val_int & 0xFF);
  while (val_dec > 0x0A)
    val_dec /= 0x0A;
  uint8_t const baud_dec = (uint8_t) (val_dec);

  uint8_t page = 1;  // switch to page 1
  this->parent_->write_wk2132_register_(REG_WK2132_SPAGE, this->channel_, &page, 1);
  this->parent_->page1_ = true;
  this->parent_->write_wk2132_register_(REG_WK2132_BRH, this->channel_, &baud_high, 1);
  this->parent_->write_wk2132_register_(REG_WK2132_BRL, this->channel_, &baud_low, 1);
  this->parent_->write_wk2132_register_(REG_WK2132_BRD, this->channel_, &baud_dec, 1);
  page = 0;  // switch back to page 0
  this->parent_->write_wk2132_register_(REG_WK2132_SPAGE, this->channel_, &page, 1);
  this->parent_->page1_ = false;

  ESP_LOGCONFIG(TAG, "  Crystal=%d baudrate=%d => registers [%d %d %d]", this->parent_->crystal_, this->baud_rate_,
                baud_high, baud_low, baud_dec);
}

void WK2132Channel::set_line_param_() {
  this->data_bits_ = 8;  // always 8 for WK2132
  uint8_t lcr;
  this->parent_->read_wk2132_register_(REG_WK2132_LCR, this->channel_, &lcr, 1);

  lcr &= 0xF0;  // Clear the lower 4 bit of LCR
  if (this->stop_bits_ == 2)
    lcr |= 0x01;  // 0001

  switch (this->parity_) {              // parity selection settings
    case uart::UART_CONFIG_PARITY_ODD:  // odd parity
      lcr |= 0x5 << 1;                  // 101x
      break;
    case uart::UART_CONFIG_PARITY_EVEN:  // even parity
      lcr |= 0x6 << 1;                   // 110x
      break;
    default:
      break;  // no parity 000x
  }
  this->parent_->write_wk2132_register_(REG_WK2132_LCR, this->channel_, &lcr, 1);
  ESP_LOGCONFIG(TAG, "  line config: %d data_bits, %d stop_bits, parity %s register [%s]", this->data_bits_,
                this->stop_bits_, parity2string(this->parity_), I2CS(lcr));
}

inline bool WK2132Channel::tx_fifo_is_not_empty_() {
  return this->parent_->read_wk2132_register_(REG_WK2132_FSR, this->channel_, &this->data_, 1) & 0x4;
}

size_t WK2132Channel::tx_in_fifo_() {
  size_t tfcnt = this->parent_->read_wk2132_register_(REG_WK2132_TFCNT, this->channel_, &this->data_, 1);
  uint8_t const fsr = this->parent_->read_wk2132_register_(REG_WK2132_FSR, this->channel_, &this->data_, 1);
  if ((fsr & 0x02) && (tfcnt == 0)) {
    ESP_LOGVV(TAG, "tx_in_fifo overflow FSR=%s", I2CS(fsr));
    tfcnt = FIFO_SIZE;
  }
  return tfcnt;
}

/// @brief number of bytes in the receive fifo
/// @return number of bytes
/// @image html write_cycles.png
size_t WK2132Channel::rx_in_fifo_() {
  size_t available = 0;

  available = this->parent_->read_wk2132_register_(REG_WK2132_RFCNT, this->channel_, &this->data_, 1);
  if (available == 0) {
    uint8_t const fsr = this->parent_->read_wk2132_register_(REG_WK2132_FSR, this->channel_, &this->data_, 1);
    if (fsr & 0x8) {  // if RDAT bit is set we set available to 256
      ESP_LOGVV(TAG, "rx_in_fifo overflow FSR=%s", I2CS(fsr));
      available = FIFO_SIZE;
    }
  }

  if (available > FIFO_SIZE)  // no more than what is set in the fifo_size
    available = FIFO_SIZE;

  ESP_LOGVV(TAG, "rx_in_fifo %d", available);
  return available;
}

bool WK2132Channel::read_data_(uint8_t *buffer, size_t len) {
  this->parent_->address_ = i2c_address(this->parent_->base_address_, this->channel_, 1);  // fifo flag is set

  // With the WK2132 we need to read data directly from the fifo buffer without passing through a register
  // note: that theoretically it should be possible to read through the REG_WK2132_FDA register
  // but beware that it does not work !
  auto error = this->parent_->read(buffer, len);
  if (error == i2c::ERROR_OK) {
    this->parent_->status_clear_warning();
    ESP_LOGV(TAG, "read_data(ch=%d buffer[0]=%02X, len=%d): I2C code %d", this->channel_, *buffer, len, (int) error);
    return true;
  } else {  // error found
    this->parent_->status_set_warning();
    ESP_LOGE(TAG, "read_data(ch=%d buffer[0]=%02X, len=%d): I2C code %d", this->channel_, *buffer, len, (int) error);
    return false;
  }
}

bool WK2132Channel::write_data_(const uint8_t *buffer, size_t len) {
  this->parent_->address_ = i2c_address(this->parent_->base_address_, this->channel_, 1);  // set fifo flag

  // With the WK2132 we need to write to the fifo buffer without passing through a register
  // note: that theoretically it should be possible to write through the REG_WK2132_FDA register
  // but beware that it does not seems to work !
  auto error = this->parent_->write(buffer, len);
  if (error == i2c::ERROR_OK) {
    this->parent_->status_clear_warning();
    ESP_LOGV(TAG, "write_data(ch=%d buffer[0]=%02X, len=%d): I2C code %d", this->channel_, *buffer, len, (int) error);
    return true;
  } else {  // error
    this->parent_->status_set_warning();
    ESP_LOGE(TAG, "write_data(ch=%d buffer[0]=%02X, len=%d): I2C code %d", this->channel_, *buffer, len, (int) error);
    return false;
  }
}

int WK2132Channel::available() {
  auto available = this->receive_buffer_.count();

  // here if we do not have bytes in buffer we want to check if
  // there are bytes in the fifo,in which case we do not want to
  // delay reading them in the next loop.
  if (!available)
    available = rx_fifo_to_buffer_();

  return available;
}

bool WK2132Channel::read_array(uint8_t *buffer, size_t len) {
  bool status = true;
  auto available = this->receive_buffer_.count();

  // here if we do not have bytes in buffer we want to check if
  // there are bytes in the fifo,in which case we do not want to
  // delay reading them in the next loop.
  if (!available)
    available = rx_fifo_to_buffer_();

  if ((len > FIFO_SIZE) || (len > available)) {
    // ESP_LOGE(TAG, "read_array buffer underflow requested %d bytes available %d ...", len, available);
    len = available;
    status = false;
  }
  // retrieve the bytes from ring buffer
  for (size_t i = 0; i < len; i++) {
    this->receive_buffer_.pop(buffer[i]);
  }
  return status;
}

void WK2132Channel::write_array(const uint8_t *buffer, size_t len) {
  if (len > FIFO_SIZE) {
    ESP_LOGE(TAG, "Write buffer invalid call: requested %d bytes max size %d ...", len, FIFO_SIZE);
    len = FIFO_SIZE;
  }

  // if we had a flush request it is time to check it has been honored
  if (this->flush_requested) {
    if (this->tx_fifo_is_not_empty_()) {
      // despite the fact that we have waited a max the fifo is still not empty
      // therefore we now have to wait until it gets empty
      uint32_t const start_time = millis();
      while (this->tx_fifo_is_not_empty_()) {  // wait until buffer empty
        if (millis() - start_time > 100) {
          ESP_LOGE(TAG, "Flush timed out: still %d bytes not sent...", this->tx_in_fifo_());
          return;
        }
        yield();  // reschedule our thread to avoid blocking
      }
    } else
      this->flush_requested = false;  // we are all set
  }

  this->write_data_(buffer, len);
}

void WK2132Channel::flush() {
  // here we just record the request but we do not wait at this time.
  // Tje next write_array() call will first check that everything
  // is gone otherwise it will wait. This gives time for the bytes
  // to go
  this->flush_requested = true;
}

size_t WK2132Channel::rx_fifo_to_buffer_() {
  // we look if some characters has been received in the fifo
  auto to_transfer = this->rx_in_fifo_();
  if (to_transfer) {
    uint8_t data[to_transfer]{0};
    this->read_data_(data, to_transfer);
    auto free = this->receive_buffer_.free();
    if (to_transfer > free) {
      ESP_LOGV(TAG, "Ring buffer overrun --> requested %d available %d", to_transfer, free);
      to_transfer = free;  // hopefully will do the rest next time
    }
    ESP_LOGV(TAG, "Transferred %d bytes from rx_fifo to buffer ring", to_transfer);
    for (size_t i = 0; i < to_transfer; i++)
      this->receive_buffer_.push(data[i]);
  }
  return to_transfer;
}

///////////////////////////////////////////////////////////////////////////////
// AUTOTEST FUNCTIONS BELOW
///////////////////////////////////////////////////////////////////////////////
#ifdef AUTOTEST_COMPONENT

/// @brief A Functor class that return incremented numbers
class Increment {  // A "Functor" (A class object that acts like a method with state!)
 public:
  Increment() : i_(0) {}
  uint8_t operator()() { return i_++; }

 private:
  uint8_t i_;
};

void print_buffer(std::vector<uint8_t> buffer) {
  // quick and ugly hex converter to display buffer in hex format
  char hex_buffer[80];
  hex_buffer[50] = 0;
  for (size_t i = 0; i < buffer.size(); i++) {
    snprintf(&hex_buffer[3 * (i % 16)], sizeof(hex_buffer), "%02X ", buffer[i]);
    if (i % 16 == 15)
      ESP_LOGI(TAG, "   %s", hex_buffer);
  }
  if (buffer.size() % 16) {
    // null terminate if incomplete line
    hex_buffer[3 * (buffer.size() % 16) + 2] = 0;
    ESP_LOGI(TAG, "   %s", hex_buffer);
  }
}

/// @brief test the write_array method
void WK2132Channel::uart_send_test_(char *preamble) {
  auto start_exec = micros();
  // we send the maximum possible
  this->flush();
  size_t const to_send = FIFO_SIZE;
  if (to_send > 0) {
    std::vector<uint8_t> output_buffer(to_send);
    generate(output_buffer.begin(), output_buffer.end(), Increment());  // fill with incrementing number
    output_buffer[0] = to_send;                     // we send as the first byte the length of the buffer
    this->write_array(&output_buffer[0], to_send);  // we send the buffer
    this->flush();                                  // we wait until they are gone
    ESP_LOGI(TAG, "%s => sending  %d bytes - exec time %d µs ...", preamble, to_send, micros() - start_exec);
  }
}

/// @brief test the read_array method
void WK2132Channel::uart_receive_test_(char *preamble, bool print_buf) {
  auto start_exec = micros();
  uint8_t const to_read = this->available();
  if (to_read > 0) {
    std::vector<uint8_t> buffer(to_read);
    this->read_array(&buffer[0], to_read);
    if (print_buf)
      print_buffer(buffer);
  }
  ESP_LOGI(TAG, "%s => received %d bytes - exec time %d µs ...", preamble, to_read, micros() - start_exec);
}
#endif

//
// This is the WK2132Component loop() method
//
void WK2132Component::loop() {
  static uint16_t loop_calls = 0;
  static uint32_t loop_time = 0;
  uint32_t time;
  auto elapsed = [](uint32_t &t) {
    auto e = micros() - t;
    t = micros();
    return e;
  };
  if (this->test_mode_.any())
    ESP_LOGI(TAG, "loop %d : %d ms since last call ...", loop_calls++, millis() - loop_time);
  loop_time = millis();

  // if the component is not fully initialized we clear and return
  if (!this->initialized_) {
    for (auto *child : this->children_) {
      child->receive_buffer_.clear();
    }
    return;
  }

  // here we transfer bytes from fifo to ring buffers
  elapsed(time);
  for (auto *child : this->children_) {
    // we look if some characters has been received in the fifo
    child->rx_fifo_to_buffer_();
  }

  if (this->test_mode_.any())
    ESP_LOGI(TAG, "transfer all fifo to ring execution time %d µs...", elapsed(time));

#ifdef AUTOTEST_COMPONENT
  //
  // This loop is used only if the wk2132 component is in test mode
  //

  if (this->test_mode_.test(0)) {  // test echo mode (bit 0)
    elapsed(time);
    for (auto *child : this->children_) {
      uint8_t data;
      if (child->available()) {
        child->read_byte(&data);
        ESP_LOGI(TAG, "echo mode: read -> send %02X", data);
        child->write_byte(data);
      }
    }
    ESP_LOGI(TAG, "echo execution time %d µs...", elapsed(time));
  }

  if (test_mode_.test(1)) {  // test loop mode (bit 1)
    char preamble[64];
    for (size_t i = 0; i < this->children_.size(); i++) {
      snprintf(preamble, sizeof(preamble), "WK2132@%02X:%d", this->get_base_addr_(), i);
      elapsed(time);
      this->children_[i]->uart_receive_test_(preamble, true);
      ESP_LOGI(TAG, "receive execution time %d µs...", elapsed(time));
      this->children_[i]->uart_send_test_(preamble);
      ESP_LOGI(TAG, "transmit execution time %d µs...", elapsed(time));
    }
  }
  if (this->test_mode_.any())
    ESP_LOGI(TAG, "loop execution time %d ms...", millis() - loop_time);
#endif
}

}  // namespace wk2132
}  // namespace esphome
