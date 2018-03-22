#include"gui.h"
#include<gtk/gtk.h>
#include<stdlib.h>
#include<stdio.h>

//GtkWidget *signup_window = NULL;
GtkWidget *login_window = NULL;
GtkWidget *window_signup = NULL;
GtkWidget *chat_window = NULL;

GtkTextBuffer *send_buffer, *receive_buffer;
GtkWidget *receive_text, *send_text;

GtkWidget *window/*?????*/,
*Send_scrolled_win/*????????*/,
*Rcv_scrolled_win/*????????*/,
*Send_textview/*???????*/,
*Rcv_textview/*???????*/;
GtkTextBuffer *Send_buffer/*?????????*/, *Rcv_buffer/*?????????*/;
GtkWidget *vbox, *hbox/*??????*/;
GtkWidget *SaveButton, *CancelButton/*??????*/;
GtkWidget *Label1, *Label2, *space_label;

extern GtkWidget *window_signup;
extern GtkWidget *chat_window;

static void show_signup_window()
{
	gtk_widget_show_all(window_signup);
}

static void show_chat_window()
{
	gtk_widget_show_all(chat_window);
}

static void name_info_input( GtkWidget *widget,
                            GtkWidget *entry )
{
  const gchar *username_text;
  username_text = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("Entry contents: %s\n", username_text);
}


static void password_info_signup( GtkWidget *widget,
                            GtkWidget *entry )
{
  const gchar *password_text_signup;
  password_text_signup = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("Entry contents: %s\n", password_text_signup);
}


static void name_info_signup( GtkWidget *widget,
                            GtkWidget *entry )
{
  const gchar *username_text_signup;
  username_text_signup = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("Entry contents: %s\n", username_text_signup);
}


static void password_info_input( GtkWidget *widget,
                            GtkWidget *entry )
{
  const gchar *password_text;
  password_text = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("Entry contents: %s\n", password_text);
}




static void delete_event()
{
	gtk_widget_hide_all(window_signup);
}

static void delete_event_chat()
{
	gtk_widget_hide_all(chat_window);
}

void clean_send_text()
{
	GtkTextIter start,end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(Send_buffer),&start,&end);
	gtk_text_buffer_delete(GTK_TEXT_BUFFER(Send_buffer),&start,&end);
}

void on_send()//(GtkButton *SaveButton, GtkWidget *Send_textview)
{

  	GtkTextIter start, end;
   gchar *text;
	//gchar *S_text, *R_text;

	//S_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Send_textview));
	//gtk_text_buffer_get_start_iter(Send_buffer, &start);
	//gtk_text_buffer_get_end_iter(Send_buffer, &end);
// gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(Send_buffer),&start,&end);
 // S_text = gtk_text_buffer_get_text(Send_buffer, &start, &end, FALSE);
  //ZZshow_text(Send_textview);
//	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(Send_buffer),&start,&end);
//	S_text = gtk_text_buffer_get_text(Send_buffer, &start, &end, FALSE);
	//gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(Send_buffer),&start,&end);
	//g_print("%s\n", S_text);

	//R_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Rcv_textview));
	//R_text = S_text;
	//gtk_text_buffer_set_text(Rcv_buffer, S_text, -1);
   //free(Send_textview);
   
   gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(Send_buffer),&start,&end);
	 text=gtk_text_buffer_get_text(GTK_TEXT_BUFFER(Send_buffer),&start,&end,FALSE);
   show_text(text);
}


void show_text(const gchar* text)
{
  GtkTextIter start, end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(Rcv_buffer),&start,&end);
 	gtk_text_buffer_insert(GTK_TEXT_BUFFER(Rcv_buffer),&end,"You said:\n",10);
 	//gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(Rcv_buffer),&start,&end);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(Rcv_buffer),&end,text,strlen(text));
 	//gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(Rcv_buffer),&start,&end);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(Rcv_buffer),&end,"\n",1);
}

