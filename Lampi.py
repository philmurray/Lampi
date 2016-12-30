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

ser = serial.Serial(lampiConfig['Serial'], int(lampiConfig['BaudRate']), timeout=1)

pins = {
    "b1": {
        "button_pin": int(lampiConfig['b1_pin']),
        "light_pin": lampiConfig['b1_char']
    },
    "b2": {
        "button_pin": int(lampiConfig['b2_pin']),
        "light_pin": lampiConfig['b2_char']
    },
    "b3": {
        "button_pin": int(lampiConfig['b3_pin']),
        "light_pin": lampiConfig['b3_char']
    },
    "b4": {
        "button_pin": int(lampiConfig['b4_pin']),
        "light_pin": lampiConfig['b4_char']
    },
}

lamps = {
    "md": {
        "online": False,
        "button_pin": int(dbConfig['md_button']),
        "light_pin": dbConfig['md_light'],
        "is_me": dbConfig['md_isme'] == 'True'
    },
    "e": {
        "online": False,
        "button_pin": int(dbConfig['e_button']),
        "light_pin": dbConfig['e_light'],
        "is_me": dbConfig['e_isme'] == 'True'
    },
    "pr": {
        "online": False,
        "button_pin": int(dbConfig['pr_button']),
        "light_pin": dbConfig['pr_light'],
        "is_me": dbConfig['pr_isme'] == 'True'
    },
    "ipac": {
        "online": False,
        "button_pin": int(dbConfig['ipac_button']),
        "light_pin": dbConfig['ipac_light'],
        "is_me": dbConfig['ipac_isme'] == 'True'
    }
}

my_lamp = ""

interval = float(lampiConfig['RunLoopInterval'])
client = MongoClient(dbConfig['connectionString'])
db = client.lampi
statusCollection = db.status
messagesCollection = db.messages

def main():
    global my_lamp
    global current_state

    GPIO.setmode(GPIO.BCM)

    for key,val in pins.items():
        GPIO.setup(val["button_pin"], GPIO.IN, pull_up_down=GPIO.PUD_UP)

    for key,val in lamps.items():
        if not val["is_me"]:
            GPIO.setup(val["button_pin"], GPIO.IN, pull_up_down=GPIO.PUD_UP)
        else:
            my_lamp = key
        ser.write(bytes(val["light_pin"] + 'f', 'UTF-8'))

    pressed = ""
    pressed_start = 0

    while (True):
        for key,val in pins.items():
            if pressed == "":
                if GPIO.input(val["button_pin"]) == False:
                    pressed = key
                    pressed_start = time.time()

            elif pressed == key and GPIO.input(val["button_pin"]):
                logging.debug("{} was pushed".format(key))
                pressed = ""
                pressed_start = 0
                current_state.handleSymbolButton(key, time.time() - pressed_start)


        for key,val in lamps.items():
            if not val["is_me"]:
                if pressed == "":
                    if GPIO.input(val["button_pin"]) == False:
                        pressed = key
                elif pressed == key and GPIO.input(val["button_pin"]):
                    logging.debug("{} was pushed".format(key))
                    pressed = ""
                    pressed_start = 0
                    current_state.handleLampButton(key, time.time() - pressed_start)

        current_state.run()
        time.sleep(interval)

class State:
    def run(self):
        logging.error("not implemented")

    def handleLampButton(self, key, time):
        logging.error("not implemented")

    def handleSymbolButton(self, key, time):
        logging.error("not implemented")


class Idle(State):
    status_interval = int(lampiConfig['status_check_timeout'])
    message_interval = int(lampiConfig['message_check_timeout'])
    long_press_time = int(lampiConfig['long_press_time'])

    def __init__(self):
        logging.debug("Entering Idle state")

        ser.write(bytes('1n2n3n4n', 'UTF-8'))
        self.last_status_check = 0
        self.last_message_check = 0

    def run(self):
        global my_lamp
        global current_state

        if (self.last_status_check + Idle.status_interval < time.time()):
            try:
                for key,val in lamps.items():
                    if val["is_me"]:
                        statusCollection.update({"lampId": key}, {"$set": {"time": time.time()}}, True)
                    else:
                        doc = statusCollection.find_one({"lampId": key, "time":{"$gt": time.time() - Idle.status_interval * 2}})
                        if doc is None and val['online']:
                            val['online'] = False
                            ser.write(bytes(val['light_pin'] + 'f', 'UTF-8'))
                            logging.debug(key + ' is not online.')
                        elif doc is not None and not val['online']:
                            val['online'] = True
                            ser.write(bytes(val['light_pin'] + 'n', 'UTF-8'))
                            logging.debug(key + ' is online.')
            except Exception as e:
                logging.error('failure getting lamp status.')
                if hasattr(e, 'message'):
                    logging.error(e.message)
                else:
                    logging.error(e)

            self.last_status_check = time.time()

        if (self.last_message_check + Idle.message_interval < time.time()):
            try:
                message = messagesCollection.find_one_and_update({"lampId": my_lamp, "time":{"$gt": time.time() - Idle.message_interval * 2}, "handled": False}, {"$set": {"handled": True}})
                if message is not None:
                    current_state = HandleMessage(message["from"], message["message"])
            except Exception as e:
                logging.error('failure getting lamp message.')
                if hasattr(e, 'message'):
                    logging.error(e.message)
                else:
                    logging.error(e)

            self.last_message_check = time.time()

    def handleSymbolButton(self, key, time):
        global current_state
        if time >= Idle.long_press_time and key == "b4_pin":
            current_state = Off()
        else:
            current_state = BuildMessage(key)


