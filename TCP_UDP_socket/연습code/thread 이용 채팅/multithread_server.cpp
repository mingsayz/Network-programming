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
#define USER 20

int user[USER];
int user_index = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //mutex 초기화

void * clnt_connection(void * arg);

int main(int argc,char ** argv)
{
 int serv_sock;
 int clnt_sock;
 struct sockaddr_in serv_addr;
 struct sockaddr_in clnt_addr;
 int clnt_len;
 
 pthread_t clnt_thread;

 if(argc != 2)
 {
  printf("%s <port>\n",argv[0]);
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
 serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 serv_addr.sin_port = htons(atoi(argv[1]));

 if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1)
 {
  puts("Error bind()");
  exit(0);
 }

 if(listen(serv_sock,5) == -1)
 {
  puts("Error listen()");
  exit(0);
 }
 clnt_len = sizeof(clnt_addr);
 while(1)

 {
  clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,(socklen_t*)&clnt_len);
  if(clnt_sock == -1)
  {
   puts("Error accept()");
   exit(0);
  }
  pthread_create(&clnt_thread,NULL,clnt_connection,(void*)&clnt_sock);
 }
 close(serv_sock);
 return 0;
}

void * clnt_connection(void * arg)

{
 int sock = *((int*)arg);
 int i;
 const char * new_message;
 char message[BUFSIZE];
 int str_len;
 int clnt_index; 
 const char * close_message;
 
 new_message = (char*)malloc(sizeof(char)*BUFSIZE);
 close_message = (char*)malloc(sizeof(char)*BUFSIZE);
 
 new_message = "new client connection!!\n";
 close_message= "client disconnection!!\n";

 pthread_mutex_lock(&mutex); // 스레드 동기
 printf("%d 클라이언트 접속 \n",sock);

 clnt_index = user_index; //현재 접속한 소켓번호의 인덱스를 저장

 user[user_index++] = sock; // 접속한 소켓번호를 배열에 추가

 

/*

클라이언트 접속 메세지 전송

*/

 for(i=0;i<user_index;i++)
 {
  write(user[i],new_message,strlen(new_message));
 }

 pthread_mutex_unlock(&mutex); // 스레드 동기 해제

 while(1)
 {
  if((str_len = read(sock,message,BUFSIZE)) != 0)
  {
   message[str_len] = 0;
   if(!strcmp(message,"q\n"))
   {
    pthread_mutex_lock(&mutex);
    write(sock,message,str_len);



    /*

클라이언트 접속 해제시 클라이언트 배열 조정

 */

    for(i=clnt_index;i<user_index;i++)
    {
     user[i] = user[i+1];
    }
    user_index--;

    printf("%d\n",user_index);
    for(i=0;i<user_index;i++)

    {
     write(user[i],close_message,strlen(close_message));
    }

    close(sock);

    pthread_mutex_unlock(&mutex);

    break;

   }else

   {
    pthread_mutex_lock(&mutex);
    for(i=0;i<user_index;i++)
    {
     write(user[i],message,str_len);
    }
    pthread_mutex_unlock(&mutex);

   }

  }

 }

 pthread_exit(NULL);

}


