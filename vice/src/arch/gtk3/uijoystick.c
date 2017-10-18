/** \file   src/arch/gtk3/uijoystick.c
 * \brief   Widget to control settings for joysticks
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  JoyDevice1
 *  JoyDevice2
 *  JoyDevice3
 *  JoyDevice4
 *  JoyDevice5
 *  ... more to come
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

#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>

#include "lib.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "machine.h"
#include "resources.h"
#include "joyport.h"
#include "joystick.h"

#include "joystickdevicewidget.h"

#include "uijoystick.h"


/** \brief  References to the joystick device widgets
 *
 * These references are used to update the UI when swapping joysticks
 */
static GtkWidget *device_widgets[JOYPORT_MAX_PORTS] = {
    NULL, NULL, NULL, NULL, NULL
};


/** \brief  Handler for the "clicked" event of the "swap joysticks" button
 *
 * Swaps resources JoyDevice1 and JoyDevice2 and updates UI accordingly.
 *
 * \param[in]   button      button (unused)
 * \param[in]   user_data   extra event data (unused)
 */
static void on_swap_joysticks_clicked(GtkWidget *button, gpointer user_data)
{
    int joy1;
    int joy2;

    /* guard against updating non-existing widgets */
    if (device_widgets[JOYPORT_1] == NULL
            || device_widgets[JOYPORT_2] == NULL) {
        return; /* cannot swap */
    }

    debug_gtk3("swapping joystick #1 and joystick #2\n");

    /* get current values */
    resources_get_int_sprintf("JoyDevice%d", &joy1, 1);
    resources_get_int_sprintf("JoyDevice%d", &joy2, 2);

    /* updating the widgets triggers updating the resources */
    joystick_device_widget_update(device_widgets[JOYPORT_1], joy2);
    joystick_device_widget_update(device_widgets[JOYPORT_2], joy1);
}


/** \brief  Handler for the "clicked" event of the "swap uport joysticks" button
 *
 * Swaps resources JoyDevice3 and JoyDevice4 and updates UI accordingly.
 *
 * \param[in]   button      button (unused)
 * \param[in]   user_data   extra event data (unused)
 */
static void on_swap_userport_joysticks_clicked(GtkWidget *button,
                                               gpointer user_data)
{
    int joy3;
    int joy4;

    /* guard against updating non-existing widgets */
    if (device_widgets[JOYPORT_3] == NULL
            || device_widgets[JOYPORT_4] == NULL) {
        return; /* cannot swap */
    }

    debug_gtk3("swapping userport joystick #1 and userport joystick #2\n");

    /* get current values */
    resources_get_int_sprintf("JoyDevice%d", &joy3, 3);
    resources_get_int_sprintf("JoyDevice%d", &joy4, 4);

    /* updating the widgets triggers updating the resources */
    joystick_device_widget_update(device_widgets[JOYPORT_3], joy4);
    joystick_device_widget_update(device_widgets[JOYPORT_4], joy3);
}


/** \brief  Create a button to swap joysticks #1 and #2
 *
 * \return  GtkButton
 */
static GtkWidget *create_swap_joysticks_button(void)
{
    GtkWidget *button;

    button = gtk_button_new_with_label("Swap joysticks");
    g_signal_connect(button, "clicked", G_CALLBACK(on_swap_joysticks_clicked),
            NULL);
    gtk_widget_show(button);
    return button;
}


/** \brief  Create a button to swap userport joysticks #1 and #2
 *
 * \return  GtkButton
 */
static GtkWidget *create_swap_userport_joysticks_button(void)
{
    GtkWidget *button;

    button = gtk_button_new_with_label("Swap Userport joysticks");
    g_signal_connect(button, "clicked",
            G_CALLBACK(on_swap_userport_joysticks_clicked), NULL);
    gtk_widget_show(button);
    return button;
}


/** \brief  Create layout for x64/x64sc/xscpu64/x128
 *
 * \param[in,out]   grid    main widget grid
 *
 * \return  number of rows added to \a grid
 */
