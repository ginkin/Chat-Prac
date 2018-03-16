#include "ChatServer.h"

/*** functions ****************************************************/




const char *Program	= NULL;/* program name for descriptive diagnostics */

int Shutdown	= 0;	/* keep running until Shutdown == 1 */

char ClockBuffer[26] = "";	/* current time in printable format */

//initialize an array of users
void InitializeUsers(USER user[], int size) {
	int i; int j;
	for (i = 0; i < size; i++) {
		strcpy(user[i].username, "None");
		strcpy(user[i].password, "None");
		user[i].isconnected = 0; // 0 is offline 1 is online
		for (j = 0; j < USERNUM; j++) {
			strcpy(user[i].friends[j], "None");
		}
	}
}

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
	int DataSocketFD, USER userbase[USERNUM])
{
	int  l, n, k, h;
	char RecvBuf[256];	/* message buffer for receiving a message */
	char SendBuf[256];	/* message buffer for sending a response */

	USER onlineuser[USERNUM];
	InitializeUsers(onlineuser, USERNUM);

	// Scan Userbase to add all onlineusers to the array
	for (k = 0; k < USERNUM; k++) {
		if (userbase[k].isconnected == 1) {
			for (h = 0; h < USERNUM; h++) {
				if (onlineuser[h].isconnected == 0) {
					onlineuser[h] = userbase[k];
				}
			}
		}
	}

	n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf) - 1);
	if (n < 0)
	{
		FatalError("reading from data socket failed");
	}
	RecvBuf[n] = 0;
#ifdef DEBUG
	printf("%s: Received message: %s\n", Program, RecvBuf);
#endif
	if (0 == strcmp(RecvBuf, "TIME"))
	{
		strncpy(SendBuf, "OK TIME: ", sizeof(SendBuf) - 1);
		SendBuf[sizeof(SendBuf) - 1] = 0;
		strncat(SendBuf, ClockBuffer, sizeof(SendBuf) - 1 - strlen(SendBuf));
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
		for (m = 0; m < USERNUM; m++) {
			if (strcmp(loginusername, userbase[m].username) == 0) { //strncmp returns 0 when there is matched username
				if (strcmp(loginpassword, userbase[m].password) == 0) {
					userbase[m].isconnected = 1;
					strncpy(SendBuf, "Logged in successfully!", sizeof(SendBuf) - 1);
					printf("\nUser %s is Logged in: %d\n", userbase[m].username, userbase[m].isconnected);
					break;
				}
				else {
					strncpy(SendBuf, "Password Incorrect!", sizeof(SendBuf) - 1);
					break;
				}
			}
			else {
				strncpy(SendBuf, "No matched username in database!", sizeof(SendBuf) - 1);
			}
		}
		SendBuf[sizeof(SendBuf) - 1] = 0;
	}
	else if (0 == strncmp(RecvBuf, "REGISTER", 7))//REGISTER command "REGISTER-username-password"
	{
		char registerusername[LOGINLEN] = " ";
		char registerpassword[LOGINLEN] = " ";
		int m;
		int i = 0;
		int used = 0;
		StringToken(RecvBuf, registerusername, registerpassword);

		// Check if the name is already used
		for (m = 0; m < USERNUM; m++) {
			if (strcmp(registerusername, userbase[m].username) == 0) { //strncmp returns 0 when there is matched username
				strncpy(SendBuf, "Error: Username already used!", sizeof(SendBuf) - 1);
				used = 1;
				break;
			}
		}

		if (used == 0) {
			for (m = 0; m < USERNUM; m++) {
				if (strcmp("None", userbase[m].username) == 0) {
					strcpy(userbase[m].username, registerusername);
					strcpy(userbase[m].password, registerpassword);
					printf("New User %d registered.", userbase[m].username);
					strncpy(SendBuf, "New account registered successfully!", sizeof(SendBuf) - 1);
					break;
				}
			}
		}
		SendBuf[sizeof(SendBuf) - 1] = 0;
	}
	else if(0 == strcmp(RecvBuf, "LIST") )//LIST command "LIST" -tells user which of its contacts are online
	{
		printf("List of online friends coming soon..");
	}
	else if(0 == strcmp(RecvBuf, "UPDATE") )//UPDATE command "UPDATE" -updates the status of the user to the server
	{
	
	}
	else if (0 == strcmp(RecvBuf, "SHUTDOWN"))
	{
		Shutdown = 1;
		strncpy(SendBuf, "OK SHUTDOWN", sizeof(SendBuf) - 1);
		SendBuf[sizeof(SendBuf) - 1] = 0;
	}
	else
	{
		strncpy(SendBuf, "ERROR unknown command ", sizeof(SendBuf) - 1);
		SendBuf[sizeof(SendBuf) - 1] = 0;
		strncat(SendBuf, RecvBuf, sizeof(SendBuf) - 1 - strlen(SendBuf));
	}
	l = strlen(SendBuf);
