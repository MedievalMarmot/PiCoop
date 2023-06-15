# ChickenSensorX
This project sets up the ESP32 MCU to read a TSL25911FN Ambient Light Sensor and send this information to the MQTT
broker on the Raspberry Pi on the same network. Originally I had this hooked directly into the rPi but got occasional I2C errors - may be wire length / pull up
resistor issues on the pre-built modules.

This code was built and uploaded using Visual Studio Code and the PlatformIO integration.
Additional libraries were added to the project (which are not uploaded to this repo)

- Wire.h
- Adafruit_Sensor.h
- Adafruit_TSL2591.h
- WiFi.h
- PubSubClient.h
- Arduino_JSON.h

To use, update the Wifi and MQTT settings, then build and upload to your ESP32. Monitoring the serial console provides useful
debugging output.

This is a functional work in progress - some code cleanup will happen after testing.