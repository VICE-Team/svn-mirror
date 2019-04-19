/** \file   vsidmainwidget.c
 * \brief   GTK3 main widget for VSD
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

#include <stdlib.h>
#include <gtk/gtk.h>

#include "vice_gtk3.h"
#include "debug.h"
#include "machine.h"
#include "lib.h"
#include "log.h"
#include "ui.h"
#include "uivsidmenu.h"
#include "uivsidwindow.h"
#include "vsidtuneinfowidget.h"
#include "vsidcontrolwidget.h"
#include "vsidmixerwidget.h"
#include "vsidplaylistwidget.h"
#include "hvscstilwidget.h"

#include "vsidmainwidget.h"

static GtkWidget *main_widget;

static GtkWidget *left_pane;

static GtkWidget *tune_info_widget;
static GtkWidget *control_widget;
static GtkWidget *mixer_widget;
static GtkWidget *stil_widget;
static GtkWidget *playlist_widget;


/** \brief  Handler for the 'drag-drop' event of the GtkWindow
 *
 * Can be used to filter certain drop targets or altering the data before
 * triggering the 'drag-drop-received' event. Currently just returns TRUE
 *
 * \param[in]   widget  widget triggering the event
 * \param[in]   context gtk drag context
 * \param[in]   x       x position of drag event
 * \param[in]   y       y position of drag event
 * \param[in]   time    (I don't have a clue)
 * \param[in]   data    extra event data (unused)
 *
 * \return  TRUE
 */
static gboolean on_drag_drop(
        GtkWidget *widget,
        GdkDragContext *context,
        gint x,
        gint y,
        guint time,
        gpointer data)
{
    debug_gtk3("called.");
    return TRUE;
}


/** \brief  Handler for the 'drag-data-received' event
 *
 * Autostarts a SID file.
 *
 * \param[in]   widget      widget triggering the event (unused)
 * \param[in]   context     drag context (unused)
 * \param[in]   x           probably X-coordinate in the drop target?
 * \param[in]   y           probablt Y-coordinate in the drop target?
 * \param[in]   data        dragged data
 * \param[in]   info        int declared in the targets array (unclear)
 * \param[in]   time        no idea
 *
 * \todo    Once this works properly, remove a lot of debugging calls, perhaps
 *          changing a few into log calls.
 *
 * \todo    Keep list of multiple files/URIs so we can add multiple SID files
 *          to the playlist without a lot of code duplication.
 *
 * \todo    Figure out why the drop event on the STIL widget only triggers
 *          *outside* the textbox.
 */
static void on_drag_data_received(
        GtkWidget *widget,
        GdkDragContext *context,
        int x,
        int y,
        GtkSelectionData *data,
        guint info,
        guint time)
{
    gchar **uris;
    gchar *filename = NULL;
    gchar **files = NULL;
    guchar *text = NULL;
    int i;

    debug_gtk3("got drag-data, info = %u:", info);
    if (widget == left_pane) {
        debug_gtk3("got data for 'left_pane'.");
    } else if (widget == stil_widget) {
        debug_gtk3("got data for 'stil_widget'.");
    } else if (widget == playlist_widget) {
        debug_gtk3("got data for 'playlist_widget.");
    } else {
        debug_gtk3("got data for unhandled widget.");
        return;
    }

    if (widget == left_pane || widget == stil_widget) {
        switch (info) {

            case DT_URI_LIST:
                /*
                 * This branch appears to be taken on both Windows and macOS.
                 */

                /* got possible list of URI's */
                uris = gtk_selection_data_get_uris(data);
                if (uris != NULL) {
                    /* dump URI's on stdout */
                    debug_gtk3("got URI's:");
                    for (i = 0; uris[i] != NULL; i++) {

                        debug_gtk3("URI: '%s'\n", uris[i]);
                        filename = g_filename_from_uri(uris[i], NULL, NULL);
                        debug_gtk3("filename: '%s'.", filename);
                        if (filename != NULL) {
                            g_free(filename);
                        }
                    }

                    /* use the first/only entry as the autostart file
                     *
                     * XXX: perhaps add any additional files to the fliplist
                     *      if Dxx?
                     */
                    if (uris[0] != NULL) {
                        filename = g_filename_from_uri(uris[0], NULL, NULL);
                    } else {
                        filename = NULL;
                    }

                    g_strfreev(uris);
                }
                break;

            case DT_TEXT:
                /*
                 * this branch appears to be taken on both Gtk and Qt based WM's
                 * on Linux
                 */


                /* text will contain a newline separated list of 'file://' URIs,
                 * and a trailing newline */
                text = gtk_selection_data_get_text(data);
                /* remove trailing whitespace */
                g_strchomp((gchar *)text);

                debug_gtk3("Got data as text: '%s'.", text);
                files = g_strsplit((const gchar *)text, "\n", -1);
                g_free(text);

                for (i = 0; files[i] != NULL; i++) {
#ifdef HAVE_DEBUG_GTK3UI
                    gchar *tmp = g_filename_from_uri(files[i], NULL, NULL);
#endif
                    debug_gtk3("URI: '%s', filename: '%s'.",
                            files[i], tmp);
                }
                /* now grab the first file */
                filename = g_filename_from_uri(files[0], NULL, NULL);
                g_strfreev(files);

                debug_gtk3("got filename '%s'.", filename);
                break;

            default:
                debug_gtk3("Warning: unhandled d'n'd target %u.", info);
                filename = NULL;
                break;
        }

        /* can we attempt autostart? */
        if (filename != NULL) {
            debug_gtk3("Attempting to autostart '%s'.", filename);
            if (ui_vsid_window_load_psid(filename) != 0) {
                debug_gtk3("failed.");
            } else {
                debug_gtk3("OK!");
            }
            g_free(filename);
        }
    }
}


