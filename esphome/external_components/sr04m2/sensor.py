import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import (
    CONF_ID,
    CONF_UPDATE_INTERVAL,
    DEVICE_CLASS_DISTANCE,
    UNIT_CENTIMETER,
)

# Define dependencies
DEPENDENCIES = ["uart"]

# Define the namespace for our component
sr04m2_ns = cg.esphome_ns.namespace("sr04m2")
SR04M2Sensor = sr04m2_ns.class_(
    "SR04M2Sensor", sensor.Sensor, cg.PollingComponent, uart.UARTDevice
)

# Configuration schema for the sr04m2 platform
CONFIG_SCHEMA = (
    sensor.sensor_schema(
        SR04M2Sensor,
        unit_of_measurement=UNIT_CENTIMETER,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_DISTANCE,
    )
    .extend({
        cv.Optional(CONF_UPDATE_INTERVAL, default="5s"): cv.update_interval,
    })
    .extend(uart.UART_DEVICE_SCHEMA)
)

# Registration function for the component
async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)