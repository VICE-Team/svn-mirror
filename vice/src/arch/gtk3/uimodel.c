/** \file   src/arch/gtk3/uimodel.c
 * \brief   Model settings dialog
 *
 * Controls the following resource(s):
 *
 *  (for more, see used widgets)
 *
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
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

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "debug_gtk3.h"
#include "not_implemented.h"
#include "machine.h"
#include "machinemodelwidget.h"


#include "uimodel.h"


GtkWidget *uimodel_create_central_widget(GtkWidget *parent)
{
    GtkWidget *layout;


    layout = gtk_grid_new();

    gtk_grid_attach(GTK_GRID(layout), create_machine_model_widget(), 0, 0, 1, 1);


    gtk_widget_show_all(layout);

    return layout;
}
