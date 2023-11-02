import logging
import esphome.codegen as cg
import esphome.config_validation as cv
from .. import sdi12
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_WIND_SPEED,
    DEVICE_CLASS_WIND_SPEED,
    STATE_CLASS_MEASUREMENT,
    UNIT_KILOMETER_PER_HOUR,
)

DEPENDENCIES = ["sdi12"]

_LOGGER = logging.getLogger(__name__)

ds2_ns = cg.esphome_ns.namespace("ds2")

DS2Component = ds2_ns.class_(
    "DS2Component", cg.PollingComponent, sdi12.SDI12Device
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DS2Component),
            cv.Optional(CONF_WIND_SPEED): sensor.sensor_schema(
                unit_of_measurement=UNIT_KILOMETER_PER_HOUR,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_WIND_SPEED,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(sdi12.sdi12_device_schema('5'))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sdi12.register_sdi12_device(var, config)

    if CONF_WIND_SPEED in config:
        conf = config[CONF_WIND_SPEED]
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_windspeed_sensor(sens))

_LOGGER.info('DS2 INIT')