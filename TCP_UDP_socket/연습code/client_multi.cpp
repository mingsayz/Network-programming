#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define BUFF_SIZE 1024

int main(int argc, char **argv) { 
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