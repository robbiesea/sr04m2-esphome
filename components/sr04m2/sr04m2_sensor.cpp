#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace sr04m2 {

class SR04M2Sensor : public sensor::Sensor, public PollingComponent, public uart::UARTDevice {
 public:
  void update() override {
    // TODO: Add UART read logic here
  }
};

}  // namespace sr04m2
}  // namespace esphome
