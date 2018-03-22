# Variables
CC     = gcc -c
DEBUG  = -DDEBUG
CFLAGS = -ansi -Wall -c
GUIFLAGS = `pkg-config --cflags --libs gtk+-2.0`

# Default target
all: clean client server

######################### Generate object files #######################
#Friendlist.o: Friendlist.c Friendlist.h Constants.h
	#$(CC) -std=c99 Friendlist.c -o Friendlist.o

########### Generate executables ###########

#target to generate client server gui
client: ChatClientGUI.c
	gcc -w -lpthread ChatClientGUI.c -o client $(GUIFLAGS)  #../../bin/gui
 
server.o: ChatServer.c ChatServer.h ChatServerlist.h Constants.h
	$(CC) ChatServer.c -o server.o #../../bin/server


ChatServerlist.o: ChatServerlist.c ChatServerlist.h Constants.h
	$(CC) ChatServerlist.c -o ChatServerlist.o

Friendlist.o: Friendlist.c Friendlist.h Constants.h
	$(CC) Friendlist.c -o Friendlist.o

server: server.o Friendlist.o ChatServerlist.o
	gcc server.o Friendlist.o ChatServerlist.o -o server
#target to clean the directory
clean:
	rm -f *.o client server gui
 
 
