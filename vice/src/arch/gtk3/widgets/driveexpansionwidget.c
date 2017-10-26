/** \file   src/arch/gtk3/widgets/driveexpansionwidget.c
 * \brief   Drive expansions widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  Drive[8-11]RAM2000
 *  Drive[8-11]RAM4000
 *  Drive[8-11]RAM6000
 *  Drive[8-11]RAM8000
 *  Drive[8-11]RAMA000
 *  Drive[8-11]ProfDOS (x64, x64sc, xscpu64 and x128)
 *  Drive[8-11]StarDos (x64, x64sc, xscpu64 and x128)
 *  Drive[8-11]SuperCard (x64, x64sc, xscpu64 and x128)
 *
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
 */

#include "vice.h"

#include <gtk/gtk.h>

#include "basewidgets.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "drive.h"
#include "drive-check.h"
#include "machine.h"

#include "driveexpansionwidget.h"



/** \brief  Create drive RAM expansion check button
 *
 * \param[in]   unit    unit number (8-11)
 * \param[in]   base    RAM base address (word)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_ram_check_button(int unit, unsigned int base)
{
    GtkWidget *check;
    char label[256];
    char resource[256];

    g_snprintf(resource, 256, "Drive%dRAM%04x", unit, base);
    g_snprintf(label, 256, "$%04X-$%04X RAM", base, base + 0x1fff);

    check = resource_check_button_create(resource, label);
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create DOS expansion check button
 *
 * \param[in]   unit    unit number (8-11)
 * \param[in]   dos     final part of the resource name (ie 'ProfDOS')
 * \param[in]   label   labe for the check button
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_dos_check_button(int unit, const char *dos,
                                          const char *label)
{
    GtkWidget *check;
    char resource[256];

    g_snprintf(resource, 256, "Drive%d%s", unit, dos);

    check = resource_check_button_create(resource, label);
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create drive expansion widget
 *
 * \return  GtkGrid
 */
GtkWidget *drive_expansion_widget_create(int unit)
{
    GtkWidget *grid;
    GtkWidget *ram2000_widget;
    GtkWidget *ram4000_widget;
    GtkWidget *ram6000_widget;
    GtkWidget *ram8000_widget;
    GtkWidget *ramA000_widget;
    GtkWidget *profdos_widget;
    GtkWidget *stardos_widget;
    GtkWidget *supercard_widget;

    grid = uihelpers_create_grid_with_label("drive expansions", 1);

    g_object_set_data(G_OBJECT(grid), "UnitNumber", GINT_TO_POINTER(unit));

    ram2000_widget = create_ram_check_button(unit, 0x2000);
    ram4000_widget = create_ram_check_button(unit, 0x4000);
    ram6000_widget = create_ram_check_button(unit, 0x6000);
    ram8000_widget = create_ram_check_button(unit, 0x8000);
    ramA000_widget = create_ram_check_button(unit, 0xA000);

    gtk_grid_attach(GTK_GRID(grid), ram2000_widget, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ram4000_widget, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ram6000_widget, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ram8000_widget, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ramA000_widget, 0, 5, 1, 1);

    /* TODO: check `machine_class` before adding these */
    if (machine_class == VICE_MACHINE_C64
            || machine_class == VICE_MACHINE_C64SC
            || machine_class == VICE_MACHINE_SCPU64
            || machine_class == VICE_MACHINE_C128) {
        profdos_widget = create_dos_check_button(unit, "ProfDos",
                "Professional DOS");
        stardos_widget = create_dos_check_button(unit, "StarDOS", "StarDos");
        supercard_widget = create_dos_check_button(unit, "SuperCard",
                "SuperCard+");
        gtk_grid_attach(GTK_GRID(grid), profdos_widget, 0, 6, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), stardos_widget, 0, 7, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), supercard_widget, 0, 8, 1, 1);
    }

    drive_expansion_widget_update(grid);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Update drive expansion widget
 *
 * \param[in,out]   widget  drive expansion widget
 * \param[in]       unit    drive unit number
 */
void drive_expansion_widget_update(GtkWidget *widget)
{
    int drive_type;
    int unit;

    /* get unit number */
    unit = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "UnitNumber"));

    /* determine drive type */
    resources_get_int_sprintf("Drive%dType", &drive_type, unit);

    /* check RAM expansions */
    gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(widget), 0, 1),
            drive_check_expansion2000(drive_type));
    gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(widget), 0, 2),
            drive_check_expansion4000(drive_type));
    gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(widget), 0, 3),
            drive_check_expansion6000(drive_type));
    gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(widget), 0, 4),
            drive_check_expansion8000(drive_type));
    gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(widget), 0, 5),
            drive_check_expansionA000(drive_type));

    /* check DOS extensions */
#if 0
    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:
            /* supported, depending on drive type */
            gtk_widget_set_sensitive(
                    gtk_grid_get_child_at(GTK_GRID(widget), 0, 6),
                    drive_check_profdos(drive_type));

            gtk_widget_set_sensitive(
                    gtk_grid_get_child_at(GTK_GRID(widget), 0, 7),
                    drive_check_stardos(drive_type));

            gtk_widget_set_sensitive(
                    gtk_grid_get_child_at(GTK_GRID(widget), 0, 8),
                    drive_check_supercard(drive_type));

            break;
        default:
            /* not supported for the current machine */
            gtk_widget_set_sensitive(
                    gtk_grid_get_child_at(GTK_GRID(widget), 0, 6), FALSE);

            gtk_widget_set_sensitive(
                    gtk_grid_get_child_at(GTK_GRID(widget), 0, 7), FALSE);

            gtk_widget_set_sensitive(
                    gtk_grid_get_child_at(GTK_GRID(widget), 0, 8), FALSE);
            break;
    }
#endif
}
