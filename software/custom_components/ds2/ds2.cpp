#include "ds2.h"

namespace esphome {
namespace ds2 {

static const char *const TAG = "sensor.ds2";

float DS2Component::get_setup_priority() const {
    return setup_priority::DATA;
}

void DS2Component::setup() {
    ESP_LOGV(TAG, "setup() DS2 Anemometer @ SDI-11 Address '%c'", this->address_);
}

void DS2Component::update() {
    ESP_LOGV(TAG, "DS2 update()");
}

void DS2Component::dump_config() {
  ESP_LOGCONFIG(TAG, "DS2:");
  LOG_SDI12_DEVICE(this);
}

}  // namespace ds2
}  // namespace esphome
