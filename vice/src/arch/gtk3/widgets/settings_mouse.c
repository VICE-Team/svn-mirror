/** \file   settings_mouse.c
 * \brief   Mouse settings widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES ps2mouse            x64dtv
 * $VICERES SmartMouseRTCSave   x64 x64sc xscpu64 x128 xvic xplus4 xcbm5x0
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

#include "resources.h"
#include "machine.h"
#include "vice_gtk3.h"

#include "settings_mouse.h"


/** \brief  Create mouse settings widget
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *settings_mouse_widget_create(GtkWidget *parent)
{
    GtkWidget *layout;
    GtkWidget *ps2_enable;
    GtkWidget *mouse_save = NULL;
    int row = 0;

    layout = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);
    g_object_set(layout, "margin", 16, NULL);

    if (machine_class == VICE_MACHINE_C64DTV) {
        ps2_enable = vice_gtk3_resource_check_button_new("ps2mouse",
                "Enable PS/2 mouse on Userport");
        gtk_grid_attach(GTK_GRID(layout), ps2_enable, 0, row, 1, 1);
        row++;
    }

    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:     /* fall through */
        case VICE_MACHINE_VIC20:    /* fall through */
        case VICE_MACHINE_PLUS4:    /* fall through */
        case VICE_MACHINE_CBM5x0:
            mouse_save = vice_gtk3_resource_check_button_new(
                    "SmartMouseRTCSave", "Enable SmartMouse RTC Saving");
            gtk_grid_attach(GTK_GRID(layout), mouse_save, 0, row, 1, 1);
            row++;
            break;
        default:
            /* No SmartMouse support */
            break;
    }

    gtk_widget_show_all(layout);
    return layout;
}
