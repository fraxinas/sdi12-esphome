#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "../sdi12/sdi12.h"

namespace esphome {
namespace ds2 {

using namespace esphome;

class DS2Component : public PollingComponent, public sdi12::SDI12Device {
  public:
    void set_windspeed_sensor(sensor::Sensor *windspeed_sensor) { windspeed_sensor_ = windspeed_sensor; }

    float get_setup_priority() const override;
    void setup() override;
    void dump_config() override;
    void update() override;

  protected:
    sensor::Sensor *windspeed_sensor_;
};

}  // namespace ds2
}  // namespace esphome
