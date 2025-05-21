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
  // Initialize sensor with a different command
  this->write_byte(0x55);  // Try a different initialization command
  this->flush();
  delay(50);
}

void SR04M2Sensor::update() {
  ESP_LOGD(TAG, "Requesting new measurement...");
  
  // Clear UART buffer first
  while (this->available()) {
    this->read();
  }
  
  // Send command
  this->write_byte(0x55);  // Try a different command
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
  
  // Process available data
  while (this->available() >= 4) {
    uint8_t data[4];
    for (int i = 0; i < 4; i++) {
      data[i] = this->read();
    }
    
    // Log all frames for debugging
    ESP_LOGD(TAG, "Frame: %02X %02X %02X %02X", data[0], data[1], data[2], data[3]);
    
    // Skip all-zero frames
    if (data[0] == 0x00 && data[1] == 0x00 && data[2] == 0x00 && data[3] == 0x00) {
      continue;
    }
    
    // Try different frame patterns
    float distance_cm = NAN;
    
    // Pattern 1: Standard distance format (00 XX XX 00)
    if (data[0] == 0x00 && data[3] == 0x00) {
      uint16_t raw_value = (data[1] << 8) | data[2];
      if (raw_value > 0 && raw_value < 4500) {
        distance_cm = raw_value / 10.0f;
      }
    }
    
    // Pattern 2: Alternative format (00 E0 00 E0)
    else if (data[0] == 0x00 && data[1] == 0xE0 && data[2] == 0x00 && data[3] == 0xE0) {
      distance_cm = 22.4f;  // This seems to be a fixed value
    }
    
    // Pattern 3: Try interpreting raw bytes as distance
    else {
      uint16_t raw_value = (data[1] << 8) | data[2];
      if (raw_value > 0 && raw_value < 4500) {
        distance_cm = raw_value / 10.0f;
      }
    }
    
    // Validate and publish distance
    if (!isnan(distance_cm)) {
      ESP_LOGD(TAG, "Raw distance value: %.1f cm", distance_cm);
      if (distance_cm >= 2.0f && distance_cm <= 450.0f) {
        this->waiting_for_response_ = false;
        this->publish_state(distance_cm);
        return;
      } else {
        ESP_LOGW(TAG, "Distance %.1f cm outside valid range (2-450 cm)", distance_cm);
      }
    }
  }
  
  // If we have less than 4 bytes, wait for more data
  if (this->available() > 0 && this->available() < 4) {
    // Reset timeout to give more time for complete frame
    this->request_time_ = now;
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