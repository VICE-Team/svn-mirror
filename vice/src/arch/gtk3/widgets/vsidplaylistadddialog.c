/** \file   vsidplaylistadddialog.c
 * \brief   GTK3 Add SID files dialog for the playlist widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
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

#include <gtk/gtk.h>

#include "vice_gtk3.h"
#include "debug_gtk3.h"
#include "filechooserhelpers.h"
#include "ui.h"

#include "vsidplaylistadddialog.h"


static void (*dialog_cb)(GSList *) = NULL;


static void on_response(GtkDialog *dialog, gint response_id, gpointer data)
{
    debug_gtk3("called with response_id %d.", response_id);

    if (response_id == GTK_RESPONSE_ACCEPT) {
        GSList *files = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
        dialog_cb(files);
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
}


static GtkWidget *vsid_playlist_add_dialog_create(void)
{
    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new(
            "Open SID file",
            ui_get_active_window(),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            "Cancel", GTK_RESPONSE_CANCEL,
            "Open", GTK_RESPONSE_ACCEPT,
            NULL);

    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);

    g_signal_connect(dialog, "response", G_CALLBACK(on_response), NULL);
    return dialog;
}


void vsid_playlist_add_dialog_exec(void (*callback)(GSList *files))
{
    GtkWidget *dialog = vsid_playlist_add_dialog_create();
    dialog_cb = callback;

    gtk_widget_show(dialog);
}
