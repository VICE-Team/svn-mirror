/*
 * uisettings.c - GTK3 settings dialog
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

#include "lib.h"
#include "vsync.h"

/* widgets */
#include "refreshratewidget.h"

#include "uisettings.h"


#define NUM_COLUMNS 1


static char *topitems[] = { "Refresh rate", "maximum speed", "warp" };


/* TODO:    find a non-convoluted way to add items with callbacks setting the
 *          proper 'main widget'
 */
static GtkWidget *create_treeview(void)
{
    GtkWidget *tree;
    GtkTreeStore *store;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkTreeIter iter;   /* parent iter */
    GtkTreeIter child;  /* child iter */

    /* create the model */
    store = gtk_tree_store_new(NUM_COLUMNS, G_TYPE_STRING);

    /* add root node */
    gtk_tree_store_append(store, &iter, NULL);

    gtk_tree_store_set(
            store, &iter,
            0, "Speed",
            -1);

    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        NULL,
        renderer,
        "text", 0,
        NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

    return tree;
}


void ui_settings_dialog_callback(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *layout;
    GtkWidget *tree;

#ifdef HAVE_DEBUG_GTK3UI
    g_print("[debug-gtk3ui] %s() called\n", __func__);
#endif

    layout = gtk_grid_new();
    tree = create_treeview();
    gtk_grid_attach(GTK_GRID(layout), tree, 0, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(layout), create_refreshrate_widget(), 1, 0, 1, 1);

    gtk_widget_show(layout);
    gtk_widget_show(tree);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_container_add(GTK_CONTAINER(window), layout);
    gtk_widget_show(window);
}
