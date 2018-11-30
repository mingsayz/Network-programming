#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	int sock , nbyte;
	struct sockaddr_in servaddr;

	if (argc < 3)
	{
		printf("Usage : %s ip_address port\n",argv[0]);
		return -1;
	}

	if((sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0 )
	{
		perror("socket failed!");
		return -1;
	}

	servaddr.sin_family = AF_INET;

	inet_pton(AF_INET, argv[1],&servaddr.sin_addr);
	servaddr.sin_port = htons(atoi(argv[2]));

	if(connect(sock,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
	{
		perror("Connection failed!");
		return -1;

	}

	puts("Connection Success!");
	close(sock);

	return 0;
}