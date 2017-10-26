/** \file   src/arch/gtk3/widgets/drivedoswidget.c
 * \brief   Drive DOS expansions widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  Drive[8-11]ProfDos
 *  Drive[8-11]StarDOS
 *  Drive[8-11]SuperCard
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

#include "drivedoswidget.h"


/** \brief  Create DOS expansion check button
 *
 * \param[in]   unit    unit number (8-11)
 * \param[in]   dos     final part of the resource name (ie 'ProfDOS')
 * \param[in]   label   label for the check button
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


GtkWidget *drive_dos_widget_create(int unit)
{
    GtkWidget *grid;
    GtkWidget *profdos;
    GtkWidget *stardos;
    GtkWidget *supercard;

    grid = uihelpers_create_grid_with_label("DOS expansions", 1);
    g_object_set_data(G_OBJECT(grid), "UnitNumber", GINT_TO_POINTER(unit));

    profdos = create_dos_check_button(unit, "ProfDos", "Professional DOS");
    stardos = create_dos_check_button(unit, "StarDOS", "StarDOS");
    supercard = create_dos_check_button(unit, "SuperCard", "SuperCard+");

    gtk_grid_attach(GTK_GRID(grid), profdos, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), stardos, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), supercard, 0, 3, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
