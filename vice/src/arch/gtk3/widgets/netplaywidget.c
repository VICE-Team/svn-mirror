/** \file   netplaywidget.c
 * \brief   Netplay settings widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES NetworkServerName           -vsid
 * $VICERES NetworkServerPort           -vsid
 * $VICERES NetworkServerBindAddress    -vsid
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

#include "vice_gtk3.h"
#include "debug_gtk3.h"
#include "lib.h"
#include "log.h"
#include "network.h"
#include "resources.h"
#include "ui.h"

#include "netplaywidget.h"


/** \brief  Net control widget info
 */
typedef struct netctrl_s {
    const char *    text;  /**< label */
    unsigned int    mask;  /**< bit to toggle */
} netctrl_t;


/** \brief  List of controls
 */
static const netctrl_t control_list[] = {
    { "Keyboard",       NETWORK_CONTROL_KEYB },
    { "Joystick #1",    NETWORK_CONTROL_JOY1 },
    { "Joystick #2",    NETWORK_CONTROL_JOY2 },
    { "Devices",        NETWORK_CONTROL_DEVC },
    { "Resources",      NETWORK_CONTROL_RSRC },
    { NULL,             0 }
};


/** \brief  Column headers for the NetworkControl resource
 */
static const char *netctrl_headers[] = { "", "server", "client" };


/** \brief  Modes for the netplay status display
 */
static const char *net_modes[] = {
    "Idle",             /* NETWORK_IDLE */
    "Server",           /* NETWORK_SERVER */
    "Server connected", /* NETWORK_SERVER_CONNECTED */
    "Client connected"  /* NETWORK_CLIENT */
};


/* Widget references for the glue logic */

/** \brief  Server address widget */
static GtkWidget *server_address = NULL;

/** \brief  Client address widget */
static GtkWidget *client_address = NULL;

/** \brief  Client and server port number */
static GtkWidget *port_number = NULL;

/** \brief  Netplay status widget */
static GtkWidget *netplay_status = NULL;

/** \brief  Netplay controls widget */
static GtkWidget *controls = NULL;

/** \brief  Network mode combo box
 */
static GtkWidget *combo_netplay = NULL;

/** \brief  Client enable widget */
static GtkWidget *netplay_enable = NULL;

/** \brief  last used setting was client or server? */
static int netplay_mode = -1;

/** \brief  Update display of the netplay status
 */
static void netplay_update_status(void)
{
    const char *text = NULL;
    char *temp;
    int mode = network_get_mode();
    int server = (gtk_combo_box_get_active(GTK_COMBO_BOX(combo_netplay)) == 0);

    if (mode < 0 || mode >= (int)(sizeof net_modes / sizeof net_modes[0])) {
        text = "invalid";
    } else {
        text = net_modes[mode];
    }

    debug_gtk3("netplay_update_status: %d (%s)\n", mode, text);

    temp = lib_msprintf("<b>%s</b>", text);
    gtk_label_set_markup(GTK_LABEL(netplay_status), temp);
    lib_free(temp);

    /* mode combobox cant be changed when network is active */
    gtk_combo_box_set_button_sensitivity(GTK_COMBO_BOX(combo_netplay),
        (mode == NETWORK_IDLE) ? GTK_SENSITIVITY_ON : GTK_SENSITIVITY_OFF);
    /* port cant be changed when network is active */
    gtk_widget_set_sensitive(port_number,
        (mode == NETWORK_IDLE) ? TRUE : FALSE);
    /* server address can only be changed when server is selected, and we are idle */
    gtk_widget_set_sensitive(server_address,
        ((mode == NETWORK_IDLE) && server) ? TRUE : FALSE);
    /* client address can only be changed when client is selected, and we are idle */
    gtk_widget_set_sensitive(client_address,
        ((mode == NETWORK_IDLE) && !server) ? TRUE : FALSE);
}

