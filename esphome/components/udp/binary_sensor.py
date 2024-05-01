import esphome.codegen as cg
from esphome.components import binary_sensor
from . import SENSOR_SCHEMA, CONF_UDP_ID, CONF_REMOTE_ID, CONF_PROVIDER
from ...config_validation import All, has_at_least_one_key
from ...const import CONF_ID

AUTO_LOAD = ["udp"]

CONFIG_SCHEMA = All(
    binary_sensor.binary_sensor_schema().extend(SENSOR_SCHEMA),
    has_at_least_one_key(CONF_ID, CONF_REMOTE_ID),
)


async def to_code(config):
    var = await binary_sensor.new_binary_sensor(config)
    comp = await cg.get_variable(config[CONF_UDP_ID])
    remote_id = str(config.get(CONF_REMOTE_ID) or config.get(CONF_ID))
    cg.add(comp.add_remote_binary_sensor(config[CONF_PROVIDER], remote_id, var))
