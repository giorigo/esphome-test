import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.automation import maybe_simple_id
from esphome.components.output import FloatOutput
from esphome.const import CONF_ID, CONF_IDLE_LEVEL, CONF_MAX_LEVEL, CONF_MIN_LEVEL, CONF_OUTPUT, \
    CONF_LEVEL, CONF_RESTORE, CONF_KEEP_ON_TIME,CONF_RUN_DURATION

servo_ns = cg.esphome_ns.namespace('servo')
Servo = servo_ns.class_('Servo', cg.Component)
ServoWriteAction = servo_ns.class_('ServoWriteAction', automation.Action)
ServoDetachAction = servo_ns.class_('ServoDetachAction', automation.Action)

MULTI_CONF = True
CONFIG_SCHEMA = cv.Schema({
    cv.Required(CONF_ID): cv.declare_id(Servo),
    cv.Required(CONF_OUTPUT): cv.use_id(FloatOutput),
    cv.Optional(CONF_MIN_LEVEL, default='3%'): cv.percentage,
    cv.Optional(CONF_IDLE_LEVEL, default='7.5%'): cv.percentage,
    cv.Optional(CONF_MAX_LEVEL, default='12%'): cv.percentage,
    cv.Optional(CONF_RESTORE, default=False): cv.boolean,
    cv.Optional(CONF_KEEP_ON_TIME, default='10s'): cv.positive_time_period_milliseconds,
    cv.Optional(CONF_RUN_DURATION, default='0s'): cv.positive_time_period_milliseconds
}).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)

    out = yield cg.get_variable(config[CONF_OUTPUT])
    cg.add(var.set_output(out))
    cg.add(var.set_min_level(config[CONF_MIN_LEVEL]))
    cg.add(var.set_idle_level(config[CONF_IDLE_LEVEL]))
    cg.add(var.set_max_level(config[CONF_MAX_LEVEL]))
    cg.add(var.set_restore(config[CONF_RESTORE]))
    cg.add(var.set_keep_on_time(config[CONF_KEEP_ON_TIME]))
    cg.add(var.set_run_duration(config[CONF_RUN_DURATION]))


@automation.register_action('servo.write', ServoWriteAction, cv.Schema({
    cv.Required(CONF_ID): cv.use_id(Servo),
    cv.Required(CONF_LEVEL): cv.templatable(cv.possibly_negative_percentage),
}))
def servo_write_to_code(config, action_id, template_arg, args):
    paren = yield cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = yield cg.templatable(config[CONF_LEVEL], args, float)
    cg.add(var.set_value(template_))
    yield var


@automation.register_action('servo.detach', ServoDetachAction, maybe_simple_id({
    cv.Required(CONF_ID): cv.use_id(Servo),
}))
def servo_detach_to_code(config, action_id, template_arg, args):
    paren = yield cg.get_variable(config[CONF_ID])
    yield cg.new_Pvariable(action_id, template_arg, paren)
