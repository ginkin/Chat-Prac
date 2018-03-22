#pragma once
#include "Constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

USERBASE *InitializeUserbase(void);

USER *CreateUSERStruct(void);

USERBASE *NewUSER(USERBASE *userbase, char username[LOGINLEN], char password[LOGINLEN]);

USER *FindUSER(USERBASE *userbase, char username[LOGINLEN]);

USER *FindBySocket(USERBASE *userbase, int socketFD);