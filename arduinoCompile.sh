#! /usr/bin/bash
echo "NIIISSAAAAAAAN"
sudo /home/rakiros/Arduino/arduino-cli lib install DS3231@1.1.2
sudo /home/rakiros/Arduino/arduino-cli board list
sudo /home/rakiros/Arduino/arduino-cli compile -b arduino:avr:mega testArduino/
sudo /home/rakiros/Arduino/arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:mega testArduino/