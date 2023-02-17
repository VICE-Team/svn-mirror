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
    GtkWidget *cwd_label;
    GtkWidget *cwd_widget;
    GtkWidget *logfile_widget;
    int        row = 0;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    /* CWD header */
    cwd_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(cwd_label), "<b>Current working directory</b>");
    gtk_widget_set_halign(cwd_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), cwd_label, 0, row, 1, 1);
    row++;

    /* CWD widget */
    cwd_widget = cwd_widget_create();
    gtk_grid_attach(GTK_GRID(grid), cwd_widget, 0, row, 1, 1);
    row++;

    /* Logfile widget */
    logfile_widget = logfile_widget_create();
    gtk_widget_set_margin_top(logfile_widget, 24);  /* extra space */
    gtk_grid_attach(GTK_GRID(grid), logfile_widget, 0, row, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
