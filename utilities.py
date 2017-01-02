import socket
import time
import logging

logging.basicConfig(format='%(asctime)s %(levelname)s:%(message)s')

def check_internet(host="8.8.8.8", port=53, timeout=3):
	try:
		socket.setdefaulttimeout(timeout)
		socket.socket(socket.AF_INET, socket.SOCK_STREAM).connect((host, port))
		return True
	except Exception as ex:
		return False

def lights_message(serial, message, wait=True):
	m = ''
	for c in message:
		m += c
		if len(m) == 2:
			logging.debug('writing to serial: ' + m)
			serial.write(bytes(m, 'UTF-8'))
			if wait:
				time.sleep(0.01)
			m = ''
