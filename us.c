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

	for(;;)
	{
		len = clilen;
		/* waiting for receive data */
		n = recvfrom(sockfd, mesg, 80, 0, pcliaddr, &len);
		/* sent data back to client */
		sendto(sockfd, mesg, n, 0, pcliaddr, len);
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
