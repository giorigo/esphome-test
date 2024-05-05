#pragma once

#include "esphome/core/defines.h"

#ifdef USE_MQTT

namespace esphome {
namespace mqtt {

#ifdef USE_MQTT_ABBREVIATIONS

constexpr const char *const MQTT_ACTION_TEMPLATE = "act_tpl";
constexpr const char *const MQTT_ACTION_TOPIC = "act_t";
constexpr const char *const MQTT_AUTOMATION_TYPE = "atype";
constexpr const char *const MQTT_AUX_COMMAND_TOPIC = "aux_cmd_t";
constexpr const char *const MQTT_AUX_STATE_TEMPLATE = "aux_stat_tpl";
constexpr const char *const MQTT_AUX_STATE_TOPIC = "aux_stat_t";
constexpr const char *const MQTT_AVAILABILITY = "avty";
constexpr const char *const MQTT_AVAILABILITY_MODE = "avty_mode";
constexpr const char *const MQTT_AVAILABILITY_TOPIC = "avty_t";
constexpr const char *const MQTT_AWAY_MODE_COMMAND_TOPIC = "away_mode_cmd_t";
constexpr const char *const MQTT_AWAY_MODE_STATE_TEMPLATE = "away_mode_stat_tpl";
constexpr const char *const MQTT_AWAY_MODE_STATE_TOPIC = "away_mode_stat_t";
constexpr const char *const MQTT_BATTERY_LEVEL_TEMPLATE = "bat_lev_tpl";
constexpr const char *const MQTT_BATTERY_LEVEL_TOPIC = "bat_lev_t";
constexpr const char *const MQTT_BLUE_TEMPLATE = "b_tpl";
constexpr const char *const MQTT_BRIGHTNESS_COMMAND_TOPIC = "bri_cmd_t";
constexpr const char *const MQTT_BRIGHTNESS_SCALE = "bri_scl";
constexpr const char *const MQTT_BRIGHTNESS_STATE_TOPIC = "bri_stat_t";
constexpr const char *const MQTT_BRIGHTNESS_TEMPLATE = "bri_tpl";
constexpr const char *const MQTT_BRIGHTNESS_VALUE_TEMPLATE = "bri_val_tpl";
constexpr const char *const MQTT_CHARGING_TEMPLATE = "chrg_tpl";
constexpr const char *const MQTT_CHARGING_TOPIC = "chrg_t";
constexpr const char *const MQTT_CLEANING_TEMPLATE = "cln_tpl";
constexpr const char *const MQTT_CLEANING_TOPIC = "cln_t";
constexpr const char *const MQTT_CODE_ARM_REQUIRED = "cod_arm_req";
constexpr const char *const MQTT_CODE_DISARM_REQUIRED = "cod_dis_req";
constexpr const char *const MQTT_COLOR_MODE = "clrm";
constexpr const char *const MQTT_COLOR_MODE_STATE_TOPIC = "clrm_stat_t";
constexpr const char *const MQTT_COLOR_MODE_VALUE_TEMPLATE = "clrm_val_tpl";
constexpr const char *const MQTT_COLOR_TEMP_COMMAND_TEMPLATE = "clr_temp_cmd_tpl";
constexpr const char *const MQTT_COLOR_TEMP_COMMAND_TOPIC = "clr_temp_cmd_t";
constexpr const char *const MQTT_COLOR_TEMP_STATE_TOPIC = "clr_temp_stat_t";
constexpr const char *const MQTT_COLOR_TEMP_TEMPLATE = "clr_temp_tpl";
constexpr const char *const MQTT_COLOR_TEMP_VALUE_TEMPLATE = "clr_temp_val_tpl";
constexpr const char *const MQTT_COMMAND_OFF_TEMPLATE = "cmd_off_tpl";
constexpr const char *const MQTT_COMMAND_ON_TEMPLATE = "cmd_on_tpl";
constexpr const char *const MQTT_COMMAND_RETAIN = "ret";
constexpr const char *const MQTT_COMMAND_TEMPLATE = "cmd_tpl";
constexpr const char *const MQTT_COMMAND_TOPIC = "cmd_t";
constexpr const char *const MQTT_CONFIGURATION_URL = "cu";
constexpr const char *const MQTT_CURRENT_HUMIDITY_TEMPLATE = "curr_hum_tpl";
constexpr const char *const MQTT_CURRENT_HUMIDITY_TOPIC = "curr_hum_t";
constexpr const char *const MQTT_CURRENT_TEMPERATURE_TEMPLATE = "curr_temp_tpl";
constexpr const char *const MQTT_CURRENT_TEMPERATURE_TOPIC = "curr_temp_t";
constexpr const char *const MQTT_DEVICE = "dev";
constexpr const char *const MQTT_DEVICE_CLASS = "dev_cla";
constexpr const char *const MQTT_DEVICE_CONNECTIONS = "cns";
constexpr const char *const MQTT_DEVICE_IDENTIFIERS = "ids";
constexpr const char *const MQTT_DEVICE_MANUFACTURER = "mf";
constexpr const char *const MQTT_DEVICE_MODEL = "mdl";
constexpr const char *const MQTT_DEVICE_NAME = "name";
constexpr const char *const MQTT_DEVICE_SUGGESTED_AREA = "sa";
constexpr const char *const MQTT_DEVICE_SW_VERSION = "sw";
constexpr const char *const MQTT_DOCKED_TEMPLATE = "dock_tpl";
constexpr const char *const MQTT_DOCKED_TOPIC = "dock_t";
constexpr const char *const MQTT_EFFECT_COMMAND_TOPIC = "fx_cmd_t";
constexpr const char *const MQTT_EFFECT_LIST = "fx_list";
constexpr const char *const MQTT_EFFECT_STATE_TOPIC = "fx_stat_t";
constexpr const char *const MQTT_EFFECT_TEMPLATE = "fx_tpl";
constexpr const char *const MQTT_EFFECT_VALUE_TEMPLATE = "fx_val_tpl";
constexpr const char *const MQTT_ENABLED_BY_DEFAULT = "en";
constexpr const char *const MQTT_ENTITY_CATEGORY = "ent_cat";
constexpr const char *const MQTT_ERROR_TEMPLATE = "err_tpl";
constexpr const char *const MQTT_ERROR_TOPIC = "err_t";
constexpr const char *const MQTT_EVENT_TYPE = "event_type";
constexpr const char *const MQTT_EVENT_TYPES = "evt_typ";
constexpr const char *const MQTT_EXPIRE_AFTER = "exp_aft";
constexpr const char *const MQTT_FAN_MODE_COMMAND_TEMPLATE = "fan_mode_cmd_tpl";
constexpr const char *const MQTT_FAN_MODE_COMMAND_TOPIC = "fan_mode_cmd_t";
constexpr const char *const MQTT_FAN_MODE_STATE_TEMPLATE = "fan_mode_stat_tpl";
constexpr const char *const MQTT_FAN_MODE_STATE_TOPIC = "fan_mode_stat_t";
constexpr const char *const MQTT_FAN_SPEED_LIST = "fanspd_lst";
constexpr const char *const MQTT_FAN_SPEED_TEMPLATE = "fanspd_tpl";
constexpr const char *const MQTT_FAN_SPEED_TOPIC = "fanspd_t";
constexpr const char *const MQTT_FLASH_TIME_LONG = "flsh_tlng";
constexpr const char *const MQTT_FLASH_TIME_SHORT = "flsh_tsht";
constexpr const char *const MQTT_FORCE_UPDATE = "frc_upd";
constexpr const char *const MQTT_GREEN_TEMPLATE = "g_tpl";
constexpr const char *const MQTT_HOLD_COMMAND_TEMPLATE = "hold_cmd_tpl";
constexpr const char *const MQTT_HOLD_COMMAND_TOPIC = "hold_cmd_t";
constexpr const char *const MQTT_HOLD_STATE_TEMPLATE = "hold_stat_tpl";
constexpr const char *const MQTT_HOLD_STATE_TOPIC = "hold_stat_t";
constexpr const char *const MQTT_HS_COMMAND_TOPIC = "hs_cmd_t";
constexpr const char *const MQTT_HS_STATE_TOPIC = "hs_stat_t";
constexpr const char *const MQTT_HS_VALUE_TEMPLATE = "hs_val_tpl";
constexpr const char *const MQTT_HUMIDITY_UNIT = "hum_unit";
constexpr const char *const MQTT_ICON = "ic";
constexpr const char *const MQTT_INITIAL = "init";
constexpr const char *const MQTT_JSON_ATTRIBUTES = "json_attr";
constexpr const char *const MQTT_JSON_ATTRIBUTES_TEMPLATE = "json_attr_tpl";
constexpr const char *const MQTT_JSON_ATTRIBUTES_TOPIC = "json_attr_t";
constexpr const char *const MQTT_LAST_RESET_TOPIC = "lrst_t";
constexpr const char *const MQTT_LAST_RESET_VALUE_TEMPLATE = "lrst_val_tpl";
constexpr const char *const MQTT_MAX = "max";
constexpr const char *const MQTT_MAX_HUMIDITY = "max_hum";
constexpr const char *const MQTT_MAX_MIREDS = "max_mirs";
constexpr const char *const MQTT_MAX_TEMP = "max_temp";
constexpr const char *const MQTT_MIN = "min";
constexpr const char *const MQTT_MIN_HUMIDITY = "min_hum";
constexpr const char *const MQTT_MIN_MIREDS = "min_mirs";
constexpr const char *const MQTT_MIN_TEMP = "min_temp";
constexpr const char *const MQTT_MODE = "mode";
constexpr const char *const MQTT_MODE_COMMAND_TEMPLATE = "mode_cmd_tpl";
constexpr const char *const MQTT_MODE_COMMAND_TOPIC = "mode_cmd_t";
constexpr const char *const MQTT_MODE_STATE_TEMPLATE = "mode_stat_tpl";
constexpr const char *const MQTT_MODE_STATE_TOPIC = "mode_stat_t";
constexpr const char *const MQTT_MODES = "modes";
constexpr const char *const MQTT_NAME = "name";
constexpr const char *const MQTT_OBJECT_ID = "obj_id";
constexpr const char *const MQTT_OFF_DELAY = "off_dly";
constexpr const char *const MQTT_ON_COMMAND_TYPE = "on_cmd_type";
constexpr const char *const MQTT_OPTIMISTIC = "opt";
constexpr const char *const MQTT_OPTIONS = "ops";
constexpr const char *const MQTT_OSCILLATION_COMMAND_TEMPLATE = "osc_cmd_tpl";
constexpr const char *const MQTT_OSCILLATION_COMMAND_TOPIC = "osc_cmd_t";
constexpr const char *const MQTT_OSCILLATION_STATE_TOPIC = "osc_stat_t";
constexpr const char *const MQTT_OSCILLATION_VALUE_TEMPLATE = "osc_val_tpl";
constexpr const char *const MQTT_PAYLOAD = "pl";
constexpr const char *const MQTT_PAYLOAD_ARM_AWAY = "pl_arm_away";
constexpr const char *const MQTT_PAYLOAD_ARM_CUSTOM_BYPASS = "pl_arm_custom_b";
constexpr const char *const MQTT_PAYLOAD_ARM_HOME = "pl_arm_home";
constexpr const char *const MQTT_PAYLOAD_ARM_NIGHT = "pl_arm_nite";
constexpr const char *const MQTT_PAYLOAD_ARM_VACATION = "pl_arm_vacation";
constexpr const char *const MQTT_PAYLOAD_AVAILABLE = "pl_avail";
constexpr const char *const MQTT_PAYLOAD_CLEAN_SPOT = "pl_cln_sp";
constexpr const char *const MQTT_PAYLOAD_CLOSE = "pl_cls";
constexpr const char *const MQTT_PAYLOAD_DISARM = "pl_disarm";
constexpr const char *const MQTT_PAYLOAD_HIGH_SPEED = "pl_hi_spd";
constexpr const char *const MQTT_PAYLOAD_HOME = "pl_home";
constexpr const char *const MQTT_PAYLOAD_LOCATE = "pl_loc";
constexpr const char *const MQTT_PAYLOAD_LOCK = "pl_lock";
constexpr const char *const MQTT_PAYLOAD_LOW_SPEED = "pl_lo_spd";
constexpr const char *const MQTT_PAYLOAD_MEDIUM_SPEED = "pl_med_spd";
constexpr const char *const MQTT_PAYLOAD_NOT_AVAILABLE = "pl_not_avail";
constexpr const char *const MQTT_PAYLOAD_NOT_HOME = "pl_not_home";
constexpr const char *const MQTT_PAYLOAD_OFF = "pl_off";
constexpr const char *const MQTT_PAYLOAD_OFF_SPEED = "pl_off_spd";
constexpr const char *const MQTT_PAYLOAD_ON = "pl_on";
constexpr const char *const MQTT_PAYLOAD_OPEN = "pl_open";
constexpr const char *const MQTT_PAYLOAD_OSCILLATION_OFF = "pl_osc_off";
constexpr const char *const MQTT_PAYLOAD_OSCILLATION_ON = "pl_osc_on";
constexpr const char *const MQTT_PAYLOAD_PAUSE = "pl_paus";
constexpr const char *const MQTT_PAYLOAD_RESET = "pl_rst";
constexpr const char *const MQTT_PAYLOAD_RESET_HUMIDITY = "pl_rst_hum";
constexpr const char *const MQTT_PAYLOAD_RESET_MODE = "pl_rst_mode";
constexpr const char *const MQTT_PAYLOAD_RESET_PERCENTAGE = "pl_rst_pct";
constexpr const char *const MQTT_PAYLOAD_RESET_PRESET_MODE = "pl_rst_pr_mode";
constexpr const char *const MQTT_PAYLOAD_RETURN_TO_BASE = "pl_ret";
constexpr const char *const MQTT_PAYLOAD_START = "pl_strt";
constexpr const char *const MQTT_PAYLOAD_START_PAUSE = "pl_stpa";
constexpr const char *const MQTT_PAYLOAD_STOP = "pl_stop";
constexpr const char *const MQTT_PAYLOAD_TURN_OFF = "pl_toff";
constexpr const char *const MQTT_PAYLOAD_TURN_ON = "pl_ton";
constexpr const char *const MQTT_PAYLOAD_UNLOCK = "pl_unlk";
constexpr const char *const MQTT_PERCENTAGE_COMMAND_TEMPLATE = "pct_cmd_tpl";
constexpr const char *const MQTT_PERCENTAGE_COMMAND_TOPIC = "pct_cmd_t";
constexpr const char *const MQTT_PERCENTAGE_STATE_TOPIC = "pct_stat_t";
constexpr const char *const MQTT_PERCENTAGE_VALUE_TEMPLATE = "pct_val_tpl";
constexpr const char *const MQTT_POSITION_CLOSED = "pos_clsd";
constexpr const char *const MQTT_POSITION_OPEN = "pos_open";
constexpr const char *const MQTT_POSITION_TEMPLATE = "pos_tpl";
constexpr const char *const MQTT_POSITION_TOPIC = "pos_t";
constexpr const char *const MQTT_POWER_COMMAND_TOPIC = "pow_cmd_t";
constexpr const char *const MQTT_POWER_STATE_TEMPLATE = "pow_stat_tpl";
constexpr const char *const MQTT_POWER_STATE_TOPIC = "pow_stat_t";
constexpr const char *const MQTT_PRESET_MODE_COMMAND_TEMPLATE = "pr_mode_cmd_tpl";
constexpr const char *const MQTT_PRESET_MODE_COMMAND_TOPIC = "pr_mode_cmd_t";
constexpr const char *const MQTT_PRESET_MODE_STATE_TOPIC = "pr_mode_stat_t";
constexpr const char *const MQTT_PRESET_MODE_VALUE_TEMPLATE = "pr_mode_val_tpl";
constexpr const char *const MQTT_PRESET_MODES = "pr_modes";
constexpr const char *const MQTT_RED_TEMPLATE = "r_tpl";
constexpr const char *const MQTT_RETAIN = "ret";
constexpr const char *const MQTT_RGB_COMMAND_TEMPLATE = "rgb_cmd_tpl";
constexpr const char *const MQTT_RGB_COMMAND_TOPIC = "rgb_cmd_t";
constexpr const char *const MQTT_RGB_STATE_TOPIC = "rgb_stat_t";
constexpr const char *const MQTT_RGB_VALUE_TEMPLATE = "rgb_val_tpl";
constexpr const char *const MQTT_RGBW_COMMAND_TEMPLATE = "rgbw_cmd_tpl";
constexpr const char *const MQTT_RGBW_COMMAND_TOPIC = "rgbw_cmd_t";
constexpr const char *const MQTT_RGBW_STATE_TOPIC = "rgbw_stat_t";
constexpr const char *const MQTT_RGBW_VALUE_TEMPLATE = "rgbw_val_tpl";
constexpr const char *const MQTT_RGBWW_COMMAND_TEMPLATE = "rgbww_cmd_tpl";
constexpr const char *const MQTT_RGBWW_COMMAND_TOPIC = "rgbww_cmd_t";
constexpr const char *const MQTT_RGBWW_STATE_TOPIC = "rgbww_stat_t";
constexpr const char *const MQTT_RGBWW_VALUE_TEMPLATE = "rgbww_val_tpl";
constexpr const char *const MQTT_SEND_COMMAND_TOPIC = "send_cmd_t";
constexpr const char *const MQTT_SEND_IF_OFF = "send_if_off";
constexpr const char *const MQTT_SET_FAN_SPEED_TOPIC = "set_fan_spd_t";
constexpr const char *const MQTT_SET_POSITION_TEMPLATE = "set_pos_tpl";
constexpr const char *const MQTT_SET_POSITION_TOPIC = "set_pos_t";
constexpr const char *const MQTT_SOURCE_TYPE = "src_type";
constexpr const char *const MQTT_SPEED_COMMAND_TOPIC = "spd_cmd_t";
constexpr const char *const MQTT_SPEED_RANGE_MAX = "spd_rng_max";
constexpr const char *const MQTT_SPEED_RANGE_MIN = "spd_rng_min";
constexpr const char *const MQTT_SPEED_STATE_TOPIC = "spd_stat_t";
constexpr const char *const MQTT_SPEED_VALUE_TEMPLATE = "spd_val_tpl";
constexpr const char *const MQTT_SPEEDS = "spds";
constexpr const char *const MQTT_STATE_CLASS = "stat_cla";
constexpr const char *const MQTT_STATE_CLOSED = "stat_clsd";
constexpr const char *const MQTT_STATE_CLOSING = "stat_closing";
constexpr const char *const MQTT_STATE_LOCKED = "stat_locked";
constexpr const char *const MQTT_STATE_OFF = "stat_off";
constexpr const char *const MQTT_STATE_ON = "stat_on";
constexpr const char *const MQTT_STATE_OPEN = "stat_open";
constexpr const char *const MQTT_STATE_OPENING = "stat_opening";
constexpr const char *const MQTT_STATE_STOPPED = "stat_stopped";
constexpr const char *const MQTT_STATE_TEMPLATE = "stat_tpl";
constexpr const char *const MQTT_STATE_TOPIC = "stat_t";
constexpr const char *const MQTT_STATE_UNLOCKED = "stat_unlocked";
constexpr const char *const MQTT_STATE_VALUE_TEMPLATE = "stat_val_tpl";
constexpr const char *const MQTT_STEP = "step";
constexpr const char *const MQTT_SUBTYPE = "stype";
constexpr const char *const MQTT_SUPPORTED_COLOR_MODES = "sup_clrm";
constexpr const char *const MQTT_SUPPORTED_FEATURES = "sup_feat";
constexpr const char *const MQTT_SWING_MODE_COMMAND_TEMPLATE = "swing_mode_cmd_tpl";
constexpr const char *const MQTT_SWING_MODE_COMMAND_TOPIC = "swing_mode_cmd_t";
constexpr const char *const MQTT_SWING_MODE_STATE_TEMPLATE = "swing_mode_stat_tpl";
constexpr const char *const MQTT_SWING_MODE_STATE_TOPIC = "swing_mode_stat_t";
constexpr const char *const MQTT_TARGET_HUMIDITY_COMMAND_TEMPLATE = "hum_cmd_tpl";
constexpr const char *const MQTT_TARGET_HUMIDITY_COMMAND_TOPIC = "hum_cmd_t";
constexpr const char *const MQTT_TARGET_HUMIDITY_STATE_TEMPLATE = "hum_state_tpl";
constexpr const char *const MQTT_TARGET_HUMIDITY_STATE_TOPIC = "hum_stat_t";
constexpr const char *const MQTT_TEMPERATURE_COMMAND_TEMPLATE = "temp_cmd_tpl";
constexpr const char *const MQTT_TEMPERATURE_COMMAND_TOPIC = "temp_cmd_t";
constexpr const char *const MQTT_TEMPERATURE_HIGH_COMMAND_TEMPLATE = "temp_hi_cmd_tpl";
constexpr const char *const MQTT_TEMPERATURE_HIGH_COMMAND_TOPIC = "temp_hi_cmd_t";
constexpr const char *const MQTT_TEMPERATURE_HIGH_STATE_TEMPLATE = "temp_hi_stat_tpl";
constexpr const char *const MQTT_TEMPERATURE_HIGH_STATE_TOPIC = "temp_hi_stat_t";
constexpr const char *const MQTT_TEMPERATURE_LOW_COMMAND_TEMPLATE = "temp_lo_cmd_tpl";
constexpr const char *const MQTT_TEMPERATURE_LOW_COMMAND_TOPIC = "temp_lo_cmd_t";
constexpr const char *const MQTT_TEMPERATURE_LOW_STATE_TEMPLATE = "temp_lo_stat_tpl";
constexpr const char *const MQTT_TEMPERATURE_LOW_STATE_TOPIC = "temp_lo_stat_t";
constexpr const char *const MQTT_TEMPERATURE_STATE_TEMPLATE = "temp_stat_tpl";
constexpr const char *const MQTT_TEMPERATURE_STATE_TOPIC = "temp_stat_t";
constexpr const char *const MQTT_TEMPERATURE_UNIT = "temp_unit";
constexpr const char *const MQTT_TILT_CLOSED_VALUE = "tilt_clsd_val";
constexpr const char *const MQTT_TILT_COMMAND_TEMPLATE = "tilt_cmd_tpl";
constexpr const char *const MQTT_TILT_COMMAND_TOPIC = "tilt_cmd_t";
constexpr const char *const MQTT_TILT_INVERT_STATE = "tilt_inv_stat";
constexpr const char *const MQTT_TILT_MAX = "tilt_max";
constexpr const char *const MQTT_TILT_MIN = "tilt_min";
constexpr const char *const MQTT_TILT_OPENED_VALUE = "tilt_opnd_val";
constexpr const char *const MQTT_TILT_OPTIMISTIC = "tilt_opt";
constexpr const char *const MQTT_TILT_STATUS_TEMPLATE = "tilt_status_tpl";
constexpr const char *const MQTT_TILT_STATUS_TOPIC = "tilt_status_t";
constexpr const char *const MQTT_TOPIC = "t";
constexpr const char *const MQTT_UNIQUE_ID = "uniq_id";
constexpr const char *const MQTT_UNIT_OF_MEASUREMENT = "unit_of_meas";
constexpr const char *const MQTT_VALUE_TEMPLATE = "val_tpl";
constexpr const char *const MQTT_WHITE_COMMAND_TOPIC = "whit_cmd_t";
constexpr const char *const MQTT_WHITE_SCALE = "whit_scl";
constexpr const char *const MQTT_WHITE_VALUE_COMMAND_TOPIC = "whit_val_cmd_t";
constexpr const char *const MQTT_WHITE_VALUE_SCALE = "whit_val_scl";
constexpr const char *const MQTT_WHITE_VALUE_STATE_TOPIC = "whit_val_stat_t";
constexpr const char *const MQTT_WHITE_VALUE_TEMPLATE = "whit_val_tpl";
constexpr const char *const MQTT_XY_COMMAND_TOPIC = "xy_cmd_t";
constexpr const char *const MQTT_XY_STATE_TOPIC = "xy_stat_t";
constexpr const char *const MQTT_XY_VALUE_TEMPLATE = "xy_val_tpl";

#else

constexpr const char *const MQTT_ACTION_TEMPLATE = "action_template";
constexpr const char *const MQTT_ACTION_TOPIC = "action_topic";
constexpr const char *const MQTT_AUTOMATION_TYPE = "automation_type";
constexpr const char *const MQTT_AUX_COMMAND_TOPIC = "aux_command_topic";
constexpr const char *const MQTT_AUX_STATE_TEMPLATE = "aux_state_template";
constexpr const char *const MQTT_AUX_STATE_TOPIC = "aux_state_topic";
constexpr const char *const MQTT_AVAILABILITY = "availability";
constexpr const char *const MQTT_AVAILABILITY_MODE = "availability_mode";
constexpr const char *const MQTT_AVAILABILITY_TOPIC = "availability_topic";
constexpr const char *const MQTT_AWAY_MODE_COMMAND_TOPIC = "away_mode_command_topic";
constexpr const char *const MQTT_AWAY_MODE_STATE_TEMPLATE = "away_mode_state_template";
constexpr const char *const MQTT_AWAY_MODE_STATE_TOPIC = "away_mode_state_topic";
constexpr const char *const MQTT_BATTERY_LEVEL_TEMPLATE = "battery_level_template";
constexpr const char *const MQTT_BATTERY_LEVEL_TOPIC = "battery_level_topic";
constexpr const char *const MQTT_BLUE_TEMPLATE = "blue_template";
constexpr const char *const MQTT_BRIGHTNESS_COMMAND_TOPIC = "brightness_command_topic";
constexpr const char *const MQTT_BRIGHTNESS_SCALE = "brightness_scale";
constexpr const char *const MQTT_BRIGHTNESS_STATE_TOPIC = "brightness_state_topic";
constexpr const char *const MQTT_BRIGHTNESS_TEMPLATE = "brightness_template";
constexpr const char *const MQTT_BRIGHTNESS_VALUE_TEMPLATE = "brightness_value_template";
constexpr const char *const MQTT_CHARGING_TEMPLATE = "charging_template";
constexpr const char *const MQTT_CHARGING_TOPIC = "charging_topic";
constexpr const char *const MQTT_CLEANING_TEMPLATE = "cleaning_template";
constexpr const char *const MQTT_CLEANING_TOPIC = "cleaning_topic";
constexpr const char *const MQTT_CODE_ARM_REQUIRED = "code_arm_required";
constexpr const char *const MQTT_CODE_DISARM_REQUIRED = "code_disarm_required";
constexpr const char *const MQTT_COLOR_MODE = "color_mode";
constexpr const char *const MQTT_COLOR_MODE_STATE_TOPIC = "color_mode_state_topic";
constexpr const char *const MQTT_COLOR_MODE_VALUE_TEMPLATE = "color_mode_value_template";
constexpr const char *const MQTT_COLOR_TEMP_COMMAND_TEMPLATE = "color_temp_command_template";
constexpr const char *const MQTT_COLOR_TEMP_COMMAND_TOPIC = "color_temp_command_topic";
constexpr const char *const MQTT_COLOR_TEMP_STATE_TOPIC = "color_temp_state_topic";
constexpr const char *const MQTT_COLOR_TEMP_TEMPLATE = "color_temp_template";
constexpr const char *const MQTT_COLOR_TEMP_VALUE_TEMPLATE = "color_temp_value_template";
constexpr const char *const MQTT_COMMAND_OFF_TEMPLATE = "command_off_template";
constexpr const char *const MQTT_COMMAND_ON_TEMPLATE = "command_on_template";
constexpr const char *const MQTT_COMMAND_RETAIN = "retain";
constexpr const char *const MQTT_COMMAND_TEMPLATE = "command_template";
constexpr const char *const MQTT_COMMAND_TOPIC = "command_topic";
constexpr const char *const MQTT_CONFIGURATION_URL = "configuration_url";
constexpr const char *const MQTT_CURRENT_HUMIDITY_TEMPLATE = "current_humidity_template";
constexpr const char *const MQTT_CURRENT_HUMIDITY_TOPIC = "current_humidity_topic";
constexpr const char *const MQTT_CURRENT_TEMPERATURE_TEMPLATE = "current_temperature_template";
constexpr const char *const MQTT_CURRENT_TEMPERATURE_TOPIC = "current_temperature_topic";
constexpr const char *const MQTT_DEVICE = "device";
constexpr const char *const MQTT_DEVICE_CLASS = "device_class";
constexpr const char *const MQTT_DEVICE_CONNECTIONS = "connections";
constexpr const char *const MQTT_DEVICE_IDENTIFIERS = "identifiers";
constexpr const char *const MQTT_DEVICE_MANUFACTURER = "manufacturer";
constexpr const char *const MQTT_DEVICE_MODEL = "model";
constexpr const char *const MQTT_DEVICE_NAME = "name";
constexpr const char *const MQTT_DEVICE_SUGGESTED_AREA = "suggested_area";
constexpr const char *const MQTT_DEVICE_SW_VERSION = "sw_version";
constexpr const char *const MQTT_DOCKED_TEMPLATE = "docked_template";
constexpr const char *const MQTT_DOCKED_TOPIC = "docked_topic";
constexpr const char *const MQTT_EFFECT_COMMAND_TOPIC = "effect_command_topic";
constexpr const char *const MQTT_EFFECT_LIST = "effect_list";
constexpr const char *const MQTT_EFFECT_STATE_TOPIC = "effect_state_topic";
constexpr const char *const MQTT_EFFECT_TEMPLATE = "effect_template";
constexpr const char *const MQTT_EFFECT_VALUE_TEMPLATE = "effect_value_template";
constexpr const char *const MQTT_ENABLED_BY_DEFAULT = "enabled_by_default";
constexpr const char *const MQTT_ENTITY_CATEGORY = "entity_category";
constexpr const char *const MQTT_ERROR_TEMPLATE = "error_template";
constexpr const char *const MQTT_ERROR_TOPIC = "error_topic";
constexpr const char *const MQTT_EVENT_TYPE = "event_type";
constexpr const char *const MQTT_EVENT_TYPES = "event_types";
constexpr const char *const MQTT_EXPIRE_AFTER = "expire_after";
constexpr const char *const MQTT_FAN_MODE_COMMAND_TEMPLATE = "fan_mode_command_template";
constexpr const char *const MQTT_FAN_MODE_COMMAND_TOPIC = "fan_mode_command_topic";
constexpr const char *const MQTT_FAN_MODE_STATE_TEMPLATE = "fan_mode_state_template";
constexpr const char *const MQTT_FAN_MODE_STATE_TOPIC = "fan_mode_state_topic";
constexpr const char *const MQTT_FAN_SPEED_LIST = "fan_speed_list";
constexpr const char *const MQTT_FAN_SPEED_TEMPLATE = "fan_speed_template";
constexpr const char *const MQTT_FAN_SPEED_TOPIC = "fan_speed_topic";
constexpr const char *const MQTT_FLASH_TIME_LONG = "flash_time_long";
constexpr const char *const MQTT_FLASH_TIME_SHORT = "flash_time_short";
constexpr const char *const MQTT_FORCE_UPDATE = "force_update";
constexpr const char *const MQTT_GREEN_TEMPLATE = "green_template";
constexpr const char *const MQTT_HOLD_COMMAND_TEMPLATE = "hold_command_template";
constexpr const char *const MQTT_HOLD_COMMAND_TOPIC = "hold_command_topic";
constexpr const char *const MQTT_HOLD_STATE_TEMPLATE = "hold_state_template";
constexpr const char *const MQTT_HOLD_STATE_TOPIC = "hold_state_topic";
constexpr const char *const MQTT_HS_COMMAND_TOPIC = "hs_command_topic";
constexpr const char *const MQTT_HS_STATE_TOPIC = "hs_state_topic";
constexpr const char *const MQTT_HS_VALUE_TEMPLATE = "hs_value_template";
constexpr const char *const MQTT_HUMIDITY_UNIT = "humidity_unit";
constexpr const char *const MQTT_ICON = "icon";
constexpr const char *const MQTT_INITIAL = "initial";
constexpr const char *const MQTT_JSON_ATTRIBUTES = "json_attributes";
constexpr const char *const MQTT_JSON_ATTRIBUTES_TEMPLATE = "json_attributes_template";
constexpr const char *const MQTT_JSON_ATTRIBUTES_TOPIC = "json_attributes_topic";
constexpr const char *const MQTT_LAST_RESET_TOPIC = "last_reset_topic";
constexpr const char *const MQTT_LAST_RESET_VALUE_TEMPLATE = "last_reset_value_template";
constexpr const char *const MQTT_MAX = "max";
constexpr const char *const MQTT_MAX_HUMIDITY = "max_humidity";
constexpr const char *const MQTT_MAX_MIREDS = "max_mireds";
constexpr const char *const MQTT_MAX_TEMP = "max_temp";
constexpr const char *const MQTT_MIN = "min";
constexpr const char *const MQTT_MIN_HUMIDITY = "min_humidity";
constexpr const char *const MQTT_MIN_MIREDS = "min_mireds";
constexpr const char *const MQTT_MIN_TEMP = "min_temp";
constexpr const char *const MQTT_MODE = "mode";
constexpr const char *const MQTT_MODE_COMMAND_TEMPLATE = "mode_command_template";
constexpr const char *const MQTT_MODE_COMMAND_TOPIC = "mode_command_topic";
constexpr const char *const MQTT_MODE_STATE_TEMPLATE = "mode_state_template";
constexpr const char *const MQTT_MODE_STATE_TOPIC = "mode_state_topic";
constexpr const char *const MQTT_MODES = "modes";
constexpr const char *const MQTT_NAME = "name";
constexpr const char *const MQTT_OBJECT_ID = "object_id";
constexpr const char *const MQTT_OFF_DELAY = "off_delay";
constexpr const char *const MQTT_ON_COMMAND_TYPE = "on_command_type";
constexpr const char *const MQTT_OPTIMISTIC = "optimistic";
constexpr const char *const MQTT_OPTIONS = "options";
constexpr const char *const MQTT_OSCILLATION_COMMAND_TEMPLATE = "oscillation_command_template";
constexpr const char *const MQTT_OSCILLATION_COMMAND_TOPIC = "oscillation_command_topic";
constexpr const char *const MQTT_OSCILLATION_STATE_TOPIC = "oscillation_state_topic";
constexpr const char *const MQTT_OSCILLATION_VALUE_TEMPLATE = "oscillation_value_template";
constexpr const char *const MQTT_PAYLOAD = "payload";
constexpr const char *const MQTT_PAYLOAD_ARM_AWAY = "payload_arm_away";
constexpr const char *const MQTT_PAYLOAD_ARM_CUSTOM_BYPASS = "payload_arm_custom_bypass";
constexpr const char *const MQTT_PAYLOAD_ARM_HOME = "payload_arm_home";
constexpr const char *const MQTT_PAYLOAD_ARM_NIGHT = "payload_arm_night";
constexpr const char *const MQTT_PAYLOAD_ARM_VACATION = "payload_arm_vacation";
constexpr const char *const MQTT_PAYLOAD_AVAILABLE = "payload_available";
constexpr const char *const MQTT_PAYLOAD_CLEAN_SPOT = "payload_clean_spot";
constexpr const char *const MQTT_PAYLOAD_CLOSE = "payload_close";
constexpr const char *const MQTT_PAYLOAD_DISARM = "payload_disarm";
constexpr const char *const MQTT_PAYLOAD_HIGH_SPEED = "payload_high_speed";
constexpr const char *const MQTT_PAYLOAD_HOME = "payload_home";
constexpr const char *const MQTT_PAYLOAD_LOCATE = "payload_locate";
constexpr const char *const MQTT_PAYLOAD_LOCK = "payload_lock";
constexpr const char *const MQTT_PAYLOAD_LOW_SPEED = "payload_low_speed";
constexpr const char *const MQTT_PAYLOAD_MEDIUM_SPEED = "payload_medium_speed";
constexpr const char *const MQTT_PAYLOAD_NOT_AVAILABLE = "payload_not_available";
constexpr const char *const MQTT_PAYLOAD_NOT_HOME = "payload_not_home";
constexpr const char *const MQTT_PAYLOAD_OFF = "payload_off";
constexpr const char *const MQTT_PAYLOAD_OFF_SPEED = "payload_off_speed";
constexpr const char *const MQTT_PAYLOAD_ON = "payload_on";
constexpr const char *const MQTT_PAYLOAD_OPEN = "payload_open";
constexpr const char *const MQTT_PAYLOAD_OSCILLATION_OFF = "payload_oscillation_off";
constexpr const char *const MQTT_PAYLOAD_OSCILLATION_ON = "payload_oscillation_on";
constexpr const char *const MQTT_PAYLOAD_PAUSE = "payload_pause";
constexpr const char *const MQTT_PAYLOAD_RESET = "payload_reset";
constexpr const char *const MQTT_PAYLOAD_RESET_HUMIDITY = "payload_reset_humidity";
constexpr const char *const MQTT_PAYLOAD_RESET_MODE = "payload_reset_mode";
constexpr const char *const MQTT_PAYLOAD_RESET_PERCENTAGE = "payload_reset_percentage";
constexpr const char *const MQTT_PAYLOAD_RESET_PRESET_MODE = "payload_reset_preset_mode";
constexpr const char *const MQTT_PAYLOAD_RETURN_TO_BASE = "payload_return_to_base";
constexpr const char *const MQTT_PAYLOAD_START = "payload_start";
constexpr const char *const MQTT_PAYLOAD_START_PAUSE = "payload_start_pause";
constexpr const char *const MQTT_PAYLOAD_STOP = "payload_stop";
constexpr const char *const MQTT_PAYLOAD_TURN_OFF = "payload_turn_off";
constexpr const char *const MQTT_PAYLOAD_TURN_ON = "payload_turn_on";
constexpr const char *const MQTT_PAYLOAD_UNLOCK = "payload_unlock";
constexpr const char *const MQTT_PERCENTAGE_COMMAND_TEMPLATE = "percentage_command_template";
constexpr const char *const MQTT_PERCENTAGE_COMMAND_TOPIC = "percentage_command_topic";
constexpr const char *const MQTT_PERCENTAGE_STATE_TOPIC = "percentage_state_topic";
constexpr const char *const MQTT_PERCENTAGE_VALUE_TEMPLATE = "percentage_value_template";
constexpr const char *const MQTT_POSITION_CLOSED = "position_closed";
constexpr const char *const MQTT_POSITION_OPEN = "position_open";
constexpr const char *const MQTT_POSITION_TEMPLATE = "position_template";
constexpr const char *const MQTT_POSITION_TOPIC = "position_topic";
constexpr const char *const MQTT_POWER_COMMAND_TOPIC = "power_command_topic";
constexpr const char *const MQTT_POWER_STATE_TEMPLATE = "power_state_template";
constexpr const char *const MQTT_POWER_STATE_TOPIC = "power_state_topic";
constexpr const char *const MQTT_PRESET_MODE_COMMAND_TEMPLATE = "preset_mode_command_template";
constexpr const char *const MQTT_PRESET_MODE_COMMAND_TOPIC = "preset_mode_command_topic";
constexpr const char *const MQTT_PRESET_MODE_STATE_TOPIC = "preset_mode_state_topic";
constexpr const char *const MQTT_PRESET_MODE_VALUE_TEMPLATE = "preset_mode_value_template";
constexpr const char *const MQTT_PRESET_MODES = "preset_modes";
constexpr const char *const MQTT_RED_TEMPLATE = "red_template";
constexpr const char *const MQTT_RETAIN = "retain";
constexpr const char *const MQTT_RGB_COMMAND_TEMPLATE = "rgb_command_template";
constexpr const char *const MQTT_RGB_COMMAND_TOPIC = "rgb_command_topic";
constexpr const char *const MQTT_RGB_STATE_TOPIC = "rgb_state_topic";
constexpr const char *const MQTT_RGB_VALUE_TEMPLATE = "rgb_value_template";
constexpr const char *const MQTT_RGBW_COMMAND_TEMPLATE = "rgbw_command_template";
constexpr const char *const MQTT_RGBW_COMMAND_TOPIC = "rgbw_command_topic";
constexpr const char *const MQTT_RGBW_STATE_TOPIC = "rgbw_state_topic";
constexpr const char *const MQTT_RGBW_VALUE_TEMPLATE = "rgbw_value_template";
constexpr const char *const MQTT_RGBWW_COMMAND_TEMPLATE = "rgbww_command_template";
constexpr const char *const MQTT_RGBWW_COMMAND_TOPIC = "rgbww_command_topic";
constexpr const char *const MQTT_RGBWW_STATE_TOPIC = "rgbww_state_topic";
constexpr const char *const MQTT_RGBWW_VALUE_TEMPLATE = "rgbww_value_template";
constexpr const char *const MQTT_SEND_COMMAND_TOPIC = "send_command_topic";
constexpr const char *const MQTT_SEND_IF_OFF = "send_if_off";
constexpr const char *const MQTT_SET_FAN_SPEED_TOPIC = "set_fan_speed_topic";
constexpr const char *const MQTT_SET_POSITION_TEMPLATE = "set_position_template";
constexpr const char *const MQTT_SET_POSITION_TOPIC = "set_position_topic";
constexpr const char *const MQTT_SOURCE_TYPE = "source_type";
constexpr const char *const MQTT_SPEED_COMMAND_TOPIC = "speed_command_topic";
constexpr const char *const MQTT_SPEED_RANGE_MAX = "speed_range_max";
constexpr const char *const MQTT_SPEED_RANGE_MIN = "speed_range_min";
constexpr const char *const MQTT_SPEED_STATE_TOPIC = "speed_state_topic";
constexpr const char *const MQTT_SPEED_VALUE_TEMPLATE = "speed_value_template";
constexpr const char *const MQTT_SPEEDS = "speeds";
constexpr const char *const MQTT_STATE_CLASS = "state_class";
constexpr const char *const MQTT_STATE_CLOSED = "state_closed";
constexpr const char *const MQTT_STATE_CLOSING = "state_closing";
constexpr const char *const MQTT_STATE_LOCKED = "state_locked";
constexpr const char *const MQTT_STATE_OFF = "state_off";
constexpr const char *const MQTT_STATE_ON = "state_on";
constexpr const char *const MQTT_STATE_OPEN = "state_open";
constexpr const char *const MQTT_STATE_OPENING = "state_opening";
constexpr const char *const MQTT_STATE_STOPPED = "state_stopped";
constexpr const char *const MQTT_STATE_TEMPLATE = "state_template";
constexpr const char *const MQTT_STATE_TOPIC = "state_topic";
constexpr const char *const MQTT_STATE_UNLOCKED = "state_unlocked";
constexpr const char *const MQTT_STATE_VALUE_TEMPLATE = "state_value_template";
constexpr const char *const MQTT_STEP = "step";
constexpr const char *const MQTT_SUBTYPE = "subtype";
constexpr const char *const MQTT_SUPPORTED_COLOR_MODES = "supported_color_modes";
constexpr const char *const MQTT_SUPPORTED_FEATURES = "supported_features";
constexpr const char *const MQTT_SWING_MODE_COMMAND_TEMPLATE = "swing_mode_command_template";
constexpr const char *const MQTT_SWING_MODE_COMMAND_TOPIC = "swing_mode_command_topic";
constexpr const char *const MQTT_SWING_MODE_STATE_TEMPLATE = "swing_mode_state_template";
constexpr const char *const MQTT_SWING_MODE_STATE_TOPIC = "swing_mode_state_topic";
constexpr const char *const MQTT_TARGET_HUMIDITY_COMMAND_TEMPLATE = "target_humidity_command_template";
constexpr const char *const MQTT_TARGET_HUMIDITY_COMMAND_TOPIC = "target_humidity_command_topic";
constexpr const char *const MQTT_TARGET_HUMIDITY_STATE_TEMPLATE = "target_humidity_state_template";
constexpr const char *const MQTT_TARGET_HUMIDITY_STATE_TOPIC = "target_humidity_state_topic";
constexpr const char *const MQTT_TEMPERATURE_COMMAND_TEMPLATE = "temperature_command_template";
constexpr const char *const MQTT_TEMPERATURE_COMMAND_TOPIC = "temperature_command_topic";
constexpr const char *const MQTT_TEMPERATURE_HIGH_COMMAND_TEMPLATE = "temperature_high_command_template";
constexpr const char *const MQTT_TEMPERATURE_HIGH_COMMAND_TOPIC = "temperature_high_command_topic";
constexpr const char *const MQTT_TEMPERATURE_HIGH_STATE_TEMPLATE = "temperature_high_state_template";
constexpr const char *const MQTT_TEMPERATURE_HIGH_STATE_TOPIC = "temperature_high_state_topic";
constexpr const char *const MQTT_TEMPERATURE_LOW_COMMAND_TEMPLATE = "temperature_low_command_template";
constexpr const char *const MQTT_TEMPERATURE_LOW_COMMAND_TOPIC = "temperature_low_command_topic";
constexpr const char *const MQTT_TEMPERATURE_LOW_STATE_TEMPLATE = "temperature_low_state_template";
constexpr const char *const MQTT_TEMPERATURE_LOW_STATE_TOPIC = "temperature_low_state_topic";
constexpr const char *const MQTT_TEMPERATURE_STATE_TEMPLATE = "temperature_state_template";
constexpr const char *const MQTT_TEMPERATURE_STATE_TOPIC = "temperature_state_topic";
constexpr const char *const MQTT_TEMPERATURE_UNIT = "temperature_unit";
constexpr const char *const MQTT_TILT_CLOSED_VALUE = "tilt_closed_value";
constexpr const char *const MQTT_TILT_COMMAND_TEMPLATE = "tilt_command_template";
constexpr const char *const MQTT_TILT_COMMAND_TOPIC = "tilt_command_topic";
constexpr const char *const MQTT_TILT_INVERT_STATE = "tilt_invert_state";
constexpr const char *const MQTT_TILT_MAX = "tilt_max";
constexpr const char *const MQTT_TILT_MIN = "tilt_min";
constexpr const char *const MQTT_TILT_OPENED_VALUE = "tilt_opened_value";
constexpr const char *const MQTT_TILT_OPTIMISTIC = "tilt_optimistic";
constexpr const char *const MQTT_TILT_STATUS_TEMPLATE = "tilt_status_template";
constexpr const char *const MQTT_TILT_STATUS_TOPIC = "tilt_status_topic";
constexpr const char *const MQTT_TOPIC = "topic";
constexpr const char *const MQTT_UNIQUE_ID = "unique_id";
constexpr const char *const MQTT_UNIT_OF_MEASUREMENT = "unit_of_measurement";
constexpr const char *const MQTT_VALUE_TEMPLATE = "value_template";
constexpr const char *const MQTT_WHITE_COMMAND_TOPIC = "white_command_topic";
constexpr const char *const MQTT_WHITE_SCALE = "white_scale";
constexpr const char *const MQTT_WHITE_VALUE_COMMAND_TOPIC = "white_value_command_topic";
constexpr const char *const MQTT_WHITE_VALUE_SCALE = "white_value_scale";
constexpr const char *const MQTT_WHITE_VALUE_STATE_TOPIC = "white_value_state_topic";
constexpr const char *const MQTT_WHITE_VALUE_TEMPLATE = "white_value_template";
constexpr const char *const MQTT_XY_COMMAND_TOPIC = "xy_command_topic";
constexpr const char *const MQTT_XY_STATE_TOPIC = "xy_state_topic";
constexpr const char *const MQTT_XY_VALUE_TEMPLATE = "xy_value_template";

#endif

}  // namespace mqtt
}  // namespace esphome

#endif  // USE_MQTT
