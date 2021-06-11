/** \file   settings_host_display.c
 * \brief   Widget to control resources related to the host display
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES FullscreenEnable        -vsid
 * (Maybe also for VSID?)
 * $VICERES StartMinimized          -vsid
 * $VICERES RestoreWindowGeometry   -vsid
 * (I guess VSID could also use this?)
 *
 * $VICERES VSync                   -vsid
 *
 * Note:    RestoreWindowGeometry works fine on Linux with a display stretched
 *          over multiple monitors. No idea how this would work with separate
 *          'desktops' for each monitor, we might have to store GDK's monitor
 *          index as well.
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
#include "resources.h"
#include "machine.h"
#include "videoarch.h"
#include "canvasrenderfilterwidget.h"
#include "settings_misc.h"

#include "settings_host_display.h"


/** \brief  Reference to the 'StartMinimized' checkbox */

static GtkWidget *minimized_widget = NULL;

/** \brief  Reference to the 'FullscreenEnable' checkbox */
static GtkWidget *fullscreen_widget = NULL;


/** \brief  Extra callback for the 'FullscreenEnable' widget
 *
 * When the resource is set, the 'StartMinimized' widget is toggled off and
 * disabled.
 *
 * \param[in]   widget  fullscreen checkbox (unused)
 * \param[in]   value   new value for \c widget
 */
static void fullscreen_callback(GtkWidget *widget, int value)
{
    gtk_widget_set_sensitive(minimized_widget, !value);
    if (value) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(minimized_widget), FALSE);
    }

}


/** \brief  Extra callback for the 'StartMinimized' widget
 *
 * When the resource is set, the 'FullscreenEnable' widget is toggled off and
 * disabled.
 *
 * \param[in]   widget  start-minimized checkbox (unused)
 * \param[in]   value   new value for \c widget
 */
static void minimized_callback(GtkWidget *widget, int value)
{
    gtk_widget_set_sensitive(fullscreen_widget, !value);
    if (value) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fullscreen_widget), FALSE);
    }

}


/** \brief  Create "Switch to fullscreen on boot" widget
 *
 * Currently completely ignores C128 VDC/VCII, maybe once we can differenciate
 * between displays (ie two or more) and put the GtkWindows there, we can
 * try to handle these scenarios. But for now, deh.
 *
 * \param[in]   index   window index (unused)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_fullscreen_widget(int index)
{
    GtkWidget *widget;

    widget = vice_gtk3_resource_check_button_new(
            "FullscreenEnable", "Switch to full screen on boot");
    vice_gtk3_resource_check_button_add_callback(widget, fullscreen_callback);
    return widget;
}


/** \brief  Create synchronization method widget
 *
 * \return  GtkGrid
 */
static GtkWidget *create_sync_widget(void)
{
    GtkWidget *grid;
    GtkWidget *vsync;
    GtkWidget *header;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, 0);

    header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), "<b>Gtk sync method</b>");
    gtk_widget_set_halign(header, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), header, 0, 0, 1, 1);

    vsync = vice_gtk3_resource_check_button_new("VSync", "VSync");
    gtk_widget_set_sensitive(vsync, TRUE);
    gtk_grid_attach(GTK_GRID(grid), vsync, 0, 2, 1, 1);
    g_object_set(vsync, "margin-left", 16, "margin-top", 8, NULL);

    return grid;
}


/** \brief  Create host display settings widget
 *
 * \param[in]   widget  parent widget (used for dialogs)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_host_display_widget_create(GtkWidget *widget)
{
    GtkWidget *grid;
    GtkWidget *filter_widget = canvas_render_filter_widget_create();
    GtkWidget *restore_window_widget;
    GtkWidget *sync_widget;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    if (machine_class != VICE_MACHINE_VSID) {

        int col = 0;
        int fullscreen = 0;
        int minimized = 0;

        fullscreen_widget = create_fullscreen_widget(0);

        minimized_widget = vice_gtk3_resource_check_button_new(
                "StartMinimized",
                "Start the emulator window minimized");

        vice_gtk3_resource_check_button_add_callback(
                minimized_widget, minimized_callback);

        restore_window_widget = vice_gtk3_resource_check_button_new(
                "RestoreWindowGeometry",
                "Restore emulator window(s) position and size from settings");

        sync_widget = create_sync_widget();

        gtk_grid_attach(GTK_GRID(grid), filter_widget, col++, 1, 2, 1);
        g_object_set(filter_widget, "margin-left",8, NULL);

        gtk_grid_attach(GTK_GRID(grid), sync_widget, col++, 1, 2, 1);

        g_object_set(fullscreen_widget, "margin-top", 16, NULL);
        gtk_grid_attach(GTK_GRID(grid), fullscreen_widget, 0, 2, 2, 1);
        gtk_grid_attach(GTK_GRID(grid), minimized_widget, 0, 3, 2, 1);
        gtk_grid_attach(GTK_GRID(grid), restore_window_widget, 0, 4, 2, 1);

        resources_get_int("FullscreenEnable", &fullscreen);
        resources_get_int("StartMinimized", &minimized);

        /* check situation regarding fullscreen and minimized */
        if (fullscreen && minimized) {
            /* illegal, clear both widgets */
            resources_set_int("FullscreenEnable", 0);
            resources_set_int("StartMinimized", 0);
            vice_gtk3_resource_check_button_sync(fullscreen_widget);
            vice_gtk3_resource_check_button_sync(minimized_widget);
        } else if (fullscreen) {
            /* fullscreen, so minimized cannot be active */
            gtk_widget_set_sensitive(minimized_widget, FALSE);
        } else if (minimized) {
            /* minimized, so fullscreen cannot be active */
            gtk_widget_set_sensitive(fullscreen_widget, FALSE);
        }
    }
    gtk_widget_show_all(grid);
    return grid;
}
