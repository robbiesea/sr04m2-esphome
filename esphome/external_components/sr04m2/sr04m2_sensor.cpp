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
  // Try to wake up the sensor
  this->write_byte(0x00);
  this->flush();
  delay(100);
}

void SR04M2Sensor::update() {
  ESP_LOGD(TAG, "Requesting new measurement...");
  
  // Clear UART buffer first
  while (this->available()) {
    uint8_t byte = this->read();
    ESP_LOGD(TAG, "Cleared byte: 0x%02X", byte);
  }
  
  // Try different command sequences
  static uint8_t commands[] = {0x55, 0x01, 0x00, 0xFF};
  static uint8_t cmd_index = 0;
  
  uint8_t cmd = commands[cmd_index];
  cmd_index = (cmd_index + 1) % sizeof(commands);
  
  ESP_LOGD(TAG, "Sending command: 0x%02X", cmd);
  this->write_byte(cmd);
  this->flush();
  
  // Give sensor more time to respond
  delay(200);  // Increased delay to 200ms
  
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
    
    // Log any partial data we received
    if (this->available() > 0) {
      ESP_LOGD(TAG, "Partial data received:");
      while (this->available()) {
        uint8_t byte = this->read();
        ESP_LOGD(TAG, "  Byte: 0x%02X", byte);
      }
    }
    
    this->waiting_for_response_ = false;
    this->publish_state(NAN);
    return;
  }
  
  // Wait for complete 4-byte response
  if (this->available() >= 4) {
    this->waiting_for_response_ = false;
    
    uint8_t data[4];
    for (int i = 0; i < 4; i++) {
      data[i] = this->read();
    }
    
    ESP_LOGD(TAG, "Complete frame: %02X %02X %02X %02X", data[0], data[1], data[2], data[3]);
    
    // Format 1: 0xFF + High byte + Low byte + Checksum
    if (data[0] == 0xFF) {
      uint8_t checksum = (data[0] + data[1] + data[2]) & 0xFF;
      if (checksum == data[3]) {
        uint16_t distance_mm = (data[1] << 8) | data[2];
        float distance_cm = distance_mm / 10.0f;
        ESP_LOGD(TAG, "Format 1 - Distance: %.1f cm", distance_cm);
        if (distance_cm >= 2.0f && distance_cm <= 450.0f) {
          this->publish_state(distance_cm);
          return;
        }
      } else {
        ESP_LOGW(TAG, "Format 1 checksum mismatch: expected 0x%02X, got 0x%02X", checksum, data[3]);
      }
    }
    
    // Format 2: High byte + Low byte + 0x00 + 0x00
    if (data[2] == 0x00 && data[3] == 0x00) {
      uint16_t distance_mm = (data[0] << 8) | data[1];
      float distance_cm = distance_mm / 10.0f;
      ESP_LOGD(TAG, "Format 2 - Distance: %.1f cm", distance_cm);
      if (distance_cm >= 2.0f && distance_cm <= 450.0f) {
        this->publish_state(distance_cm);
        return;
      }
    }
    
    // Format 3: Try raw value
    uint16_t raw_value = (data[0] << 8) | data[1];
    if (raw_value > 20 && raw_value < 4500) {  // Reasonable mm range
      float distance_cm = raw_value / 10.0f;
      ESP_LOGD(TAG, "Format 3 - Distance: %.1f cm", distance_cm);
      this->publish_state(distance_cm);
      return;
    }
    
    ESP_LOGW(TAG, "Could not parse response - Invalid data format");
    this->publish_state(NAN);
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