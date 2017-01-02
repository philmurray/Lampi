import os
import time
import configparser
import logging
import serial
import utilities
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
time.sleep(2)

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
            utilities.lights_message(ser, val["light_pin"] + 'f')
        else:
            my_lamp = key

    lamps.pop(my_lamp)

    pressed = ""
    pressed_start = 0

    current_state = Idle()

    while (True):
        current_state.run()
        for key,val in pins.items():
            if pressed == "":
                if GPIO.input(val["button_pin"]) == False:
                    pressed = key
                    pressed_start = time.time()

            elif pressed == key and GPIO.input(val["button_pin"]):
                secs = time.time() - pressed_start
                logging.debug("{} was pushed for {}".format(key, secs))
                current_state.handleSymbolButton(key, secs)
                pressed = ""
                pressed_start = 0


        for key,val in lamps.items():
            if pressed == "":
                if GPIO.input(val["button_pin"]) == False:
                    pressed = key
            elif pressed == key and GPIO.input(val["button_pin"]):
                secs = time.time() - pressed_start
                logging.debug("{} was pushed for {}".format(key, secs))
                current_state.handleLampButton(key, secs)
                pressed = ""
                pressed_start = 0

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

    def __init__(self, slow = False):
        logging.debug("Entering Idle state")

        utilities.lights_message(ser, '1n2n3n4n')

        if (slow):
            utilities.lights_message(ser, 'sm')
        else:
            utilities.lights_message(ser, 'sn')

        for key,val in lamps.items():
            if val["online"]:
                utilities.lights_message(ser, val['light_pin'] + 'n')
            else:
                utilities.lights_message(ser, val['light_pin'] + 'f')

        self.last_status_check = 0
        self.last_message_check = 0

    def run(self):
        global my_lamp
        global current_state

        if (self.last_status_check + Idle.status_interval < time.time()):
            try:
                for key,val in lamps.items():
                    doc = statusCollection.find_one({"lampId": key, "time":{"$gt": time.time() - Idle.status_interval * 2}})
                    if doc is None and val['online']:
                        val['online'] = False
                        utilities.lights_message(ser, val['light_pin'] + 'f')
                        logging.debug(key + ' is not online.')
                    elif doc is not None and not val['online']:
                        val['online'] = True
                        utilities.lights_message(ser, val['light_pin'] + 'n')
                        logging.debug(key + ' is online.')

                statusCollection.update({"lampId": my_lamp}, {"$set": {"time": time.time()}}, True)

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

    def handleSymbolButton(self, key, tm):
        global current_state
        if tm >= Idle.long_press_time and key == "b4":
            current_state = Off()
        else:
            current_state = BuildMessage(key)


class Off(State):
    def __init__(self):
        logging.debug("Entering Off state")
        for key,val in pins.items():
            if key == "b4":
                utilities.lights_message(ser, val['light_pin'] + 'n')
            else:
                utilities.lights_message(ser, val['light_pin'] + 'f')

        for key,val in lamps.items():
            utilities.lights_message(ser, val['light_pin'] + 'f')

        utilities.lights_message(ser, 'sf')

    def run(self):
        pass

    def handleSymbolButton(self, key, time):
        global current_state
        if key == "b4":
            current_state = Idle(True)


class BuildMessage(State):
    timeout = int(lampiConfig['build_message_timeout'])

    def __init__(self, key):
        logging.debug("Entering BuildMessage state")
        self.button_key = key
        self.start_time = time.time()

        for key,val in lamps.items():
            if val["online"]:
                utilities.lights_message(ser, val['light_pin'] + 'b')
            else:
                utilities.lights_message(ser, val['light_pin'] + 'f')

        for key,val in pins.items():
            if self.button_key == key:
                utilities.lights_message(ser, val['light_pin'] + 'n')
            else:
                utilities.lights_message(ser, val['light_pin'] + 'f')

    def run(self):
        global current_state
        if self.start_time + BuildMessage.timeout < time.time():
            current_state = Idle()

    def handleSymbolButton(self, key, time):
        global current_state
        global my_lamp
        if key == self.button_key:
            current_state = HandleMessage(my_lamp, self.button_key)
        else:
            current_state = BuildMessage(key)

    def handleLampButton(self, key, time):
        global current_state
        if lamps[key]["online"]:
            current_state = SendMessage(key, self.button_key)


class SendMessage(State):
    global my_lamp
    timeout = int(lampiConfig['send_message_timeout'])

    def __init__(self, lamp_key, button_key):
        logging.debug("Entering SendMessage state")
        self.start_time = time.time()

        for key,val in lamps.items():
            if lamp_key == key:
                utilities.lights_message(ser, val['light_pin'] + 's')
            else:
                utilities.lights_message(ser, val['light_pin'] + 'f')

        for key,val in pins.items():
            if button_key == key:
                utilities.lights_message(ser, val['light_pin'] + 's')
            else:
                utilities.lights_message(ser, val['light_pin'] + 'f')

        utilities.lights_message(ser, 'su')

        try:
            messagesCollection.insert({"lampId": lamp_key, "from": my_lamp, "message": button_key, "time": time.time(), "handled": False })
        except:
            logging.error('failure sending message.')

    def run(self):
        global current_state
        if self.start_time + SendMessage.timeout < time.time():
            current_state = Idle()


class HandleMessage(State):
    timeout = int(lampiConfig['handle_message_timeout'])

    def __init__(self, lamp_key, button_key):
        logging.debug("Entering HandleMessage state")
        self.start_time = time.time()

        for key,val in lamps.items():
            if lamp_key == key:
                utilities.lights_message(ser, val['light_pin'] + 's')
            else:
                utilities.lights_message(ser, val['light_pin'] + 'f')

        for key,val in pins.items():
            if button_key == key:
                utilities.lights_message(ser, val['light_pin'] + 's')
            else:
                utilities.lights_message(ser, val['light_pin'] + 'f')

        utilities.lights_message(ser, 's' + pins[button_key]["light_pin"])

    def run(self):
        global current_state
        if self.start_time + HandleMessage.timeout < time.time():
            current_state = Idle()

main()
