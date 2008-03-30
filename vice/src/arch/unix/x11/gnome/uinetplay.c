/*
 * uinetplay.c - UI controls for netplay
 *
 * Written by
 *  Martin Pottendorfer (Martin.Pottendorfer@alcatel.at)
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */
#include "vice.h"

#include <stdio.h>

#ifdef HAVE_NETWORK
#include "ui.h"
#include "uiarch.h"
#include "network.h"
#include "resources.h"
#include "util.h"
#include "log.h"
#include "uinetplay.h"

static GtkWidget *netplay_dialog, *current_mode, *dcb, *ctrls, *np_server, *np_port;
static log_t np_log = LOG_ERR;

typedef struct np_control_s 
{
    char *name;
    GtkWidget *s_cb;
    GtkWidget *c_cb;
    unsigned int s_mask;
    unsigned int c_mask;
} np_control_t;

#define NR_NPCONROLS 5
static np_control_t np_controls[] = 
{ { N_("Keyboard"), NULL, NULL, 
    NETWORK_CONTROL_KEYB, 
    NETWORK_CONTROL_KEYB << NETWORK_CONTROL_CLIENTOFFSET },
  { N_("Joystick 1"), NULL, NULL, 
    NETWORK_CONTROL_JOY1, 
    NETWORK_CONTROL_JOY1 << NETWORK_CONTROL_CLIENTOFFSET },
  { N_("Joystick 2"), NULL, NULL, 
    NETWORK_CONTROL_JOY2, 
    NETWORK_CONTROL_JOY2 << NETWORK_CONTROL_CLIENTOFFSET },
  { N_("Devices"), NULL, NULL, 
    NETWORK_CONTROL_DEVC, 
    NETWORK_CONTROL_DEVC << NETWORK_CONTROL_CLIENTOFFSET },
  { N_("Settings"), NULL, NULL, 
    NETWORK_CONTROL_RSRC, 
    NETWORK_CONTROL_RSRC << NETWORK_CONTROL_CLIENTOFFSET },
  { NULL, NULL, 0, 0 }};

static void
netplay_update_control_res (GtkWidget *w, gpointer data)
{
    unsigned int control, mask;

    g_return_if_fail(GTK_IS_CHECK_BUTTON(w));
    g_return_if_fail(data != 0);
    
    mask = *((unsigned int *) data);
    resources_get_int("NetworkControl", (int *)&control);
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(w)))
	control |= mask;
    else
	control &= ~mask;
    
    resources_set_int("NetworkControl", (int)control);
    /* log_message(np_log, _("Updated control: 0x%04x"), control); */
}

static void
netplay_update_control_gui (void)
{
    int i;
    unsigned int control;

    resources_get_int("NetworkControl", (int *)&control);
    for (i = 0; i < NR_NPCONROLS; i++)
    {
	if (control & np_controls[i].s_mask)
	    gtk_toggle_button_set_active (
		GTK_TOGGLE_BUTTON(np_controls[i].s_cb), TRUE);
	if (control & np_controls[i].c_mask)
	    gtk_toggle_button_set_active (
		GTK_TOGGLE_BUTTON(np_controls[i].c_cb), TRUE);
    }
}

static void
netplay_update_resources (void)
{
    const gchar *server_name;
    char p[256];
    long port;
    
    strncpy(p, gtk_entry_get_text(GTK_ENTRY(np_port)), 256);
    server_name = gtk_entry_get_text(GTK_ENTRY(np_server));
    util_string_to_long(p, NULL, 10, &port);
    if (port < 1 || port > 0xFFFF) {
        ui_error(_("Invalid Port number"));
	return;
    }
    resources_set_int("NetworkServerPort", (int)port);
    resources_set_string("NetworkServerName", server_name);
}

