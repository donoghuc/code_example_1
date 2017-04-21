# Code Example
I wrote this simple chat client for a class. I do not want to make it easily searchable for other students so I waited until the class was over and have tried to make the repo name non-descript. I want to demonstrate some python and c code that I wrote recently to beef up my resume. 
# General notes
The approach i took for this project was to write everything in python3 first because it is way easier for me to sketch ideas out in python than in c. Once i was satisfied with the project written in python3 i implemented the client side in c. I choose to cram everything in one c source file instead of separating out. I did modularize with functions, just didnt think this project needed an extra .o and .h file for the .c source code. 

# Compile insructions
chatclient: simply type 'make' in directory with my code

chatserve and py_client (if you care): you can just run it the way shown below or choose to 
chmod a+x if you want please note that it is in python3 (i think osu servers default to python 2)

# Execution instructions
### SERVER
1. open a terminal and navigate to directory with source code
2. start server with:
```
	>> python3 chatserve.py [port number]  
```

EXAMPLE CALL
  
```
	>>python3 chatserve.py 3490
```
3. chatserve will tell you the name and port it is running on. NOTE THE SERVER HOST NAME AND PORT NUM for client process

### CLIENT
4. open a new terminal and navigate to directory with source code
5. start chatclient with:
```
	>>./chatclient [hostname] [host port number]
```

EXAMPLE CALL
```
	>>./chatclient flip2.engr.oregonstate.edu 3490
```
# Expected runtime rules
chatclient will ask you for a user name
chatclient will ask for an initial message to send
chatclient will send that message to chatserve
chatserve will recieve message and display it
chatserve will wait for a user to type in return message
once chatserve user has entered message it is sent to chat client
the conversation continues in that order

NOTE: you must repect the order of the chat ie, wait until the other end has returned a message
before you can reply (otherwise it gets thrown off cadence)

# Quitting
to quit on either chatserve or chatclient durring a conversation, wait until it is 
your turn to write a message and type '/quit' without the quotes. If you quit from the server the client 
process will close and the server will be ready to accept a new client (similar if you quit from the client).
To shut down the server just ^c (sig interupt) and the script will close up in a way that closes the socket. 
