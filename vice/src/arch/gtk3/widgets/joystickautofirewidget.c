/** \file   joystickautofirewidget.c
 * \brief   Widget to set joystick autofire resources
 *
 * Allows setting the autofire options for a joystick.
 *
 * Three settings are available:
 *  - autofire enable   (resource JoyStick[1-10]AutoFire)
 *  - autofire mode     (resource JoyStick[1-10]AutoFireMode)
 *  - autofire speed    (resource Joystick[1-10]AutoFireSpeed)
 *
 * Autofire mode has two settings:
 * - autofire when button is pressed (0)
 * - autofire when button is not pressed (1)
 *
 * Autofire speed is in button presses per second, with a range of 1-255
 * inclusive.
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES JoyStick1AutoFire       -vsid -xpet -xcbm2
 * $VICERES JoyStick2AutoFire       -vsid -xpet -xcbm2 -xvic
 * $VICERES JoyStick3AutoFire       -vsid
 * $VICERES JoyStick4AutoFire       -vsid
 * $VICERES JoyStick5AutoFire       -vsid -xpet
 * $VICERES JoyStick6AutoFire       -vsid -xpet
 * $VICERES JoyStick7AutoFire       -vsid -xpet -xplus4
 * $VICERES JoyStick8AutoFire       -vsid -xpet -xplus4
 * $VICERES JoyStick9AutoFire       -vsid -xpet -xplus4
 * $VICERES JoyStick10AutoFire      -vsid -xpet -xplus4
 * $VICERES JoyStick1AutoFireMode   -vsid -xpet -xcbm2
 * $VICERES JoyStick2AutoFireMode   -vsid -xpet -xcbm2 -xvic
 * $VICERES JoyStick3AutoFireMode   -vsid
 * $VICERES JoyStick4AutoFireMode   -vsid
 * $VICERES JoyStick5AutoFireMode   -vsid -xpet
 * $VICERES JoyStick6AutoFireMode   -vsid -xpet
 * $VICERES JoyStick7AutoFireMode   -vsid -xpet -xplus4
 * $VICERES JoyStick8AutoFireMode   -vsid -xpet -xplus4
 * $VICERES JoyStick9AutoFireMode   -vsid -xpet -xplus4
 * $VICERES JoyStick10AutoFireMode  -vsid -xpet -xplus4
 * $VICERES JoyStick1AutoFireSpeed  -vsid -xpet -xcbm2
 * $VICERES JoyStick2AutoFireSpeed  -vsid -xpet -xcbm2 -xvic
 * $VICERES JoyStick3AutoFireSpeed  -vsid
 * $VICERES JoyStick4AutoFireSpeed  -vsid
 * $VICERES JoyStick5AutoFireSpeed  -vsid -xpet
 * $VICERES JoyStick6AutoFireSpeed  -vsid -xpet
 * $VICERES JoyStick7AutoFireSpeed  -vsid -xpet -xplus4
 * $VICERES JoyStick8AutoFireSpeed  -vsid -xpet -xplus4
 * $VICERES JoyStick9AutoFireSpeed  -vsid -xpet -xplus4
 * $VICERES JoyStick10AutoFireSpeed -vsid -xpet -xplus4
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

#include "joystickautofirewidget.h"


/** \brief  List of autofire modes
 *
 * \note    Currently only two modes, so perhaps a different widget can be used,
 *          such as a switch or toggle button?
 */
static const vice_gtk3_radiogroup_entry_t autofire_modes[] = {
    { "button pressed", 0 },
    { "button released", 1 },
    VICE_GTK3_RADIOGROUP_ENTRY_LIST_END
};


/** \brief  Create left-aligned but indented label
 *
 * \param[in]   text    label text
 *
 * \return  GtkLabel
 */
static GtkWidget *create_label(const char *text)
{
    GtkWidget *label = gtk_label_new(text);

    g_object_set(G_OBJECT(label), "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_valign(label, GTK_ALIGN_START);
    return label;
}


/** \brief  Create widget to toggle autofire for a joystick
 *
 * \param[in]   joy joystick number (1-10)
 *
 * \return  GtkSwitch
 */
static GtkWidget *create_autofire_enable_widget(int joy)
{
    GtkWidget *widget;

    widget = vice_gtk3_resource_switch_new_sprintf("JoyStick%dAutoFire", joy);
    gtk_widget_set_halign(widget, GTK_ALIGN_START);
    gtk_widget_set_valign(widget, GTK_ALIGN_START);
    gtk_widget_set_hexpand(widget, FALSE);
    gtk_widget_set_vexpand(widget, FALSE);
    return widget;
}


/** \brief  Create widget to set the autofire mode of a joystick
 *
 * \param[in]   joy joystick number (1-10)
 *
 * \return  GtkGrid
 */
static GtkWidget *create_autofire_mode_widget(int joy)
{
    GtkWidget *widget;

    widget = vice_gtk3_resource_radiogroup_new_sprintf(
            "JoyStick%dAutoFireMode",
            autofire_modes,
            GTK_ORIENTATION_HORIZONTAL,
            joy);
    return widget;
}


/** \brief  Create widget to set the autofire speed of a joystick
 *
 * \param[in]   joy joystick number (1-10)
 *
 * \return  GtkScale
 */
static GtkWidget *create_autofire_speed_widget(int joy)
{
    GtkWidget *widget;

    widget = vice_gtk3_resource_scale_int_new_sprintf(
            "JoyStick%dAutoFireSpeed",
            GTK_ORIENTATION_HORIZONTAL,
            1, 255, 10,
            joy);
    gtk_scale_set_value_pos(GTK_SCALE(widget), GTK_POS_RIGHT);
    return widget;
}


/** \brief  Create widget to set autofire resources for a joystick
 *
 * \param[in]   joy     joystick number in the resource names (1-10)
 * \param[in]   title   title for the widget
 *
 * \return  GtkGrid
 */
GtkWidget *joystick_autofire_widget_create(int joy, const char *title)
{
    GtkWidget *grid;
    GtkWidget *enable;
    GtkWidget *mode;
    GtkWidget *speed;

    grid = vice_gtk3_grid_new_spaced_with_label(32, 4, title, 2);

    enable = create_autofire_enable_widget(joy);
    gtk_grid_attach(GTK_GRID(grid), create_label("Active"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), enable, 1, 1, 1, 1);

    mode = create_autofire_mode_widget(joy);
    gtk_grid_attach(GTK_GRID(grid), create_label("Mode"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), mode, 1, 2, 1, 1);

    speed = create_autofire_speed_widget(joy);
    gtk_grid_attach(GTK_GRID(grid), create_label("Speed"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), speed, 1, 3, 1, 1);
    gtk_widget_show_all(grid);

    return grid;
}
