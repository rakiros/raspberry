#ifndef MODELS_H
#define MODELS_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Define the SensorData and StatusData models
struct SensorData {
  float waterLevel;
  float waterTemperature;
  float ambTemperature;
  float ambHumidity;
  float soilHumidity;
  float tds;
  float ph;
  unsigned long timestamp;
};

struct StatusData {
  int id;
  bool ligth;
  bool fan;
  bool humidifier;
  bool pump;
  unsigned long serverTimestamp;
};

// Encode the SensorData object to JSON
String encodeSensorData(SensorData sensorData);
void sendSensorData(SensorData sensorData);
// Decode the JSON data to a SensorData object
SensorData decodeSensorData(String jsonData);

// Encode the StatusData object to JSON
String encodeStatusData(StatusData statusData);

// Decode the JSON data to a StatusData object
StatusData decodeStatusData(const char* jsonData);

#endif