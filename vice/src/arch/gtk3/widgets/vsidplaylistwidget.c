/** \file   vsidplaylistwidget.c
 * \brief   GTK3 playlist widget for VSID
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

#include "vsidplaylistwidget.h"


/** \brief  Control button types
 */
enum {
    CTRL_ACTION,    /**< action button: simple push button */
    CTRL_TOGGLE     /**< toggle button: for 'repeat' and 'shuffle' */
};


/** \brief  Playlist control button struct
 */
typedef struct plist_ctrl_button_s {
    const char *icon_name;  /**< icon-name in the Gtk3 theme */
    int         type;       /**< type of button (action/toggle) */
    void        (*callback)(GtkWidget *, gpointer); /**< callback function */
    const char *tooltip;    /**< tooltip text */
} plist_ctrl_button_t;


/** \brief  List of playlist controls
 */
static const plist_ctrl_button_t controls[] = {
    { "media-skip-backward", CTRL_ACTION,
        NULL,
        "Go to start of playlist" },
    { "media-seek-backward", CTRL_ACTION,
        NULL,
        "Go to previous tune" },
    { "media-seek-forward", CTRL_ACTION,
        NULL,
        "Go to next tune" },
    { "media-skip-forward", CTRL_ACTION,
        NULL,
        "Go to end of playlist" },
    { "media-playlist-repeat", CTRL_TOGGLE,
        NULL,
        "Repeat playlist" },
    { "media-playlist-shuffle", CTRL_TOGGLE,
        NULL,
        "Shuffle playlist" },
    { "list-add", CTRL_ACTION,
        NULL,
        "Add tune to playlist" },
    { "list-remove", CTRL_ACTION,
        NULL,
        "Remove selected tune from playlist" },
    { "document-open", CTRL_ACTION,
        NULL,
        "Open playlist file" },
    { "document-save", CTRL_ACTION,
        NULL,
        "Save playlist file" },
    { "edit-clear-all", CTRL_ACTION,
        NULL,
        "Clear playlist" },
    { NULL, 0, NULL, NULL }
};



/** \brief  Reference to the playlist model
 */
static GtkListStore *playlist_model;

/** \brief  Reference to the playlist view
 */
static GtkWidget *playlist_view;


/** \brief  Create playlist model
 */
static void vsid_playlist_model_create(void)
{
    playlist_model = gtk_list_store_new(1, G_TYPE_STRING);
}


/** \brief  Create playlist view widget
 *
 */
static void vsid_playlist_view_create(void)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    playlist_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(playlist_model));

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
            "Path",
            renderer,
            "text", 0,
            NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(playlist_view), column);
}


/** \brief  Create a grid with a list of buttons to control the playlist
 *
 * Most of the playlist should also be controllable via a context-menu
 * (ie mouse right-click), which is a big fat TODO now.
 *
 * \return  GtkGrid
 */
static GtkWidget *vsid_playlist_controls_create(void)
{
    GtkWidget *grid;
    int i;

    grid = vice_gtk3_grid_new_spaced(0, VICE_GTK3_DEFAULT);
    for (i = 0; controls[i].icon_name != NULL; i++) {
        GtkWidget *button;

        button = gtk_button_new_from_icon_name(
                controls[i].icon_name,
                GTK_ICON_SIZE_LARGE_TOOLBAR);
        /* always show icon and don't grab focus on click/tab */
        gtk_button_set_always_show_image(GTK_BUTTON(button), TRUE);
        gtk_widget_set_can_focus(button, FALSE);

        gtk_grid_attach(GTK_GRID(grid), button, i, 0, 1, 1);
        if (controls[i].callback != NULL) {
            g_signal_connect(
                    button, "clicked",
                    G_CALLBACK(controls[i].callback),
                    (gpointer)(controls[i].icon_name));
        }
        if (controls[i].tooltip != NULL) {
            gtk_widget_set_tooltip_text(button, controls[i].tooltip);
        }
    }
    return grid;
}


/** \brief  Create main playlisy widget
 *
 * \return  GtkGrid
 */
GtkWidget *vsid_playlist_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *scroll;

    vsid_playlist_model_create();
    vsid_playlist_view_create();

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Playlist</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-bottom", 8, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scroll, 400, 500);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    gtk_container_add(GTK_CONTAINER(scroll), playlist_view);

    gtk_grid_attach(GTK_GRID(grid), scroll, 0, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid),
            vsid_playlist_controls_create(),
            0, 2, 1, 1);



    gtk_widget_show_all(grid);
    return grid;
}
