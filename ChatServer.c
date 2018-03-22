#include "ChatServer.h"
#include "ChatServerlist.h"
#include "Constants.h"
#include "Friendlist.h"
/*** functions ****************************************************/


const char *Program	= NULL;/* program name for descriptive diagnostics */

int Shutdown	= 0;	/* keep running until Shutdown == 1 */

USERBASE *userbase;

char sendusername[LOGINLEN],sendpassword[LOGINLEN];

int read_fds[8];
int write_fds[8];

char ClockBuffer[26] = "";	/* current time in printable format */

//String Tokenizer
void StringToken(char str[], char username[], char password[]) {
	const char s[2] = "-"; // The character to separate strings 
	char *token;
	int count = 0;
	/* get the first token */
	token = strtok(str, s);

	/* walk through other tokens */
	while (token != NULL) {
		token = strtok(NULL, s);
		if (count == 0) { strcpy(username, token); }
		if (count == 1) { strcpy(password, token); }
		count++;
	}
}

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

int MakeServerSocket(		/* create a socket on this server */
	uint16_t PortNo)
{
	int ServSocketFD;
	struct sockaddr_in ServSocketName;

	/* create the socket */
	ServSocketFD = socket(PF_INET, SOCK_STREAM, 0);
	if (ServSocketFD < 0)
	{
		FatalError("service socket creation failed");
	}
	/* bind the socket to this server */
	ServSocketName.sin_family = AF_INET;
	ServSocketName.sin_port = htons(PortNo);
	ServSocketName.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(ServSocketFD, (struct sockaddr*)&ServSocketName,
		sizeof(ServSocketName)) < 0)
	{
		FatalError("binding the server to a socket failed");
	}
	/* start listening to this socket */
	if (listen(ServSocketFD, 5) < 0)	/* max 5 clients in backlog */
	{
		FatalError("listening on socket failed");
	}
	return ServSocketFD;
} /* end of MakeServerSocket */

void PrintCurrentTime(void)	/*  print/update the current real time */
{
	time_t CurrentTime; /* seconds since 1970 (see 'man 2 time') */
	char   *TimeString;	/* printable time string (see 'man ctime') */
	char   Wheel,
		*WheelChars = "|/-\\";
	static int WheelIndex = 0;

	CurrentTime = time(NULL);	/* get current real time (in seconds) */
	TimeString = ctime(&CurrentTime);	/* convert to printable format */
	strncpy(ClockBuffer, TimeString, 25);
	ClockBuffer[24] = 0;	/* remove unwanted '/n' at the end */
	WheelIndex = (WheelIndex + 1) % 4;
	Wheel = WheelChars[WheelIndex];
	printf("\rClock: %s %c",	/* print from beginning of current line */
		ClockBuffer, Wheel);	/* print time plus a rotating wheel */
	fflush(stdout);
} /* end of PrintCurrentTime */

void ProcessRequest(		/* process a time request by a client */
	int DataSocketFD)
{
	int  i,l, n, k, h;
	char RecvBuf[256];	/* message buffer for receiving a message */
	char SendBuf[256];	/* message buffer for sending a response */

	USER onlineuser[USERNUM];


	printf("\nDataFD:%d\n",DataSocketFD);
	n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf) - 1);
	printf("\nread success:%s\n",RecvBuf);
/*				mark				*/
	if (n < 0)
	{
		FatalError("reading from data socket failed");
	}
	RecvBuf[n] = 0;
#ifdef DEBUG
	printf("%s: Received message: %s\n", Program, RecvBuf);
