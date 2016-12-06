import time

print("Running Lampi")

config = configparser.ConfigParser()
config.read('config.ini')

lampiConfig = config['Lampi']
interval = int(startupConfig['RunLoopInterval'])

while (true):
    time.sleep(interval)
