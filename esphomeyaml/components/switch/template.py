import voluptuous as vol

from esphomeyaml import automation
from esphomeyaml.automation import ACTION_REGISTRY
from esphomeyaml.components import switch
import esphomeyaml.config_validation as cv
from esphomeyaml.const import CONF_ID, CONF_LAMBDA, CONF_NAME, CONF_OPTIMISTIC, \
    CONF_RESTORE_STATE, \
    CONF_STATE, CONF_TURN_OFF_ACTION, CONF_TURN_ON_ACTION
from esphomeyaml.cpp_generator import Pvariable, add, get_variable, process_lambda, templatable
from esphomeyaml.cpp_helpers import setup_component
from esphomeyaml.cpp_types import Action, App, Component, NoArg, bool_, optional

TemplateSwitch = switch.switch_ns.class_('TemplateSwitch', switch.Switch, Component)
SwitchPublishAction = switch.switch_ns.class_('SwitchPublishAction', Action)

PLATFORM_SCHEMA = cv.nameable(switch.SWITCH_PLATFORM_SCHEMA.extend({
    cv.GenerateID(): cv.declare_variable_id(TemplateSwitch),
    vol.Optional(CONF_LAMBDA): cv.lambda_,
    vol.Optional(CONF_OPTIMISTIC): cv.boolean,
    vol.Optional(CONF_TURN_OFF_ACTION): automation.validate_automation(single=True),
    vol.Optional(CONF_TURN_ON_ACTION): automation.validate_automation(single=True),
    vol.Optional(CONF_RESTORE_STATE): cv.boolean,
}).extend(cv.COMPONENT_SCHEMA.schema), cv.has_at_least_one_key(CONF_LAMBDA, CONF_OPTIMISTIC))


def to_code(config):
    rhs = App.make_template_switch(config[CONF_NAME])
    template = Pvariable(config[CONF_ID], rhs)

    switch.setup_switch(template, config)

    if CONF_LAMBDA in config:
        for template_ in process_lambda(config[CONF_LAMBDA], [],
                                        return_type=optional.template(bool_)):
            yield
        add(template.set_state_lambda(template_))
    if CONF_TURN_OFF_ACTION in config:
        automation.build_automation(template.get_turn_off_trigger(), NoArg,
                                    config[CONF_TURN_OFF_ACTION])
    if CONF_TURN_ON_ACTION in config:
        automation.build_automation(template.get_turn_on_trigger(), NoArg,
                                    config[CONF_TURN_ON_ACTION])
    if CONF_OPTIMISTIC in config:
        add(template.set_optimistic(config[CONF_OPTIMISTIC]))

    if CONF_RESTORE_STATE in config:
        add(template.set_restore_state(config[CONF_RESTORE_STATE]))

    setup_component(template, config)


BUILD_FLAGS = '-DUSE_TEMPLATE_SWITCH'

CONF_SWITCH_TEMPLATE_PUBLISH = 'switch.template.publish'
SWITCH_TEMPLATE_PUBLISH_ACTION_SCHEMA = vol.Schema({
    vol.Required(CONF_ID): cv.use_variable_id(switch.Switch),
    vol.Required(CONF_STATE): cv.templatable(cv.boolean),
})


@ACTION_REGISTRY.register(CONF_SWITCH_TEMPLATE_PUBLISH, SWITCH_TEMPLATE_PUBLISH_ACTION_SCHEMA)
def switch_template_publish_to_code(config, action_id, arg_type, template_arg):
    for var in get_variable(config[CONF_ID]):
        yield None
    rhs = var.make_switch_publish_action(template_arg)
    type = SwitchPublishAction.template(arg_type)
    action = Pvariable(action_id, rhs, type=type)
    for template_ in templatable(config[CONF_STATE], arg_type, bool_):
        yield None
    add(action.set_state(template_))
    yield action


def to_hass_config(data, config):
    return switch.core_to_hass_config(data, config)
