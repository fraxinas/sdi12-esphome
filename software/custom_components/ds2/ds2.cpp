#include <sstream>
#include <string>
#include <iomanip>
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
    std::string request = this->address_ + "R0!";
    std::string response = this->bus_->send_command(request);

    ESP_LOGV(TAG, "Received DS2 sensor response: '%s'", response.c_str());

    // Expected fixed format at the beginning of the response
    std::string expected_prefix = "?R0!0+";

    if (response.rfind(expected_prefix, 0) == 0) {
        // Strip the fixed prefix
        std::string values_str = response.substr(expected_prefix.length());

        std::istringstream iss(values_str);
        std::string token;
        float wind_speed = 0;
        float wind_direction = 0;
        float wind_temperature = 0;

        // Wind Speed
        if (std::getline(iss, token, '+') && !token.empty()) {
            std::istringstream converter(token);
            converter >> wind_speed;
            if (converter.fail() || !converter.eof()) {
                ESP_LOGW(TAG, "Failed to parse wind speed");
            } else {
                ESP_LOGV(TAG, "Parsed Wind Speed: %.2f km/h", wind_speed);
            }
        }

        // Wind Direction
        if (std::getline(iss, token, '+') && !token.empty()) {
            std::istringstream converter(token);
            converter >> wind_direction;
            if (converter.fail() || !converter.eof()) {
                ESP_LOGW(TAG, "Failed to parse wind direction");
            } else {
                ESP_LOGV(TAG, "Parsed Wind Direction: %.0f degrees", wind_direction);
            }
        }

        // Wind Temperature
        if (std::getline(iss, token, '+') && !token.empty()) {
            std::istringstream converter(token);
            converter >> wind_temperature;
            if (converter.fail() || !converter.eof()) {
                ESP_LOGW(TAG, "Failed to parse wind temperature");
            } else {
                ESP_LOGV(TAG, "Parsed Wind Temperature: %.1f °C", wind_temperature);
            }
        }

        // Publish the values even if one of the above fails, as requested.
        if (this->windspeed_sensor_ != nullptr) {
            this->windspeed_sensor_->publish_state(wind_speed);
        }
        // Repeat for wind_direction_sensor_ and wind_temperature_sensor_
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
