from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import UNIT_CENTIMETER, DEVICE_CLASS_DISTANCE

DEPENDENCIES = ["uart"]

sr04m2_ns = cg.esphome_ns.namespace('sr04m2')
SR04M2Sensor = sr04m2_ns.class_('SR04M2Sensor', sensor.Sensor, cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_CENTIMETER,
    icon="mdi:waves",
    accuracy_decimals=1,
    device_class=DEVICE_CLASS_DISTANCE
).extend({
    cv.GenerateID(): cv.declare_id(SR04M2Sensor),
}).extend(cv.COMPONENT_SCHEMA).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[cv.ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)
    yield sensor.register_sensor(var, config)
