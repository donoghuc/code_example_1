all: chatclient

chatclient: chatclient.c
	gcc -g -o chatclient chatclient.c

clean:
	rm chatclient