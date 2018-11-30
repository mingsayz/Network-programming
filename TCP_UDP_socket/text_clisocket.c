#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#define CLIENT_IP "127.0.0.1"
#define CLIENT_PORT 5000

int main(void)

{
  int client_socket;
  struct sockaddr_in server_address;
  unsigned char Buff[250];
  int check; 


  client_socket = socket(PF_INET,SOCK_STREAM,0);

  if (client_socket == -1)
    {
      printf("Client Socket ERROR");
      exit(0);
    }
  bzero((char *)&server_address, sizeof(server_address));

  server_address.sin_family = AF_INET;
  server_address.sin_port = htonl(CLIENT_PORT); //2byte
  server_address.sin_addr.s_addr = inet_addr(CLIENT_IP); //4byte



  if(connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1)
    {
      printf("Connect ERROR\n");
      exit(0);
    }

  printf("\nclient socket = [%d]\n\n",client_socket);

  while (1)
    {
  check = read(client_socket,Buff,sizeof(Buff));
  if (client_socket == -1)
    {
      printf("Disconnection Check\n");
      close(client_socket);
      break;
    }
  Buff[sizeof(Buff)] = '\0';
  printf(">> %s\n",Buff);
    }
  close(client_socket);

  return 0;

}
