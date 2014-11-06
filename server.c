
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "duckchat.h"

int Run = 1;
char *serverAddr;
int serverPort;
int recvlen;
int serverSocket;
#define MAX_USERS 100 //max users that can connect
#define MAX_CHANNELS 32  //max channels open
int usercount = 0;
socklen_t addrlen;

struct hostent *hp;
struct sockaddr_in hostaddr;		
struct sockaddr remaddr;

//Attempts at data structures for keeping track of channels and users
struct User{
	char *cur_username;
	//struct Channel cur_channels[MAX_CHANNELS];
	int numchannels;
	struct sockaddr cur_address;
};

struct Channel{
	char cur_channel[CHANNEL_MAX];
	struct User cur_usernames[MAX_USERS];
	int numusers;
};

struct User all_Users[MAX_USERS];
struct Channel all_Channels[MAX_CHANNELS];

struct Channel Common;




void list(struct request reqmes){
	printf("list\n");
	struct request_list *reqlist;
	reqlist = (struct request_list *)&reqmes;

}

void who(struct request reqmes){
	printf("who\n");
	struct request_who *reqwho;
	reqwho = (struct request_who *)&reqmes;
}

void say(struct request reqmes){
	printf("say\n");
	struct request_say *reqsay;
	reqsay = (struct request_say *)&reqmes;
}


void login(struct request reqmes){
	printf("join\n");
	struct request_login *reqlogin;
	reqlogin = (struct request_login *)&reqmes;
}

void join(struct request reqmes){
	printf("join\n");
	struct request_join *reqjoin;
	reqjoin = (struct request_join *)&reqmes;

}
void logout(struct request reqmes){
	printf("logout\n");
	struct request_logout *reqlogout;
	reqlogout = (struct request_logout *)&reqmes;
}

void leave(struct request reqmes){
	printf("leave\n");
	struct request_leave *reqleave;
	reqleave = (struct request_leave *)&reqmes;
}


//Once recvfrom works it sends the basic request struct to here for
//further deciphering.
//All login/logout/say/join/who/list/leave functions just print
//what function they are. (for now)
void recieve(struct request request_message){
	printf("got to recieve\n");

	if (request_message.req_type == REQ_LOGIN){
		login(request_message);
	}
	else if (request_message.req_type == REQ_LOGOUT){
		logout(request_message);
	}
	else if (request_message.req_type == REQ_SAY){
		say(request_message);
	}
	else if (request_message.req_type == REQ_JOIN){
		join(request_message);
	}
	else if (request_message.req_type == REQ_WHO){
		who(request_message);
	}
	else if (request_message.req_type == REQ_LIST){
		list(request_message);
	}	
	else if (request_message.req_type == REQ_LEAVE){
		leave(request_message);
	}		

}

int main(int argc, char *argv[])
{

	if (argc != 3)
	{
		printf("Server requires 2 arguments. Address and Port.\n");
		return(-1);

	}	
		//creates the first channel and adds it to list of active channels
		//Also sets the channel common to have 0 users on it.
		all_Channels[0] = Common;
		Common.numusers = 0;

		serverAddr = argv[1];
		serverPort = atoi(argv[2]);

		//Setting up server information and socket
 
		hp = gethostbyname(serverAddr);

		memset((char *)&hostaddr, 0, sizeof(hostaddr));
		hostaddr.sin_family = AF_INET;
		memcpy((void *)&hostaddr.sin_addr, hp->h_addr_list[0], hp->h_length);
		hostaddr.sin_port = htons(serverPort);

		if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			printf("socket failed\n");
			return(-1);
		};

		if (bind(serverSocket, (struct sockaddr *)&hostaddr, sizeof(hostaddr)) == -1){
			printf("bind failded\n");
			perror("Error: ");
			close(serverSocket);
			return(-1);
		};

		//loop while waiting for messages?
		
		printf("waiting for connection\n");
		while(Run == 1){


		//Attempting to recieve a message then pass the request struct to
		//recieve() to decifer the type.

		struct request request_message;	
		addrlen = sizeof(remaddr);
		int temp;
		temp = recvfrom(serverSocket, &request_message, sizeof(request_message), 0, (struct sockaddr *)&remaddr, &addrlen);
		if ( temp > 0 ){
			recieve(request_message);


		//Attempt at select() for server. Might not need to use select
		// for the server.	

		/*struct timeval tv;
		fd_set readfds;
		fd_set master;
		tv.tv_sec = 5;
		int fdmax;

		FD_ZERO(&readfds);
		FD_ZERO(&master);
		//FD_SET(STDIN, &master);
		FD_SET(serverSocket, &master);
		fdmax = serverSocket;
		readfds = master;
		*/	
			//printf("Waiting for message\n");
			//if (select(fdmax + 1, &readfds, NULL, NULL, &tv) == -1){
			//	perror("select");
			//	exit(0);
			//}
			//int i;
			//for (i = 0; i <= fdmax; i++){	
			//	if (FD_ISSET(i, &readfds)){
			//		printf("connection\n");



		}
		};
	close(serverSocket);
	return 0;

	
}