static int create_c64_layout(GtkGrid *grid)
{
    GtkWidget *swap_button1;
    GtkWidget *swap_button2;

    device_widgets[JOYPORT_1] = joystick_device_widget_create(
            JOYPORT_1, "Joystick #1");
    device_widgets[JOYPORT_2] = joystick_device_widget_create(
            JOYPORT_2, "Joystick #2");
    device_widgets[JOYPORT_3] = joystick_device_widget_create(
            JOYPORT_3, "Userport Joystick #1");
    device_widgets[JOYPORT_4] = joystick_device_widget_create(
            JOYPORT_4, "Userport Joystick #2");

    gtk_grid_attach(grid, device_widgets[JOYPORT_1], 0, 0, 1, 1);
    gtk_grid_attach(grid, device_widgets[JOYPORT_2], 1, 0, 1, 1);
    gtk_grid_attach(grid, device_widgets[JOYPORT_3], 0, 1, 1, 1);
    gtk_grid_attach(grid, device_widgets[JOYPORT_4], 1, 1, 1, 1);

    swap_button1 = create_swap_joysticks_button();
    g_object_set(swap_button1, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), swap_button1, 0, 2, 1, 1);

    swap_button2 = create_swap_userport_joysticks_button();
    gtk_grid_attach(GTK_GRID(grid), swap_button2, 1, 2, 1, 1);

    return 3;
}


/** \brief  Create layout for x64dtv
 *
 * \param[in,out]   grid    main widget grid
 *
 * \return  number of rows added to \a grid
 */
static int create_c64dtv_layout(GtkGrid *grid)
{
    GtkWidget *swap_button1;

    device_widgets[JOYPORT_1] = joystick_device_widget_create(
            JOYPORT_1, "Joystick #1");
    device_widgets[JOYPORT_2] = joystick_device_widget_create(
            JOYPORT_2, "Joystick #2");
    device_widgets[JOYPORT_3] = joystick_device_widget_create(
            JOYPORT_3, "Userport Joystick");

    gtk_grid_attach(grid, device_widgets[JOYPORT_1], 0, 0, 1, 1);
    gtk_grid_attach(grid, device_widgets[JOYPORT_2], 1, 0, 1, 1);
    gtk_grid_attach(grid, device_widgets[JOYPORT_3], 0, 1, 1, 1);

    swap_button1 = create_swap_joysticks_button();
    g_object_set(swap_button1, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), swap_button1, 0, 2, 1, 1);

    return 3;
}


/** \brief  Create layout for xvic
 *
 * \param[in,out]   grid    main widget grid
 *
 * \return  number of rows added to \a grid
 */
static int create_vic20_layout(GtkGrid *grid)
{
    GtkWidget *swap_button2;

    device_widgets[JOYPORT_1] = joystick_device_widget_create(
            JOYPORT_1, "Joystick");
    device_widgets[JOYPORT_3] = joystick_device_widget_create(
            JOYPORT_3, "Userport Joystick #1");
    device_widgets[JOYPORT_4] = joystick_device_widget_create(
            JOYPORT_4, "Userport Joystick #2");

    gtk_grid_attach(grid, device_widgets[JOYPORT_1], 0, 0, 1, 1);
    gtk_grid_attach(grid, device_widgets[JOYPORT_3], 0, 1, 1, 1);
    gtk_grid_attach(grid, device_widgets[JOYPORT_4], 1, 1, 1, 1);

    swap_button2 = create_swap_userport_joysticks_button();
    g_object_set(swap_button2, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), swap_button2, 0, 2, 1, 1);

    return 2;
}

/** \brief  Create layout for xplus4
 *
 * \param[in,out]   grid    main widget grid
 *
 * \return  number of rows added to \a grid
 */
