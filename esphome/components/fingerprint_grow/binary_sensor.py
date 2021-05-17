import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ICON, ICON_KEY_PLUS
from . import CONF_FINGERPRINT_GROW_ID, FingerprintGrowComponent

DEPENDENCIES = ["fingerprint_grow"]

CONFIG_SCHEMA = binary_sensor.BINARY_SENSOR_SCHEMA.extend(
    {
        cv.GenerateID(CONF_FINGERPRINT_GROW_ID): cv.use_id(FingerprintGrowComponent),
        cv.Optional(CONF_ICON, default=ICON_KEY_PLUS): cv.icon,
    }
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_FINGERPRINT_GROW_ID])
    var = yield binary_sensor.new_binary_sensor(config)
    cg.add(hub.set_enrolling_binary_sensor(var))
