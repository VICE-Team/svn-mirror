/** \file   actions-machine.c
 * \brief   UI action implementations for machine-related dialogs and settings
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
 *
 */

#include "vice.h"

#include <gtk/gtk.h>
#include <stddef.h>
#include <stdbool.h>

#include "archdep.h"
#include "basedialogs.h"
#include "machine.h"
#include "mainlock.h"
#include "monitor.h"
#include "resources.h"
#include "ui.h"
#include "uiactions.h"
#include "vsync.h"

#include "actions-machine.h"


/** \brief  Callback for the confirm-on-exit dialog
 *
 * Exit VICE if \a result is TRUE.
 *
 * \param[in]   dialog  dialog reference (unused)
 * \param[in]   result  dialog result
 */
static void confirm_exit_callback(GtkDialog *dialog, gboolean result)
{
    if (result) {
        mainlock_release();
        archdep_vice_exit(0);
        mainlock_obtain();
    }
    /* mark action finished in case the user selected "No" */
    ui_action_finish(ACTION_QUIT);
}


/** \brief  Quit emulator, possibly popping up a confirmation dialog */
static void quit_action(void *unused)
{
    int confirm = FALSE;

    resources_get_int("ConfirmOnExit", &confirm);
    if (!confirm) {
        ui_action_finish(ACTION_QUIT);
        archdep_vice_exit(0);
        return;
    }

    vice_gtk3_message_confirm(
            confirm_exit_callback,
            "Exit VICE",
            "Do you really wish to exit VICE?");
}


/** \brief  Open the monitor */
static void monitor_open_action(void *unused)
{
    int server = 0;

    /* don't spawn the monitor if the monitor server is running */
    resources_get_int("MonitorServer", &server);
    if (server == 0) {
        vsync_suspend_speed_eval();
#ifdef HAVE_MOUSE
        /* FIXME: restore mouse in case it was grabbed */
        /* ui_restore_mouse(); */
#endif
        if (ui_pause_active()) {
            ui_pause_enter_monitor();
        } else {
            monitor_startup_trap();
        }
    }
}

/** \brief  Trigger soft reset of the machine */
static void reset_soft_action(void *unused)
{
    machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
    ui_pause_disable();
}

/** \brief  Trigger hard reset of the machine */
static void reset_hard_action(void *unused)
{
    machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    ui_pause_disable();
}

/** \brief  Toggle PET userport diagnostic pin */
static void diagnostic_pin_toggle_action(void *unused)
{
    int active = 0;

    resources_get_int("DiagPin", &active);
    resources_set_int("DiagPin", active ? 0 : 1);
}


/** \brief  List of machine-related actions */
static const ui_action_map_t machine_actions[] = {
    {
        .action  = ACTION_QUIT,
        .handler = quit_action,
        .blocks  = true,
        .dialog  = true
    },
    {
        .action   = ACTION_MONITOR_OPEN,
        .handler  = monitor_open_action,
        .uithread = true
    },
    {
        .action  = ACTION_RESET_SOFT,
        .handler = reset_soft_action
    },
    {
        .action  = ACTION_RESET_HARD,
        .handler = reset_hard_action
    },
    {
        .action  = ACTION_DIAGNOSTIC_PIN_TOGGLE,
        .handler = diagnostic_pin_toggle_action,
        /* no need for UI thread, the status bar code will update the LED when
         * it runs */
        .uithread = false
    },

    UI_ACTION_MAP_TERMINATOR
};


void actions_machine_register(void)
{
    ui_actions_register(machine_actions);
}
