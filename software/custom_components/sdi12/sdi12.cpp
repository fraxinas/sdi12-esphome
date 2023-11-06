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
    this->sdi12_scan();
  }

  initialized_ = true;
}

void SDI12Bus::dump_config() {
  ESP_LOGCONFIG(TAG, "SDI-12 Bus:");
  LOG_PIN("  RX Pin: ", this->rx_pin_);
  LOG_PIN("  TX Pin: ", this->tx_pin_);
  LOG_PIN("  OE Pin: ", this->oe_pin_);

  if (this->scan_) {
    ESP_LOGI(TAG, "Results from SDI-12 bus scan:");
    if (scan_results_.empty()) {
      ESP_LOGI(TAG, "Found no SDI-12 devices!");
    } else {
      for (const auto &s : scan_results_) {
        if (s.second) {
          ESP_LOGI(TAG, "Found SDI-12 device at address %c", s.first);
        } else {
          ESP_LOGE(TAG, "Unknown error at address %c", s.first);
        }
      }
    }
  }
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

boolean SDI12Bus::checkActive_(char i) {
  String myCommand = String(i) + "!";

  for (int j = 1; j <= 3; j++) {
    unsigned long startTime = millis();
    this->SDI12_.sendCommand(myCommand);
    this->SDI12_.clearBuffer();
    ESP_LOGD(TAG, "startTime=%lu millis=%lu diff=%lu ms", startTime, millis(), millis() - startTime);

    delay(30);

    if (this->SDI12_.available()) {
      return true;
    }
    ESP_LOGD(TAG, "address %c unavailable %d", i, j);
  }

  this->SDI12_.clearBuffer();
  return false;
}

void SDI12Bus::printInfo_(char i)
{
  String command = "";
  command += (char)i;
  command += "I!";
  this->SDI12_.sendCommand(command);
  this->SDI12_.clearBuffer();
  delay(30);

  ESP_LOGD(TAG, "Print info on SDI-12 device with address %c...", i);

  std::string buffer;
  while (this->SDI12_.available()) {
    char c = static_cast<char>(this->SDI12_.read());
    buffer += c;
    delay(10);  // 1 character ~ 7.5ms.
  }

  if (!buffer.empty()) {
    ESP_LOGI(TAG, "SDI-12 device %c info: %s", i, buffer.c_str());
  } else {
    ESP_LOGI(TAG, "SDI-12 device %c did not respond with info.", i);
  }
}

void SDI12Bus::sdi12_scan() {
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

void SDI12Bus::loop() {
  if (!this->addresses_to_scan_.empty()) {
    char address = this->addresses_to_scan_.front();
    this->addresses_to_scan_.erase(this->addresses_to_scan_.begin());

    ESP_LOGD(TAG, "Scanning address %c", address);
    this->SDI12_.begin();
    if (checkActive_(address)) {
      this->scan_results_.emplace_back(address, true);
      printInfo_(address);
    } else {
      this->scan_results_.emplace_back(address, false);
    }
    this->SDI12_.end();
  }
}

}  // namespace sdi12
}  // namespace esphome
