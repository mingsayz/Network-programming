#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#define MAX_CLIENT 10
#define CHATDATA 1024
#define INVALID_SOCK -1


void *do_chat(void *);
int	pushClient(int);
int	popClient(int);
pthread_t thread; //요청을 처리할 스레드 선언
pthread_mutex_t mutex; //임계영역을 지정해줄 mutex 선언

int	list_c[MAX_CLIENT]; // 소켓을 담을 리스트 생성
int count = 0;

char	escape[] = "exit";
char	greeting[] = "Welcome to chatting room\n";
char	CODE200[] = "Sorry No More Connection\n";

main(int argc, char *argv[ ])
{
	long int	c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int	len;
	int	i, j, n;
	int	res;

	if(argc < 2) {
		printf("usage: %s port_number\n", argv[0]);
		exit(-1);
	}

	if(pthread_mutex_init(&mutex, NULL) != 0)  //mutex 초기화
	{
		printf("Can not create mutex\n");
		return -1;
	}

	s_socket = socket(PF_INET, SOCK_STREAM, 0);

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(atoi(argv[1]));

	if(bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
		printf("Can not Bind\n");
		return -1;
	}

	if(listen(s_socket, MAX_CLIENT) == -1) {
		printf("listen Fail\n");
		return -1;
	}

	printf("Listening..... \n");

	for(i = 0; i < MAX_CLIENT; i++) 
		list_c[i] = INVALID_SOCK; //소켓의 파일 디스크럽트 값은 항상 양수이다. 구분을 위해 -1 로 초기화한다. 


	while(1) {
		len = sizeof(c_addr);
		c_socket = accept(s_socket, (struct sockaddr *) &c_addr, (socklen_t*)&len);
		printf("New client connected !, IP : %s\n",inet_ntoa(c_addr.sin_addr));
		printf("접속자 수 :%d , 남은 공간:%d\n",count+1,9-count);

		res = pushClient(c_socket);
		if(res < 0) {
			write(c_socket, CODE200, strlen(CODE200)); //연결된 c_socket에 define 된 오류 전달
			close(c_socket);
		} else {
			write(c_socket, greeting, strlen(greeting));  //c_socket으로 인사말 전달
			pthread_create(&thread, NULL, do_chat, (void *) c_socket); //소켓에서 값을 읽고 전달하는 스레드 생성 attr=NULL (default) 인자로는 c_socket 전달
	}
}

void *
do_chat(void *arg)
{
	int c_socket = (long int) arg;
	char	chatData[CHATDATA];
	int	i, n;

	while(1) {
		memset(chatData, 0, sizeof(chatData));
		if((n = read(c_socket, chatData, sizeof(chatData))) > 0) {
			for(i = 0; i < MAX_CLIENT; i++) {
				if(list_c[i] != INVALID_SOCK) {
					write(list_c[i], chatData, n);
				}
			}

				if(strstr(chatData, escape) != NULL) 
				{
					popClient(c_socket);
					break;
				}
		}
	}
}

int pushClient(int c_socket) {
	int	i;

	for(i = 0; i < MAX_CLIENT; i++) {
		pthread_mutex_lock(&mutex);
		if(list_c[i] == INVALID_SOCK) {
			list_c[i] = c_socket;
			count++;
			pthread_mutex_unlock(&mutex);
			return i;
		}
		pthread_mutex_unlock(&mutex);
	}
	//printf("접속자 수 :%d , 남은 공간:%d\n",count+1,9-count);

	
	if(i == MAX_CLIENT)
		return -1;
	}		
		
int popClient(int s)
{			
	int	i;	 
				
	close(s); 
					
		for(i = 0; i < MAX_CLIENT; i++) {
			pthread_mutex_lock(&mutex); 
			if(s == list_c[i]) //전역 변수이므로 임계영역으로써 처리해줘야함 list_c
			{
					list_c[i] = INVALID_SOCK;
					count--;
					pthread_mutex_unlock(&mutex);
					break;
			}
			pthread_mutex_unlock(&mutex);
		}
	//printf("접속자 수 :%d , 남은 공간:%d\n",count+1,9-count);
	
	return 0;
}
