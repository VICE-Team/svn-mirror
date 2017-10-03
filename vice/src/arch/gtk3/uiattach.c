/*
 * uiattach.c - GTK3 attach dialog
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
#include <stdio.h>
#include <stdlib.h>

#include "attach.h"
#include "lib.h"
#include "vsync.h"

#include "uiattach.h"


/** \brief  Custom response IDs for the dialog
 */
enum {
    VICE_RESPONSE_AUTOSTART = 1 /**< autostart button clicked (not implemented) */
};


/** \brief  Read-Only flag opening images/archives
 */
static gboolean readonly_mode = 0;


/** \brief  Handler for "toggle" event of the 'readonly' toggle button
 *
 * \param[in,out]   widget      widget triggering the event
 * \param[in]       user_data   data for the event (unused)
 */
static void readonly_callback(GtkWidget *widget, gpointer user_data)
{
    gboolean active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
#ifdef HAVE_DEBUG_GTK3UI
    g_print("[debug-gtk3ui] %s(): readonly = %d\n", __func__, (int)active);
#endif
    readonly_mode = active;
}


/** \brief  Create widget for extra options such as 'readonly'
 *
 * \return  extra widget
 */
static GtkWidget *create_extra_widget(void)
{
    GtkWidget *grid;
    GtkWidget *readonly;

    grid = gtk_grid_new();

    readonly = gtk_toggle_button_new_with_label("Read only");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(readonly), FALSE);
    gtk_grid_attach(GTK_GRID(grid), readonly, 0, 0, 1, 1);
    gtk_widget_show(grid);
    gtk_widget_show(readonly);

    g_signal_connect(readonly, "toggled", G_CALLBACK(readonly_callback), NULL);
    return grid;
}


/** \brief  Attach an image/archive to VICE
 *
 * \param[in,out]   widget  the FileChooser dialog
 */
static void do_attach(GtkWidget *widget)
{
    char *filename;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
    g_print("Got filename \"%s\"\n", filename);

    if (file_system_attach_disk(8, filename) < 0) {
        // ui_error("Invalid disk image");
    }

    g_free(filename);
}


/** \brief  Handler for the "response" event of the FileChooser dialog
 *
 * This is where the magic should happen: handle button/key presses and decide
 * what to do.
 *
 * \param[in,out]   widget      file chooser dialog
 * \param[in]       response_id response ID
 * \param[in]       user_data   extra data (unused)
 */
static void response_callback(GtkWidget *widget, gint response_id,
                              gpointer user_data)
{
#ifdef HAVE_DEBUG_GTK3UI
    g_print("[debug-gtk3ui] %s(): response ID = %d\n", __func__,response_id);
#endif

    vsync_suspend_speed_eval();

    switch (response_id) {
        /* accepted */
        case GTK_RESPONSE_ACCEPT:
#ifdef HAVE_DEBUG_GTK3UI
            g_print("[debug-gtk3ui: %s(): OPEN clicked\n", __func__);
#endif
            do_attach(widget);
            gtk_widget_destroy(widget);
            break;

        case GTK_RESPONSE_REJECT:
            /* canceled */
            gtk_widget_destroy(widget);
            break;
        default:
#ifdef HAVE_DEBUG_GTK3UI
            g_print("[debug-gtk3ui: %s(): ID %d unhandled\n",
                    __func__, response_id);
#endif
            break;
    }
}


/** \brief  Create attach dialog
 *
 * \param[in]   widget      widget triggering event (the menu item)
 * \param[in]   user_data   data for the even (unused)
 *
 * \todo        add event handler to determine which type of image/archive is
 *              being attached to call the proper attach handler
 */
void ui_attach_dialog_callback(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog;
    GtkWidget *window;
    /* file filters */
    GtkFileFilter *disk_filter;
    GtkFileFilter *zipcode_filter;
    GtkFileFilter *tape_filter;
    GtkFileFilter *archive_filter;
    GtkFileFilter *cart_filter;

#ifdef HAVE_DEBUG_GTK3UI
    g_print("[debug-gtk3ui] %s() called\n", __func__);
#endif

    /* get parent window, Gtk doesn't like dialogs without parents */
    window = gtk_widget_get_toplevel(widget);
    if (!gtk_widget_is_toplevel(window)) {
        window = NULL;  /* the gtk_widget_get_toplevel() function returns the
                           widget itself if it can't find a topleve widget,
                           which would screw the following call, so set it to
                           NULL and (temporarily) live with being orphaned */
    }

    /* create an 'open file' dialog */
    dialog = gtk_file_chooser_dialog_new(
            "Attach disk/tape/car/archive",
            GTK_WINDOW(window),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            "Open", GTK_RESPONSE_ACCEPT,
            "Autostart", VICE_RESPONSE_AUTOSTART,
            "Cancel", GTK_RESPONSE_REJECT,
            NULL, NULL);

    /* add extra widget for 'readonly' and stuff */
    gtk_file_chooser_set_extra_widget(GTK_FILE_CHOOSER(dialog),
            create_extra_widget());

    /* add disk file type filters
     * XXX: just for testing, more filters should be added
     */
    disk_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(disk_filter, "Disk images");
    gtk_file_filter_add_pattern(disk_filter, "*.d64");
    gtk_file_filter_add_pattern(disk_filter, "*.d71");
    gtk_file_filter_add_pattern(disk_filter, "*.d81");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), disk_filter);

    /* add zipdisk/zipfile/zipsix archives */
    zipcode_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(zipcode_filter, "ZipCode archives");
    gtk_file_filter_add_pattern(zipcode_filter, "1!*");
    gtk_file_filter_add_pattern(zipcode_filter, "1!!*");
    gtk_file_filter_add_pattern(zipcode_filter, "a!*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), zipcode_filter);

    /* add tape images */
    tape_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(tape_filter, "Tape images");
    gtk_file_filter_add_pattern(tape_filter, "*.tap");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), tape_filter);

    /* add archives */
    archive_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(archive_filter, "Archives");
    gtk_file_filter_add_pattern(archive_filter, "*.awk");
    gtk_file_filter_add_pattern(archive_filter, "*.lnx");
    gtk_file_filter_add_pattern(archive_filter, "*.t64");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), archive_filter);

    /* add cartridges */
    cart_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(cart_filter, "Cartridges");
    gtk_file_filter_add_pattern(cart_filter, "*.crt");
    /* unlikely to work properly */
    gtk_file_filter_add_pattern(cart_filter, "*.bin");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), cart_filter);



    gtk_file_filter_add_pattern(tape_filter, "*.t64");
    /* hook up the event handler that does the actual work */
    g_signal_connect(dialog, "response", G_CALLBACK(response_callback), NULL);

    gtk_widget_show(dialog);
}
