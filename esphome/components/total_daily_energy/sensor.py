import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, time
from esphome.const import (
    CONF_ID,
    CONF_TIME_ID,
    DEVICE_CLASS_ENERGY,
    ICON_EMPTY,
    LAST_RESET_TYPE_AUTO,
    STATE_CLASS_MEASUREMENT,
    UNIT_WATT_HOURS,
)

DEPENDENCIES = ["time"]

CONF_POWER_ID = "power_id"
total_daily_energy_ns = cg.esphome_ns.namespace("total_daily_energy")
TotalDailyEnergy = total_daily_energy_ns.class_(
    "TotalDailyEnergy", sensor.Sensor, cg.Component
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(TotalDailyEnergy),
            cv.GenerateID(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
            cv.Required(CONF_POWER_ID): cv.use_id(sensor.Sensor),
        }
    )
    .extend(
        sensor.sensor_schema(
            UNIT_WATT_HOURS,
            ICON_EMPTY,
            0,
            DEVICE_CLASS_ENERGY,
            STATE_CLASS_MEASUREMENT,
            LAST_RESET_TYPE_AUTO,
        )
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)

    sens = await cg.get_variable(config[CONF_POWER_ID])
    cg.add(var.set_parent(sens))
    time_ = await cg.get_variable(config[CONF_TIME_ID])
    cg.add(var.set_time(time_))
