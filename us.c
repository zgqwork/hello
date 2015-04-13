#if 1
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 80
#define SERV_PORT 8888

//void do_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen)
void do_echo(int sockfd, struct sockaddr_in *client_addr)
{
	int n;
	socklen_t sin_size;
	char mesg[80];
	
	for(;;)
	{
		sin_size = sizeof(struct sockaddr_in);
		/* waiting for receive data */
		printf("---sin_size=%d\n", sin_size);
		n = recvfrom(sockfd, mesg, 80, 0, (struct sockaddr *)client_addr, &sin_size);
		/* sent data back to client */
		printf("---sin_size=%d\n", sin_size);
		//client_addr = (struct sockaddr_in*)pcliaddr;
		//printf("%s %d", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
		printf("received packet from %s:\n", inet_ntoa(client_addr->sin_addr));
		
		//sendto(sockfd, mesg, n, 0, (struct sockaddr *)client_addr, sin_size);
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

	//do_echo(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
	
	//do_echo(sockfd, &cliaddr);
	
	
	int n;
    socklen_t len;
    char mesg[80];
	struct sockaddr *pcliaddr = (struct sockaddr *)&cliaddr;
    for(;;)
    {
		len = sizeof(cliaddr);
        /* waiting for receive data */
        n = recvfrom(sockfd, mesg, 80, 0, pcliaddr, &len);
        /* sent data back to client */
		printf("received packet from %s:\n", inet_ntoa(pcliaddr.sin_addr));
        sendto(sockfd, mesg, n, 0, pcliaddr, len);
    }
	
	
	return 0;
}
#endif




#if 0
#include <stdio.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#define BUFSIZE 1000

int

main (int argc, char *argv[])

{

  int s;

  int fd;

  int len;

  struct sockaddr_in my_addr;

  struct sockaddr_in remote_addr;

  int sin_size;

  char buf[BUFSIZE];

  //memset (&my_addr, 0, sizeof (my_addr));

  my_addr.sin_family = AF_INET;

  my_addr.sin_addr.s_addr = INADDR_ANY;

  my_addr.sin_port = htons (8888);

  if ((s = socket (PF_INET, SOCK_DGRAM, 0)) < 0)

    {

      perror ("socket");

      return 1;

    }

  if (bind (s, (struct sockaddr *) &my_addr, sizeof (struct sockaddr)) < 0)

    {

      perror ("bind");

      return 1;

    }

  sin_size = sizeof (struct sockaddr_in);

  printf ("waiting for a packet...\n");

  for(;;)
  {
	if ((len = recvfrom (s, buf, BUFSIZE, 0, (struct sockaddr *)&remote_addr, &sin_size)) < 0)
    {
      perror ("recvfrom");
      return 1;
    }
    printf("received packet from %s:%d\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
	sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&remote_addr, &sin_size);
  }
  /*
  buf[len] = '\0';

  printf ("contents:%s\n", buf);

  close (s);
  */
  return 0;
}
#endif


