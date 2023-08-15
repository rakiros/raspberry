/*
   created by Rui Santos, https://randomnerdtutorials.com

   Complete Guide for Ultrasonic Sensor HC-SR04

    Ultrasonic sensor Pins:
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND


#include "Models.h"
#include "DHT.h"
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 
// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

long previousMillis = 0;        // will store last time dht11 was updated
// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long intervalDHT = 2000;           // interval at which to read dht11 (milliseconds)

int relayPin1 = 2;    // Relay 1
int relayPin2 = 3;    // Relay 2

int trigPin = 11;    // Trigger
int echoPin = 12;    // Echo
long duration, cm, inches;

unsigned long lastServerTimestamp;
StatusData lastServerStatus;

void setup() {
  //Serial Port begin
  Serial.begin (115200);
  //Define inputs and outputs
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  dht.begin();
}

void loop() {
  SensorData sensorData = SensorData();
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  // Convert the time into a distance
  cm = (duration / 2) / 29.1;   // Divide by 29.1 or multiply by 0.0343
  inches = (duration / 2) / 74; // Divide by 74 or multiply by 0.0135

  // Serial.print(inches);
  // Serial.print("in, ");
  // Serial.print(cm);
  // Serial.print("cm");
  // Serial.println();

  sensorData.waterLevel = cm;

  // delay(250);

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > intervalDHT) {
    // save the last time dht11 messure
    previousMillis = currentMillis;
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      // Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Compute heat index
    // Must send in temp in Fahrenheit!
    float hi = dht.computeHeatIndex(f, h);

    // Serial.print("Humidity: ");
    // Serial.print(h);
    // Serial.print(" %\t");
    // Serial.print("Temperature: ");
    // Serial.print(t);
    // Serial.print(" *C ");
    // Serial.print(f);
    // Serial.print(" *F\t");
    // Serial.print("Heat index: ");
    // Serial.print(hi);
    // Serial.println(" *F");

    sensorData.ambHumidity = h;
    sensorData.ambTemperature = t;
    sensorData.timestamp = millis();
    // Encode the SensorData object to JSON
    String jsonStr = encodeSensorData(sensorData);

    // Print the JSON data to the serial port
    Serial.println(jsonStr);

    // Relay test
  //      digitalWrite(relayPin1, !digitalRead(relayPin1));
    //    Serial.println("Relay1 changed to: " + digitalRead(relayPin1));

//        digitalWrite(relayPin2, !digitalRead(relayPin2));
    //    Serial.println("Relay2 changed to: " + digitalRead(relayPin2));
  }
}
*/

#include "Models.h"
#include "DHT.h"
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 
// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
long intervalDHT = 2000;           // interval at which to read dht11 (milliseconds)
long previousMillis = 0;        // will store last time dht11 was updated

int relayPin1 = 2;    // Relay 1
int relayPin2 = 3;    // Relay 2

int trigPin = 11;    // Trigger
int echoPin = 12;    // Echo
long duration, cm, inches;

const byte numChars = 64;
char receivedChars[numChars];

boolean newData = false;

StatusData statusData;

void setup() {
    //Define inputs and outputs
    pinMode(relayPin1, OUTPUT);
    pinMode(relayPin2, OUTPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    dht.begin();
    
    Serial.begin(115200);
    Serial.println("<Arduino is ready>");
}

void loop() {
    recvWithStartEndMarkers();
    showNewData();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > intervalDHT) {
     // save the last time dht11 messure
    previousMillis = currentMillis;
    read();
  }
}

void read() {
    SensorData sensorData = SensorData();
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      // Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Compute heat index
    // Must send in temp in Fahrenheit!
    float hi = dht.computeHeatIndex(f, h);
    sensorData.ambHumidity = h;
    sensorData.ambTemperature = t;
    sensorData.ph = 6.7;
    sensorData.tds = 1024;
    sensorData.soilHumidity = 60.3;
    sensorData.waterLevel = 0.76;
    sensorData.waterTemperature = 20.3;
    sensorData.timestamp = millis();

    // Encode the SensorData object to JSON
    String jsonStr = encodeSensorData(sensorData);
    // Print the JSON data to the serial port
    Serial.println(jsonStr);

    // statusData = StatusData();
    // statusData.ligth = true;
    // statusData.id = 1234;
    // statusData.fan = true;
    // statusData.humidifier = false;

    // String statusStr = encodeStatusData(statusData);
    // Serial.println(statusStr);
    if (statusData.ligth == true) {
       digitalWrite(relayPin1, HIGH);
    } else {
       digitalWrite(relayPin1, LOW);
    }

    if (statusData.humidifier == true) {
       digitalWrite(relayPin2, HIGH);
    } else {
       digitalWrite(relayPin2, LOW);
    }

    // Relay test
    // Serial.println("Relay1 changed to: " + digitalRead(relayPin1));
    // Serial.println("Relay2 changed to: " + digitalRead(relayPin2));
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        String jsonData = charArrayToString(receivedChars);
        statusData = decodeStatusData(jsonData);
        newData = false;
    }
}

String charArrayToString(char* charArray) {
  String string = "";
  for (int i = 0; i < strlen(charArray); i++) {
    string += charArray[i];
  }
  return string;
}