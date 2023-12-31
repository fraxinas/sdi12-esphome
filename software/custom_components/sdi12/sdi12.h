#pragma once

#include <vector>
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/component.h"
#include "sdi12_bus.h"

namespace esphome {
namespace sdi12 {

#define LOG_SDI12_DEVICE(this) ESP_LOGCONFIG(TAG, "  Address: %c", this->address_);

class SDI12Bus : public Component {
 public:
  void setup() override;
  void dump_config() override;
  std::string send_command(std::string command);
  char read_char();
  float get_setup_priority() const override { return setup_priority::BUS; }
  void set_scan(bool scan) { scan_ = scan; }
  void set_tx_pin(InternalGPIOPin *tx_pin) { this->tx_pin_ = tx_pin; }
  void set_rx_pin(InternalGPIOPin *rx_pin) { this->rx_pin_ = rx_pin; }
  void set_oe_pin(InternalGPIOPin *oe_pin) { this->oe_pin_ = oe_pin; }
  bool is_scanning() { return this->scan_; }

 protected:
  void loop() override;
  InternalGPIOPin *tx_pin_;
  InternalGPIOPin *rx_pin_;
  InternalGPIOPin *oe_pin_;
  bool initialized_ = false;
  SDI12 SDI12_;
  std::vector<std::pair<uint8_t, std::string>> scan_results_;
  bool scan_{false};

 private:
  boolean check_device_active_(char i);
  std::string get_device_info_(char i);
  void init_scan_();
  void do_scan_();
  std::vector<char> addresses_to_scan_{};
};

/**
 * The SDI12Device is what components connected to SDI-12 will create.
 *
 */
class SDI12Device;
class SDI12Register {
 public:
  explicit operator uint8_t() const { return get(); }

  uint8_t get() const;

 protected:
  friend class SDI12Device;

  SDI12Register(SDI12Device *parent, uint8_t a_register) : parent_(parent), register_(a_register) {}

  SDI12Device *parent_;
  uint8_t register_;
};

class SDI12Device {
 public:
  SDI12Device() = default;

  void set_sdi12_address(std::string address);
  void set_sdi12_bus(SDI12Bus *bus) { bus_ = bus; }

  SDI12Register reg(uint8_t a_register) { return {this, a_register}; }

 protected:
  void parse_sdi12_values_(const std::string &response, std::vector<float*> values);
  char address_{'0'};
  SDI12Bus *bus_{nullptr};
};

}  // namespace sdi12
}  // namespace esphome
