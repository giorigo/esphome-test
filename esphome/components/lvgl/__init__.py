import logging

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components.display import Display
from esphome.const import (
    CONF_BUFFER_SIZE,
    CONF_ID,
    CONF_PAGES,
    CONF_DISPLAY_ID,
    CONF_LAMBDA,
    CONF_AUTO_CLEAR_ENABLED,
    CONF_DISPLAY,
)
from esphome.core import (
    CORE,
    Lambda,
)
from esphome.cpp_generator import RawExpression
from esphome.helpers import write_file_if_changed
from . import defines as df
from . import helpers
from . import lv_validation as lvalid
from . import types as ty
from .label import label_spec
from .lvcode import (
    lv,
    LvContext,
    MainContext,
)

# from .menu import menu_spec
from .obj import obj_spec
from .schemas import (
    obj_schema,
    any_widget_schema,
    WIDGET_TYPES,
)
from .widget import (
    Widget,
    LvScrActType,
    set_obj_properties,
    widget_to_code,
)

DOMAIN = "lvgl"
DEPENDENCIES = ("display",)
AUTO_LOAD = ("key_provider",)
CODEOWNERS = ("@clydebarrow",)
LOGGER = logging.getLogger(__name__)

for widg in (
    label_spec,
    obj_spec,
):
    WIDGET_TYPES[widg.name] = widg

lv_scr_act_spec = LvScrActType()
lv_scr_act = Widget.create(None, "lv_scr_act()", lv_scr_act_spec, {}, parent=None)

WIDGET_SCHEMA = any_widget_schema()


async def add_init_lambda(lv_component, init):
    if init:
        lamb = await cg.process_lambda(Lambda(init), [(ty.lv_disp_t_ptr, "lv_disp")])
        cg.add(lv_component.add_init_lambda(lamb))


lv_defines = {}  # Dict of #defines to provide as build flags


def add_define(macro, value="1"):
    if macro in lv_defines and lv_defines[macro] != value:
        LOGGER.error(
            "Redefinition of %s - was %s now %s", macro, lv_defines[macro], value
        )
    lv_defines[macro] = value


def as_macro(macro, value):
    if value is None:
        return f"#define {macro}"
    return f"#define {macro} {value}"


LV_CONF_FILENAME = "lv_conf.h"
LV_CONF_H_FORMAT = """\
#pragma once
{}
"""


def generate_lv_conf_h():
    definitions = [as_macro(m, v) for m, v in lv_defines.items()]
    definitions.sort()
    return LV_CONF_H_FORMAT.format("\n".join(definitions))


def get_display_list(config):
    result = []
    display_list = config.get(df.CONF_DISPLAYS)
    if isinstance(display_list, list):
        for display in config.get(df.CONF_DISPLAYS, []):
            result.append(display[CONF_DISPLAY_ID])
    else:
        result.append(display_list)
    return result


def warning_checks(config):
    global_config = CORE.config
    displays = get_display_list(config)
    if display_conf := global_config.get(CONF_DISPLAY):
        for display_id in displays:
            display = list(
                filter(lambda c, k=display_id: c[CONF_ID] == k, display_conf)
            )[0]
            if CONF_LAMBDA in display:
                LOGGER.warning(
                    "Using lambda: in display config not recommended with LVGL"
                )
            if display[CONF_AUTO_CLEAR_ENABLED]:
                LOGGER.warning(
                    "Using auto_clear_enabled: true in display config not recommended with LVGL"
                )
    buffer_frac = config[CONF_BUFFER_SIZE]
    if not CORE.is_host and buffer_frac > 0.5 and "psram" not in global_config:
        LOGGER.warning("buffer_size: may need to be reduced without PSRAM")


