/** \file   uiactions.c
 * \brief   Gtk3 UI action names and descriptions
 *
 * Provides names and descriptions for Gtk3 UI actions.
 *
 * Used by menu namedescs and custom hotkeys. There will be no Doxygen docblocks
 * for most of the defines, since they're self-explanatory. And obviously I will
 * not bitch about keeping the text within 80 columns :D
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

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "lib.h"
#include "machine.h"

#include "uiactions.h"



/** \brief  Generate name, description and mask for an array element
 *
 * Generate '{ ACTION_FOO_ID, ACTION_FOO, ACTION_FOO_DESC, ACTION_FOO_MASK }'
 * for use in the UI actions array.
 *
 * \param[in]   x   action name define
 */
#define mkinfo(x) { ACTION_##x##_ID, ACTION_##x, ACTION_##x##_DESC, ACTION_##x##_MASK }


/** \brief  Mapping of action names to descriptions and machine support
 *
 * The 'machine' members is a bitmask that indicates which machines support
 * the action.
 */
typedef struct ui_action_info_internal_s {
    int         id;         /**< action ID */
    const char *name;       /**< action name */
    const char *desc;       /**< action description */
    uint32_t    machine;    /**< bitmask indicating which machines support the
                                 action */
} ui_action_info_internal_t;


/** \brief  List of UI actions
 */
static const ui_action_info_internal_t action_info_list[] = {

    /* smart attach */
    mkinfo(SMART_ATTACH),

    /* disk image attach */
    mkinfo(DRIVE_ATTACH_8_0),
    mkinfo(DRIVE_ATTACH_8_1),
    mkinfo(DRIVE_ATTACH_9_0),
    mkinfo(DRIVE_ATTACH_9_1),
    mkinfo(DRIVE_ATTACH_10_0),
    mkinfo(DRIVE_ATTACH_10_1),
    mkinfo(DRIVE_ATTACH_11_0),
    mkinfo(DRIVE_ATTACH_11_1),

    /* disk image create & attach */
    mkinfo(DRIVE_CREATE),

    /* disk image detach */
    mkinfo(DRIVE_DETACH_8_0),
    mkinfo(DRIVE_DETACH_8_1),
    mkinfo(DRIVE_DETACH_9_0),
    mkinfo(DRIVE_DETACH_9_1),
    mkinfo(DRIVE_DETACH_10_0),
    mkinfo(DRIVE_DETACH_10_1),
    mkinfo(DRIVE_DETACH_11_0),
    mkinfo(DRIVE_DETACH_11_1),

    /* fliplist */
    mkinfo(FLIPLIST_ADD),
    mkinfo(FLIPLIST_REMOVE),
    mkinfo(FLIPLIST_NEXT),
    mkinfo(FLIPLIST_PREVIOUS),
    mkinfo(FLIPLIST_LOAD),
    mkinfo(FLIPLIST_SAVE),
    mkinfo(FLIPLIST_CLEAR),

    /* datasette image */
    mkinfo(TAPE_ATTACH_1),
    mkinfo(TAPE_ATTACH_1),
    mkinfo(TAPE_ATTACH_2),
    mkinfo(TAPE_DETACH_1),
    mkinfo(TAPE_DETACH_2),
    mkinfo(TAPE_CREATE_1),
    mkinfo(TAPE_CREATE_2),

    /* datasette controls */
    mkinfo(TAPE_RECORD_1),
    mkinfo(TAPE_RECORD_2),
    mkinfo(TAPE_PLAY_1),
    mkinfo(TAPE_PLAY_2),
    mkinfo(TAPE_REWIND_1),
    mkinfo(TAPE_REWIND_2),
    mkinfo(TAPE_FFWD_1),
    mkinfo(TAPE_FFWD_2),
    mkinfo(TAPE_STOP_1),
    mkinfo(TAPE_STOP_2),
    mkinfo(TAPE_RESET_1),
    mkinfo(TAPE_RESET_2),
    mkinfo(TAPE_RESET_COUNTER_1),
    mkinfo(TAPE_RESET_COUNTER_2),

    /* open monitor */
    mkinfo(MONITOR_OPEN),

    /* reset items */
    mkinfo(RESET_SOFT),
    mkinfo(RESET_HARD),
    mkinfo(RESET_DRIVE_8),
    mkinfo(RESET_DRIVE_9),
    mkinfo(RESET_DRIVE_10),
    mkinfo(RESET_DRIVE_11),

    /* quit emulator */
    mkinfo(QUIT),

    /* edit items */
    mkinfo(EDIT_COPY),
    mkinfo(EDIT_PASTE),

    /* pause, warp, advance-frame */
    mkinfo(PAUSE_TOGGLE),
    mkinfo(ADVANCE_FRAME),
    mkinfo(WARP_MODE_TOGGLE),

    /* CPU speed */
    mkinfo(SPEED_CPU_10),
    mkinfo(SPEED_CPU_20),
    mkinfo(SPEED_CPU_50),
    mkinfo(SPEED_CPU_100),
    mkinfo(SPEED_CPU_200),
    mkinfo(SPEED_CPU_CUSTOM),

    /* video clock */
    mkinfo(SPEED_FPS_50),
    mkinfo(SPEED_FPS_60),
    mkinfo(SPEED_FPS_CUSTOM),
    mkinfo(SPEED_FPS_REAL),

    /* fullscreen, fullscreen decs, restore display */
    mkinfo(FULLSCREEN_TOGGLE),
    mkinfo(FULLSCREEN_DECORATIONS_TOGGLE),
    mkinfo(RESTORE_DISPLAY),

    /* joystick, mouse etc */
    mkinfo(SWAP_CONTROLPORT_TOGGLE),
    mkinfo(MOUSE_GRAB_TOGGLE),
    mkinfo(KEYSET_JOYSTICK_TOGGLE),

    /* settings items */
    mkinfo(SETTINGS_DIALOG),
    mkinfo(SETTINGS_LOAD),
    mkinfo(SETTINGS_LOAD_FROM),
    mkinfo(SETTINGS_LOAD_EXTRA),
    mkinfo(SETTINGS_SAVE),
    mkinfo(SETTINGS_SAVE_TO),
    mkinfo(SETTINGS_DEFAULT),

    /* snapshots, media recording, events */
    mkinfo(SNAPSHOT_LOAD),
    mkinfo(SNAPSHOT_SAVE),
    mkinfo(SNAPSHOT_QUICKLOAD),
    mkinfo(SNAPSHOT_QUICKSAVE),
    mkinfo(HISTORY_RECORD_START),
    mkinfo(HISTORY_RECORD_STOP),
    mkinfo(HISTORY_PLAYBACK_START),
    mkinfo(HISTORY_PLAYBACK_STOP),
    mkinfo(HISTORY_MILESTONE_SET),
    mkinfo(HISTORY_MILESTONE_RESET),
    mkinfo(MEDIA_RECORD),
    mkinfo(MEDIA_STOP),
    mkinfo(SCREENSHOT_QUICKSAVE),

    /* debug items */
#ifdef DEBUG
    mkinfo(DEBUG_TRACE_MODE),
    mkinfo(DEBUG_TRACE_CPU_TOGGLE),
    mkinfo(DEBUG_TRACE_IEC_TOGGLE),
    mkinfo(DEBUG_TRACE_IEEE488_TOGGLE),
    mkinfo(DEBUG_TRACE_DRIVE_8_TOGGLE),
    mkinfo(DEBUG_TRACE_DRIVE_9_TOGGLE),
    mkinfo(DEBUG_TRACE_DRIVE_10_TOGGLE),
    mkinfo(DEBUG_TRACE_DRIVE_11_TOGGLE),
    mkinfo(DEBUG_AUTOPLAYBACK_FRAMES),
    mkinfo(DEBUG_CORE_DUMP_TOGGLE),
    /* DTV-specific */
    mkinfo(DEBUG_BLITTER_LOG_TOGGLE),
    mkinfo(DEBUG_DMA_LOG_TOGGLE),
    mkinfo(DEBUG_FLASH_LOG_TOGGLE),
#endif

    /* Help items */
    mkinfo(HELP_MANUAL),
    mkinfo(HELP_COMMAND_LINE),
    mkinfo(HELP_COMPILE_TIME),
    mkinfo(HELP_HOTKEYS),
    mkinfo(HELP_ABOUT),

    { ACTION_INVALID_ID, NULL, NULL, 0 }
};


