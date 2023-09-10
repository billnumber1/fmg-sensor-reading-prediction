#include <Wire.h>
#include "MPU6050.h"
#include "scktmodel.h" // Include the generated C++ code for the trained model

// Initialize RandomForest object for prediction

Eloquent::ML::Port::RandomForest rf;  
MPU6050 mpu; // Initialize MPU6050 object
 
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
 
// Setup function
void setup() {
  Serial.begin(115200); // Use for debugging
  Serial1.begin(9600); // Use for HC-06 communication (default baud rate is 9600)
  Wire.begin(); // Initialize I2C communication
  mpu.initialize(); // Initialize MPU6050
 
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
 
  // Create an input array for the model
  float input[3] = {filteredAccelX, filteredAccelY, filteredAccelZ};
 
  // Predict the position using the generated model
  uint8_t position = rf.predict(input);
 
  // Print the predicted position to the Serial monitor (for debugging)
  //Serial.print("Predicted position: ");
  Serial.println(position);
 
  // Send the predicted position to the HC-06 module
  //Serial1.print("Predicted position: ");
  Serial1.println(position);

 // Add a delay to control the prediction frequency
 delay(100); 
}