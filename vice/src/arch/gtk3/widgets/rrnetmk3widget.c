/** \file   src/arch/gtk3/widgets/rrnetmk3widget.c
 * \brief   Widget to control RRNet MK3 resourcs
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *
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
 */

#include "vice.h"
#include <gtk/gtk.h>
#include <stdlib.h>

#include "machine.h"
#include "resources.h"
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "widgethelpers.h"
#include "basedialogs.h"
#include "cartridge.h"

#include "rrnetmk3widget.h"


GtkWidget *rrnetmk3_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *flash_jumper;
    GtkWidget *bios_write;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    flash_jumper = resource_check_button_create("RRNETMK3_flashjumper",
            "Enable flash jumper");

    gtk_grid_attach(GTK_GRID(grid), flash_jumper, 0, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
