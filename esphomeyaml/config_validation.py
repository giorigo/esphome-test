# coding=utf-8
"""Helpers for config validation using voluptuous."""
from __future__ import print_function

import logging
from datetime import timedelta

import voluptuous as vol

from esphomeyaml import core
from esphomeyaml.const import CONF_AVAILABILITY, CONF_COMMAND_TOPIC, CONF_DISCOVERY, CONF_ID, \
    CONF_NAME, CONF_PAYLOAD_AVAILABLE, \
    CONF_PAYLOAD_NOT_AVAILABLE, CONF_PLATFORM, CONF_RETAIN, CONF_STATE_TOPIC, CONF_TOPIC, \
    ESP_PLATFORM_ESP32, ESP_PLATFORM_ESP8266
from esphomeyaml.core import HexInt, IPAddress
from esphomeyaml.helpers import ensure_unique_string

_LOGGER = logging.getLogger(__name__)

# pylint: disable=invalid-name

port = vol.All(vol.Coerce(int), vol.Range(min=1, max=65535))
positive_float = vol.All(vol.Coerce(float), vol.Range(min=0))
zero_to_one_float = vol.All(vol.Coerce(float), vol.Range(min=0, max=1))
positive_int = vol.All(vol.Coerce(int), vol.Range(min=0))
positive_not_null_int = vol.All(vol.Coerce(int), vol.Range(min=0, min_included=False))

ALLOWED_NAME_CHARS = u'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_'

RESERVED_IDS = [
    # C++ keywords http://en.cppreference.com/w/cpp/keyword
    'alignas', 'alignof', 'and', 'and_eq', 'asm', 'auto', 'bitand', 'bitor', 'bool', 'break',
    'case', 'catch', 'char', 'char16_t', 'char32_t', 'class', 'compl', 'concept', 'const',
    'constexpr', 'const_cast', 'continue', 'decltype', 'default', 'delete', 'do', 'double',
    'dynamic_cast', 'else', 'enum', 'explicit', 'export', 'export', 'extern', 'false', 'float',
    'for', 'friend', 'goto', 'if', 'inline', 'int', 'long', 'mutable', 'namespace', 'new',
    'noexcept', 'not', 'not_eq', 'nullptr', 'operator', 'or', 'or_eq', 'private', 'protected',
    'public', 'register', 'reinterpret_cast', 'requires', 'return', 'short', 'signed', 'sizeof',
    'static', 'static_assert', 'static_cast', 'struct', 'switch', 'template', 'this',
    'thread_local', 'throw', 'true', 'try', 'typedef', 'typeid', 'typename', 'union', 'unsigned',
    'using', 'virtual', 'void', 'volatile', 'wchar_t', 'while', 'xor', 'xor_eq',

    'App', 'pinMode', 'delay', 'delayMicroseconds', 'digitalRead', 'digitalWrite', 'INPUT',
    'OUTPUT',
]


def alphanumeric(value):
    if value is None:
        raise vol.Invalid("string value is None")
    value = unicode(value)
    if not value.isalnum():
        raise vol.Invalid("string value is not alphanumeric")
    return value


def valid_name(value):
    value = string_strict(value)
    if not all(c in ALLOWED_NAME_CHARS for c in value):
        raise vol.Invalid(u"Valid characters for name are {}".format(ALLOWED_NAME_CHARS))
    return value


def string(value):
    if isinstance(value, (dict, list)):
        raise vol.Invalid("string value cannot be dictionary or list.")
    if value is not None:
        return unicode(value)
    raise vol.Invalid("string value is None")


def string_strict(value):
    """Strictly only allow strings."""
    if isinstance(value, (str, unicode)):
        return value
    raise vol.Invalid("Must be string, did you forget putting quotes "
                      "around the value?")


def icon(value):
    """Validate icon."""
    value = string_strict(value)
    if value.startswith('mdi:'):
        return value
    raise vol.Invalid('Icons should start with prefix "mdi:"')


def boolean(value):
    """Validate and coerce a boolean value."""
    if isinstance(value, str):
        value = value.lower()
        if value in ('1', 'true', 'yes', 'on', 'enable'):
            return True
        if value in ('0', 'false', 'no', 'off', 'disable'):
            return False
        raise vol.Invalid('invalid boolean value {}'.format(value))
    return bool(value)


def ensure_list(value):
    """Wrap value in list if it is not one."""
    if value is None:
        return []
    if isinstance(value, list):
        return value
    return [value]


