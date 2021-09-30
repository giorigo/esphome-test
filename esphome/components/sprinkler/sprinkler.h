#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace sprinkler {

enum SprinklerTimerIndex : size_t {
  TIMER_VALVE_OPEN_DELAY = 0,
  TIMER_VALVE_OVERLAP_DELAY = 1,
  TIMER_VALVE_RUN_DURATION = 2,
};

class SprinklerSwitch;
template<typename... Ts> class StartSingleValveAction;
template<typename... Ts> class ShutdownAction;
template<typename... Ts> class ResumeOrStartAction;

struct SprinklerTimer {
  const std::string name;
  bool active;
  uint32_t time;
  uint32_t start_time;
  std::function<void()> func;
};

struct SprinklerValve {
  std::unique_ptr<SprinklerSwitch> controller_switch;
  std::unique_ptr<SprinklerSwitch> enable_switch;
  switch_::Switch *pump_switch;
  switch_::Switch *valve_switch;
  uint32_t valve_run_duration;
  bool valve_cycle_complete;
  std::unique_ptr<ShutdownAction<>> valve_shutdown_action;
  std::unique_ptr<StartSingleValveAction<>> valve_resumeorstart_action;
  std::unique_ptr<Automation<>> valve_turn_off_automation;
  std::unique_ptr<Automation<>> valve_turn_on_automation;
};

class SprinklerSwitch : public switch_::Switch, public Component {
 public:
  SprinklerSwitch();

  void setup() override;
  void dump_config() override;

  void set_state_lambda(std::function<optional<bool>()> &&f);
  void set_restore_state(bool restore_state);
  Trigger<> *get_turn_on_trigger() const;
  Trigger<> *get_turn_off_trigger() const;
  void set_optimistic(bool optimistic);
  void set_assumed_state(bool assumed_state);
  void loop() override;

  float get_setup_priority() const override;

 protected:
  bool assumed_state() override;

  void write_state(bool state) override;

  optional<std::function<optional<bool>()>> f_;
  bool optimistic_{false};
  bool assumed_state_{false};
  Trigger<> *turn_on_trigger_;
  Trigger<> *turn_off_trigger_;
  Trigger<> *prev_trigger_{nullptr};
  bool restore_state_{false};
};

class Sprinkler : public Component {
 public:
  Sprinkler();
  void pre_setup(const std::string &name, const std::string &auto_adv_name, const std::string &reverse_name = "");
  void setup() override;
  void dump_config() override;

  /// add a valve to the controller
  void add_valve(const std::string &valve_sw_name, const std::string &enable_sw_name = "");

  /// add another controller to the controller so it can check if pumps/main valves are in use
  void add_controller(Sprinkler *other_controller);

  /// configure a valve's switch object, run duration and pump switch (if provided).
  ///  valve_run_duration is time in seconds.
  void configure_valve_switch(size_t valve_number, switch_::Switch *valve_switch, uint32_t valve_run_duration,
                              switch_::Switch *pump_switch = nullptr);

  /// value multiplied by configured run times -- used to extend or shorten the cycle
  void set_multiplier(optional<float> multiplier);

  /// how long the controller should wait to open/switch on the valve after it becomes active
  void set_valve_open_delay(uint32_t valve_open_delay);

  /// how long the controller should wait after opening a valve before closing the previous valve
  void set_valve_overlap(uint32_t valve_overlap);

  /// how long the valve should remain on/open (time in seconds)
  void set_valve_run_duration(optional<size_t> valve_number, optional<uint32_t> valve_run_duration);

  /// if auto_advance is true, controller will iterate through all enabled valves
  void set_auto_advance(bool auto_advance);

  /// if reverse is true, controller will iterate through all enabled valves in reverse (descending) order
  void set_reverse(bool reverse);

  /// returns valve_number's run duration. note that these times are stored in seconds
  uint32_t valve_run_duration(size_t valve_number);

  /// returns valve_number's run duration adjusted by multiplier_. note that these times are stored in seconds
  uint32_t valve_run_duration_adjusted(size_t valve_number);

  /// returns true if auto_advance is enabled
  bool auto_advance();

  /// returns true if reverse is enabled
  bool reverse();

  /// starts a full cycle of all enabled valves and enables auto_advance.
  /// if no valves are enabled, all valves will be enabled.
  void start_full_cycle();

  /// activates a single valve and disables auto_advance.
  void start_single_valve(optional<size_t> valve_number);

  /// advances to the next valve (numerically)
  void next_valve();

  /// advances to the previous valve (numerically)
  void previous_valve();

  /// turns off all valves, effectively shutting down the system.
  void shutdown();

  /// same as shutdown(), but also stores active_valve() and time_remaining() allowing resume() to continue the cycle
  void pause();

  /// resumes a cycle that was suspended using pause()
  void resume();

  /// if a cycle was suspended using pause(), resumes it. otherwise calls start_full_cycle()
  void resume_or_start_full_cycle();

  /// returns a pointer to a valve's name string object; returns nullptr if valve_number is invalid
  const char *valve_name(size_t valve_number);

  /// returns the number of the valve that is currently active, if any. check with 'has_value()'
  optional<uint8_t> active_valve();

  /// returns the number of the valve that is paused, if any. check with 'has_value()'
  optional<uint8_t> paused_valve();

