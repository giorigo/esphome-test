import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import core, pins
from esphome.components import display, spi, font
from esphome.components.display import validate_rotation
from esphome.core import CORE, HexInt
from esphome.const import (
    CONF_COLOR_PALETTE,
    CONF_DC_PIN,
    CONF_ID,
    CONF_LAMBDA,
    CONF_MODEL,
    CONF_RAW_DATA_ID,
    CONF_PAGES,
    CONF_RESET_PIN,
    CONF_DIMENSIONS,
    CONF_WIDTH,
    CONF_HEIGHT,
    CONF_ROTATION,
)

DEPENDENCIES = ["spi"]


def AUTO_LOAD():
    if CORE.is_esp32:
        return ["psram"]
    return []


CODEOWNERS = ["@nielsnl68", "@clydebarrow"]

ili9XXX_ns = cg.esphome_ns.namespace("ili9xxx")
ili9XXXSPI = ili9XXX_ns.class_(
    "ILI9XXXDisplay", cg.PollingComponent, spi.SPIDevice, display.DisplayBuffer
)

ILI9XXXColorMode = ili9XXX_ns.enum("ILI9XXXColorMode")
ColorOrder = display.display_ns.enum("ColorMode")

MODELS = {
    "M5STACK": ili9XXX_ns.class_("ILI9XXXM5Stack", ili9XXXSPI),
    "M5CORE": ili9XXX_ns.class_("ILI9XXXM5CORE", ili9XXXSPI),
    "TFT_2.4": ili9XXX_ns.class_("ILI9XXXILI9341", ili9XXXSPI),
    "TFT_2.4R": ili9XXX_ns.class_("ILI9XXXILI9342", ili9XXXSPI),
    "ILI9341": ili9XXX_ns.class_("ILI9XXXILI9341", ili9XXXSPI),
    "ILI9342": ili9XXX_ns.class_("ILI9XXXILI9342", ili9XXXSPI),
    "ILI9481": ili9XXX_ns.class_("ILI9XXXILI9481", ili9XXXSPI),
    "ILI9481-18": ili9XXX_ns.class_("ILI9XXXILI948118", ili9XXXSPI),
    "ILI9486": ili9XXX_ns.class_("ILI9XXXILI9486", ili9XXXSPI),
    "ILI9488": ili9XXX_ns.class_("ILI9XXXILI9488", ili9XXXSPI),
    "ILI9488_A": ili9XXX_ns.class_("ILI9XXXILI9488A", ili9XXXSPI),
    "ST7796": ili9XXX_ns.class_("ILI9XXXST7796", ili9XXXSPI),
    "ST7789V": ili9XXX_ns.class_("ILI9XXXST7789V", ili9XXXSPI),
    "S3BOX": ili9XXX_ns.class_("ILI9XXXS3Box", ili9XXXSPI),
    "S3BOX_LITE": ili9XXX_ns.class_("ILI9XXXS3BoxLite", ili9XXXSPI),
}

COLOR_ORDERS = {
    "RGB": ColorOrder.COLOR_ORDER_RGB,
    "BGR": ColorOrder.COLOR_ORDER_BGR,
}

COLOR_PALETTE = cv.one_of("NONE", "GRAYSCALE", "IMAGE_ADAPTIVE")

CONF_LED_PIN = "led_pin"
CONF_COLOR_PALETTE_IMAGES = "color_palette_images"
CONF_INVERT_DISPLAY = "invert_display"
CONF_MIRROR_X = "mirror_x"
CONF_MIRROR_Y = "mirror_y"
CONF_SWAP_XY = "swap_xy"
CONF_COLOR_ORDER = "color_order"
CONF_OFFSET_HEIGHT = "offset_height"
CONF_OFFSET_WIDTH = "offset_width"


def _validate(config):
    if CONF_DIMENSIONS in config:
        dimensions = config[CONF_DIMENSIONS]
        if isinstance(dimensions, dict):
            config.update(dimensions)
        else:
            (config[CONF_WIDTH], config[CONF_HEIGHT]) = dimensions
            config[CONF_OFFSET_WIDTH] = 0
            config[CONF_OFFSET_HEIGHT] = 0

    if CONF_ROTATION in config:
        rotation = config[CONF_ROTATION]
        config[CONF_ROTATION] = 0
        if isinstance(rotation, dict):
            config.update(rotation)
        elif rotation == 0:
            config[CONF_SWAP_XY] = False
            config[CONF_MIRROR_X] = False
            config[CONF_MIRROR_Y] = False
        elif rotation == 90:
            config[CONF_SWAP_XY] = True
            config[CONF_MIRROR_X] = True
            config[CONF_MIRROR_Y] = False
        elif rotation == 270:
            config[CONF_SWAP_XY] = True
            config[CONF_MIRROR_X] = False
            config[CONF_MIRROR_Y] = True
        elif rotation == 180:
            config[CONF_SWAP_XY] = False
            config[CONF_MIRROR_X] = True
            config[CONF_MIRROR_Y] = True

    if config.get(CONF_COLOR_PALETTE) == "IMAGE_ADAPTIVE" and not config.get(
        CONF_COLOR_PALETTE_IMAGES
    ):
        raise cv.Invalid(
            "Color palette in IMAGE_ADAPTIVE mode requires at least one 'color_palette_images' entry to generate palette"
        )
    if (
        config.get(CONF_COLOR_PALETTE_IMAGES)
        and config.get(CONF_COLOR_PALETTE) != "IMAGE_ADAPTIVE"
    ):
        raise cv.Invalid(
            "Providing color palette images requires palette mode to be 'IMAGE_ADAPTIVE'"
        )
    if CORE.is_esp8266 and config.get(CONF_MODEL) not in [
        "M5STACK",
        "TFT_2.4",
        "TFT_2.4R",
        "ILI9341",
        "ILI9342",
        "ST7789V",
    ]:
        raise cv.Invalid(
            "Provided model can't run on ESP8266. Use an ESP32 with PSRAM onboard"
        )
    return config


