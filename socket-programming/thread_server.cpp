#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>

//#include <iostream>
#define MAX_CLIENT 50
#define CHATDATA 1024
#define INVALID_SOCK -1
#define room_DATA 10

/* protocol type */
#define register_name 1
#define create_room 2
#define room_list 3
#define exit_room 4


/* hashtable linked list 로 채팅방 구현*/

struct bucket* hashTable = NULL;
int SIZE = 10;


struct node {
	int key;
	int value;
	struct node* next;
};

struct bucket {
	struct node* head;
	int count;
};

struct node* createNode(int key,int value){
	struct node* newNode;
	newNode = (struct node*)malloc(sizeof(struct node));
	newNode->key = key;
	newNode->value = value;

	newNode->next = NULL;
	return newNode;

};


int hashFunction(int key){
	return key%SIZE;
}

void insert(int key, int value){
	int hashIndex = hashFunction(key);

	struct node* newNode = createNode(key,value);

	if(hashTable[hashIndex].count == 0){

		hashTable[hashIndex].head = newNode;
		hashTable[hashIndex].count = 1;
	}
	else{
		newNode->next = hashTable[hashIndex].head;
		hashTable[hashIndex].head = newNode;
		hashTable[hashIndex].count++;
	}
	return;
}

void remove(int key){
	int hashIndex = hashFunction(key);
	
	struct node* node;
	struct node* trace;
	
	node = hashTable[hashIndex].head;

	if(node == NULL){
		printf("\nno key found\n");
		return ;
	}

	while(node!=NULL){
		if(node->key == key) {
			if(node == hashTable[hashIndex].head){
				hashTable[hashIndex].head = node->next;
			}
			else{
				trace->next = node->next;

			}

			hashTable[hashIndex].count--;
			free(node);
			break; 

		}

		trace = node;
		node = node->next;
	}
	printf("\n not key found\n");
	return;
}


void search(int key){

	int hashIndex = hashFunction(key);
	struct node* node = hashTable[hashIndex].head;

	if(node == NULL){
		printf("\nnot key found\n");
		return;
	}

	while(node != NULL){

		if(node->key == key){
			printf("key found key= [%d] value= [%d]",node->key,node->value);
			break;
		}
		node = node->next;
	}

	printf("\nno key found\n ");
	return;
}

void display(){

	struct node* horse;
	int i = 0;
	for (i = 0; i<SIZE ; i++){

		horse =  hashTable[i].head;
		printf("[%d]번 테이블 ,인원: %d ==>",i,hashTable[i].count);

		while(horse != NULL){
			printf("(key:%d, val:%d) ->",horse->key,horse->value);
			horse = horse->next;
		}
		printf("\n");
	}
	printf("\n-----------end of display-----------\n");
	return;
}

struct RECV_INFO{
	int data_size;
	int proto; //4byte
	char user_name[50]; //4byte
};


struct thread_sock{
	int proto_parse;
	int sock_new;
};

struct RECV_INFO info;

int rsize;
void *parse_proto(void *);
void *do_chat(void *);
pthread_t thread,thread1; //요청을 처리할 스레드 선언
pthread_mutex_t mutex; //임계영역을 지정해줄 mutex 선언
int room_number1;
int proto;
char room_name[CHATDATA];
int check_roomname=0;

int count = 0;
char count_user(int s){

	printf("접속자 수 :%d , 남은 공간:%d\n",s,50-s);
	return 0;

}


//char room_number[]="room number? ===> ";
char success_message[]="success to send room_list\n";
char fail_message[]="fail to send room_list\n";
char	escape[] = "exit";
char wrong_type[] = "\nwrong !!! try again!\n";
char	greeting[] = "\n---------------Welcome to chatting room---------------\n---------------(exit를 누르면 종료합니다)---------------\n";
char	CODE200[] = "!!!!!!!!!!!!!!!Sorry No More Connection!!!!!!!!!!!!!!\n";
char c_choice[]="(1)방에 들어가기 (2) 방 만들기 (3) 방 리스트 출력 (4) 메세지 보내기\n";
char enter_room[]="번방으로 입장합니다!!";
char error_room[]="There is no room !";
char make_room[]="번 방이 만들어졌습니다!";
char quest_room[]= "다른 방에 들어가시겠습니까? (n)을 누르면 종료";
char break_room[] = "n";


char print_roomlist(int c){


	if((write(c,room_name,strlen(room_name)))>0){
		write(1,success_message,strlen(success_message));

	}

	else{
		write(1,fail_message,strlen(fail_message));
	}

	return 0;

}

