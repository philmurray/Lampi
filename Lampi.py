#!/usr/bin/python3

import os
import time
import configparser
import logging
import serial
import RPi.GPIO as GPIO

logging.basicConfig(format='%(asctime)s %(levelname)s:%(message)s')
logging.info("Running Lampi")

config = configparser.ConfigParser()
config.read(os.path.join(os.path.abspath(os.path.dirname(__file__)), 'config.ini'))

lampiConfig = config['Lampi']
interval = float(lampiConfig['RunLoopInterval'])

ser = serial.Serial(lampiConfig['Serial'], int(lampiConfig['BaudRate']), timeout=1)

GPIO.setmode(GPIO.BCM)

pins = {
    "b1_pin": int(lampiConfig['b1_pin']),
    "b2_pin": int(lampiConfig['b2_pin']),
    "b3_pin": int(lampiConfig['b3_pin']),
    "b4_pin": int(lampiConfig['b4_pin']),
    "t1_pin": int(lampiConfig['t1_pin']),
    "t2_pin": int(lampiConfig['t2_pin']),
    "t3_pin": int(lampiConfig['t3_pin']),
}

for key,val in pins.items():
    GPIO.setup(val, GPIO.IN, pull_up_down=GPIO.PUD_UP)

pressed = ""

ser.write(bytes('1n2n3n4n', 'UTF-8'))
while (True):
    for key,val in pins.items():
        if pressed == "":
            if GPIO.input(val) == False:
                pressed = key
        elif pressed == key and GPIO.input(val):
            logging.debug("{} was pushed".format(key))
            pressed = ""
    
    time.sleep(interval)
