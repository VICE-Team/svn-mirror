/** \file   settings_gmod2c128.c
 * \brief   Settings widget to control GMod2-C128 resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES GMod128EEPROMImage    x128
 * $VICERES GMod128EEPROMRW       x128
 * $VICERES GMod128FlashWrite     x128
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

/* #define DEBUG_GMOD2C128 */

#include "vice.h"
#include <gtk/gtk.h>

#include "vice_gtk3.h"
#include "cartridge.h"
#include "machine.h"
#include "resources.h"
#include "ui.h"
#include "log.h"

#include "settings_gmod2c128.h"

#ifdef DEBUG_GMOD2C128
#define DBG(x)  log_printf x
#else
#define DBG(x)
#endif


/** \brief  Create widget to control GMOD2-C128 resources
 *
 * \param[in]   parent  parent widget, used for dialogs
 *
 * \return  GtkGrid
 */
GtkWidget *settings_gmod2c128_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *primary;
    GtkWidget *secondary;

    DBG(("settings_gmod2c128_widget_create"));

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 32);

    /* primary image */
    primary = cart_image_widget_new(CARTRIDGE_C128_MAKEID(CARTRIDGE_C128_GMOD2C128),        /* cart id */
                                    CARTRIDGE_C128_NAME_GMOD2C128,   /* cart name */
                                    CART_IMAGE_PRIMARY,     /* image number */
                                    "cartridge",            /* image tag */
                                    NULL,                   /* resource name */
                                    TRUE,                   /* flush button */
                                    TRUE                    /* save button */
                                    );
    cart_image_widget_append_check(primary,
                                   "GMod128FlashWrite",
                                   "Save image when changed");

    /* secondary image */
    secondary = cart_image_widget_new(CARTRIDGE_C128_MAKEID(CARTRIDGE_C128_GMOD2C128),
                                      CARTRIDGE_C128_NAME_GMOD2C128,
                                      CART_IMAGE_SECONDARY,
                                      "EEPROM",
                                      "GMod128EEPROMImage",
                                      TRUE,
                                      TRUE);
    cart_image_widget_append_check(secondary,
                                   "GMod128EEPROMRW",
                                   "Enable writes to " CARTRIDGE_C128_NAME_GMOD2C128
                                   " EEPROM image");

    gtk_grid_attach(GTK_GRID(grid), primary,   0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), secondary, 0, 1, 1, 1);

    gtk_widget_show_all(grid);

    return grid;
}
