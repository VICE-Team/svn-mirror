/*
 * fileopendialog.c - GTK3 file open dialog
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

#include "debug_gtk3.h"


#include "openfiledialog.h"

static void (*user_callback)(GtkWidget *widget, void *data);


static void response_callback(GtkWidget *widget, gpointer user_data)
{
    int response_id = GPOINTER_TO_INT(user_data);
    char *filename = NULL;

    debug_gtk3("response ID = %d\n", response_id);

    if (response_id == GTK_RESPONSE_ACCEPT) {
        filename  = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
    }
    gtk_widget_destroy(widget);
    user_callback(widget, filename);
}



void ui_open_file_dialog_create(
        GtkWidget *widget,
        const char *title,
        const char *filter_desc,
        const char **filter_list,
        void (*callback)(GtkWidget *, void *))
{
    GtkWidget *dialog;
    GtkWidget *parent = NULL;
    GtkFileFilter *filter;
    size_t i = 0;

    user_callback = callback;

#if 0
    parent = gtk_widget_get_toplevel(widget);
    if (!gtk_widget_is_toplevel(parent)) {
        parent = NULL;
    }
#endif
    dialog = gtk_file_chooser_dialog_new(
            title,
            GTK_WINDOW(parent),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            "Open", GTK_RESPONSE_ACCEPT,
            "Cancel", GTK_RESPONSE_REJECT,
            NULL, NULL);

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, filter_desc);
    for (i = 0; filter_list[i] != NULL; i++) {
        gtk_file_filter_add_pattern(filter, filter_list[i]);
    }
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    g_signal_connect(dialog, "response", G_CALLBACK(response_callback), NULL);
    gtk_widget_show(dialog);
    g_print("FILE DIALOG SHOWN\n");
}




