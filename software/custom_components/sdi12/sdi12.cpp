#include <vector>
#include "sdi12.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sdi12 {

static const char *const TAG = "sdi12";

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

std::string SDI12Bus::send_command(const std::string &command) {
  if (!initialized_) {
    ESP_LOGE(TAG, "SDI12 bus not initialized!");
    return "";
  }

  ESP_LOGD(TAG, "Sending command on SDI-12 bus...");

  return "dummy response";
}

char SDI12Bus::read_char() {
  if (!initialized_) {
    ESP_LOGE(TAG, "SDI12 bus not initialized!");
    return '\0';
  }

  ESP_LOGD(TAG, "Reading from SDI-12 bus...");

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
  this->SDI12_.begin();

  // Create a vector that contains all addresses that should be scanned
  std::vector<char> addresses_to_scan;

  // Add numerical addresses
  for (char i = '0'; i <= '9'; i++)
    addresses_to_scan.push_back(i);
  // Add lowercase alphabetical addresses
  for (char i = 'a'; i <= 'z'; i++)
    addresses_to_scan.push_back(i);
  // Add uppercase alphabetical addresses
  for (char i = 'A'; i <= 'Z'; i++)
    addresses_to_scan.push_back(i);

  // Now iterate over the addresses and perform the scan
  for (char address : addresses_to_scan) {
    ESP_LOGD(TAG, "Scan Address Space = %c", address);
    yield();
    if (checkActive_(address)) {
      scan_results_.emplace_back(address, true);
      printInfo_(address);
    } else {
      scan_results_.emplace_back(address, false);
    }
  }

  this->SDI12_.end();
}

}  // namespace sdi12
}  // namespace esphome
