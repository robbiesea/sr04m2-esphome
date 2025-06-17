#include <Arduino.h>

// Garage Door Sensor using HC-SR04
// Determines if the garage door is open or closed based on distance

#define ECHO_PIN 13

// Constants for measurement
const int MIN_DISTANCE = 2;  // Minimum valid distance in cm
const int MAX_DISTANCE = 400; // Maximum valid distance in cm
const int TIMEOUT = 30000;   // Timeout in microseconds
const int OPEN_THRESHOLD = 100; // Distance threshold for open door in cm

void setup() {
  Serial.begin(115200);
  pinMode(ECHO_PIN, INPUT);
  Serial.println("🚪 Garage Door Sensor Initialized");
}

float getDistance() {
  long duration = pulseIn(ECHO_PIN, HIGH, TIMEOUT);
  Serial.print("Raw duration: ");
  Serial.println(duration);
  float distance = duration * 0.034 / 2;
  if (distance >= MIN_DISTANCE && distance <= MAX_DISTANCE) {
    return distance;
  }
  return -1; // Invalid reading
}

void loop() {
  float distance = getDistance();
  if (distance == -1) {
    Serial.println("⚠️  Invalid reading");
  } else if (distance > OPEN_THRESHOLD) {
    Serial.println("🚪 Door is Open");
  } else {
    Serial.println("🚪 Door is Closed");
  }
  delay(1000); // Wait before next reading
} 