#endif
	if (0 == strncmp(RecvBuf, "WRITING", 7))//writing socket
	{
		char loginusername[LOGINLEN], loginpassword[LOGINLEN];
    StringToken(RecvBuf, loginusername, loginpassword);
    USER *temp_user = FindUSER(userbase, loginusername);
    temp_user->socketFD = DataSocketFD;
    for(i = 0; i < 8; i++)
    {
      if(write_fds[i] == 0)
      {
			  write_fds[i] = DataSocketFD;
        break;
      }
    }
    strncpy(SendBuf, "Writing socket connected.", sizeof(SendBuf) - 1);
    SendBuf[strlen(SendBuf)] = 0;
    printf("\n%s\n",SendBuf);
    n = send(temp_user->socketFD, SendBuf, strlen(SendBuf)+1,0);
    /*for(i=0; i<USERNUM; i++){
  		if(strcmp(temp_user->friends[i], "")  != 0){
        USER *friend = FindUSER(userbase,temp_user->friends[i]);
  			strncpy(SendBuf, "FRIEND-", 8);
        strcat(SendBuf, friend->username);
        strcat(SendBuf, "-");
        if (friend->socketFD != 0)
          strcat(SendBuf, "ONLINE");
        else
          strcat(SendBuf, "OFFLINE");
        SendBuf[strlen(SendBuf)] = 0;
        printf("\n%s\n",SendBuf);
        n = send(temp_user->socketFD,SendBuf,strlen(SendBuf)+1,0);
  		}
	  }*/
    
	}
	else if (0 == strncmp(RecvBuf, "LOGIN", 5))//LOGIN command "LOGIN-username-password"
	{
		char loginusername[LOGINLEN] = " ";
		char loginpassword[LOGINLEN] = " ";
		int m;
		int i = 0;
		StringToken(RecvBuf, loginusername, loginpassword);
		//For test
		//printf("\nUsername entered: %s\n", loginusername);
		//printf("\nPassword entered: %s\n", loginpassword);
		//printf("\nUsername[0]: %s\n", userbase[0].username);
		//printf("\nstrcmp = : %d\n", strcmp(loginusername, userbase[m].username));
    USER *found = FindUSER(userbase, loginusername);
    if (found != NULL)
    {
      if (0 == strcmp(found->password,loginpassword))
      {
        strncpy(SendBuf, "LOGIN-", 7);
        strcat(SendBuf, loginusername);
        strcat(SendBuf, "-");
        strcat(SendBuf, loginpassword);
      }
  		else {
  		  strncpy(SendBuf, "Password Incorrect!", sizeof(SendBuf) - 1);
  		}
		}
		else {
		  strncpy(SendBuf, "No matched username in database!", sizeof(SendBuf) - 1);
		}
		SendBuf[sizeof(SendBuf) - 1] = 0;
    n = write(DataSocketFD, SendBuf, strlen(SendBuf));
    printf("\nFD:%d\n",DataSocketFD);
    close(DataSocketFD);
	}
	else if (0 == strncmp(RecvBuf, "REGISTER", 7))//REGISTER command "REGISTER-username-password"
	{
		char registerusername[LOGINLEN] = " ";
		char registerpassword[LOGINLEN] = " ";
		int m;
		int i = 0;
	  USER *used = NULL;
		StringToken(RecvBuf, registerusername, registerpassword);

		// Check if the name is already used
		used = FindUSER(userbase, registerusername);

		if (used == NULL) {
			userbase = NewUSER(userbase, registerusername, registerpassword);
      strncpy(SendBuf, "New account registered successfully!", sizeof(SendBuf) - 1);
		}
    else{
      strncpy(SendBuf, "Error: Username already used!", sizeof(SendBuf) - 1);
    }
		SendBuf[sizeof(SendBuf) - 1] = 0;
    n = write(DataSocketFD, SendBuf, strlen(SendBuf));
    printf("\nFD:%d\n",DataSocketFD);
    close(DataSocketFD);
	}		

	else if (0 == strncmp(RecvBuf, "ADD", 3))
	{
    char username[LOGINLEN] = " ";
		char message[256] = " ";
    StringToken(RecvBuf, username, message);
    USER *from = FindUSER(userbase, sendusername);
    USER *target = FindUSER(userbase, username);
    strncpy(SendBuf, "REQUEST-", 9);
    strcat(SendBuf, sendusername);
    strcat(SendBuf, "-");
    strcat(SendBuf, message);
    if (target == NULL){
      strncpy(SendBuf, "Error: No such user!", sizeof(SendBuf) - 1);
    }
    else if (target->socketFD != 0)
    {
      n = send(target->socketFD,SendBuf,strlen(SendBuf),0);
      strncpy(SendBuf, "Invitation sent!", sizeof(SendBuf) - 1);
    }
    else
    {
      strncpy(SendBuf, "Error: Friend not online!", sizeof(SendBuf) - 1);
    }
    SendBuf[sizeof(SendBuf) - 1] = 0;
    n = write(DataSocketFD, SendBuf, strlen(SendBuf));
    close(DataSocketFD);
	}
  else if (0 == strncmp(RecvBuf, "ACCEPT",6))
  {
    char username[LOGINLEN] = " ";
		char message[256] = " ";
    StringToken(RecvBuf, username, message);
    USER *from = FindUSER(userbase, sendusername);
    USER *target = FindUSER(userbase, username);
    strncpy(SendBuf, "ADD-", 5);
    strcat(SendBuf, sendusername);
    strcat(SendBuf, "-");
    strcat(SendBuf, message);
    if (target->socketFD != 0)
    {
      n = send(target->socketFD,SendBuf,strlen(SendBuf),0);
      from = AddUser(from,target);
      target = AddUser(target,from);
      strncpy(SendBuf, "ACCEPT-", 8);
      strcat(SendBuf, username);
      strcat(SendBuf, "-");
      strcat(SendBuf, message);
    }
    else
    {
      strncpy(SendBuf, "Error: Friend not online!", sizeof(SendBuf) - 1);
    }
    SendBuf[sizeof(SendBuf) - 1] = 0;
    n = write(DataSocketFD, SendBuf, strlen(SendBuf));
    close(DataSocketFD);
  }
  else if (0 == strncmp(RecvBuf, "REJECT",6))
  {
    strncpy(SendBuf, "Invitation rejected!", sizeof(SendBuf) - 1);
    SendBuf[sizeof(SendBuf) - 1] = 0;
    n = write(DataSocketFD, SendBuf, strlen(SendBuf));
    close(DataSocketFD);
  }
  else if (0 == strncmp(RecvBuf, "DELETE",6))
  {
    char username[LOGINLEN] = " ";
		char message[256] = " ";
    StringToken(RecvBuf, username, message);
    USER *from = FindUSER(userbase, sendusername);
    USER *target = FindUSER(userbase, username);
    strncpy(SendBuf, "DELETE-", 8);
    strcat(SendBuf, sendusername);
    strcat(SendBuf, "-");
    strcat(SendBuf, message);
    if (target->socketFD != 0)
    {
      n = send(target->socketFD,SendBuf,strlen(SendBuf),0);
      from = DeleteUser(from,target);
      target = DeleteUser(target,from);
      strncpy(SendBuf, "Deleted", sizeof(SendBuf) - 1);
    }
    else
    {
      strncpy(SendBuf, "Error: Friend not online!", sizeof(SendBuf) - 1);
    }
    SendBuf[sizeof(SendBuf) - 1] = 0;
    n = write(DataSocketFD, SendBuf, strlen(SendBuf));
    close(DataSocketFD);
  }
  else if (0 == strncmp(RecvBuf, "CLOSE",5))
  {
    char username[LOGINLEN] = " ";
		char message[256] = " ";
    char filecreate[256];
    StringToken(RecvBuf, username, message);
    strcpy(filecreate, username);
  	strcat(filecreate, "_friends.txt");	
    int rem = remove(filecreate);
  	FILE *fp = NULL;
    fp = fopen(filecreate, "w");
    USER *closed = FindUSER(userbase, username);
    for (i=0; i<USERNUM; i++){ 
  		if(strcmp(closed->friends[i], "") == 0){
        fprintf(fp, "%s\n",username);
  		}
	  }  
  	fclose(fp);
    close(closed->socketFD);
    closed->socketFD = 0;
    strncpy(SendBuf, "Closed", sizeof(SendBuf) - 1);
    SendBuf[sizeof(SendBuf) - 1] = 0;
    n = write(DataSocketFD, SendBuf, strlen(SendBuf));
    close(DataSocketFD);
  }
  else if (0 == strncmp(RecvBuf, "CHAT",4))
  {
    char username[LOGINLEN] = " ";
		char message[256] = " ";
    StringToken(RecvBuf, username, message);
  }
  else if (0 == strncmp(RecvBuf, "SEND",4))
  {
    char username[LOGINLEN] = " ";
		char message[256] = " ";
    StringToken(RecvBuf, username, message);
    USER *from = FindUSER(userbase, sendusername);
    USER *target = FindUSER(userbase, username);
    strncpy(SendBuf, "SEND-", 7);
    strcat(SendBuf, sendusername);
    strcat(SendBuf, "-");
    strcat(SendBuf, message);
    //printf("\n%d,%d\n",DataSocketFD, target->socketFD);
    n = send(target->socketFD,SendBuf,strlen(SendBuf),0);
        printf("\nn:%d,%s\n",n,message);
	strcpy(SendBuf, "Sent");
        //cnt++;
       // break;
	  n = write(DataSocketFD, SendBuf, strlen(SendBuf));
     printf("\nFD:%d\n",DataSocketFD);
    close(DataSocketFD);
  }
	else
	{
		strncpy(SendBuf, "ERROR unknown command ", sizeof(SendBuf) - 1);
		SendBuf[sizeof(SendBuf) - 1] = 0;
		strncat(SendBuf, RecvBuf, sizeof(SendBuf) - 1 - strlen(SendBuf));
    n = write(DataSocketFD, SendBuf, strlen(SendBuf));
    printf("\nFD:%d\n",DataSocketFD);
    close(DataSocketFD);
	}
	//l = strlen(SendBuf);
