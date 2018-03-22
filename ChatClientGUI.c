/* ClockClient.c: simple interactive TCP/IP client for ClockServer
 * Author: Rainer Doemer, 2/16/15 (updated 2/20/17)
 */


#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include "ChatServer.h"
#include "Constants.h"
#include <errno.h>
#define MSGLEN 1025
#define StringLen 256

void *Recv_Msg(void *socket);
void show_text(const gchar* text);
void show_chat_window();
GtkWidget *create_friend_list_window();
void delete_event_friend();

//GtkWidget *signup_window = NULL;
GtkWidget *login_window = NULL;
GtkWidget *window_signup = NULL;
GtkWidget *chat_window = NULL;
GtkWidget *friend_list_window = NULL;
GtkWidget *invitation_window = NULL;
GtkWidget *error_window = NULL;
GtkWidget *friendlist_window[100];
GtkWidget *friendlist_window2;
int fwindow; 
GtkWidget *button[100];
GtkWidget *label_name;
GtkWidget *errorlabel;

GtkTextBuffer *send_buffer, *receive_buffer;
GtkWidget *receive_text, *send_text;

GtkWidget *window,
*Send_scrolled_win,
*Rcv_scrolled_win,
*Send_textview,
*Rcv_textview;
GtkTextBuffer *Send_buffer, *Rcv_buffer;
GtkWidget *vbox, *hbox;
GtkWidget *SaveButton, *CancelButton;
GtkWidget *Label1, *Label2, *space_label;

/* #define DEBUG */	/* be verbose */

/*********************Global variables for GUI  ****************************************************/
extern GtkWidget *window_signup;
extern GtkWidget *chat_window;
extern GtkWidget *friend_list_window;
extern GtkWidget *invitation_window;
extern GtkWidget *error_window;
extern GtkWidget *friendlist_window[100];
extern GtkWidget *friendlist_window2;
//GUI texts
const gchar *username_text;
const gchar *password_text;
const gchar *username_textsignup;
const gchar *password_textsignup;
const gchar *username_text_add_friend;
const gchar *username_text_delete_friend;
const gchar *username_request;
char loginusername[LOGINLEN],loginpassword[LOGINLEN];

gchar *chatsendmessage;
/************** Global variables for Client functions ***********************/
char LoginBuf[256];
const char *Program	= NULL;
int login; //To indicate whether the user has logged in
int signup; //To indicate whether the user has signed up
int socket_close = 0;
///
int l, n;
int SocketFD,SocketFD2,	/* socket file descriptor */
	PortNo;		/* port number */
struct sockaddr_in
	ServerAddress;	/* server address we connect with */
struct hostent
	*Server;	/* server host */
char SendBuf[256];	/* message buffer for sending a message */
char RecvBuf[256];	/* message buffer for receiving a response */
char RecvMsg[256];
char match_user_name[256],user_name[256];

char addusername[LOGINLEN];
char deleteusername[LOGINLEN];
char friendstatuslist[100][StringLen]; 
int processed;
char friendname[StringLen];
char status[StringLen];
char friendstatus[StringLen];
char printtext[StringLen];
char friendmessage[StringLen];
char requestfriend[StringLen];
/********************Global Client functions ****************************************************/
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

