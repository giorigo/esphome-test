import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.const import CONF_ID, CONF_PIN, CONF_IGNORE_ERROR_WHEN_NO_SENSORS_FOUND

MULTI_CONF = True
AUTO_LOAD = ["sensor"]

dallas_ns = cg.esphome_ns.namespace("dallas")
DallasComponent = dallas_ns.class_("DallasComponent", cg.PollingComponent)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(DallasComponent),
        cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        cv.Optional(CONF_IGNORE_ERROR_WHEN_NO_SENSORS_FOUND, default="false"): cv.boolean
    }
).extend(cv.polling_component_schema("60s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_ignore_error_when_no_sensors_found(config[CONF_IGNORE_ERROR_WHEN_NO_SENSORS_FOUND]))
    await cg.register_component(var, config)

    pin = await cg.gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_pin(pin))
