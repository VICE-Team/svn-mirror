/** \file   settings_environment.c
 * \brief   Settings widget to control host environment settings
 *
 * /author  Bas Wassink <b.wassink@ziggo.nl>
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

#include "cwdwidget.h"
#include "logfilewidget.h"
#include "vice_gtk3.h"

#include "settings_environment.h"


/** \brief  Create widget to change host environment settings
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_environment_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    int        row = 1;

    grid = vice_gtk3_grid_new_spaced(16, 24);

    gtk_grid_attach(GTK_GRID(grid), cwd_widget_create(), 0, row, 1, 1);
    row++;

    gtk_grid_attach(GTK_GRID(grid), logfile_widget_create(), 0, row, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