  /// returns true if valve number is valid
  bool is_a_valid_valve(size_t valve_number);

  /// returns the number of valves the controller is configured with
  size_t number_of_valves();

  /// returns true if the pump the pointer points to is in use
  bool pump_in_use(switch_::Switch *pump_switch);

  /// returns the amount of time remaining in seconds for the active valve, if any. check with 'has_value()'
  optional<uint32_t> time_remaining();

  /// returns a pointer to a valve's control switch object
  SprinklerSwitch *control_switch(size_t valve_number);

  /// returns a pointer to a valve's enable switch object
  SprinklerSwitch *enable_switch(size_t valve_number);

 protected:
  /// returns true if valve number is enabled
  bool valve_is_enabled_(size_t valve_number);

  /// marks a valve's cycle as complete
  void mark_valve_cycle_complete_(size_t valve_number);

  /// returns true if valve's cycle is flagged as complete
  bool valve_cycle_complete_(size_t valve_number);

  /// returns a pointer to a valve's switch object
  switch_::Switch *valve_switch_(size_t valve_number);

  /// returns a pointer to a valve's pump switch object
  switch_::Switch *valve_pump_switch_(size_t valve_number);

  /// returns the number of the next/previous valve in the vector
  uint8_t next_valve_number_(uint8_t first_valve);
  uint8_t previous_valve_number_(uint8_t first_valve);

  /// returns the number of the next valve that should be activated in a full cycle.
  ///  if no valve is next (cycle is complete), returns no value (check with 'has_value()')
  optional<uint8_t> next_valve_number_in_cycle_(optional<uint8_t> first_valve);

  /// returns the number of the next/previous valve that should be activated.
  ///  if no valve is next (cycle is complete), returns no value (check with 'has_value()')
  optional<uint8_t> next_enabled_incomplete_valve_number_(optional<uint8_t> first_valve);
  optional<uint8_t> previous_enabled_incomplete_valve_number_(optional<uint8_t> first_valve);

  /// returns true if any valve is enabled
  bool any_valve_is_enabled_();

  /// if available, turns on the pump if pump_state is true, otherwise turns off the pump
  void set_pump_state_(size_t valve_number, bool pump_state);

  /// starts a valve with related timers, etc.
  ///  run_duration = 0 means we'll look up how long the valve is configured to run (with multiplier)
  void start_valve_(optional<size_t> valve_number, optional<uint32_t> run_duration = nullopt);

  /// switches to the specified valve's pump/valve
  void switch_to_pump_(size_t valve_number, bool no_shutdown = false);
  void switch_to_valve_(size_t valve_number, bool no_shutdown = false);

  /// turns off/closes all valves, including pump if include_pump is true
  void all_valves_off_(bool include_pump = false);

  /// resets the cycle state for all valves
  void reset_cycle_states_();

  /// resets resume state
  void reset_resume_();

  /// Start/cancel/get status of valve timers
  void start_timer_(SprinklerTimerIndex timer_index);
  bool cancel_timer_(SprinklerTimerIndex timer_index);
  /// returns true if the specified timer is active/running
  bool timer_active_(SprinklerTimerIndex timer_index);
  /// time is converted to milliseconds (ms) for set_timeout()
  void set_timer_duration_(SprinklerTimerIndex timer_index, uint32_t time);
  /// returns time in milliseconds (ms)
  uint32_t timer_duration_(SprinklerTimerIndex timer_index);
  std::function<void()> timer_cbf_(SprinklerTimerIndex timer_index);

  /// callback functions for timers
  void valve_open_delay_callback_();
  void valve_overlap_delay_callback_();
  void valve_cycle_complete_callback_();

  /// The name of the sprinkler controller component
  std::string name_{""};

  /// The number of the valve that is currently active
  optional<uint8_t> active_valve_;

  /// The number of the valve to resume from (if paused)
  optional<uint8_t> paused_valve_;

  /// Set from time_remaining() when paused
  optional<uint32_t> resume_duration_;

  /// Sprinkler valve run time multiplier value
  float multiplier_{1.0};

  /// Sprinkler valve objects
  std::vector<SprinklerValve> valve_;

  /// Valve control timers
  std::vector<SprinklerTimer> timer_{
      {this->name_ + "open_delay", false, 0, 0, std::bind(&Sprinkler::valve_open_delay_callback_, this)},
      {this->name_ + "overlap_delay", false, 0, 0, std::bind(&Sprinkler::valve_overlap_delay_callback_, this)},
      {this->name_ + "cycle_complete", false, 0, 0, std::bind(&Sprinkler::valve_cycle_complete_callback_, this)}};

  /// Other Sprinkler instances we should be aware of (used to check if pumps are in use)
  std::vector<Sprinkler *> other_controllers_;

  /// Switches we'll present to the front end
  SprinklerSwitch auto_adv_sw_;
  SprinklerSwitch controller_sw_;
  SprinklerSwitch reverse_sw_;

  std::unique_ptr<ShutdownAction<>> sprinkler_shutdown_action_;
  std::unique_ptr<ResumeOrStartAction<>> sprinkler_resumeorstart_action_;

  std::unique_ptr<Automation<>> sprinkler_turn_off_automation_;
  std::unique_ptr<Automation<>> sprinkler_turn_on_automation_;
};

}  // namespace sprinkler
}  // namespace esphome