/** \brief  Test if \a action is valid for the current machine
 *
 * \param[in]   action  UI action
 *
 * \return  bool
 */
static bool is_current_machine_action(const ui_action_info_internal_t *action)
{
    return (bool)(action->machine & machine_class);
}


/** \brief  Get description of an action
 *
 * Looks up the description for action \a name.
 *
 * \param[in]   name    action name
 *
 * \return  description or `NULL` when \a name not found
 */
const char *ui_action_get_desc(const char *name)
{
    int i = 0;

    if (name == NULL || *name == '\0') {
        return NULL;
    }

    while (action_info_list[i].name != NULL) {
        if (strcmp(action_info_list[i].name, name) == 0) {
            return action_info_list[i].desc;
        }
        i++;
    }
    return NULL;
}


/** \brief  Get list of actions
 *
 * \return  list of (id, name, desc) tuples for UI actions
 *
 * \note    The returned list is allocated with lib_malloc() and should be
 *          freed after use with lib_free(), the members of each element should
 *          not be freed.
 */
ui_action_info_t *ui_action_get_info_list(void)
{
    const ui_action_info_internal_t *action = action_info_list;
    ui_action_info_t *list = NULL;
    size_t valid = 0;
    size_t index = 0;

    /* determine number of valid actions */
    while (action->name != NULL) {
        if (is_current_machine_action(action)) {
            valid++;
        }
        action++;
    }

    /* create list of valid actions */
    list = lib_malloc((valid + 1) * sizeof *list);
    action = action_info_list;
    while (action-> name != NULL) {
        if (is_current_machine_action(action)) {
            list[index].id = action->id;
            list[index].name = action->name;
            list[index].desc = action->desc;
            index++;
        }
        action++;
    }
    /* terminate list */
    list[index].id = -1;
    list[index].name = NULL;
    list[index].desc = NULL;

    return list;
}
