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
serialUtil = utilities.SerialUtil(ser)

pins = {
    "b1": {
        "button_pin": int(lampiConfig['b1_pin']),
        "light_pin": lampiConfig['b1_char'],
        "strip_code": int(lampiConfig['b1_s'], 2)
    },
    "b2": {
        "button_pin": int(lampiConfig['b2_pin']),
        "light_pin": lampiConfig['b2_char'],
        "strip_code": int(lampiConfig['b2_s'], 2)
    },
    "b3": {
        "button_pin": int(lampiConfig['b3_pin']),
        "light_pin": lampiConfig['b3_char'],
        "strip_code": int(lampiConfig['b3_s'], 2)
    },
    "b4": {
        "button_pin": int(lampiConfig['b4_pin']),
        "light_pin": lampiConfig['b4_char'],
        "strip_code": int(lampiConfig['b4_s'], 2)
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

strip_codes = {
    int('0001', 2): "1",
    int('0010', 2): "2",
    int('0100', 2): "3",
    int('1000', 2): "4",
    int('0011', 2): "5",
    int('0101', 2): "6",
    int('1001', 2): "7",
    int('0110', 2): "8",
    int('1010', 2): "9",
    int('1100', 2): "0",
    int('0111', 2): "a",
    int('1011', 2): "b",
    int('1101', 2): "c",
    int('1110', 2): "d",
    int('1111', 2): "e"
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
            serialUtil.lights_message( val["light_pin"] + 'f')
        else:
            my_lamp = key

    lamps.pop(my_lamp)

    pressed = ""
    pressed_start = 0
    press_handled = False
    long_press_time = float(lampiConfig['long_press_time'])

    current_state = Idle()

    while (True):
        current_state.run()
        long_pressed = bool(pressed_start != 0 and time.time() - pressed_start > long_press_time)

        for key,val in pins.items():
            if pressed == "":
                if GPIO.input(val["button_pin"]) == False:
                    pressed = key
                    pressed_start = time.time()
                    press_handled = False

            elif pressed == key and GPIO.input(val["button_pin"]):
                if not press_handled:
                    logging.debug("{} was pushed".format(key))
                    current_state.handleSymbolButton(key)
                pressed = ""
                pressed_start = 0
                press_handled = True
            elif pressed == key and long_pressed and not press_handled:
                logging.debug("{} was long pressed".format(key))
                pressed_start = 0
                press_handled = current_state.handleSymbolLongPress(key)

        for key,val in lamps.items():
            if pressed == "":
                if GPIO.input(val["button_pin"]) == False:
                    pressed = key
                    pressed_start = time.time()
                    press_handled = False

            elif pressed == key and GPIO.input(val["button_pin"]):
                if not press_handled:
                    logging.debug("{} was pushed".format(key))
                    current_state.handleLampButton(key)
                pressed = ""
                pressed_start = 0
                press_handled = True
            elif pressed == key and long_pressed and not press_handled:
                logging.debug("{} was long pressed".format(key))
                pressed_start = 0
                press_handled = current_state.handleLampLongPress(key)

        time.sleep(interval)


class State:
    def run(self):
        logging.warning("not implemented")

    def handleLampButton(self, key):
        logging.warning("not implemented")

    def handleLampLongPress(self, key):
        logging.warning("not implemented")
        return False

    def handleSymbolButton(self, key):
        logging.warning("not implemented")

    def handleSymbolLongPress(self, key):
        logging.warning("not implemented")
        return False

class Idle(State):
    status_interval = int(lampiConfig['status_check_timeout'])
    message_interval = int(lampiConfig['message_check_timeout'])

    def __init__(self):
        logging.debug("Entering Idle state")

        serialUtil.lights_message( '1n2n3n4nsn')

        for key,val in lamps.items():
            if val["online"]:
                serialUtil.lights_message( val['light_pin'] + 'n')
            else:
                serialUtil.lights_message( val['light_pin'] + 'f')

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
                        serialUtil.lights_message( val['light_pin'] + 'f')
                        logging.debug(key + ' is not online.')
                    elif doc is not None and not val['online']:
                        val['online'] = True
                        serialUtil.lights_message( val['light_pin'] + 'n')
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
                    current_state = HandleMessage(message["from"], message["message"], message.get("stripCode"))
            except Exception as e:
                logging.error('failure getting lamp message.')
                if hasattr(e, 'message'):
                    logging.error(e.message)
                else:
                    logging.error(e)

            self.last_message_check = time.time()

    def handleSymbolButton(self, key):
        global current_state
        current_state = BuildMessage(key)

    def handleSymbolLongPress(self, key):
        global current_state
        if key == "b4":
            current_state = Off()
            return True
        if key == "b1":
            current_state = ChangeIdle()
            return True
        else:
            return False


class Off(State):
    def __init__(self):
        logging.debug("Entering Off state")
        for key,val in pins.items():
            if key == "b4":
                serialUtil.lights_message( val['light_pin'] + 'n')
            else:
                serialUtil.lights_message( val['light_pin'] + 'f')

        for key,val in lamps.items():
            serialUtil.lights_message( val['light_pin'] + 'f')

        serialUtil.lights_message( 'sf')

    def run(self):
        pass

    def handleSymbolButton(self, key):
        global current_state
        if key == "b4":
            current_state = Idle()


class BuildMessage(State):
    timeout = int(lampiConfig['build_message_timeout'])

    def __init__(self, button_key):
        logging.debug("Entering BuildMessage state")
        self.start_time = time.time()
        self.buttons = 0

        for key,val in lamps.items():
            if val["online"]:
                serialUtil.lights_message( val['light_pin'] + 'b')
            else:
                serialUtil.lights_message( val['light_pin'] + 'f')

        for key,val in pins.items():
            serialUtil.lights_message( val['light_pin'] + 'f')

        self.addButton(button_key)

    def run(self):
        global current_state
        if self.start_time + BuildMessage.timeout < time.time():
            current_state = Idle()

    def addButton(self, key):
        logging.debug("BuildMessage: Adding " + key)
        serialUtil.lights_message( pins[key]['light_pin'] + 'n')
        self.buttons += pins[key]['strip_code']

    def handleSymbolButton(self, key):
        global current_state
        global my_lamp
        if pins[key]['strip_code'] & self.buttons:
            current_state = HandleMessage(my_lamp, "", self.buttons)
        else:
            self.addButton(key)

    def handleLampButton(self, key):
        global current_state
        if lamps[key]["online"]:
            current_state = SendMessage(key, self.buttons)


class SendMessage(State):
    global my_lamp
    timeout = int(lampiConfig['send_message_timeout'])

    def __init__(self, lamp_key, buttons):
        logging.debug("Entering SendMessage state")
        self.start_time = time.time()

        for key,val in lamps.items():
            if lamp_key == key:
                serialUtil.lights_message( val['light_pin'] + 's')
            else:
                serialUtil.lights_message( val['light_pin'] + 'f')

        button_key = ""
        for key,val in pins.items():
            if buttons & val['strip_code']:
                serialUtil.lights_message( val['light_pin'] + 's')
                if (button_key == ""):
                    button_key = key
            else:
                serialUtil.lights_message( val['light_pin'] + 'f')

        serialUtil.lights_message( 'su')

        try:
            logging.debug("SendMessage: button_key: {}\nbuttons: {}\nto: {}".format(button_key, buttons, lamp_key))
            messagesCollection.insert({"lampId": lamp_key, "from": my_lamp, "message": button_key, "stripCode": buttons, "time": time.time(), "handled": False })
        except Exception as e:
            logging.error('failure sending lamp message.')
            if hasattr(e, 'message'):
                logging.error(e.message)
            else:
                logging.error(e)

    def run(self):
        global current_state
        if self.start_time + SendMessage.timeout < time.time():
            current_state = Idle()


class HandleMessage(State):
    timeout = int(lampiConfig['handle_message_timeout'])

    def __init__(self, lamp_key, button_key, buttons):
        logging.debug("Entering HandleMessage state")
        self.start_time = time.time()

        for key,val in lamps.items():
            if lamp_key == key:
                serialUtil.lights_message( val['light_pin'] + 's')
            else:
                serialUtil.lights_message( val['light_pin'] + 'f')

        for key,val in pins.items():
            if button_key == key or ( buttons is not None and val['strip_code'] & buttons ):
                serialUtil.lights_message( val['light_pin'] + 's')
            else:
                serialUtil.lights_message( val['light_pin'] + 'f')

        key = 's'
        if (buttons is None):
            key += pins[button_key]["light_pin"]
        else:
            key += strip_codes[buttons]

        logging.debug("HandleMessage: showing: {}".format(key))
        serialUtil.lights_message( key)

    def run(self):
        global current_state
        if self.start_time + HandleMessage.timeout < time.time():
            current_state = Idle()


class ChangeIdle(State):
    timeout = int(lampiConfig['change_idle_timeout'])

    def __init__(self):
        logging.debug("Entering ChangeIdle state")
        self.start_time = time.time()

        serialUtil.lights_message( '1f2b3c4f')

    def run(self):
        global current_state
        if self.start_time + BuildMessage.timeout < time.time():
            current_state = Idle()

    def handleSymbolButton(self, key):
        if key == "b2":
            self.start_time = time.time()
            serialUtil.lights_message( 'z-')
        elif key == "b3":
            self.start_time = time.time()
            serialUtil.lights_message( 'z+')

main()