void Login_Process()// generate a login command line
{
    strcpy(SendBuf, "LOGIN-");
    strcat(SendBuf, username_text);
    strcat(SendBuf, "-");    
    strcat(SendBuf, password_text);
    printf("\nSendBuf: %s\n", SendBuf);
}
void Register_Process()// generate a register command line
{
    strcpy(SendBuf, "REGISTER-");
    strcat(SendBuf, username_textsignup);
    strcat(SendBuf, "-");    
    strcat(SendBuf, password_textsignup);
    printf("\nSendBuf: %s\n", SendBuf);
}
void Chat_Process()// generate a chat command line
{
    strcpy(SendBuf, "SEND-");
    strcat(SendBuf, match_user_name);
    strcat(SendBuf, "-");  
    strcat(SendBuf, chatsendmessage);
    strcat(SendBuf, " "); 
    printf("\nSendBuf: %s\n", SendBuf);
}
void AddFriend_Process()// generate a chat command line
{
	strcpy(SendBuf, "ADD-");
	strcat(SendBuf, username_text_add_friend);
	strcat(SendBuf, "-");
	strcat(SendBuf, "message");
	printf("\nSendBuf: %s\n", SendBuf);
}
void DeleteFriend_Process()// generate a chat command line
{
	strcpy(SendBuf, "DELETE-");
	strcat(SendBuf, username_text_delete_friend);
	strcat(SendBuf, "-");
	strcat(SendBuf, "message");
	printf("\nSendBuf: %s\n", SendBuf);
}
void AcceptFriend_Process()// generate a chat command line
{
	strcpy(SendBuf, "ACCEPT-");
	strcat(SendBuf, requestfriend);
	strcat(SendBuf, "-");
	strcat(SendBuf, "message");
	printf("\nSendBuf: %s\n", SendBuf);
}
void RejectFriend_Process()// generate a chat command line
{
	strcpy(SendBuf, "REJECT-");
	strcat(SendBuf, requestfriend);
	strcat(SendBuf, "-");
	strcat(SendBuf, "message");
	printf("\nSendBuf: %s\n", SendBuf);
}
void LOGOUT_Process()// generate a chat command line
{
	strcpy(SendBuf, "CLOSE-");
	strcat(SendBuf, loginusername);
	strcat(SendBuf, "-");
	strcat(SendBuf, "message");
	printf("\nSendBuf: %s\n", SendBuf);
	socket_close = 1;
  Send_Msg();
  delete_event_friend();
}
void InitialNameArray(char array[][StringLen], int len) {
	int m;
	for (m = 0; m < len; m++) {
		strcpy(array[m], "None");
	}
}
// copy input string to the first "None" string in the array
void NameArrayAppend(char array[][StringLen], char string[], int len) {
	int m;
	for (m = 0; m < len; m++) {
		if (strcmp(array[m], "None") == 0) {
			strcpy(array[m], string);
			break;
		}
	}
}
// delete input string in the array and shift the rest elements to left
void NameArrayDelete(char array[][StringLen], char string[], int len) {
	int m, n;
	int shift;
	char newarray[100][StringLen];
	InitialNameArray(newarray, len);
	for (m = 0; m < len; m++) {
		if (strcmp(array[m], string) == 0) {
			strcpy(array[m], "None");
			shift = m;
			break;
		}
	}
	//shift to left
	for (m = 0; m < len; m++) {
		if (strcmp(array[m], "None") != 0) {
			NameArrayAppend(newarray, array[m], len);
		}
	}
	for (m = 0; m < len; m++) {
		strcpy(array[m], newarray[m]);
	}
}
// Check how many strings in the array are not "None" - see how many friends in the list
int GetFriendLen(char array[][StringLen], int len) {
	int m;
	int count = 0;
	for (m = 0; m < len; m++) {
		if (strcmp(array[m], "None") != 0) {
			count++;
		}
	}
	return count;
}
void RecvComp()
{
  close(SocketFD2);

  if(strncmp(RecvBuf, "LOGIN", 5) == 0){
    printf("\nmark\n");
    StringToken(RecvBuf, loginusername, loginpassword);
    SocketFD = socket(AF_INET, SOCK_STREAM, 0);
    printf("\nFD:%d\n",SocketFD);
    if (connect(SocketFD, (struct sockaddr*)&ServerAddress,sizeof(ServerAddress)) < 0)
    {   
      FatalError("connecting to server failed");
    }
    socket_close = 0;
    char temp[256];
 	  strncpy(temp,"WRITING-",9);
    strcat(temp, loginusername);
    strcat(temp, "-");
    strcat(temp, loginpassword);
    printf("\n%s\n",temp);
 	  n = send(SocketFD, temp, sizeof(temp)-1,0);	
	  n = send(SocketFD, temp, sizeof(temp)-1,0);
 	  n = recv(SocketFD, temp, sizeof(temp)-1,0);
    printf("\n%s\n",temp);
   	pthread_t t1;
    pthread_create(&t1,NULL,Recv_Msg,&SocketFD);
    show_friend_list_window();
    gtk_label_set_text(errorlabel,"Logged in!");
  }
  else if(strcmp(RecvBuf, "New account registered successfully!") == 0){
    signup = 1;// change int signup to 1 to indicate the user has registered a new account
  }	//generate a friendstatus string and append it to the list
  else if (0 == strncmp(RecvBuf, "ACCEPT", 6)) {
		StringToken(RecvBuf, friendname, status);
		strcpy(friendstatus, friendname);
		strcat(friendstatus, "-");
		strcat(friendstatus, "ONLINE");
		NameArrayAppend(friendstatuslist, friendstatus, 100);
    printf("\nmark\n");
    updatefriendlistwindow();
	}
}

