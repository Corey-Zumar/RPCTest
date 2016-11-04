import zmq
import time
import sys

if __name__ == "__main__":
	clipper_address = "tcp://127.0.0.1:{}".format(sys.argv[1])
	context = zmq.Context()
	socket = context.socket(zmq.REQ)
	socket.connect(clipper_address)
	socket.send("tcp://127.0.0.1:{}".format(sys.argv[2]))
	reply = socket.recv()
	print reply
