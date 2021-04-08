import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cover, rs485
from esphome.const import CONF_ADDRESS, CONF_ID

CODEOWNERS = ["@loongyh"]

AUTO_LOAD = ["rs485"]

chenyang_ns = cg.esphome_ns.namespace("chenyang")
Chenyang = chenyang_ns.class_(
    "Chenyang", cover.Cover, cg.PollingComponent, rs485.RS485Device
)

CONFIG_SCHEMA = (
    cover.COVER_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Chenyang),
            cv.Optional(CONF_ADDRESS): cv.hex_uint8_t,
        }
    )
    .extend(cv.polling_component_schema("1s"))
    .extend(rs485.RS485_DEVICE_SCHEMA)
)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield rs485.register_rs485_device(var, config)
    yield cover.register_cover(var, config)

    if CONF_ADDRESS in config:
        address = config[CONF_ADDRESS]
        cg.add(var.set_address(address))
