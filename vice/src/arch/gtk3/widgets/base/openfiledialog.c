/** \file   src/arch/gtk3/widgets/openfiledialog.c
 * \brief   Wrapper around the GtkFileChooser dialog for simple cases
 *
 * openfiledialog.c - GTK3 file open dialog
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
#include "ui.h"

#include "openfiledialog.h"


/** \brief  Create an 'open file' dialog
 *
 * \param[in]   widget      parent widget
 * \param[in]   title       dialog title
 * \param[in]   filter_desc file filter name
 * \param[in]   filter_list list of file type filters, NULL-terminated
 * \param[in]   path        directory to use (optional)
 *
 * \return  filename or `NULL` on cancel
 *
 * \note    the filename returned is allocated by GLib and needs to be freed
 *          after use with g_free()
 */
gchar *ui_open_file_dialog(
        GtkWidget *widget,
        const char *title,
        const char *filter_desc,
        const char **filter_list,
        const char *path)
{
    GtkWidget *dialog;
    GtkFileFilter *filter;
    GtkWindow *parent;
    gint result;
    gchar *filename;

    parent = ui_get_active_window();

    dialog = gtk_file_chooser_dialog_new(
            title,
            parent,
            GTK_FILE_CHOOSER_ACTION_OPEN,
            "Open", GTK_RESPONSE_ACCEPT,
            "Cancel", GTK_RESPONSE_REJECT,
            NULL, NULL);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

    /* create * filter */
    filter = create_file_chooser_filter(file_chooser_filter_all, TRUE);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    /* add extra filter if given */
    if (filter_desc != NULL && filter_list != NULL) {
        ui_file_filter_t temp = { filter_desc, filter_list };
        filter = create_file_chooser_filter(temp, TRUE);
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
        gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);
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



/** \brief  Create a 'open or create file' dialog
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
gchar *ui_open_create_file_dialog(
        GtkWidget *widget,
        const char *title,
        const char *proposed,
        gboolean confirm,
        const char *path)
{
    GtkWidget *dialog;
    GtkWindow *parent;
    gint result;
    gchar *filename;

    parent = ui_get_active_window();

    dialog = gtk_file_chooser_dialog_new(
            title,
            parent,
            GTK_FILE_CHOOSER_ACTION_SAVE,
            "Open/Create", GTK_RESPONSE_ACCEPT,
            "Cancel", GTK_RESPONSE_REJECT,
            NULL, NULL);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

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
