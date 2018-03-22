#include <stdio.h>
#include "ChatServerlist.h"
#include "Constants.h"
#include <stdlib.h>
#include <string.h>
//#include "Constants.h"
//#include "Friendlist.h"
USERBASE *InitializeUserbase(void)
 {
	 USER *TempUSER;
   USERBASE *userbase = (USERBASE *)malloc(sizeof(USERBASE));
	 USERBASE *e;
	 char username[LOGINLEN];
 	 char password[LOGINLEN];
	 char line[LOGINLEN];
	 char line2[LOGINLEN];
	 char friend_temp[LOGINLEN];
	 char filecreate[256];
 	 FILE *fp = NULL;
	 FILE *ff = NULL;
 	 int friends = 0;
	 int requests = 0;
   if(!userbase){
     return NULL;
   }
  
  USER *default_user = CreateUSERStruct();
  strcpy(default_user->username, "PeterAnteater");
  strcpy(default_user->password, "87654321"); 
  ff = fopen("PeterAnteater_friends.txt", "r");
  friends = 0;
	while(fgets(line2, LOGINLEN, ff) != NULL){
	  sscanf(line2, "%s \n", friend_temp);
	  strcpy(TempUSER->friends[friends], friend_temp);
    friends++;
	}
 
	fclose(ff);
  ff = fopen("PeterAnteater_requests.txt", "r");
  requests = 0;
  while(fgets(line2, LOGINLEN, ff) != NULL){
    sscanf(line2, "%s \n", friend_temp);
    strcpy(TempUSER->Requests[requests], friend_temp);
    requests++;
  }
	fclose(ff);
  
 	userbase->prev = NULL;
	userbase->user = default_user;

	int curr_line = 0;

  fp = fopen("USERBASE.txt" ,"r");
  
/*	if(fgets(line, LOGINLEN, fp) != NULL){ // Grabs First USER
		sscanf(line, "%s \n", username);
		TempUSER = CreateUSERStruct();
		strcpy(TempUSER->username, username);
		if(fgets(line, LOGINLEN, fp) != NULL){	
			sscanf(line, "%s \n", password);
			strcpy(TempUSER->password, password);
			userbase->user = TempUSER;
		}
	}*/
	while(fgets(line, LOGINLEN, fp) != NULL){ // Grabs rest of USERs
		if ((curr_line % 2) == 0){
			sscanf(line, "%s \n", username);
			TempUSER = CreateUSERStruct();
			strcpy(TempUSER->username, username);		 
			strcpy(filecreate, username);
			strcat(filecreate, "_friends.txt");
			ff = fopen(filecreate, "r");
      friends = 0;
			while(fgets(line2, LOGINLEN, ff) != NULL){
				sscanf(line2, "%s \n", friend_temp);
				strcpy(TempUSER->friends[friends], friend_temp);
				friends++;
			}
			fclose(ff);
			strcpy(filecreate, username);
      strcat(filecreate, "_requests.txt");
      ff = fopen(filecreate, "r");
      requests = 0;
      while(fgets(line2, LOGINLEN, ff) != NULL){
        sscanf(line2, "%s \n", friend_temp);
        strcpy(TempUSER->Requests[requests], friend_temp);
        requests++;
      }
			fclose(ff);
		}
		if ((curr_line % 2) == 1){
			sscanf(line, "%s \n", password);
			strcpy(TempUSER->password, password);
			e = (USERBASE *)malloc(sizeof(USERBASE));
      e->user = TempUSER;
      e->prev = userbase;
			userbase = e;
		}
		curr_line++;
	}
	fclose(fp);
  return userbase;
}

USER *CreateUSERStruct(void){
	int i;
	USER *temp = (USER *)malloc(sizeof(USER));
	strcpy(temp->username, "");
	strcpy(temp->password, "");
  temp->socketFD = 0;
	for(i = 0; i < USERNUM; i++){
		strcpy(temp->friends[i], "");//Sets All Friends to NULL
		strcpy(temp->Requests[i], "");//Sets All Requests to NULL
	}
	return temp;
}

USERBASE *NewUSER(USERBASE *userbase, char username[LOGINLEN], char password[LOGINLEN]){
	USER *User = CreateUSERStruct();
	USERBASE *e =(USERBASE *)malloc(sizeof(USERBASE));
	char filecreate[256];
	FILE *fp = NULL;
 
	strcpy(filecreate, username);
	strcat(filecreate, "_friends.txt");	
	fp = fopen(filecreate, "w");
	fclose(fp);
	strcpy(filecreate, username);
  strcat(filecreate, "_requests.txt");
	fp = fopen(filecreate, "w");
  fclose(fp);
	strcpy(User->username, username);
  strcpy(User->password, password);
  
	e->user = User;
	e->prev = userbase;
	userbase = e;
	fp = fopen("USERBASE.txt", "a");
	fprintf(fp, "%s\n%s\n",username, password);
	fclose(fp);
	return userbase;
}

USER *FindUSER(USERBASE *userbase, char username[LOGINLEN])
{
  USERBASE *temp = userbase;
  while (temp->prev != NULL)
  {
    if (0 == strcmp(temp->user->username, username))
      return temp->user;
    else
    {
      temp = temp->prev;
    }
  }
  return NULL;
}

USER *FindBySocket(USERBASE *userbase, int socketFD)
{
  USERBASE *temp = userbase;
  while (temp->prev != NULL)
  {
    if (socketFD == temp->user->socketFD)
      return temp->user;
    else
    {
      temp = temp->prev;
    }
  }
  return NULL;
}