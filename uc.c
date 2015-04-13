#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 80
#define SERV_PORT 8888
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void do_cli(FILE *fp, int sockfd, struct sockaddr *pservaddr, 
		socklen_t servlen)
{
	int n;
	char sendline[80], recvline[80 + 1];
	struct sockaddr local_addr;
	socklen_t len = sizeof(struct sockaddr);
	struct sockaddr_in *client_addr;
	char port[10];
	
	/* connect to server */
	if(connect(sockfd, (struct sockaddr *)pservaddr, servlen) == -1)
	{
		perror("connect error");
		exit(1);
	}
	
	
	if (getsockname(sockfd, &local_addr, &len) == 0) {
		client_addr = (struct sockaddr_in*)&local_addr;
		printf("%s %d\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
	}
	
	
	while(fgets(sendline, 80, fp) != NULL)
	{
		/* read a line and send to server */
		write(sockfd, sendline, strlen(sendline));
		/* receive data from server */
		n = read(sockfd, recvline, 80);
		if(n == -1)
		{
			perror("read error");
			exit(1);
		}
		recvline[n] = 0; /* terminate string */
		//fputs(recvline, stdout);
		printf("recv==%s\n", recvline);
		
		int sock_fd;  
				struct sockaddr_in server_addr;
				int yes = 1;
				
				socklen_t sin_size;
				if ((sock_fd = socket(AF_INET, SOCK_DGRAM/*SOCK_STREAM*/, 0)) == -1) {
					perror("socket");
					exit(1);
				}
				if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
					perror("setsockopt");
					exit(1);
				}
				server_addr.sin_family = AF_INET;         // host byte order
				server_addr.sin_port = htons(atoi(port));     // short, network byte order
				server_addr.sin_addr.s_addr = INADDR_ANY;//inet_addr("192.168.0.24"); // automatically fill with my IP
				memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));
				if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
					perror("bind");
					exit(1);
				}
				/*if (listen(sock_fd, 100) == -1) {
					perror("listen");
					exit(1);
				}*/
				printf("listen port %s\n", port);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	/* check args */
	if(argc != 2)
	{
		printf("usage: udpclient <IPaddress>\n");
		exit(1);
	}

	/* init servaddr */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8888);
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
	{
		printf("[%s] is not a valid IPaddress\n", argv[1]);
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	do_cli(stdin, sockfd, (struct sockaddr *)&servaddr,
			sizeof(servaddr));

	return 0;
}