GtkWidget *create_chat_window()
{

	chat_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
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
  gtk_scrolled_window_add_with_viewport(Rcv_scrolled_win,Rcv_textview);
  gtk_scrolled_window_add_with_viewport(Send_scrolled_win,Send_textview);
	hbox = gtk_hbox_new(0,0);
  vbox = gtk_vbox_new(FALSE, 0);
 	SaveButton = gtk_button_new_with_label("send");
  CancelButton = gtk_button_new_with_label("cancel");
	space_label = gtk_label_new("                         ");
	g_signal_connect(G_OBJECT(SaveButton), "clicked", G_CALLBACK(on_send), (gpointer)Send_textview);
  g_signal_connect(G_OBJECT(SaveButton),"clicked", G_CALLBACK(clean_send_text),NULL);
	g_signal_connect(G_OBJECT(CancelButton),"clicked",G_CALLBACK(delete_event_chat),NULL);
  gtk_box_pack_start(hbox,space_label,1,1,2);
  gtk_box_pack_start(hbox,SaveButton,0,0,2);
  gtk_box_pack_start(hbox,CancelButton,0,0,2);
  gtk_box_pack_start(vbox,Rcv_scrolled_win,1,1,2);
  gtk_box_pack_start(vbox,Send_scrolled_win,0,0,2);
  gtk_box_pack_start(vbox,hbox,0,0,2);
  gtk_container_add(chat_window,vbox);
  return chat_window;
}

GtkWidget *create_signup_window()
{
	GtkWidget *window_signup;
	GtkWidget *button1, *button2;
	GtkWidget *label1, *label2;
	GtkWidget *entry1, *entry2;
	GtkWidget *hbox1, *hbox2, *hbox3;
	GtkWidget *vbox;
	
	window_signup = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window_signup),"CHAT APP");
	entry1 = gtk_entry_new();
	entry2 = gtk_entry_new();
	
	label1 = gtk_label_new("Username");
	label2 = gtk_label_new("Password");

	button1 = gtk_button_new_with_label("Sumbit");
	button2 = gtk_button_new_with_label("Back");
		
	hbox1 = gtk_hbox_new(0,0);
	hbox2 = gtk_hbox_new(0,0);
	hbox3 = gtk_hbox_new(0,0);
	vbox = gtk_vbox_new(0,0);
	
	gtk_box_pack_start(GTK_BOX(hbox1),label1,0,0,100);
	gtk_box_pack_start(GTK_BOX(hbox1),entry1,0,0,0);

	gtk_box_pack_start(GTK_BOX(hbox2),label2,0,0,100);
        gtk_box_pack_start(GTK_BOX(hbox2),entry2,0,0,0);

        gtk_box_pack_start(GTK_BOX(hbox3),button1,0,0,100);
        gtk_box_pack_start(GTK_BOX(hbox3),button2,0,0,0);

	gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbox1),0,0,10);
        gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbox2),0,0,10);
	gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(hbox3),0,0,10);
	
	gtk_container_add(GTK_CONTAINER(window_signup),vbox);
 
	g_signal_connect(button1,"clicked",G_CALLBACK(delete_event),NULL);	
	g_signal_connect(button2,"clicked",G_CALLBACK(delete_event),NULL);
	g_signal_connect(window_signup, "delete_event",G_CALLBACK(delete_event), NULL);
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
	g_signal_connect(button1,"clicked",G_CALLBACK(show_chat_window),NULL);
	g_signal_connect(button3,"clicked",G_CALLBACK(show_signup_window),NULL);
	return window;

}

int main( int argc, char *argv[])
{
        GtkWidget *login_window;
        gtk_init(&argc, &argv);
        login_window = create_login_window();
        window_signup = create_signup_window();
        chat_window = create_chat_window();
        gtk_widget_show_all(login_window);
        gtk_main();
        return 0;

}