main(int argc, char *argv[ ])
{	
	hashTable = (struct bucket*)malloc(SIZE*sizeof(struct bucket));
	long int	c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int	len;
	int	i, j, n;
	int	res;
	char room_number[room_DATA];
	//int rsize;

	if(argc < 2) {
		printf("usage: %s port_number\n", argv[0]);
		exit(1);
	}

	if(pthread_mutex_init(&mutex, NULL) != 0){  //mutex 초기화
	
		printf("Can not create mutex!\n");
		return -1;
	}

	s_socket = socket(PF_INET, SOCK_STREAM, 0);

	memset(&s_addr, 0, sizeof(s_addr)); //s_addr 구조체 초기화
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY); //현재 사용중인 IP주소 htonl(host -> network 바이트 순서로 변환)
	s_addr.sin_family = AF_INET; //TCP IPv4로 통신
	s_addr.sin_port = htons(atoi(argv[1])); //입력받은 (argv[1]) 포트번호를 구조체 포트로 사용

	if(bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
		printf("Can not Bind!!\n");
		return -1;
	} //서버 소켓과 구조체를 바인드 (묶는 과정) ==> 포트와 IP를 묶어야 클라이언트 측에서 연결 가능

	if(listen(s_socket, MAX_CLIENT) == -1) {
		printf("listen Fail!!\n");
		return -1;
	} //listen 상태로 대기

	printf("Listening..... \n");
	//display();

	while(1) {
		len = sizeof(c_addr);
		c_socket = accept(s_socket, (struct sockaddr *) &c_addr, (socklen_t*)&len); //클라이언트의 connect()요청을 수락
		
		printf("New client connected !, IP : %s\n",inet_ntoa(c_addr.sin_addr));

		struct RECV_INFO info; //미리 정의해둔 RECV_INFO 구조체 선언 (프로토콜 , 데이터사이즈, 유저네임 받아옴)
		if((rsize=recv(c_socket,(struct RECV_INFO*)&info,sizeof(info)+1,0)) > 0){ //클라이언트가 넘기는 구조체를 받는다.

			
			thread_sock *struct_th; //thread_sock 구조체 포인터 선언(프로토콜, 소켓 스레해드로 전달하기 위해)
			
			struct_th =(thread_sock*)malloc(sizeof(thread_sock)); //공간 할당

			struct_th->proto_parse =info.proto; 
			struct_th->sock_new = c_socket; //전달할 소켓,프로토콜 담기

			pthread_create(&thread1, NULL, parse_proto, (void *)struct_th);

			printf("\nsuccess\n");
			printf("data_size : %d \nprotocol: %d  \nuser_name : %s\n",info.data_size,info.proto,info.user_name);
			printf("-------------------------------------------\n");



		}
		
		else {
			printf("fail to get struct from client!");
		}

	}
}

/*프로토콜 , 소켓을 넘겨 받아서 각각을 프로토콜 별로 처리해주는 스레드
처음 만들때 계산 미스로 굳이 안만들어도 될걸 만든듯.. */

void * parse_proto (void *struct_th){  //
	struct node* horse;
	char chatData[CHATDATA];
	long int c_socket;
	int n,i = 0;
	int count = 0;
	char room_number[room_DATA];


	//int proto;
	thread_sock *parse_struct_th = (thread_sock *)struct_th;
	//struct_th = *pt; 

	c_socket = parse_struct_th->sock_new; //받아온 구조체로부터 소켓 정보 뽑아내서 저장

	proto = parse_struct_th->proto_parse; //받아온 구조체로 부터 프로토콜 뽑아내서 저장



	while(1){	
		if(proto == 1){
			for (i = 0; i<SIZE ; i++){
				horse =  hashTable[i].head;
				if(horse == NULL){
					count ++;
				}			
			}

			if(count == 10){

				write(c_socket,error_room,strlen(error_room));
				proto =	 2 ;
				continue;
			}

			else{

				print_roomlist(c_socket);
			}
			pthread_mutex_lock(&mutex);

			if((n= read(c_socket,room_number,sizeof(room_number)))>0){
				
				room_number1 = atoi(room_number);
				pthread_mutex_unlock(&mutex);
		
				insert(room_number1,c_socket);
				write(c_socket,greeting,strlen(greeting));
				display();
				pthread_create(&thread, NULL, do_chat, (void *) c_socket);
				break;
			}
		}
		
		else if(proto == 2){
			pthread_create(&thread,NULL,do_chat,(void*)c_socket); 
			break;

		}
		else if(proto == 3){
			count = 0;

			for (i = 0; i<SIZE ; i++){
				horse =  hashTable[i].head;	
				if(horse == NULL){
					count ++;
				}			
			}
			if(count == 10){
				write(c_socket,error_room,strlen(error_room));
				proto = 2;
				continue;
			 }else {
				proto = 1;
				continue;
			}
			break;
		}

	}
}
		


