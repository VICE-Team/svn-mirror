/** \file   uinetplay_new.c
 * \brief   GTK3 Netplay UI
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES NetworkServerName           -vsid
 * $VICERES NetworkServerPort           -vsid
 * $VICERES NetworkServerBindAddress    -vsid
 *
 * Not sure about this one (TODO):
 * $VICERES NetworkControl              -vsid
 */

/*
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
 */

#include "vice.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "basewidgets.h"
#include "debug_gtk3.h"
#include "lib.h"
#include "network.h"
#include "resources.h"
#include "ui.h"
#include "widgethelpers.h"

#include "uinetplay_new.h"


static void netplay_update_status(void);


typedef struct netctrl_s {
    const char *    text;  /**< label */
    unsigned int    mask;  /**< bit to toggle */
} netctrl_t;


static const netctrl_t control_list[] = {
    { "Keyboard",       NETWORK_CONTROL_KEYB },
    { "Joystick #1",    NETWORK_CONTROL_JOY1 },
    { "Joystick #2",    NETWORK_CONTROL_JOY2 },
    { "Devices",        NETWORK_CONTROL_DEVC },
    { "Resources",      NETWORK_CONTROL_RSRC },
    { NULL,             0 }
};


static GtkWidget *server_addr = NULL;
static GtkWidget *server_port = NULL;
static GtkWidget *server_enable = NULL;
static GtkWidget *bind_addr = NULL;
static GtkWidget *bind_enable = NULL;
static GtkWidget *status_widget = NULL;
static GtkWidget *controls_widget = NULL;


static const char *net_modes[] = {
    "Idle",
    "Server",
    "Server connected"
    "Client connected"
};


static gboolean netplay_update_resources(void)
{
    const gchar *s_addr;
    const gchar *s_port;
    const gchar *b_addr;
    long port;
    char *endptr;

    s_addr = gtk_entry_get_text(GTK_ENTRY(server_addr));
    s_port = gtk_entry_get_text(GTK_ENTRY(server_port));
    b_addr = gtk_entry_get_text(GTK_ENTRY(bind_addr));

    errno = 0;
    port = strtol(s_port, &endptr, 0);
    if (errno == ERANGE || endptr == s_port || *endptr != '\0') {
        /* oops */
        return FALSE;
    }

    if (resources_set_int("NetworkServerPort", (int)port) < 0) {
        return FALSE;
    }
    if (resources_set_string("NetworkServerName", s_addr) < 0) {
        return FALSE;
    }
    if (resources_set_string("NetworkServerBindAddress", b_addr) < 0) {
        return FALSE;
    }

    /* TODO: the controls mappings */

    return TRUE;
}



static GtkWidget *create_indented_label(const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    return label;
}


static void on_server_enable_toggled(GtkSwitch *widget, gpointer data)
{
    int state = gtk_switch_get_active(widget);
    int bind = gtk_switch_get_active(GTK_SWITCH(bind_enable));


    debug_gtk3("Server %s requested.", state ? "ENABLE" : "DISABLE");

    if (state) {
        if (bind) {
            /* cannot have both */
            gtk_switch_set_active(GTK_SWITCH(bind_enable), FALSE);
        }
        /* try starting server */
        if (network_start_server() == 0) {
            debug_gtk3("Started Netplay server.");
        }
        netplay_update_resources();
    } else {
        network_disconnect();
        debug_gtk3("Disconnected server.");
    }
    netplay_update_status();
}


static void on_client_enable_toggled(GtkSwitch *widget, gpointer data)
{
    int state = gtk_switch_get_active(widget);
    int server = gtk_switch_get_active(GTK_SWITCH(server_enable));

    debug_gtk3("Client %s requested.", state ? "CONNECT" : "DISCONNECT");

    if (state) {
        if (server) {
            gtk_switch_set_active(GTK_SWITCH(server_enable), FALSE);
        }
        if (network_connect_client() == 0) {
            debug_gtk3("Netplay listening.");
        }
        netplay_update_resources();
    } else {
        network_disconnect();
        debug_gtk3("Disconnected client.");
    }
    netplay_update_status();
}



static GtkWidget *create_server_enable_widget(void)
{
    GtkWidget *widget;
    int mode = network_get_mode();

    widget = gtk_switch_new();
    gtk_widget_set_halign(widget, GTK_ALIGN_START);

    gtk_switch_set_active(GTK_SWITCH(widget),
            mode == NETWORK_SERVER || mode == NETWORK_SERVER_CONNECTED);

    g_signal_connect(widget, "notify::active",
            G_CALLBACK(on_server_enable_toggled), NULL);
    return widget;
}


static GtkWidget *create_client_enable_widget(void)
{
    GtkWidget *widget;
    int mode = network_get_mode();

    widget = gtk_switch_new();
    gtk_widget_set_halign(widget, GTK_ALIGN_START);

    gtk_switch_set_active(GTK_SWITCH(widget), mode == NETWORK_CLIENT);

    g_signal_connect(widget, "notify::active",
            G_CALLBACK(on_client_enable_toggled), NULL);
    return widget;
}



/** \brief  Handler for the "response" event of the dialog
 *
 * \param[in,out]   dialog      dialog
 * \param[in]       response_id response ID
 * \param[in]       extra event data (unused)
 */
static void on_response(GtkWidget *dialog, gint response_id, gpointer data)
{
    switch (response_id) {
        case GTK_RESPONSE_DELETE_EVENT:
            gtk_widget_destroy(dialog);
            break;
        default:
            debug_gtk3("unknown response ID %d, ignoring.", response_id);
            break;
    }
}


/** \brief  Update display of the netplay status
 */
