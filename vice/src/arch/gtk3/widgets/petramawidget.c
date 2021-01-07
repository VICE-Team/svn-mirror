/** \file   petramawidget.c
 * \brief   Widget to set the RAMA area type (PET 8296 only)
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 * \author  André Fachat <fachat@web.de>
 */

/*
 * $VICERES Ram9  xpet
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

#include "petiosizewidget.h"


/** \brief  Available I/O sizes
 * Defines values for jumper JU2/JU4
 * for details see http://www.6502.org/users/andre/petindex/local/8296desc.txt
 */
static const vice_gtk3_radiogroup_entry_t area_types[] = {
    { "ROM", 0 },
    { "RAM", 1 },
    { NULL, -1 }
};


static void (*user_callback)(int) = NULL;

static void on_rama_changed(GtkWidget *widget, int id)
{
    if (user_callback != NULL) {
        user_callback(id);
    }
}

/** \brief  Create PET I/O area size widget
 *
 * \return  GtkGrid
 */
GtkWidget *pet_rama_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *group;

    user_callback = NULL;

    grid = vice_gtk3_grid_new_spaced_with_label(-1, -1, "$Axxx area type", 1);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    group = vice_gtk3_resource_radiogroup_new("RamA", area_types,
            GTK_ORIENTATION_VERTICAL);
    vice_gtk3_resource_radiogroup_add_callback(group, on_rama_changed);
    g_object_set(group, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), group, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Set custom callback function for \a widget
 *
 * \param[in,out]   widget  PET RAMA widget
 * \param[in        func    callback function
 */
void pet_rama_widget_set_callback(GtkWidget *widget,
                                     void (*func)(int))
{
    user_callback = func;
}


/** \brief  Synchronize \a widget with its resource
 *
 * \param[in,out]   widget  PET RAMA widget
 */
void pet_rama_widget_sync(GtkWidget *widget)
{
    int size;

    resources_get_int("RamA", &size);
    GtkWidget *group = gtk_grid_get_child_at(GTK_GRID(widget), 0, 1);
    vice_gtk3_resource_radiogroup_set(group, size);
}
