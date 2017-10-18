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
 *  UserportJoy
 *  JoyOpposite
 *  KeySetEnable
 *
 *  (see used widget for more)
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
#include "joystickuserportadapterwidget.h"

#include "uijoystick.h"


/** \brief  References to the joystick device widgets
 *
 * These references are used to update the UI when swapping joysticks
 */
static GtkWidget *device_widgets[JOYPORT_MAX_PORTS] = {
    NULL, NULL, NULL, NULL, NULL
};


static GtkWidget *adapter_widget = NULL;


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


static void on_keyset_enable_toggled(GtkWidget *check, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));

    debug_gtk3("setting KeySetEnable to %s\n", state ? "ON" : "OFF");
    resources_set_int("KeySetEnable", state);
}


static void on_opposite_enable_toggled(GtkWidget *check, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));

    debug_gtk3("setting JoyOpposite to %s\n", state ? "ON" : "OFF");
    resources_set_int("JoyOpposite", state);
}


static void on_userportjoy_enable_toggled(GtkWidget *check, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));

    debug_gtk3("setting UserportJoy to %s\n", state ? "ON" : "OFF");
    resources_set_int("UserportJoy", state);

    gtk_widget_set_sensitive(adapter_widget, state ? TRUE: FALSE);
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
    gtk_widget_set_vexpand(button, FALSE);
    gtk_widget_set_valign(button, GTK_ALIGN_END);
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
    gtk_widget_set_vexpand(button, FALSE);
    gtk_widget_set_valign(button, GTK_ALIGN_END);
    gtk_widget_show(button);
    return button;
}


static GtkWidget *create_keyset_enable_checkbox(void)
{
    GtkWidget *check;
    int current;

    resources_get_int("KeySetEnable", &current);

    check = gtk_check_button_new_with_label("Enable user-defined keysets");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), current);
    g_signal_connect(check, "toggled", G_CALLBACK(on_keyset_enable_toggled),
            NULL);

    gtk_widget_show(check);
    return check;
}


static GtkWidget *create_opposite_enable_checkbox(void)
{
    GtkWidget *check;
    int current;

    resources_get_int("JoyOpposite", &current);

    check = gtk_check_button_new_with_label("Allow opposite directions");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), current);
    g_signal_connect(check, "toggled", G_CALLBACK(on_opposite_enable_toggled),
            NULL);

    gtk_widget_show(check);
    return check;
}


static GtkWidget *create_userportjoy_enable_checkbox(void)
{
    GtkWidget *check;
    int current;

    resources_get_int("UserportJoy", &current);

    check = gtk_check_button_new_with_label("Enable Userport joysticks");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), current);
    g_signal_connect(check, "toggled",
            G_CALLBACK(on_userportjoy_enable_toggled), NULL);

    gtk_widget_show(check);
    return check;
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
    g_object_set(swap_button2, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), swap_button2, 1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), adapter_widget, 2, 2, 1, 1);

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
    gtk_grid_attach(GTK_GRID(grid), swap_button2, 1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), adapter_widget, 2, 2, 1, 1);

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
    g_object_set(swap_button2, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), swap_button2, 1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), adapter_widget, 2, 2, 1, 1);

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
    gtk_grid_attach(GTK_GRID(grid), swap_button2, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), adapter_widget, 2, 2, 1, 1);

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
    GtkWidget *keyset_widget;
    GtkWidget *opposite_widget;
    GtkWidget *userportjoy_widget;
    int rows = 0;
    int adapter_state;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 8);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 8);

    adapter_widget = joystick_userport_adapter_widget_create();
    resources_get_int("UserportJoy", &adapter_state);
    gtk_widget_set_sensitive(adapter_widget, adapter_state ? TRUE : FALSE);


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

    keyset_widget = create_keyset_enable_checkbox();
    g_object_set(keyset_widget, "margin-left", 16, NULL);
    opposite_widget = create_opposite_enable_checkbox();
    g_object_set(opposite_widget, "margin-left", 16, NULL);
    userportjoy_widget = create_userportjoy_enable_checkbox();
    g_object_set(userportjoy_widget, "margin-left", 16, NULL);

    gtk_grid_attach(GTK_GRID(layout), keyset_widget, 0, rows, 1, 1);
    gtk_grid_attach(GTK_GRID(layout), opposite_widget, 1, rows, 1, 1);
    gtk_grid_attach(GTK_GRID(layout), userportjoy_widget, 2, rows, 1, 1);

    gtk_widget_show_all(layout);
    return layout;
}
