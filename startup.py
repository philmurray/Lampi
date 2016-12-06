#!/usr/bin/python3

import configparser
import time
import internet
import subprocess

print("Running startup.py")

config = configparser.ConfigParser()
config.read('config.ini')
startupConfig = config['Startup']


def callStartup():
	if (internet.check()):
        	subprocess.call("/home/pi/Lampi/startup.sh")


interval = float(startupConfig['InternetCheckInterval'])
internetTimeout = time.time() + int(startupConfig['InternetWaitTimeout'])

while (not internet.check() and time.time() < internetTimeout):
	print("No internet, waiting")
	time.sleep(interval)

if (not internet.check()):
	print("internet timeout")
	subprocess.call(['wpa_cli', 'wps_pbc'])
	wpsTimeout = time.time() + int(startupConfig['WPSWaitTimeout'])
	while (not internet.check() and time.time() < wpsTimeout):
		print("Still no internet, waiting for wps")
		time.sleep(interval)
	callStartup()
else:
	callStartup()


import Lampi
