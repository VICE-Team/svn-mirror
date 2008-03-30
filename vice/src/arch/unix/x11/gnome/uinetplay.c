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
#include <gnome.h>

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

static void
netplay_update_resources (void)
{
    gchar *server_name;
    char p[256];
    long port;

    strncpy(p, gtk_entry_get_text(
		GTK_ENTRY(gnome_entry_gtk_entry(GNOME_ENTRY(np_port)))), 
	    256);
    server_name = gtk_entry_get_text(
	GTK_ENTRY(gnome_entry_gtk_entry(GNOME_ENTRY(np_server))));
    
    util_string_to_long(p, NULL, 10, &port);
    if (port < 1 || port > 0xFFFF) {
        ui_error(_("Invalid Port number"));
	return;
    }
    
    resources_set_value("NetworkServerPort", (resource_value_t) port);
    resources_set_value("NetworkServerName", (resource_value_t) server_name);
}

static void
netplay_update_status(void)
{
    gchar *text = NULL;
    char *server_name;
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

    resources_get_value("NetworkServerPort", (void *) &port);
    resources_get_value("NetworkServerName", (void *) &server_name);
    snprintf(st, 256, "%d", port);
    gtk_entry_set_text(GTK_ENTRY(gnome_entry_gtk_entry(GNOME_ENTRY(np_port))), st);
    gtk_entry_set_text(GTK_ENTRY(gnome_entry_gtk_entry(GNOME_ENTRY(np_server))), 
		       server_name);
    log_message(np_log, _("Status: %s, Server: %s, Port: %d"),
		text, server_name, port);
}

static void 
netplay_start_server (GtkWidget *w, gpointer data)
{
    netplay_update_resources();
    if (network_start_server() < 0)
	ui_error(_("Couldn't start netplay server."));
    netplay_update_status();
    gnome_dialog_close(GNOME_DIALOG(netplay_dialog));
}

static void
netplay_connect (GtkWidget *w, gpointer data)
{
    netplay_update_resources();
    if (network_connect_client() < 0)
	ui_error(_("Couldn't connect client."));
    netplay_update_status();
    gnome_dialog_close(GNOME_DIALOG(netplay_dialog));
}

static void
netplay_disconnect (GtkWidget *w, gpointer data)
{
    netplay_update_resources();
    network_disconnect();
    netplay_update_status();
    gnome_dialog_close(GNOME_DIALOG(netplay_dialog));
}

static GtkWidget *
build_netplay_dialog(void)
{
    GtkWidget *d, *f, *b, *hb, *rb, *l, *entry;

    d = gnome_dialog_new(_("Netplay Settings"),
			 GNOME_STOCK_BUTTON_CANCEL,
			 NULL);
    
    ctrls = f = gtk_frame_new(_("Netplay Settings"));

    b = gtk_vbox_new(FALSE, 5);

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
    gtk_signal_connect(GTK_OBJECT(rb), "clicked",
		       GTK_SIGNAL_FUNC(netplay_start_server),
		       rb);
    GTK_WIDGET_UNSET_FLAGS (rb, GTK_CAN_FOCUS);
    gtk_widget_show(rb);

    l = gtk_label_new(_("TCP-Port: "));
    gtk_container_add(GTK_CONTAINER(hb), l);
    gtk_widget_show(l);
    
    /* entry port */
    np_port = entry = gnome_entry_new("vice: netplay_port");
    gtk_box_pack_start(GTK_BOX(hb), entry, FALSE, FALSE, GNOME_PAD);
    gtk_widget_show(entry);
    
    gtk_box_pack_start(GTK_BOX(b), hb, FALSE, FALSE, 5);
    gtk_widget_show(hb);

    hb = gtk_hbox_new(FALSE, 0);
    rb = gtk_button_new_with_label(_("Connect to "));
    gtk_box_pack_start(GTK_BOX(hb), rb, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(rb), "clicked",
		       GTK_SIGNAL_FUNC(netplay_connect),
		       rb);
    GTK_WIDGET_UNSET_FLAGS (rb, GTK_CAN_FOCUS);
    gtk_widget_show(rb);
    
    /* entry IP */
    np_server = entry = gnome_entry_new("vice: netplay_IP");
    gtk_box_pack_start(GTK_BOX(hb), entry, FALSE, FALSE, GNOME_PAD);
    gtk_widget_show(entry);
    
    gtk_box_pack_start(GTK_BOX(b), hb, FALSE, FALSE, 5);
    gtk_widget_show(hb);

    gtk_container_add(GTK_CONTAINER(f), b);
    gtk_widget_show(b);
    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(d)->vbox), f, TRUE, TRUE,
		       GNOME_PAD);
    gtk_widget_show(f);

    dcb = rb = gtk_button_new_with_label(_("Disconnect"));
    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(d)->vbox), rb, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(rb), "clicked",
		       GTK_SIGNAL_FUNC(netplay_disconnect),
		       rb);
    GTK_WIDGET_UNSET_FLAGS (rb, GTK_CAN_FOCUS);
    gtk_widget_show(rb);
    netplay_update_status();

    gnome_dialog_close_hides(GNOME_DIALOG(d), TRUE);
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
	gtk_signal_connect(GTK_OBJECT(netplay_dialog),
			   "destroy",
			   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			   &netplay_dialog);
    }
    ui_popup(netplay_dialog, "Netplay Dialog", FALSE);
    res = gnome_dialog_run(GNOME_DIALOG(netplay_dialog));
    ui_popdown(netplay_dialog);
}

#endif /* HAVE_NETWORK */
