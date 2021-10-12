#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/time/real_time_clock.h"

namespace esphome {
namespace ds3231 {

enum DS3231Alarm1Type {
  EverySecond = 0x0F,
  EverySecondWithInterupt = 0x4F,
  MatchSecond = 0x0E,
  MatchSecondWithInterupt = 0x4E,
  MatchMinuteSecond = 0x0C,
  MatchMinuteSecondWithInterupt = 0x4C,
  MatchHourMinuteSecond = 0x08,
  MatchHourMinuteSecondWithInterupt = 0x48,
  MatchDayOfMonthHourMinuteSecond = 0x00,
  MatchDayOfMonthHourMinuteSecondWithInterupt = 0x40,
  MatchDayOfWeekHourMinuteSecond = 0x10,
  MatchDayOfWeekHourMinuteSecondWithInterupt = 0x50,
};

enum DS3231Alarm2Type {
  EveryMinute = 0x0E,
  EveryMinuteWithInterupt = 0x4E,
  MatchMinute = 0x0C,
  MatchMinuteWithInterupt = 0x4C,
  MatchHourMinute = 0x08,
  MatchHourMinuteWithInterupt = 0x48,
  MatchDayOfMonthHourMinute = 0x00,
  MatchDayOfMonthHourMinuteWithInterupt = 0x40,
  MatchDayOfWeekHourMinute = 0x10,
  MatchDayOfWeekHourMinuteWithInterupt = 0x50,
};

enum DS3231SquareWaveMode {
  SquareWave,
  Interupt,
};

enum DS3231SquareWaveFrequency {
  Frequency1Hz = 0x00,
  Frequency1024Hz = 0x01,
  Frequency4096Hz = 0x02,
  Frequency8192Hz = 0x03,
};

class DS3231RTC;

class DS3231Component : public PollingComponent, public i2c::I2CDevice {
 public:
  float get_setup_priority() const override { return setup_priority::DATA; };
  void setup() override;
  void dump_config() override;
  void update() override { this->read_status_(); }
  void set_alarm_1(DS3231Alarm1Type alarm_type, uint8_t second, uint8_t minute, uint8_t hour, uint8_t day);
  void reset_alarm_1();
  void set_alarm_2(DS3231Alarm2Type alarm_type, uint8_t minute, uint8_t hour, uint8_t day);
  void reset_alarm_2();
  void set_square_wave_mode(DS3231SquareWaveMode mode);
  void set_square_wave_frequency(DS3231SquareWaveFrequency frequency);

 protected:
  friend DS3231RTC;
  bool read_rtc_();
  bool write_rtc_();
  bool read_alarm_();
  bool write_alarm_();
  bool read_control_();
  bool write_control_();
  bool read_status_();
  bool write_status_();
  struct DS3231Reg {
    union DS3231RTC {
      struct {
        uint8_t second : 4;
        uint8_t second_10 : 3;
        uint8_t unused_1 : 1;

        uint8_t minute : 4;
        uint8_t minute_10 : 3;
        uint8_t unused_2 : 1;

        uint8_t hour : 4;
        uint8_t hour_10 : 2;
        uint8_t unused_3 : 2;

        uint8_t weekday : 3;
        uint8_t unused_4 : 5;

        uint8_t day : 4;
        uint8_t day_10 : 2;
        uint8_t unused_5 : 2;

        uint8_t month : 4;
        uint8_t month_10 : 1;
        uint8_t unused_6 : 2;
        uint8_t cent : 1;

        uint8_t year : 4;
        uint8_t year_10 : 4;
      } reg;
      mutable uint8_t raw[sizeof(reg)];
    } rtc;
    union DS3231Alrm {
      struct {
        uint8_t a1_second : 4;
        uint8_t a1_second_10 : 3;
        bool a1_m1 : 1;

        uint8_t a1_minute : 4;
        uint8_t a1_minute_10 : 3;
        bool a1_m2 : 1;

        uint8_t a1_hour : 4;
        uint8_t a1_hour_10 : 2;
        uint8_t unused_1 : 1;
        bool a1_m3 : 1;

        uint8_t a1_day : 4;
        uint8_t a1_day_10 : 2;
        uint8_t a1_day_mode : 1;
        bool a1_m4 : 1;

        uint8_t a2_minute : 4;
        uint8_t a2_minute_10 : 3;
        bool a2_m2 : 1;

        uint8_t a2_hour : 4;
        uint8_t a2_hour_10 : 2;
        uint8_t unused_2 : 1;
        bool a2_m3 : 1;

        uint8_t a2_day : 4;
        uint8_t a2_day_10 : 2;
        uint8_t a2_day_mode : 1;
        bool a2_m4 : 1;
      } reg;
      mutable uint8_t raw[sizeof(reg)];
    } alrm;
    union DS3231Ctrl {
      struct {
        bool alrm_1_int : 1;
        bool alrm_2_int : 1;
        bool int_ctrl : 1;
        uint8_t rs : 2;
        bool conv_tmp : 1;
        bool bat_sqw : 1;
        bool osc_dis : 1;
      } reg;
      mutable uint8_t raw[sizeof(reg)];
    } ctrl;
    union DS3231Stat {
      struct {
        bool alrm_1_act : 1;
        bool alrm_2_act : 1;
        bool busy : 1;
        bool en32khz : 1;
        uint8_t unused : 3;
        bool osc_stop : 1;
      } reg;
    mutable uint8_t raw[sizeof(reg)];
    } stat;
  } ds3231_;
};

class DS3231RTC : public time::RealTimeClock {
 public:
  void set_ds3231_parent(DS3231Component *parent) { this->parent_ = parent; }
  float get_setup_priority() const override { return setup_priority::DATA; };
  void dump_config() override;
  void update() override { this->read_time(); }
  void read_time();
  void write_time();

 protected:
  DS3231Component *parent_;
};

template<typename... Ts> class WriteTimeAction : public Action<Ts...>, public Parented<DS3231RTC> {
 public:
  void play(Ts... x) override { this->parent_->write_time(); }
};

template<typename... Ts> class ReadTimeAction : public Action<Ts...>, public Parented<DS3231RTC> {
 public:
  void play(Ts... x) override { this->parent_->read_time(); }
};

}  // namespace ds3231
}  // namespace esphome
