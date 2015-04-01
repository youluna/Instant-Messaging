#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

#define MAXLINE 4096
#define SERV_PORT 3000
//maximum of require to be handled that can be stored
#define LISTENQ 10

//local database
struct USER{
	char name[10];
	char key[10];
	int tag;//1:avaliable  0:not free
	
};
struct USER users[10];

struct onlineUSER
{
	char name[10];
	int on_off;//1for on; 0 for off
	int connfd;//for each user online
};
struct onlineUSER onlineUsers[10];

struct SENDTOCLI
{
	char ctrl1;
	char ctrl2;
	char from[10];
	char dst[10];
	char data[2048];

};
struct SENDTOCLI sorline;
struct SENDTOCLI dstline;

struct CLIENT_SEND
{
	char ctrl1;
	char ctrl2;
	char local_name[10];//your name
	char local_key[10];
	char dst_name[10];//whom to send to
	char data[2048];
};
struct CLIENT_SEND recvl;

char recvline[MAXLINE];
char sendlineSor[MAXLINE];
char sendlineDst[MAXLINE];
int findOnlineStatus(char name[]);
int findConnfd(char name[]);
void loginVerify(char name[],char key[]);
void registeVerify(char name[],char key[]);
void unlogin(char name[]);
int findDBFree();
int findFree();
void *dealing(void *j);

int findOnlineStatus(char name[]){
	for (int i = 0; i < 10; ++i)
	{
		if (strcmp(name,onlineUsers[i].name))
		{
			return onlineUsers[i].on_off;
		}
	}
	printf("%s\n","can't find the name" );
	return 0;
}
int findConnfd(char name[]){
	for (int i = 0; i < 10; ++i)
	{
		if (strcmp(name,onlineUsers[i].name))
		{
			return onlineUsers[i].connfd;
		}
	}
	printf("%s\n","can't find the name" );
	return -1;

}
void loginVerify(char name[],char key[]){
	for (int i = 0; i < 10; ++i)
	{
		if (strcmp(name,users[i].name))
		{
			if (strcmp(key,users[i].key))
			{
				sorline.ctrl1='3';
				sorline.ctrl2='1';
				printf("%s\n","login succeed" );
				return ;
			}else 
			printf("%s\n","password incorrect" );
		}
		else ;
	}

	printf("%s\n","name not found" );
	sorline.ctrl1='3';
	sorline.ctrl2='2';
	return ;
}
void registeVerify(char name[],char key[]){
	for (int i = 0; i < 10; ++i)
	{
		if (strcmp(name,users[i].name))
		{
			sorline.ctrl1='3';
			sorline.ctrl2='4';
			printf("%s\n","name already exits" );
			return ;
		}
	}

	sorline.ctrl1='3';
	sorline.ctrl2='3';
	int i;
	i=findDBFree();
	strcpy(users[i].name,name);
	strcpy(users[i].key,key);
	users[i].tag=0;
	printf("%s\n","registe succeed" );
}
void unlogin(char name[]){
	for (int i = 0; i < 10; ++i)
	{
		if (strcmp(name,onlineUsers[i].name))
		{
			onlineUsers[i].on_off=0;//offline
			sorline.ctrl1='5';
			sorline.ctrl1='1';
			return ;
		}
	}
	sorline.ctrl1='5';
	sorline.ctrl1='2';		
	printf("%s\n","unlogin error" );
}
int findDBFree(){
	for (int i = 0; i < 10; ++i)
	{
		if (users[i].tag==1)
			return i;
	}
}
int findFree(){
	for (int i = 0; i < 10; ++i)
	{
		if (onlineUsers[i].on_off==0)
			return i;
	}
}
void *dealing(void *jj){
/*--------------------getting data------------------------*/
	int connfd;
	int j=*(int*)jj;
	connfd=onlineUsers[j].connfd;
	recv(connfd,recvline,MAXLINE,0);/////////////////
	recvl.ctrl1=recvline[0];
	recvl.ctrl2=recvline[1];
	for (int i = 0; i < 10; ++i)
	{
		recvl.local_name[i]=recvline[i+2];
	}
	for (int i = 0; i < 10; ++i)
	{
		recvl.local_key[i]=recvline[i+12];
	}
	for (int i = 0; i < 10; ++i)
	{
		recvl.dst_name[i]=recvline[i+22];
	}
	for (int i = 0; recvline[i+32]!='\0'; ++i)
	{
		recvl.data[i]=recvline[i+32];
	}
	strcmp(onlineUsers[j].name,recvl.local_name);
/*--------------------getting data------------------------*/

	if (recvl.ctrl1=='1')
	{
		if (recvl.ctrl2=='1')
		{
			loginVerify(recvl.local_name,recvl.local_key);
		}
		else if (recvl.ctrl2=='2')
		{
			registeVerify(recvl.local_name,recvl.local_key);
		}
	}
	else if (recvl.ctrl1=='2')
	{
		if (recvl.ctrl2=='1')
		{//dst client
			dstline.ctrl1='4';
			dstline.ctrl1='3';
			strcpy(dstline.from,recvl.local_name);
			strcpy(dstline.dst,recvl.dst_name);
			strcpy(dstline.data,recvl.data);
		//source client
			sorline.ctrl1='4';
			sorline.ctrl2='1';

		}
		else if (recvl.ctrl2=='2')
		{
			dstline.ctrl1='4';
			dstline.ctrl1='3';
			strcpy(dstline.from,recvl.local_name);
			strcpy(dstline.dst,recvl.dst_name);//all
			strcpy(dstline.data,recvl.data);

			sorline.ctrl1='4';
			sorline.ctrl2='1';
		}
		else 
		{
		sorline.ctrl1='4';
		sorline.ctrl2='2';
		}
	}
	else if (recvl.ctrl1=='3')
	{
		if (recvl.ctrl2=='1')
		{
			unlogin(recvl.local_name);
		}
	}
	else 
	{
		sorline.ctrl1='0';
		sorline.ctrl2='0';
	}

	sendlineSor[0]=sorline.ctrl1;
	sendlineSor[1]=sorline.ctrl2;
	for (int i = 0; i < 10; ++i)
	{
		sendlineSor[2+i]=sorline.from[i];
	}
	for (int i = 0; i < 10; ++i)
	{
		sendlineSor[12+i]=sorline.dst[i];
	}
	for (int i = 0; sorline.data[i]!='\0'; ++i)
	{
		sendlineSor[22+i]=sorline.data[i];
	}

	sendlineDst[0]=dstline.ctrl1;
	sendlineDst[1]=dstline.ctrl2;
	for (int i = 0; i < 10; ++i)
	{
		sendlineDst[2+i]=dstline.from[i];
	}
	for (int i = 0; i < 10; ++i)
	{
		sendlineDst[12+i]=dstline.dst[i];
	}
	for (int i = 0; dstline.data[i]!='\0'; ++i)
	{
		sendlineDst[22+i]=dstline.data[i];
	}

	send(findConnfd(recvl.local_name),sendlineSor,MAXLINE,0);
	if (recvl.ctrl1=='2'&&recvl.ctrl2=='2')//group send
	{
		for (int i = 0; i < 10; ++i)
		{
			if (onlineUsers[i].on_off)//if on
			{
				send(onlineUsers[i].connfd,sendlineDst,MAXLINE,0);
			}
		}
	}
	else send(findConnfd(recvl.dst_name),sendlineDst,MAXLINE,0);

}

