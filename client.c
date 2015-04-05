#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<unistd.h>

#define MAXLINE 1000
#define SERV_PORT 3000


int sockfd;
int local_log;//0 offline    1 online
char this_name[10];//my name(if login)
pthread_t main_thread;
pthread_t recv_thread;

struct SENDTOCLI
{
	char ctrl1;
	char ctrl2;
	char from[10];
	char dst[10];
	char data[500];

};
struct CLIENT_SEND
{
	char ctrl1;
	char ctrl2;
	char local_name[10];//your name
	char local_key[10];
	char dst_name[10];//whom to send to
	char data[500];
};
struct SENDTOCLI recvl;
struct CLIENT_SEND sendl;
char sendline[MAXLINE]={'0'};
char recvline[MAXLINE]={'0'};

void *mainThread(void *j);
void *recvThread(void *j);
void login();
void unlogin(char c);
void regist();
void dealMessage();
void sendMessage(char choose);


/*-----------------------mainThread()  recv everything but words------------------------*/
void *mainThread(void *j){

	//sendline[0]='8';
	//sendline[1]='8';
	//send(sockfd,sendline,MAXLINE,0);
	//recv(sockfd,recvline,MAXLINE,0);

	dealMessage();//system dignals (except words send to you)
	
	return NULL;
}

/*-----------------------recvThread()  only recv the words someone send to you------------------------*/
void *recvThread(void *j){
	while(1){
		memset(recvline,0,522);
		memset(recvl.data,0,500);
	if(recv(sockfd,recvline,MAXLINE,0)==0){
		perror("The server terminated prematurely");
		exit(4);
	}
/*----------------getting data------------------------*/
	recvl.ctrl1=recvline[0];
	recvl.ctrl2=recvline[1];
	//string to stuct
	int i;
	for (i = 0; i < 10; ++i)
	{
		recvl.from[i]=recvline[i+2];
	}
	for (i = 0; i < 10; ++i)
	{
		recvl.dst[i]=recvline[i+12];
	}
	for (i = 0;recvline[i+22]!='\0'; ++i)
	{
		recvl.data[i]=recvline[i+22];
	}
/*----------------getting data------------------------*/


	if (recvl.ctrl1=='3')
		{
			if (recvl.ctrl2=='1')//login succeed
			{
				printf("%s\n","Login succeed!" );
				local_log=1;//login state
				strcpy(this_name,recvl.from);
				sendl.ctrl1='2';
			}
			else if (recvl.ctrl2=='2')//login failed
			{
				printf("%s\n","Login failed!" );
				local_log=0;//unlog state
			}
			else if (recvl.ctrl2=='3')//login failed
			{
				printf("%s\n","Regist succeed! And please login~" );
				local_log=0;//unlog state
			}
			else if (recvl.ctrl2=='4')//login failed
			{
				printf("%s\n","Regist failed!Maybe no space" );
				local_log=0;//unlog state
			}
			
		}
	else if (recvl.ctrl1=='4')
	{
		if (recvl.ctrl2=='1')
		{
			printf("%s\n","Message send succeed!" );
		}
		else if (recvl.ctrl2=='2')
		{
			printf("%s\n","Message send failed" );
		}
		else if (recvl.ctrl2=='3')//some one send you words
		{
			printf("%s:%s\n",recvl.from,recvl.data );	
		}
		else if (recvl.ctrl2=='4')//some one send you words
		{
			printf("online list(divide by ##):\n" );
			printf("%s\n",recvl.data );
			
		}else if (recvl.ctrl2=='5')//some one send you words
		{
			printf("%s\n","Online list receiving failed" );	
		}
	}

	else if (recvl.ctrl1=='5')
	{
		if (recvl.ctrl2=='1')
		{
			printf("%s\n","unlogin succeed!" );
			exit(0);
		}
		else if (recvl.ctrl2=='2')
		{
			printf("%s\n","unlogin failed" );
		}
	}


	}
}

void unlogin(char c){
	if (c=='6')//unlog only
	{
		sendl.ctrl1='3';
		sendl.ctrl2='1';	
	}
	else if (c=='7')
	{
		sendl.ctrl1='3';
		sendl.ctrl2='2';
	}
	else if (c=='8')
	{
		sendl.ctrl1='3';
		sendl.ctrl2='3';	
	}
	sendline[0]=sendl.ctrl1;
	sendline[1]=sendl.ctrl2;
	int i;
	for(i = 0; i < 10; i++)
	{
		sendline[2+i]=this_name[i];
	}
	local_log=0;
	send(sockfd,sendline,MAXLINE,0);
}

