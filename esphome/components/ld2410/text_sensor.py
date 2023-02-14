import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC, CONF_VERSION
from . import CONF_LD2410_ID, LD2410Component

DEPENDENCIES = ["ld2410"]

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_LD2410_ID): cv.use_id(LD2410Component),
    cv.Optional(CONF_VERSION): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon="mdi:label"
    ),
}


async def to_code(config):
    ld2410_component = await cg.get_variable(config[CONF_LD2410_ID])
    if CONF_VERSION in config:
        sens = await text_sensor.new_text_sensor(config[CONF_VERSION])
        cg.add(ld2410_component.set_version_text_sensor(sens))
