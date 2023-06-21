# PiCoop
## Raspberry Pi / ESP32 Chicken Coop environmental monitoring and control.

(This is a work in progres)

This is a personal project I put together to monitor and maintain our chicken coop's environment and keep it relatively stable
through the ups and downs of the PNW annual seasons. According to the research we've done chickens are quite happy being in an
environment between 65-75 degrees F. Anything outside of that can apparently cause distress, especially in laying hens. This 
little IoT chicken coop project is probably overkill for raising happy chickens (several people just let them roam free in all
sorts of weather) but I thought it'd be a good project to learn some new technology stacks while keeping our hens happy :)

## Design Goals:
- Monitor the outside and inside environments of our chicken coop
  - Using single board computer technogoly and common IoT sensors, montitor the physical environment
  - Publish the monitored values on an MQTT stream
  - Display a subset of the values on an LCD screen at the coop
  - Store monitored values in a time-series database, in this case InfluxDB
  - Visualize the data in Grafana over time
  - Make environmental decisions on the data recieved:
    - Turn on / off fans or heat as needed
    - Turn on any lighting inside or outside the coop via I2C relays
    - Alert users if the environment is outside of limits for more than a specific time

## Hardware:
- Raspberry Pi 4 8GB RAM / 128GB SD
- ESP32 MCU ESP32S Development Board

## Input / Output:
- GeeekPi IIC I2C TWI Serial LCD 2004 20x4 Display Module with I2C Interface Adapter
  - http://wiki.sunfounder.cc/index.php?title=I2C_LCD2004
  - Note: I may replace this LCD as it overheats during the day with the backlight on outdoors and causes I2C issues (works great when cool)
  - Note 2: I'm going to attempt to hook the LCD to another ESP32 and have it pull MQTT messages and display appr. output from there.
- TSL25911FN Ambient Light Sensor (1)
  - https://www.cqrobot.com/index.php?route=product/product&product_id=1112
  - https://www.waveshare.com/wiki/TSL25911_Light_Sensor
- DS18B20 Temperature Sensor Module (4)
  - https://www.amazon.com/dp/B0924NBNZP?psc=1&ref=ppx_yo2ov_dt_b_product_details
  - http://www.airspayce.com/mikem/bcm2835/
- Waveshare BME280 Environmental Sensor (1)
  - https://www.waveshare.com/wiki/BME280_Environmental_Sensor
- SparkFun Qwiic Single Relay-Up to 5.5A at 240VAC I2C (3)
  - https://www.sparkfun.com/products/15093
  - https://learn.sparkfun.com/tutorials/qwiic-single-relay-hookup-guide/all
  - https://github.com/sparkfun/Qwiic_Relay
  - https://github.com/fourstix/Sparkfun_CircuitPython_QwiicRelay (for rPi)
- HiLetgo GY-521 MPU-6050 MPU6050 3 Axis Accelerometer Gyroscope Module
  - https://www.electronicwings.com/raspberry-pi/mpu6050-accelerometergyroscope-interfacing-with-raspberry-pi
  - https://www.electronicwings.com/sensors-modules/mpu6050-gyroscope-accelerometer-temperature-sensor-module
- MakerFocus Raspberry Pi 4 Battery Pack UPS, RPI Pack Standard 4000mAh Raspberry Pi Battery USB Pack Raspberry Pi Latest Version V3Plus Expansion Board
  - https://www.makerfocus.com/products/raspberry-pi-expansion-board-ups-pack-standard-power-supply
  - https://github.com/rcdrones/UPSPACK_V3/blob/master/README_en.md

## Operating System:
- RaspianOS 64 bit

## Software
- InfluxDB 2 - Time Series DB
  - https://www.influxdata.com/
  - https://randomnerdtutorials.com/install-influxdb-2-raspberry-pi/
- Grafana - Data visualizer
  - https://grafana.com/
  - https://grafana.com/tutorials/install-grafana-on-raspberry-pi/
- Node-Red - node based flows
  - https://nodered.org/
  - https://nodered.org/docs/getting-started/raspberrypi
- Telegraf - data collection server agent
  - https://linuxhint.com/monitor-raspberry-pi-system-using-influxdb-telegraf-grafana/
- Mosquitto - MQTT message broker / server
  - https://mosquitto.org/
  - https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi/
- Visual Studio Code with PlatformIO installed (for programming the ESP32, code maintenance)
  - https://code.visualstudio.com/
  - https://platformio.org/
  - https://randomnerdtutorials.com/vs-code-platformio-ide-esp32-esp8266-arduino/
- Paho MQTT Python client
  - https://pypi.org/project/paho-mqtt/

## Resources
- https://www.thechickengeek.com/knowing-ideal-temperature-for-chickens/