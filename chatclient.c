/* Cas Donoghue
CS372 Project 1
12Feb2017
This is the client side program of the chat. Once the server is started call this program with:
./client [server name] [port number]
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define USER_HANDLE_LEN 14
#define MESSAGE_LEN 550

void error(const char *msg)
{
    perror(msg);
    //exit(1);
}

// forward function definitions
//functions for communication with server
void send_message(int sockfd, char *message_to_send);
int receive_message(int sockfd, char *message_to_receive);
int sendall(int s, char *buf, int *len);
int recvall(int s, char *buf, int *len); 
//functions for user interface and formatting c strings
void c_input(char *var_to_fill, int max_size);
void get_user_handle(char *user_handle);
void get_raw_message(char *user_message, char *user_handle);
void make_pad_string(char *pad_string, int len);
void finalize_out_message(char *finalized_outging_message, char *pad_string, char *user_handle, char *user_message);


int main(int argc, char *argv[]){
    //variable declarations for connecting with server
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

	//variable declarations for client side interface and formatting of messages
	char user_handle[USER_HANDLE_LEN];
	char user_message[MESSAGE_LEN];
	char pad_string[4];
	char finalized_outging_message[MESSAGE_LEN];  //switch to malloc? 
	int outgoing_msg_len = 0; 
	char *recieved_message;

	// make the connection with server
	// note, this idea was modified from my operating systems project 4
	// there was some instructor provided TCP connect code, this uses the same
	// logic
	server = gethostbyname(argv[1]); 
    portno = atoi(argv[2]); 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    //query for user name
	get_user_handle(user_handle);
	// main loop. converse until you quit or server quits. 
	while(1){
		fflush(NULL);
		get_raw_message(user_message, user_handle);
		if(strcmp(user_message, "\\quit") == 0){
			outgoing_msg_len = (int)(strlen(user_message));
			make_pad_string(pad_string, outgoing_msg_len);
			char *empty_string = "";
			finalize_out_message(finalized_outging_message, pad_string, empty_string, user_message);
			send_message(sockfd, finalized_outging_message);
			break;
		}
		else {
			outgoing_msg_len = (int)(strlen(user_handle)) + (int)(strlen(user_message));
			make_pad_string(pad_string, outgoing_msg_len);
			finalize_out_message(finalized_outging_message, pad_string, user_handle, user_message);
			send_message(sockfd, finalized_outging_message);
		}

		if(receive_message(sockfd, recieved_message) == -1){
			printf("Server Side Quit.\n");
			break;
		}
	}
	close(sockfd);
	return 0;
}

/*sendall function
SOURCE: Beej's guide: advance socket tutorial
http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#setsockoptman
I literally just copy pasted from Beej's guide. Works awesome, sleek way of
making sure that all data has been sent. 
*/
int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
} 
/*adapted from Beej's guide. just change send to recv. Simple as that. lol*/
int recvall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = recv(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
} 

/*wrapped the send all for my formatted message to send. */
void send_message(int sockfd, char *message_to_send){
    int numAsStringLen = strlen(message_to_send); 
    int n;
    n = sendall(sockfd, message_to_send, &numAsStringLen);
}

/*wrapper around the recvall from beej to interperate my message format */
int receive_message(int sockfd, char *message_to_receive){
    int prepended_len = 3;
    int extracted_val = 0;
    int n;
    char buffer[4];
    bzero(buffer, 4);
    n = recvall(sockfd, buffer, &prepended_len);
    int i = 0;
    for(i = 0; i < 3; i++){
        if(buffer[i] == 'x'){
            buffer[i] = '\0';
        }
    }
    extracted_val = atoi(buffer);
    message_to_receive = (char*) malloc((extracted_val + 1) * sizeof(char));
    n = recvall(sockfd, message_to_receive, &extracted_val);
    message_to_receive[extracted_val] = '\0';

    if(strcmp(message_to_receive, "\\quit") == 0){
    	return -1;
    }
    else{
    	printf("%s\n", message_to_receive);
    }
    free(message_to_receive);
    return 0;
}

/* dealing c strings. with it try to make as easy as possible */
void c_input(char *var_to_fill, int max_size){
	fflush(stdin);
	fflush(stdout);
	fgets(var_to_fill, max_size, stdin);
    if ((strlen(var_to_fill)>0) && (var_to_fill[strlen (var_to_fill) - 1] == '\n'))
        var_to_fill[strlen (var_to_fill) - 1] = '\0';
    if(strlen(var_to_fill) < 1){
    	c_input(var_to_fill, max_size);
    }
}

/*pretty simple. just get the user handle and format it with '->' */
void get_user_handle(char *user_handle){
	int format_counter;
	char handle_format[4] = {'-', '>', ' ', '\0'}; 
	int format_indexer = 0; 
	int i; //fucking c are you serious with this shit?

	printf("Enter chat name (10 character max): ");
	fflush(stdout);
	c_input(user_handle, 11);
	format_counter = (int)(strlen(user_handle));
	for(i = format_counter; i < format_counter + 4; i++){
		user_handle[i] = handle_format[format_indexer];
		format_indexer ++;
	};
}

/*get the message to send. */
void get_raw_message(char *user_message, char *user_handle){
	fflush(stdin);
	printf("%s", user_handle);
	fflush(stdout);
	c_input(user_message, 501);
}

/*this part will make the prepended info to every message,
the purpose of this is to let the recieving size know how long of
a message is expected */
void make_pad_string(char *pad_string, int len){
	// len = len + 4; 
	if (len < 10){
		sprintf(pad_string, "%d", len);
		pad_string[1] = 'x';
		pad_string[2] = 'x';
		pad_string[3] = '\0';
	}
	else if (len < 100){
		sprintf(pad_string, "%d", len);
		pad_string[2] = 'x';
		pad_string[3] = '\0';
	}
	else if (len < 1000){
		sprintf(pad_string, "%d", len);
	}
}

/* this is the final formatting of the message to send. packs it and gets it ready to 
send to the server in a format that is expected (prepended info, user handle, message)*/
void finalize_out_message(char *finalized_outging_message, char *pad_string, char *user_handle, char *user_message){
	int fill_index = 0;
	int pad_len = (int)(strlen(pad_string));
	int handle_len = (int)(strlen(user_handle));
	int msg_len = (int)(strlen(user_message));
	int i; 
	for(i = 0; i < pad_len; i++){
		finalized_outging_message[fill_index] = pad_string[i];
		fill_index ++; 
	}
	for(i = 0; i < handle_len; i++){
		finalized_outging_message[fill_index] = user_handle[i];
		fill_index ++; 
	}
	for(i = 0; i < msg_len; i++){
		finalized_outging_message[fill_index] = user_message[i];
		fill_index ++; 
	}
	finalized_outging_message[fill_index] = '\0';
}



