#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "../sdi12/sdi12.h"

namespace esphome {
namespace cs215 {

using namespace esphome;

class CS215Component : public PollingComponent, public sdi12::SDI12Device {
  public:
    void set_humidity_sensor(sensor::Sensor *humidity_sensor) { humidity_sensor_ = humidity_sensor; }
    void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }

    float get_setup_priority() const override;
    void setup() override;
    void dump_config() override;
    void update() override;

  protected:
    sensor::Sensor *humidity_sensor_;
    sensor::Sensor *direction_sensor_;
    sensor::Sensor *temperature_sensor_;
    void loop() override;

 private:
/**
   * send_data_() issues the command which requests the sensor to send its data.
*/
    void send_data_();
    uint32_t send_data_timestamp_{0};
};

}  // namespace cs215
}  // namespace esphome
