/** \file   magicvoicewidget.c
 * \brief   Magic Voice widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES MagicVoiceCartridgeEnabled  x64 x64sc xscpu64 x128
 * $VICERES MagicVoiceImage             x64 x64sc xscpu64 x128
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
#include "cartridge.h"
#include "debug_gtk3.h"
#include "machine.h"
#include "openfiledialog.h"
#include "resources.h"
#include "ui.h"
#include "widgethelpers.h"

#include "magicvoicewidget.h"


/** \brief  Create widget to control MagicVoice cartridge
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *magic_voice_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *enable;
    GtkWidget *browser;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    /* enable checkbutton */
    enable = carthelpers_create_enable_check_button(
            CARTRIDGE_NAME_MAGIC_VOICE,
            CARTRIDGE_MAGIC_VOICE);
    gtk_grid_attach(GTK_GRID(grid), enable, 0, 0, 1, 1);

    /* image browser widget */
    browser = vice_gtk3_resource_browser_new(
            "MagicVoiceImage",
            NULL,
            NULL,
            "Select Magic Voice ROM image",
            "Magic Voice ROM",
            NULL);
    gtk_grid_attach(GTK_GRID(grid), browser, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
