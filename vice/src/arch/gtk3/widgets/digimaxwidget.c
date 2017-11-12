/** \file   src/arch/gtk3/widgets/digimaxwidget.c
 * \brief   DigiMAX widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
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
#include "cartio.h"
#include "cartridge.h"

#include "digimaxwidget.h"


static void on_combo_changed(GtkWidget *widget, gpointer user_data)
{
    int value;
    char *endptr;


}


GtkWidget *digimax_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *combo;
    unsigned int base;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    gtk_grid_attach(GTK_GRID(grid), resource_check_button_create("DIGIMAX",
                "Enable DigiMax"), 0, 0, 1, 1);

    combo = gtk_combo_box_text_new();
    for (base = 0xde00; base < 0xe000; base += 0x20) {
        char text[256];
        char id_str[80];

        g_snprintf(text, 256, "$%04X", base);
        g_snprintf(id_str, 80, "%u", base);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), id_str, text);
    }
    g_signal_connect(combo, "changed", G_CALLBACK(on_combo_changed), NULL);

    label = gtk_label_new("DigiMAX base");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo, 1, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}

