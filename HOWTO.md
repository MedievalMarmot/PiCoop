## HOWTO.md

This abbreviated HOWTO assumes the reader is somewhat familiar with Linux and working via the command line comfortably. 

Work in progress! :)

I did all of this with a single Raspberry Pi 4 and a few sensors / ESP32 boards. The Raspberry Pi and it's installed software listed below could easily be set up on any Linux host, including cloud-hosted systems.

Future ideas:
- Put all sensors on ESP32 boards and use MQTT for data to a broker on the rPI, or in the cloud
- Cloud-hosted MQTT broker / InfluxDB / Grafana?
- 4.3 Inch Color LCD module at the coop with weather / sensor information on a single screen - refreshing often

# Getting started
Using Raspberry Pi Imager, image an SD card with Raspian OS 64 bit - latest version. Configure as needed for SSH access, WiFI / network connectivity and anything else you wish to use on the board. I do not use the Desktop version as my Pi runs outside in an enclosure. 
https://www.raspberrypi.com/software/

Verify you can access your rPi via SSH on the network you installed it on. A static IP address for the rPi is recommended!

Make sure I2C and other protocols you may need are enabled on the rPi. See https://pimylifeup.com/raspberry-pi-i2c/ (Other protocols you may want to use include 1Wire and SPI)

Install and configure the following software packages and modules - read the documentation at the included links for instructions:

- sudo apt update / apt upgrade after the rPi is online to get it current with software and security patches.
- Python / Pip - I use python3 and python3-pip ()
- I2C Tools - via ssh: $ sudo apt-get install i2c-tools
    - To detect I2C devices use the command: $ sudo i2cdetect -y 1
- InfluxDB2 - https://randomnerdtutorials.com/install-influxdb-2-raspberry-pi/ - install InfluxDB2 and make sure the service is enabled
  - Create a bucket for your sensor data
  - Create an orginazation and an auth token for Telegraf / Grafana to use for connecting (see the documentation in the links)
- Node-Red - https://nodered.org/docs/getting-started/raspberrypi - install the current version of Node-Red and make sure the servuce is enabled
- Install Mosquitto and it's clients with the command: sudo apt install mosquitto mosquitto-clients
  - Enable the service with the command: sudo systemctl enable mosquitto.service
  - Set a password and enable / restart the service:
    - sudo mosquitto_passwd -c /etc/mosquitto/passwd <username>
    - sudo systemctl enable mosquitto.service
    - sudo systemctl restart mosquitto
    - sudo systemctl status mosquitto
- Grafana - https://grafana.com/tutorials/install-grafana-on-raspberry-pi/ - install Grafana and enable the service
  - Add the running InfluxDB 2 as a data source for Grafana in it's configuration
- Telegraf - https://linuxhint.com/monitor-raspberry-pi-system-using-influxdb-telegraf-grafana/
  - Once installed, you will need to supply the Telegraf configuration with a token, a host to connect to, and the organization in InfluxDB that you wish to connect to. The UI of InfluxDB has a Telegraf section to set this up and retrieve these values. They can be placed in /etc/telegraf/telegraf.conf (they are commented out initially)

# Post-install
Once these packages and services are set up and running correctly, you should be able to configure any sensor data to be pulled into Node-Red via an appropriate input node / flow and pass that data into an InfluxDB bucket.