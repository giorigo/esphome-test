import re

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.automation import LambdaAction
from esphome import automation
from esphome.automation import maybe_simple_id
from esphome.core import CORE, EsphomeError, Lambda, coroutine, coroutine_with_priority
from esphome.components import sensor
from esphome.py_compat import text_type
from esphome.const import CONF_ID, CONF_TRIGGER_ID

IS_PLATFORM_COMPONENT = True

CONF_ON_RECEIVE = 'on_receive'
CONF_CANBUS_ID = 'canbus_id'
CONF_CAN_ID = 'can_id'
CONF_CAN_DATA = 'can_data'
CONF_SENDER_ID = 'sender_id'

CONF_CANBUS_SEND_ACTION = 'canbus.send'

canbus_ns = cg.esphome_ns.namespace('canbus')
CanbusComponent = canbus_ns.class_('CanbusComponent', cg.Component)
CanbusTrigger = canbus_ns.class_('CanbusTrigger',
                                 automation.Trigger.template(cg.std_string),
                                 cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(CanbusComponent),
    cv.Required(CONF_SENDER_ID): cv.int_range(min=0, max=255),
    cv.Optional(CONF_ON_RECEIVE): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CanbusTrigger),
        cv.GenerateID(CONF_CAN_ID): cv.int_range(min=1, max=4096),
    }),
}).extend(cv.COMPONENT_SCHEMA)

# Actions
SendAction = canbus_ns.class_('SendAction', automation.Action)

CANBUS_ACTION_SCHEMA = maybe_simple_id({
    cv.Required(CONF_CANBUS_ID): cv.use_id(CanbusComponent),
    cv.Required(CONF_CAN_ID): cv.int_range(min=1, max=4096),
    cv.Required(CONF_CAN_DATA): cv.templatable(cv.int_),
})

@coroutine
def setup_canbus_core_(var, config):
    yield cg.register_component(var, config)
    if CONF_CANBUS_ID in config:
        cg.add(var.set_canbus_id(config[CONF_CANBUS_ID]))
    if CONF_SENDER_ID in config:
        cg.add(var.set_sender_id([config[CONF_SENDER_ID]]))
    if CONF_CAN_DATA in config:
        cg.add(var.set_can_data([config[CONF_CAN_DATA]]))


@coroutine
def register_canbus(var, config):
    if not CORE.has_id(config[CONF_ID]):
        var = cg.Pvariable(config[CONF_ID], var)
    yield setup_canbus_core_(var, config)


@automation.register_action(CONF_CANBUS_SEND_ACTION, SendAction, CANBUS_ACTION_SCHEMA)
def canbus_action_to_code(config, action_id, template_arg, args):
    canbus = yield cg.get_variable(config[CONF_CANBUS_ID])
    var = yield cg.new_Pvariable(action_id, template_arg,  canbus)
    lambda_ = yield cg.process_lambda(config[CONF_CAN_DATA], args, return_type=cg.float_)
    can_id = yield cg.templatable(config[CONF_CAN_ID], args, cg.uint16)
    cg.add(var.set_can_id(can_id))
    cg.add(var.set_data(lambda_))
    yield var


@coroutine_with_priority(100.0)
def to_code(config):
    cg.add_global(canbus_ns.using)
    cg.add_define("USE_CANBUS")
