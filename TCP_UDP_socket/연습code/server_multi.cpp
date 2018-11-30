#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <signal.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXLINE 1024
#define BUFF_SIZE 1024
#define quit "quit"

pid_t pid; 

void do_keyboard(int client_fd){
	int n;
	char sbuf[BUFF_SIZE];
		while((n= read(0,sbuf,BUFF_SIZE)) > 0){
		if(write(client_fd,sbuf,n) != n){
			printf("Talk server fail in sending\n");
		}
			if(strncmp(sbuf,quit,4) ==0){
			kill(pid,SIGQUIT);
			break;
		}
	}
		}

void do_socket(int client_fd){
	int n;
	char rbuf[BUFF_SIZE];

	while(1){
		if((n=read(client_fd,rbuf,BUFF_SIZE)) > 0){
			rbuf[n]='\0';
			printf("%s",rbuf);
				if(strncmp(rbuf,quit,4) == 0){
				kill(getppid(),SIGQUIT);
				break;
			}
		}
	}
}


int main(int argc, char *argv[]) { 
	int server_fd, client_fd;	/* 소켓번호 */ 
	int clilen, num; 
	char sendline[MAXLINE], rbuf[MAXLINE];
	int size;  
	struct sockaddr_in client_addr, server_addr; 
	if (argc < 2) 
	{ 
		printf("Usage: %s TCP_PORT\n", argv[0]); 
		return -1; 
	} 
	/* 소켓 생성 */ 
	if((server_fd=socket(PF_INET, SOCK_STREAM, 0)) < 0) { 
		printf("Server: Can't open stream socket\n"); 
		return -1; 
	} 
	/* 서버의 소켓주소 구조체 server_addr을 '0'으로 초기화 */ 
	memset(&server_addr,0,sizeof(server_addr)); 
	/* server_addr을 세팅 */ 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	server_addr.sin_port = htons(atoi(argv[1])); 
	/* bind() 호출 */ 
	if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) { 
		printf("Server: Can't bind local address \n"); 
		return -1; 
	} 
	printf("Server started.\nWaiting for client.."); 
	listen(server_fd, 5); 
	/* 클라이언트 연결요청 수락 */ 
	clilen = sizeof(client_addr); 
	if((client_fd = accept(server_fd,(struct sockaddr *)&client_addr,(socklen_t*)&clilen)) != -1) { 
		printf("Server: client connected\n"); 
	} 
	else { 
		printf("Server: failed in accepting.\n"); 
		return -1; 
	}
	if((pid= fork()) < 0) {
		printf("server can not folk()\n");
		return -1;
	}
	else if(pid > 0){
		do_keyboard(client_fd);
	} 
	else if(pid == 0){
		do_socket(client_fd);
	}
	close(server_fd);
	close(client_fd);

}


