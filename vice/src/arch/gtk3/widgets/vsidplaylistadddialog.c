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
#include "lastdir.h"
#include "ui.h"

#include "vsidplaylistadddialog.h"


/** \brief  callback function of the dialog
 */
static void (*dialog_cb)(GSList *) = NULL;


static char *last_used_dir = NULL;



static void on_destroy(GtkWidget *widget, gpointer data)
{
}



/** \brief  Handler for the 'response' event of the dialog
 *
 * Calls the registered callback with a list of files.
 *
 * \param[in]   dialog      dialog triggering the event
 * \param[in]   response_id ID of response of the \a dialog
 * \param[in]   data        extra event data (unused)
 */
static void on_response(GtkDialog *dialog, gint response_id, gpointer data)
{
    debug_gtk3("called with response_id %d.", response_id);

    if (response_id == GTK_RESPONSE_ACCEPT) {
        GSList *files = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
        char *first = files->data;
        gchar *dir = g_path_get_dirname(first);

        debug_gtk3("Setting lastdir to '%s'.", dir);
        lastdir_update_raw(dir, &last_used_dir);

        /* do not free dir, that gets freed in lastdir.c */

        dialog_cb(files);
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
}


/** \brief  Create GtkFileChooser instance to open SID files
 *
 * \return  GtkFileChooser
 */
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

    debug_gtk3("Setting last user dir to '%s'.", last_used_dir);
    lastdir_set(dialog, &last_used_dir);

    g_signal_connect(dialog, "response", G_CALLBACK(on_response), NULL);
    g_signal_connect(dialog, "destroy", G_CALLBACK(on_destroy), NULL);
    return dialog;
}


/** \brief  Run dialog to add SID files to the playlist
 *
 * \param[in]   callback    function accepting a list of selected files
 */
void vsid_playlist_add_dialog_exec(void (*callback)(GSList *files))
{
    GtkWidget *dialog = vsid_playlist_add_dialog_create();
    dialog_cb = callback;

    gtk_widget_show(dialog);
}


void vsid_playlist_add_dialog_free(void)
{
    lastdir_shutdown(&last_used_dir);
}