static void
netplay_update_status(void)
{
    gchar *text = NULL;
    const char *server_name;
    int port;
    char st[256];
    
    switch(network_get_mode())
    {
    case NETWORK_IDLE:
	gtk_widget_set_sensitive(GTK_WIDGET(dcb), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(ctrls), TRUE);
	text = _("Idle");
	break;
    case NETWORK_SERVER:
	gtk_widget_set_sensitive(GTK_WIDGET(dcb), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(ctrls), FALSE);
	text = _("Server listening");
	break;
    case NETWORK_SERVER_CONNECTED:
	gtk_widget_set_sensitive(GTK_WIDGET(dcb), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(ctrls), FALSE);
	text = _("Connected Server");
	break;
    case NETWORK_CLIENT:
	gtk_widget_set_sensitive(GTK_WIDGET(dcb), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(ctrls), FALSE);
	text = _("Connected Client");
	break;
    default:
	break;
    }
    gtk_label_set_text(GTK_LABEL(current_mode), text);

    resources_get_int("NetworkServerPort", &port);
    resources_get_string("NetworkServerName", &server_name);
    snprintf(st, 256, "%d", port);
    gtk_entry_set_text(GTK_ENTRY(np_port), st);
    gtk_entry_set_text(GTK_ENTRY(np_server), server_name);
    log_message(np_log, _("Status: %s, Server: %s, Port: %d"),
		text, server_name, port);
    netplay_update_control_gui();
}

static void 
netplay_start_server (GtkWidget *w, gpointer data)
{
    netplay_update_resources();
    if (network_start_server() < 0)
	ui_error(_("Couldn't start netplay server."));
    netplay_update_status();
    gtk_dialog_response(GTK_DIALOG(netplay_dialog), GTK_RESPONSE_CANCEL);
}

static void
netplay_connect (GtkWidget *w, gpointer data)
{
    netplay_update_resources();
    if (network_connect_client() < 0)
	ui_error(_("Couldn't connect client."));
    netplay_update_status();
    gtk_dialog_response(GTK_DIALOG(netplay_dialog), GTK_RESPONSE_CANCEL);
}

static void
netplay_disconnect (GtkWidget *w, gpointer data)
{
    netplay_update_resources();
    network_disconnect();
    netplay_update_status();
    gtk_dialog_response(GTK_DIALOG(netplay_dialog), GTK_RESPONSE_CANCEL);
}

