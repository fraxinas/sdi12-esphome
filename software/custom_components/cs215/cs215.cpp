#include <sstream>
#include <string>
#include <iomanip>
#include "cs215.h"

namespace esphome {
namespace cs215 {

static const char *const TAG = "sensor.cs215";

float CS215Component::get_setup_priority() const {
    return setup_priority::DATA;
}

void CS215Component::setup() {
    ESP_LOGI(TAG, "setup() CS215 Humidity Probe @ SDI-12 Address '%c'", this->address_);
}

void CS215Component::update() {
    ESP_LOGI(TAG, "CS215 update() address %c", this->address_);
    std::string request = this->address_ + "M!";
    std::string response = this->bus_->send_command(request);

    ESP_LOGI(TAG, "Received CS215 sensor response: '%s'", response.c_str());
}

void CS215Component::dump_config() {
  ESP_LOGCONFIG(TAG, "CS215:");
  LOG_SDI12_DEVICE(this);
}

}  // namespace cs215
}  // namespace esphome
