#include "Friendlist.h"
#include <string.h>
#include <stdio.h>

int i = 0;

//This function is called when you want to delete another user from your friendlist.
//This function must be called twice together to remove friend from user's list and user from friend's list
USER* DeleteUser(USER *user, USER *Friend){  //Deletes "USER Friend" out of "USER user"'s friendlist
	for(i=0; i<USERNUM; i++){
		if(strcmp(user->friends[i], Friend->username)==0){
			strcpy(user->friends[i], "");
			break;
		}
	}
	return user;
}
//This function is called when you want to add another user.
USER *Request(USER *user, USER *Friend){ //Places USER user in USER Friend's Request list.
	for (i=0; i<USERNUM; i++){ 
		if(strcmp(Friend->Requests[i], "") == 0){
			strcpy(Friend->Requests[i], user->username);
			break;
		}
	}
	return Friend;
}
//This function is called when you want to decline a request.
/*USER *Decline(USER *user, USER *Friend){ //Deletes Friend from your Requests list
		for(i=0; i<USERNUM; i++){
			if(strcmp(user->Friends[i], Friend->username)==0){
				strcpy(user->Friends[i], "");
				break;
			}
		}
	return user;
}*/
//This function is called when you accept a request. Must be called twice, for User to add friend, and vice versa
USER *AddUser(USER *user, USER *Friend){ //Adds USER Friend into USER user's friendlist
	for(i=0; i<USERNUM; i++){
		if(strcmp(user->friends[i], "")  == 0){
			strcpy(user->friends[i], Friend->username);
			break;
		}
	}
	return user;
}











