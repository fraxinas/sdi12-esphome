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
  std::string send_command(const std::string &command);
  char read_char();
  float get_setup_priority() const override { return setup_priority::BUS; }
  void set_scan(bool scan) { scan_ = scan; }
  void set_tx_pin(InternalGPIOPin *tx_pin) { this->tx_pin_ = tx_pin; }
  void set_rx_pin(InternalGPIOPin *rx_pin) { this->rx_pin_ = rx_pin; }
  void set_oe_pin(InternalGPIOPin *oe_pin) { this->oe_pin_ = oe_pin; }

 protected:
  InternalGPIOPin *tx_pin_;
  InternalGPIOPin *rx_pin_;
  InternalGPIOPin *oe_pin_;
  bool initialized_ = false;
  std::vector<std::pair<uint8_t, bool>> scan_results_;
  bool scan_{false};
};

}  // namespace sdi12
}  // namespace esphome
