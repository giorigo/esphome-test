import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome import pins
from esphome.const import (
    CONF_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
    UNIT_AMPERE,
    UNIT_WATT,
)

CODEOWNERS = ["@angelnu"]

CONF_IRQ_PIN = "irq_pin"
CONF_CURRENT_A = "current_a"
CONF_CURRENT_B = "current_b"
CONF_ACTIVE_POWER_A = "active_power_a"
CONF_ACTIVE_POWER_B = "active_power_b"

ADE7953_CONFIG_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_IRQ_PIN): pins.internal_gpio_input_pin_schema,
        cv.Optional(CONF_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CURRENT_A): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CURRENT_B): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_ACTIVE_POWER_A): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_ACTIVE_POWER_B): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
).extend(cv.polling_component_schema("1s"))


async def register_ade7953(var, config):
    await cg.register_component(var, config)

    if irq_pin_config := config.get(CONF_IRQ_PIN):
        irq_pin = await cg.gpio_pin_expression(irq_pin_config)
        cg.add(var.set_irq_pin(irq_pin))

    for key in [
        CONF_VOLTAGE,
        CONF_CURRENT_A,
        CONF_CURRENT_B,
        CONF_ACTIVE_POWER_A,
        CONF_ACTIVE_POWER_B,
    ]:
        if key not in config:
            continue
        conf = config[key]
        sens = await sensor.new_sensor(conf)
        cg.add(getattr(var, f"set_{key}_sensor")(sens))
