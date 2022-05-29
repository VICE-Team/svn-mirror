/** \file   uicommands.c
 * \brief   Simple commands triggered from the menu
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * TODO:    Delete this file once all UI actions are implemented and all
 *          function in this file are deprecated.
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

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "archdep.h"
#include "basedialogs.h"
#include "debug_gtk3.h"
#include "lib.h"
#include "log.h"
#include "uiactions.h"
#include "util.h"
#include "uiactions.h"
#include "uisettings.h"

#include "uicommands.h"


/******************************************************************************
 *    Event handlers, callbacks and helpers for CPU speed and FPS targets     *
 *****************************************************************************/

/** \brief  Handler for the 'delete-event' of a main window
 *
 * \param[in]   widget      window triggering the event (unused)
 * \param[in]   event       event details (unused)
 * \param[in]   user_data   extra data for the event (unused)
 *
 * \return  TRUE, if the function returns at all
 */
gboolean ui_main_window_delete_event(GtkWidget *widget,
                                     GdkEvent *event,
                                     gpointer user_data)
{
    ui_action_trigger(ACTION_QUIT);
    return TRUE;
}


/** \brief  Callback for the 'destroy' event of a main window
 *
 * \param[in]   widget      widget triggering the event
 * \param[in]   user_data   extra data for the callback (unused)
 */
void ui_main_window_destroy_callback(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *grid;

    /*
     * This should not be needed, destroying a GtkWindow should trigger
     * destruction of all widgets it contains.
     */
    debug_gtk3("Manually calling destroy() on the CRT widgets. This should not"
            " be necesarry, but right now it is.");
    grid = gtk_bin_get_child(GTK_BIN(widget));
    if (grid != NULL) {
        GtkWidget *crt = gtk_grid_get_child_at(GTK_GRID(grid), 0, 2);
        if (crt != NULL) {
            gtk_widget_destroy(crt);
        }
    }
}


/** \brief  Open the Manual
 *
 * Event handler for the 'Manual' menu item
 *
 * \param[in]   widget      parent widget triggering the event (unused)
 * \param[in]   user_data   extra event data (unused)
 *
 * \return  TRUE if opening the manual succeeded, FALSE otherwise
 *          (unreliable: gtk_show_uri_on_window() will return TRUE if the
 *           associated application could be openened but not the actual
 *           manual file)
 *
 * \note    Keep the debug_gtk3() calls for now, this code hardly works on
 *          Windows at all and needs work.
 */
gboolean ui_open_manual_callback(GtkWidget *widget, gpointer user_data)
{
    GError *error = NULL;
    gboolean res;
    char *uri;
    const char *path;
    gchar *final_uri;

    /*
     * Get arch-dependent documentation dir (doesn't contain the HTML docs
     * on Windows, but that's an other issue to fix.
     */
    path = archdep_get_vice_docsdir();

    /* first try opening the pdf */
    uri = util_join_paths(path, "vice.pdf", NULL);
    debug_gtk3("URI before GTK3: %s", uri);
    final_uri = g_filename_to_uri(uri, NULL, &error);
    debug_gtk3("final URI (pdf): %s", final_uri);
    if (final_uri == NULL) {
        /*
         * This is a fatal error, if a proper URI can't be built something is
         * wrong and should be looked at. This is different from failing to
         * load the PDF or not having a program to show the PDF
         */
        log_error(LOG_ERR, "failed to construct a proper URI from '%s',"
                " not trying the HTML fallback, this is an error that"
                " should not happen.",
                uri);
        g_clear_error(&error);
        lib_free(uri);
        return FALSE;
    }

    debug_gtk3("pdf uri: '%s'.", final_uri);

    /* NOTE:
     *
     * On Windows this at least opens Acrobat reader with a file-not-found
     * error message, any other URI/path given to this call results in a
     * "Operation not supported" message, which doesn't help much.
     *
     * Since Windows (or perhaps Gtk3 on Windows) fails, I've removed the
     * Windows-specific code that didn't work anyway
     */
    res = gtk_show_uri_on_window(NULL, final_uri, GDK_CURRENT_TIME, &error);
    if (!res) {
        /* will contain the args for the archep_spawn() call */
        char *args[3];
        char *tmp_name;

        debug_gtk3("gtk_show_uri_on_window Failed!");

        /* fallback to xdg-open */
        args[0] = lib_strdup("xdg-open");
        args[1] = lib_strdup(uri);
        args[2] = NULL;

        debug_gtk3("Calling xgd-open");
        if (archdep_spawn("xdg-open", args, &tmp_name, NULL) < 0) {
            debug_gtk3("xdg-open Failed!");
            vice_gtk3_message_error(
                    "Failed to load PDF",
                    "Error message: %s",
                    error != NULL ? error->message : "<no message>");
        } else {
            debug_gtk3("OK");
            res = TRUE;
        }
        /* clean up */
        lib_free(args[0]);
        lib_free(args[1]);
    }

    lib_free(uri);
    g_free(final_uri);
    g_clear_error(&error);

    return res;
}


/** \brief  Show settings dialog with hotkeys node activated
 *
 * \param[in]   widget  parent widget (unused)
 * \param[in]   data    extra event data (unused)
 *
 * \return  TRUE to signal the accelerator event has been consumed.
 */
gboolean ui_popup_hotkeys_settings(GtkWidget *widget, gpointer data)
{
    ui_settings_dialog_create_and_activate_node("host/hotkeys");
    return TRUE;
}
