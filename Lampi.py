import time
import configparser

print("Running Lampi")

config = configparser.ConfigParser()
config.read('config.ini')

lampiConfig = config['Lampi']
interval = float(lampiConfig['RunLoopInterval'])

while (True):
    time.sleep(interval)
