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

#include "uisettings.h"


static GtkWidget *create_treeview(void)
{
    GtkWidget *tree;

    tree = gtk_tree_view_new();




    return tree;
}


void ui_settings_dialog_callback(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *layout;
    GtkWidget *tree;

#ifdef HAVE_DEBUG_GTK3UI
    g_print("[debug-gtk3ui] %s() called\n", __func__);
#endif

    layout = gtk_grid_new();
    tree = create_treeview();
    gtk_grid_attach(GTK_GRID(layout), tree, 0, 0, 1, 1);
}
