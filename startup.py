#!/usr/bin/python

import socket
import RPi.GPIO as GPIO

def internet(host="8.8.8.8", port=53, timeout=3):
	try:
		socket.setdefaulttimeout(timeout)
		socket.socket(socket.AF_INET, socket.SOCK_STREAM).connect((host, port))
		return True
	except Exception as ex:
		print ex.message
		return False


GPIO.setmode(GPIO.BCM)

while (true):
	if (internet()):
		break
		subprocess.call("/home/pi/Lampi/startup.sh")
		import
	
