import esphome.codegen as cg
from esphome import automation
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    CONF_EC,
    CONF_TEMPERATURE,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_TEMPERATURE,
    ICON_EMPTY,
    UNIT_CELSIUS,
    UNIT_MILLISIEMENS_PER_CENTIMETER,
)

DEPENDENCIES = ["i2c", "sensor"]
CODEOWNERS = ["@pvizeli"]

CONF_SOLUTION = "solution"
CONF_TEMPERATURE_SENSOR = "temperature_sensor"
CONF_TEMPERATURE_COMPENSATION = "temperature_compensation"

ufire_ec_ns = cg.esphome_ns.namespace("ufire_ec")
UFireECComponent = ufire_ec_ns.class_(
    "UFireECComponent", cg.PollingComponent, i2c.I2CDevice
)

# Actions
UFireISECalibrateProbeAction = ufire_ec_ns.class_(
    "UFireISECalibrateProbeAction", automation.Action
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(UFireECComponent),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                UNIT_CELSIUS, ICON_EMPTY, 1, DEVICE_CLASS_TEMPERATURE
            ),
            cv.Optional(CONF_EC): sensor.sensor_schema(
                UNIT_MILLISIEMENS_PER_CENTIMETER, ICON_EMPTY, 1, DEVICE_CLASS_EMPTY
            ),
            cv.Optional(CONF_TEMPERATURE_SENSOR): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_TEMPERATURE_COMPENSATION, default=25.0): cv.float_,
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x3C))
)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    cg.add(var.set_temperature_compensation(config[CONF_TEMPERATURE_COMPENSATION]))

    if CONF_TEMPERATURE in config:
        sens = yield sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(var.set_temperature_sensor(sens))

    if CONF_EC in config:
        sens = yield sensor.new_sensor(config[CONF_EC])
        cg.add(var.set_ec_sensor(sens))

    if CONF_TEMPERATURE_SENSOR in config:
        sens = yield cg.get_variable(config[CONF_TEMPERATURE_SENSOR])
        cg.add(var.set_temperature_sensor_external(sens))

    yield i2c.register_i2c_device(var, config)


UFIRE_ISE_CALIBRATE_PROBE_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(UFireECComponent),
        cv.Required(CONF_SOLUTION): cv.templatable(float),
    }
)


@automation.register_action(
    "ufire_ec.calibrate_probe",
    UFireISECalibrateProbeAction,
    UFIRE_ISE_CALIBRATE_PROBE_SCHEMA,
)
def ufire_ise_calibrate_probe_to_code(config, action_id, template_arg, args):
    paren = yield cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = yield cg.templatable(config[CONF_SOLUTION], args, float)
    cg.add(var.set_solution(template_))
    yield var
