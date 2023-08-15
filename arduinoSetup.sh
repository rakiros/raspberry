#! /usr/bin/bash
# https://www.caronteconsulting.com/en/news/how-to/raspberry-arduino-cli/

curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
mv bin/ Arduino/
cd Arduino/
chmod a+x arduino-cli
sudo ./arduino-cli core update-index
sudo ./arduino-cli core install arduino:avr
sudo ./arduino-cli core search