void *do_chat(void *arg)
{
	int c_socket = (long int) arg;
	
	char enter_room1[CHATDATA];
	char	chatData[CHATDATA];
	char make_room1[CHATDATA];
	char itoa_data[room_DATA];
	char noti_room[]="번 방";
	char tail_key[]=";";
	char head_key[]="_";
	char null_key[]="\0";
	char room_list1[room_DATA];
	



	int	i = 0, n,l,j;
	int key =room_number1;
	int hashIndex = hashFunction(key); //hashindex = key % 10
	int change_roomnum;
	int count = 0;
	int *input;
	int total_count = 0;
	//char *str[20];
	char *str;
	// char room_name[CHATDATA];
	int room_numcount=0;

	struct node* node = hashTable[hashIndex].head; //해시테이블을 돌면서 키값을 찾아주고 buf를 전달해줄 head노드 생성
	struct node* find_node;
	struct node* horse;
	//struct node* prev_find;
	
	sprintf(enter_room1,"%d %s\n",room_number1,enter_room);
	

	while(1){
		memset(chatData, 0, sizeof(chatData));



		if(proto == 1){
			if((n = read(c_socket, chatData, sizeof(chatData))) > 0) {
				node = hashTable[hashIndex].head;
				if(strstr(chatData, escape) != NULL){
					if(node->value = c_socket){
								
						remove(node->key);
						display();

						if((l=read(c_socket,chatData,sizeof(chatData))) > 0){
							if(strstr(chatData,break_room) != NULL){

								close(node->value);
							}
							else {
							change_roomnum = atoi(chatData);
							insert(change_roomnum,c_socket);
							write(c_socket,greeting,strlen(greeting));
							display();
							}
						}
					}

					else {

						node = node -> next;
						continue;
					}

					key = change_roomnum;
					hashIndex = hashFunction(key);
				}

				//node = hashTable[hashIndex].head;

				if(node == NULL){

					write(c_socket,error_room,strlen(error_room));
					exit(1);

				}

				while(node != NULL)  {
					if(node ->key == hashIndex ||node->key == key){
						if(node->key == key){
							write(node->value,chatData,n);
						}
						
					}
					node = node->next;
				}
			}
		}	 

		else if(proto == 2){

			memset(chatData,0,sizeof(chatData));

			for(i=0 ; i<20 ; i++){

				horse = hashTable[i].head;
				if(count < 1) {

					if(horse == NULL){

						if((j = read(c_socket,chatData,sizeof(chatData)))> 0 ){

							str = (char*)malloc(sizeof(chatData)+7);
							//strcat(chatData,tail_key);
							sprintf(str,"%d%s%s%s%s%s",i,noti_room,head_key,chatData,tail_key,null_key);
							strcat(room_name,str);
							write(1,room_name,strlen(room_name));
							//room_numcount++;
							//write(1,str[i],strlen(str[i]));

						}
						insert(i,c_socket);
						write(c_socket, greeting, strlen(greeting));
					
						hashIndex = hashFunction(key);

						display();

						sprintf(make_room1,"%d %s \n",i,make_room);
						write(c_socket,make_room1,strlen(make_room1));
						count++;
					}
				}
			}


			if((n = read(c_socket, chatData, sizeof(chatData))) > 0) {
				node = hashTable[hashIndex].head;
				if(strstr(chatData, escape) != NULL){
					if(node->value = c_socket){
								
						remove(node->key);
						display();

						if((l=read(c_socket,chatData,sizeof(chatData))) > 0){
							if(strstr(chatData,break_room) != NULL){

								close(node->value);
								//pthread_kill(thread,SIGINT);
							}
							else {

								change_roomnum = atoi(chatData);
								insert(change_roomnum,c_socket);
								write(c_socket,greeting,strlen(greeting));
								display();
							}
						}
					}
					else {
						node = node -> next;
						continue;
					}

					key = change_roomnum;
					hashIndex = hashFunction(key);
				}

				//node = hashTable[hashIndex].head;

				if(node == NULL){

					write(c_socket,error_room,strlen(error_room));
					exit(1);
				}

				while(node != NULL)  {
					if(node ->key == hashIndex ||node->key == key){
						if(node->key == key){
							write(node->value,chatData,n);
						}					
					}
					node = node->next;
				}
			}
		}else {
			write(1,fail_message,strlen(fail_message));
			exit(1);
		}
	}	
}
	





			
