import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.final_validate as fv
from esphome import pins
from esphome.const import (
    CONF_ID,
    CONF_NUMBER,
    CONF_RX_PIN,
    CONF_TX_PIN,
    CONF_ENABLE_PIN,
    CONF_SCAN,
    CONF_ADDRESS,
)
from esphome.core import coroutine_with_priority, CORE

CONF_SDI12_ID = "sdi12_id"

CODEOWNERS = ["@fraxinas"]
sdi12_ns = cg.esphome_ns.namespace("sdi12")
SDI12Bus = sdi12_ns.class_("SDI12Bus", cg.Component)
SDI12Device = sdi12_ns.class_("SDI12Device")

MULTI_CONF = True

def _bus_declare_type(value):
    return cv.declare_id(SDI12Bus)(value)

def validate_rx_pin(value):
    value = pins.internal_gpio_input_pin_schema(value)
    if CORE.is_esp8266 and value[CONF_NUMBER] >= 16:
        raise cv.Invalid("Pins GPIO16 and GPIO17 cannot be used as RX pins on ESP8266.")
    return value

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): _bus_declare_type,
            cv.Optional(CONF_TX_PIN): pins.internal_gpio_output_pin_schema,
            cv.Optional(CONF_RX_PIN): validate_rx_pin,
            cv.Optional(CONF_ENABLE_PIN): pins.internal_gpio_output_pin_schema,
            cv.Optional(CONF_SCAN, default=True): cv.boolean,
        }
    ).extend(cv.COMPONENT_SCHEMA),
)

@coroutine_with_priority(1.0)
async def to_code(config):
    cg.add_global(sdi12_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    tx_pin = await cg.gpio_pin_expression(config[CONF_TX_PIN])
    cg.add(var.set_tx_pin(tx_pin))

    rx_pin = await cg.gpio_pin_expression(config[CONF_RX_PIN])
    cg.add(var.set_rx_pin(rx_pin))

    oe_pin = await cg.gpio_pin_expression(config[CONF_ENABLE_PIN])
    cg.add(var.set_oe_pin(oe_pin))

    cg.add(var.set_scan(config[CONF_SCAN]))

def sdi12_device_schema(default_address):
    """Create a schema for an SDI-12 device.

    :param default_address: The default address of the SDI-12 device, can be None to represent
      a required option.
    :return: The SDI-12 device schema, `extend` this in your config schema.
    """
    schema = {
        cv.GenerateID(CONF_SDI12_ID): cv.use_id(SDI12Bus),
    }
    if default_address is None:
        schema[cv.Required(CONF_ADDRESS)] = cv.uint8_t
    else:
        schema[cv.Optional(CONF_ADDRESS, default=default_address)] = cv.uint8_t
    return cv.Schema(schema)


async def register_sdi12_device(var, config):
    """Register an SDI-12 device with the given config.

    Sets the SDI-12 bus to use and the SDI-12 address.

    This is a coroutine, you need to await it with a 'yield' expression!
    """
    parent = await cg.get_variable(config[CONF_SDI12_ID])
    cg.add(var.set_sdi12_bus(parent))
    cg.add(var.set_sdi12_address(config[CONF_ADDRESS]))


def final_validate_device_schema(
    name: str,
):
    hub_schema = {}

    return cv.Schema(
        {cv.Required(CONF_SDI12_ID): fv.id_declaration_match_schema(hub_schema)},
        extra=cv.ALLOW_EXTRA,
    )

