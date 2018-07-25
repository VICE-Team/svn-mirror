/** \file   statusbarspeedwidget.c
 * \brief   CPU speed, FPS display widget for the statusbar
 *
 * Widget for the status bar that display CPU speed, FPS and warp/pause state.
 * When left-clicking on the widget a menu will pop up allowing the user to
 * control refresh rate, CPU speed, warp and pause.
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
#include "resources.h"

#include "statusbarspeedwidget.h"


/** \brief  Add separator to \a menu
 *
 * Little helper function to add a separator item to a menu.
 *
 * \param[in,out]   menu    GtkMenu instance
 */
static void add_separator(GtkWidget *menu)
{
    GtkWidget *item = gtk_separator_menu_item_new();
    gtk_container_add(GTK_CONTAINER(menu), item);
}


/** \brief  Handler for the toggled event of a refresh rate submenu item
 *
 * \param[in]   widget  refresh rate submenu item
 * \param[in]   data    new fresh rate
 */
static void on_refreshrate_toggled(GtkWidget *widget, gpointer data)
{
    int refresh = GPOINTER_TO_INT(data);

    debug_gtk3("setting Refresh Rate to %d.", refresh);
    resources_set_int("RefreshRate", refresh);
}


/** \brief  Create refresh rate submenu
 */
static GtkWidget *refreshrate_submenu_create(void)
{
    GtkWidget *menu;
    GtkWidget *item;
    int i;
    char buffer[16];
    int refresh;

    if (resources_get_int("RefreshRate", &refresh) < 0) {
        refresh = 0;
    }
    debug_gtk3("got refresh rate %d.", refresh);

    menu = gtk_menu_new();

    /* Auto */
    item = gtk_check_menu_item_new_with_label("Auto");
    gtk_check_menu_item_set_draw_as_radio(GTK_CHECK_MENU_ITEM(item), TRUE);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
            (gboolean)(refresh == 0));
    gtk_container_add(GTK_CONTAINER(menu), item);
    g_signal_connect(item, "toggled", G_CALLBACK(on_refreshrate_toggled),
            GINT_TO_POINTER(0));

    add_separator(menu);

    /* 1/1 through 1/10 */
    for (i = 1; i <= 10; i++) {
        g_snprintf(buffer, 16, "1/%d", i);
        item = gtk_check_menu_item_new_with_label(buffer);
        gtk_check_menu_item_set_draw_as_radio(GTK_CHECK_MENU_ITEM(item), TRUE);
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
                (gboolean)(refresh == i));
        gtk_container_add(GTK_CONTAINER(menu), item);
        g_signal_connect(item, "toggled", G_CALLBACK(on_refreshrate_toggled),
                GINT_TO_POINTER(i));

    }

    gtk_widget_show_all(menu);
    return menu;
}


/** \brief  Create popup menu for the statusbar speed widget
 *
 * \return  GtkMenu
 */
GtkWidget *speed_menu_popup_create(void)
{
    GtkWidget *menu;
    GtkWidget *submenu;
    GtkWidget *item;

    menu = gtk_menu_new();

    item = gtk_menu_item_new_with_label("Refresh rate");
    gtk_container_add(GTK_CONTAINER(menu), item);

    submenu = refreshrate_submenu_create();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu);

    gtk_widget_show_all(menu);
    return menu;
}


/** \brief  Event handler for the mouse clicks on the speed widget
 *
 * \param[in]   widget  event box around the speed label
 * \param[in]   event   event
 * \param[in]   data    extra event data (unused)
 *
 * \return  TRUE when event was handled, FALSE otherwise
 */
static gboolean on_widget_clicked(GtkWidget *widget,
                                  GdkEvent *event,
                                  gpointer data)
{
    if (((GdkEventButton *)event)->button == GDK_BUTTON_PRIMARY) {
        GtkWidget *menu = speed_menu_popup_create();
        gtk_menu_popup_at_widget(GTK_MENU(menu), widget,
                GDK_GRAVITY_NORTH_WEST, GDK_GRAVITY_SOUTH_WEST,
                event);
        return TRUE;
    }
    return FALSE;
}


/** \brief  Create widget to display CPU/FPS/pause
 *
 * \return  GtkLabel
 */
GtkWidget *statusbar_speed_widget_create(void)
{
    GtkWidget *label;
    GtkWidget *event_box;

    label = gtk_label_new("CPU: 100%, FPS: 50.125");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);

    event_box = gtk_event_box_new();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(event_box), FALSE);
    gtk_container_add(GTK_CONTAINER(event_box), label);
    gtk_widget_show(label);

    g_signal_connect(event_box, "button-press-event",
            G_CALLBACK(on_widget_clicked), NULL);

    return event_box;
}


/** \brief  Update the speed widget's display state
 *
 * \param[in]   percent     CPU speed in percentage
 * \param[in]   framerate   number of frames per second
 * \param[in]   warp_flag   warp is enabled flag
 *
 * \todo    Also handle/display pause state
 */
void statusbar_speed_widget_update(
        GtkWidget *widget,
        float percent,
        float framerate,
        int warp_flag)
{
    GtkWidget *label;
    char buffer[1024];
    int cpu = (int)(percent + 0.5);
    int fps = (int)(framerate + 0.5);

    g_snprintf(buffer, 1024, "%d%% cpu, %d fps %s",
            cpu, fps, warp_flag ? " (warp)" : "");

    label = gtk_bin_get_child(GTK_BIN(widget));

    gtk_label_set_text(GTK_LABEL(label), buffer);
}
