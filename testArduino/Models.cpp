#include "Models.h"

// Encode the SensorData object to JSON
String encodeSensorData(SensorData sensorData) {
DynamicJsonDocument json(1024);
  json["l"] = sensorData.waterLevel;
  json["wT"] = sensorData.waterTemperature;
  json["aT"] = sensorData.ambTemperature;
  json["aH"] = sensorData.ambHumidity;
  json["sH"] = sensorData.soilHumidity;
  json["tds"] = sensorData.tds;
  json["ph"] = sensorData.ph;
  json["t"] = sensorData.timestamp;
  String jsonStr;
  serializeJson(json, jsonStr);
  return jsonStr;
}

// Encode the SensorData object to JSON
void sendSensorData(SensorData sensorData) {
DynamicJsonDocument json(1024);
  json["l"] = sensorData.waterLevel;
  json["wT"] = sensorData.waterTemperature;
  json["aT"] = sensorData.ambTemperature;
  json["aH"] = sensorData.ambHumidity;
  json["sH"] = sensorData.soilHumidity;
  json["tds"] = sensorData.tds;
  json["ph"] = sensorData.ph;
  json["t"] = sensorData.timestamp;
  serializeJson(json, Serial);
}

// Decode the JSON data to a SensorData object
SensorData decodeSensorData(String jsonData) {
DynamicJsonDocument json(1024);
  deserializeJson(json, jsonData);
  SensorData sensorData;
  sensorData.waterLevel = json["l"];
  sensorData.waterTemperature = json["wT"];
  sensorData.ambTemperature = json["aT"];
  sensorData.ambHumidity = json["aH"];
  sensorData.soilHumidity = json["sH"];
  sensorData.tds = json["tds"];
  sensorData.ph = json["ph"];
  sensorData.timestamp = json["t"];
  return sensorData;
}

// Encode the StatusData object to JSON
String encodeStatusData(StatusData statusData) {
DynamicJsonDocument json(1024);
  json["id"] = statusData.id;
  json["l"] = statusData.ligth?1:0;
  json["f"] = statusData.fan?1:0;
  json["h"] = statusData.humidifier?1:0;
  json["p"] = statusData.pump?1:0;
  json["t"] = statusData.serverTimestamp;
  String jsonStr;
  serializeJson(json, jsonStr);
  return jsonStr;
}

// Decode the JSON data to a StatusData object
StatusData decodeStatusData(const char* jsonData) {
DynamicJsonDocument json(1024);
  deserializeJson(json, jsonData);
  StatusData statusData;
  statusData.id = json["id"];
  statusData.ligth = json["l"];
  statusData.fan = json["f"];
  statusData.humidifier = json["h"];
  statusData.pump = json["p"];
  const char* datetime = json["t"];
  // String str_timestamp_lifts = datetime;
  // str_timestamp_lifts.remove(10);
  // long str_timestamp_lifts_toint = str_timestamp_lifts.toInt();
  // statusData.serverTimestamp = str_timestamp_lifts_toint;
  statusData.serverTimestamp = atol(datetime);
  return statusData;
}