class Off(State):
    def __init__(self):
        logging.debug("Entering Off state")
        for key,val in pins.items():
            if key == "b4_pin":
                ser.write(bytes(val['light_pin'] + 'n', 'UTF-8'))
            else:
                ser.write(bytes(val['light_pin'] + 'f', 'UTF-8'))

        for key,val in lamps.items():
            if not val["is_me"]:
                ser.write(bytes(val['light_pin'] + 'f', 'UTF-8'))

    def run(self):
        pass

    def handleSymbolButton(self, key, time):
        global current_state
        if key == "b4_pin":
            current_state = Idle()


class BuildMessage(State):
    timeout = int(lampiConfig['build_message_timeout'])

    def __init__(self, key):
        logging.debug("Entering BuildMessage state")
        self.button_key = key
        self.start_time = time.time()

        for key,val in lamps.items():
            if val["online"]:
                ser.write(bytes(val['light_pin'] + 'b', 'UTF-8'))
            elif not val["is_me"]:
                ser.write(bytes(val['light_pin'] + 'f', 'UTF-8'))

        for key,val in pins.items():
            if self.button_key == key:
                ser.write(bytes(val['light_pin'] + 'n', 'UTF-8'))
            else:
                ser.write(bytes(val['light_pin'] + 'f', 'UTF-8'))

    def run(self):
        global current_state
        if self.start_time + timeout > time.time():
            current_state = Idle()

    def handleSymbolButton(self, key, time):
        global current_state
        current_state = BuildMessage(key)

    def handleLampButton(self, key, time):
        global current_state
        if lamps["key"]["online"]:
            current_state = SendMessage(key, button_key)


class SendMessage(State):
    global my_lamp
    timeout = int(lampiConfig['send_message_timeout'])

    def __init__(self, lamp_key, button_key):
        logging.debug("Entering SendMessage state")
        self.start_time = time.time()

        for key,val in lamps.items():
            if lamp_key == key:
                ser.write(bytes(val['light_pin'] + 's', 'UTF-8'))
            else:
                ser.write(bytes(val['light_pin'] + 'f', 'UTF-8'))

        for key,val in pins.items():
            if button_key == key:
                ser.write(bytes(val['light_pin'] + 's', 'UTF-8'))
            else:
                ser.write(bytes(val['light_pin'] + 'f', 'UTF-8'))

        ser.write(bytes('s' + pins["button_key"]["light_pin"], 'UTF-8'))

        try:
            messagesCollection.insert({"lampId": lamp_key, "from": my_lamp, "message": button_key, "time": time.time(), "handled": False })
        except:
            logging.error('failure sending message.')

    def run(self):
        global current_state
        if self.start_time + timeout > time.time():
            current_state = Idle()


class HandleMessage(State):
    timeout = int(lampiConfig['handle_message_timeout'])

    def __init__(self, lamp_key, button_key):
        logging.debug("Entering HandleMessage state")
        self.start_time = time.time()

        for key,val in lamps.items():
            if lamp_key == key:
                ser.write(bytes(val['light_pin'] + 's', 'UTF-8'))
            else:
                ser.write(bytes(val['light_pin'] + 'f', 'UTF-8'))

        for key,val in pins.items():
            if button_key == key:
                ser.write(bytes(val['light_pin'] + 's', 'UTF-8'))
            else:
                ser.write(bytes(val['light_pin'] + 'f', 'UTF-8'))

        ser.write(bytes('s' + pins["button_key"]["light_pin"], 'UTF-8'))

    def run(self):
        global current_state
        if self.start_time + timeout > time.time():
            current_state = Idle()


current_state = Idle()

main()