void login(){
	char name[10]={'0'};
	char key[10]={'0'};
	sendl.ctrl1='1';
	sendl.ctrl2='1';
	printf("%s\n","Please input your name and password(login) with in 10 letters:" );
	scanf("%s",name);
	scanf("%s",key);

	strcpy(sendl.local_name,name);
	strcpy(sendl.local_key,key);

	sendline[0]=sendl.ctrl1;
	sendline[1]=sendl.ctrl2;
	int i;
	for(i = 0; i < 10; i++)
	{
		sendline[2+i]=sendl.local_name[i];
	}
	for (i = 0; i < 10; ++i)
	{
		sendline[12+i]=sendl.local_key[i];
	}
		printf("%s\n",sendline);
	send(sockfd,sendline,MAXLINE,0);
	for (i = 0; i < 10; ++i)
	{
		sendline[12+i]='\0';
	}


}
void regist(){
	char name[20]={'0'};
	char key[20]={'0'};
	sendl.ctrl1='1';
	sendl.ctrl2='2';
	printf("%s\n","Please input name and password(regist)with in 10 letters:" );
	scanf("%s",name);
	scanf("%s",key);
	while (strlen(name)>10||strlen(key)>10)
	{
		printf("All with in 10 letters!\n");
		scanf("%s",name);
		scanf("%s",key);
	}
	
	strcpy(sendl.local_name,name);
	strcpy(sendl.local_key,key);

	sendline[0]=sendl.ctrl1;
	sendline[1]=sendl.ctrl2;
	int i;
	for(i = 0; i < 10; i++)
	{
		sendline[2+i]=sendl.local_name[i];
	}
	for(i = 0; i < 10; ++i)
	{
		sendline[12+i]=sendl.local_key[i];
	}
	send(sockfd,sendline,MAXLINE,0);

}
void sendMessage(char choose){

	memset(sendline,0,522);
	strcpy(sendl.local_name,this_name);
	sendl.ctrl1='2';
	char to_name[10];
	char data[500];
	//strcpy(sendl.local_name,);
	if (choose!='5')
	{
		if (choose=='4')
		{
			sendl.ctrl2='2';				//group send
		}
		else if(choose=='3')				 //send to certain one
		{
			sendl.ctrl2='1';				//to certain one
			printf("%s\n","Please input his name" );
			scanf("%s",to_name);
			strcpy(sendl.dst_name,to_name);
		}
	
		printf("%s\n","Please input your words:" );
		scanf("%s",data);
		strcpy(sendl.data,data);
	}else {									//print online list
		sendl.ctrl1='1';
		sendl.ctrl2='3';
	}
	sendline[0]=sendl.ctrl1;
	sendline[1]=sendl.ctrl2;
	int i;
	for(i = 0; i < 10; i++)
	{
		sendline[2+i]=sendl.local_name[i];
	}
	for (i = 0; i < 10; ++i)
	{
		sendline[22+i]=sendl.dst_name[i];
	}
	for (i = 0; sendl.data[i]!='\0'; ++i)
	{
		sendline[32+i]=sendl.data[i];
	}

	send(sockfd,sendline,MAXLINE,0);
	memset(sendline,0,532);
	memset(sendl.dst_name,0,10);
	memset(sendl.data,0,500);
	

}

void dealMessage(){
	
	while(1)
	{
	printf("---------------------------------\n" );
	printf("%s\n","Please input numbers:" );
	printf("%s\n","1:login" );
	printf("%s\n","2:regist" );
	printf("%s\n","3:send to one" );
	printf("%s\n","4:send to all" );
	printf("%s\n","5:print online list" );
	printf("%s\n","6:quit" );
	printf("%s\n","7:quit and close" );
	printf("%s\n","8:delete me from database" );
	printf("---------------------------------\n" );
	char choose[10];
	scanf("%s",choose);
	printf("this is choose:%s\n",choose );

	if (!strcmp(choose,"1"))
	{
		if (local_log==1)
		{
			printf("%s\n","You have logined already." );
		}else
			login();
	}
	else if (!strcmp(choose,"2"))
	{
		regist();
	}
	else if (!strcmp(choose,"3"))
	{
		if (local_log==0)
		{
			printf("%s\n","Login first, please!" );
		}else
			sendMessage(choose[0]);//certain one send
	}
	else if (!strcmp(choose,"4"))
	{
		if (local_log==0)
		{
			printf("%s\n","Login first, please!" );
		}else
			sendMessage(choose[0]);//group send
	}
	else if (!strcmp(choose,"5"))
	{
		sendMessage(choose[0]);
	}
	else if (!strcmp(choose,"6"))
	{
		if (local_log==0)
		{
			printf("%s\n","You didn't login!" );
		}else
			{
				unlogin(choose[0]);

			}
	}
	else if (!strcmp(choose,"7"))
	{
		if (local_log==1)
		{
			unlogin(choose[0]);
		}	
			exit(0);
	}
	else if (!strcmp(choose,"8"))
	{
		if (local_log==0)
		{
			printf("%s\n","You didn't login!I don't know who you are :(" );
		}else
			{
				unlogin(choose[0]);

			}
	}
	else
		printf("%s\n","Input error" );
}
}
int main()
{
	struct sockaddr_in servaddr;

	sockfd=socket(AF_INET,SOCK_STREAM,0);

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	servaddr.sin_port=htons(SERV_PORT);

	local_log=0;

	if(connect(sockfd,(struct sockaddr *)&servaddr ,sizeof(servaddr))==0)
		{
			printf("%s\n","Connect succeed" );
			int tag2=pthread_create(&recv_thread,NULL,recvThread,NULL);
				if(tag2!=0)
					printf("%s\n","thread recv create error!" );
			mainThread(NULL);
		}
	else
		printf("%s\n","Connect failed" );

	return 0;
}
