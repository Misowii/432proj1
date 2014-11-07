
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
//#define MAX_USERS 100 //max users that can connect
//#define MAX_CHANNELS 32  //max channels open
//int usercount = 0;
socklen_t addrlen;

struct hostent *hp;
struct sockaddr_in hostaddr;		
struct sockaddr remaddr;

//Attempts at data structures for keeping track of channels and users
struct User {
	char username[USERNAME_MAX];
	struct User * nextuser;
	struct sockaddr address;
};


struct Channel {
	char channelname[CHANNEL_MAX];
	struct User *users;
	struct Channel * nextchannel;
};

	struct Channel *rootchannel;
	struct Channel *channelsearch;
	struct User *rootUser;
	struct User *Usersearch;

void createchannel(struct Channel *chansearch, char *channelName){
	while (chansearch -> nextchannel != NULL){
		chansearch = chansearch->nextchannel;
	}
	chansearch->nextchannel = malloc( sizeof(struct Channel));
	chansearch = chansearch->nextchannel;
	chansearch->nextchannel = NULL;
	strcpy(chansearch->channelname, channelName);
}


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
	//printf("say\n");
	//char temp[SAY_MAX];
	struct request_say *reqsay;
	struct text_say say;
	reqsay = (struct request_say *)&reqmes;
	say.txt_type = REQ_SAY;
	strcpy(say.txt_text, reqsay->req_text);
	//strcpy(say.txt_username, reqsay->req_username);
	strcpy(say.txt_channel, reqsay->req_channel);
	struct sockaddr tempaddr;
	int flag;
	flag = 0;
	Usersearch = rootUser->nextuser;
	while (flag){
		tempaddr = Usersearch->address;
		sendto(serverSocket, &say, sizeof(say), 0, (struct sockaddr *)&tempaddr, sizeof(tempaddr));

		if (Usersearch->nextuser != NULL){
			Usersearch = Usersearch->nextuser;
		}
		else{
			flag = 1;
		}
	}

}

void join(struct request reqmes){
	//printf("join\n");
	struct request_join *reqjoin;
	reqjoin = (struct request_join *)&reqmes;



}
void logout(struct request reqmes){
	//printf("logout\n");
	//char temp[USERNAME_MAX];
	struct request_logout *reqlogout;
	reqlogout = (struct request_logout *)&reqmes;
	//strcpy(temp, reqlogout->req_username);

	printf("User has logged out.\n");

}

void leave(struct request reqmes){
	printf("leave\n");
	struct request_leave *reqleave;
	reqleave = (struct request_leave *)&reqmes;
}
void newUser(char *userna){
	struct User *tempusearch;
	tempusearch = rootchannel->users;
	while (Usersearch->nextuser != NULL){
		Usersearch = Usersearch->nextuser;
	}
	Usersearch->nextuser = malloc(sizeof(struct User));
	Usersearch = Usersearch->nextuser;
	Usersearch->nextuser = NULL;
	strcpy(Usersearch->username, userna);
	Usersearch->address = remaddr;

	printf("%s has logged in.\n", userna);
}



void login(struct request reqmes){
	char temp[USERNAME_MAX];
	struct request_login *reqlogin;
	reqlogin = (struct request_login *)&reqmes;
	strcpy(temp, reqlogin->req_username);

	newUser(reqlogin->req_username);
	//printf("User %s has logged in\n", temp);

}

//Once recvfrom works it sends the basic request struct to here for
//further deciphering.
//All login/logout/say/join/who/list/leave functions just print
//what function they are. (for now)
void recieve(struct request request_message){
	//printf("got to recieve\n");

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


		serverAddr = argv[1];
		serverPort = atoi(argv[2]);

		//Setting up server information and socket
 
		hp = gethostbyname(serverAddr);

		memset((char *)&hostaddr, 0, sizeof(hostaddr));
		hostaddr.sin_family = AF_INET;
		memcpy((void *)&hostaddr.sin_addr, hp->h_addr_list[0], hp->h_length);
		hostaddr.sin_port = htons(serverPort);

		if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
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


		rootchannel = malloc(sizeof(struct Channel));
		//printf("after malloc chan\n");
		strcpy(rootchannel->channelname, "Common");
		//printf("after cpy chan\n");
		channelsearch = rootchannel;
		struct User *firstcommon;
		firstcommon = malloc(sizeof(struct User));
		rootchannel->users = firstcommon;
		//printf("after channel\n");
		rootUser = malloc(sizeof(struct User));
		//rootUser->address = hostaddr;
		strcpy(rootUser->username,"ROOT");
		Usersearch = rootUser;
		//printf("after user\n");
		while (1){


		//Attempting to recieve a message then pass the request struct to
		//recieve() to decifer the type.

		struct request request_message;	
		addrlen = sizeof(remaddr);
		int temp;
		temp = recvfrom(serverSocket, &request_message, sizeof(request_message), 0, (struct sockaddr *)&remaddr, &addrlen);
		if ( temp > 0 ){
			recieve(request_message);
		}
		
		};
	close(serverSocket);
	return 0;

	
}