async def to_code(config):
    warning_checks(config)
    cg.add_library("lvgl/lvgl", "8.4.0")
    CORE.add_define("USE_LVGL")
    # suppress default enabling of extra widgets
    add_define("_LV_KCONFIG_PRESENT")
    # Always enable - lots of things use it.
    add_define("LV_DRAW_COMPLEX", "1")
    add_define("LV_TICK_CUSTOM", "1")
    add_define("LV_TICK_CUSTOM_INCLUDE", '"esphome/components/lvgl/lvgl_hal.h"')
    add_define("LV_TICK_CUSTOM_SYS_TIME_EXPR", "(lv_millis())")
    add_define("LV_MEM_CUSTOM", "1")
    add_define("LV_MEM_CUSTOM_ALLOC", "lv_custom_mem_alloc")
    add_define("LV_MEM_CUSTOM_FREE", "lv_custom_mem_free")
    add_define("LV_MEM_CUSTOM_REALLOC", "lv_custom_mem_realloc")
    add_define("LV_MEM_CUSTOM_INCLUDE", '"esphome/components/lvgl/lvgl_hal.h"')

    add_define("LV_LOG_LEVEL", f"LV_LOG_LEVEL_{config[df.CONF_LOG_LEVEL]}")
    add_define("LV_COLOR_DEPTH", config[df.CONF_COLOR_DEPTH])
    default_font = config[df.CONF_DEFAULT_FONT]
    add_define("LV_FONT_DEFAULT", default_font)
    if lvalid.is_esphome_font(default_font):
        add_define("LV_FONT_CUSTOM_DECLARE", f"LV_FONT_DECLARE(*{default_font})")
    for font in helpers.lv_fonts_used:
        add_define(f"LV_FONT_{font.upper()}")

    if config[df.CONF_COLOR_DEPTH] == 16:
        add_define(
            "LV_COLOR_16_SWAP",
            "1" if config[df.CONF_BYTE_ORDER] == "big_endian" else "0",
        )
    add_define(
        "LV_COLOR_CHROMA_KEY",
        await lvalid.lv_color.process(config[df.CONF_TRANSPARENCY_KEY]),
    )
    CORE.add_build_flag("-Isrc")

    cg.add_global(ty.lvgl_ns.using)
    lv_component = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(lv_component, config)
    Widget.create(config[CONF_ID], lv_component, WIDGET_TYPES[df.CONF_OBJ], config)
    displays = get_display_list(config)
    for display in displays:
        cg.add(lv_component.add_display(await cg.get_variable(display)))
    frac = config[CONF_BUFFER_SIZE]
    if frac >= 0.75:
        frac = 1
    elif frac >= 0.375:
        frac = 2
    elif frac > 0.19:
        frac = 4
    else:
        frac = 8
    cg.add(lv_component.set_buffer_frac(int(frac)))
    cg.add(lv_component.set_full_refresh(config[df.CONF_FULL_REFRESH]))

    with MainContext():
        lv.init()
    cg.new_variable(
        ty.CUSTOM_EVENT, RawExpression("(lv_event_code_t) lv_event_register_id()")
    )

    with LvContext():
        if widgets := config.get(df.CONF_WIDGETS):
            await set_obj_properties(lv_scr_act, config)
            for w in widgets:
                lv_w_type, w_cnfig = next(iter(w.items()))
                await widget_to_code(w_cnfig, lv_w_type, lv_scr_act.obj)
    automation.widgets_completed = True
    await add_init_lambda(lv_component, LvContext.get_code())
    for comp in helpers.lvgl_components_required:
        CORE.add_define(f"LVGL_USES_{comp.upper()}")
    for use in helpers.lv_uses:
        add_define(f"LV_USE_{use.upper()}")
    lv_conf_h_file = CORE.relative_src_path(LV_CONF_FILENAME)
    write_file_if_changed(lv_conf_h_file, generate_lv_conf_h())
    CORE.add_build_flag("-DLV_CONF_H=1")
    CORE.add_build_flag(f'-DLV_CONF_PATH="{LV_CONF_FILENAME}"')


CONFIG_SCHEMA = (
    cv.polling_component_schema("1s")
    .extend(obj_schema("obj"))
    .extend(
        {
            cv.Optional(CONF_ID, default=df.CONF_LVGL_COMPONENT): cv.declare_id(
                ty.LvglComponent
            ),
            cv.GenerateID(df.CONF_DISPLAYS): cv.Any(
                cv.use_id(Display),
                cv.ensure_list(
                    cv.maybe_simple_value(
                        {
                            cv.Required(CONF_DISPLAY_ID): cv.use_id(Display),
                        },
                        key=CONF_DISPLAY_ID,
                    ),
                ),
            ),
            cv.Optional(df.CONF_COLOR_DEPTH, default=16): cv.one_of(16),
            cv.Optional(df.CONF_DEFAULT_FONT, default="montserrat_14"): lvalid.lv_font,
            cv.Optional(df.CONF_FULL_REFRESH, default=False): cv.boolean,
            cv.Optional(CONF_BUFFER_SIZE, default="100%"): cv.percentage,
            cv.Optional(df.CONF_LOG_LEVEL, default="WARN"): cv.one_of(
                *df.LOG_LEVELS, upper=True
            ),
            cv.Optional(df.CONF_BYTE_ORDER, default="big_endian"): cv.one_of(
                "big_endian", "little_endian"
            ),
            cv.Optional(df.CONF_WIDGETS): cv.ensure_list(WIDGET_SCHEMA),
            cv.Optional(df.CONF_TRANSPARENCY_KEY, default=0x000400): lvalid.lv_color,
        }
    )
).add_extra(cv.has_at_least_one_key(CONF_PAGES, df.CONF_WIDGETS))
