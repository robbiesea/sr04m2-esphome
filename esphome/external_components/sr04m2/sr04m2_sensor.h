#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace sr04m2 {

class SR04M2Sensor : public sensor::Sensor, public PollingComponent, public uart::UARTDevice {
 public:
  void setup() override;
  void update() override;
  void loop() override;
  void dump_config() override;

 protected:
  static const uint32_t TIMEOUT_MS = 1000;
  static const uint8_t CMD_BYTE = 0x55;
  bool waiting_for_response_ = false;
  uint32_t request_time_ = 0;
};

} // namespace sr04m2
} // namespace esphome