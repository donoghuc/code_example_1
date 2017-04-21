#! /usr/bin/python3

# Cas Donoghue
# CS372 
# Project 1
# 12Feb2017
# This is a class for sockets. It is used for the server and client side. 
# i got the idea from: https://docs.python.org/3/howto/sockets.html
# basically just followed the docs. 

import socket
import sys


class MySocket:
    """class for message app. 
    """

    def __init__(self, sock=None):
        if sock is None:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)              
        else:
            self.sock = sock

    def client_connect(self, host, port):
        try: 
            self.sock.connect((host, port))
        except socket.error as e:
            print(str(e))
            sys.exit()
    # listen function. 
    def server_listen(self, host, port, connections):
        try: 
            self.sock.bind((host, port))
        except socket.error as e:
            print(str(e))
            sys.exit()
        self.sock.listen(connections)
    # super simple accept function (returns conn, addr)
    def server_accept(self):
        return self.sock.accept()
    # python3 has a sendall so you dont have to worry about writing one yourself
    def client_send(self, msg):
        self.sock.sendall(str.encode(msg))
    # subtle diff between client and server
    def server_send(self, conn, msg):
        conn.sendall(str.encode(msg))
    # use my system of prepending expected message len to ensure you get the whole message. 
    def server_receive(self, conn):
        chunks = []
        bytes_expected = ''
        bytes_recd = 0

        while True:
            data = conn.recv(3).decode()
            for x in range(len(data)):
                if data[x] != 'x':
                    bytes_expected = bytes_expected + data[x]

            if not data:
                break

            bytes_expected = int(bytes_expected)
            while bytes_recd < bytes_expected:
                chunk = conn.recv(min(bytes_expected - bytes_recd, 2048)).decode()
                chunks.append(chunk)
                bytes_recd = bytes_recd + len(chunk)
            
            return ''.join(chunks)
    # again use the prepend message idea
    def client_recv(self):
        chunks = []
        bytes_expected = ''
        bytes_recd = 0

        while True:
            data = self.sock.recv(3).decode()
            for x in range(len(data)):
                if data[x] != 'x':
                    bytes_expected = bytes_expected + data[x]

            if not data:
                break

            bytes_expected = int(bytes_expected)
            while bytes_recd < bytes_expected:
                chunk = self.sock.recv(min(bytes_expected - bytes_recd, 2048)).decode()
                chunks.append(chunk)
                bytes_recd = bytes_recd + len(chunk)

            return ''.join(chunks)
    # simply close up connections when done. 
    def client_close(self):
        self.sock.close()

    def server_close(self, conn):
        conn.close()
