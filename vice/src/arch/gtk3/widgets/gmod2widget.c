/** \file   src/arch/gtk3/widgets/gmod2widget.c
 * \brief   Widget to control GMod2 resources
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  GMOD2EEPROMImage
 *  GMOD2EEPROMRW
 *  GMod2FlashWrite
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
 *
 */

#include "vice.h"
#include <gtk/gtk.h>

#include "machine.h"
#include "resources.h"
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "widgethelpers.h"
#include "basedialogs.h"
#include "openfiledialog.h"
#include "savefiledialog.h"
#include "cartridge.h"
#include "cartimagewidget.h"

#include "gmod2widget.h"


/* list of widgets, used to enable/disable depending on GEORAM resource */
static GtkWidget *gmod2_image = NULL;
static GtkWidget *gmod2_rw_widget = NULL;

static int (*gmod2_save_func)(int, const char *) = NULL;
static int (*gmod2_flush_func)(int) = NULL;


/** \brief  Create widget to load/save GMod2 image file
 *
 * \return  GtkGrid
 */
static GtkWidget *create_gmod2_image_widget(GtkWidget *parent)
{
    return cart_image_widget_create(parent, "GMOD2 EEPROM image",
            "GMOD2EEPROMImage", "GMod2FlashWrite",
            gmod2_save_func, gmod2_flush_func,
            CARTRIDGE_NAME_GMOD2, CARTRIDGE_GMOD2);
}


/** \brief  Create widget to control GMod2 EEPROM writes
*
* \return  GtkGrid
*/
static GtkWidget *create_gmod2_rw_widget(void)
{
    GtkWidget *widget;

    widget = resource_check_button_create("GMOD2EEPROMRW",
            "Enable writes to GMod2 EEPROM image");
    g_object_set(widget, "margin-left", 16, NULL);
    return widget;
}


/** \brief  Create widget to control GMOD2 resources
 *
 * \param[in]   parent  parent widget, used for dialogs
 *
 * \return  GtkGrid
 */
GtkWidget *gmod2_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    gmod2_image = create_gmod2_image_widget(parent);
    gtk_grid_attach(GTK_GRID(grid), gmod2_image, 0, 1, 1, 1);

    gmod2_rw_widget = create_gmod2_rw_widget();
    gtk_grid_attach(GTK_GRID(grid), gmod2_rw_widget, 0, 2, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Set save function for the GMod2 extension
 *
 * \param[in]   func    save function
 */
void gmod2_widget_set_save_handler(int (*func)(int, const char *))
{
    gmod2_save_func = func;
}


/** \brief  Set flush function for the GMod2 extension
 *
 * \param[in]   func    flush function
 */
void gmod2_widget_set_flush_handler(int (*func)(int))
{
    gmod2_flush_func = func;
}
