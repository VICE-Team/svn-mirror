/** \file   uidatasette.c
 * \brief   Create independent datasette control widgets
 *
 * \author  Michael C. Martin <mcmartin@gmail.com>
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
 *
 */

#include "vice.h"
#include "uidatasette.h"
#include "datasette.h"
#include "resources.h"
#include "uitapeattach.h"
#include "uisettings.h"

#include <stdio.h>


/** \brief  Handler for the 'activate' event of the "Configure" menu item
 *
 * Pop up the settings UI and select "I/O extensions" -> "Tapeport devices"
 *
 * \param[in]   widget  menu item triggering the event (unused)
 * \param[in]   data    extra event data (unused)
 */
static void on_configure_activate(GtkWidget *widget, gpointer data)
{
    ui_settings_dialog_create_and_activate_node(
            "peripheral/tapeport-devices");
}


/** \brief  Datasette UI action callback
 *
 * \param[in]   widget  parent widget (unused)
 * \param[in]   data    action value
 *
 * \return  TRUE to indicate the UI event was handled
 */
gboolean ui_datasette_tape_action_cb(GtkWidget *widget, gpointer data)
{
    int val = GPOINTER_TO_INT(data);

    if (val >= DATASETTE_CONTROL_STOP && val <= DATASETTE_CONTROL_RESET_COUNTER) {
        datasette_control(val);
    } else {
        fprintf(stderr,
                "Got an impossible Datasette Control action, code %ld (valid range %d-%d)\n",
                (long)val,
                DATASETTE_CONTROL_STOP,
                DATASETTE_CONTROL_RESET_COUNTER);
    }
    return TRUE;
}


/** \brief  Create datasette control menu
 *
 * \return  GtkMenu with datasette controls
 */
GtkWidget *ui_create_datasette_control_menu(void)
{
    GtkWidget *menu, *item, *menu_items[DATASETTE_CONTROL_RESET_COUNTER+1];
    int i;

    menu = gtk_menu_new();
    item = gtk_menu_item_new_with_label("Attach tape image...");
    gtk_container_add(GTK_CONTAINER(menu), item);
    g_signal_connect_unlocked(item, "activate", G_CALLBACK(ui_tape_attach_callback), NULL);
    item = gtk_menu_item_new_with_label("Detach tape image");
    gtk_container_add(GTK_CONTAINER(menu), item);
    g_signal_connect(item, "activate", G_CALLBACK(ui_tape_detach_callback), NULL);
    gtk_container_add(GTK_CONTAINER(menu), gtk_separator_menu_item_new());
    menu_items[0] = gtk_menu_item_new_with_label("Stop");
    menu_items[1] = gtk_menu_item_new_with_label("Play");
    menu_items[2] = gtk_menu_item_new_with_label("Forward");
    menu_items[3] = gtk_menu_item_new_with_label("Rewind");
    menu_items[4] = gtk_menu_item_new_with_label("Record");
    menu_items[5] = gtk_menu_item_new_with_label("Reset");
    menu_items[6] = gtk_menu_item_new_with_label("Reset Counter");
    for (i = 0; i <= DATASETTE_CONTROL_RESET_COUNTER; ++i) {
        gtk_container_add(GTK_CONTAINER(menu), menu_items[i]);
        g_signal_connect(menu_items[i], "activate", G_CALLBACK(ui_datasette_tape_action_cb), GINT_TO_POINTER(i));
    }

    /* add "configure tapeport devices" */
    gtk_container_add(GTK_CONTAINER(menu), gtk_separator_menu_item_new());
    item = gtk_menu_item_new_with_label("Configure tapeport devices ...");
    g_signal_connect(item, "activate", G_CALLBACK(on_configure_activate),
            NULL);
    gtk_container_add(GTK_CONTAINER(menu), item);

    gtk_widget_show_all(menu);
    return menu;
}


/** \brief  Update sensitivity of the datasettet controls
 *
 * Disables/enables the datasette controls (keys), depending on whether the
 * datasette is enabled.
 *
 * \param[in,out]   menu    tape menu
 */
void ui_datasette_update_sensitive(GtkWidget *menu)
{
    int datasette;
    int y;
    GList *children;
    GList *controls;

    resources_get_int("Datasette", &datasette);

    /* get all children of the menu */
    children = gtk_container_get_children(GTK_CONTAINER(menu));
    /* skip 'Attach', 'Detach' and separator item */
    controls = g_list_nth(children, 3);

    for (y = 0; y <= DATASETTE_CONTROL_RESET_COUNTER; y++) {
        GtkWidget *item = controls->data;
        gtk_widget_set_sensitive(item, datasette);
        controls = g_list_next(controls);
    }

    /* free list of children */
    g_list_free(children);
}
