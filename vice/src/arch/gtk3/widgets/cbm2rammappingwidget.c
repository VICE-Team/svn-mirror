/** \file   src/arch/gtk3/widgets/cbm2rammappingwidget.c
 * \brief   Widget to map RAM into bank 15
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  Ram08
 *  Ram1
 *  Ram2
 *  Ram4
 *  Ram6
 *  RamC
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

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"

#include "cbm2rammappingwidget.h"

typedef struct ram_mapping_s {
    const char *text;
    char *resource;
} ram_mapping_t;


static const ram_mapping_t mappings[] = {
    { "$0800-$0FFF", "Ram08" },
    { "$1000-$1FFF", "Ram1" },
    { "$2000-$3FFF", "Ram2" },
    { "$4000-$5FFF", "Ram4" },
    { "$6000-$7FFF", "Ram6" },
    { "$C000-$CFFF", "RamC" },
    { NULL, NULL }
};


static void on_ram_mapping_toggled(GtkWidget *widget, gpointer user_data)
{
    char *res = (char *)user_data;
    int enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    debug_gtk3("setting %s to %s\n", res, enabled ? "ON" : "OFF");
    resources_set_int(res, enabled);
}


GtkWidget *cbm2_ram_mapping_widget_create(void)
{
    GtkWidget *grid;

    grid = uihelpers_create_grid_with_label("Map RAM into bank 15", 1);
    for (int i = 0; mappings[i].text != NULL; i++) {
        GtkWidget *check;
        int enabled;

        check = gtk_check_button_new_with_label(mappings[i].text);
        g_object_set(check, "margin-left", 16, NULL);
        resources_get_int(mappings[i].resource, &enabled);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), enabled);
        g_signal_connect(check, "toggled", G_CALLBACK(on_ram_mapping_toggled),
                (gpointer)mappings[i].resource);
        gtk_grid_attach(GTK_GRID(grid), check, 0, i + 1, 1, 1);
    }

    gtk_widget_show_all(grid);
    return grid;
}
