/** \file   rexramfloppywidget.c
 * \brief   Widget to control REX Ram-Floppy resources
 *
 * \author  Marco van den Heuvel <blackystardust68@yahoo.com>
 */

/*
 * $VICERES RRFfilename    x64 x64sc xscpu64 x128
 * $VICERES RRFImageWrite  x64 x64sc xscpu64 x128
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
#include "debug_gtk3.h"
#include "machine.h"
#include "openfiledialog.h"
#include "resources.h"
#include "savefiledialog.h"
#include "widgethelpers.h"

#include "dqbbwidget.h"


/** \brief  Create widget to load/save REX Ram-Floppy image file
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
static GtkWidget *create_rexramfloppy_image_widget(GtkWidget *parent)
{
    return cart_image_widget_create(parent, "REX Ram-Floppy image",
            "RRFfilename", "RRFImageWrite",
            carthelpers_save_func, carthelpers_flush_func,
            carthelpers_can_save_func, carthelpers_can_flush_func,
            CARTRIDGE_NAME_REX_RAMFLOPPY, CARTRIDGE_REX_RAMFLOPPY);
}


/** \brief  Create widget to control REX Ram-Floppy resources
 *
 * \param[in]   parent  parent widget, used for dialogs
 *
 * \return  GtkGrid
 */
GtkWidget *rexramfloppy_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *rexramfloppy_image;

    grid = vice_gtk3_grid_new_spaced(8, 8);

    rexramfloppy_image = create_rexramfloppy_image_widget(parent);
    gtk_grid_attach(GTK_GRID(grid), rexramfloppy_image, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