def ensure_dict(value):
    if value is None:
        return {}
    if not isinstance(value, dict):
        raise vol.Invalid("Expected a dictionary")
    return value


def hex_int_(value):
    if isinstance(value, (int, long)):
        return HexInt(value)
    value = string_strict(value).lower()
    if value.startswith('0x'):
        return HexInt(int(value, 16))
    return HexInt(int(value))


def int_(value):
    if isinstance(value, (int, long)):
        return value
    value = string_strict(value).lower()
    if value.startswith('0x'):
        return int(value, 16)
    return int(value)


hex_int = vol.Coerce(hex_int_)
match_cpp_var_ = vol.Match(r'^[a-zA-Z_][a-zA-Z0-9_]+$', msg=u"Must be a valid C++ variable name")


def variable_id(value):
    value = match_cpp_var_(value)
    if value in RESERVED_IDS:
        raise vol.Invalid(u"ID {} is reserved internally and cannot be used".format(value))
    return value


def only_on(platforms):
    if not isinstance(platforms, list):
        platforms = [platforms]

    def validator_(obj):
        if core.ESP_PLATFORM not in platforms:
            raise vol.Invalid(u"This feature is only available on {}".format(platforms))
        return obj

    return validator_


only_on_esp32 = only_on(ESP_PLATFORM_ESP32)
only_on_esp8266 = only_on(ESP_PLATFORM_ESP8266)


# Adapted from:
# https://github.com/alecthomas/voluptuous/issues/115#issuecomment-144464666
def has_at_least_one_key(*keys):
    """Validate that at least one key exists."""

    def validate(obj):
        """Test keys exist in dict."""
        if not isinstance(obj, dict):
            raise vol.Invalid('expected dictionary')

        for k in obj.keys():
            if k in keys:
                return obj
        raise vol.Invalid('must contain one of {}.'.format(', '.join(keys)))

    return validate


TIME_PERIOD_ERROR = "Time period {} should be format 5ms, 5s, 5min, 5h"

time_period_dict = vol.All(
    dict, vol.Schema({
        'days': vol.Coerce(int),
        'hours': vol.Coerce(int),
        'minutes': vol.Coerce(int),
        'seconds': vol.Coerce(int),
        'milliseconds': vol.Coerce(int),
    }),
    has_at_least_one_key('days', 'hours', 'minutes',
                         'seconds', 'milliseconds'),
    lambda value: timedelta(**value))


def time_period_str(value):
    """Validate and transform time offset."""
    if isinstance(value, int):
        raise vol.Invalid("Make sure you wrap time values in quotes")
    elif not isinstance(value, (str, unicode)):
        raise vol.Invalid(TIME_PERIOD_ERROR.format(value))

    value = unicode(value)
    if value.endswith(u'ms'):
        return vol.Coerce(int)(value[:-2])
    elif value.endswith(u's'):
        return vol.Coerce(float)(value[:-1]) * 1000
    elif value.endswith(u'min'):
        return vol.Coerce(float)(value[:-3]) * 1000 * 60
    elif value.endswith(u'h'):
        return vol.Coerce(float)(value[:-1]) * 1000 * 60 * 60
    raise vol.Invalid(TIME_PERIOD_ERROR.format(value))


def time_period_milliseconds(value):
    try:
        return timedelta(milliseconds=int(value))
    except (ValueError, TypeError):
        raise vol.Invalid('Expected milliseconds, got {}'.format(value))


def time_period_to_milliseconds(value):
    if isinstance(value, (int, long)):
        return value
    if isinstance(value, float):
        return int(value)
    return value / timedelta(milliseconds=1)


time_period = vol.All(vol.Any(time_period_str, timedelta, time_period_dict,
                              time_period_milliseconds), time_period_to_milliseconds)
positive_time_period = vol.All(time_period, vol.Range(min=0))
positive_not_null_time_period = vol.All(time_period, vol.Range(min=0, min_included=False))


METRIC_SUFFIXES = {
    'E': 1e18, 'P': 1e15, 'T': 1e12, 'G': 1e9, 'M': 1e6, 'k': 1e3, 'da': 10, 'd': 1e-1,
    'c': 1e-2, 'm': 0.001, u'µ': 1e-6, 'u': 1e-6, 'n': 1e-9, 'p': 1e-12, 'f': 1e-15, 'a': 1e-18,
}


