import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import i2c
from esphome.const import (
    CONF_ID,
    CONF_INPUT,
    CONF_NUMBER,
    CONF_MODE,
    CONF_INVERTED,
    CONF_OUTPUT,
    CONF_OPEN_DRAIN,
)

CODEOWNERS = ["@jgus"]
DEPENDENCIES = ["i2c"]
MULTI_CONF = True

ads7128_ns = cg.esphome_ns.namespace("ads7128")

ADS7128Component = ads7128_ns.class_("ADS7128Component", cg.Component, i2c.I2CDevice)
ADS7128GPIOPin = ads7128_ns.class_("ADS7128GPIOPin", cg.GPIOPin)

CONF_ADS7128 = "ads7128"
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.declare_id(ADS7128Component),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x10))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)


def validate_mode(value):
    if not (value[CONF_INPUT] or value[CONF_OUTPUT]):
        raise cv.Invalid("Mode must be either input or output")
    if value[CONF_INPUT] and value[CONF_OUTPUT]:
        raise cv.Invalid("Mode must be either input or output")
    return value


ADS7128_PIN_SCHEMA = cv.All(
    {
        cv.GenerateID(): cv.declare_id(ADS7128GPIOPin),
        cv.Required(CONF_ADS7128): cv.use_id(ADS7128Component),
        cv.Required(CONF_NUMBER): cv.int_range(min=0, max=7),
        cv.Optional(CONF_MODE, default={}): cv.All(
            {
                cv.Optional(CONF_INPUT, default=False): cv.boolean,
                cv.Optional(CONF_OUTPUT, default=False): cv.boolean,
                cv.Optional(CONF_OPEN_DRAIN, default=False): cv.boolean,
            },
            validate_mode,
        ),
        cv.Optional(CONF_INVERTED, default=False): cv.boolean,
    }
)


@pins.PIN_SCHEMA_REGISTRY.register("ads7128", ADS7128_PIN_SCHEMA)
async def ads7128_pin_to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    parent = await cg.get_variable(config[CONF_ADS7128])

    cg.add(var.set_parent(parent))

    cg.add(var.set_pin(config[CONF_NUMBER]))
    cg.add(var.set_inverted(config[CONF_INVERTED]))
    cg.add(var.set_flags(pins.gpio_flags_expr(config[CONF_MODE])))
    return var
