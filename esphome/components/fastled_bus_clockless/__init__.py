from esphome import pins
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import fastled_bus
import esphome.components.fastled_clockless.light as fastled_clockless_light
from esphome.const import CONF_CHIPSET, CONF_ID, CONF_NUM_CHIPS, CONF_PIN

CODEOWNERS = ["@mabels"]
MULTI_CONF = True

CONFIG_SCHEMA = cv.All(
    fastled_bus.CONFIG_BUS_SCHEMA.extend(
        {
            cv.GenerateID(CONF_ID): cv.declare_id(fastled_bus.FastLEDBus),
            cv.Required(CONF_CHIPSET): cv.one_of(
                *fastled_clockless_light.CHIPSETS, upper=True
            ),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_number,
        }
    ),
    fastled_bus.REQUIRED_FRAMEWORK,
)


async def to_code(config):
    var = cg.new_Pvariable(
        config[CONF_ID], config[fastled_bus.CONF_CHIP_CHANNELS], config[CONF_NUM_CHIPS]
    )
    await cg.register_component(var, config)

    fastled_bus.new_fastled_bus(var, config)

    template_args = cg.TemplateArguments(
        cg.RawExpression(config[CONF_CHIPSET]), config[CONF_PIN]
    )
    cg.add(
        var.set_controller(
            cg.RawExpression(
                f"fastled_bus::CLEDControllerFactory::create{template_args}()"
            )
        )
    )
