import socket
import time

def check_internet(host="8.8.8.8", port=53, timeout=3):
	try:
		socket.setdefaulttimeout(timeout)
		socket.socket(socket.AF_INET, socket.SOCK_STREAM).connect((host, port))
		return True
	except Exception as ex:
		return False

class SerialUtil:
	states = {}
	def __init__(self, serial):
		self.serial = serial

	def lights_message(self, message, wait=True):
		m = ''
		for c in message:
			m += c
			if len(m) == 2:
				if SerialUtil.states.get(m[0]) != m[1]:
					SerialUtil.states[m[0]] = m[1]
					serial.write(bytes(m, 'UTF-8'))
					if wait:
						time.sleep(0.005)
				m = ''