/** \brief  Handler for the 'toggled' event of the NetworkControl checkboxes
 *
 * Basically EOR's the resource NetworkControl with \c data
 *
 * \param[in,out]   widget  checkbox
 * \param[in]       data    bitmask
 */
static void on_server_mask_toggled(GtkWidget *widget, gpointer data)
{
    int value;
    unsigned int newval;
    unsigned int mask;

    resources_get_int("NetworkControl", &value);
    mask = GPOINTER_TO_UINT(data);

    /* do the actual work: flip a bit */
    newval = value ^ mask;
#if 1
    debug_gtk3("State: %s, Mask: %02x, Value: %02x",
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
            ? "Enabled" : "Disabled",
            mask,
            newval);
#endif
    if ((unsigned int)value != newval) {
        resources_set_int("NetworkControl", (int)newval);
    }
}

/** \brief  Handler for the 'notify::toggled' event of the client enable switch
 *
 * \param[in,out]   widget  client enable switch
 * \param[in]       data    extra event data (unused)
 */
static void on_netplay_enable_toggled(GtkSwitch *widget, gpointer data)
{
    int state = gtk_switch_get_active(widget);
    int idx = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_netplay));

    debug_gtk3("on_netplay_enable_toggled to %d\n", idx);

    /* just disconnect, doing it always is what the SDL port does and its stable */
    network_disconnect();
    netplay_update_status();

    if (state) {
        if (idx == 0) {
            /* attempt to start server */
            if (network_start_server() < 0) {
                log_error(LOG_ERR, "Failed to start netplay server.");
                gtk_switch_set_active(widget, 0);
            }
        } else {
            /* start the client */
            if (network_connect_client() < 0) {
                log_error(LOG_ERR, "Failed to start client.");
                gtk_switch_set_active(widget, 0);
            }
        }
    }
    netplay_update_status();
}

/** \brief  Handler for the "changed" event of the network mode combo box
 *
 * \param[in]   combo       combo box
 * \param[in]   user_data   extra event data (unused)
 */
static void on_combo_changed(GtkComboBox *combo, gpointer user_data)
{
    int server = (gtk_combo_box_get_active(combo) == 0);
    int mode = network_get_mode();

    netplay_mode = gtk_combo_box_get_active(combo);

    /* server address can only be changed when server is selected, and we are idle */
    gtk_widget_set_sensitive(server_address,
        ((mode == NETWORK_IDLE) && server) ? TRUE : FALSE);
    /* client address can only be changed when client is selected, and we are idle */
    gtk_widget_set_sensitive(client_address,
        ((mode == NETWORK_IDLE) && !server) ? TRUE : FALSE);
}

/** \brief  Create combo box with network modes
 *
 * \return  GtkComboBoxText
 */
static GtkWidget *create_combo_box(void)
{
    GtkWidget *combo;
    int mode = network_get_mode();

    combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo),
            NULL, "This emulator is the server");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo),
            NULL, "This emulator is the client");

    if (netplay_mode < 0) {
        netplay_mode = (mode == NETWORK_CLIENT ? 1 : 0);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), netplay_mode);

    g_signal_connect(combo, "changed", G_CALLBACK(on_combo_changed), NULL);
    return combo;
}

/** \brief  Create netplay switch
 *
 * \return  GtkSwitch
 */
static GtkWidget *create_netplay_enable_widget(void)
{
    GtkWidget *widget;
    int mode;

    mode = network_get_mode();
    widget = gtk_switch_new();

    gtk_widget_set_halign(widget, GTK_ALIGN_START);
    gtk_switch_set_active(GTK_SWITCH(widget), mode != NETWORK_IDLE);

    g_signal_connect(widget, "notify::active",
            G_CALLBACK(on_netplay_enable_toggled), NULL);
    return widget;
}

/** \brief  Create controls checkboxes
 *
 * \return  GtkGrid
 */