void updatefriendlistwindow(){
    friendlist_window[fwindow] = NULL;
    friendlist_window[fwindow] = create_friend_list_window();
    if(fwindow == 0){
        gtk_widget_hide_all(friend_list_window);
    }else{
        gtk_widget_hide_all(friendlist_window[fwindow-1]);
    }
    gtk_widget_show_all(friendlist_window[fwindow]);
    gtk_widget_show_all(friendlist_window2);
    fwindow++;
}

void ProcessServerRequest(){
	// Server to Client: ADD-user-status Delete-user-status
	// Client adds it to the friendstatus list or delete it from friendstatus list
	strcpy(friendname, "");
	strcpy(status, "");
	strcpy(friendstatus, "");
	strcpy(printtext, "");
  strcpy(friendmessage, "");
  strcpy(requestfriend, "");
  
 if (0 == strncmp(RecvMsg, "ADD", 3)) {
		StringToken(RecvMsg, friendname, status);
		strcpy(friendstatus, friendname);
		strcat(friendstatus, "-");
		strcat(friendstatus, "ONLINE");
		NameArrayAppend(friendstatuslist, friendstatus, 100);
    updatefriendlistwindow();
	}
	else if (0 == strncmp(RecvMsg, "DELETE", 6)) {
		StringToken(RecvMsg, friendname, status);
		strcpy(friendstatus, friendname);
		strcat(friendstatus, "-");
		strcat(friendstatus, status);
		NameArrayDelete(friendstatuslist, friendstatus, 100);
    updatefriendlistwindow();
	}
  
	//Server to Client: FRIEND-user-status 
	//Client adds it to the friendstatus list 
	else if (0 == strncmp(RecvMsg, "FRIEND", 6)) {
		StringToken(RecvMsg, friendname, status);
		strcpy(friendstatus, friendname);
		strcat(friendstatus, "-");
		strcat(friendstatus, status);
		NameArrayAppend(friendstatuslist, friendstatus, 100);
    updatefriendlistwindow();
	}
  //Server sends a friend invitation to a client: REQUEST-fromuser-status
  else if (0 == strncmp(RecvMsg, "REQUEST", 7)) {
		StringToken(RecvMsg, requestfriend, status);
    gtk_label_set_text(label_name,requestfriend);
    show_invitation_window();
	}
	//generate a text of received message to print in GUI
	else if (0 == strncmp(RecvMsg, "SEND", 4)) {
   
		strcpy(friendmessage,"");
    StringToken(RecvMsg, friendname, friendmessage);
    printf("friendname: %s\n", friendname);
    printf("message: %s\n", friendmessage);
		strcpy(printtext, friendname);
		strcat(printtext, ": \n");
		strcat(printtext, friendmessage);
    printf("printtext: %s\n", printtext);

     /*
    int i;
    strncpy(printtext,RecvMsg,StringLen);
    printf("printtext: %s\n", printtext);
    for (i=0; i < StringLen-5; i++){
      printtext[i] = printtext[i + 5];
    }
    for (i=0; i < StringLen; i++){
      if(printtext[i] == "-"){
         printtext[i] = ":";
      }
    }
    */
		show_recv(printtext);
	} 
}

void *Recv_Msg(void *socket)
{
  int _Socket = *((int *)socket);
  int n;
  strcpy(RecvMsg,"");
  while (socket_close == 0)
  {
    //printf("\n%d\n",SocketFD);
    n = read(_Socket,RecvMsg,sizeof(RecvMsg)-1);
    RecvMsg[strlen(RecvMsg)] = 0;
    printf("\n%d\n",n);
    //if (n == -1) printf("\n%s\n",strerror(errno));
    if (n != -1) {
    printf("\n%d,RecvMsg: %s\n",n,RecvMsg);
    ProcessServerRequest();
    strcpy(RecvMsg,"\0");
    }
  }
}

