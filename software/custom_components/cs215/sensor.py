import logging
import esphome.codegen as cg
import esphome.config_validation as cv
from .. import sdi12
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_HUMIDITY,
    DEVICE_CLASS_HUMIDITY,
    STATE_CLASS_MEASUREMENT,
    UNIT_PERCENT,
    CONF_TEMPERATURE,
    UNIT_CELSIUS,
)

DEPENDENCIES = ["sdi12"]

_LOGGER = logging.getLogger(__name__)

cs215_ns = cg.esphome_ns.namespace("cs215")

CS215Component = cs215_ns.class_(
    "CS215Component", cg.PollingComponent, sdi12.SDI12Device
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(CS215Component),
            cv.Optional(CONF_HUMIDITY): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
            ),
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(sdi12.sdi12_device_schema(2))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sdi12.register_sdi12_device(var, config)

    if CONF_HUMIDITY in config:
        conf = config[CONF_HUMIDITY]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_humidity_sensor(sens))

    if CONF_TEMPERATURE in config:
        conf = config[CONF_TEMPERATURE]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_temperature_sensor(sens))
