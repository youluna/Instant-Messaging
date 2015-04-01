#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<pthread.h>

#define MAXLINE 4096
#define SERV_PORT 3000
int sockfd;
struct SENDTOCLI
{
	char ctrl1;
	char ctrl2;
	char from[10];
	char dst[10];
	char data[2048];

};
struct CLIENT_SEND
{
	char ctrl1;
	char ctrl2;
	char local_name[10];//your name
	char local_key[10];
	char dst_name[10];//whom to send to
	char data[2048];
};
struct SENDTOCLI recvl;
struct CLIENT_SEND sendl;
char sendline[MAXLINE];
char recvline[MAXLINE];
void *mainThread(void *j);
void *recvThread(void *j);
void login();
void unlogin();
void regist();
void recvmsg();
void sendmsg();


/*-----------------------mainThread()  recv everything but words------------------------*/
void *mainThread(void *j){

		recvmsg();//system dignals (except words send to you)

}

/*-----------------------recvThread()  only recv the words someone send to you------------------------*/
void *recvThread(void *j){
	recv(sockfd,recvline,MAXLINE,0);
	recvl.ctrl1=recvline[0];
	recvl.ctrl2=recvline[1];
	//string to stuct
	for (int i = 0; i < 10; ++i)
	{
		recvl.from[i]=recvline[i+2];
	}
	for (int i = 0; i < 10; ++i)
	{
		recvl.dst[i]=recvline[i+12];
	}
	for (int i = 0;recvline[i+22]!='\0'; ++i)
	{
		recvl.data[i]=recvline[i+22];
	}

	if (recvl.ctrl1=='4'&&recvl.ctrl2=='3')//some one send you words
	{
		printf("%s\n",recvl.data );
		
	}
}

void unlogin(){
	sendl.ctrl1=='3';
	sendl.ctrl2=='1';
	sendline[0]=sendl.ctrl1;
	sendline[1]=sendl.ctrl2;
	send(sockfd,sendline,MAXLINE,0);
}

