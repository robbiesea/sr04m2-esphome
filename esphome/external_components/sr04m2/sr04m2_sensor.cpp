#include "sr04m2_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sr04m2 {

static const char *const TAG = "sr04m2.sensor";

void SR04M2Sensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SR04M2...");
  // Clear any pending data
  while (this->available()) {
    this->read();
  }
  // Initialize sensor
  this->write_byte(0x00);
  this->flush();
  delay(100);
}

void SR04M2Sensor::update() {
  ESP_LOGD(TAG, "Requesting new measurement...");
  
  // Clear UART buffer first
  while (this->available()) {
    this->read();
  }
  
  // Send command
  this->write_byte(0x00);  // Using 0x00 as it seems to work
  this->flush();
  
  // Give sensor time to respond
  delay(50);
  
  this->waiting_for_response_ = true;
  this->request_time_ = millis();
  
  ESP_LOGD(TAG, "Command sent, waiting for response...");
}

void SR04M2Sensor::loop() {
  if (!this->waiting_for_response_) {
    return;
  }
  
  // Check for timeout
  uint32_t now = millis();
  if (now - this->request_time_ > TIMEOUT_MS) {
    ESP_LOGW(TAG, "Timeout waiting for data after %dms", now - this->request_time_);
    ESP_LOGD(TAG, "UART available bytes: %d", this->available());
    this->waiting_for_response_ = false;
    this->publish_state(NAN);
   return;
  }
  
  // Only process if we have at least 4 bytes
  while (this->available() > 4) {
    this->read(); // Discard oldest bytes
  }
  
  if (this->available() == 4) {
    uint8_t data[4];
    for (int i = 0; i < 4; i++) {
      data[i] = this->read();
    }
    
    ESP_LOGD(TAG, "Frame: %02X %02X %02X %02X", data[0], data[1], data[2], data[3]);
    
    if (data[0] == 0x00 && data[3] == 0x00) {
      uint16_t raw_value = (data[1] << 8) | data[2];
      if (raw_value > 0 && raw_value < 4500) {
        float distance_cm = raw_value / 10.0f;
        ESP_LOGD(TAG, "Valid distance: %.1f cm", distance_cm);
        if (distance_cm >= 2.0f && distance_cm <= 450.0f) {
          this->waiting_for_response_ = false;
          this->publish_state(distance_cm);
          return;
        }
      }
    }
    // If not valid, just ignore and wait for next frame
  }
}

void SR04M2Sensor::dump_config() {
  ESP_LOGCONFIG(TAG, "SR04M2 Ultrasonic Sensor:");
  LOG_SENSOR("  ", "Distance", this);
  LOG_UPDATE_INTERVAL(this);
  this->check_uart_settings(9600);
}

} // namespace sr04m2
} // namespace esphome