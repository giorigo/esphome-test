import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID, CONF_INVERTED, CONF_RESOLUTION

CODEOWNERS = ["@ellull"]

DEPENDENCIES = ["i2c"]

CONF_I2C_ADDR = 0x4C

CONF_PWM = "pwm"
CONF_DIVIDER = "divider"
CONF_DAC = "dac"
CONF_CONVERSION_RATE = "conversion_rate"

emc2101_ns = cg.esphome_ns.namespace("emc2101")
Emc2101Component = emc2101_ns.class_("Emc2101Component", cg.Component, i2c.I2CDevice)

Emc2101DACConversionRate = emc2101_ns.enum("Emc2101DACConversionRate")
CONVERSIONS_PER_SECOND = {
    "1/16": Emc2101DACConversionRate.Emc2101_DAC_1_EVERY_16S,
    "1/8": Emc2101DACConversionRate.Emc2101_DAC_1_EVERY_8S,
    "1/4": Emc2101DACConversionRate.Emc2101_DAC_1_EVERY_4S,
    "1/2": Emc2101DACConversionRate.Emc2101_DAC_1_EVERY_2S,
    "1": Emc2101DACConversionRate.Emc2101_DAC_1_EVERY_SECOND,
    "2": Emc2101DACConversionRate.Emc2101_DAC_2_EVERY_SECOND,
    "4": Emc2101DACConversionRate.Emc2101_DAC_4_EVERY_SECOND,
    "8": Emc2101DACConversionRate.Emc2101_DAC_8_EVERY_SECOND,
    "16": Emc2101DACConversionRate.Emc2101_DAC_16_EVERY_SECOND,
    "32": Emc2101DACConversionRate.Emc2101_DAC_32_EVERY_SECOND,
}

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Emc2101Component),
            cv.Optional(CONF_PWM): cv.Schema(
                {
                    cv.Optional(CONF_RESOLUTION, default=0x17): cv.int_range(
                        min=0x00, max=0x1F
                    ),
                    cv.Optional(CONF_DIVIDER, default=0x01): cv.uint8_t,
                }
            ),
            cv.Optional(CONF_DAC): cv.Schema(
                {
                    cv.Optional(
                        CONF_CONVERSION_RATE, default=CONVERSIONS_PER_SECOND["16"]
                    ): cv.enum(CONVERSIONS_PER_SECOND),
                }
            ),
            cv.Optional(CONF_INVERTED, default=False): cv.boolean,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(CONF_I2C_ADDR)),
    cv.has_exactly_one_key(CONF_PWM, CONF_DAC),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_PWM in config:
        cg.add(var.set_dac_mode(False))
        pwm_config = config[CONF_PWM]
        cg.add(var.set_pwm_resolution(pwm_config[CONF_RESOLUTION]))
        cg.add(var.set_pwm_divider(pwm_config[CONF_DIVIDER]))
    if CONF_DAC in config:
        cg.add(var.set_dac_mode(True))
        dac_config = config[CONF_DAC]
        cg.add(var.set_dac_conversion_rate(dac_config[CONF_CONVERSION_RATE]))
    cg.add(var.set_inverted(config[CONF_INVERTED]))
