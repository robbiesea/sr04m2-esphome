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
        cv.Optional("min_distance", default=2.0): cv.float_,
        cv.Optional("max_distance", default=450.0): cv.float_,
        cv.Optional("timeout", default="1s"): cv.positive_time_period_milliseconds,
    })
    .extend(uart.UART_DEVICE_SCHEMA)
)

# Registration function for the component
async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    
    # Add configuration values
    if "min_distance" in config:
        cg.add(var.set_min_distance(config["min_distance"]))
    if "max_distance" in config:
        cg.add(var.set_max_distance(config["max_distance"]))
    if "timeout" in config:
        cg.add(var.set_timeout(config["timeout"]))