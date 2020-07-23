/** \file   settings_keyboard.c
 * \brief   GTK3 keyboard settings main widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES KbdStatusbar    all
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
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "vice_gtk3.h"
#include "keyboard.h"
#include "lib.h"
#include "util.h"
#include "ui.h"
#include "resources.h"
#include "vsync.h"

#include "kbdmappingwidget.h"
#include "kbdlayoutwidget.h"
#include "uistatusbar.h"

#include "settings_keyboard.h"


/** \brief  Toggle statusbar keyboard debugging widget display
 *
 *
 * \param[in]   widget  toggle button
 * \param[in]   data    extra event data (unused)
 */
static void on_kbd_debug_toggled(GtkWidget *widget, gpointer data)
{
    ui_statusbar_set_kbd_debug(
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}


static void on_save_filename(GtkDialog *dialog, char *filename)
{
    if (filename != NULL && *filename != '\0') {
        /* we got something at least */

        util_add_extension(&filename, "vkm");

        int oops = keyboard_keymap_dump(filename);
        if (oops == 0) {
            vice_gtk3_message_info("Succesfully saved current keymap",
                    "Wrote current keymap as '%s'.", filename);
        } else {
            vice_gtk3_message_error("Failed to save custom keymap",
                    "Error %d: %s", errno, strerror(errno));
        }
        if (filename != NULL) {
            lib_free(filename);
        }
    }
}


/** \brief  Write current keymap to host system
 *
 * \param[in]   widget  button triggering the event (ignored)
 * \param[in]   data    extra event data (ignored)
 */
static void on_save_custom_keymap_clicked(GtkWidget *widget, gpointer data)
{
/*    vice_gtk3_message_info("Save current keymap", "Nothing to see here..."); */
    vice_gtk3_save_file_dialog(
            NULL,
            "Save current keymap",  /* title */
            NULL,                   /* proposed filename: might use this later */
            TRUE,                   /* query user before overwrite */
            NULL,                   /* base path, maybe ~ ?) */
            on_save_filename        /* filename callback */
         );
}


/** \brief  Create keyboard settings widget
 *
 * \param[in]   widget  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *settings_keyboard_widget_create(GtkWidget *widget)
{
    GtkWidget *layout;
    GtkWidget *mapping_widget;
    GtkWidget *layout_widget;
    GtkWidget *kbdstatusbar;
    GtkWidget *custom_button;

    layout = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    mapping_widget = kbdmapping_widget_create(widget);
    gtk_grid_attach(GTK_GRID(layout), mapping_widget, 0, 0, 1, 1);

    layout_widget = kbdlayout_widget_create();
    g_object_set(G_OBJECT(layout_widget), "margin-top", 32, NULL);

    gtk_grid_attach(GTK_GRID(layout), layout_widget, 0, 1, 1, 1);

    /* Add button to save current (perhaps altered) keymap */
    custom_button = gtk_button_new_with_label("Save current keymap");
    g_signal_connect(custom_button, "clicked",
            G_CALLBACK(on_save_custom_keymap_clicked), NULL);
    gtk_grid_attach(GTK_GRID(layout), custom_button, 0, 2, 1, 1);
    g_object_set(custom_button, "margin-top", 16, NULL);

    kbdstatusbar = vice_gtk3_resource_check_button_new("KbdStatusbar",
            "Enable keyboard debugging on statusbar");
    gtk_grid_attach(GTK_GRID(layout), kbdstatusbar, 0, 3, 1, 1);
    g_signal_connect(kbdstatusbar, "toggled", G_CALLBACK(on_kbd_debug_toggled),
            NULL);
    g_object_set(kbdstatusbar, "margin-top", 16, NULL);
    gtk_widget_show_all(layout);

    /* update widget so sym/pos is greyed out correctly */
    kbdmapping_widget_update();

    return layout;
}
