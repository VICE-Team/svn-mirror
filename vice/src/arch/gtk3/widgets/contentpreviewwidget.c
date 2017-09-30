/*
 * contentpreviewwidget.c - GTK3 disk/tape/archive preview widget
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

#include "contentpreviewwidget.h"


/** \brief  Create widget to show image contents in a file dialog
 *
 * \param[in]   selected    callback used when the user double-clicked a file
 *
 * \return  GtkGrid
 */
GtkWidget *content_preview_widget_create(void (*selected)(GtkWidget *, void *))
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *scroll;

    grid = gtk_grid_new();

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Image contents</b>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_grid_attach(GTK_GRID(grid), scroll, 0, 1, 1, 1);

    gtk_widget_set_size_request(scroll, 250, -1);

    gtk_widget_show_all(grid);
    return grid;
}
