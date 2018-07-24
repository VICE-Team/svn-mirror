/** \file   statusbarspeedwidget.c
 * \brief   CPU speed, FPS display widget for the statusbar
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
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
#include "vice_gtk3.h"

#include "statusbarspeedwidget.h"


/** \brief  Create widget to display CPU/FPS/pause
 *
 * \return  GtkLabel
 */
GtkWidget *statusbar_speed_widget_create(void)
{
    GtkWidget *label;

    label = gtk_label_new("CPU: 100%, FPS: 50.125");
    gtk_widget_show(label);
    return label;
}


void statusbar_speed_widget_update(
        GtkWidget *widget,
        float percent,
        float framerate,
        int warp_flag)
{
    char buffer[1024];
    int cpu = (int)(percent + 0.5);
    int fps = (int)(framerate + 0.5);

    g_snprintf(buffer, 1024, "%d%% cpu, %d fps %s",
            cpu, fps, warp_flag ? " (warp)" : "");
    gtk_label_set_text(GTK_LABEL(widget), buffer);
}
