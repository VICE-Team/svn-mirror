/** \file   drivefixedsizewidget.c
 * \brief   Drive fixed size widget
 *
 * Used to set a fixed size for a CMDHD drive image
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES Drive8FixedSize     -vsid
 * $VICERES Drive9FixedSize     -vsid
 * $VICERES Drive10FixedSize    -vsid
 * $VICERES Drive11fixedSize    -vsid
 *
 *  (for more, see used widgets)
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
#include "resources.h"

#include "drivefixedsizewidget.h"


/** \brief  Create widget to set a fixed size for \a unit
 *
 * \param[in]   unit    drive unit number (8-11)
 *
 * \return  GtkGrid
 */
GtkWidget *drive_fixed_size_widget_create(int unit)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *entry;
    char resource[1024];

    grid = vice_gtk3_grid_new_spaced(16, 0);

    /* set unit number */
    g_object_set_data(G_OBJECT(grid), "Unit", GINT_TO_POINTER(unit));

    label = gtk_label_new("CMD-HD fixed size");
    g_object_set(label, "margin-left", 16, "margin-top", 8, NULL);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    /* create entry */
    g_snprintf(resource, sizeof(resource), "Drive%dFixedSize", unit);
    entry = vice_gtk3_resource_numeric_string_new(resource);
    /* set limits */
    vice_gtk3_resource_numeric_string_set_limits(
            entry, 75 * 1024, UINT64_MAX, TRUE);

    gtk_widget_set_halign(entry, GTK_ALIGN_START);
    gtk_widget_set_hexpand(entry, TRUE);
    g_object_set(entry, "margin-left", 16, "margin-top", 8, NULL);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}

