/*
 * kbdmappingwidget.c - GTK3 keyboard mapping widget for the settings dialog
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
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
 */


#include "vice.h"

#include <gtk/gtk.h>

#include "lib.h"
#include "ui.h"
#include "resources.h"
#include "vsync.h"
#include "widgethelpers.h"

#include "debug_gtk3.h"

#include "kbdmappingwidget.h"



/** \brief  Keyboard mapping types
 */
static ui_text_int_pair_t mappings[] = {
    { "Symbolic mapping", 0 },
    { "Positional mapping", 1 },
    { "Symbolic mapping (User)", 2 },
    { "Positional mapping (User)", 3 },
    { NULL, -1 }
};


/** \brief  Temporary implemention: display message and exit
 *
 * This should be a 'open file' dialog, but since I'm working on a helper
 * widget to handle all sorts of VICE files, this isn't implemented yet.
 *
 * \param[in]   title   open file dialog title
 */
static char *get_file_dialog(const char *title)
{
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Sorry, '%s' not implemented yet", title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return NULL;
}


static void on_mapping_changed(GtkWidget *widget, gpointer user_data)
{
    int index = GPOINTER_TO_INT(user_data);

    debug_gtk3("setting mapping to %d\n", index);
    resources_set_int("KeymapIndex", index);
}


static void on_button_symbolic_clicked(GtkWidget *widget, gpointer user_data)
{
    debug_gtk3("clicked\n");

    get_file_dialog("Select symbolic keymap");
}


static void on_button_positional_clicked(GtkWidget *widget, gpointer user_data)
{
    debug_gtk3("clicked\n");

    get_file_dialog("Select positional keymap");
}




/** \brief  Create a keyboard mapping selection widget
 *
 * \return  GtkWidget
 *
 * \fixme   I'm not really satisfied with the 'select file' buttons, perhaps
 *          they should be placed next to the radio buttons?
 */
GtkWidget *create_kbdmapping_widget(void)
{
    GtkWidget *layout;
    GtkWidget *btn_sym;
    GtkWidget *btn_pos;
    int index = 0;

    resources_get_int("KeymapIndex", &index);

    /* create grid with label and four radio buttons */
    layout = uihelpers_create_int_radiogroup_with_label(
            "Keyboard mapping", mappings, on_mapping_changed, index);

    btn_sym = gtk_button_new_with_label("Select file ...");
    g_signal_connect(btn_sym, "clicked",
            G_CALLBACK(on_button_symbolic_clicked), NULL);
    g_object_set(btn_sym, "margin-left", 32, NULL);

    gtk_grid_insert_row(GTK_GRID(layout), 4);
    gtk_grid_attach(GTK_GRID(layout), btn_sym, 0, 4, 1, 1);


    btn_pos = gtk_button_new_with_label("Select file ...");
    g_signal_connect(btn_pos, "clicked",
            G_CALLBACK(on_button_positional_clicked), NULL);

    g_object_set(btn_pos, "margin-left", 32, NULL);
    gtk_grid_attach(GTK_GRID(layout), btn_pos, 0, 6, 1, 1);


    gtk_widget_show_all(layout);
    return layout;
}