void Send_Msg()//Send and Recv messages
{
    	//printf("%s: Enter a command to send to the clock server:\n"
  		//"command: ", argv[0]);
    	//fgets(SendBuf, sizeof(SendBuf), stdin);
     SocketFD2 = socket(AF_INET, SOCK_STREAM, 0);
     printf("\nFD:%d\n",SocketFD);
    	    if (SocketFD2 < 0)
    	    {   FatalError("socket creation failed");
    	    }
    	    printf("%s: Connecting to the server at port %d...\n",
    			Program, PortNo);
    	    if (connect(SocketFD2, (struct sockaddr*)&ServerAddress,
    			sizeof(ServerAddress)) < 0)
    	    {   FatalError("connecting to server failed");
    	    }
          char temp[256];
 	        strncpy(temp,"WRITING-",9);
          strcat(temp, loginusername);
          strcat(temp, "-");
          strcat(temp, loginpassword);
       	  n = send(SocketFD2, temp, sizeof(temp)-1,0);	
       	  //n = recv(SocketFD2, temp, sizeof(temp)-1,0);  

    	    printf("%s: Sending message '%s'...\n", Program, SendBuf);
    	    n = send(SocketFD2, SendBuf, sizeof(SendBuf)-1,0);
    	    if (n < 0)
    	    {   FatalError("writing to socket failed");
    	    }
    #ifdef DEBUG
    	    printf("%s: Waiting for response...\n", Program);
    #endif
    	    n = recv(SocketFD2, RecvBuf, sizeof(RecvBuf)-1,0);
    	    if (n < 0) 
    	    {   FatalError("reading from socket failed");
    	    }
    	    RecvBuf[n] = 0;
    	    printf("%s: Received response: %s\n", Program, RecvBuf);
    #ifdef DEBUG
    	    printf("%s: Closing the connection...\n", Program);
    #endif
          RecvComp();
          //if (login){
            //close(SocketFD2);
            
          //}
          //else
    //	      close(SocketFD2);
    printf("%s: Exiting...\n", Program);
}
/*********************Global functions for GUI  ****************************************************/

//// Window operations/////
void show_friend_list_window()
{
	gtk_widget_show_all(friend_list_window);
}
static void show_signup_window()
{
	gtk_widget_show_all(window_signup);
}

void show_chat_window()
{
	gtk_widget_show_all(chat_window);
}
void show_invitation_window()
{
  gtk_widget_show_all(invitation_window);
}

void show_error_window()
{
  gtk_widget_show_all(error_window);
}
//////Hide windows
static void delete_event()
{
	gtk_widget_hide_all(window_signup);
}

static void delete_event_chat()
{
	gtk_widget_hide_all(chat_window);
}

void delete_event_friend()
{

  gtk_widget_hide_all(friend_list_window);
}

static void delete_event_invitation()
{
  gtk_widget_hide_all(invitation_window);
}

static void delete_event_error()
{
  gtk_widget_hide_all(error_window);
}
//// Get texts from Window
static void name_info_input( GtkWidget *widget,
                            GtkWidget *entry )
{
  username_text = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("Entry contents: %s\n", username_text);
}


static void password_info_input( GtkWidget *widget,
                            GtkWidget *entry )
{
  password_text = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("Entry contents: %s\n", password_text);
}

static void name_info_inputsignup( GtkWidget *widget,
                            GtkWidget *entry )
{
  username_textsignup = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("Entry contents: %s\n", username_textsignup);
}


static void password_info_inputsignup( GtkWidget *widget,
                            GtkWidget *entry )
{
  password_textsignup = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("Entry contents: %s\n", password_textsignup);
}

void add_user(GtkWidget *widget,
	GtkWidget *entry)
{
	username_text_add_friend = gtk_entry_get_text(GTK_ENTRY(entry));
	printf("User added: %s\n", username_text_add_friend);
}

void delete_user(GtkWidget *widget,
	GtkWidget *entry)
{
	username_text_delete_friend = gtk_entry_get_text(GTK_ENTRY(entry));
	printf("User deleted: %s\n", username_text_delete_friend);
}

//accept and reject functions
static void accept_user_and_then_delete_on_window(GtkWidget *widget,
                            GtkWidget *label)
{
  AcceptFriend_Process();
  username_request = gtk_label_get_text (label_name);
  printf("User accepted: %s\n", username_request);
  //gtk_label_set_text(label_name,"       ");
  gtk_widget_hide_all(invitation_window);
  //gtk_widget_show_all(invitation_window);
}

