#include "ds2.h"

namespace esphome {
namespace ds2 {

static const char *const TAG = "sensor.ds2";

float DS2Component::get_setup_priority() const {
    return setup_priority::DATA;
}

void DS2Component::setup() {
    ESP_LOGI(TAG, "setup() DS2 Anemometer @ SDI-12 Address '%c'", this->address_);
}

void DS2Component::update() {
    ESP_LOGI(TAG, "DS2 update() address %c", this->address_);
    std::string request = this->address_ + "R0!";
    std::string response = this->bus_->send_command(request);
    ESP_LOGI(TAG, "Received DS2 sensor response: '%s'", response.c_str());

    // Expected fixed string at the beginning of the response
    std::string expected_prefix = "?R0!0+";

    if (response.rfind(expected_prefix, 0) == 0) {
        // Strip the fixed prefix and prepare float variables
        std::string values_str = response.substr(expected_prefix.length());
        float wind_speed, wind_direction, wind_temperature;

        this->parse_sdi12_values_(values_str, {&wind_speed, &wind_direction, &wind_temperature});

        ESP_LOGI(TAG, "Parsed Wind Speed: %.2f km/h", wind_speed);
        ESP_LOGI(TAG, "Parsed Wind Direction: %.0f degrees", wind_direction);
        ESP_LOGI(TAG, "Parsed Wind Temperature: %.1f °C", wind_temperature);

        if (this->windspeed_sensor_ != nullptr) {
            this->windspeed_sensor_->publish_state(wind_speed);
        }
        if (this->direction_sensor_ != nullptr) {
            this->direction_sensor_->publish_state(wind_direction);
        }
        if (this->temperature_sensor_ != nullptr) {
            this->temperature_sensor_->publish_state(wind_temperature);
        }
    } else {
        ESP_LOGW(TAG, "Response format is incorrect. Expected format to start with '?R0!0+'.");
    }
}

void DS2Component::dump_config() {
  ESP_LOGCONFIG(TAG, "DS2:");
  LOG_SDI12_DEVICE(this);
}

}  // namespace ds2
}  // namespace esphome
