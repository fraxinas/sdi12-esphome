#include <vector>
#include <sstream>
#include <string>
#include <iomanip>
#include "sdi12.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sdi12 {

static const char *const TAG = "sdi12";

void SDI12Device::set_sdi12_address(std::string address) {
    this->address_ = address.c_str()[0];
    ESP_LOGI(TAG, "Set SDI12 Address '%c'", this->address_);
}

// Function to tokenize and parse the SDI-12 response containing a variable number of values
void SDI12Device::parse_sdi12_values_(const std::string &response, std::vector<float*> values) {
    std::istringstream iss(response);
    std::string token;
    size_t index = 0;

    while (std::getline(iss, token, '+') && index < values.size()) {
        // If this is the last token, trim line endings
        if (index == values.size() - 1) {
            size_t pos = token.find_last_of("\r\n");
            if (pos != std::string::npos) {
                token = token.substr(0, pos);
            }
        }
        if (!token.empty()) {
            std::istringstream converter(token);
            converter >> *values[index];
            if (converter.fail()) {
                *values[index] = NAN; // Indicate a parsing error
            }
            ++index;
        }
    }
}


void SDI12Bus::setup() {
  ESP_LOGD(TAG, "Setting up SDI-12 bus...");
  this->SDI12_ = SDI12(this->rx_pin_->get_pin(), this->tx_pin_->get_pin(), this->oe_pin_->get_pin());

  if (this->scan_) {
    this->init_scan_();
  }

  initialized_ = true;
}

void SDI12Bus::dump_config() {
  ESP_LOGCONFIG(TAG, "SDI-12 Bus:");
  LOG_PIN("  RX Pin: ", this->rx_pin_);
  LOG_PIN("  TX Pin: ", this->tx_pin_);
  LOG_PIN("  OE Pin: ", this->oe_pin_);
}

std::string SDI12Bus::send_command(std::string command) {
  if (!initialized_) {
    ESP_LOGW(TAG, "SDI12 bus not initialized!");
    return "";
  }

  ESP_LOGV(TAG, "Sending command '%s' on SDI-12 bus...", command.c_str());

  this->SDI12_.begin();

  this->SDI12_.sendCommand(command.c_str(), 100);
  this->SDI12_.clearBuffer();

  delay(30);

  std::string buffer;
  while (this->SDI12_.available()) {
    char c = static_cast<char>(this->SDI12_.read());
    buffer += c;
    delay(10);  // 1 character ~ 7.5ms.
  }

  ESP_LOGV(TAG, "SDI-12 Received Response: %s", buffer.c_str());

  this->SDI12_.end();
  return buffer;
}

char SDI12Bus::read_char() {
  if (!initialized_) {
    ESP_LOGW(TAG, "SDI12 bus not initialized!");
    return '\0';
  }

  ESP_LOGV(TAG, "Reading from SDI-12 bus...");

  return '\0';
}

boolean SDI12Bus::check_device_active_(char i) {
  String command = String(i) + "!";

  for (int j = 1; j <= 3; j++) {
    unsigned long startTime = millis();
    this->SDI12_.sendCommand(command, 20);
    this->SDI12_.clearBuffer();
    delay(20);
    if (this->SDI12_.available()) {
      return true;
    }
  }
  this->SDI12_.clearBuffer();
  return false;
}

std::string SDI12Bus::get_device_info_(char i)
{
  String command = String(i) + "I!";
  this->SDI12_.sendCommand(command);
  this->SDI12_.clearBuffer();
  delay(30);

  std::string buffer = "";
  while (this->SDI12_.available()) {
    char c = static_cast<char>(this->SDI12_.read());
    buffer += c;
    delay(10);  // 1 character ~ 7.5ms.
  }

  if (!buffer.empty()) {
    ESP_LOGD(TAG, "SDI-12 device %c info: %s", i, buffer.c_str());
  } else {
    ESP_LOGD(TAG, "SDI-12 device %c respond without info.", i);
    return "No device info";
  }
  return buffer;
}

void SDI12Bus::init_scan_() {
  ESP_LOGD(TAG, "Scanning for devices on SDI-12 bus...");

  // Create a vector that contains all addresses that should be scanned
  this->addresses_to_scan_.clear();
  // Add numerical addresses
  for (char i = '0'; i <= '9'; i++)
    this->addresses_to_scan_.push_back(i);
  // Add lowercase alphabetical addresses
  for (char i = 'a'; i <= 'z'; i++)
    this->addresses_to_scan_.push_back(i);
  // Add uppercase alphabetical addresses
  for (char i = 'A'; i <= 'Z'; i++)
    this->addresses_to_scan_.push_back(i);
}

void SDI12Bus::do_scan_() {
  char address = this->addresses_to_scan_.front();
  this->addresses_to_scan_.erase(this->addresses_to_scan_.begin());

  ESP_LOGV(TAG, "Scanning address %c", address);
  this->SDI12_.begin();
  if (this->check_device_active_(address)) {
    std::string device_info = this->get_device_info_(address);
    // Explicitly copy the string before placing it in the vector
    this->scan_results_.emplace_back(address, std::string(device_info));
  }
  this->SDI12_.end();

  if (this->addresses_to_scan_.empty()) {
    ESP_LOGI(TAG, "SDI-12 bus scan finished! Results:");
    this->scan_ = false;

    if (scan_results_.empty()) {
      ESP_LOGW(TAG, "Found no SDI-12 devices!");
    } else {
      for (const auto &s : scan_results_) {
          ESP_LOGI(TAG, "Found SDI-12 device at address %c: %s", s.first, s.second.c_str());
      }
    }
  }
}

void SDI12Bus::loop() {
  if (!this->addresses_to_scan_.empty()) {
    this->do_scan_();
  }
}

}  // namespace sdi12
}  // namespace esphome
