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

#include "driveexpansionwidget.h"


/** \brief  Unit number (8-11)
 *
 * XXX: probably not required
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


/** \brief  Handler for "toggled" event of RAM expansion check buttons
 *
 * \param[in]   widget      check button
 * \param[in]   user_data   RAM base address (word)
 */
static void on_ram_toggled(GtkWidget *widget, gpointer user_data)
{
    gchar res_name[256];
    int state;

    debug_gtk3("called\n");

    state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    g_snprintf(res_name, 256, "Drive%dRAM%04X", unit_number,
            GPOINTER_TO_UINT(user_data));
    debug_gtk3("setting %s to %s\n", res_name, state ? "ON" : "OFF");
    resources_set_int(res_name, state);
}


/** \brief  Handler for "toggled" event of DOS expansion check buttons
 *
 * \param[in]   widget      check button
 * \param[in]   user_data   last part of resource name (ie 'ProfDOS')
 */
static void on_dos_toggled(GtkWidget *widget, gpointer user_data)
{
    gchar buffer[256];
    int state;

    state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    g_snprintf(buffer, 256, "Drive%d%s", unit_number, (const char *)user_data);
    debug_gtk3("setting %s to %s\n", buffer, state ? "ON" : "OFF");
    resources_set_int(buffer, state);
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
    g_signal_connect(check, "toggled", G_CALLBACK(on_ram_toggled),
            GUINT_TO_POINTER(base));

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
    g_signal_connect(check, "toggled", G_CALLBACK(on_dos_toggled),
            (gpointer)res);

    return check;
}


/** \brief  Create drive expansion widget
 *
 * \return  GtkGrid
 */
GtkWidget *create_drive_expansion_widget(int unit)
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

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Update drive expansion widget
 *
 * \param[in,out]   widget  drive expansion widget
 * \param[in]       unit    drive unit number
 */
void update_drive_expansion_widget(GtkWidget *widget, int unit)
{
    int drive_type;
    gchar buffer[256];

    unit_number = unit;

    /* determine drive type */
    g_snprintf(buffer, 256, "Drive%dType", unit);
    resources_get_int(buffer, &drive_type);

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
    gtk_widget_set_sensitive(profdos_widget, drive_check_profdos(drive_type));
    gtk_widget_set_sensitive(stardos_widget, drive_check_stardos(drive_type));
    gtk_widget_set_sensitive(supercard_widget, drive_check_supercard(drive_type));


    return;
}
