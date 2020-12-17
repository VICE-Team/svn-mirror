/** \file   src/arch/gtk3/widgets/jamactionwidget.c
 * \brief   GTK3 default JAM action setting dialog
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES JAMAction   all
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
#include "debug_gtk3.h"
#include "machine.h"
#include "vice_gtk3.h"

#include "jamactionwidget.h"


/** \brief  List of possible actions on a CPU JAM
 */
static const vice_gtk3_radiogroup_entry_t actions[] = {
    { "Show dialog",        MACHINE_JAM_ACTION_DIALOG },
    { "Continue emulation", MACHINE_JAM_ACTION_CONTINUE },
    { "Start monitor",      MACHINE_JAM_ACTION_MONITOR },
    { "Soft RESET",         MACHINE_JAM_ACTION_RESET },
    { "Hard RESET",         MACHINE_JAM_ACTION_HARD_RESET },
    { "Quit emulator",      MACHINE_JAM_ACTION_QUIT },
    { NULL, -1 }
};


/** \brief  Create widget to control the "JAMAction" resource
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *jam_action_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *selection;

    grid = vice_gtk3_grid_new_spaced_with_label(
            -1, -1,
            "Default action on CPU JAM",
            1);

    selection = vice_gtk3_resource_radiogroup_new("JAMAction", actions,
            GTK_ORIENTATION_VERTICAL);
    g_object_set(selection, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), selection, 0, 1, 1,1);

    gtk_widget_show_all(grid);
    return grid;
}
