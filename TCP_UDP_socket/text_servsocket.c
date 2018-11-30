#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000
#define MAX_SEND_SIZE 100
int main(void)
{
  int server_socket;
  unsigned int size;

  int toClient;
  char message[15];
  struct sockaddr_in server_address;
  struct sockaddr_in Client;

  server_socket = socket(PF_INET,SOCK_STREAM,0);

  if (server_socket == -1)
    {
      printf("Cannot Create Socket.\n");
      exit(0);
    }
  memset(&server_address, 0x00, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(SERVER_PORT);
  server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
  /*
  if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
      printf("bind() ERROR\n");
      exit(0);
    }
  */
  if (listen(server_socket,3) == -1)
    {
      printf("listen() ERROR");
      exit(0);
    }

  size = sizeof(server_address);
  toClient = accept(server_socket,(struct sockaddr *)&Client, &size);

  if (toClient < 0)
    {
      printf("accept ERROR");
    }

  printf("\nserver_socket = [%d]\n\n",server_socket);
  printf("\nclient_socket = [%d]\n\n",toClient);
  while(1)
    {
  printf(">> ");
  fgets(message,MAX_SEND_SIZE,stdin);
  if (message[0] == 'q')
    {
      close(server_socket);
      close(toClient);
      break;
    }
  message[sizeof(message)] = '\0';
  write(toClient,message,sizeof(message));
    }
  close(server_socket);
  close(toClient);

  return 0;

}