static void netplay_update_status(void)
{
    const char *text = NULL;
    char *temp;
    int mode = network_get_mode();

    if (mode < 0 || mode >= (int)(sizeof net_modes / sizeof net_modes[0])) {
        text = "invalid";
    } else {
        text = net_modes[mode];
    }

    temp = lib_msprintf("<b>%s</b>", text);
    gtk_label_set_markup(GTK_LABEL(status_widget), temp);
    lib_free(temp);
}


/** \brief  Handler for the 'toggled' events of the server/client controls
 *
 * \param[in,out]   widget  toggle button triggering the event
 * \param[in]       data    bit to toggle in the "NetworkControl" resource
 */
static void on_control_checkbox_toggled(GtkWidget *widget, gpointer data)
{
    unsigned int bit = GPOINTER_TO_UINT(data);
    unsigned int old;
    gboolean active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    unsigned new;
    int tmp;

    resources_get_int("NetworkControl", &tmp);
    old = (unsigned int)tmp;

    if (active) {
        /* switch bit on */
        new = old | bit;
    } else {
        new = old & ~bit;
    }

    /* only update resource when something changed */
    if (new != old) {
        resources_set_int("NetworkControl", new);
        debug_gtk3("Setting NetworkControl to %x.", new);
    }

}


/** \brief  Create widget to switch various stuff controlled from server/client
 *
 * \see src/network.h for some idea of what this probably does.
 *
 * \return  GtkGrid
 */
static GtkWidget *create_controls_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *controls;
    unsigned int i;
    int cstatus;

    resources_get_int("NetworkControl", &cstatus);


    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);
    g_object_set(grid, "margin-top", 16, NULL);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label),
            "<b>I don't have a clue what this does</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);


    /* create header for the checkboxes */
    controls = vice_gtk3_grid_new_spaced(32, 8);
    label = gtk_label_new("Server");
    gtk_grid_attach(GTK_GRID(controls), label, 1, 0, 1, 1);
    label = gtk_label_new("Client");
    gtk_grid_attach(GTK_GRID(controls), label, 2, 0, 1, 1);

    for (i = 0; control_list[i].text != NULL; i++) {

        GtkWidget *server;
        GtkWidget *client;

        label = gtk_label_new(control_list[i].text);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        g_object_set(label, "margin-left", 16, NULL);
        gtk_grid_attach(GTK_GRID(controls), label, 0, i + 1, 1, 1);

        server = gtk_check_button_new();
        gtk_widget_set_halign(server, GTK_ALIGN_CENTER);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(server),
                cstatus & (1 << i));
        gtk_grid_attach(GTK_GRID(controls), server, 1, i + 1, 1, 1);

        client = gtk_check_button_new();
        gtk_widget_set_halign(client, GTK_ALIGN_CENTER);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(server),
                (cstatus >> 8) & (1 << i));
        gtk_grid_attach(GTK_GRID(controls), client, 2, i + 1, 1, 1);

        g_signal_connect(server, "toggled",
                G_CALLBACK(on_control_checkbox_toggled),
                GUINT_TO_POINTER(control_list[i].mask));
        g_signal_connect(client, "toggled",
                G_CALLBACK(on_control_checkbox_toggled),
                GUINT_TO_POINTER(control_list[i].mask << NETWORK_CONTROL_CLIENTOFFSET));




    }

    gtk_grid_attach(GTK_GRID(grid), controls, 0, 1, 4, 1);
    return grid;
}


/** \brief  Create the 'content' widget for the dialog
 *
 * This creates the content of the dialog, holding all controls
 *
 * \return  GtkGrid
 */
static GtkWidget *create_content_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    int row;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);
    g_object_set(G_OBJECT(grid),
            "margin-top", 8,
            "margin-bottom", 8,
            "margin-left", 16,
            "margin-right", 16,
            NULL);

    row = 0;

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Server</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 4, 1);
    row++;

    label = create_indented_label("Server IP address");
    server_addr = vice_gtk3_resource_entry_full_new("NetworkServerName");
    gtk_widget_set_hexpand(server_addr, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), server_addr, 1, row, 1, 1);

    server_port = vice_gtk3_resource_spin_int_new("NetworkServerPort",
            1, 65535, 1);
    gtk_grid_attach(GTK_GRID(grid), server_port, 2, row, 1, 1);
    server_enable = create_server_enable_widget();

    gtk_grid_attach(GTK_GRID(grid), server_enable, 3, row, 1, 1);
    row++;

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Client</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 4, 1);
    row++;

    label = create_indented_label("Bind IP address");
    bind_addr = vice_gtk3_resource_entry_full_new(
            "NetworkServerBindAddress");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), bind_addr, 1, row, 1, 1);

    bind_enable = create_client_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), bind_enable, 3, row, 2, 1);
    row++;

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Network status</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);

    status_widget = gtk_label_new("Idle");
    gtk_widget_set_halign(status_widget, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), status_widget, 1, row, 3, 1);

    row++;
    controls_widget = create_controls_widget();
    gtk_grid_attach(GTK_GRID(grid), controls_widget, 0, row, 4, 1);

    gtk_widget_show_all(grid);
    return grid;
}



/** \brief  Show Netplay dialog
 *
 * \param[in]   parent  parent widget
 * \param[in]   data    extra data (unused)
 */
void ui_netplay_dialog_new(GtkWidget *parent, gpointer data)
{
    GtkWidget *dialog;
    GtkWidget *content;

    dialog = gtk_dialog_new_with_buttons("Netplay",
            ui_get_active_window(),
            GTK_DIALOG_MODAL,
            "Close", GTK_RESPONSE_DELETE_EVENT,
            NULL);

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(content), create_content_widget());
    netplay_update_status();

    g_signal_connect(dialog, "response", G_CALLBACK(on_response), NULL);

    gtk_widget_show_all(dialog);
}
