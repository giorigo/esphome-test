import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, esp32_ble_tracker
from esphome.const import CONF_HUMIDITY, CONF_MAC_ADDRESS, CONF_TEMPERATURE, \
    CONF_PRESSURE, CONF_ACCELERATION_X, CONF_ACCELERATION_Y, CONF_ACCELERATION_Z, \
    CONF_BATTERY_VOLTAGE, CONF_TX_POWER, CONF_MEASUREMENT_SEQUENCE_NUMBER, \
    CONF_MOVEMENT_COUNTER, UNIT_CELSIUS, ICON_THERMOMETER, UNIT_PERCENT, UNIT_VOLT, \
    UNIT_HECTOPASCAL, UNIT_G, UNIT_DECIBEL_MILLIWATT, UNIT_EMPTY, ICON_WATER_PERCENT, \
    ICON_BATTERY, ICON_GAUGE, ICON_ACCELERTATION_X, ICON_ACCELERTATION_Y, \
    ICON_ACCELERTATION_Z, ICON_SIGNAL, CONF_ID

DEPENDENCIES = ['esp32_ble_tracker']
AUTO_LOAD = ['ruuvi_ble']

ruuvitag_ns = cg.esphome_ns.namespace('ruuvitag')
RuuviTag = ruuvitag_ns.class_(
    'RuuviTag', esp32_ble_tracker.ESPBTDeviceListener, cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(RuuviTag),
    cv.Required(CONF_MAC_ADDRESS): cv.mac_address,
    cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(UNIT_CELSIUS, ICON_THERMOMETER, 2),
    cv.Optional(CONF_HUMIDITY): sensor.sensor_schema(UNIT_PERCENT, ICON_WATER_PERCENT, 2),
    cv.Optional(CONF_PRESSURE): sensor.sensor_schema(UNIT_HECTOPASCAL, ICON_GAUGE, 2),
    cv.Optional(CONF_ACCELERATION_X): sensor.sensor_schema(UNIT_G, ICON_ACCELERTATION_X, 3),
    cv.Optional(CONF_ACCELERATION_Y): sensor.sensor_schema(UNIT_G, ICON_ACCELERTATION_Y, 3),
    cv.Optional(CONF_ACCELERATION_Z): sensor.sensor_schema(UNIT_G, ICON_ACCELERTATION_Z, 3),
    cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(UNIT_VOLT, ICON_BATTERY, 3),
    cv.Optional(CONF_TX_POWER): sensor.sensor_schema(UNIT_DECIBEL_MILLIWATT, ICON_SIGNAL, 0),
    cv.Optional(CONF_MOVEMENT_COUNTER): sensor.sensor_schema(UNIT_EMPTY, ICON_GAUGE, 0),
    cv.Optional(CONF_MEASUREMENT_SEQUENCE_NUMBER): sensor.sensor_schema(UNIT_EMPTY, ICON_GAUGE, 0),
}).extend(esp32_ble_tracker.ESP_BLE_DEVICE_SCHEMA).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield esp32_ble_tracker.register_ble_device(var, config)

    cg.add(var.set_address(config[CONF_MAC_ADDRESS].as_hex))

    if CONF_TEMPERATURE in config:
        sens = yield sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(var.set_temperature(sens))
    if CONF_HUMIDITY in config:
        sens = yield sensor.new_sensor(config[CONF_HUMIDITY])
        cg.add(var.set_humidity(sens))
    if CONF_PRESSURE in config:
        sens = yield sensor.new_sensor(config[CONF_PRESSURE])
        cg.add(var.set_pressure(sens))
    if CONF_ACCELERATION_X in config:
        sens = yield sensor.new_sensor(config[CONF_ACCELERATION_X])
        cg.add(var.set_acceleration_x(sens))
    if CONF_ACCELERATION_Y in config:
        sens = yield sensor.new_sensor(config[CONF_ACCELERATION_Y])
        cg.add(var.set_acceleration_y(sens))
    if CONF_ACCELERATION_Z in config:
        sens = yield sensor.new_sensor(config[CONF_ACCELERATION_Z])
        cg.add(var.set_acceleration_z(sens))
    if CONF_BATTERY_VOLTAGE in config:
        sens = yield sensor.new_sensor(config[CONF_BATTERY_VOLTAGE])
        cg.add(var.set_battery_voltage(sens))
    if CONF_TX_POWER in config:
        sens = yield sensor.new_sensor(config[CONF_TX_POWER])
        cg.add(var.set_tx_power(sens))
    if CONF_MOVEMENT_COUNTER in config:
        sens = yield sensor.new_sensor(config[CONF_MOVEMENT_COUNTER])
        cg.add(var.set_movement_counter(sens))
    if CONF_MEASUREMENT_SEQUENCE_NUMBER in config:
        sens = yield sensor.new_sensor(config[CONF_MEASUREMENT_SEQUENCE_NUMBER])
        cg.add(var.set_measurement_sequence_number(sens))
