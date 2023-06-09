# ChickenSensorX
This project sets up the ESP32 MCU to read 1-wire temperature sensors and send this information to the MQTT
broker on the Raspberry Pi on the same network.

Currently configured for four temperature probe modules but more could be added. The code would need to be updated to support this
or find them dynamically and set them up.

This code was built and uploaded using Visual Studio Code and the PlatformIO integration.
Additional libraries were added to the project (which are not uploaded to this repo)

- OneWire.h
- DallasTemperature.h
- WiFi.h
- PubSubClient.h
- Arduino_JSON.h

To use, update the Wifi and MQTT settings, then build and upload to your ESP32. Monitoring the serial console provides useful
debugging output.

This is a functional work in progress - some code cleanup will happen after testing.