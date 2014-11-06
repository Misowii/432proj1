
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

#include "duckchat.h"

	char *serverName;
	int serverPort;
	char *username;
	unsigned char recvmessage[SAY_MAX];
	int recvlen; 
	int temp;
	char cmd[1024];
	int clientSocket;
	int running = 1;
	char command = '/';
	char *comtype;
	const char *exitcom = "/exit";
	const char *listcom = "/list";
	const char *whocom = "/who";
	const char *joincom = "/join";
	const char *leavecom = "/leave";
	const char *switchcom = "/switch";
	#define MAX_CHANNELS 32
	char Common[] = "common";
	char cur_channels[MAX_CHANNELS][CHANNEL_MAX];
	int channel_count = 0;
	char *current_channel;

	struct hostent *hp;
	struct sockaddr_in serv_address;
	socklen_t addrlen;

void list(){ //channels
	//printf("list\n");
	struct request_list rlist;
    rlist.req_type = REQ_LIST;
    sendto(clientSocket, &rlist, sizeof(rlist), 0, (struct sockaddr *)&serv_address, sizeof(serv_address));


	//send message to server to recieve list of active channels
}

void who(char *wanted_channel){ //users
	//printf("who\n");
	struct request_who rwho;
    rwho.req_type = REQ_WHO;
    strcpy(rwho.req_channel, wanted_channel); 
	//send message to server to recieve list of users in channel
    sendto(clientSocket, &rwho, sizeof(rwho), 0, (struct sockaddr *)&serv_address, sizeof(serv_address));
   }


void say(char *message){
	//printf("%s\n", cmd);
	//send message with channel/username/message?
	struct request_say rsay;
	rsay.req_type = REQ_SAY;
    strcpy(rsay.req_channel, current_channel); 
    strcpy(rsay.req_text, message);
    sendto(clientSocket, &rsay, sizeof(rsay), 0, (struct sockaddr *)&serv_address, sizeof(serv_address));

}

void join(char *channel){
	//printf("join\n");
	int flag = 0;
	int i;
	for (i = 0; i < channel_count; i++){
		if (strcmp(cur_channels[i], channel) == 0){
			current_channel = channel;
			printf("Channel already joined, switch to suggested channel now");
			flag = 1;
			//printf("%s\n", current_channel);
		}
	}
	if (!flag){
		strcpy(cur_channels[channel_count], channel);
		channel_count += 1;
		current_channel = channel;
		//printf("%s\n", current_channel);
		//send message to server to join channel
		struct request_join rjoin;
		rjoin.req_type = REQ_JOIN;
		strcpy(rjoin.req_channel, channel);
		sendto(clientSocket, &rjoin, sizeof(rjoin), 0, (struct sockaddr *)&serv_address, sizeof(serv_address));

	}
}

void leave(char *wanted_channel){
	int flag = 0;
	int i;
	for (i = 0; i < channel_count; i++){
		if (strcmp(cur_channels[i], wanted_channel) == 0){
			current_channel = cur_channels[0];
			printf("Left channel, switching to common channel.");
			flag = 1;
			//printf("%s\n", current_channel);

			//send message to server to leave
			struct request_leave rleave;
        	rleave.req_type = REQ_LEAVE;
        	strcpy(rleave.req_channel, wanted_channel);
      		sendto(clientSocket, &rleave, sizeof(rleave), 0, (struct sockaddr *)&serv_address, sizeof(serv_address));


		}
	}
	if (flag == 0){
		printf("Channel suggested has not been joined");
	}
}

void switchto(char *wanted_channel){
	int flag = 0;
	int i;
	//printf("%s\n", current_channel);
	for (i = 0; i < channel_count; i++){
		if (strcmp(cur_channels[i], wanted_channel) == 0){
			current_channel = wanted_channel;
			flag = 1;
			//printf("%s\n", current_channel);
		}
	}
	if (flag == 0){
		printf("Suggested channel needs to be joined first.\n");
	}
}

void login(char *usertitle){
	struct request_login logreq;
	logreq.req_type = REQ_LOGIN;
	strcpy(logreq.req_username, usertitle);
	sendto(clientSocket, &logreq, sizeof(logreq), 0, (struct sockaddr *)&serv_address, sizeof(serv_address));
	//send login request with logreq
}
void  logout(){
	struct request_logout rlogout;
	rlogout.req_type = REQ_LOGOUT;
	sendto(clientSocket, &rlogout, sizeof(rlogout), 0, (struct sockaddr *)&serv_address, sizeof(serv_address));
}

void exitclient(){
	logout();
	running = 0;
	//send logout
}

void recieve(){
	struct text rectext;
	recvlen = recvfrom(clientSocket, &rectext, sizeof(rectext), 0, (struct sockaddr *)&serv_address, &addrlen );
	
	if (rectext.txt_type == TXT_SAY){

	}

	else if (rectext.txt_type == TXT_WHO){

	}

	else if (rectext.txt_type == TXT_LIST){
		struct text_list reclist;
		reclist = (struct text_list)rectext;


		int i;
		//for (i = 0; i < reclist.txt_nchannels; i++){
		//printf("%s\n", reclist.txt_channels[i].ch_channel);
		//}

	}

	else if (rectext.txt_type == TXT_ERROR){

	}		
}

int main(int argc, char *argv[])
{

	

	if (argc != 4)
	{
		printf("Client requires 3 arguments. Servername, Port, and Username.\n");
		return(-1);
	}
	
	serverName = argv[1];	
	serverPort = atoi(argv[2]);
	username = argv[3];
	
	clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

	hp = gethostbyname(serverName);

	memset((char *)&serv_address, 0, sizeof(serv_address));
	serv_address.sin_family = AF_INET;
	memcpy((void *)&serv_address.sin_addr, hp->h_addr_list[0], hp->h_length);
	serv_address.sin_port = htons(serverPort);

	//printf("%s\n", serverName);
	addrlen = sizeof(serv_address);

	login(username);
	join(Common);

	while (running == 1){
		//recvlen = recvfrom(clientSocket, recvmessage, strlen(recvmessage), 0, (struct sockaddr *)&serv_address, &addrlen );
		
		//select()
		temp = read(1, cmd, 1024);
		cmd[temp-1] = '\0';

		if (cmd[0] == command){
			comtype = strtok(cmd, " ");
			//printf("%s\n", comtype);
			if (strcmp(cmd, exitcom) == 0){	
				exitclient();
			}
			else if(strcmp(cmd, listcom)== 0){
				list();
			}
			else if(strcmp(comtype, whocom)== 0){
				comtype = strtok(NULL, " ");
				who(comtype);
			}
			else if(strcmp(comtype, joincom) == 0){
				comtype = strtok(NULL, " ");
				join(comtype);
			}
			else if(strcmp(comtype, leavecom) == 0){
				comtype = strtok(NULL, " ");
				leave(comtype);
			}
			else if(strcmp(comtype, switchcom) == 0){
				comtype = strtok(NULL, " ");
				switchto(comtype);
			}
			else if (strcmp(cmd, "/currentchannel") == 0){
				printf("%s\n", current_channel);
			}
			else{
				printf("Unrecognized Command!\n");
			}
		}
		else{
			//printf("%s\n", cmd);
			say(cmd);
			}

		
	};
	close(clientSocket);
	return 0;

}