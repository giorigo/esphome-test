from esphome import automation
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_RANGE_FROM, CONF_RANGE_TO, CONF_STEP, CONF_VALUE

from .automation import action_to_code, update_to_code
from .defines import (
    CONF_CURSOR,
    CONF_DECIMAL_PLACES,
    CONF_DIGITS,
    CONF_MAIN,
    CONF_ROLLOVER,
    CONF_SCROLLBAR,
    CONF_SELECTED,
    CONF_TEXTAREA_PLACEHOLDER,
)
from .label import CONF_LABEL
from .lv_validation import lv_bool, lv_float
from .lvcode import lv
from .textarea import CONF_TEXTAREA
from .types import LvNumber, ObjUpdateAction
from .widget import Widget, WidgetType, get_widgets

CONF_SPINBOX = "spinbox"

lv_spinbox_t = LvNumber("lv_spinbox_t")

SPIN_ACTIONS = (
    "INCREMENT",
    "DECREMENT",
    "STEP_NEXT",
    "STEP_PREV",
    "CLEAR",
)

SPINBOX_SCHEMA = {
    cv.Optional(CONF_VALUE): lv_float,
    cv.Optional(CONF_RANGE_FROM, default=0): cv.float_,
    cv.Optional(CONF_RANGE_TO, default=100): cv.float_,
    cv.Optional(CONF_DIGITS, default=4): cv.int_range(1, 10),
    cv.Optional(CONF_STEP, default=1.0): cv.positive_float,
    cv.Optional(CONF_DECIMAL_PLACES, default=0): cv.int_range(0, 6),
    cv.Optional(CONF_ROLLOVER, default=False): lv_bool,
}

SPINBOX_MODIFY_SCHEMA = {
    cv.Required(CONF_VALUE): lv_float,
}


def validate_spinbox(config):
    max_val = 2**31 - 1
    min_val = -1 - max_val
    scale = 10 ** config[CONF_DECIMAL_PLACES]
    range_from = int(config[CONF_RANGE_FROM] * scale)
    range_to = int(config[CONF_RANGE_TO] * scale)
    step = int(config[CONF_STEP] * scale)
    if (
        range_from > max_val
        or range_from < min_val
        or range_to > max_val
        or range_to < min_val
    ):
        raise cv.Invalid("Range outside allowed limits")
    if step <= 0 or step >= (range_to - range_from) / 2:
        raise cv.Invalid("Invalid step value")
    if config[CONF_DIGITS] <= config[CONF_DECIMAL_PLACES]:
        raise cv.Invalid("Number of digits must exceed number of decimal places")
    return config


class SpinboxType(WidgetType):
    def __init__(self):
        super().__init__(
            CONF_SPINBOX,
            lv_spinbox_t,
            (
                CONF_MAIN,
                CONF_SCROLLBAR,
                CONF_SELECTED,
                CONF_CURSOR,
                CONF_TEXTAREA_PLACEHOLDER,
            ),
            SPINBOX_SCHEMA,
            SPINBOX_MODIFY_SCHEMA,
        )

    async def to_code(self, w: Widget, config):
        if CONF_DIGITS in config:
            digits = config[CONF_DIGITS]
            scale = 10 ** config[CONF_DECIMAL_PLACES]
            range_from = int(config[CONF_RANGE_FROM])
            range_to = int(config[CONF_RANGE_TO])
            step = int(config[CONF_STEP])
            w.scale = scale
            w.step = step
            w.range_to = range_to
            w.range_from = range_from
            lv.spinbox_set_range(w.obj, range_from, range_to)
            w.set_property(CONF_STEP, step * scale)
            w.set_property(CONF_ROLLOVER, config)
            lv.spinbox_set_digit_format(
                w.obj, digits, digits - config[CONF_DECIMAL_PLACES]
            )
        if value := config.get(CONF_VALUE):
            lv.spinbox_set_value(w.obj, await lv_float.process(value))

    def get_uses(self):
        return CONF_TEXTAREA, CONF_LABEL


spinbox_spec = SpinboxType()


@automation.register_action(
    "lvgl.spinbox.increment",
    ObjUpdateAction,
    cv.maybe_simple_value(
        {
            cv.Required(CONF_ID): cv.use_id(lv_spinbox_t),
        },
        key=CONF_ID,
    ),
)
async def spinbox_increment(config, action_id, template_arg, args):
    widgets = await get_widgets(config)

    async def do_increment(w: Widget):
        lv.spinbox_increment(w.obj)

    return await action_to_code(widgets, do_increment, action_id, template_arg, args)


@automation.register_action(
    "lvgl.spinbox.decrement",
    ObjUpdateAction,
    cv.maybe_simple_value(
        {
            cv.Required(CONF_ID): cv.use_id(lv_spinbox_t),
        },
        key=CONF_ID,
    ),
)
async def spinbox_decrement(config, action_id, template_arg, args):
    widgets = await get_widgets(config)

    async def do_increment(w: Widget):
        lv.spinbox_decrement(w.obj)

    return await action_to_code(widgets, do_increment, action_id, template_arg, args)


@automation.register_action(
    "lvgl.spinbox.update",
    ObjUpdateAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(lv_spinbox_t),
            cv.Required(CONF_VALUE): lv_float,
        }
    ),
)
async def spinbox_update_to_code(config, action_id, template_arg, args):
    return await update_to_code(config, action_id, template_arg, args)