int main()
{
/*-------------database initial--------------------*/
	strcpy(users[0].name,"about");
	strcpy(users[0].key,"112233");
	strcpy(users[1].name,"blank");
	strcpy(users[1].key,"112233");
	strcpy(users[2].name,"test");
	strcpy(users[2].key,"112233");
	users[0].tag=0;

	for (int i = 0; i < 10; ++i)
	{
		onlineUsers[i].on_off=false;
	}

/*-------------database initial--------------------*/
	int listenfd,connfd,n;
	//pid_t childpid;
	socklen_t clilen;
	char buf[MAXLINE];
	struct sockaddr_in cliaddr,servaddr;

	listenfd=socket(AF_INET,SOCK_STREAM,0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(SERV_PORT);


	bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

	listen(listenfd,LISTENQ);
	pthread_t thread[10];
	printf("%s\n","Server running...waiting for connections." );

	while(1){
		clilen=sizeof(cliaddr);
		int j=findFree();
		onlineUsers[j].connfd=accept (listenfd,(struct sockaddr *) &cliaddr, &clilen);
		pthread_create(&thread[j],NULL,dealing,(void *)&j);

	}

/*
	for(;;){
		clilen=sizeof(cliaddr);
		connfd=accept (listenfd,(struct sockaddr *) &cliaddr, &clilen);
		printf("%s\n","Received request..." );
		/////

		if((childpid=fork())==0){
			printf("%s\n","Child created for dealing with client requests" );
		//close
		close(listenfd);


		//print data from client
		while((n=recv(connfd,buf,MAXLINE,0))>0){
			printf("%s\n","String received from and resent to the client:" );
			puts(buf);
			send(connfd,buf,n,0);
			memset(buf,0,MAXLINE);
		}

		if(n<0)
			printf("%s\n","Read error" );
		exit(0);
	}
	
	close(connfd);
}*/


}
