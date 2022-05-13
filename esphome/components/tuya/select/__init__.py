from esphome.components import select
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_OPTIONS, CONF_OPTIMISTIC, CONF_ID, CONF_SELECT_DATAPOINT
from .. import tuya_ns, CONF_TUYA_ID, Tuya

DEPENDENCIES = ["tuya"]
CODEOWNERS = ["@bearpawmaxim"]

TuyaSelect = tuya_ns.class_("TuyaSelect", select.Select, cg.Component)


def ensure_option_map():
    def validator(value):
        cv.check_not_templatable(value)
        option = cv.All(cv.string_strict)
        mapping = cv.All(cv.int_range(0, 2**8 - 1))
        options_map_schema = cv.Schema({option: mapping})
        value = options_map_schema(value)

        all_values = list(value.values())
        unique_values = set(value.values())
        if len(all_values) != len(unique_values):
            raise cv.Invalid("Mapping values must be unique.")

        return value

    return validator


CONFIG_SCHEMA = select.SELECT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(TuyaSelect),
        cv.GenerateID(CONF_TUYA_ID): cv.use_id(Tuya),
        cv.Required(CONF_SELECT_DATAPOINT): cv.uint8_t,
        cv.Required(CONF_OPTIONS): ensure_option_map(),
        cv.Optional(CONF_OPTIMISTIC, default=False): cv.boolean,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    options_map = config[CONF_OPTIONS]
    await select.register_select(var, config, options=list(options_map.keys()))
    cg.add(var.set_select_mappings(list(options_map.values())))
    parent = await cg.get_variable(config[CONF_TUYA_ID])
    cg.add(var.set_tuya_parent(parent))
    cg.add(var.set_select_id(config[CONF_SELECT_DATAPOINT]))
    cg.add(var.set_optimistic(config[CONF_OPTIMISTIC]))
