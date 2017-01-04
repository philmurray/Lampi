#!/usr/bin/python3

import configparser
import time
import utilities
import subprocess
import os
import logging
import serial

logging.basicConfig(format='%(asctime)s %(levelname)s:%(message)s', level=logging.CRITICAL)

logging.info("Running startup.py")

cmddir = os.path.abspath(os.path.dirname(__file__))
config = configparser.ConfigParser()
config.read(os.path.join(cmddir, 'config.ini'))
startupConfig = config['Startup']
lampiConfig = config['Lampi']

ser = serial.Serial(lampiConfig['Serial'], int(lampiConfig['BaudRate']), timeout=1)
time.sleep(2)
utilities.lights_message(ser, '1n2b')

def callStartup():
	if (utilities.check_internet()):
		utilities.lights_message(ser, '2n3b')
		logging.info("Running startup.sh")
		p = subprocess.Popen(os.path.join(cmddir, 'startup.sh'), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		stdout, stderr = p.communicate()
		if stdout:
		    logging.info(stdout)
		if stderr:
		    logging.error(stderr)

		utilities.lights_message(ser, '3n4b')
		logging.info("Running arduino install")
		p = subprocess.Popen(os.path.join(cmddir, 'arduino/install.sh'), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		stdout, stderr = p.communicate()
		if stdout:
		    logging.info(stdout)
		if stderr:
		    logging.error(stderr)

		time.sleep(1)
		ser.close()

interval = float(startupConfig['InternetCheckInterval'])
internetTimeout = time.time() + int(startupConfig['InternetWaitTimeout'])

while (not utilities.check_internet() and time.time() < internetTimeout):
	logging.debug("No internet, waiting")
	time.sleep(interval)

if (not utilities.check_internet()):
	logging.info("internet timeout.  starting wps")
	utilities.lights_message(ser, '2s', False)
	subprocess.call(['/sbin/wpa_cli', 'wps_pbc'])
	wpsTimeout = time.time() + int(startupConfig['WPSWaitTimeout'])
	while (not utilities.check_internet() and time.time() < wpsTimeout):
		logging.debug("Still no internet, waiting for wps")
		time.sleep(interval)
	callStartup()
else:
	callStartup()


import Lampi
