import zmq
import time
import sys
import threading

class Server(threading.Thread):

	def __init__(self, context, ip, port):
		threading.Thread.__init__(self)
		self.socket = context.socket(zmq.ROUTER)
		self.ip = ip
		self.port = port

	def respond(self, msg):
		self.socket.send(msg.identity, flags=zmq.SNDMORE)
		self.socket.send("", flags=zmq.SNDMORE)
		self.socket.send(msg.content);

	def handle_message(self, msg):
		# Do work
		print "Received Message:", msg
		msg.set_content("Acknowledged!")
		time.sleep(2)
		return msg

	def run(self):
		self.socket.bind("tcp://{0}:{1}".format(self.ip, self.port))
		print("Serving...")
		while True:
			routing_identity = self.socket.recv()
			# Receive delimiter between identity and content
			self.socket.recv()
			msg_id = self.socket.recv()
			content = self.socket.recv()
			received_msg = Message(routing_identity, msg_id, content)
			response = self.handle_message(received_msg)
			response.send(self.socket)

class Message:

	def __init__(self, routing_identity, msg_id, content):
		self.identity = routing_identity
		self.msg_id = msg_id
		self.content = content

	def __str__(self):
		return self.content

	def set_content(self, content):
		self.content = content

	def send(self, socket):
		socket.send(self.identity, flags=zmq.SNDMORE)
		socket.send("", flags=zmq.SNDMORE)
		socket.send(self.msg_id, flags=zmq.SNDMORE)
		socket.send(self.content)
		print self.content


if __name__ == "__main__":
	context = zmq.Context()
	server = Server(context, "127.0.0.1", sys.argv[1])
	server.run()



	# server1 = server(socket, "127.0.0.1", 7643)
	# server2 = server(socket, "127.0.0.1", 7644)
	# server1.start()
	# server2.start()
	# server1.join()
	# server2.join()
