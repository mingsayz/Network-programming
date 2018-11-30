#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>

#define CHATDATA 1024
/* protocol type */
#define register_name 1
#define create_room 2
#define room_list 3
#define exit_room 4

#define room_DATA 10

#pragma pack(1)
struct SEND_INFO{
	int data_size; //4byte
	int proto; //4byte 
	char user_name[50]; //4byte
};
#pragma pack()

char	nickname[20];
char quest_room[]= "다른 방에 들어가시겠습니까? (n)을 누르면 종료==>";
char	escape[ ] = "exit";
char break_room[] = "n";
int choice_proto;
void *do_send_chat(void *);
void *do_receive_chat(void *);
int room_choice_ch;
char room_choice[CHATDATA];
char new_room_num[room_DATA];

pthread_t thread_1, thread_2;

void * do_send_chat(void *arg)
{
	char	chatData[CHATDATA];
	char	buf[CHATDATA];
	int	n;
	int c_choice;
	int	c_socket = (long int) arg;		// client socket
	//char room_choice[CHATDATA];

	// if(choice_proto == 1){
	// 		//printf("where do you want to come in ?");

	// 		//read(0,room_choice,sizeof(room_choice));
			
	// 		//room_choice_ch=htonl(atoi(room_choice));

	// 	write(c_socket,room_choice,sizeof(room_choice));

	// 	printf("-----");
	// 	printf("%s",room_choice);
	// }



	while(1) {
		
		memset(buf, 0, sizeof(buf));

		if((n = read(0, buf, sizeof(buf))) > 0 ) // 키보드로 부터 입력(0) 받은 값을 변수 buf 에 넣는다
		 {
		 	sprintf(chatData, "[%s] %s", nickname, buf); //nickname과 받은 buf를 묶는다
		 	write(c_socket, chatData, strlen(chatData)); //c_socket 으로 입력받은 chatData를 보낸다

		 	if(!strncmp(buf, escape, strlen(escape))) //strncmp() 1이면
		 	{
		 		write(1,quest_room,strlen(quest_room));
		 		read(0,new_room_num,sizeof(new_room_num));
		 		write(c_socket,new_room_num,sizeof(new_room_num));
		 		if(!strncmp(new_room_num,break_room,strlen(break_room))){
			 		pthread_kill(thread_2, SIGINT); //ctrl + c signal
			 		break; //thread exit
			 	}
		 	}
		 }
	}
}	

void * do_receive_chat(void *arg)
{
	char	chatData[CHATDATA];
	int	n;
	long int	c_socket = (long int) arg;		// client socket

	// read(c_socket,chatData,sizeof(chatData));
	// printf("======>");

	while(1) {
		memset(chatData, 0, sizeof(chatData));
		if((n = read(c_socket, chatData, sizeof(chatData))) > 0 ) {
			write(1, chatData, n); //c_socket으로 부터 데이터를 받아와서 1(=표준 출력 -> monitor)
		}
	}
}





main(int argc, char *argv[ ])
{
	char *pch;
	char *p_name = nickname;
	struct SEND_INFO info;
	long int	c_socket;
	struct sockaddr_in c_addr;
	int	len;
	char	chatData[CHATDATA];
	char	buf[CHATDATA];
	int	n;
	char room_choice[room_DATA];
	int room_choice_ch;
	char room_name[30];
	char error_room[]="There is no room !";
	char make_room[]="\n방을 생성합니다 ! 방 이름을 입력하세요 :";
	//int compare;

	if(argc < 3) {
		printf("usgae : %s ip_address port_number\n", argv[0]);
		exit(-1);
	}

	c_socket = socket(PF_INET, SOCK_STREAM, 0);

	memset(&c_addr, 0, sizeof(c_addr));
	c_addr.sin_addr.s_addr = inet_addr(argv[1]);
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(atoi(argv[2]));

	if(connect(c_socket, (struct sockaddr *) &c_addr, sizeof(c_addr)) == -1) {
		printf("Can not connect\n");
		return -1;
	}
	//if((n = read(c_socket,chatData,sizeof(chatData)))>0){
	
	//	write(1,chatData,n);
	
	// else
	// {
	// 	printf("fail to read question!");
	// 	exit(1);
	// }
	while(1){
		printf("\n(1)방에 들어가기 (2) 방 만들기 (3) 방 리스트 출력  \n======>");
		scanf("%d",&choice_proto);
		
		printf("Input Nickname : ");
		scanf("%s", nickname); 

		memset(room_choice,0,sizeof(room_choice));

		// if(choice_proto == 1){
		// 	printf("where do you want to come in ?");
		// 	scanf("%s",room_choice);	
		// }
		

		//printf("%d",choice_proto);
		info.proto=choice_proto;
		info.data_size = sizeof(int)+strlen(nickname)+1;
		strcpy(info.user_name,p_name);
		
		if(send(c_socket,(struct SEND_INFO*)&info,sizeof(int)*2+strlen(p_name)+1,0) > 0 ){
			
			printf("\nsend struct success\n");

			if(choice_proto == 1){
				while(1){
					if (read(c_socket,chatData,sizeof(chatData))> 0){
						//write(1,chatData,strlen(chatData));
						//compare = );
						if(!strncmp(chatData,error_room,14)){
							write(1,error_room,strlen(error_room));
							choice_proto = 2;
							write(1,make_room,strlen(make_room));
							scanf("%s",room_name);
							write(c_socket,room_name,strlen(room_name));
							break;
						}
						
						pch = strtok(chatData,";");

						while(pch != NULL){

							printf("%s\n",pch);
							pch = strtok(NULL,";");

						}
						printf("where do you want to come in ?");
						
						scanf("%s",room_choice);



						write(c_socket,room_choice,sizeof(room_choice));
					
						printf("-----");
					
						printf("%s",room_choice);

						break;
						
					}
					//break;
				}
				break;
			}
			else if(choice_proto == 2){
				
				write(1,make_room,strlen(make_room));
				//printf("\n방을 생성합니다! 방 이름을 입력하세요:");
				scanf("%s",room_name);

				write(c_socket,room_name,strlen(room_name));

				break;

			}
			
			else if(choice_proto == 3){

				while(1){
					if(read(c_socket,chatData,sizeof(chatData))>0){
						if(!strncmp(chatData,error_room,14)){
							write(1,error_room,strlen(error_room));
							choice_proto = 2;
							write(1,make_room,strlen(make_room));
							scanf("%s",room_name);
							write(c_socket,room_name,strlen(room_name));
							break;
						}
							
						pch = strtok(chatData,";");

						while(pch != NULL){
							printf("%s\n",pch);
							pch=strtok(NULL,";");
						}
						break;
							//break;
					}
					//continue;
				}break;
			} else {

				printf("잘못 입력하셨습니다! 다시 시도하세요!");
				close(c_socket);
				exit(1);

			}
		}

		else {
			printf("\nfail to send struct\n");
			exit(1);
		}
	}

	pthread_create(&thread_1, NULL, do_send_chat, (void *) c_socket); //송신용 thread 생성
	pthread_create(&thread_2, NULL, do_receive_chat, (void *) c_socket); //수신용 thread 생성

	pthread_join(thread_1, NULL); //thread_1이 종료될 때 까지 main 스레드 대기
	pthread_join(thread_2, NULL); //thread_2이 종료될 때 까지 main 스레드 대기 

	close(c_socket);
}



