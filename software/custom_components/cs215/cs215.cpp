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
    std::string request(1, this->address_);
    request += "M!";
    ESP_LOGD(TAG, "Updating CS215 sensor with request: '%s'", request.c_str());
    std::string response = this->bus_->send_command(request);

    ESP_LOGD(TAG, "Received CS215 sensor response: '%s'", response.c_str());

    if (response.length() == 7 && response.substr(response.length() - 3) == "2\r\n") {
        std::string data_str = response.substr(0, 4);
        char* end_ptr;

        // The sensor values will be available in the amount of seconds
        // contained in the the first 4 characters of the response
        unsigned long avail_delay = std::strtoul(data_str.c_str(), &end_ptr, 10);

        // Check if the conversion was successful
        if (end_ptr != data_str.c_str()) {
            avail_delay *= 1000;
            ESP_LOGI(TAG, "CS215 sensor data will be available in %lu ms)", avail_delay);
            this->send_data_timestamp_ = millis() + avail_delay;
        } else {
            ESP_LOGW(TAG, "Failed to convert CS215 sensor data availibity delay");
        }
    } else {
        ESP_LOGW(TAG, "Invalid response format from CS215 sensor");
    }
}
void CS215Component::send_data_() {
    this->send_data_timestamp_ = 0;
    std::string request(1, this->address_);
    request += "D0!";
    ESP_LOGD(TAG, "Requesting sensor to send its data: '%s'", request.c_str());
    std::string response = this->bus_->send_command(request);
    ESP_LOGD(TAG, "Received CS215 sensor response: '%s'", response.c_str());
}

void CS215Component::loop() {
    unsigned long now = millis();
    static int last = now;
    if (this->send_data_timestamp_ && now > this->send_data_timestamp_) {
        send_data_();
    }
}

void CS215Component::dump_config() {
  ESP_LOGCONFIG(TAG, "CS215:");
  LOG_SDI12_DEVICE(this);
}

}  // namespace cs215
}  // namespace esphome
