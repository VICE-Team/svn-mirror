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
 *  Drive[8-11]ProfDOS
 *  Drive[8-11]StarDos
 *  Drive[8-11]SuperCard
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
 *
 * TODO:    check machine type (x64, x64sc, xscpu64 or x128) to enable/disable
 *          the DOS expansion check buttons
 */

#include "vice.h"

#include <gtk/gtk.h>

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "drive.h"
#include "drive-check.h"
#include "machine.h"

#include "driveexpansionwidget.h"


/** \brief  Unit number (8-11)
 */
static int unit_number = 8;

/*
 * References to check buttons in the widget
 */
static GtkWidget *ram2000_widget = NULL;
static GtkWidget *ram4000_widget = NULL;
static GtkWidget *ram6000_widget = NULL;
static GtkWidget *ram8000_widget = NULL;
static GtkWidget *ramA000_widget = NULL;
static GtkWidget *profdos_widget = NULL;
static GtkWidget *stardos_widget = NULL;
static GtkWidget *supercard_widget = NULL;



static void on_destroy(GtkWidget *widget, gpointer user_data)
{
    ram2000_widget = NULL;
    ram4000_widget = NULL;
    ram6000_widget = NULL;
    ram8000_widget = NULL;
    ramA000_widget = NULL;
    profdos_widget = NULL;
    stardos_widget = NULL;
    supercard_widget = NULL;
}



/** \brief  Handler for "toggled" event of RAM expansion check buttons
 *
 * \param[in]   widget      check button
 * \param[in]   user_data   RAM base address (word)
 */
static void on_ram_toggled(GtkWidget *widget, gpointer user_data)
{
    int state;
    unsigned int base = GPOINTER_TO_UINT(user_data);

    state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    debug_gtk3("setting Drive%dRAM%04X to %s\n", unit_number, base,
            state ? "ON" : "OFF");
    resources_set_int_sprintf("Drive%dRAM%04X", state, unit_number, base);
}


/** \brief  Handler for "toggled" event of DOS expansion check buttons
 *
 * \param[in]   widget      check button
 * \param[in]   user_data   last part of resource name (ie 'ProfDOS')
 */
static void on_dos_toggled(GtkWidget *widget, gpointer user_data)
{
    int state;
    const char *dos = (const char *)user_data;

    state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    debug_gtk3("setting Drive%d%s to %s\n", unit_number, dos,
            state ? "ON" : "OFF");
    resources_set_int_sprintf("Drive%d%s", state, unit_number, dos);
}


/** \brief  Create drive RAM expansion check button
 *
 * \param[in]   base    RAM base address (word)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_ram_check_button(unsigned int base)
{
    GtkWidget *check;
    gchar buffer[256];

    g_snprintf(buffer, 256, "$%04X-$%04X RAM", base, base + 0x1fff);

    check = gtk_check_button_new_with_label(buffer);
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create DOS expansion widget
 *
 * \param[in]   name    name of the expansion
 * \param[in]   res     final part of resource name
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_dos_check_button(const char *name, const char *res)
{
    GtkWidget *check;

    check = gtk_check_button_new_with_label(name);
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

    unit_number = unit;

    grid = uihelpers_create_grid_with_label("drive expansions", 1);

    ram2000_widget = create_ram_check_button(0x2000);
    ram4000_widget = create_ram_check_button(0x4000);
    ram6000_widget = create_ram_check_button(0x6000);
    ram8000_widget = create_ram_check_button(0x8000);
    ramA000_widget = create_ram_check_button(0xA000);

    gtk_grid_attach(GTK_GRID(grid), ram2000_widget, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ram4000_widget, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ram6000_widget, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ram8000_widget, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ramA000_widget, 0, 5, 1, 1);

    profdos_widget = create_dos_check_button("Professional DOS", "ProfDOS");
    stardos_widget = create_dos_check_button("StarDOS", "StarDos");
    supercard_widget = create_dos_check_button("SuperCard+", "SuperCard");
    gtk_grid_attach(GTK_GRID(grid), profdos_widget, 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), stardos_widget, 0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), supercard_widget, 0, 8, 1, 1);

    drive_expansion_widget_update(grid, unit);

    g_signal_connect(ram2000_widget, "toggled", G_CALLBACK(on_ram_toggled),
            GUINT_TO_POINTER(0x2000));
    g_signal_connect(ram4000_widget, "toggled", G_CALLBACK(on_ram_toggled),
            GUINT_TO_POINTER(0x4000));
    g_signal_connect(ram6000_widget, "toggled", G_CALLBACK(on_ram_toggled),
            GUINT_TO_POINTER(0x6000));
    g_signal_connect(ram8000_widget, "toggled", G_CALLBACK(on_ram_toggled),
            GUINT_TO_POINTER(0x8000));
    g_signal_connect(ramA000_widget, "toggled", G_CALLBACK(on_ram_toggled),
            GUINT_TO_POINTER(0xA000));

    g_signal_connect(profdos_widget, "toggled", G_CALLBACK(on_dos_toggled),
            (gpointer)"ProfDOS");
    g_signal_connect(stardos_widget, "toggled", G_CALLBACK(on_dos_toggled),
            (gpointer)"StarDos");
    g_signal_connect(supercard_widget, "toggled", G_CALLBACK(on_dos_toggled),
            (gpointer)"SuperCard");

    g_signal_connect(grid, "destroy", G_CALLBACK(on_destroy), NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Update drive expansion widget
 *
 * \param[in,out]   widget  drive expansion widget
 * \param[in]       unit    drive unit number
 */
void drive_expansion_widget_update(GtkWidget *widget, int unit)
{
    int drive_type;

    unit_number = unit;

    /* determine drive type */
    resources_get_int_sprintf("Drive%dType", &drive_type, unit);

    /* check RAM expansions */
    gtk_widget_set_sensitive(ram2000_widget,
            drive_check_expansion2000(drive_type));
    gtk_widget_set_sensitive(ram4000_widget,
            drive_check_expansion4000(drive_type));
    gtk_widget_set_sensitive(ram6000_widget,
            drive_check_expansion6000(drive_type));
    gtk_widget_set_sensitive(ram8000_widget,
            drive_check_expansion8000(drive_type));
    gtk_widget_set_sensitive(ramA000_widget,
            drive_check_expansionA000(drive_type));

    /* check DOS extensions */
    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:
            /* supported, depending on drive type */
            gtk_widget_set_sensitive(profdos_widget,
                    drive_check_profdos(drive_type));
            gtk_widget_set_sensitive(stardos_widget,
                    drive_check_stardos(drive_type));
            gtk_widget_set_sensitive(supercard_widget,
                    drive_check_supercard(drive_type));
            break;
        default:
            /* not supported for the current machine */
            gtk_widget_set_sensitive(profdos_widget, FALSE);
            gtk_widget_set_sensitive(stardos_widget, FALSE);
            gtk_widget_set_sensitive(supercard_widget, FALSE);
    }
}
