#!/usb/bin/env python3
import serial
import time
import json

class StatusData:
    def __init__(self, id, light, fan, humidifier, pump, timestamp):
        self.id = id
        self.light = light
        self.fan = fan
        self.humidifier = humidifier
        self.pump = pump
        self.timestamp = timestamp

    def toJson(self):
        return {
            "id": self.id,
            "l": self.light,
            "f": self.fan,
            "h": self.humidifier,
            "p": self.pump,
            "t": self.timestamp
        }
        
    def toJsonStr(self):
        jsonData = json.dumps(self.__dict__, separators=(',', ':'), sort_keys=True)
        return jsonData.replace(' ', '').replace('id', 'id').replace('light', 'l').replace('fan', 'f').replace('humidifier', 'h').replace('pump', 'p').replace('timestamp', 't')
    @classmethod
    def fromJson(cls, jsonStr):
        jsonData = json.loads(jsonStr)
        return cls(
            jsonData["id"],
            jsonData["l"],
            jsonData["f"],
            jsonData["h"],
            jsonData["p"],
            jsonData["t"]
        )
        
class SensorData:
    def __init__(self, light, waterTemp, airTemp, airHumidity, soilHumidity, tds, ph, timestamp):
        self.light = light
        self.waterTemp = waterTemp
        self.airTemp = airTemp
        self.airHumidity = airHumidity
        self.soilHumidity = soilHumidity
        self.tds = tds
        self.ph = ph
        self.timestamp = timestamp

    def toJson(self):
        return {
            "l": self.light,
            "wT": self.waterTemp,
            "aT": self.airTemp,
            "aH": self.airHumidity,
            "sH": self.soilHumidity,
            "tds": self.tds,
            "ph": self.ph,
            "t": self.timestamp
        }
        
    def toJsonStr(self):
        jsonData = json.dumps(self.__dict__, separators=(',', ':'))
        return jsonData.replace(' ', '')
    @classmethod
    def fromJson(cls, jsonStr):
        if not jsonStr:
            return None

        jsonData = json.loads(jsonStr)
        return cls(
            jsonData["l"],
            jsonData["wT"],
            jsonData["aT"],
            jsonData["aH"],
            jsonData["sH"],
            jsonData["tds"],
            jsonData["ph"],
            jsonData["t"]
        )
        
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1.0)
time.sleep(3)
ser.reset_input_buffer()
print("Serial OK!")

previousMillis = 0
intervalStatus = 3000

try:
    while True:
        time.sleep(0.5)
        timestamp = time.time()
        statusData = StatusData(1234, 1, 1, 0, 0, 0)
        jsonStr = "<" + statusData.toJsonStr() + ">"
        # print(jsonStr)
        ser.write(jsonStr.encode('utf-8'))
        time.sleep(0.5)
        while ser.in_waiting <= 0:
            time.sleep(0.01)
        response = ser.readline().decode('utf-8').rstrip()
        sensorData = SensorData.fromJson(response)
        print(response)
except KeyboardInterrupt:
    print('Serial closed!')
    ser.close()