import esphome.config_validation as cv
import esphome.final_validate as fv
import esphome.codegen as cg
from esphome.components import sensor, text_sensor
from esphome.const import (
    CONF_ID,
    ICON_COUNTER,
    CONF_DEVICE,
    CONF_FREE,
    CONF_FRAGMENTATION,
    CONF_BLOCK,
    CONF_LOOP_TIME,
    UNIT_MILLISECOND,
    ICON_TIMER,
    UNIT_COUNTS,
    UNIT_BYTES,
    DEVICE_CLASS_EMPTY,
)
from esphome.core import CORE

CODEOWNERS = ["@OttoWinter"]
DEPENDENCIES = ["logger"]

debug_ns = cg.esphome_ns.namespace("debug")
DebugComponent = debug_ns.class_("DebugComponent", cg.PollingComponent)


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(DebugComponent),
        cv.Optional(CONF_DEVICE): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {cv.GenerateID(): cv.declare_id(text_sensor.TextSensor)}
        ),
        cv.Optional(CONF_FREE): sensor.sensor_schema(
            UNIT_BYTES, ICON_COUNTER, 1, DEVICE_CLASS_EMPTY
        ),
        cv.Optional(CONF_FRAGMENTATION): cv.All(
            cv.only_on_esp8266,
            sensor.sensor_schema(UNIT_COUNTS, ICON_COUNTER, 1, DEVICE_CLASS_EMPTY),
        ),
        cv.Optional(CONF_BLOCK): cv.All(
            cv.only_on_esp8266,
            sensor.sensor_schema(UNIT_BYTES, ICON_COUNTER, 1, DEVICE_CLASS_EMPTY),
        ),
        cv.Optional(CONF_LOOP_TIME): sensor.sensor_schema(
            UNIT_MILLISECOND, ICON_TIMER, 1, DEVICE_CLASS_EMPTY
        ),
    }
).extend(cv.polling_component_schema("60s"))


def validate_framework(value):
    framework_version = fv.get_arduino_framework_version()
    if framework_version is None or framework_version == "dev":
        return

    if framework_version < "2.5.2":
        raise cv.Invalid(
            "This sensor is not supported on Arduino framework versions below 2.5.2, "
            "please change esphome->arduino_version or remove this sensor."
        )


FINAL_VALIDATE_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_FRAGMENTATION): validate_framework,
        cv.Optional(CONF_BLOCK): validate_framework,
    },
    extra=cv.ALLOW_EXTRA,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_DEVICE in config:
        sens = cg.new_Pvariable(config[CONF_DEVICE][CONF_ID])
        await text_sensor.register_text_sensor(sens, config[CONF_DEVICE])
        cg.add(var.set_device_info_sensor(sens))

    if CONF_FREE in config:
        sens = await sensor.new_sensor(config[CONF_FREE])
        cg.add(var.set_free_sensor(sens))

    if CONF_FRAGMENTATION in config:
        sens = await sensor.new_sensor(config[CONF_FRAGMENTATION])
        cg.add(var.set_fragmentation_sensor(sens))

    if CONF_BLOCK in config:
        sens = await sensor.new_sensor(config[CONF_BLOCK])
        cg.add(var.set_block_sensor(sens))

    if CONF_LOOP_TIME in config:
        sens = await sensor.new_sensor(config[CONF_LOOP_TIME])
        cg.add(var.set_loop_time_sensor(sens))