static void reject_user_and_then_delete_on_window(GtkWidget *widget,
                            GtkWidget *label)
{
  RejectFriend_Process();
  //gtk_label_set_text(label_name,"      ");
  gtk_widget_hide_all(invitation_window);
  //gtk_widget_show_all(invitation_window);
}
// DELETE BUTTON
void delete_button()
{
  int i;
  for (i=0; i<GetFriendLen(friendstatuslist, 100);i++)
  {
  gtk_widget_hide(button[i]);
  }
  gtk_widget_show_all(friend_list_window);
}

// send text operations
void clean_send_text()
{
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(Send_buffer), &start, &end);
	gtk_text_buffer_delete(GTK_TEXT_BUFFER(Send_buffer), &start, &end);
}

void on_send()//(GtkButton *SaveButton, GtkWidget *Send_textview)
{

	GtkTextIter start, end;
	gchar *text;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(Send_buffer), &start, &end);
	text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(Send_buffer), &start, &end, FALSE);
  chatsendmessage = text;
	show_text(text);
}
void show_text(const gchar* text)
{
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(Rcv_buffer), &start, &end);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(Rcv_buffer), &end, "You said:\n", 10);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(Rcv_buffer), &end, text, strlen(text));
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(Rcv_buffer), &end, "\n", 1);
}
void show_recv(const gchar* text)
{
  printf("\n In Show.\n");
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(Rcv_buffer), &start, &end);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(Rcv_buffer), &end, text, strlen(text));
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(Rcv_buffer), &end, "\n", 1);
  printf("\n Finish Show.\n");
}
// Windows
GtkWidget *create_error_window()
{
  GtkWidget *error_window, *label, *button, *hbox;
  error_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  errorlabel = gtk_label_new("Click on any name button to start chat: \n You need to send the message in the following format: SEND-username-message:");
  button = gtk_button_new_with_label("OK");
  g_signal_connect(button,"clicked",G_CALLBACK(delete_event_error),NULL);
  g_signal_connect(error_window,"delete_event",G_CALLBACK(delete_event_error),NULL);
  hbox = gtk_hbox_new(0,2);
  gtk_box_pack_start(hbox,errorlabel,1,1,2);
  gtk_box_pack_start(hbox,button,0,0,2);
  gtk_container_add(error_window,hbox);
  return error_window;
}

GtkWidget *create_signup_window()
{
	GtkWidget *window_signup;
	GtkWidget *button1, *button2;
	GtkWidget *label1, *label2;
	GtkWidget *hbox1, *hbox2, *hbox3;
	GtkWidget *vbox;
	GtkWidget *Username_entry, *Password_entry;
	window_signup = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window_signup), "CHAT APP");
	//gtk_widget_set_size_request(GTK_WINDOW(window_signup),800,600);

	label1 = gtk_label_new("Username");
	label2 = gtk_label_new("Password");

	button1 = gtk_button_new_with_label("Sumbit");
	button2 = gtk_button_new_with_label("Back");

	Username_entry = gtk_entry_new();
	Password_entry = gtk_entry_new();

	g_signal_connect(button2, "clicked", G_CALLBACK(gtk_main_quit), button2);
	g_signal_connect(button1, "clicked", G_CALLBACK(name_info_inputsignup), Username_entry);
	g_signal_connect(button1, "clicked", G_CALLBACK(password_info_inputsignup), Password_entry);

	hbox1 = gtk_hbox_new(0, 0);
	hbox2 = gtk_hbox_new(0, 0);
	hbox3 = gtk_hbox_new(0, 0);
	vbox = gtk_vbox_new(0, 0);

	gtk_box_pack_start(GTK_BOX(hbox1), label1, 0, 0, 100);
	gtk_box_pack_start(GTK_BOX(hbox1), Username_entry, 0, 0, 0);

	gtk_box_pack_start(GTK_BOX(hbox2), label2, 0, 0, 100);
	gtk_box_pack_start(GTK_BOX(hbox2), Password_entry, 0, 0, 0);

	gtk_box_pack_start(GTK_BOX(hbox3), button1, 0, 0, 100);
	gtk_box_pack_start(GTK_BOX(hbox3), button2, 0, 0, 0);

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(hbox1), 0, 0, 10);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(hbox2), 0, 0, 10);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(hbox3), 0, 0, 10);

	gtk_container_add(GTK_CONTAINER(window_signup), vbox);

	g_signal_connect(button1, "clicked", G_CALLBACK(delete_event), NULL);
	g_signal_connect(button2, "clicked", G_CALLBACK(delete_event), NULL);
 	g_signal_connect(button1,"clicked",G_CALLBACK(Register_Process),NULL);
 	g_signal_connect(button1,"clicked",G_CALLBACK(Send_Msg),NULL);
	g_signal_connect(window_signup, "delete_event", G_CALLBACK(delete_event), NULL);
	return window_signup;
}

