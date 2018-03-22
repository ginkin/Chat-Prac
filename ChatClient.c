/* ClockClient.c: simple interactive TCP/IP client for ClockServer
 * Author: Rainer Doemer, 2/16/15 (updated 2/20/17)
 */

#include "Constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include "ChatClient.h"
#include <pthread.h>
/* #define DEBUG */	/* be verbose */

void FatalError(		/* print error diagnostics and abort */
	const char *ErrorMsg)
{
    fputs(Program, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program, stderr);
    fputs(": Exiting!\n", stderr);
    exit(20);
} /* end of FatalError */

void *recv_msg(void *SocketFD)
{
	int *socket = (int *)SocketFD;
	char temp[256];
	int n;
	while(1)
	{
		n = recv(*socket,temp,256,0);
       		printf("\n%d,%s\n",n,temp);
	         printf("\n%s",temp);
	}
	
}

int main(int argc, char *argv[])
{
    int l, n;
    int SocketFD,SocketFD2,	/* socket file descriptor */
	PortNo;		/* port number */
    struct sockaddr_in
	ServerAddress;	/* server address we connect with */
    struct hostent
	*Server;	/* server host */
    char SendBuf[256];	/* message buffer for sending a message */
    char RecvBuf[256];	/* message buffer for receiving a response */
	
//	pthread_t t1;
//	pthread_create(&t1,NULL,recv_msg,(void *)SocketFD);
//	pthread_join(t1,NULL);
    Program = argv[0];	/* publish program name (for diagnostics) */
#ifdef DEBUG
    printf("%s: Starting...\n", argv[0]);
#endif
    if (argc < 3)
    {   fprintf(stderr, "Usage: %s hostname port\n", Program);
	exit(10);
    }
    Server = gethostbyname(argv[1]);
    if (Server == NULL)
    {   fprintf(stderr, "%s: no such host named '%s'\n", Program, argv[1]);
        exit(10);
    }
    PortNo = atoi(argv[2]);
    if (PortNo <= 2000)
    {   fprintf(stderr, "%s: invalid port number %d, should be >2000\n",
		Program, PortNo);
        exit(10);
    }
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PortNo);
    ServerAddress.sin_addr = *(struct in_addr*)Server->h_addr_list[0];
	
	printf("\n\n**********************************************************************\nWelcome to the Chat program. \nTo Login, use command: \n"
		"LOGIN-username-password\n\nTo Register with the server, use command: \n"
		"REGISTER-username-password\n\nFor alpha version two preset usernames and passwords \n"
		"are available: \'LOGIN-Ray-Ray\' and \'LOGIN-Micky-Micky\'.\n\nEnter SHUTDOWN to quit server and client\nEnter bye to disconnect" 
		"from server\n\nHAPPY CHATTING!!!\n**************************************************************************\n\n");
SocketFD = socket(AF_INET, SOCK_STREAM, 0);
if (connect(SocketFD, (struct sockaddr*)&ServerAddress,
                        sizeof(ServerAddress)) < 0)
            {   FatalError("connecting to server failed");
            }
	pthread_t t1;
        pthread_create(&t1,NULL,recv_msg,(void *)&SocketFD);
//        pthread_join(t1,NULL);

	char temp[256];
	strncpy(temp,"WritingFD",sizeof(temp)-1);
	n = send(SocketFD, temp, 9,0);	
    	n = recv(SocketFD, temp,9,0);
//	printf("\n%s\n",temp);
	int cnt = 1;
	do
    {	printf("%s: Enter a command to send to the chat server:\n"
		//"         'TIME' to obtain the current time,\n"
		//"         'SHUTDOWN' to terminate the server,\n"
		//"         or 'bye' to quit this client\n"
		"command: ", argv[0]);
	fgets(SendBuf, sizeof(SendBuf), stdin);
	/*if (!cnt){
		n = recv(SocketFD,temp,256,0);
		printf("\n%s",temp);
	}*/
	//strcpy(SendBuf,"REGISTER-a-a");
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{   SendBuf[--l] = 0;
	}
	if (0 == strcmp("bye", SendBuf))
	{   break;
	}
	if (l && cnt)
	{
	cnt--;
	   SocketFD2 = socket(AF_INET, SOCK_STREAM, 0);
	    printf("\n%d\n",SocketFD2);
      if (SocketFD2 < 0)
	    {   FatalError("socket creation failed");
	    }
	    printf("%s: Connecting to the server at port %d...\n",
			Program, PortNo);
	    if (connect(SocketFD2, (struct sockaddr*)&ServerAddress,
			sizeof(ServerAddress)) < 0)
	    {   FatalError("connecting to server failed");
	    }
            strncpy(temp,"ReadingFD",sizeof(temp)-1);
            n = send(SocketFD2, temp, 9,0);
	    n = recv(SocketFD2, temp, 9,0);	    
            printf("%s: Sending message '%s'...\n", Program, SendBuf);
	    n = write(SocketFD2, SendBuf, l);
	    if (n < 0)
	    {   FatalError("writing to socket failed");
	    }
#ifdef DEBUG
	    printf("%s: Waiting for response...\n", Program);
#endif
	    n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	    if (n < 0) 
	    {   FatalError("reading from socket failed");
	    }
	    RecvBuf[n] = 0;
	    printf("%s: Received response: %s\n", Program, RecvBuf);
#ifdef DEBUG
	    printf("%s: Closing the connection...\n", Program);
#endif
	    close(SocketFD2);
	}
    } while(0 != strcmp("SHUTDOWN", SendBuf));
    printf("%s: Exiting...\n", Program);
    return 0;
}

/* EOF ClockClient.c */
