#include "esphome.h"

class SR04M2Sensor : public Component, public UARTDevice, public Sensor {
 public:
  SR04M2Sensor(UARTComponent *parent) : UARTDevice(parent) {}

  void setup() override {
    // Called when ESP boots
  }

  void loop() override {
    if (available() >= 4) {
      uint8_t header = read();
      if (header == 0xFF) {
        uint8_t high = read();
        uint8_t low = read();
        uint8_t sum = read();

        if (((high + low) & 0xFF) == sum) {
          int distance = (high << 8) + low;
          publish_state(distance);
        }
      }
    }
  }
};
