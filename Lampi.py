import os
import time
import configparser
import logging
import serial
import RPi.GPIO as GPIO
from pymongo import MongoClient

logging.basicConfig(format='%(asctime)s %(levelname)s:%(message)s')
logging.info("Running Lampi")

config = configparser.ConfigParser()
config.read(os.path.join(os.path.abspath(os.path.dirname(__file__)), 'config.ini'))

db_config = configparser.ConfigParser()
db_config.read(os.path.join(os.path.abspath(os.path.dirname(__file__)), 'db_config.ini'))

dbConfig = db_config['DbConfig']

lampiConfig = config['Lampi']
interval = float(lampiConfig['RunLoopInterval'])

ser = serial.Serial(lampiConfig['Serial'], int(lampiConfig['BaudRate']), timeout=1)

GPIO.setmode(GPIO.BCM)
client = MongoClient(dbConfig['connectionString'])
db = client.lampi
statusCollection = db.status
messagesCollection = db.messages

pins = {
    "b1_pin": int(lampiConfig['b1_pin']),
    "b2_pin": int(lampiConfig['b2_pin']),
    "b3_pin": int(lampiConfig['b3_pin']),
    "b4_pin": int(lampiConfig['b4_pin']),
    "t1_pin": int(lampiConfig['t1_pin']),
    "t2_pin": int(lampiConfig['t2_pin']),
    "t3_pin": int(lampiConfig['t3_pin']),
}

lamps = {
    "md": {
        "online": False,
        "button_pin": dbConfig['md_button'],
        "light_pin": dbConfig['md_light'],
        "is_me": dbConfig['md_isme'] == 'True'
    },
    "e": {
        "online": False,
        "button_pin": dbConfig['md_button'],
        "light_pin": dbConfig['md_light'],
        "is_me": dbConfig['md_isme'] == 'True'
    },
    "pr": {
        "online": False,
        "button_pin": dbConfig['md_button'],
        "light_pin": dbConfig['md_light'],
        "is_me": dbConfig['md_isme'] == 'True'
    },
    "ipac": {
        "online": False,
        "button_pin": dbConfig['md_button'],
        "light_pin": dbConfig['md_light'],
        "is_me": dbConfig['md_isme'] == 'True'
    }
}

for key,val in pins.items():
    GPIO.setup(val, GPIO.IN, pull_up_down=GPIO.PUD_UP)

pressed = ""

status_interval = int(lampiConfig['status_check_timeout'])
message_interval = int(lampiConfig['message_check_timeout'])
last_status_check = 0
last_message_check = 0

ser.write(bytes('1n2n3n4n', 'UTF-8'))
while (True):
    for key,val in pins.items():
        if pressed == "":
            if GPIO.input(val) == False:
                pressed = key
        elif pressed == key and GPIO.input(val):
            logging.debug("{} was pushed".format(key))

    if (last_status_check + status_interval < time.time()):
        for key,val in lamps.items():
            if val["is_me"]:
                statusCollection.update({"lampId": key}, {"$set": {"time": time.time()}}, True)
            else:
                doc = statusCollection.find_one({"lampId": key, "time":{"$gt": time.time() - status_interval * 2}})
                if doc is None and val['online']:
                    val['online'] = False
                    ser.write(val['light_pin'] + 'f')
                elif doc is not None and not val['online']:
                    val['online'] = False
                    ser.write(val['light_pin'] + 'n')
        last_status_check = time.time()
        
    if (last_message_check + message_interval < time.time()):
        #messageCollection.findOne()
        last_message_check = time.time()
    
    time.sleep(interval)
