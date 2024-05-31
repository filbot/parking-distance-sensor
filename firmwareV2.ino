#include <NewPing.h>
#include <Adafruit_NeoPixel.h>

// Define pins for HC-SR04
#define TRIGGER_PIN 9
#define ECHO_PIN 10
#define MAX_DISTANCE 400  // Maximum distance to ping (in centimeters)

#define LED_PIN 6    // Define the pin where the NeoPixel data line is connected
#define LED_COUNT 1  // Number of NeoPixels (1 if controlling a single RGB LED)

// Create NeoPixel strip object
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Create NewPing object
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Define thresholds in centimeters
#define TWO_FEET 61
#define FOUR_FEET 95

// Timing constants
#define SLEEP_INTERVAL 60000  // One minute in milliseconds
#define DEBOUNCE_DELAY 50     // Debounce delay in milliseconds

// Define variance threshold in centimeters
#define VARIANCE_THRESHOLD 10

// Global variables to store time and distance
unsigned long lastPingTime = 0;
unsigned long lastChangeTime = 0;
unsigned int lastDistance = 0;

void setup() {
  Serial.begin(115200);          // Initialize serial communication
  pinMode(TRIGGER_PIN, OUTPUT);  // Set trigger pin as output
  pinMode(ECHO_PIN, INPUT);      // Set echo pin as input
  strip.begin();                 // Initialize the NeoPixel strip
  strip.show();                  // Initialize all pixels to 'off'
}

void loop() {
  delay(DEBOUNCE_DELAY);
  unsigned long currentTime = millis();

  // Perform a ping every DEBOUNCE_DELAY milliseconds
  if (currentTime - lastPingTime > DEBOUNCE_DELAY) {
    lastPingTime = currentTime;

    // Take multiple readings to ensure a stable distance measurement
    const int numReadings = 5;
    unsigned long totalDistance = 0;

    for (int i = 0; i < numReadings; i++) {
      totalDistance += sonar.ping_cm();
      delay(10);  // Short delay between readings
    }

    unsigned int distance = totalDistance / numReadings;

    // Check if the distance has changed beyond the variance threshold
    if (abs(distance - lastDistance) > VARIANCE_THRESHOLD) {
      lastChangeTime = currentTime;
      lastDistance = distance;

      // Set the color based on distance
      if (distance <= TWO_FEET && distance > 0) {
        red();
      } else if (distance <= FOUR_FEET) {
        yellow();
      } else {
        green();
      }
    }
  }

  // Check if it's time to sleep
  if (currentTime - lastChangeTime >= SLEEP_INTERVAL) {
    Serial.println("Entering sleep mode.");
    sleep();
  }
}

void red() {
  strip.setPixelColor(0, 0, 255, 0);  // Set the color to red
  strip.show();                       // Update the strip to show the color
}

void yellow() {
  strip.setPixelColor(0, 0, 0, 255);  // Set the color to yellow
  strip.show();                       // Update the strip to show the color
}

void green() {
  strip.setPixelColor(0, 255, 0, 0);  // Set the color to green
  strip.show();                       // Update the strip to show the color
}

void sleep() {
  strip.setPixelColor(0, 0);  // Turn off the LED
  strip.show();               // Update the strip to show the off state
}
