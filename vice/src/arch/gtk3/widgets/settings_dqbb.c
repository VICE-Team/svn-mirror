/** \file   settings_dqbb.c
 * \brief   Widget to control Double Quick Brown Box resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES DQBB            x64 x64sc xscpu64 x128
 * $VICERES DQBBfilename    x64 x64sc xscpu64 x128
 * $VICERES DQBBImageWrite  x64 x64sc xscpu64 x128
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

#include "basedialogs.h"
#include "basewidgets.h"
#include "carthelpers.h"
#include "cartimagewidget.h"
#include "cartridge.h"
#include "widgethelpers.h"

#include "settings_dqbb.h"


/** \brief  Create widget to load/save Double Quick Brown Box image file
 *
 * \return  GtkGrid
 */
static GtkWidget *create_dqbb_image_widget(void)
{
    return cart_image_widget_create(NULL,
                                    "DQBBfilename",
                                    "DQBBImageWrite",
                                    CARTRIDGE_NAME_DQBB,
                                    CARTRIDGE_DQBB);
}


/** \brief  Create widget to control Double Quick Brown Box resources
 *
 * \param[in]   parent  parent widget, used for dialogs
 *
 * \return  GtkGrid
 */
GtkWidget *settings_dqbb_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *dqbb_enable_widget; /* dqbb_enable shadows */
    GtkWidget *dqbb_image;

    grid = vice_gtk3_grid_new_spaced(8, 8);

    dqbb_enable_widget = carthelpers_create_enable_check_button(CARTRIDGE_NAME_DQBB,
                                                                CARTRIDGE_DQBB);
    gtk_grid_attach(GTK_GRID(grid), dqbb_enable_widget, 0, 0, 1, 1);

    dqbb_image = create_dqbb_image_widget();
    gtk_widget_set_margin_top(dqbb_image, 8);
    gtk_grid_attach(GTK_GRID(grid), dqbb_image, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