#ifdef DEBUG
	printf("%s: Sending response: %s.\n", Program, SendBuf);
#endif
	n = write(DataSocketFD, SendBuf, l);
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
	USER user[USERNUM],
	USER userbase[USERNUM]
)			/* timeout in micro seconds */
{
	int DataSocketFD;	/* socket for a new client */
	socklen_t ClientLen;
	struct sockaddr_in
		ClientAddress;	/* client address we connect with */
	fd_set ActiveFDs;	/* socket file descriptors to select from */
	fd_set ReadFDs;	/* socket file descriptors ready to read from */
	struct timeval TimeVal;
	int res, i;

	FD_ZERO(&ActiveFDs);		/* set of active sockets */
	FD_SET(ServSocketFD, &ActiveFDs);	/* server socket is active */
	while (!Shutdown)
	{
		ReadFDs = ActiveFDs;
		TimeVal.tv_sec = Timeout / 1000000;	/* seconds */
		TimeVal.tv_usec = Timeout % 1000000;	/* microseconds */
												/* block until input arrives on active sockets or until timeout */
		res = select(FD_SETSIZE, &ReadFDs, NULL, NULL, &TimeVal);
		if (res < 0)
		{
			FatalError("wait for input or timeout (select) failed");
		}
		if (res == 0)	/* timeout occurred */
		{
#ifdef DEBUG
			printf("%s: Handling timeout...\n", Program);
#endif
			HandleTimeout();
		}
		else		/* some FDs have data ready to read */
		{
			for (i = 0; i<FD_SETSIZE; i++)
			{
				if (FD_ISSET(i, &ReadFDs)) {
					if (i == ServSocketFD) {	/* connection request on server socket */
						printf("%s: Accepting new client %d...\n", Program, i);
						ClientLen = sizeof(ClientAddress);
						DataSocketFD = accept(ServSocketFD,
							(struct sockaddr*)&ClientAddress, &ClientLen);
						if (DataSocketFD < 0)
						{
							FatalError("data socket creation (accept) failed");
						}
						printf("%s: Client %d connected from %s:%hu.\n",
							Program, i,
							inet_ntoa(ClientAddress.sin_addr),
							ntohs(ClientAddress.sin_port));
						FD_SET(DataSocketFD, &ActiveFDs);
						//printf("New connection , socket fd is %d , ip is : %s , port : %d \n",
						//DataSocketFD, inet_ntoa(ClientAddress.sin_addr), ntohs(ClientAddress.sin_port));
					}
					else
					{   /* active communication with a client */
#ifdef DEBUG
						printf("%s: Dealing with client %d...\n", Program, i);
#endif
						HandleClient(i, userbase);
#ifdef DEBUG
						printf("%s: Closing client %d connection.\n", Program, i);
#endif
						close(i);
						FD_CLR(i, &ActiveFDs);
					}
				}
			}
		}
	}
} /* end of ServerMainLoop */

  /*** main function *******************************************************/

int main(int argc, char *argv[])
{
	int ServSocketFD;	/* socket file descriptor for service */
	int PortNo;		/* port number */

					//arrays of usernames and passwords in database
	USER userbase[USERNUM];
	InitializeUsers(userbase, USERNUM);

	//We will be able to read all user's info into the program
	strcpy(userbase[0].username, "Ray");
	strcpy(userbase[0].password, "Ray");
	strcpy(userbase[1].username, "Micky");
	strcpy(userbase[1].password, "Micky");

	//arrays of usernames and passwords for connected users
	USER user[USERNUM];
	InitializeUsers(user, 30);

	//arrays of users' msgs sent to server;
	char usermsg[30][MSGNUM][MSGLEN];
	int usermsgnum[30] = { 0 };

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
		PrintCurrentTime, 250000, user, userbase);
	printf("\n%s: Shutting down.\n", Program);
	close(ServSocketFD);
	return 0;
}
