// SR04M-2 ECHO-only Mode
// Using GPIO 13 for ECHO only

#define ECHO_PIN 13

// Constants for measurement
const int NUM_READINGS = 5;  // Number of readings to average
const int MIN_DISTANCE = 2;  // Minimum valid distance in cm
const int MAX_DISTANCE = 400; // Maximum valid distance in cm
const int TIMEOUT = 30000;   // Timeout in microseconds

// Variables for filtering
float readings[NUM_READINGS];
int readIndex = 0;
float total = 0;
float average = 0;

// Variables for statistics
int validReadings = 0;
int invalidReadings = 0;
unsigned long lastStatsTime = 0;
const unsigned long STATS_INTERVAL = 5000; // Print stats every 5 seconds

void setup() {
  Serial.begin(115200);
  pinMode(ECHO_PIN, INPUT);
  
  // Initialize readings array
  for (int i = 0; i < NUM_READINGS; i++) {
    readings[i] = 0;
  }
  
  Serial.println("📡 Starting SR04M-2 in ECHO-only Mode");
  Serial.println("Min distance: " + String(MIN_DISTANCE) + " cm");
  Serial.println("Max distance: " + String(MAX_DISTANCE) + " cm");
}

float getDistance() {
  // Read the ECHO_PIN, return the sound wave travel time in microseconds
  long duration = pulseIn(ECHO_PIN, HIGH, TIMEOUT);
  
  // Calculate distance
  float distance = duration * 0.034 / 2;
  
  // Validate reading
  if (distance >= MIN_DISTANCE && distance <= MAX_DISTANCE) {
    return distance;
  }
  
  return -1; // Invalid reading
}

void printStats() {
  Serial.println("\n📊 Reading Statistics:");
  Serial.print("Valid readings: ");
  Serial.println(validReadings);
  Serial.print("Invalid readings: ");
  Serial.println(invalidReadings);
  if (validReadings + invalidReadings > 0) {
    float successRate = (float)validReadings / (validReadings + invalidReadings) * 100;
    Serial.print("Success rate: ");
    Serial.print(successRate);
    Serial.println("%");
  }
  Serial.println();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Get new reading
  float distance = getDistance();
  
  if (distance == -1) {
    invalidReadings++;
    Serial.println("⚠️  Invalid reading");
  } else {
    validReadings++;
    
    // Update the readings array
    total = total - readings[readIndex];
    readings[readIndex] = distance;
    total = total + readings[readIndex];
    readIndex = (readIndex + 1) % NUM_READINGS;
    
    // Calculate the average
    average = total / NUM_READINGS;
    
    // Print the reading
    Serial.print("📏 Distance: ");
    Serial.print(average);
    Serial.println(" cm");
  }
  
  // Print statistics every STATS_INTERVAL
  if (currentTime - lastStatsTime >= STATS_INTERVAL) {
    printStats();
    lastStatsTime = currentTime;
  }
  
  // Wait before next reading
  delay(100);
}
