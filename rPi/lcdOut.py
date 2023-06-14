#!/usr/bin/env python3

'''
lcdOut.py -
Runs in the background on the rPi via commandline: $ nohup ./lcdOut.py &
connects to the MQTT broker as a client and displays temp values published by the ESP32 and the BME280 module
The LCD module is connected to the rPi via I2C
'''

import paho.mqtt.client as mqtt
import time
import json
import datetime
import requests
import smbus2 as smbus
from datetime import datetime

import os
from time import sleep
os.system('i2cdetect -y 1')
sleep(0.1)

BUS = smbus.SMBus(1)

def write_word(addr, data):
    global BLEN
    temp = data
    if BLEN == 1:
        temp |= 0x08
    else:
        temp &= 0xF7
    BUS.write_byte(addr, temp)

def send_command(comm):
    buf = comm & 0xF0
    buf |= 0x04
    write_word(LCD_ADDR, buf)
    time.sleep(0.002)
    buf &= 0xFB
    write_word(LCD_ADDR, buf)

    buf = (comm & 0x0F) << 4
    buf |= 0x04
    write_word(LCD_ADDR, buf)
    time.sleep(0.002)
    buf &= 0xFB
    write_word(LCD_ADDR, buf)

def send_data(data):
    buf = data & 0xF0
    buf |= 0x05
    write_word(LCD_ADDR, buf)
    time.sleep(0.002)
    buf &= 0xFB
    write_word(LCD_ADDR, buf)

    buf = (data & 0x0F) << 4
    buf |= 0x05
    write_word(LCD_ADDR, buf)
    time.sleep(0.002)
    buf &= 0xFB
    write_word(LCD_ADDR, buf)

def init(addr, bl):
    global LCD_ADDR
    global BLEN
    LCD_ADDR = addr
    BLEN = bl
    try:
        send_command(0x33)
        time.sleep(0.005)
        send_command(0x32)
        time.sleep(0.005)
        send_command(0x28)
        time.sleep(0.005)
        send_command(0x0C)
        time.sleep(0.005)
        send_command(0x01)
        BUS.write_byte(LCD_ADDR, 0x08)
    except:
        return False
    else:
        return True

def clear():
    send_command(0x01)

def openlight():
    BUS.write_byte(0x27, 0x08)
    BUS.close()

def write(x, y, str):
    if x < 0:
        x = 0
    if x > 19:
        x = 19
    if y < 0:
        y = 0
    if y > 3:
        y = 3

    # Fixed row addresses
    row_offsets = [0x00, 0x40, 0x14, 0x54]
    addr = 0x80 + row_offsets[y] + x
    send_command(addr)

    for chr in str:
        send_data(ord(chr))

def on_connect(client, userdata, flags, rc):
  print("Connected with result code "+str(rc))
  client.subscribe("esp32/temperature")
  client.subscribe("rPi/evironment")

def on_message_esp32(client, userdata, msg):
  time.sleep(1)
  x = json.loads(msg.payload.decode('utf-8'))
  if (x):
   # print(x)
   temp1_F = x['temp1_F']
   temp2_F = x['temp2_F']
   temp3_F = x['temp3_F']
   temp4_F = x['temp4_F']

   now = datetime.now().strftime("%b/%d/%Y - %H:%M:%S")

   # LCD display temps top two lines
   lcdLine0 = "T: {}F {}F".format(temp1_F, temp2_F)
   print(lcdLine0)
   write(0, 0, '                    ')
   write(0, 1, '                    ')
   write(0, 0, lcdLine0)
   lcdLine1 = "B: {}F {}F".format(temp3_F, temp4_F)
   print(lcdLine1)
   write(0, 1, lcdLine1)
   time.sleep(1)

def on_message_rpi(client, userdata, msg):
  time.sleep(1)
  x = json.loads(msg.payload.decode('utf-8'))
  if (x):
    # print(x)
    temp_C = x['temperature_C']
    humidity = x['humidity']
    air_press_hPa = x['pressure_hPa']
    heat_index = x['heatIndex']
    dew_point = x['dewPoint_C'] * 9/5 + 32
    altitude = x['altitude_M']
    temp_F = x['temperature_F']
    air_pressure_hq = x['pressure_Hg']
    # print("Ambient levels: ", temp_C, humidity, air_press_hPa, heat_index, dew_point, altitude, temp_F, air_pressure_hq)

    # LCD display temps top two lines
    lcdLine2 = "AT {:.2f}F H {:.2f}%".format(temp_F, humidity)
    print(lcdLine2)
    write(0, 2, '                    ')
    write(0, 3, '                    ')
    write(0, 2, lcdLine2)
    lcdLine3 = "HI {:.2f}F DP {:.2f}F".format(heat_index, dew_point)
    print(lcdLine3)
    write(0, 3, lcdLine3)
    time.sleep(1)

# Init LCD
init(0x27, 1)
write(0, 0, 'Chicken MCP 2000')
write(0, 1, 'AutoChicken Sys')
write(0, 2, 'v0.01a loaded')
write(0, 3, 'Master of Clucks')

client = mqtt.Client()

# Give a name to this MQTT client
client = mqtt.Client('piCoop')
client.message_callback_add('esp32/temperature', on_message_esp32)
client.message_callback_add('rPi/evironment', on_message_rpi)
client.connect("192.168.1.205",1883)

client.on_connect = on_connect
client.loop_forever()