# Dummy integration to allow relying on AsyncTCP
import esphome.codegen as cg
from esphome.const import ARDUINO_VERSION_ESP32_1_0_0, ARDUINO_VERSION_ESP32_1_0_1, \
    ARDUINO_VERSION_ESP32_1_0_2
from esphome.core import CORE, coroutine_with_priority


@coroutine_with_priority(200.0)
def to_code(config):
    if CORE.is_esp32:
        # https://github.com/OttoWinter/AsyncTCP/blob/master/library.json
        cg.add_library('AsyncTCP-esphome', '1.1.1')
    elif CORE.is_esp8266:
        # https://github.com/OttoWinter/ESPAsyncTCP
        cg.add_library('ESPAsyncTCP-esphome', '1.2.2')
