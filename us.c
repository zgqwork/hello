#if 0
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 80
#define SERV_PORT 8888

void do_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen)
{
	int n;
	socklen_t len;
	char mesg[80];
	struct sockaddr_in *client_addr;
	
	for(;;)
	{
		len = clilen;
		/* waiting for receive data */
		n = recvfrom(sockfd, mesg, 80, 0, pcliaddr, &len);
		/* sent data back to client */
		
		client_addr = (struct sockaddr_in*)pcliaddr;
		printf("%s %d", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
		
		sendto(sockfd, mesg, strlen(mesg), 0, pcliaddr, len);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0); /* create a socket */

	/* init servaddr */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //#define INADDR_ANY   ((unsigned long int) 0x00000000)
	servaddr.sin_port = htons(8888);

	/* bind address and port to socket */
	if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		perror("bind error");
		exit(1);
	}

	do_echo(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

	return 0;
}
#endif

#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
 
#define lport 8888
 
int main()
{
    int mysock,len;
    struct sockaddr_in addr;
    int i=0;
    char msg[256];
 
    int addr_len;
 
    if((mysock=socket(AF_INET,SOCK_DGRAM,0))<0)
    {
        perror("error");
        exit(1);
    }
    else
    {
        printf("socket created.\n");
        printf("socket ID: %d\n",mysock);
    }
    addr_len=sizeof(struct sockaddr_in);
    bzero(&addr,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(lport);
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
 
    if(bind(mysock,&addr,sizeof(addr))<0)
    {
        perror("connect");
        exit(1);
    }
    else
    {
        printf("bind ok.\n");
        printf("local port: %d\n",lport);
    }
    while(1)
    {
        bzero(msg,sizeof(msg));
        len=recvfrom (mysock,msg,sizeof(msg),0,&addr,&addr_len);
 
        printf("%d: ",i);
        i++;
        printf("message form: %s\n",inet_ntoa(addr.sin_addr));
        printf("message: %s\n\n",msg);
        sendto(mysock,msg,len,0,&addr,addr_len);
    }
    return 0;
}