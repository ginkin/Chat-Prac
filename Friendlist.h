#include "Constants.h"
#include <stdio.h>
#include <stdlib.h>




USER *DeleteUser(USER *user, USER *Friend); //Deletes a Friend from a USER's friendslist. Must be called twice (one for your list and one for theirs) 

USER *Request(USER *user, USER *Friend); //Places your user name in their Requests array

USER* Decline(USER *user, USER *Friend); //Decline a USER's request to add, and deletes their user name from your Friendslist.

USER* AddUser(USER *user, USER *Friend); // Adds USER Friend into USER user's friendlist. (Call twice)