static GtkWidget *create_controls_widget(void)
{
    GtkWidget *grid;
    int status;

    grid = vice_gtk3_grid_new_spaced_with_label(32, 8, "Controls", 3);
    g_object_set(grid, "margin-left", 16, "margin-top", 32, NULL);

    resources_get_int("NetworkControl", &status);

    for (int i = 0; i < 3; i++) {
        GtkWidget *label;

        label = gtk_label_new(netctrl_headers[i]);
        gtk_grid_attach(GTK_GRID(grid), label, i, 1, 1, 1);
    }

    for (int i = 0; control_list[i].text != NULL; i++) {

        GtkWidget *label;
        GtkWidget *check;
        netctrl_t data = control_list[i];

        /* name of the thing */
        label = gtk_label_new(data.text);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        g_object_set(label, "margin-left", 16, NULL);
        gtk_grid_attach(GTK_GRID(grid), label, 0, i + 2, 1, 1);

        /* checkbutton to toggle the server side */
        check = gtk_check_button_new();
        gtk_widget_set_halign(check, GTK_ALIGN_CENTER);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),
                status & data.mask);
        g_signal_connect(check, "toggled",
                G_CALLBACK(on_server_mask_toggled),
                GINT_TO_POINTER(data.mask));
        gtk_grid_attach(GTK_GRID(grid), check, 1, i + 2, 1, 1);

        /* checkbutton to toggle the client side */
        check = gtk_check_button_new();
        gtk_widget_set_halign(check, GTK_ALIGN_CENTER);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),
                (status >> 8) & data.mask);
        g_signal_connect(check, "toggled",
                G_CALLBACK(on_server_mask_toggled),
                GINT_TO_POINTER(data.mask << 8));

        gtk_grid_attach(GTK_GRID(grid), check, 2, i + 2, 1, 1);

    }

    gtk_widget_show_all(grid);
    return grid;
}



/** \brief  Create Netplay settings widget
 *
 * \param[in]   parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *netplay_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;

    grid = vice_gtk3_grid_new_spaced_with_label(-1, -1, "Netplay settings", 4);

    /* client or server? */
    label = vice_gtk3_create_indented_label("Mode");
    combo_netplay = create_combo_box();
    netplay_enable = create_netplay_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_netplay, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), netplay_enable, 2, 1, 1, 1);

    /* Server widgets */

    /* label */
    label = vice_gtk3_create_indented_label("Server Address");
    /* address */
    server_address = vice_gtk3_resource_entry_full_new("NetworkServerBindAddress");
    gtk_widget_set_hexpand(server_address, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), server_address, 1, 2, 1, 1);

    /* Client widgets */

    /* label */
    label = vice_gtk3_create_indented_label("Remote Server");
    /* address */
    client_address = vice_gtk3_resource_entry_full_new("NetworkServerName");
    gtk_widget_set_hexpand(client_address, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), client_address, 1, 3, 1, 1);

    /* Port widgets */

    /* label */
    label = vice_gtk3_create_indented_label("Port");
    /* port */
    port_number = vice_gtk3_resource_spin_int_new("NetworkServerPort",
            1, 65535, 1);
    gtk_widget_set_hexpand(port_number, FALSE);
    gtk_widget_set_halign(port_number, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), port_number, 1, 4, 1, 1);

    /* Network status widgets */

    /* label */
    label = vice_gtk3_create_indented_label("Network status");
    /* status widget */
    netplay_status = gtk_label_new(NULL);
    gtk_widget_set_halign(netplay_status, GTK_ALIGN_START);
    gtk_widget_set_hexpand(netplay_status, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), netplay_status, 1, 5, 2, 1);
    /* update status text */
    netplay_update_status();

    controls = create_controls_widget();
    g_object_set(controls, "margin-top", 32, "margin-left", 0, NULL);

    gtk_grid_attach(GTK_GRID(grid), create_controls_widget(), 0, 6, 3, 1);

    gtk_widget_show_all(grid);
    return grid;
}
