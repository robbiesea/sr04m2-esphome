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

  void set_min_distance(float min_distance) { this->min_distance_ = min_distance; }
  void set_max_distance(float max_distance) { this->max_distance_ = max_distance; }
  void set_timeout(uint32_t timeout) { this->timeout_ = timeout; }

 protected:
  static const uint32_t TIMEOUT_MS = 1000;
  static const uint8_t CMD_BYTE = 0x55;
  bool waiting_for_response_ = false;
  uint32_t request_time_ = 0;
  float min_distance_{2.0f};
  float max_distance_{450.0f};
  uint32_t timeout_{1000};
};

} // namespace sr04m2
} // namespace esphome