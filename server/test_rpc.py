import zmq
import time
import sys
import threading

class server(threading.Thread):

	def __init__(self, socket, ip, port):
		threading.Thread.__init__(self)
		self.socket = socket
		self.ip = ip
		self.port = port

	def respond(self):
		self.socket.send("Test response");

	def run(self):
		self.socket.bind("tcp://{0}:{1}".format(self.ip, self.port))
		print("Serving...")
		while True:
			message = self.socket.recv()
			print "Received Message:", message
			self.respond()
	

if __name__ == "__main__":
	context = zmq.Context()
	socket = context.socket(zmq.REP)
	server = server(socket, "127.0.0.1", sys.argv[1])
	server.run()


	# server1 = server(socket, "127.0.0.1", 7643)
	# server2 = server(socket, "127.0.0.1", 7644)
	# server1.start()
	# server2.start()
	# server1.join()
	# server2.join()
