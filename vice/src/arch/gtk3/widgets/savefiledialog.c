/** \file   src/arch/gtk3/widgets/savefiledialog.c
 * \brief   Wrapper around the GtkFileChooser dialog for simple cases
 *
 * savefiledialog.c - GTK3 save file dialog
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
#include "filechooserhelpers.h"

#include "savefiledialog.h"


/** \brief  Create a 'save file' dialog
 *
 * \param[in]   widget      parent widget
 * \param[in]   title       dialog title
 * \param[in]   proposed    proposed file name (optional)
 * \param[in]   confirm     confirm overwriting an existing file
 * \param[in]   path        set starting directory (optional)
 *
 * \return  filename or `NULL` on cancel
 *
 * \note    the filename returned is allocated by GLib and needs to be freed
 *          after use with g_free()
 */
gchar *ui_save_file_dialog(
        GtkWidget *widget,
        const char *title,
        const char *proposed,
        gboolean confirm,
        const char *path)
{
    GtkWidget *dialog;
    GtkWidget *parent;
    gint result;
    gchar *filename;

    parent = gtk_widget_get_toplevel(widget);

    dialog = gtk_file_chooser_dialog_new(
            title,
            GTK_WINDOW(parent),
            GTK_FILE_CHOOSER_ACTION_SAVE,
            "Save", GTK_RESPONSE_ACCEPT,
            "Cancel", GTK_RESPONSE_REJECT,
            NULL, NULL);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));

    /* set overwrite confirmation */
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog),
            confirm);

    /* set proposed file name, if any */
    if (proposed != NULL && *proposed != '\0') {
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), proposed);
    }

    /* change directory if specified */
    if (path != NULL && *path != '\0') {
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
    }

    result = gtk_dialog_run(GTK_DIALOG(dialog));
    if (result == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    } else {
        filename = NULL;
    }
    gtk_widget_destroy(dialog);
    return filename;
}
