#include <SPI.h>
#include <SD.h>
// #include <Wire.h>
// #include <DS3231.h>
#include "Database.h"
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
unsigned long lastServerTimestamp;
unsigned long lastLocalTimestamp;
StatusData statusData;

File myFile;
String fileContent;

// RTClib myRTC;
// DS3231 Clock;

void setup() {
    //Define inputs and outputs
    pinMode(relayPin1, OUTPUT);
    pinMode(relayPin2, OUTPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    dht.begin();
    
    Serial.begin(115200);

    // Wire.begin();

  Serial.print("Initializing SD card...");

  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
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
    // DateTime now = myRTC.now();
    
    // Serial.print(now.year(), DEC);
    // Serial.print('/');
    // Serial.print(now.month(), DEC);
    // Serial.print('/');
    // Serial.print(now.day(), DEC);
    // Serial.print(' ');
    // Serial.print(now.hour(), DEC);
    // Serial.print(':');
    // Serial.print(now.minute(), DEC);
    // Serial.print(':');
    // Serial.print(now.second(), DEC);
    // Serial.println();
    
    // Serial.print(" since midnight 1/1/1970 = ");
    // Serial.print(now.unixtime());
    // Serial.print("s = ");
    // Serial.print(now.unixtime() / 86400L);
    // Serial.println("d");


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
    sensorData.timestamp = calculateTimestamp();

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
  sensorData.waterLevel = cm;

    // Encode the SensorData object to JSON
    // String jsonStr = encodeSensorData(sensorData);
    // // Print the JSON data to the serial port
    // Serial.println(jsonStr);
    sendSensorData(sensorData);
    // statusData = StatusData();
    // statusData.ligth = true;
    // statusData.id = 1234;
    // statusData.fan = true;
    // statusData.humidifier = false;

    // String statusStr = encodeStatusData(statusData);
    // Serial.println(statusStr);
    // if (statusData.ligth == true) {
    //    digitalWrite(relayPin1, HIGH);
    // } else {
    //    digitalWrite(relayPin1, LOW);
    // }

    // if (statusData.humidifier == true) {
    //    digitalWrite(relayPin2, HIGH);
    // } else {
    //    digitalWrite(relayPin2, LOW);
    // }

    String sdCardStr = readDatabase();
    Serial.println("sdcontent: " + sdCardStr);
    // Serial.println(statusData.serverTimestampStr);
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
        // String jsonData = charArrayToString(receivedChars);
        statusData = decodeStatusData(receivedChars);
        writeSD(receivedChars);
        lastServerTimestamp = statusData.serverTimestamp;
        lastLocalTimestamp = millis();

            // feed UnixTimeStamp and don' t use localtime
    // Clock.setEpoch(lastServerTimestamp, false);
    // set to 24h
    // Clock.setClockMode(false);

        newData = false;
    }
}

long calculateTimestamp() {
  int newTimestampDiff = millis() - lastLocalTimestamp;
  return lastServerTimestamp + (newTimestampDiff/1000);
}

String charArrayToString(char* charArray) {
  String string = "";
  for (int i = 0; i < strlen(charArray); i++) {
    string += charArray[i];
  }
  return string;
}

void writeSD(char* statusData) {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  SD.remove("test1.txt");
  myFile = SD.open("test1.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test1.txt...");
    myFile.println(statusData);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test1.txt");
  }
}

String readSD() {
  // re-open the file for reading:
  myFile = SD.open("test1.txt");
  if (myFile) {
    Serial.println("test1.txt:");
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      // Serial.write(myFile.read());
      fileContent = myFile.readString();
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test1.txt");
  }

  return fileContent;
}