static GtkWidget *
build_netplay_dialog(void)
{
    GtkWidget *d, *f, *b, *hb, *rb, *l, *entry, *h;

    d = gtk_dialog_new_with_buttons(_("Netplay Settings"),
				    NULL,
				    GTK_DIALOG_DESTROY_WITH_PARENT,
				    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				    NULL);
    
    f = gtk_frame_new(_("Netplay Settings"));

    h = gtk_hbox_new(FALSE, 5);
    ctrls = b = gtk_vbox_new(FALSE, 5);

    hb = gtk_hbox_new(FALSE, 0);
    l = gtk_label_new(_("Current mode: "));
    gtk_container_add(GTK_CONTAINER(hb), l);
    gtk_widget_show(l);
    current_mode = gtk_label_new(_("<unknown>"));
    gtk_container_add(GTK_CONTAINER(hb), current_mode);
    gtk_widget_show(current_mode);
    gtk_box_pack_start(GTK_BOX(b), hb, FALSE, FALSE, 5);
    gtk_widget_show(hb);
        
    hb = gtk_hbox_new(FALSE, 0);
    rb = gtk_button_new_with_label(_("Start Server"));
    gtk_box_pack_start(GTK_BOX(hb), rb, FALSE, FALSE, 5);
    g_signal_connect(G_OBJECT(rb), "clicked",
		     G_CALLBACK(netplay_start_server),
		     rb);
    GTK_WIDGET_UNSET_FLAGS (rb, GTK_CAN_FOCUS);
    gtk_widget_show(rb);

    l = gtk_label_new(_("TCP-Port: "));
    gtk_container_add(GTK_CONTAINER(hb), l);
    gtk_widget_show(l);
    
    /* entry port */
    np_port = entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hb), entry, FALSE, FALSE, 0);
    gtk_widget_set_size_request(entry, 50, -1);
    gtk_widget_show(entry);
    
    gtk_box_pack_start(GTK_BOX(b), hb, FALSE, FALSE, 5);
    gtk_widget_show(hb);

    hb = gtk_hbox_new(FALSE, 0);
    rb = gtk_button_new_with_label(_("Connect to "));
    gtk_box_pack_start(GTK_BOX(hb), rb, FALSE, FALSE, 5);
    g_signal_connect(G_OBJECT(rb), "clicked",
		     G_CALLBACK(netplay_connect),
		     rb);
    GTK_WIDGET_UNSET_FLAGS (rb, GTK_CAN_FOCUS);
    gtk_widget_show(rb);
    
    /* entry IP */
    np_server = entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hb), entry, FALSE, FALSE, 0);
    gtk_widget_set_size_request(entry, 100, -1);
    gtk_widget_show(entry);
    
    gtk_box_pack_start(GTK_BOX(b), hb, FALSE, FALSE, 5);
    gtk_widget_show(hb);

    gtk_box_pack_start(GTK_BOX(h), b, FALSE, FALSE, 5);
    gtk_widget_show(b);
    
    /* Control widgets */
    {
	GtkWidget *cf, *tmp, *table;
	int i;
	
	cf = gtk_frame_new(_("Control"));
	gtk_box_pack_start(GTK_BOX(h), cf, FALSE, FALSE, 5);
	gtk_widget_show(cf);

	table = gtk_table_new(NR_NPCONROLS + 1, 3, FALSE);
	tmp = gtk_label_new(_("Server"));
	gtk_table_attach(GTK_TABLE(table), tmp, 
			 1, 2, 0, 1,
			 0, 0, 5, 0);
	gtk_widget_show(tmp);
	tmp = gtk_label_new(_("Client"));
	gtk_table_attach(GTK_TABLE(table), tmp, 
			 2, 3, 0, 1,
			 0, 0, 5, 0);
	gtk_widget_show(tmp);
	
	for (i = 0; i < NR_NPCONROLS; i++)
	{
	    tmp = gtk_label_new(_(np_controls[i].name));
	    gtk_table_attach_defaults(GTK_TABLE(table), tmp, 
				      0, 1, i+1, i+2);
	    gtk_widget_show(tmp);
	    np_controls[i].s_cb = gtk_check_button_new();
	    gtk_table_attach_defaults(GTK_TABLE(table), np_controls[i].s_cb,
				      1, 2, i+1, i+2);
	    g_signal_connect(G_OBJECT(np_controls[i].s_cb), "toggled",
			     G_CALLBACK(netplay_update_control_res), 
			     (gpointer) &np_controls[i].s_mask);
	    gtk_widget_show(np_controls[i].s_cb);
	    np_controls[i].c_cb = gtk_check_button_new();
	    gtk_table_attach_defaults(GTK_TABLE(table), np_controls[i].c_cb,
				      2, 3, i+1, i+2);
	    g_signal_connect(G_OBJECT(np_controls[i].c_cb), "toggled",
			     G_CALLBACK(netplay_update_control_res), 
			     (gpointer) &np_controls[i].c_mask);
	    gtk_widget_show(np_controls[i].c_cb);
	}
	gtk_container_add(GTK_CONTAINER(cf), table);
	gtk_widget_show(table);
    }
    
    gtk_container_add(GTK_CONTAINER(f), h);
    gtk_widget_show(h);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(d)->vbox), f, TRUE, TRUE,
		       0);
    gtk_widget_show(f);

    dcb = rb = gtk_button_new_with_label(_("Disconnect"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(d)->vbox), rb, FALSE, FALSE, 5);
    g_signal_connect(G_OBJECT(rb), "clicked",
		     G_CALLBACK(netplay_disconnect),
		     rb);
    GTK_WIDGET_UNSET_FLAGS (rb, GTK_CAN_FOCUS);
    gtk_widget_show(rb);
    netplay_update_status();

    /* gtk_dialog_close_hides(GTK_DIALOG(d), TRUE); */
    return d;
}

void 
ui_netplay_dialog(void)
{
    gint res;
    
    if (netplay_dialog) 
    {
	gdk_window_show(netplay_dialog->window);
	gdk_window_raise(netplay_dialog->window);
	gtk_widget_show(netplay_dialog);
	netplay_update_status();
    }
    else
    {
	np_log = log_open("Netplay");
	netplay_dialog = build_netplay_dialog();
	g_signal_connect(G_OBJECT(netplay_dialog),
			 "destroy",
			 G_CALLBACK(gtk_widget_destroyed),
			 &netplay_dialog);
    }
    ui_popup(netplay_dialog, "Netplay Dialog", FALSE);
    res = gtk_dialog_run(GTK_DIALOG(netplay_dialog));
    ui_popdown(netplay_dialog);
}

#endif /* HAVE_NETWORK */
