#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#define MAXLINE 4096
#define SERV_PORT 3000

int main(int argc,char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE],recvline[MAXLINE];

	if(argc !=2){
		perror("Usage:TCPClient<IP address of the server>");
		exit(1);
	}

	sockfd=socket(AF_INET,SOCK_STREAM,0);

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=inet_addr(argv[1]);
	servaddr.sin_port=htons(SERV_PORT);

	connect(sockfd,(struct sockaddr *)&servaddr ,sizeof(servaddr));

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
}
