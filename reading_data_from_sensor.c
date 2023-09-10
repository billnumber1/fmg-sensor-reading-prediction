#include <Wire.h>
#include "MPU6050.h"
 
// Initialize MPU6050 object
MPU6050 mpu;
 
// Force sensor pins
const int forceSensor1Pin = A0;
const int forceSensor2Pin = A2;
 
// Moving average filter parameters
const int filterWindowSize = 10;
float axBuffer[filterWindowSize];
float ayBuffer[filterWindowSize];
float azBuffer[filterWindowSize];
int bufferIndex = 0;
 
// Moving average filter function
float movingAverage(float newValue, float buffer[], int bufferSize) {
  buffer[bufferIndex] = newValue;
  float sum = 0;
  for (int i = 0; i < bufferSize; i++) {
    sum += buffer[i];
  }
  bufferIndex = (bufferIndex + 1) % bufferSize;
  return sum / bufferSize;
}
 
// Function to send data via Bluetooth
void sendBluetoothData(float ax, float ay, float az, int force1, int force2) {
  Serial1.print(ax);
  Serial1.print(",");
  Serial1.print(ay);
  Serial1.print(",");
  Serial1.println(az);
  //Serial1.print(",");
  //Serial1.print(force1);
  //Serial1.print(",");
  //Serial1.println(force2);
}
 
// Setup function
void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  // Initialize Bluetooth serial communication
  Serial1.begin(9600);
  // Initialize I2C communication
  Wire.begin();
  // Initialize MPU6050
  mpu.initialize();
 
  // Set input mode for force sensor pins
  pinMode(forceSensor1Pin, INPUT);
  pinMode(forceSensor2Pin, INPUT);
 
  // Check MPU6050 connection
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connection successful");
  } else {
    Serial.println("MPU6050 connection failed");
    while (1);
  }
}
 
// Main loop
void loop() {
  int16_t ax, ay, az;
 
  // Get raw accelerometer data
  mpu.getAcceleration(&ax, &ay, &az);
 
  // Convert raw data to acceleration in g
  float accelX = ax / 16384.0;
  float accelY = ay / 16384.0;
  float accelZ = az / 16384.0;
 
  // Apply moving average filter
  float filteredAccelX = movingAverage(accelX, axBuffer, filterWindowSize);
  float filteredAccelY = movingAverage(accelY, ayBuffer, filterWindowSize);
  float filteredAccelZ = movingAverage(accelZ, azBuffer, filterWindowSize);
 
  // Read force sensor values
  int forceSensor1Value = analogRead(forceSensor1Pin);
  int forceSensor2Value = analogRead(forceSensor2Pin);
 
// Send filtered accelerometer data and force sensor values via Bluetooth
sendBluetoothData(filteredAccelX,filteredAccelY, filteredAccelZ, forceSensor1Value, forceSensor2Value);
}