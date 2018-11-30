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
#define  BUFF_SIZE 1024


int readline(int, char *, int); 
const char *escapechar = "exit\n";	/* 종료문자 */ 

int main(int argc, char *argv[]) { 
int server_fd, client_fd;	/* 소켓번호 */ 
int clilen, num; 
char sendline[MAXLINE], rbuf[MAXLINE]; 
int size; 
pid_t pid; 
struct sockaddr_in client_addr, server_addr; 
if (argc < 2) { 
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
listen(server_fd, 1); 
/* 클라이언트 연결요청 수락 */ 
clilen = sizeof(client_addr); 
if((client_fd = accept(server_fd,(struct sockaddr *)&client_addr,(socklen_t*)&clilen)) != -1) { 
printf("Server: client connected\n"); 
} else { 
printf("Server: failed in accepting.\n"); 
return -1; 
} 
/* 부모 프로세스는 키보드 입력을 클라이언트에게 전송 */ 
if((pid = fork()) > 0) { 
while(readline(0, sendline, MAXLINE) != 0) { 
size = strlen(sendline); 
if(write(client_fd, sendline, size) != size) { 
printf("Server: fail in writing\n"); 
} 
/* 종료 문자열 입력 확인 */ 
if(strncmp(sendline, escapechar, 4) == 0) { 
kill(pid, SIGQUIT); 
break; 
} 
} 

/* 자식프로세스는 소켓으로부터 들어오는 메시지를 화면에 출력 */ 
} else if (pid == 0) { 
while(1) { 
if((size = read(client_fd, rbuf, MAXLINE)) > 0) { 
rbuf[size] = '\0'; 
/* 종료문자열 수신 처리 */ 
if (strncmp(rbuf, escapechar, 4) == 0) break; 
printf("%s", rbuf); 
} 
} 
} 
close(server_fd); 
close(client_fd); 
}


//https://blog.naver.com/sdug12051205/221059889586
//https://blog.naver.com/bitnang/70172674084
//http://downman.tistory.com/151 << diffcult
//http://donghwada.tistory.com/entry/%EB%84%A4%ED%8A%B8%EC%9B%8C%ED%81%AC-Server-Client%EA%B0%84-%EB%8B%A4%EC%A4%91-%EC%A0%91%EC%86%8D%EC%B1%84%ED%8C%85?category=288387 << code
//https://blog.naver.com/blue7red/100047547985