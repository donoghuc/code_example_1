#! /usr/bin/python3

# Cas Donoghue
# CS372
# Project1
# 12Feb2017
# This is the server side of the chat. call using:
# python3 serve.py [port number]


import socket
import sys  
import signal
import atexit
from termios import tcflush, TCIFLUSH
import socket_class

def main():
	host = socket.gethostname()
	port = sys.argv[1]
	server_name = 'imaginary friend-> '
	#set up to catch sig interrupt (to close down safely)
	signal.signal(signal.SIGINT, handler)
	# server_socket uses MySocket class
	server_socket = socket_class.MySocket()
	print('Server starting on host: {}, port: {}'.format(host, port))
	# use a MySocket method to listen on specified port
	server_socket.server_listen(host, int(port), 1)
	# just used to close down my socket when signal int is caught
	atexit.register(close_on_sigint, server_socket)
	# main loop. handle connection, communicate
	while True:
		conn, addr = server_socket.server_accept()
		while True:
			current_message = server_socket.server_receive(conn)
			if current_message == "\quit":
				print('client quit')
				break
			else:
				print(current_message)
				current_message = get_message_to_send(server_name)
				if current_message == "\quit":
					print("quit from the server")
					server_socket.server_send(conn, format_quit_message(current_message))
					break
				server_socket.server_send(conn, current_message)
		server_socket.server_close(conn)
	
# get a message to send to the client side, format for appropriate send
def get_message_to_send(name_info):
	tcflush(sys.stdin, TCIFLUSH)
	input_message = input(name_info)
	if input_message == "\quit":
		return input_message
	else:
		build_message = name_info + input_message
		padded_info = pad_message_length(len(build_message))
		while padded_info == -1:
			get_message_to_send(name_info)
		packed_message = padded_info + build_message
		return packed_message

# format the /quit message to send ot client letting them know server is done chatting
def format_quit_message(quit_msg):
	quit_msg_len_str = pad_message_length(len(quit_msg)) 
	return quit_msg_len_str + quit_msg

# this is the format for ensuring entire message is recieved. it prepends expected len to message
def pad_message_length(message_len):
	if message_len < 10:
		padded = str(message_len) + 'xx'
	elif message_len < 100:
		padded = str(message_len) + 'x'
	elif message_len < 990:
		padded = str(message_len)
	else:
		return -1
	return padded

# catch the sig int
def handler(signum, frame):
	print("you killed server with cntr+c")
	sys.exit(0)

# close the socket on exit so it can be reused instantly. 
def close_on_sigint(server_socket):
	print('closing socket...')
	server_socket.client_close()


if __name__ == '__main__':
    main()