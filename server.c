#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

#define MAXLINE 1000
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
	char data[500];

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
	char data[500];
};
struct CLIENT_SEND recvl;

char recvline[MAXLINE];
char sendlineSor[MAXLINE];
char sendlineDst[MAXLINE];
pthread_mutex_t count_mutex;
int findOnlineStatus(char name[]);
int findConnfd(char name[]);
void loginVerify(char name[],char key[],int j);
void registeVerify(char name[],char key[]);
void unlogin(char name[]);
int findDBFree();
int findFree();
void *dealing(void *j);
void sendOnlineList();


void sendOnlineList(){
	sorline.ctrl1='4';
	sorline.ctrl2='4';//send online list succeed!
	int i;
	memset(sorline.data,'\0',500);
	char a[2]={'#','#'};
	printf("sendOnlineList:\n" );
	for ( i = 0; i < 10; ++i)
	{
		if (onlineUsers[i].on_off==1)
		{
			printf("in circle\n");
			strcat(sorline.data,onlineUsers[i].name);
			printf("%s\n",sorline.data );
			strcat(sorline.data,a);
			printf("%s\n",sorline.data );
		}
		else ;
	}

}

int findOnlineStatus(char name[]){
	int i;
	for (i = 0; i < 10; ++i)
	{
		if (!strcmp(name,onlineUsers[i].name))
		{
			int k=onlineUsers[i].on_off;
			return k;
		}
	}
	printf("%s\n","can't find the name" );
	return 0;
}
int findConnfd(char name[]){
	int i;
	for (i = 0; i < 10; ++i)
	{
		if (!strcmp(name,onlineUsers[i].name))
		{
			//printf("%d\n", onlineUsers[i].connfd);
			return onlineUsers[i].connfd;
		}
		//printf("%s\n",onlineUsers[i].name );
		//printf("%d\n",onlineUsers[i].on_off );
		//printf("%d\n",onlineUsers[i].connfd );////////////////////////////////////////////////////
	}
	printf("%s\n","can't find the name" );
	return -1;

}
void loginVerify(char name[],char key[],int j){
	int i;
	for (i = 0; i < 10; ++i)
	{
		if (!strcmp(name,users[i].name)&&findOnlineStatus(users[i].name)==0)
		{
			if (!strcmp(key,users[i].key))
			{
					sorline.ctrl1='3';
					sorline.ctrl2='1';
					strcpy(sorline.from,name);
					printf("%s\n","login succeed" );
					strcpy(onlineUsers[j].name,name);
					onlineUsers[j].on_off=1;
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
	int i;
	for (i = 0; i < 10; ++i)
	{
		if (!strcmp(name,users[i].name))
		{
			sorline.ctrl1='3';
			sorline.ctrl2='4';
			printf("%s\n","regist failed.name already exits" );
			return ;
		}
	}

	

	i=findDBFree();
	if (i==-1)
	{
		sorline.ctrl1='3';
		sorline.ctrl2='4';
		printf("%s\n","regist failed.No space " );
	}
	else
	sorline.ctrl1='3';
	sorline.ctrl2='3';
	strcpy(users[i].name,name);
	strcpy(users[i].key,key);
	users[i].tag=0;
	printf("%s\n","regist succeed" );
}
void unlogin(char name[]){
	int i;
	for (i = 0; i < 10; ++i)
	{
		if (!strcmp(name,onlineUsers[i].name))
		{
			onlineUsers[i].on_off=0;//offline
			onlineUsers[i].connfd=0;
			sorline.ctrl1='5';
			sorline.ctrl1='1';
			return ;
		}
	}
	pthread_exit(NULL);		
	printf("%s\n","unlogin error" );
}
int findDBFree(){
	int i;
	for (i = 0; i < 10; ++i)
	{
		if (users[i].tag==1)
			return i;
	}
	return -1;
}
int findFree(){
	int i;
	for (i = 0; i < 10; ++i)
	{
		if (onlineUsers[i].connfd==0)
			return i;
	}
	return -1;
}
void *dealing(void *jj){
/*--------------------getting data------------------------*/

	printf("%s\n","dealing begin" );
	int connfd;
	int j=(int)jj;
	connfd=onlineUsers[j].connfd;
	printf("%d\n",j );
	printf("%d",connfd);
	memset(recvline,0,532);

	int n;

	while((n=recv(connfd,recvline,MAXLINE,0))>0){
		printf("%s\n","--------------onlineUsers data----------------" );
		int i;
		for (i = 0; i < 10; ++i)
		{
		printf("%s\n",onlineUsers[i].name );
		printf("%d\n",onlineUsers[i].on_off );
		printf("%d\n",onlineUsers[i].connfd );
		}
		printf("%s\n","--------------onlineUsers data----------------" );
	printf("recvline:%s\n",recvline );

	recvl.ctrl1=recvline[0];
	recvl.ctrl2=recvline[1];
	
	for ( i = 0; i < 10; ++i)
	{
		recvl.local_name[i]=recvline[i+2];
	}
	for ( i = 0; i < 10; ++i)
	{
		recvl.local_key[i]=recvline[i+12];
	}
	for ( i = 0; i < 10; ++i)
	{
		recvl.dst_name[i]=recvline[i+22];
	}
	for ( i = 0; recvline[i+32]!='\0'; ++i)
	{
		recvl.data[i]=recvline[i+32];
	}

/*--------------------getting data------------------------*/

	if (recvl.ctrl1=='1')
	{
		if (recvl.ctrl2=='1')
		{
			loginVerify(recvl.local_name,recvl.local_key,j);
		}
		else if (recvl.ctrl2=='2')
		{
			registeVerify(recvl.local_name,recvl.local_key);
		}
		else if (recvl.ctrl2=='3')
		{
			sendOnlineList();
		}
	}
	else if (recvl.ctrl1=='2')
	{
		if (recvl.ctrl2=='1')
		{//dst client

			dstline.ctrl1='4';
			dstline.ctrl2='3';
			strcpy(dstline.from,recvl.local_name);
			strcpy(dstline.dst,recvl.dst_name);
			strcpy(dstline.data,recvl.data);
			
			printf("dstline.from:%s\n",dstline.from );
			printf("dstline.dst:%s\n",dstline.dst );
			printf("dstline.data:%s\n",dstline.data );
		}
		else if (recvl.ctrl2=='2')
		{
			dstline.ctrl1='4';
			dstline.ctrl2='3';
			strcpy(dstline.from,recvl.local_name);
			strcpy(dstline.dst,recvl.dst_name);//all
			strcpy(dstline.data,recvl.data);

		}

	}
	else if (recvl.ctrl1=='3')
	{
		if (recvl.ctrl2=='1')
		{
			unlogin(recvl.local_name);
		}
	}
		
		sendlineDst[0]=dstline.ctrl1;
		sendlineDst[1]=dstline.ctrl2;
		for ( i = 0; i < 10; ++i)
		{
			sendlineDst[2+i]=dstline.from[i];
		}
		for ( i = 0; i < 10; ++i)
		{
			sendlineDst[12+i]=dstline.dst[i];
		}
		for ( i = 0; dstline.data[i]!='\0'; ++i)
		{
			sendlineDst[22+i]=dstline.data[i];
		}


		//if need to send msg
		if (recvl.ctrl1=='2'&&recvl.ctrl2=='2')//group send
		{
			for ( i = 0; i < 10; ++i)
			{
				if (onlineUsers[i].on_off)//if on
				{
					sorline.ctrl1='4';
					sorline.ctrl2='1';
					send(onlineUsers[i].connfd,sendlineDst,MAXLINE,0);
					memset(sendlineDst,0,522);
					memset(dstline.data,0,500);
				}
			}
		}
		else if (recvl.ctrl1=='2'&&recvl.ctrl2=='1')
		{
			int findConnfd_t=-1;
			findConnfd_t=findConnfd(recvl.dst_name);
			printf("%d\n",findConnfd_t );
			if (findConnfd_t==-1)///send failed
			{
				sorline.ctrl1='4';
				sorline.ctrl2='2';
			}
			else 
			{
				sorline.ctrl1='4';
				sorline.ctrl2='1';
				send(findConnfd_t,sendlineDst,MAXLINE,0);//to certain one dst
				memset(sendlineDst,0,522);
				memset(dstline.data,0,500);
			}
		}
		
		sendlineSor[0]=sorline.ctrl1;
		sendlineSor[1]=sorline.ctrl2;
		for ( i = 0; i < 10; ++i)
		{
			sendlineSor[2+i]=sorline.from[i];
		}
		for ( i = 0; i < 10; ++i)
		{
			sendlineSor[12+i]=sorline.dst[i];
		}
		for ( i = 0; sorline.data[i]!='\0'; ++i)
		{
			sendlineSor[22+i]=sorline.data[i];
		}
		printf("sendlineSor:%s\n",sendlineSor );
		send(connfd,sendlineSor,MAXLINE,0);//to source
		memset(sendlineSor,0,522);
		memset(sorline.data,0,500);
		memset(recvline,0,532);
		memset(recvl.data,0,500);

	//}	
}
	if(n<0)
		printf("loop end\n");
	return NULL;
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
	int i;
	for ( i = 0; i < 10; ++i)
	{
		onlineUsers[i].on_off=0;//initial 0"offline
	}

/*-------------database initial--------------------*/
	int listenfd;
	//pid_t childpid;
	socklen_t clilen;

	struct sockaddr_in cliaddr,servaddr;
	listenfd=socket(AF_INET,SOCK_STREAM,0);

	const int on=1;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
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
		if (j==-1)
		{
			printf("%s\n","No room for new account!Please wait!" );
		}
		else
		onlineUsers[j].connfd=accept (listenfd,(struct sockaddr *) &cliaddr, &clilen);
		pthread_create(&thread[j],NULL,dealing,(void *)j);
		printf("%s\n","thread create succeed!" );

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
