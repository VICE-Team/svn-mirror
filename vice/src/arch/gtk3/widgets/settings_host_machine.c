/** \file   settings_host_machine.c
 * \brief   Widget to control resources control host machine settings
 *
 * These resources are hard to place, so if someone has a better idea, please
 * do mention it.
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
#include "resources.h"
#include "machine.h"

#include "jamactionwidget.h"

#include "settings_misc.h"


/** \brief  Create miscellaneous settings widget
 *
 * Basically a widget to contain (hopefully temporarily) widgets controlling
 * resources that can't (yet) be placed in a more logical location
 *
 * \param[in]   widget  parent widget (used for dialogs)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_host_machine_widget_create(GtkWidget *widget)
{
    GtkWidget *grid;
    GtkWidget *jam_widget = jam_action_widget_create();

    grid = gtk_grid_new();

    gtk_grid_attach(GTK_GRID(grid), jam_widget, 0, 0, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}
