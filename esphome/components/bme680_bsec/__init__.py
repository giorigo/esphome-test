import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID

CODEOWNERS = ['@trvrnrth']
DEPENDENCIES = ['i2c']
AUTO_LOAD = ['sensor', 'text_sensor']
MULTI_CONF = True

CONF_BME680_BSEC_ID = 'bme680_bsec_id'
CONF_TEMPERATURE_OFFSET = 'temperature_offset'
CONF_IAQ_MODE = 'iaq_mode'
CONF_STATE_SAVE_INTERVAL = 'state_save_interval'

bme680_bsec_ns = cg.esphome_ns.namespace('bme680_bsec')

BME680IAQMode = bme680_bsec_ns.enum('BME680IAQMode')
IAQ_MODE_OPTIONS = {
    'STATIC': BME680IAQMode.BME680_IAQ_MODE_STATIC,
    'MOBILE': BME680IAQMode.BME680_IAQ_MODE_MOBILE,
}

BME680BSECComponent = bme680_bsec_ns.class_('BME680BSECComponent', cg.Component, i2c.I2CDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(BME680BSECComponent),
    cv.Optional(CONF_TEMPERATURE_OFFSET, default=0): cv.temperature,
    cv.Optional(CONF_IAQ_MODE, default='STATIC'):
        cv.enum(IAQ_MODE_OPTIONS, upper=True),
    cv.Optional(CONF_STATE_SAVE_INTERVAL, default='6hours'): cv.positive_time_period_minutes,
}).extend(i2c.i2c_device_schema(0x76))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)

    cg.add(var.set_temperature_offset(config[CONF_TEMPERATURE_OFFSET]))
    cg.add(var.set_iaq_mode(config[CONF_IAQ_MODE]))
    cg.add(var.set_state_save_interval(config[CONF_STATE_SAVE_INTERVAL]))

    cg.add_build_flag('-DUSING_BSEC')
    cg.add_library('bsec=https://github.com/trvrnrth/BSEC-Arduino-library', None)
