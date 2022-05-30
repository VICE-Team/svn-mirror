/** \file   actions-debug.c
 * \brief   UI action implementations for debug-related dialogs and settings
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

/* Resources altered by this file:
 *
 * $VICERES MainCPU_TRACE   all
 * $VICERES IEC_TRACE       -vsid
 * $VICERES IEEE_TRACE      -vsid
 * $VICERES Drive0CPU_TRACE -vsid
 * $VICERES Drive1CPU_TRACE -vsid
 * $VICERES Drive2CPU_TRACE -vsid
 * $VICERES Drive3CPU_TRACE -vsid
 * $VICERES DoCoreDump      -vsid
 */

#include "vice.h"
#include "debug.h"

#ifdef DEBUG

#include <gtk/gtk.h>
#include <stddef.h>
#include <stdbool.h>

#include "resources.h"
#include "uiactions.h"
#include "uidebug.h"
#include "uimenu.h"

#include "actions-debug.h"


/** \brief  Toggle boolean resource and update its check menu item
 *
 * \param[in]   resource    resource name
 * \param[in]   action      UI action ID
 */
static void toggle_resource_and_update_menu(const char *resource, int action)
{
    int enabled;

    resources_get_int(resource, &enabled);
    enabled = !enabled;
    resources_set_int(resource, enabled);
    ui_set_check_menu_item_blocked_by_action(action, enabled);
}

/** \brief  Pop up dialog to change trace mode */
static void debug_trace_mode_action(void)
{
    ui_debug_trace_mode_dialog_show();
}

/** \brief  Toggle CPU trace */
static void debug_trace_cpu_toggle_action(void)
{
    toggle_resource_and_update_menu("MainCPU_TRACE",
                                    ACTION_DEBUG_TRACE_CPU_TOGGLE);
}

/** \brief  Toggle IEC bus trace */
static void debug_trace_iec_toggle_action(void)
{
    toggle_resource_and_update_menu("IEC_TRACE",
                                    ACTION_DEBUG_TRACE_IEC_TOGGLE);
}

/** \brief  Toggle IEEE-488 bus trace */
static void debug_trace_ieee488_toggle_action(void)
{
    toggle_resource_and_update_menu("IEEE_TRACE",
                                    ACTION_DEBUG_TRACE_IEEE488_TOGGLE);
}

/** \brief  Pop up dialog to change number of auto-playback frames */
static void debug_autoplayback_frames_action(void)
{
    ui_debug_playback_frames_dialog_show();
}

/** \brief  Toggle Drive 8 CPU trace */
static void debug_trace_drive_8_toggle_action(void)
{
    toggle_resource_and_update_menu("Drive0CPU_TRACE",
                                    ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE);
}

/** \brief  Toggle Drive 9 CPU trace */
static void debug_trace_drive_9_toggle_action(void)
{
    toggle_resource_and_update_menu("Drive1CPU_TRACE",
                                    ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE);
}

/** \brief  Toggle Drive 10 CPU trace */
static void debug_trace_drive_10_toggle_action(void)
{
    toggle_resource_and_update_menu("Drive2CPU_TRACE",
                                    ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE);
}

/** \brief  Toggle Drive 11 CPU trace */
static void debug_trace_drive_11_toggle_action(void)
{
    toggle_resource_and_update_menu("Drive3CPU_TRACE",
                                    ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE);
}

/** \brief  Toggle core dump */
static void debug_core_dump_toggle_action(void)
{
    toggle_resource_and_update_menu("DoCoreDump",
                                    ACTION_DEBUG_CORE_DUMP_TOGGLE);
}


/** \brief  List of cartridge-related actions */
static const ui_action_map_t debug_actions[] = {
    {
        .action = ACTION_DEBUG_TRACE_MODE,
        .handler = debug_trace_mode_action,
        .blocks = true,
        .dialog = true
    },
    {
        .action = ACTION_DEBUG_TRACE_CPU_TOGGLE,
        .handler = debug_trace_cpu_toggle_action
    },
    {
        .action = ACTION_DEBUG_TRACE_IEC_TOGGLE,
        .handler = debug_trace_iec_toggle_action
    },
    {
        .action = ACTION_DEBUG_TRACE_IEEE488_TOGGLE,
        .handler = debug_trace_ieee488_toggle_action
    },
    {
        .action = ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE,
        .handler = debug_trace_drive_8_toggle_action
    },
    {
        .action = ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE,
        .handler = debug_trace_drive_9_toggle_action
    },
    {
        .action = ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE,
        .handler = debug_trace_drive_10_toggle_action
    },
    {
        .action = ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE,
        .handler = debug_trace_drive_11_toggle_action
    },
    {
        .action = ACTION_DEBUG_AUTOPLAYBACK_FRAMES,
        .handler = debug_autoplayback_frames_action,
        .blocks = true,
        .dialog = true
    },
    {
        .action = ACTION_DEBUG_CORE_DUMP_TOGGLE,
        .handler = debug_core_dump_toggle_action
    },

    UI_ACTION_MAP_TERMINATOR
};


/** \brief  Register debugging actions */
void actions_debug_register(void)
{
    ui_actions_register(debug_actions);
}

#endif
