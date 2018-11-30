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

#define create_room 1
#define connect_room 2
#define exit_room 3
#define move_room 4
#define list_room 5

#define CHATDATA 1024

#pragma pack(1)
struct SEND_INFO{
	int Proto; //4byte
	int Size_data; //4byte 
	char Name[50]; //50byte
};
#pragma pack()

struct RECV_INFO{
	int res_proto;
	bool res_bool;
};

struct arg_INFO{
	int c;
	int proto;
};
void *do_send_chat(void *);
void *do_receive_chat(void *);
void *parse_proto(void*);

pthread_t thread_1, thread_2;

char nickname[20];

char send_suc_struct[]="send struct success!";
char send_fail_struct[]="fail to send struct!";
char recv_suc_struct[]="receive struct success!";
char recv_fail_struct[]="fail to receive struct!";
char serv_error[] = "server error!!!";
char escape[] = "exit";

main(int argc, char *argv[ ]){

	struct SEND_INFO info;
	struct RECV_INFO recv_info;
	struct arg_INFO *arg_info;
	char *p_name;
	long int c_socket;
	struct sockaddr_in c_addr;
	int choice_proto;


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

	while(1){
		printf("Input Nickname : ");
		scanf("%s", nickname); 

		printf("\n(1)방 생성 (2) 방 접속 (3) 방 나가기 (4) 방 옮기기 (5) 방 리스트");
		scanf("%d",&choice_proto);

		info.Proto=choice_proto;
		info.Size_data = sizeof(int)+strlen(nickname)+1;
		if(choice_proto == 4){
			printf("where do you want to move in?");
			scanf("%s",p_name);
			strcpy(info.Name,p_name);
		}
		

		arg_info->proto = choice_proto;
		arg_info->c = c_socket;


		if(send(c_socket,(struct SEND_INFO*)&info,sizeof(int)*2+strlen(p_name)+1,0) > 0 ){
			write(1,send_suc_struct,strlen(send_suc_struct));
			parse_proto(arg_info);	
		}else{
			write(1,send_fail_struct,strlen(send_fail_struct));
		}

		if(recv(c_socket,(struct RECV_INFO*)&recv_info,sizeof(int)+sizeof(bool),0) > 0){
			if(recv_info.res_bool == 1){
				write(1,recv_suc_struct,strlen(recv_suc_struct));
			}
			else if(recv_info.res_bool == 0){
				write(1,serv_error,strlen(serv_error));
			}

		}else{
			write(1,recv_fail_struct,strlen(recv_fail_struct));
		}
	}
}


void *parse_proto(arg_INFO *arg_info){

	arg_INFO *parse_info = (arg_INFO *)arg_info;
	int choice_proto = parse_info -> proto;
	long int c_socket = parse_info -> c;
	char *pch;
	char room_choice[5];
	char chatData[CHATDATA];

	while(1){
		if(choice_proto == 1){
			pthread_create(&thread_1, NULL, do_send_chat, (void *) c_socket); //송신용 thread 생성
			pthread_create(&thread_2, NULL, do_receive_chat, (void *) c_socket); //수신용 thread 생성
			break;
		}
		else if(choice_proto == 2){
			printf("where do you want to come in ?");
			scanf("%s",room_choice);
			write(c_socket,room_choice,sizeof(room_choice));
			pthread_create(&thread_1, NULL, do_send_chat, (void *) c_socket); //송신용 thread 생성
			pthread_create(&thread_2, NULL, do_receive_chat, (void *) c_socket); //수신용 thread 생성
			break;
		}
		else if(choice_proto == 3){
			pthread_create(&thread_1, NULL, do_send_chat, (void *) c_socket); //송신용 thread 생성
			pthread_create(&thread_2, NULL, do_receive_chat, (void *) c_socket); //수신용 thread 생성
			break;
		}
		else if(choice_proto == 4){
			pthread_create(&thread_1, NULL, do_send_chat, (void *) c_socket); //송신용 thread 생성
			pthread_create(&thread_2, NULL, do_receive_chat, (void *) c_socket); //수신용 thread 생성
			break;
		}
		else if(choice_proto == 5){
			pch = strtok(chatData,";");
			
			while(pch != NULL){
				printf("%s\n",pch);
				pch = strtok(NULL,";");
			}
			// pthread_create(&thread_1, NULL, do_send_chat, (void *) c_socket); //송신용 thread 생성
			// pthread_create(&thread_2, NULL, do_receive_chat, (void *) c_socket); //수신용 thread 생성
			break;
		}else{
			write(1,serv_error,strlen(serv_error));
			break;
		}
	}
}


void * do_send_chat(void *arg)
{
	char	chatData[CHATDATA];
	char	buf[CHATDATA];
	int	n;
	int c_choice;
	int	c_socket = (long int) arg;		// client socket


	while(1) {
		
		memset(buf, 0, sizeof(buf));

		if((n = read(0, buf, sizeof(buf))) > 0 ) // 키보드로 부터 입력(0) 받은 값을 변수 buf 에 넣는다
		 {
		 	sprintf(chatData, "[%s] %s", nickname, buf); //nickname과 받은 buf를 묶는다
		 	write(c_socket, chatData, strlen(chatData)); //c_socket 으로 입력받은 chatData를 보낸다

		 	if(!strncmp(buf, escape, strlen(escape))) //strncmp() 1이면
		 	{	
		 		pthread_kill(thread_2 ,SIGINT);
		 		break;//write(1,quest_room,strlen(quest_room));
		 		
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


