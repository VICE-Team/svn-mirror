/** \file   src/arch/gtk3/uicommands.c
 * \brief   Simple commands triggered from the menu
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  WarpMode
 *  KeySetEnable
 *  JoyDevice1
 *  JoyDevice2
 *  JoyDevice3
 *  JoyDevice4
 *  Mouse
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

#include "resources.h"
#include "debug_gtk3.h"

#include "uicommands.h"


/** \brief  Switch warp mode
 *
 * \param[in]   widget      widget triggering the event (invalid)
 * \param[in]   user_data   extra data for event (unused)
 *
 * \return  TRUE
 */
gboolean ui_warp_callback(GtkWidget *widget, gpointer user_data)
{
    int state;

    resources_get_int("WarpMode", &state);
    resources_set_int("WarpMode", state ? 0 : 1);
    return TRUE;
}


/** \brief  Swap joysticks
 *
 * \param[in]   widget      widget triggering the event (invalid)
 * \param[in]   user_data   extra data for event (unused)
 *
 * \return  TRUE
 */
gboolean ui_swap_joysticks_callback(GtkWidget *widget, gpointer user_data)
{
    int joy1;
    int joy2;

    resources_get_int("JoyDevice1", &joy1);
    resources_get_int("JoyDevice2", &joy2);
    resources_set_int("JoyDevice1", joy2);
    resources_set_int("JoyDevice2", joy1);

    return TRUE;
}


/** \brief  Swap userport joysticks
 *
 * \param[in]   widget      widget triggering the event (invalid)
 * \param[in]   user_data   extra data for event (unused)
 *
 * \return  TRUE
 */
gboolean ui_swap_userport_joysticks_callback(GtkWidget *widget,
                                             gpointer user_data)
{
    int joy3;
    int joy4;

    resources_get_int("JoyDevice3", &joy3);
    resources_get_int("JoyDevice4", &joy4);
    resources_set_int("JoyDevice3", joy4);
    resources_set_int("JoyDevice4", joy3);

    return TRUE;
}


/** \brief  Toggle keyset joystick support
 *
 * \param[in]   widget      widget triggering the event (the menu item)
 * \param[in]   user_data   extra data for event (unused)
 *
 * \return  TRUE
 */
gboolean ui_allow_keyset_joystick_callback(GtkWidget *widget,
                                           gpointer user_data)
{
    int state;

    resources_get_int("KeySetEnable", &state);
    resources_set_int("KeySetEnable", state ? 0 : 1);

    return TRUE;
}


/** \brief  Toggle mouse grab
 *
 * \param[in]   widget      widget triggering the event (the menu item)
 * \param[in]   user_data   extra data for event (unused)
 *
 * \return  TRUE
 */
gboolean ui_mouse_grab_callback(GtkWidget *widget, gpointer user_data)
{
    int state;

    resources_get_int("Mouse", &state);
    resources_set_int("Mouse", state ? 0 : 1);

    return TRUE;
}
