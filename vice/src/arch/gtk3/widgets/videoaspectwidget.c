/** \file   videoaspectwidget.c
 * \brief   GTK3 widget to select renderer filter
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 * \author  groepaz <groepaz@gmx.net>
 */

/*
 * $VICERES CrtcAspectMode      xpet xcbm2
 * $VICERES TEDAspectMode       xplus4
 * $VICERES VDCAspectMode       x128
 * $VICERES VICAspectMode       xvic
 * $VICERES VICIIAspectMode     x64 x64sc xscpu64 xdtv64 x128 xcbm5x0
 * $VICERES CrtcAspectRatio     xpet xcbm2
 * $VICERES TEDAspectRatio      xplus4
 * $VICERES VDCAspectRatio      x128
 * $VICERES VICAspectRatio      xvic
 * $VICERES VICIIAspectRatio    x64 x64sc xscpu64 xdtv64 x128 xcbm5x0
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
 *
 */

#include "vice.h"

#include <gtk/gtk.h>

#include "vice_gtk3.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "video.h"

#include "videoaspectwidget.h"



/** \brief  List of radio buttons
 */
static const vice_gtk3_radiogroup_entry_t aspect_modes[] = {
    { "Off",     0 },
    { "Custom",  1 },
    { "True",    2 },
    { NULL, -1 }
};


/** \brief  Create widget to control render filter resources
 *
 * \param[in]   chip    video chip prefix
 *
 * \return  GtkGrid
 */
GtkWidget *video_aspect_widget_create(const char *chip)
{
    GtkWidget *grid;
    GtkWidget *render_widget;
    GtkWidget *aspect_ratio_s;
    GtkWidget *label;

    grid = vice_gtk3_grid_new_spaced_with_label(
            VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT, "Aspect Mode", 1);
    render_widget = vice_gtk3_resource_radiogroup_new_sprintf(
            "%sAspectMode", aspect_modes, GTK_ORIENTATION_VERTICAL, chip);
    gtk_widget_set_margin_start(render_widget, 16);
    gtk_grid_attach(GTK_GRID(grid), render_widget, 0, 1, 1, 1);

    /* label */
    label = vice_gtk3_create_indented_label("Ratio");
    /* address */
    aspect_ratio_s = vice_gtk3_resource_entry_full_new_sprintf(
        "%sAspectRatio", chip);
    gtk_widget_set_hexpand(aspect_ratio_s, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), aspect_ratio_s, 1, 2, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Set callback function to trigger on radio button toggles
 *
 * \param[in,out]   widget      render filter widget
 * \param[in]       callback    function accepting the radio button and its value
 */
void video_aspect_widget_add_callback(GtkWidget *widget,
                                             void (*callback)(GtkWidget *, int))
{
    GtkWidget *group;

    group = gtk_grid_get_child_at(GTK_GRID(widget), 0, 1);
    vice_gtk3_resource_radiogroup_add_callback(group, callback);
}
