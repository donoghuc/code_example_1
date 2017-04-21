#! /usr/bin/python3

import socket
import sys  
from termios import tcflush, TCIFLUSH
import socket_class

def main():
	# host = socket.gethostname()
	# port = sys.argv[1]
	host = socket.gethostbyname(sys.argv[1])
	port = sys.argv[2]
	socket_connection = socket_class.MySocket()
	socket_connection.client_connect(host, int(port))
	print('Connected with: {} on port: {}'.format(host, port))

	name_info = get_user_handle()

	while True:
		current_message = get_message_to_send(name_info)
		if current_message == "\quit":
			print("i fucking quit from the cleint")
			socket_connection.client_send(format_quit_message(current_message))
			break
		else:
			socket_connection.client_send(current_message)
			current_message = socket_connection.client_recv()
			if current_message == "\quit":
				print('{} left the chat'.format(host))
				break
			else:
				print(current_message)
				
	socket_connection.client_close()


def get_user_handle():
	user_name = input('Enter chat name (10 character max): ')
	while not user_name or (len(user_name) > 10):
		user_name = input('Enter chat name (10 character max:): ')
	return user_name  + '-> '


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

def format_quit_message(quit_msg):
	quit_msg_len_str = pad_message_length(len(quit_msg)) 
	return quit_msg_len_str + quit_msg


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


if __name__ == '__main__':
    main()