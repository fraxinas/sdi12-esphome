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
    if (this->bus_->is_scanning())
        return;

    std::string request(1, this->address_);
    request += "R0!";
    std::string response = this->bus_->send_command(request);
    ESP_LOGI(TAG, "Received DS2 sensor response: '%s'", response.c_str());

    // Expected fixed string at the beginning of the response
    std::string expected_prefix(1, this->address_);
    expected_prefix += "+";

    if (response.rfind(expected_prefix, 0) == 0) {
        // Strip the fixed prefix and prepare float variables
        std::string values_str = response.substr(expected_prefix.length());
        float wind_speed, wind_direction, wind_temperature;

        this->parse_sdi12_values_(values_str, {&wind_speed, &wind_direction, &wind_temperature});

        ESP_LOGI(TAG, "Parsed Wind Speed: %.3f km/h", wind_speed);
        ESP_LOGI(TAG, "Parsed Wind Direction: %.0f degrees", wind_direction);
        ESP_LOGI(TAG, "Parsed Wind Temperature: %.3f °C", wind_temperature);

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
        ESP_LOGW(TAG, "Response format is incorrect.");
    }
}

void DS2Component::dump_config() {
  ESP_LOGCONFIG(TAG, "DS2:");
  LOG_SDI12_DEVICE(this);
}

}  // namespace ds2
}  // namespace esphome