static int create_plus4_layout(GtkGrid *grid)
{
    GtkWidget *swap_button1;
    GtkWidget *swap_button2;

    device_widgets[JOYPORT_1] = joystick_device_widget_create(
            JOYPORT_1, "Joystick #1");
    device_widgets[JOYPORT_2] = joystick_device_widget_create(
            JOYPORT_2, "Joystick #2");
    device_widgets[JOYPORT_3] = joystick_device_widget_create(
            JOYPORT_3, "Userport Joystick #1");
    device_widgets[JOYPORT_4] = joystick_device_widget_create(
            JOYPORT_4, "Userport Joystick #2");
    device_widgets[JOYPORT_5] = joystick_device_widget_create(
            JOYPORT_5, "SIDCard Joystick");

    gtk_grid_attach(grid, device_widgets[JOYPORT_1], 0, 0, 1, 1);
    gtk_grid_attach(grid, device_widgets[JOYPORT_2], 1, 0, 1, 1);
    gtk_grid_attach(grid, device_widgets[JOYPORT_3], 0, 1, 1, 1);
    gtk_grid_attach(grid, device_widgets[JOYPORT_4], 1, 1, 1, 1);
    gtk_grid_attach(grid, device_widgets[JOYPORT_5], 2, 1, 1, 1);

    swap_button1 = create_swap_joysticks_button();
    g_object_set(swap_button1, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), swap_button1, 0, 2, 1, 1);
    swap_button2 = create_swap_userport_joysticks_button();
    gtk_grid_attach(GTK_GRID(grid), swap_button2, 1, 2, 1, 1);

    return 3;
}


/** \brief  Create layout for xcbm5x0
 *
 * \param[in,out]   grid    main widget grid
 *
 * \return  number of rows added to \a grid
 */
static int create_cbm5x0_layout(GtkGrid *grid)
{
    GtkWidget *swap_button1;

    device_widgets[JOYPORT_1] = joystick_device_widget_create(
            JOYPORT_1, "Joystick #1");
    device_widgets[JOYPORT_2] = joystick_device_widget_create(
            JOYPORT_2, "Joystick #2");

    gtk_grid_attach(grid, device_widgets[JOYPORT_1], 0, 0, 1, 1);
    gtk_grid_attach(grid, device_widgets[JOYPORT_2], 1, 0, 1, 1);

    swap_button1 = create_swap_joysticks_button();
    g_object_set(swap_button1, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), swap_button1, 0, 1, 1, 1);

    return 2;
}


/** \brief  Create layout for xcbm2/xpet
 *
 * \param[in,out]   grid    main widget grid
 *
 * \return  number of rows added to \a grid
 */
static int create_cbm6x0_layout(GtkGrid *grid)
{
    GtkWidget *swap_button2;

    device_widgets[JOYPORT_3] = joystick_device_widget_create(
            JOYPORT_3, "Userport Joystick #1");
    device_widgets[JOYPORT_4] = joystick_device_widget_create(
            JOYPORT_4, "Userport Joystick #2");
    gtk_grid_attach(grid, device_widgets[JOYPORT_3], 0, 0, 1, 1);
    gtk_grid_attach(grid, device_widgets[JOYPORT_4], 1, 0, 1, 1);

    swap_button2 = create_swap_joysticks_button();
    g_object_set(swap_button2, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), swap_button2, 0, 1, 1, 1);

    return 2;
}



/** \brief  Create joystick settings main widget
 *
 * \param   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *uijoystick_widget_create(GtkWidget *parent)
{
    GtkWidget *layout;
    int rows = 0;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 8);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 8);

    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:
            rows = create_c64_layout(GTK_GRID(layout));
            break;
        case VICE_MACHINE_C64DTV:
            rows = create_c64dtv_layout(GTK_GRID(layout));
            break;
        case VICE_MACHINE_VIC20:
            rows = create_vic20_layout(GTK_GRID(layout));
            break;
        case VICE_MACHINE_PLUS4:
            rows = create_plus4_layout(GTK_GRID(layout));
            break;
        case VICE_MACHINE_CBM5x0:
            rows = create_cbm5x0_layout(GTK_GRID(layout));
            break;
        case VICE_MACHINE_PET:          /* fall through */
        case VICE_MACHINE_CBM6x0:
            rows = create_cbm6x0_layout(GTK_GRID(layout));
            break;
        case VICE_MACHINE_VSID:
            break;  /* no control ports or user ports */
        default:
            break;
    }

    gtk_widget_show_all(layout);
    return layout;
}