GtkWidget *create_login_window()
{	
	GtkWidget *window, *button1, *button2, *button3, *label1, *label2, *label3, *hbox1, *hbox2, *hbox3, *vbox;
	GtkWidget *Username_entry, *Password_entry;	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	button2 = gtk_button_new_with_label("Cancel");
	button1 = gtk_button_new_with_mnemonic("Login");
	button3 = gtk_button_new_with_label("Sign up");
	gtk_widget_set_size_request(window,800,600);
	gtk_window_set_title(GTK_WINDOW(window), "CHAT APP");	
	
	label1 = gtk_label_new("User Name");
	label2 = gtk_label_new("Password");
	Username_entry = gtk_entry_new();
	Password_entry = gtk_entry_new();
	gtk_entry_set_visibility (GTK_ENTRY (Password_entry),FALSE);
	g_signal_connect(button2,"clicked",G_CALLBACK(gtk_main_quit),button2);
	g_signal_connect(button1,"clicked",G_CALLBACK(name_info_input),Username_entry);
	g_signal_connect(button1,"clicked",G_CALLBACK(password_info_input),Password_entry);
	hbox1 = gtk_hbox_new(0,0);
	hbox2 = gtk_hbox_new(0,0);
	hbox3 = gtk_hbox_new(0,0);
	vbox = gtk_vbox_new(0,0);
	
	gtk_box_pack_start(GTK_BOX(hbox1),label1,0,0,100);
	gtk_box_pack_start(GTK_BOX(hbox1),Username_entry,0,0,0);
        
	gtk_box_pack_start(GTK_BOX(hbox2),label2,0,0,100);
        gtk_box_pack_start(GTK_BOX(hbox2),Password_entry,0,0,0);
	
	gtk_box_pack_start(GTK_BOX(hbox3),button1,0,0,100);
	gtk_box_pack_start(GTK_BOX(hbox3),button2,0,0,0);
	gtk_box_pack_start(GTK_BOX(hbox3),button3,0,0,100);
        gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbox1),0,0,10);
        gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbox2),0,0,10);


	gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbox3),0,0,10);
	gtk_container_add(GTK_CONTAINER(window),vbox);

	g_signal_connect(window, "delete_event",G_CALLBACK(gtk_main_quit), NULL);  
	g_signal_connect(button1,"clicked",G_CALLBACK(Login_Process),NULL);
 	g_signal_connect(button1,"clicked",G_CALLBACK(Send_Msg),NULL);
 	//g_signal_connect(button1,"clicked",G_CALLBACK(show_invitation_window),NULL);
  //gtk_label_set_text(label_name,requestfriend);
 	g_signal_connect(button1,"clicked",G_CALLBACK(show_error_window),NULL);
  //if(login==1){show_chat_window();}
 // g_signal_connect(button1,"clicked",G_CALLBACK(show_chat_window),NULL);
	g_signal_connect(button3,"clicked",G_CALLBACK(show_signup_window),NULL);
	return window;

}

