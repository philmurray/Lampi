#!/usr/bin/python3

import configparser
import time
import internet
import subprocess
import os
import logging
import serial

logging.basicConfig(format='%(asctime)s %(levelname)s:%(message)s', level=logging.DEBUG)

logging.info("Running startup.py")

cmddir = os.path.abspath(os.path.dirname(__file__))
config = configparser.ConfigParser()
config.read(os.path.join(cmddir, 'config.ini'))
startupConfig = config['Startup']
lampiConfig = config['Lampi']

ser = serial.Serial(lampiConfig['Serial'], int(lampiConfig['BaudRate']), timeout=1)
ser.write(bytes('1n2b', 'UTF-8'))

def callStartup():
	if (internet.check()):
		ser.write(bytes('2n3b', 'UTF-8'))
		logging.info("Running startup.sh")
		subprocess.call(os.path.join(cmddir, 'startup.sh'))
		ser.write(bytes('3n4b', 'UTF-8'))
		logging.info("Running arduino install")
		subprocess.call(os.path.join(cmddir, 'arduino/install.sh'))
		ser.close()


interval = float(startupConfig['InternetCheckInterval'])
internetTimeout = time.time() + int(startupConfig['InternetWaitTimeout'])

while (not internet.check() and time.time() < internetTimeout):
	logging.debug("No internet, waiting")
	time.sleep(interval)

if (not internet.check()):
	logging.info("internet timeout.  starting wps")
	ser.write(bytes('2s', 'UTF-8'))
	subprocess.call(['/sbin/wpa_cli', 'wps_pbc'])
	wpsTimeout = time.time() + int(startupConfig['WPSWaitTimeout'])
	while (not internet.check() and time.time() < wpsTimeout):
		logging.debug("Still no internet, waiting for wps")
		time.sleep(interval)
	callStartup()
else:
	callStartup()


import Lampi
