#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 1024 //buf 크기
#define TOTALFORK 5 //클라이언트 수

struct client_info {
	char clientAddr[32];
	int clientPort;
};

void createClient(char *port, char *serverIP);
int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("Use %s ip_addr port\n", argv[0]);
		exit(0);
	}

	pid_t pids[TOTALFORK];
	int runProcess = 0;
	
	while(runProcess < TOTALFORK) {
		sleep(1);
		pids[runProcess] = fork();

		if(pids[runProcess] < 0) {
			return -1;
		}
		
		if(pids[runProcess] == 0) {
			createClient(argv[2], argv[1]);
			exit(0);
		} else { //부모 프로세스
			printf("parent %ld, child %ld\n", (long)getpid(), (long)pids[runProcess]);
		}
		runProcess++;
	}
	return 0;
}

void createClient(char *port, char *serverIP) {
	struct sockaddr_in servaddr;
	struct client_info *cliinfo;
	int strlen = sizeof(servaddr);
	cliinfo = malloc(&TOTALFORK);
	int sockfd, nbyte, cNum;//cNum 연결 번호
	char buf[MAXLINE];

	if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}

	memset(&servaddr, 0, strlen);
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, serverIP, &servaddr.sin_addr);
	servaddr.sin_port = htons(atoi(port));

	if(connect(sockfd, (struct sockaddr *)&servaddr, strlen) < 0) {
		perror("connect fail");
		exit(0);
	}
	
	//접속 성공 메시지
	strcpy(buf, "Connect Success");
	write(sockfd, buf, sizeof(buf));

	//모든 client 접속 정보 받아 옴
	recv(sockfd, cliinfo, sizeof(struct client_info) * 5, 0);

	strcpy(buf, "GetOK");
	write(sockfd, buf, sizeof(buf));
	
	printf("pid : %ld, 받은 정보 출력\n", (long)getpid());
	int i;
	for(i = 0; i < TOTALFORK; i++) {
		printf("pid:%ld, ip:%s port:%d\n", (long)getpid(), cliinfo[i].clientAddr, cliinfo[i].clientPort);
	}
	
	close(sockfd);
}