CONFIG_SCHEMA = cv.All(
    font.validate_pillow_installed,
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ili9XXXSPI),
            cv.Required(CONF_MODEL): cv.enum(MODELS, upper=True, space="_"),
            cv.Optional(CONF_DIMENSIONS): cv.Any(
                cv.dimensions,
                cv.Schema(
                    {
                        cv.Required(CONF_WIDTH): cv.int_,
                        cv.Required(CONF_HEIGHT): cv.int_,
                        cv.Optional(CONF_OFFSET_HEIGHT, default=0): cv.int_,
                        cv.Optional(CONF_OFFSET_WIDTH, default=0): cv.int_,
                    }
                ),
            ),
            cv.Required(CONF_DC_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_LED_PIN): cv.invalid(
                "This property is removed. To use the backlight use proper light component."
            ),
            cv.Optional(CONF_COLOR_PALETTE, default="NONE"): COLOR_PALETTE,
            cv.GenerateID(CONF_RAW_DATA_ID): cv.declare_id(cg.uint8),
            cv.Optional(CONF_COLOR_PALETTE_IMAGES, default=[]): cv.ensure_list(
                cv.file_
            ),
            cv.Optional(CONF_INVERT_DISPLAY): cv.boolean,
            cv.Optional(CONF_COLOR_ORDER): cv.one_of(*COLOR_ORDERS.keys(), upper=True),
            cv.Optional(CONF_ROTATION): cv.Any(
                validate_rotation,
                cv.Schema(
                    {
                        cv.Optional(CONF_SWAP_XY, default=False): cv.boolean,
                        cv.Optional(CONF_MIRROR_X, default=False): cv.boolean,
                        cv.Optional(CONF_MIRROR_Y, default=False): cv.boolean,
                    }
                ),
            ),
        }
    )
    .extend(cv.polling_component_schema("1s"))
    .extend(spi.spi_device_schema(False, "40MHz")),
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),
    _validate,
)


async def to_code(config):
    rhs = MODELS[config[CONF_MODEL]].new()
    var = cg.Pvariable(config[CONF_ID], rhs)

    await cg.register_component(var, config)
    await display.register_display(var, config)
    await spi.register_spi_device(var, config)
    dc = await cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc))
    if CONF_COLOR_ORDER in config:
        cg.add(var.set_color_order(COLOR_ORDERS[config[CONF_COLOR_ORDER]]))
    if CONF_SWAP_XY in config:
        cg.add(var.set_swap_xy(config[CONF_SWAP_XY]))
    if CONF_MIRROR_X in config:
        cg.add(var.set_mirror_x(config[CONF_MIRROR_X]))
    if CONF_MIRROR_Y in config:
        cg.add(var.set_mirror_y(config[CONF_MIRROR_Y]))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    if CONF_RESET_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(reset))

    if CONF_DIMENSIONS in config:
        cg.add(var.set_dimensions(config[CONF_WIDTH], config[CONF_HEIGHT]))
        cg.add(var.set_offsets(config[CONF_OFFSET_WIDTH], config[CONF_OFFSET_HEIGHT]))

    rhs = None
    if config[CONF_COLOR_PALETTE] == "GRAYSCALE":
        cg.add(var.set_buffer_color_mode(ILI9XXXColorMode.BITS_8_INDEXED))
        rhs = []
        for x in range(256):
            rhs.extend([HexInt(x), HexInt(x), HexInt(x)])
    elif config[CONF_COLOR_PALETTE] == "IMAGE_ADAPTIVE":
        cg.add(var.set_buffer_color_mode(ILI9XXXColorMode.BITS_8_INDEXED))
        from PIL import Image

        def load_image(filename):
            path = CORE.relative_config_path(filename)
            try:
                return Image.open(path)
            except Exception as e:
                raise core.EsphomeError(f"Could not load image file {path}: {e}")

        # make a wide horizontal combined image.
        images = [load_image(x) for x in config[CONF_COLOR_PALETTE_IMAGES]]
        total_width = sum(i.width for i in images)
        max_height = max(i.height for i in images)

        ref_image = Image.new("RGB", (total_width, max_height))
        x = 0
        for i in images:
            ref_image.paste(i, (x, 0))
            x = x + i.width

        # reduce the colors on combined image to 256.
        converted = ref_image.convert("P", palette=Image.Palette.ADAPTIVE, colors=256)
        # if you want to verify how the images look use
        # ref_image.save("ref_in.png")
        # converted.save("ref_out.png")
        palette = converted.getpalette()
        assert len(palette) == 256 * 3
        rhs = palette
    else:
        cg.add(var.set_buffer_color_mode(ILI9XXXColorMode.BITS_16))

    if rhs is not None:
        prog_arr = cg.progmem_array(config[CONF_RAW_DATA_ID], rhs)
        cg.add(var.set_palette(prog_arr))

    if CONF_INVERT_DISPLAY in config:
        cg.add(var.invert_display(config[CONF_INVERT_DISPLAY]))