def frequency(value):
    value = string(value).replace(' ', '').lower()
    if value.endswith('Hz') or value.endswith('hz') or value.endswith('HZ'):
        value = value[:-2]
    if not value:
        raise vol.Invalid(u"Frequency must have value")
    multiplier = 1
    if value[:-1] in METRIC_SUFFIXES:
        multiplier = METRIC_SUFFIXES[value[:-1]]
        value = value[:-1]
    elif len(value) >= 2 and value[:-2] in METRIC_SUFFIXES:
        multiplier = METRIC_SUFFIXES[value[:-2]]
        value = value[:-2]
    float_val = vol.Coerce(float)(value)
    return float_val * multiplier


def hostname(value):
    value = string(value)
    if len(value) > 63:
        raise vol.Invalid("Hostnames can only be 63 characters long")
    for c in value:
        if not (c.isalnum() or c in '_-'):
            raise vol.Invalid("Hostname can only have alphanumeric characters and _ or -")
    return value


def ssid(value):
    if value is None:
        raise vol.Invalid("SSID can not be None")
    if not isinstance(value, str):
        raise vol.Invalid("SSID must be a string. Did you wrap it in quotes?")
    if not value:
        raise vol.Invalid("SSID can't be empty.")
    if len(value) > 32:
        raise vol.Invalid("SSID can't be longer than 32 characters")
    return value


def ipv4(value):
    if isinstance(value, list):
        parts = value
    elif isinstance(value, str):
        parts = value.split('.')
    else:
        raise vol.Invalid("IPv4 address must consist of either string or "
                          "integer list")
    if len(parts) != 4:
        raise vol.Invalid("IPv4 address must consist of four point-separated "
                          "integers")
    parts_ = list(map(int, parts))
    if not all(0 <= x < 256 for x in parts_):
        raise vol.Invalid("IPv4 address parts must be in range from 0 to 255")
    return IPAddress(*parts_)


def publish_topic(value):
    value = string_strict(value)
    if value.endswith('/'):
        raise vol.Invalid("Publish topic can't end with '/'")
    if '+' in value or '#' in value:
        raise vol.Invalid("Publish topic can't contain '+' or '#'")
    return value


subscribe_topic = string_strict  # TODO improve this
mqtt_payload = string  # TODO improve this
uint8_t = vol.All(int_, vol.Range(min=0, max=255))
uint16_t = vol.All(int_, vol.Range(min=0, max=65535))
uint32_t = vol.All(int_, vol.Range(min=0, max=4294967295))
hex_uint8_t = vol.All(hex_int, vol.Range(min=0, max=255))
hex_uint16_t = vol.All(hex_int, vol.Range(min=0, max=65535))
hex_uint32_t = vol.All(hex_int, vol.Range(min=0, max=4294967295))
i2c_address = hex_uint8_t


def invalid(_):
    raise vol.Invalid("This shouldn't happen.")


def valid(value):
    return value


REGISTERED_IDS = set()


def register_variable_id(value):
    s = variable_id(value)
    if s in REGISTERED_IDS:
        raise vol.Invalid("This ID has already been used")
    REGISTERED_IDS.add(s)
    return s


class GenerateID(vol.Optional):
    def __init__(self, basename, key=CONF_ID):
        self._basename = basename
        super(GenerateID, self).__init__(key, default=self.default_variable_id)

    def default_variable_id(self):
        return ensure_unique_string(self._basename, REGISTERED_IDS)


REQUIRED_ID_SCHEMA = vol.Schema({
    vol.Required(CONF_ID): register_variable_id,
})

PLATFORM_SCHEMA = vol.Schema({
    vol.Required(CONF_PLATFORM): valid,
})

MQTT_COMPONENT_AVAILABILITY_SCHEMA = vol.Schema({
    vol.Required(CONF_TOPIC): subscribe_topic,
    vol.Optional(CONF_PAYLOAD_AVAILABLE, default='online'): mqtt_payload,
    vol.Optional(CONF_PAYLOAD_NOT_AVAILABLE, default='offline'): mqtt_payload,
})

MQTT_COMPONENT_SCHEMA = vol.Schema({
    vol.Required(CONF_NAME): string,
    vol.Optional(CONF_RETAIN): boolean,
    vol.Optional(CONF_DISCOVERY): boolean,
    vol.Optional(CONF_STATE_TOPIC): publish_topic,
    vol.Optional(CONF_AVAILABILITY): MQTT_COMPONENT_AVAILABILITY_SCHEMA,
})

MQTT_COMMAND_COMPONENT_SCHEMA = MQTT_COMPONENT_SCHEMA.extend({
    vol.Optional(CONF_COMMAND_TOPIC): subscribe_topic,
})
