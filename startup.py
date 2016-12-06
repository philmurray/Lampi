#!/usr/bin/python

import configparser
import time
import internet

print("Running startup.py")

config = configparser.ConfigParser()
config.read('config.ini')

startupConfig = config['Startup']
interval = int(startupConfig['InternetCheckInterval'])
internetTimeout = time.time() + int(startupConfig['InternetWaitTimeout'])

while (!internet.check() && time.time() < internetTimeout):
	print("No internet, waiting")
	time.sleep(interval)

if (!internet.check()):
	print("internet timeout")
	subprocess.call(['wpa_cli', 'wps_pbc'])
	wpsTimeout = time.time() + int(startupConfig['WPSWaitTimeout'])
	while (!internet.check() && time.time() < wpsTimeout):
		print("Still no internet")
		time.sleep(interval)

if (internet.check()):
	subprocess.call("/home/pi/Lampi/startup.sh")

import Lampi
