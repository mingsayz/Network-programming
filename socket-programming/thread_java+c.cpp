#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>

#define create_room 1
#define connect_room 2
#define exit_room 3
#define move_room 4
#define list_room 5

#define MAX_CLIENT 100
#define CHATDATA 1024
#define room_DATA 10


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
	int Proto;
	int Size_Data;
	char Name[50];
};

struct RECV_INFO info;

struct INFO_SOCK{
	int Proto;
	//int Size_Data;
	char Name[50];
	int sock_new;
};

#pragma pack(1)
struct SEND_INFO{
	int res_proto;
	bool res_bool;
};
#pragma pack()

pthread_mutex_t mutex;
char tail_key[]=";";
char head_key[]="_";
char null_key[]="\0";
char noti_room[]="번 방";
char success_message[]="success to send room_list\n";
char fail_message[]="fail to send room_list\n";
char	escape[] = "exit";
char wrong_type[] = "\nwrong !!! try again!\n";
char	greeting[] = "\n---------------Welcome to chatting room---------------\n---------------(exit를 누르면 종료합니다)---------------\n";
char	CODE200[] = "!!!!!!!!!!!!!!!Sorry No More Connection!!!!!!!!!!!!!!\n";
char enter_room[]="번방으로 입장합니다!!";
char error_room[]="There is no room !";
char make_room[]="번 방이 만들어졌습니다!";
char quest_room[]= "다른 방에 들어가시겠습니까? (n)을 누르면 종료";
char break_room[] = "n";
char room_name[CHATDATA];
int room_number1;
pthread_t thread,thread1;


//void *parse_proto(void *);
void *do_chat(void *);
void *proto_01(INFO_SOCK *);
void *proto_02(INFO_SOCK *);
void *proto_03(INFO_SOCK *);
void *proto_04(INFO_SOCK *);
void *proto_05(INFO_SOCK *);

char print_roomlist(int c){
	if((write(c,room_name,strlen(room_name)))>0){
		write(1,success_message,strlen(success_message));
	}else{
		write(1,fail_message,strlen(fail_message));
	}
	return 0;
}
// char res_True[] = "True";
// char res_False[] = "False";


void *parse_proto(void *info_sock) {

	int proto; 
	int c_socket;
	// int i=0;
	// int count=0;
	struct node* horse;

	INFO_SOCK *parse_info =(INFO_SOCK *)info_sock;
	proto = parse_info -> Proto;
	c_socket = parse_info -> sock_new;
	while(1){
		if(proto == 1){
			proto_01(parse_info);
			break;
		}
		else if(proto == 2){
			proto_02(parse_info);
			break;
		}
		else if(proto == 3){
			proto_03(parse_info);
			break;
		}
		else if(proto == 4){
			proto_04(parse_info);
			break;
		}
		else if(proto == 5){
			proto_05(parse_info);
			break;
		}else{
			write(1,wrong_type,strlen(wrong_type));
			break;
		}
	}
}



void *proto_01(INFO_SOCK *parse_info){
	
	int i , j = 0;
	int count = 0;
	long int c_socket ;
	char chatData[CHATDATA];
	char make_room1[20];
	char *str;
	//char *ch_str;
	struct node* horse;
	int hashIndex;
	//struct SEND_INFO res_info;
	//write(1,error_room,strlen(error_room));
	
	//INFO_SOCK *proto_info =(INFO_SOCK *)parse_info;
	c_socket = parse_info -> sock_new;
	while(1){
		for (i=0 ; i < 20 ; i++){
			horse = hashTable[i].head;
			if(count < 1){
				if(horse == NULL){
					if((j = read(c_socket,chatData,sizeof(chatData)))>0){
						str = (char*)malloc(sizeof(chatData)+ 7);
						sprintf(str,"%d%s%s%s%s%s",i,noti_room,head_key,chatData,tail_key,null_key);
						strcat(room_name,str);
						write(1,room_name,strlen(room_name));
						//break;
					}

					insert(i,c_socket);
					write(c_socket,greeting,strlen(greeting));
				//key = i;
				//hashIndex = hashFunction(i);
					display();

					sprintf(make_room1,"%d %s \n",i,make_room);
					//print_roomlist(c_socket);
					count ++;
				// res_info.res_proto = 1;
				// res_info.res_bool = 1;
				// send(c_socket,(struct SEND_INFO*)&res_info,sizeof(int)+sizeof(bool),0);
					pthread_create(&thread, NULL, do_chat, (void *) c_socket);
					break;
				}
			// res_info.res_proto = 1;
			// res_info.res_bool = 0;
			// send(c_socket,(struct SEND_INFO*)&res_info,sizeof(int)+sizeof(bool),0);
			//break;
			}
		}
	//ch_str = (char *)realloc(str, str +  )

	}
}

