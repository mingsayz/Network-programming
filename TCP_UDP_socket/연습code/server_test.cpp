#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 

#define PORT 4000
#define  BUFF_SIZE 1024

int main (void){

  int server_socket,client_socket,client_addr_size,readnum;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  char fromClient[BUFF_SIZE+1];
  char toClient[BUFF_SIZE+1];

  server_socket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  if (server_socket == -1 ){
    printf("error: fail create server socket!");
    exit(1);
  }
  memset(&server_addr,0,sizeof(server_addr)); // struct initialize
  server_addr.sin_family=AF_INET;
  server_addr.sin_addr.s_addr=htonl(INADDR_ANY); //long type (IP : 4byte)
  server_addr.sin_port = htons(PORT); // from strinig to integer -> network byte code (short type port : 2byte)

  if(bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0)
  {
    printf("error: fail bind()!!\n");
    return -1;
  }
  if(listen(server_socket,5) == -1 )
  {
    printf("error: fail listen()\n");
    exit(1);
  }
  else {
    printf("listening..\n");
  }

  client_addr_size = sizeof(client_addr);

  client_socket = accept(server_socket,(struct sockaddr*)&client_addr,(socklen_t*)&client_addr_size);
  if (client_socket == -1) {
    printf("fail accept()!!\n");
    exit(1);
  }
  printf("Client connected");
 
  printf(" #################### 1 : 1 채팅이 시작 되었습니다. ####################\n");
  printf(" #########        종료를 원할 경우 /quit 를 입력 하세요       ##########\n");
 /* 접속한 클라이언트 정보(1.2.3.4:41532) 출력 */
  printf ("Connected from %s:%d...\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
 
 /* 수신 / 송신 반복문 시작 */
 while(1)
 {
 /* 수신 받을 버퍼 생성 */
  char buffer[BUFF_SIZE];
  memset(buffer,0,sizeof(buffer));
 
 /* 데이터 수신 받음 */
  ssize_t numbytesrcvd = recv(client_socket,buffer,BUFF_SIZE,0);
  if(numbytesrcvd == -1)
    printf(" recv () error");
 
 /* 수신받은 데이터 출력*/
  printf("from %s : %s  ",inet_ntoa(client_addr.sin_addr),buffer);
  buffer[strlen(buffer)-1] = '\0';
  if(strcmp(buffer,"/quit") == 0)
    break;
  memset(buffer,0,sizeof(buffer));
 
 

 /* 서버측 입력받을 버퍼 생성   */
  char message[BUFF_SIZE];
  memset(message,0,sizeof(message));
 /* 보낼 메시지 입력 받음 */
  printf("Server : ");
  fgets(message,BUFF_SIZE,stdin);
 
 /* 서버측 문자열 입력 버퍼 센드 */
  if(send(client_socket,message,strlen(message),0)== -1)
  printf( " send () error ");
  /* 우선 송신후, 입력받은 버퍼가 quit 일 경우 종료  */
  if(strcmp(message,"/quit\n") == 0)
    break;
}
 fputc('\n',stdout); 
 printf(" #############   1 : 1 채팅이 종료 되었습니다  #############\n");
  /* close() : 소켓 종료 */
 close (client_socket);
 close (server_socket);

 return 0;
}

//http://disclosure.tistory.com/7

