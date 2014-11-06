
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
char message[SAY_MAX];
int recvlen;
int serverSocket;
unsigned char sendmessage;
#define MAX_USERS 100
#define MAX_CHANNELS 32 
int usercount = 0;
socklen_t addrlen;



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


struct sockaddr_in hostaddr;		
struct sockaddr remaddr;

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
	/*int i;
	int f = chan.numusers;
	for (i = 0; i < f; i++){
		struct User utemp = Channel.cur_usernames[i];
		struct sockaddr temp = utemp.cur_address;

		sendto(serverSocket, mesg, strlen(mesg), 0, (struct sockaddr *)&temp, sizeof(temp));
	}

}
*/

void login(struct request reqmes){
	printf("join\n");
	struct request_login *reqlogin;
	reqlogin = (struct request_login *)&reqmes;

	/*//printf("%s\n", username);
	struct User user;
	user.cur_username = username;
	user.cur_address = remaddr;
	//user.cur_channels[0] = Common;
	user.numchannels += 1;
	all_Users[0] = user;
	int i;
	i = Common.numusers;
	Common.cur_usernames[i] = user;
	Common.numusers += 1;
	//all_Users[0].cur_address = remaddr;
	usercount += 1;
	*/

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

void recieve(){
	struct request request_message;

	addrlen = sizeof(remaddr);
	recvlen = recvfrom(serverSocket, &request_message, sizeof(request_message), 0, (struct sockaddr *)&remaddr, &addrlen );
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
	else{

		all_Channels[0] = Common;
		Common.numusers = 0;

		serverAddr = argv[1];
		serverPort = atoi(argv[2]);

		//printf("%s\n", serverAddr);
		//printf("%d\n", serverPort);

		memset((char *)&hostaddr, 0, sizeof(hostaddr));
		hostaddr.sin_family = AF_INET;
		//hostaddr.sin_addr.s_addr = inet_addr(serverAddr);
		hostaddr.sin_addr.s_addr = htonl(INADDR_ANY); //I DONT KNOW!!!!
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

		struct timeval tv;
		fd_set readfds;
		fd_set master;
		tv.tv_sec = 3;
		int fdmax;

		FD_ZERO(&readfds);
		FD_ZERO(&master);
		//FD_SET(STDIN, &master);
		FD_SET(serverSocket, &master);
		fdmax = serverSocket;
		
		printf("waiting for connection\n");
		while(Run == 1){	
			int temp;
			temp = select(fdmax + 1, &readfds, NULL, NULL, &tv);
			if (FD_ISSET(serverSocket, &readfds)){
				printf("connection\n");
				recieve();
			}

		};
	close(serverSocket);
	return 0;
	};
	
}