void *proto_02(INFO_SOCK *parse_info){

	long int c_socket;
	int n,i = 0;
	int count = 0;
	struct node* horse;
	//struct SEND_INFO res_info;
	char room_number[room_DATA];
	char chatData[CHATDATA];



	INFO_SOCK *proto_info =(INFO_SOCK *)parse_info;
	c_socket = parse_info -> sock_new;

	while(1){
		for (i = 0; i<SIZE ; i++){
			horse =  hashTable[i].head;
			if(horse == NULL){
				count ++;
			}			
		}
		if(count == 10){
			write(c_socket,error_room,strlen(error_room));
			//prase_info->Proto = 2;
			proto_01(proto_info);
		}else{
		// write(c_socket,room_name,strlen(room_name));
			print_roomlist(c_socket);
		}
		if((n= read(c_socket,room_number,sizeof(room_number)))>0){
					
			room_number1 = atoi(room_number);
				//pthread_mutex_unlock(&mutex);
			
			insert(room_number1,c_socket);
			write(c_socket,greeting,strlen(greeting));
			display();
			pthread_create(&thread, NULL, do_chat, (void *) c_socket);
			break;
		}
	}
}


void *proto_03(INFO_SOCK *parse_info){
	INFO_SOCK *proto_info =(INFO_SOCK *)parse_info;
	long int c_socket = proto_info -> sock_new;
	struct node* horse;
	int key = room_number1;
	int hashIndex = hashFunction(key);
	struct node* node = hashTable[hashIndex].head;
	while(1){
		if(node->value = c_socket){
			remove(node->key);
			display();
			break;
		}else{
			node = node->next;
		}
	}
}

void *proto_04(INFO_SOCK *parse_info){
	INFO_SOCK *proto_info=(INFO_SOCK *)parse_info;
	long int c_socket = proto_info -> sock_new;
	struct node* horse;
	int room_number1;
	int n , key;
	char room_number[10];
	write(c_socket,room_name,strlen(room_name));
	while(1){
		if((n = read(c_socket,room_number,sizeof(room_number)))> 0){
			room_number1 = atoi(room_number);
			break;
		}
	}
	// key = room_number1;
	// hashIndex = hashFunction(key);
	// struct node* node = hashTable[hashIndex].head;
	// while(1){
	// 	if(node->key = room_number1){
	// 		insert(room_number1,c_socket);
	// 		display();
	// 		break;
	// 	}else{
	// 		node = node -> next;
	// 	}
	// }
	
	insert(room_number1,c_socket);
	display();

}

void *proto_05(INFO_SOCK *parse_info){
	INFO_SOCK *proto_info = (INFO_SOCK *)parse_info;
	long int c_socket = proto_info -> sock_new;
	write(c_socket,room_name,strlen(room_name));
	display();
}	


main(int argc,char *argv[]){
	hashTable = (struct bucket*)malloc(SIZE*sizeof(struct bucket));
	long int	c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len,rsize;


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

	while(1){
		len = sizeof(c_addr);
		c_socket = accept(s_socket, (struct sockaddr *) &c_addr, (socklen_t*)&len);
		printf("New client connected !, IP : %s\n",inet_ntoa(c_addr.sin_addr));
		//insert(1,c_socket);
		struct RECV_INFO info;
		
		//struct SEND_INFO *res_info;

		if((rsize=recv(c_socket,(struct RECV_INFO*)&info,sizeof(info)+1,0)) > 0){

			//RECV_INFO *info = (RECV_INFO *)malloc(sizeof(RECV_INFO) + strlen(info->Name));

			printf("\nsuccess\n");
			printf("protocol %d \n name size: %d  \nName : %s\n",info.Proto,info.Size_Data,info.Name);
			printf("-------------------------------------------\n");

			struct INFO_SOCK *info_sock;

			info_sock = (INFO_SOCK *)malloc(sizeof(INFO_SOCK));
			info_sock->Proto=info.Proto;
			//info_sock->Size_Data = info.Size_Data;
			strcpy(info_sock->Name,info.Name);
			info_sock->sock_new = c_socket;


			// res_info->res_proto = info->Proto;
			// res_info->res_bool = 1;
			// send(c_socket,(struct SEND_INFO*)&res_info,sizeof(int)+sizeof(bool),0);
			
			pthread_create(&thread1, NULL, parse_proto, (void *)info_sock);
		}else{
			printf("failed to get struct from client!");
			// res_info->res_proto = info->Proto;
			// res_info->res_bool = 0;
			// send(c_socket,(struct SEND_INFO*)&res_info,sizeof(int)+sizeof(bool),0);
		}
	}
}


void *do_chat(void *arg){
	int c_socket = (long int) arg;
	int n;
	char chatData[CHATDATA];
	int key =room_number1;
	int hashIndex = hashFunction(key);
	struct node* node = hashTable[hashIndex].head;


	while(1){
		if((n = read(c_socket, chatData, sizeof(chatData))) > 0) {
			node = hashTable[hashIndex].head;
			if(strstr(chatData, escape) != NULL){
				if(node->value = c_socket){
					break;
				}
			}
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
}
