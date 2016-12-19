import os
import time
import configparser
import logging
import serial

logging.basicConfig(format='%(asctime)s %(levelname)s:%(message)s')
logging.info("Running Lampi")

config = configparser.ConfigParser()
config.read(os.path.join(os.path.abspath(os.path.dirname(__file__)), 'config.ini'))

lampiConfig = config['Lampi']
interval = float(lampiConfig['RunLoopInterval'])

ser = serial.Serial(lampiConfig.Serial, int(lampiConfig.BaudRate), timeout=1)

ser.write('1o')
ser.write('2o')
ser.write('3o')
ser.write('4o')
while (True):
    time.sleep(interval)
