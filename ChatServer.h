#pragma once
#include "Constants.h"
#include "ChatServerlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>

typedef void(*ClientHandler)(int DataSocketFD);
typedef void(*TimeoutHandler)(void);


void StringToken(char str[], char username[], char password[]);//String Tokenizer

void FatalError(const char *ErrorMsg);		/* print error diagnostics and abort */

int MakeServerSocket(uint16_t PortNo);	/* create a socket on this server */

void PrintCurrentTime(void);	/*  print/update the current real time */

void ProcessRequest(int DataSocketFD);		/* process a time request by a client */

void ServerMainLoop(int ServSocketFD,	ClientHandler HandleClient,	TimeoutHandler HandleTimeout,	int Timeout, USERBASE *userbase);

