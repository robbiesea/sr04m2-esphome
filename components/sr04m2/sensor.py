import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import (
    UNIT_CENTIMETER,
    DEVICE_CLASS_DISTANCE,
    STATE_CLASS_MEASUREMENT,
)

DEPENDENCIES = ["uart"]

sr04m2_ns = cg.esphome_ns.namespace("sr04m2")
SR04M2Sensor = sr04m2_ns.class_("SR04M2Sensor", sensor.Sensor, cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_CENTIMETER,
    accuracy_decimals=1,
    device_class=DEVICE_CLASS_DISTANCE,
    state_class=STATE_CLASS_MEASUREMENT,
).extend({
    cv.GenerateID(): cv.declare_id(SR04M2Sensor),
    cv.Required("uart_id"): cv.use_id(uart.UARTComponent),
}).extend(cv.polling_component_schema("1s"))

async def to_code(config):
    var = cg.new_Pvariable(config[cv.GenerateID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
    await uart.register_uart_device(var, config)
