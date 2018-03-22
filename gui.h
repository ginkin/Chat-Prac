#include<gtk/gtk.h>
#include<stdlib.h>
#include<stdio.h>

static void show_signup_window();

static void show_chat_window();

static void name_info_input( GtkWidget *widget,
                            GtkWidget *entry );

static void password_info_signup( GtkWidget *widget,
                            GtkWidget *entry );

static void name_info_signup( GtkWidget *widget,
                            GtkWidget *entry );

static void password_info_input( GtkWidget *widget,
                            GtkWidget *entry );

static void delete_event();

static void delete_event_chat();

void clean_send_text();

void on_send();

void show_text(const gchar* text);


GtkWidget *create_chat_window();

GtkWidget *create_signup_window();

GtkWidget *create_login_window();