/** \brief  Create VSID main widget
 *
 * \return  GtkGrid
 */
GtkWidget *vsid_main_widget_create(void)
{
    GtkWidget *grid;

    grid = vice_gtk3_grid_new_spaced(32, 32);
    g_object_set(G_OBJECT(grid),
            "margin-left", 16,
            "margin-right", 16,
            "margin-top", 16,
            "margin-bottom", 16,
            NULL);

    /* left pane: info, playback controls, mixer */
    left_pane = vice_gtk3_grid_new_spaced(0, 16);

    tune_info_widget = vsid_tune_info_widget_create();
    gtk_grid_attach(GTK_GRID(left_pane), tune_info_widget, 0, 0, 1, 1);

    control_widget = vsid_control_widget_create();
    gtk_grid_attach(GTK_GRID(left_pane), control_widget, 0, 1, 1, 1);

    mixer_widget = vsid_mixer_widget_create();
    gtk_grid_attach(GTK_GRID(left_pane), mixer_widget, 0, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), left_pane, 0, 0, 1, 1);

    /* middle pane: STIL widget */
    stil_widget = hvsc_stil_widget_create();
    gtk_widget_set_vexpand(stil_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), stil_widget, 1, 0, 1, 1);

    /* right pane: playlist */
    playlist_widget = vsid_playlist_widget_create();
    gtk_grid_attach(GTK_GRID(grid), playlist_widget, 2, 0, 1, 1);


    /*
     * Set up drag-n-drop handlers
     */

    /* left pane: info, playback controls, mixer */
    gtk_drag_dest_set(
            left_pane,
            GTK_DEST_DEFAULT_ALL,
            ui_drag_targets,
            UI_DRAG_TARGETS_COUNT,
            GDK_ACTION_COPY);
    g_signal_connect(left_pane, "drag-data-received",
                     G_CALLBACK(on_drag_data_received), NULL);
    g_signal_connect(left_pane, "drag-drop",
                     G_CALLBACK(on_drag_drop), NULL);

    /* middle pane: STIL widget */
    gtk_drag_dest_set(
            stil_widget,
            GTK_DEST_DEFAULT_ALL,
            ui_drag_targets,
            UI_DRAG_TARGETS_COUNT,
            GDK_ACTION_COPY);
    g_signal_connect(stil_widget, "drag-data-received",
                     G_CALLBACK(on_drag_data_received), NULL);
    g_signal_connect(stil_widget, "drag-drop",
                     G_CALLBACK(on_drag_drop), NULL);

    /* right pane: playlist */
    gtk_drag_dest_set(
            playlist_widget,
            GTK_DEST_DEFAULT_ALL,
            ui_drag_targets,
            UI_DRAG_TARGETS_COUNT,
            GDK_ACTION_COPY);
    g_signal_connect(playlist_widget, "drag-data-received",
                     G_CALLBACK(on_drag_data_received), NULL);
    g_signal_connect(playlist_widget, "drag-drop",
                     G_CALLBACK(on_drag_drop), NULL);




    gtk_widget_show_all(grid);



    main_widget = grid;
    return grid;
}


/** \brief  Set number of tunes
 *
 * \param[in]   n   tune count
 */
void vsid_main_widget_set_tune_count(int n)
{
    vsid_control_widget_set_tune_count(n);
    vsid_tune_info_widget_set_tune_count(n);
}

/** \brief  Set current tune
 *
 * \param[in]   n   tune number
 */
void vsid_main_widget_set_tune_current(int n)
{

    vsid_control_widget_set_tune_current(n);
    vsid_tune_info_widget_set_tune_current(n);

    /* update mixer widget to use the SID model of the current tune */
    if (mixer_widget != NULL) {

        GtkWidget *left = gtk_grid_get_child_at(
                GTK_GRID(main_widget), 0, 0);

        gtk_widget_destroy(mixer_widget);
        mixer_widget = vsid_mixer_widget_create();

        gtk_grid_attach(GTK_GRID(left), mixer_widget, 0, 2, 1, 1);
    }
}


/** \brief  Set default tune
 *
 * \param[in]   n   tune number
 */
void vsid_main_widget_set_tune_default(int n)
{
    vsid_control_widget_set_tune_default(n);
    vsid_tune_info_widget_set_tune_default(n);
}
