#!/usr/bin/env python3

'''
mpu6050.py -
connects to the MPU6050 gyro / accelerometer module connected to the rPi via I2C and publishes the output to the MQTT broker
'''

import smbus
import time
import random
from paho.mqtt import client as mqtt_client
from time import sleep

time.sleep(1)

# MQTT Broker
broker = '192.168.1.205'
port = 1883
topic = "rPi/6axis"
# Generate a Client ID with the publish prefix.
client_id = f'publish-{random.randint(0, 1000)}'
# username = 'emqx'
# password = 'public'

#some MPU6050 Registers and their Address
PWR_MGMT_1   = 0x6B
SMPLRT_DIV   = 0x19
CONFIG       = 0x1A
GYRO_CONFIG  = 0x1B
INT_ENABLE   = 0x38
ACCEL_XOUT_H = 0x3B
ACCEL_YOUT_H = 0x3D
ACCEL_ZOUT_H = 0x3F
GYRO_XOUT_H  = 0x43
GYRO_YOUT_H  = 0x45
GYRO_ZOUT_H  = 0x47

def MPU_Init():
        #write to sample rate register
        bus.write_byte_data(Device_Address, SMPLRT_DIV, 7)
        #Write to power management register
        bus.write_byte_data(Device_Address, PWR_MGMT_1, 1)
        #Write to Configuration register
        bus.write_byte_data(Device_Address, CONFIG, 0)
        #Write to Gyro configuration register
        bus.write_byte_data(Device_Address, GYRO_CONFIG, 24)
        #Write to interrupt enable register
        bus.write_byte_data(Device_Address, INT_ENABLE, 1)

def read_raw_data(addr):
        #Accelero and Gyro value are 16-bit
        high = bus.read_byte_data(Device_Address, addr)
        low = bus.read_byte_data(Device_Address, addr+1)
        #concatenate higher and lower value
        value = ((high << 8) | low)
        #to get signed value from mpu6050
        if(value > 32768):
                value = value - 65536
        return value

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    # client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client

# Connect to MQTT Broker
client = connect_mqtt()

# Start MPU6050
bus = smbus.SMBus(1)    # or bus = smbus.SMBus(0) for older version boards
Device_Address = 0x68   # MPU6050 device address
MPU_Init()

while True:
    #Read Accelerometer raw value
    acc_x = read_raw_data(ACCEL_XOUT_H)
    acc_y = read_raw_data(ACCEL_YOUT_H)
    acc_z = read_raw_data(ACCEL_ZOUT_H)

    #Read Gyroscope raw value
    gyro_x = read_raw_data(GYRO_XOUT_H)
    gyro_y = read_raw_data(GYRO_YOUT_H)
    gyro_z = read_raw_data(GYRO_ZOUT_H)

    #Full scale range +/- 250 degree/C as per sensitivity scale factor
    Ax = acc_x/16384.0
    Ay = acc_y/16384.0
    Az = acc_z/16384.0

    Gx = gyro_x/131.0
    Gy = gyro_y/131.0
    Gz = gyro_z/131.0

    msg = "{"
    msg += "\"Gx\":" + str(Gx) + ","
    msg += "\"Gy\":" + str(Gy) + ","
    msg += "\"Gz\":" + str(Gz) + ","
    msg += "\"Ax\":" + str(Ax) + ","
    msg += "\"Ay\":" + str(Ay) + ","
    msg += "\"Az\":" + str(Az)
    msg += "}"
    print(msg)
    client.publish(topic, msg)
    sleep(1)