//chat_client_select.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define CHATDATA 1024

char	escape[ ] = "exit";
char	nickname[20];

main(int argc, char *argv[ ])
{
	int	c_socket;
	struct sockaddr_in c_addr;
	int	len;
	char	chatData[CHATDATA];
	char	buf[CHATDATA];
	int	nfds;
	fd_set read_fds;
	int	n;

	if(argc < 3) {
		printf("usage : %s ip_address port_number\n", argv[0]);
		exit(-1);
	}

	c_socket = socket(PF_INET, SOCK_STREAM, 0);

	memset( &c_addr, 0, sizeof(c_addr));
	c_addr.sin_addr.s_addr = inet_addr(argv[1]);
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(atoi(argv[2]));

	printf("Input Nickname : ");
	scanf("%s", nickname);

	if(connect(c_socket, (struct sockaddr *)&c_addr, sizeof(c_addr)) == -1) {
		printf("Can not connect\n");
		return -1;
	}

	nfds = c_socket + 1;

	while(1) {

		FD_ZERO(&read_fds);
		FD_SET(0, &read_fds);		// standard input
		FD_SET(c_socket, &read_fds);	// from chat server
	 
		if(select(nfds, &read_fds, (fd_set *)0, (fd_set *)0, (struct timeval *)0) < 0) {
			printf("select error\n");
			exit(1);
		}

		if(FD_ISSET(c_socket, &read_fds)) {
			memset(chatData, 0, sizeof(chatData));
			if((n = read(c_socket, chatData, sizeof(chatData))) > 0 ) {
				write(1, chatData, n);
			}
		}
	 
		if(FD_ISSET(0, &read_fds)) {
			memset(buf, 0, sizeof(buf));
			if((n = read(0, buf, sizeof(buf))) > 0 ) {
				sprintf(chatData, "[%s] %s", nickname, buf);
				write(c_socket, chatData, strlen(chatData));

				if(!strncmp(buf, escape, strlen(escape))) {
					break;
				}
			}

		}

	}

	close(c_socket);
}
