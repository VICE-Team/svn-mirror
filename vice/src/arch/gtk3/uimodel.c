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
#include "videomodelwidget.h"
#include "vdcmodelwidget.h"
#include "sidmodelwidget.h"


#include "uimodel.h"


GtkWidget *uimodel_create_central_widget(GtkWidget *parent)
{
    GtkWidget *layout;

    GtkWidget *model_widget;
    GtkWidget *video_widget = NULL;
    GtkWidget *vdc_widget = NULL;       /* for the C128's VDC widget */

    GtkWidget *video_wrapper = NULL;    /* wrapper to have two video model
                                           widgets in case of the C128 */
    GtkWidget *sid_widget = NULL;

    layout = gtk_grid_new();

    model_widget = create_machine_model_widget();
    gtk_grid_attach(GTK_GRID(layout), model_widget, 0, 0, 1, 1);

    if (machine_class != VICE_MACHINE_CBM6x0 &&
            machine_class != VICE_MACHINE_PET) {

        video_wrapper = gtk_grid_new();

        video_widget = create_video_model_widget();
        gtk_grid_attach(GTK_GRID(video_wrapper), video_widget, 0, 0, 1, 1);

        if (machine_class == VICE_MACHINE_C128) {
            vdc_widget = vdc_model_widget_create();
            gtk_grid_attach(GTK_GRID(video_wrapper), vdc_widget, 0, 1, 1, 1);
        }
        gtk_widget_show_all(video_wrapper);

        gtk_grid_attach(GTK_GRID(layout), video_wrapper, 1, 0, 1, 1);
    }

    sid_widget = sid_model_widget_create(model_widget);

    gtk_grid_attach(GTK_GRID(layout), sid_widget, 2, 0, 1, 1);


    connect_machine_model_widget_signals(model_widget);
    if (machine_class != VICE_MACHINE_CBM6x0 &&
            machine_class != VICE_MACHINE_PET) {
        connect_video_model_widget_signals(video_widget);
    }
    if (machine_class == VICE_MACHINE_C128) {
        vdc_model_widget_connect_signals(vdc_widget);
    }
    gtk_widget_show_all(layout);

    return layout;
}


