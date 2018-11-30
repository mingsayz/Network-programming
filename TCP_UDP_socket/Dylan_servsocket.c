#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	struct sockaddr_in servaddr,cliaddr;
	int sockfd; //socket descripter
	int acc_sock;
	socklen_t addrlen = sizeof(cliaddr);

	if(argc < 2)
	{
		printf("Usage : %s port \n",argv[0]);
		return -1;
	}

	if((sockfd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))< 0)
	{
		perror("Socket failed !");
		return -1;
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[1]));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
	{
		perror("bind failed!");
		return -1;
	}

	listen(sockfd,3);

	while(1)
	{
		puts("Listening.....");
		acc_sock = accept(sockfd, (struct sockaddr*)&cliaddr, (socklen_t*)sizeof(cliaddr));
		if(acc_sock < 0)
		{
			perror("accept failed!");
			return -1;
		}
		puts("Connected client");

	

	}

	close(sockfd);

	return 0;
}

//http://byneonkid.tistory.com/116