void login(){
	char name[10]={'0'};
	char key[10]={'0'};
	sendl.ctrl1='1';
	sendl.ctrl2='1';
	printf("%s\n","Please input your name and password(login):" );
	scanf("%s",name);
	scanf("%s",key);
	strcpy(sendl.local_name,name);
	strcpy(sendl.local_key,key);

	sendline[0]=sendl.ctrl1;
	sendline[1]=sendl.ctrl2;
	for(int i = 0; i < 10; i++)
	{
		sendline[2+i]=sendl.local_name[i];
	}
	for (int i = 0; i < 10; ++i)
	{
		sendline[12+i]=sendl.local_key[i];
	}
	send(sockfd,sendline,MAXLINE,0);

}
void regist(){
	char name[10]={'0'};
	char key[10]={'0'};
	sendl.ctrl1='1';
	sendl.ctrl2='2';
	printf("%s\n","Please input your name and password(regist):" );
	scanf("%s",name);
	scanf("%s",key);
	strcpy(sendl.local_name,name);
	strcpy(sendl.local_key,key);

	sendline[0]=sendl.ctrl1;
	sendline[1]=sendl.ctrl2;
	for(int i = 0; i < 10; i++)
	{
		sendline[2+i]=sendl.local_name[i];
	}
	for (int i = 0; i < 10; ++i)
	{
		sendline[12+i]=sendl.local_key[i];
	}
	send(sockfd,sendline,MAXLINE,0);
}
void sendmsg(){

	sendl.ctrl1='2';
	char choose[10];
	char to_name[10];
	char data[2048];
	printf("%s\n","PLease input the numbers you want to send:" );
	printf("%s\n","1:all" );
	printf("%s\n","others:certain one" );
	scanf("%s",choose);
	if (strcmp(choose,"1"))
	{
		sendl.ctrl2='2';
	}
	else {
		sendl.ctrl1='1';
		printf("%s\n","Please input his name" );
		scanf("%s",to_name);
		strcpy(sendl.dst_name,to_name);
	}
	
	printf("%s\n","Please input your words:" );
	scanf("%s",data);
	strcpy(sendl.data,data);

	sendline[0]=sendl.ctrl1;
	sendline[1]=sendl.ctrl2;
	for(int i = 0; i < 10; i++)
	{
		sendline[2+i]=sendl.local_name[i];
	}
	for (int i = 0; i < 10; ++i)
	{
		sendline[22+i]=sendl.dst_name[i];
	}
	for (int i = 0; sendl.data[i]!='\0'; ++i)
	{
		sendline[32+i]=sendl.data[i];
	}

	send(sockfd,sendline,MAXLINE,0);

}
void recvmsg(){
	recv(sockfd,recvline,MAXLINE,0);
	recvl.ctrl1=recvline[0];
	recvl.ctrl2=recvline[1];
	//string to stuct
	for (int i = 0; i < 10; ++i)
	{
		recvl.from[i]=recvline[i+2];
	}
	for (int i = 0; i < 10; ++i)
	{
		recvl.dst[i]=recvline[i+12];
	}
	for (int i = 0;recvline[i+22]!='\0'; ++i)
	{
		recvl.data[i]=recvline[i+22];
	}


	char choose[10];
	if ((recvl.ctrl1=='0'&&recvl.ctrl2=='0')||(recvl.ctrl1=='3'&&recvl.ctrl2=='2')||(recvl.ctrl1=='3'&&recvl.ctrl2=='4'))
	{
		
		printf("%s\n","Please input numbers:" );
		printf("%s\n","1:login" );
		printf("%s\n","2:regist" );
		printf("%s\n","3:quit" );
		scanf("%s",choose);
		if (strcmp(choose,"1"))
		{
			login();
		}else if (strcmp(choose,"2"))
		{
			regist();
		}else if (strcmp(choose,"3"))
		{
			unlogin();
			exit(0);
		}
		else printf("%s\n","error!" );
	}
	else {
		

	if (recvl.ctrl1=='3')
	{
		if (recvl.ctrl2=='1')
		{
			printf("%s\n","Login succeed!" );
			sendl.ctrl1=='2';
		}
		else if (recvl.ctrl2=='2')
		{
			printf("%s\n","Login failed!" );
		}
		else if (recvl.ctrl2=='3')
		{
			printf("%s\n","unlogin succeed!" );
		}
		else if (recvl.ctrl2=='4')
		{
			printf("%s\n", "unlogin failed!");
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

	if (!(recvl.ctrl1=='3'&&recvl.ctrl2=='2')||!(recvl.ctrl1=='3'&&recvl.ctrl2=='4'))
	{
	
	printf("%s\n","Please input numbers:" );
		printf("%s\n","1:send message" );
		printf("%s\n","2:log out" );
		printf("%s\n","3:quit" );
		scanf("%s",choose);
		if (strcmp(choose,"1"))
		{
			sendmsg();
		}
		else if (strcmp(choose,"2"))
		{
			unlogin();
		}
		else if (strcmp(choose,"3"))
		{
			unlogin();
			exit(0);
		}
	}

}
}
int main()
{
	printf("ddd11111111111ddd");
	
	struct sockaddr_in servaddr;

	printf("dddddddddddddddddd");
	sockfd=socket(AF_INET,SOCK_STREAM,0);

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=inet_addr("192.168.113.128");
	servaddr.sin_port=htons(SERV_PORT);

	printf("dddddd");

	connect(sockfd,(struct sockaddr *)&servaddr ,sizeof(servaddr));
//didn't login
	recvl.ctrl1='0';
	recvl.ctrl2='0';
	pthread_t main_thread;
	pthread_t recv_thread;
	printf("dddaaaaaaadd");
	send(sockfd,sendline,MAXLINE,0);
	sendline[0]='8';

	if(pthread_create(&main_thread,NULL,mainThread,NULL)!=0)
		printf("%s\n","thread main create error!" );
	if(pthread_create(&recv_thread,NULL,recvThread,NULL)!=0)
		printf("%s\n","thread recv create error!" );

	pthread_join(main_thread,NULL);
	pthread_join(recv_thread,NULL);
/*
	while(fgets(sendline,MAXLINE,stdin)!=NULL){
		send(sockfd,sendline,strlen(sendline),0);

		if(recv(sockfd,recvline,MAXLINE,0)==0){
			perror("The server terminated prematurely");
			exit(4);
		}
		printf("%s\n","String receied from server:" );
		fputs(recvline,stdout);

		memset(sendline,0,MAXLINE);
		memset(recvline,0,MAXLINE);
	}
	exit(0);
	*/
}
