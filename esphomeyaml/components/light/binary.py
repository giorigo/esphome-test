import voluptuous as vol

import esphomeyaml.config_validation as cv
from esphomeyaml.components import light, output
from esphomeyaml.const import CONF_MAKE_ID, CONF_NAME, CONF_OUTPUT, CONF_EFFECTS
from esphomeyaml.helpers import App, get_variable, variable, setup_component

PLATFORM_SCHEMA = cv.nameable(light.LIGHT_PLATFORM_SCHEMA.extend({
    cv.GenerateID(CONF_MAKE_ID): cv.declare_variable_id(light.MakeLight),
    vol.Required(CONF_OUTPUT): cv.use_variable_id(output.BinaryOutput),
    vol.Optional(CONF_EFFECTS): light.validate_effects(light.BINARY_EFFECTS),
}).extend(cv.COMPONENT_SCHEMA.schema))


def to_code(config):
    for output in get_variable(config[CONF_OUTPUT]):
        yield
    rhs = App.make_binary_light(config[CONF_NAME], output)
    light_struct = variable(config[CONF_MAKE_ID], rhs)
    light.setup_light(light_struct.Pstate, light_struct.Pmqtt, config)
    setup_component(light_struct.Pstate, config)


def to_hass_config(data, config):
    return light.core_to_hass_config(data, config, brightness=False, rgb=False, color_temp=False,
                                     white_value=False)
