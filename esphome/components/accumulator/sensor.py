import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import sensor
from esphome.const import CONF_ID, CONF_SENSOR

# TODO: Constants, move to const.py
CONF_RESET = 'reset'
CONF_RESET_VALUE = 'reset_value'
CONF_MAX_TIME_INTERVAL = 'max_time_interval'
CONF_MIN_TIME_INTERVAL = 'min_time_interval'
CONF_MAX_VALUE_INTERVAL = 'max_value_interval'

accumulator_ns = cg.esphome_ns.namespace('accumulator')
AccumulatorSensor = accumulator_ns.class_('AccumulatorSensor', sensor.Sensor, cg.Component)
ResetAction = accumulator_ns.class_('ResetAction', automation.Action)

CONFIG_SCHEMA = sensor.SENSOR_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(AccumulatorSensor),
    cv.Required(CONF_SENSOR): cv.use_id(sensor.Sensor),

    cv.Optional(CONF_RESET_VALUE, default=0.0): cv.float_,
    cv.Optional(CONF_MAX_TIME_INTERVAL, default='10s'): cv.positive_time_period_milliseconds,
    cv.Optional(CONF_MIN_TIME_INTERVAL, default='60m'): cv.positive_time_period_milliseconds,
    cv.Optional(CONF_MAX_VALUE_INTERVAL, default=1.0): cv.float_,
}).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    yield cg.register_component(var, config)
    yield sensor.register_sensor(var, config)

    sens = yield cg.get_variable(config[CONF_SENSOR])
    cg.add(var.set_sensor(sens))

    if CONF_RESET_VALUE in config:
        cg.add(var.set_reset(True))
        cg.add(var.set_reset_value(config[CONF_RESET_VALUE]))

    cg.add(var.set_max_time_interval(config[CONF_MAX_TIME_INTERVAL]))
    cg.add(var.set_min_time_interval(config[CONF_MIN_TIME_INTERVAL]))
    cg.add(var.set_max_value_interval(config[CONF_MAX_VALUE_INTERVAL]))


@automation.register_action('sensor.accumulator.reset', ResetAction, automation.maybe_simple_id({
    cv.Required(CONF_ID): cv.use_id(AccumulatorSensor),
}))
def sensor_accumulator_reset_to_code(config, action_id, template_arg, args):
    paren = yield cg.get_variable(config[CONF_ID])
    yield cg.new_Pvariable(action_id, template_arg, paren)