void connect_user_name()
{ 
  strcpy(match_user_name, user_name);
}
GtkWidget *create_friend_list_window()
{
	GtkWidget *friend_list_window;
	GtkWidget *table;
 
	GtkWidget *Add_button, *Delete_button;
	GtkWidget *entry;
	GtkWidget *hbox, *vbox;
	int i, j;
	friend_list_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(friend_list_window,800,600);
	table = gtk_table_new(2, 2, 0);
	for (i = 0; i<GetFriendLen(friendstatuslist, 100); i++) {
		button[i] = gtk_button_new_with_label(friendstatuslist[i]);
		gtk_table_attach_defaults(table, button[i], 0, 1, i, i + 1);
    strcpy(user_name,strtok(friendstatuslist[i],"-"));
    g_signal_connect(button[i], "clicked", G_CALLBACK(connect_user_name), NULL);
		g_signal_connect(button[i], "clicked", G_CALLBACK(show_chat_window), NULL);
	}
	entry = gtk_entry_new();
	gtk_entry_set_text(entry, "Enter the Username");
	Add_button = gtk_button_new_with_label("Add");
	g_signal_connect(Add_button, "clicked", G_CALLBACK(add_user), entry); // Get username text
	g_signal_connect(Add_button, "clicked", G_CALLBACK(AddFriend_Process), NULL); // generate a ADD-User-message command line
	g_signal_connect(Add_button, "clicked", G_CALLBACK(Send_Msg), NULL); // Sendbuf
	Delete_button = gtk_button_new_with_label("Delete");
	g_signal_connect(Delete_button, "clicked", G_CALLBACK(delete_user), entry);
	g_signal_connect(Delete_button, "clicked", G_CALLBACK(DeleteFriend_Process), NULL); // generate a DELETE-User-message command line
	g_signal_connect(Delete_button, "clicked", G_CALLBACK(Send_Msg), NULL); // Sendbuf
	hbox = gtk_hbox_new(0, 2);
	gtk_box_pack_start(hbox, Add_button, 1, 1, 2);
	gtk_box_pack_start(hbox, Delete_button, 0, 0, 2);
	vbox = gtk_vbox_new(0, 2);
	gtk_box_pack_start(vbox, table, 1, 1, 2);
	gtk_box_pack_start(vbox, entry, 0, 0, 2);
	gtk_box_pack_start(vbox, hbox, 0, 0, 2);
  g_signal_connect(friend_list_window,"delete_event",G_CALLBACK(LOGOUT_Process),NULL);
  //g_signal_connect(friend_list_window,"delete_event",G_CALLBACK(Send_Msg),NULL);
  //g_signal_connect(friend_list_window,"delete_event",G_CALLBACK(delete_event_friend),NULL);
	gtk_container_add(friend_list_window, vbox);
	return friend_list_window;
}

GtkWidget *create_invitation_window()
{
  GtkWidget *invitation_window,
  *label1, *label3,
  *Accept_button, *Reject_button,
  *hbox1, *hbox2, *vbox;
  invitation_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  label1 = gtk_label_new("Friend Request");
    /* "Jack" should be a friend who send the request*/
  label_name = gtk_label_new("Somebody");
  label3 = gtk_label_new(" wants to add you as friend");
  Accept_button = gtk_button_new_with_label("Accept");
  Reject_button = gtk_button_new_with_label("Reject");
  hbox1 = gtk_hbox_new(0,2);
  hbox2 = gtk_hbox_new(0,2);
  vbox = gtk_vbox_new(0,2);
  gtk_box_pack_start(hbox1,label_name,1,1,2);
  gtk_box_pack_start(hbox1,label3,0,0,2);
  gtk_box_pack_start(hbox2,Accept_button,1,1,2);
  gtk_box_pack_start(hbox2,Reject_button,0,0,2);
  gtk_box_pack_start(vbox,label1,1,1,2);
  gtk_box_pack_start(vbox,hbox1,0,0,2);
  gtk_box_pack_start(vbox,hbox2,0,0,2);
  g_signal_connect(Accept_button,"clicked",G_CALLBACK(accept_user_and_then_delete_on_window),label_name);
  g_signal_connect(Accept_button,"clicked",G_CALLBACK(Send_Msg),NULL);
  g_signal_connect(Reject_button,"clicked",G_CALLBACK(reject_user_and_then_delete_on_window),label_name);
  g_signal_connect(Reject_button,"clicked",G_CALLBACK(Send_Msg),NULL);
  g_signal_connect(invitation_window,"delete_event",G_CALLBACK(delete_event_invitation),NULL);
  gtk_container_add(invitation_window, vbox);
  return invitation_window; 
}

