#pragma once



#define PORT 55554
#define LOGINLEN 50 //length for username and password
#define USERNUM 100 //number of all users 
#define MSGNUM 100 
#define MSGLEN 1025

/* #define DEBUG */	/* be verbose */

/*** type definitions ****************************************************/

typedef struct appuser USER;
struct appuser {
	char username[LOGINLEN];
	char password[LOGINLEN];
	int socketFD;
	char friends[USERNUM][LOGINLEN];
	char Requests[USERNUM][LOGINLEN];
};

typedef struct base{
	struct base *prev;
	USER *user;
} USERBASE;




