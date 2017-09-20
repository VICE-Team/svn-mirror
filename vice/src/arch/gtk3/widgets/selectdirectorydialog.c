/** \file   src/arch/gtk3/widgets/selectdirectorydialog.c
 * \brief   Wrapper around the GtkFileChooser dialog for simple cases
 *
 * GTK3 directory selection dialog
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

#include "selectdirectorydialog.h"


/** \brief  Create a 'save file' dialog
 *
 * \param[in]   widget          parent widget
 * \param[in]   title           dialog title
 * \param[in]   proposed        proposed directory name (optional)
 * \param[in]   allow_create    allow creating a new directory
 *
 * \return  dircetory name or `NULL` on cancel
 *
 * \note    the directory name returned is allocated by GLib and needs to be
 *          freed after use with g_free()
 */
gchar *ui_select_directory_dialog(
        GtkWidget *widget,
        const char *title,
        const char *proposed,
        gboolean allow_create)
{
    GtkWidget *dialog;
    GtkWidget *parent;
    gint result;
    gchar *filename;

    parent = gtk_widget_get_toplevel(widget);

    dialog = gtk_file_chooser_dialog_new(
            title,
            GTK_WINDOW(parent),
            allow_create
                ? GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER
                : GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
            "Select", GTK_RESPONSE_ACCEPT,
            "Cancel", GTK_RESPONSE_REJECT,
            NULL, NULL);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));

    /* set proposed file name, if any */
    if (proposed != NULL && *proposed != '\0') {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), proposed);
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
