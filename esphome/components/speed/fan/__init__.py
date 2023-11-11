import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import fan, output
from esphome.components.fan import FAN_DIRECTION_ENUM, FanDirection
from esphome.const import (CONF_DIRECTION,  CONF_PRESET_MODES,
                           CONF_DIRECTION_OUTPUT, CONF_NAME, CONF_OSCILLATING,
                           CONF_OSCILLATION_OUTPUT, CONF_OUTPUT,
                           CONF_OUTPUT_ID, CONF_SPEED, CONF_SPEED_COUNT)

from .. import speed_ns

SpeedFan = speed_ns.class_("SpeedFan", cg.Component, fan.Fan)

_PRESET_MODES_SCHEMA = cv.All(
    cv.ensure_list(
        cv.All({
            cv.Required(CONF_NAME): cv.string_strict,
            cv.Optional(CONF_SPEED): cv.int_range(1),
            cv.Optional(CONF_OSCILLATING): cv.boolean,
            cv.Optional(CONF_DIRECTION): cv.enum(FAN_DIRECTION_ENUM, upper=True),
        },
            cv.has_at_least_one_key(CONF_SPEED, CONF_OSCILLATING, CONF_DIRECTION),
        )
    ),
    cv.Length(min=1),
)


def _validate_preset_modes(value):
    # Check against defined schema
    value = _PRESET_MODES_SCHEMA(value)

    # Ensure preset names are unique
    errors = []
    names = set()
    for i, preset in enumerate(value):
        name = preset[CONF_NAME]
        # If name does not exist yet add it
        if not name in names:
            names.add(name)
            continue

        # Otherwise it's an error
        errors.append(
            cv.Invalid(
                f"Found duplicate preset name '{name}'. Presets must have unique names.",
                [i],
            )
        )

    if errors:
        raise cv.MultipleInvalid(errors)

    return value


CONFIG_SCHEMA = fan.FAN_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(SpeedFan),
        cv.Required(CONF_OUTPUT): cv.use_id(output.FloatOutput),
        cv.Optional(CONF_OSCILLATION_OUTPUT): cv.use_id(output.BinaryOutput),
        cv.Optional(CONF_DIRECTION_OUTPUT): cv.use_id(output.BinaryOutput),
        cv.Optional(CONF_SPEED): cv.invalid(
            "Configuring individual speeds is deprecated."
        ),
        cv.Optional(CONF_SPEED_COUNT, default=100): cv.int_range(min=1),
        cv.Optional(CONF_PRESET_MODES): _validate_preset_modes,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    output_ = await cg.get_variable(config[CONF_OUTPUT])
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID], output_, config[CONF_SPEED_COUNT])
    await cg.register_component(var, config)
    await fan.register_fan(var, config)

    if CONF_OSCILLATION_OUTPUT in config:
        oscillation_output = await cg.get_variable(config[CONF_OSCILLATION_OUTPUT])
        cg.add(var.set_oscillating(oscillation_output))

    if CONF_DIRECTION_OUTPUT in config:
        direction_output = await cg.get_variable(config[CONF_DIRECTION_OUTPUT])
        cg.add(var.set_direction(direction_output))

    for preset in config[CONF_PRESET_MODES]:
        speed = cg.optional.template(int)(preset.get(CONF_SPEED))
        oscillating = cg.optional.template(bool)(preset.get(CONF_OSCILLATING))
        direction = cg.optional.template(FanDirection)(preset.get(CONF_DIRECTION))
        
        cg.add(var.add_preset_mode(preset[CONF_NAME], speed, oscillating, direction))
        
