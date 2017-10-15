/** \file   src/arch/gtk3/uicmdline.c
 * \brief   Dialog to display command line options
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
 *
 */

#include "vice.h"

#include <gtk/gtk.h>

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "cmdline.h"

#include "uicmdline.h"


/** \brief  Create textview with scrollbars
 *
 * \return  GtkScrolledWindow
 */
static GtkWidget *create_content_widget(void)
{
    GtkWidget *view;
    GtkWidget *scrolled;
    GtkTextBuffer *buffer;
    int num_options = cmdline_get_num_options();
    int i;

    view = gtk_text_view_new();
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrolled, 800, 600);
    gtk_container_add(GTK_CONTAINER(scrolled), view);

    for (i = 0; i < num_options; i++) {
        char *name;
        char *param;
        char *desc;

        name = cmdline_options_get_name(i);
        param = cmdline_options_get_param(i);
        desc = cmdline_options_get_description(i);

        gtk_text_buffer_insert_at_cursor(buffer, name, -1);
        if (param != NULL) {
            gtk_text_buffer_insert_at_cursor(buffer, " ", -1);
            gtk_text_buffer_insert_at_cursor(buffer, param, -1);
        }
        gtk_text_buffer_insert_at_cursor(buffer, "\n", -1);
        gtk_text_buffer_insert_at_cursor(buffer, "\t", -1);
        gtk_text_buffer_insert_at_cursor(buffer, desc, -1);
        gtk_text_buffer_insert_at_cursor(buffer, "\n\n", -1);

    }

    gtk_widget_show_all(scrolled);
    return scrolled;
}


/** \brief  Show list of command line options
 *
 * \param[in]   widget      parent widget
 * \param[in]   user_data   extra data (unused)
 */
void uicmdline_dialog_show(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog;
    GtkWidget *content;

    dialog = gtk_dialog_new_with_buttons("Command line options",
            GTK_WINDOW(gtk_widget_get_toplevel(widget)), GTK_DIALOG_MODAL,
            "Close", GTK_RESPONSE_ACCEPT,
            NULL);

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_box_pack_start(GTK_BOX(content), create_content_widget(), TRUE, TRUE, 0);


    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gtk_widget_destroy(dialog);
    }
}
