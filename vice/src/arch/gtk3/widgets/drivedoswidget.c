/** \file   drivedoswidget.c
 * \brief   Drive DOS expansions widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES Drive8ProfDOS       x64 x64sc xscpu64 x128
 * $VICERES Drive9ProfDOS       x64 x64sc xscpu64 x128
 * $VICERES Drive10ProfDOS      x64 x64sc xscpu64 x128
 * $VICERES Drive11ProfDOS      x64 x64sc xscpu64 x128
 * $VICERES Drive8StarDos       x64 x64sc xscpu64 x128
 * $VICERES Drive9StarDos       x64 x64sc xscpu64 x128
 * $VICERES Drive10StarDos      x64 x64sc xscpu64 x128
 * $VICERES Drive11StarDos      x64 x64sc xscpu64 x128
 * $VICERES Drive8SuperCard     x64 x64sc xscpu64 x128
 * $VICERES Drive9SuperCard     x64 x64sc xscpu64 x128
 * $VICERES Drive10SuperCard    x64 x64sc xscpu64 x128
 * $VICERES Drive11SuperCard    x64 x64sc xscpu64 x128
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
 */

#include "vice.h"

#include <gtk/gtk.h>

#include "basewidgets.h"
#include "debug_gtk3.h"
#include "drive-check.h"
#include "drive.h"
#include "machine.h"
#include "resources.h"
#include "widgethelpers.h"

#include "drivedoswidget.h"


/** \brief  Create DOS expansion check button
 *
 * \param[in]   unit    unit number (8-11)
 * \param[in]   dos     final part of the resource name (ie 'ProfDOS')
 * \param[in]   label   label for the check button
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_dos_check_button(int unit,
                                          const char *dos,
                                          const char *label)
{
    GtkWidget *check;

    check = vice_gtk3_resource_check_button_new_sprintf(
            "Drive%d%s", label, unit, dos);
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create drive DOS widget
 *
 * Create widget to select a DOS expansion for \a unit.
 *
 * \param[in]   unit    drive unit (8-11)
 *
 * \return  GtkGrid
 */
GtkWidget *drive_dos_widget_create(int unit)
{
    GtkWidget *grid;
    GtkWidget *profdos;
    GtkWidget *stardos;
    GtkWidget *supercard;
    int model = drive_get_disk_drive_type(unit - DRIVE_UNIT_MIN);

    grid = vice_gtk3_grid_new_spaced_with_label(-1, -1, "DOS expansions", 1);
    g_object_set_data(G_OBJECT(grid), "UnitNumber", GINT_TO_POINTER(unit));

    profdos = create_dos_check_button(unit, "ProfDos", "Professional DOS");
    stardos = create_dos_check_button(unit, "StarDOS", "StarDOS");
    supercard = create_dos_check_button(unit, "Supercard", "Supercard+");

    /* enable/disable widgets based on drive model */
    gtk_widget_set_sensitive(profdos, drive_check_profdos(model));
    gtk_widget_set_sensitive(stardos, drive_check_stardos(model));
    gtk_widget_set_sensitive(supercard, drive_check_supercard(model));

    gtk_grid_attach(GTK_GRID(grid), profdos, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), stardos, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), supercard, 0, 3, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
