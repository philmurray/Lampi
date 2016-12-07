import os
import time
import configparser
import logging

logging.basicConfig(format='%(asctime)s %(levelname)s:%(message)s')
logging.info("Running Lampi")

config = configparser.ConfigParser()
config.read(os.path.join(os.path.abspath(os.path.dirname(__file__)), 'config.ini'))

lampiConfig = config['Lampi']
interval = float(lampiConfig['RunLoopInterval'])

while (True):
    time.sleep(interval)