GtkWidget *create_chat_window()
{

	chat_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(chat_window,800,600);
	gtk_container_set_border_width(chat_window, 50);
	Send_textview = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(Send_textview), GTK_WRAP_WORD);
	gtk_text_view_set_justification(GTK_TEXT_VIEW(Send_textview), GTK_JUSTIFY_LEFT);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(Send_textview), TRUE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(Send_textview), TRUE);
	gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(Send_textview), 10);
	gtk_text_view_set_pixels_below_lines(GTK_TEXT_VIEW(Send_textview), 10);
	gtk_text_view_set_pixels_inside_wrap(GTK_TEXT_VIEW(Send_textview), 10);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(Send_textview), 10);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(Send_textview), 10);
	Send_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Send_textview));
	Rcv_textview = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(Rcv_textview), GTK_WRAP_WORD);
	gtk_text_view_set_justification(GTK_TEXT_VIEW(Rcv_textview), GTK_JUSTIFY_LEFT);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(Rcv_textview), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(Rcv_textview), TRUE);
	gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(Rcv_textview), 10);
	gtk_text_view_set_pixels_below_lines(GTK_TEXT_VIEW(Rcv_textview), 10);
	gtk_text_view_set_pixels_inside_wrap(GTK_TEXT_VIEW(Rcv_textview), 10);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(Rcv_textview), 10);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(Rcv_textview), 10);
	Rcv_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Rcv_textview));
	Send_scrolled_win = gtk_scrolled_window_new(NULL, NULL);
	Rcv_scrolled_win = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(Send_scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(Rcv_scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_add_with_viewport(Rcv_scrolled_win, Rcv_textview);
	gtk_scrolled_window_add_with_viewport(Send_scrolled_win, Send_textview);
	hbox = gtk_hbox_new(0, 0);
	vbox = gtk_vbox_new(FALSE, 0);
	SaveButton = gtk_button_new_with_label("send");
	CancelButton = gtk_button_new_with_label("cancel");
	space_label = gtk_label_new("                         ");
	g_signal_connect(G_OBJECT(SaveButton), "clicked", G_CALLBACK(on_send), (gpointer)Send_textview);
	g_signal_connect(G_OBJECT(SaveButton), "clicked", G_CALLBACK(clean_send_text), NULL);
	g_signal_connect(G_OBJECT(CancelButton), "clicked", G_CALLBACK(delete_event_chat), NULL);
 	g_signal_connect(G_OBJECT(SaveButton),"clicked",G_CALLBACK(Chat_Process),NULL);
 	g_signal_connect(G_OBJECT(SaveButton),"clicked",G_CALLBACK(Send_Msg),NULL);
	gtk_box_pack_start(hbox, space_label, 1, 1, 2);
	gtk_box_pack_start(hbox, SaveButton, 0, 0, 2);
	gtk_box_pack_start(hbox, CancelButton, 0, 0, 2);
	gtk_box_pack_start(vbox, Rcv_scrolled_win, 1, 1, 2);
	gtk_box_pack_start(vbox, Send_scrolled_win, 0, 0, 2);
	gtk_box_pack_start(vbox, hbox, 0, 0, 2);
	gtk_container_add(chat_window, vbox);
	return chat_window;
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

int main(int argc, char *argv[])
{
    login = 0;
    Program = argv[0];	/* publish program name (for diagnostics) */
    fwindow = 0;
    //
   	InitialNameArray(friendstatuslist, 100);
    //Testing friendstatuslist
   	printf("\n string1: %s", friendstatuslist[0]);
	  printf("\n string2: %s", friendstatuslist[1]);
	  printf("\n string3: %s", friendstatuslist[2]);
	  NameArrayAppend(friendstatuslist, "PeterAnteater - Online", 100);
	  NameArrayAppend(friendstatuslist, "B - Offline", 100);
	  NameArrayAppend(friendstatuslist, "DefaultC - Online", 100);
	  NameArrayDelete(friendstatuslist, "B - Offline", 100);
	  printf("\n string1: %s", friendstatuslist[0]);
	  printf("\n string2: %s", friendstatuslist[1]);
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
    
    strcpy(loginusername," ");
    strcpy(loginpassword," ");    

    
    /**********GUI***********/
        GtkWidget *login_window;
        gtk_init(&argc, &argv);
        login_window = create_login_window();
        window_signup = create_signup_window();
      	chat_window = create_chat_window();
       	friend_list_window = create_friend_list_window();

        invitation_window = create_invitation_window();
        error_window = create_error_window();
        gtk_widget_show_all(login_window);
        gtk_main();
    /*************************/
    
	/*printf("\n\n**********************************************************************\nWelcome to the Chat program. \nTo Login, use command: \n"
		"LOGIN-username-password\n\nTo Register with the server, use command: \n"
		"REGISTER-username-password\n\nFor this version two preset usernames and passwords \n"
		"are available: \'LOGIN-Ray-Ray\' and \'LOGIN-Micky-Micky\'.\n\nEnter SHUTDOWN to quit server and client\nEnter bye to disconnect" 
		"from server\n\nHAPPY CHATTING!!!\n**************************************************************************\n\n");
    /******************************/
    
    return 0;
}

/* EOF ClockClient.c */
