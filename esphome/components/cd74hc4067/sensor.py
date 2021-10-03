import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, voltage_sampler
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_NUMBER,
    ICON_FLASH,
    UNIT_WATT,
    STATE_CLASS_MEASUREMENT,
    DEVICE_CLASS_POWER,
)
from . import cd74hc4067_ns, CD74HC4067Component

DEPENDENCIES = ["cd74hc4067"]

CD74HC4067Sensor = cd74hc4067_ns.class_(
    "CD74HC4067Sensor",
    sensor.Sensor,
    cg.PollingComponent,
    voltage_sampler.VoltageSampler,
)

CONF_cd74hc4067_ID = "cd74hc4067_id"
CONFIG_SCHEMA = (
    sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
        icon=ICON_FLASH,
    )
    .extend(
        {
            cv.GenerateID(): cv.declare_id(CD74HC4067Sensor),
            cv.GenerateID(CONF_cd74hc4067_ID): cv.use_id(CD74HC4067Component),
            cv.Required(CONF_NUMBER): cv.int_range(0, 15),
        }
    )
    .extend(cv.polling_component_schema("60s"))
)


def to_code(config):
    parent = yield cg.get_variable(config[CONF_cd74hc4067_ID])
    var = cg.new_Pvariable(
        config[CONF_ID], parent, config[CONF_NAME], config[CONF_NUMBER]
    )
    yield cg.register_component(var, config)
    yield sensor.register_sensor(var, config)
