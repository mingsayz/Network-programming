#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>


#define BUFSIZE 1024

void * send_message(void * arg);
void * recv_message(void * arg);

int main(int argc,char ** argv)
{
 int serv_sock;
 struct sockaddr_in serv_addr;
 int str_len;
 int status;
 pthread_t send_thread;
 pthread_t recv_thread;
 if(argc != 3)
 {
  printf("%s <ip><port>\n",argv[0]);
  exit(0);
  }

 serv_sock = socket(PF_INET,SOCK_STREAM,0);

 if(serv_sock == -1)
 {
  puts("Error socket()");
  exit(0);
 }

 memset(&serv_addr,0,sizeof(serv_addr));

 serv_addr.sin_family = AF_INET;
 serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
 serv_addr.sin_port = htons(atoi(argv[2]));

 if(connect(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1)

 {
  puts("Error connection()");
  exit(0);
 }

 pthread_create(&recv_thread,NULL,recv_message,(void*)&serv_sock); //수신스레드 생성
 pthread_create(&send_thread,NULL,send_message,(void*)&serv_sock); // 송신스레드 생성
 pthread_join(send_thread,(void**)&status); //송신스레드 종료
 pthread_join(recv_thread,(void**)&status); //수신스레드 종료

 close(serv_sock);

 return 0;
}

/*

송신스레드 처리기

*/

void * send_message(void * arg)

{

 int sock = *((int*)arg);
 char message[BUFSIZE];
 int str_len;

 while(1)
 {
  str_len = read(0,message,BUFSIZE);
  message[str_len] = 0;
  write(sock,message,str_len);
  if(!strcmp(message,"q\n"))
  {
   puts("송신 스레드 종료");
   break;
  }
 }

 pthread_exit(NULL);

}

/*
수신스레드 처리기
*/
void * recv_message(void * arg)
{
 int sock = *((int*)arg);
 char message[BUFSIZE];
 int str_len;
 while((str_len = read(sock,message,BUFSIZE)) != 0)
 {
  message[str_len] = 0;
  if(!strcmp(message,"q\n"))
  {
   puts("수신 스레드 종료");
   break;
  }
  printf("받은 메세지 : %s",message);
 }
 pthread_exit(NULL);

}