#ifdef DEBUG
	printf("%s: Sending response: %s.\n", Program, SendBuf);
#endif
	printf("\nresponse:%s\n",SendBuf);
	//n = write(DataSocketFD, SendBuf, l);
	if (n < 0)
	{
		FatalError("writing to data socket failed");
	}

} /* end of ProcessRequest */

void ServerMainLoop(		/* simple server main loop */
	int ServSocketFD,		/* server socket to wait on */
	ClientHandler HandleClient,	/* client handler to call */
	TimeoutHandler HandleTimeout,	/* timeout handler to call */
	int Timeout,
	USERBASE *userbase)			
{
	int DataSocketFD;	/* socket for a new client */
	socklen_t ClientLen;
	struct sockaddr_in
	ClientAddress;	/* client address we connect with */
	fd_set WriteFDs;	/* socket file descriptors to select from */
	fd_set ReadFDs;	/* socket file descriptors ready to read from */
	struct timeval TimeVal;
	int n,res, i, max_fd,k = 0;
	char temp[256];

  for(i = 0; i < 8; i++) 
  {
  	read_fds[i] = 0;
  	write_fds[i] = 0;
  	printf("\n%d,%d,%d\n",i,read_fds[i],write_fds[i]);
  }
	while (!Shutdown)
	{
    FD_ZERO(&ReadFDs);  
    FD_ZERO(&WriteFDs);
    FD_SET(ServSocketFD, &ReadFDs);
  	for(i = 0; i < 8; i++)  
  	{
  	  if(read_fds[i]!=0){
  		FD_SET(read_fds[i],&ReadFDs);
  		//printf("\n%d\n",read_fds[i]);
  	  }
  	  if(write_fds[i]!=0){
  		  FD_SET(write_fds[i],&WriteFDs);
  		//  printf("%d\n",write_fds[i]);
  	  }  
  	}	
  	
  	TimeVal.tv_sec = Timeout / 1000000;	/* seconds */
  	TimeVal.tv_usec = Timeout % 1000000;	/* microseconds */
  	/* block until input arrives on active sockets or until timeout */
  	
    res = select(FD_SETSIZE, &ReadFDs, NULL, NULL, &TimeVal);
  	if (res < 0)
  	{
  		FatalError("wait for input or timeout (select) failed");
  	}
  	else if (res == 0)	/* timeout occurred */
  	{
    #ifdef DEBUG
      printf("%s: Handling timeout...\n", Program);
    #endif
    HandleTimeout();
    }		
    else		/* some FDs have data ready to read */
    {
  		if(FD_ISSET(ServSocketFD, &ReadFDs)){
    		printf("%s: Accepting new client %d...\n", Program, i);
        ClientLen = sizeof(ClientAddress);
    	  DataSocketFD = accept(ServSocketFD,(struct sockaddr*)&ClientAddress, &ClientLen);
        if (DataSocketFD < 0)
        {
          FatalError("data socket creation (accept) failed");
    	  }
        printf("%s: Client %d connected from %s:%hu.\n",Program, i,inet_ntoa(ClientAddress.sin_addr),ntohs(ClientAddress.sin_port));
  	    n = recv(DataSocketFD,temp, sizeof(temp)-1,0);
        temp[n] = 0;
  		  StringToken(temp, sendusername,sendpassword);
    		printf("\n%s\n",temp);

    		for(i = 0; i < 8; i++)  
        {  
          if(read_fds[i] == 0)  
          {   
            read_fds[i] = DataSocketFD;  
            break;  
          }  
        }  

  	  }
    	for (i = 0; i<8; i++)
    	{
    	  if (read_fds[i] != 0)
    	  {
    			if (FD_ISSET(read_fds[i], &ReadFDs))
    			{
    			  HandleClient(read_fds[i]);
            read_fds[i] = 0; 
            break;
    			}
    	  }
      }
  				 /* active communication with a client */
  }
}
}
 /* end of ServerMainLoop */



  /*** main function *******************************************************/

int main(int argc, char *argv[])
{
	int ServSocketFD;	/* socket file descriptor for service */
	int PortNo;		/* port number */

					//arrays of usernames and passwords in database
	printf("\nmark\n");
	userbase = InitializeUserbase();
	printf("\nmark\n");

	Program = argv[0];	/* publish program name (for diagnostics) */
#ifdef DEBUG
	printf("%s: Starting...\n", Program);
#endif
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s port\n", Program);
		exit(10);
	}
	PortNo = atoi(argv[1]);	/* get the port number */
	if (PortNo <= 2000)
	{
		fprintf(stderr, "%s: invalid port number %d, should be >2000\n",
			Program, PortNo);
		exit(10);
	}
#ifdef DEBUG
	printf("%s: Creating the server socket...\n", Program);
#endif

	ServSocketFD = MakeServerSocket(PortNo);
	printf("%s: Providing current time at port %d...\n", Program, PortNo);
	ServerMainLoop(ServSocketFD, ProcessRequest,
		PrintCurrentTime, 250000, userbase);
	printf("\n%s: Shutting down.\n", Program);
	close(ServSocketFD);
	return 0;
}
