import socket
import time

def check_internet(host="8.8.8.8", port=53, timeout=3):
	try:
		socket.setdefaulttimeout(timeout)
		socket.socket(socket.AF_INET, socket.SOCK_STREAM).connect((host, port))
		return True
	except Exception as ex:
		return False

def lights_message(serial, message, wait=True)

	m = ''
	for c in message:
		m += c
		if len(m) == 2:
			serial.write(bytes(message, 'UTF-8'))
			if wait:
				time.sleep(0.1)
			m = ''
