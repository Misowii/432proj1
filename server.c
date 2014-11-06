
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
unsigned char message[SAY_MAX];
int recvlen;
int serverSocket;
unsigned char sendmessage;
#define MAX_USERS 100
#define MAX_CHANNELS 32 
int usercount = 0;


struct User{
	char *cur_username;
	char cur_channels[MAX_CHANNELS][CHANNEL_MAX];
	struct sockaddr_in cur_address;
};

struct Channel{
	char cur_channel[CHANNEL_MAX];
	char cur_usernames[MAX_USERS][USERNAME_MAX];
};

struct User all_Users[MAX_USERS];
struct Channel all_Channels[MAX_CHANNELS];

struct Channel Common;

struct sockaddr_in hostaddr;		
struct sockaddr_in remaddr;

void list(){
	printf("list\n");
}

void who(){
	printf("who\n");
}

void say(){
	

}


void login(char *username){
	printf("%s\n", username);
	all_Users[0].cur_username = username;
	all_Users[0].cur_address = remaddr;
	usercount += 1;


}

void join(char *username, char *channel){
	printf("join\n");
	printf("%s\n", channel);
	printf("%s\n", username);

}

void switchto(){
	printf("switch\n");
}




void decifermessage(char *mesg){

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


		socklen_t addrlen = sizeof(remaddr);

		serverAddr = argv[1];
		serverPort = atoi(argv[2]);

		printf("%s\n", serverAddr);
		printf("%d\n", serverPort);

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

		


		

			printf("waiting for connection\n");
		while(Run == 1){	
			recvlen = recvfrom(serverSocket, message, sizeof(message), 0, (struct sockaddr *)&remaddr, &addrlen);
			if (recvlen > 0){
				printf("recieved message\n");
				decifermessage(message);
				printf("%s\n",message);
				//if (sendto(serverSocket, sendmessage, strlen(message), 0, (struct sockaddr *)&serv_address, sizeof(serv_address)) < (0)){
									
				//}

				Run = 0;
			};
			


		};
	close(serverSocket);
	};
	
}