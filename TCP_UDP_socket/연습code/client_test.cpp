#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 4000
#define BUFF_SIZE 1024


int main(void) {

	int client_socket;
	struct sockaddr_in server_addr;

	client_socket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(client_socket == -1){
		printf("create client socket failed");
		return -1;
	}

	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(PORT);
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);

	if(connect(client_socket,(struct sockaddr*)&server_addr,sizeof(server_addr))== -1)
	{
		printf("connect failed\n");
		return -1;
	}
	printf(" #################### 1 : 1 채팅이 시작 되었습니다. ####################\n");
	printf(" #########        종료를 원할 경우 /quit 를 입력 하세요       ##########\n");

	while(1)
	{
		char message[BUFF_SIZE];
		memset(message,0,strlen(message));

		printf("Client: ");
		fgets(message,BUFF_SIZE,stdin);

		size_t echoStringLen = strlen(message);
		ssize_t numBytes = send(client_socket,message,echoStringLen,0);
		if(numBytes == -1)
		{
			printf("send failed");
			return -1;
		}

		if(strcmp(message,"/quit\n") == 0)
		{
			break;
		}

		char buffer[BUFF_SIZE];
		memset(buffer,0,sizeof(buffer));

		numBytes=recv(client_socket,buffer,BUFF_SIZE,0);
		if(numBytes == -1)
		{
			printf("receive buffer failed!\n");
			return -1;

		}

		if(strcmp(buffer,"/quit\n")==0)
		{
			break;
		}
		printf("from %s : %s",inet_ntoa(server_addr.sin_addr),buffer);
		memset(buffer,0,sizeof(buffer));
	}

	fputc('\n',stdout);
	printf("#############     1 : 1 채팅이 종료 되었습니다.    #############\n");
	close(client_socket);
	return 0;


}