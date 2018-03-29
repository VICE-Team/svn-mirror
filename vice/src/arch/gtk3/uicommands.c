/** \file   uicommands.c
 * \brief   Simple commands triggered from the menu
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 */

/*
 * $VICERES WarpMode        all
 * $VICERES JoyDevice1      -vsid
 * $VICERES JoyDevice2      -vsid
 * $VICERES JoyDevice3      -vsid
 * $VICERES JoyDevice4      -vsid
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

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <stdbool.h>

#include "resources.h"
#include "debug_gtk3.h"
#include "basedialogs.h"
#include "drive.h"
#include "machine.h"
#include "vsync.h"

#include "ui.h"
#include "uicommands.h"

extern ui_resource_t ui_resources;

static bool crt_controls_enable = false;


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


/** \brief  Callback for the soft/hard reset items
 *
 * \param[in]   widget      menu item triggering the event (unused)
 * \param[in]   user_data   MACHINE_RESET_MODE_SOFT/MACHINE_RESET_MODE_HARD
 */
void ui_machine_reset_callback(GtkWidget *widget, gpointer user_data)
{
    vsync_suspend_speed_eval();
    machine_trigger_reset(GPOINTER_TO_INT(user_data));
}


/** \brief  Callback for the drive reset items
 *
 * \param[in]   widget      menu item triggering the event (unused)
 * \param[in]   user_data   drive unit number (8-11) (int)
 */
void ui_drive_reset_callback(GtkWidget *widget, gpointer user_data)
{
    vsync_suspend_speed_eval();
    drive_cpu_trigger_reset(GPOINTER_TO_INT(user_data) - 8);
}


/** \brief  Callback for the File->Exit menu item
 *
 * This asks the user to confirm to exit the emulator if ConfirmOnExit is set.
 *
 * \param[in]   widget      menu item triggering the event (unused)
 * \param[in]   user_data   window index, optional, defaults to primary
 */
void ui_close_callback(GtkWidget *widget, gpointer user_data)
{
    int index;
    int confirm;

    if (user_data == NULL) {
        index = PRIMARY_WINDOW;
    } else {
        index = GPOINTER_TO_INT(user_data);
    }

    resources_get_int("ConfirmOnExit", &confirm);
    if (!confirm) {
        gtk_widget_destroy(ui_resources.window_widget[index]);
        return;
    }

    if (vice_gtk3_message_confirm("Exit VICE",
                "Do you really wish to exit VICE?")) {
        debug_gtk3("Exit confirmed\n");
        gtk_widget_destroy(ui_resources.window_widget[index]);
    }
}


/** \brief  Handler for the "delete-event" of a GtkWindow
 *
 * \param[in]   widget      window triggering the event
 * \param[in]   event       event details (unused)
 * \param[in]   user_data   extra data for the event (unused)
 *
 * \return  `FALSE` to exit the emulator, `TRUE` to continue
 */
gboolean on_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    int confirm;

    debug_gtk3("got 'delete-event'\n'");

    resources_get_int("ConfirmOnExit", &confirm);
    if (!confirm) {
        return FALSE;
    }

    if (vice_gtk3_message_confirm("Exit VICE",
                "Do you really wish to exit VICE?")) {
        debug_gtk3("Exit confirmed\n");
        return FALSE;
    }
    return TRUE;
}


/** \brief  Callback for a windows' "destroy" event
 *
 * \param[in]   widget      widget triggering the event (unused)
 * \param[in]   user_data   extra data for the callback (unused)
 */
void ui_window_destroy_callback(GtkWidget *widget, gpointer user_data)
{
    debug_gtk3("called\n");
    vsync_suspend_speed_eval();
    ui_exit();
}


/** \brief  Toggle boolean resource from the menu
 *
 * Toggles \a resource when a valid.
 *
 * \param[in]   widget      menu item triggering the event
 * \param[in]   resource    resource name
 *
 * \return  TRUE if succesful, FALSE otherwise
 */
gboolean ui_toggle_resource(GtkWidget *widget, gpointer resource)
{
    const char *res = (const char *)resource;

    if (res != NULL) {
        int state;

        /* attempt to get resource */
        if (resources_get_int(res, &state) < 0) {
            debug_gtk3("reading resource %s failed\n", res);
            return FALSE;
        }
        debug_gtk3("toggling %s to %s\n", res, state ? "False" : "True");
        if (resources_set_int(res, state ? 0 : 1) < 0) {
            debug_gtk3("setting resource %s failed\n", res);
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}


gboolean ui_toggle_crt_controls(void)
{
    crt_controls_enable = !crt_controls_enable;

    ui_enable_crt_controls(crt_controls_enable);
    return TRUE;
}

