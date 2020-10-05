/** \file   v364speechwidget.c
 * \brief   V364 Speech widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES SpeechEnabled   xplus4
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

#include "basewidgets.h"
#include "debug_gtk3.h"
#include "lib.h"
#include "machine.h"
#include "openfiledialog.h"
#include "resources.h"
#include "ui.h"
#include "widgethelpers.h"

#include "v364speechwidget.h"


/** \brief  Handler for the "toggled" event of the Enable check button
 *
 * \param[in]   widget      check button
 * \param[in]   user_data   unused
 */
static void on_enable_toggled(GtkWidget *widget, gpointer user_data)
{
}

/** \brief  Create V364 Speech widget
 *
 * \return  GtkGrid
 */
GtkWidget *v364_speech_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *enable;

    grid = gtk_grid_new();

    enable = vice_gtk3_resource_check_button_new("SpeechEnabled",
            "Enable V364 Speech");

    gtk_grid_attach(GTK_GRID(grid), enable, 0, 0, 1, 1);

    g_signal_connect(enable, "toggled", G_CALLBACK(on_enable_toggled), NULL);

    gtk_widget_show_all(grid);
    return grid;
}
