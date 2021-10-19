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
 * Generate '{ ACTION_FOO, ACTION_FOO_DESC, ACTION_FOO_MASK }' for use in the
 * actions array.
 *
 * \param[in]   x   action name define
 */
#define mkinfo(x) { x, x##_DESC, x##_MASK }


/** \brief  Mapping of action names to descriptions and machine support
 *
 * The 'machine' members is a bitmask that indicates which machines support
 * the action.
 */
typedef struct ui_action_info_internal_s {
    const char *name;       /**< action name */
    const char *desc;       /**< action description */
    uint32_t    machine;    /**< bitmask indicating which machines support the
                                 action */
} ui_action_info_internal_t;


/** \brief  List of UI actions
 */
static const ui_action_info_internal_t action_info_list[] = {

    /* smart attach */
    mkinfo(ACTION_SMART_ATTACH),

    /* disk image attach */
    mkinfo(ACTION_DRIVE_ATTACH_8_0),
    mkinfo(ACTION_DRIVE_ATTACH_8_1),
    mkinfo(ACTION_DRIVE_ATTACH_9_0),
    mkinfo(ACTION_DRIVE_ATTACH_9_1),
    mkinfo(ACTION_DRIVE_ATTACH_10_0),
    mkinfo(ACTION_DRIVE_ATTACH_10_1),
    mkinfo(ACTION_DRIVE_ATTACH_11_0),
    mkinfo(ACTION_DRIVE_ATTACH_11_1),

    /* disk image create & attach */
    mkinfo(ACTION_DRIVE_CREATE),

    /* disk image detach */
    mkinfo(ACTION_DRIVE_DETACH_8_0),
    mkinfo(ACTION_DRIVE_DETACH_8_1),
    mkinfo(ACTION_DRIVE_DETACH_9_0),
    mkinfo(ACTION_DRIVE_DETACH_9_1),
    mkinfo(ACTION_DRIVE_DETACH_10_0),
    mkinfo(ACTION_DRIVE_DETACH_10_1),
    mkinfo(ACTION_DRIVE_DETACH_11_0),
    mkinfo(ACTION_DRIVE_DETACH_11_1),

    /* fliplist */
    mkinfo(ACTION_FLIPLIST_ADD),
    mkinfo(ACTION_FLIPLIST_REMOVE),
    mkinfo(ACTION_FLIPLIST_NEXT),
    mkinfo(ACTION_FLIPLIST_PREVIOUS),
    mkinfo(ACTION_FLIPLIST_LOAD),
    mkinfo(ACTION_FLIPLIST_SAVE),
    mkinfo(ACTION_FLIPLIST_CLEAR),

    /* datasette image */
    mkinfo(ACTION_TAPE_ATTACH_1),
    mkinfo(ACTION_TAPE_ATTACH_1),
    mkinfo(ACTION_TAPE_ATTACH_2),
    mkinfo(ACTION_TAPE_DETACH_1),
    mkinfo(ACTION_TAPE_DETACH_2),
    mkinfo(ACTION_TAPE_CREATE_1),
    mkinfo(ACTION_TAPE_CREATE_2),

    /* datasette controls */
    mkinfo(ACTION_TAPE_RECORD_1),
    mkinfo(ACTION_TAPE_RECORD_2),
    mkinfo(ACTION_TAPE_PLAY_1),
    mkinfo(ACTION_TAPE_PLAY_2),
    mkinfo(ACTION_TAPE_REWIND_1),
    mkinfo(ACTION_TAPE_REWIND_2),
    mkinfo(ACTION_TAPE_FFWD_1),
    mkinfo(ACTION_TAPE_FFWD_2),
    mkinfo(ACTION_TAPE_STOP_1),
    mkinfo(ACTION_TAPE_STOP_2),
    mkinfo(ACTION_TAPE_RESET_1),
    mkinfo(ACTION_TAPE_RESET_2),
    mkinfo(ACTION_TAPE_RESET_COUNTER_1),
    mkinfo(ACTION_TAPE_RESET_COUNTER_2),

    /* open monitor */
    mkinfo(ACTION_MONITOR_OPEN),

    /* reset items */
    mkinfo(ACTION_RESET_SOFT),
    mkinfo(ACTION_RESET_HARD),
    mkinfo(ACTION_RESET_DRIVE_8),
    mkinfo(ACTION_RESET_DRIVE_9),
    mkinfo(ACTION_RESET_DRIVE_10),
    mkinfo(ACTION_RESET_DRIVE_11),

    /* quit emulator */
    mkinfo(ACTION_QUIT),

    /* edit items */
    mkinfo(ACTION_EDIT_COPY),
    mkinfo(ACTION_EDIT_PASTE),

    /* pause, warp, advance-frame */
    mkinfo(ACTION_PAUSE_TOGGLE),
    mkinfo(ACTION_ADVANCE_FRAME),
    mkinfo(ACTION_WARP_MODE_TOGGLE),

    /* fullscreen, fullscreen decs, restore display */
    mkinfo(ACTION_FULLSCREEN_TOGGLE),
    mkinfo(ACTION_FULLSCREEN_DECORATIONS_TOGGLE),
    mkinfo(ACTION_RESTORE_DISPLAY),

    /* joystick, mouse etc */
    mkinfo(ACTION_SWAP_CONTROLPORT_TOGGLE),
    mkinfo(ACTION_SWAP_USERPORT_TOGGLE),
    mkinfo(ACTION_MOUSE_GRAB_TOGGLE),
    mkinfo(ACTION_KEYSET_JOYSTICK_TOGGLE),

    /* settings items */
    mkinfo(ACTION_SETTINGS_DIALOG),
    mkinfo(ACTION_SETTINGS_LOAD),
    mkinfo(ACTION_SETTINGS_LOAD_FROM),
    mkinfo(ACTION_SETTINGS_LOAD_EXTRA),
    mkinfo(ACTION_SETTINGS_SAVE),
    mkinfo(ACTION_SETTINGS_SAVE_TO),
    mkinfo(ACTION_SETTINGS_DEFAULT),

    /* snapshots, media recording, events */
    mkinfo(ACTION_SNAPSHOT_LOAD),
    mkinfo(ACTION_SNAPSHOT_SAVE),
    mkinfo(ACTION_SNAPSHOT_QUICKLOAD),
    mkinfo(ACTION_SNAPSHOT_QUICKSAVE),
    mkinfo(ACTION_HISTORY_RECORD_START),
    mkinfo(ACTION_HISTORY_RECORD_STOP),
    mkinfo(ACTION_HISTORY_PLAYBACK_START),
    mkinfo(ACTION_HISTORY_PLAYBACK_STOP),
    mkinfo(ACTION_HISTORY_MILESTONE_SET),
    mkinfo(ACTION_HISTORY_MILESTONE_RESET),
    mkinfo(ACTION_MEDIA_RECORD),
    mkinfo(ACTION_MEDIA_STOP),
    mkinfo(ACTION_SCREENSHOT_QUICKSAVE),

    /* debug items */
#ifdef DEBUG
    mkinfo(ACTION_DEBUG_TRACE_MODE),
    mkinfo(ACTION_DEBUG_TRACE_CPU_TOGGLE),
    mkinfo(ACTION_DEBUG_TRACE_IEC_TOGGLE),
    mkinfo(ACTION_DEBUG_TRACE_IEEE488_TOGGLE),
    mkinfo(ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE),
    mkinfo(ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE),
    mkinfo(ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE),
    mkinfo(ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE),
    mkinfo(ACTION_DEBUG_AUTOPLAYBACK_FRAMES),
    mkinfo(ACTION_DEBUG_CORE_DUMP_TOGGLE),
    /* DTV-specific */
    mkinfo(ACTION_DEBUG_BLITTER_LOG_TOGGLE),
    mkinfo(ACTION_DEBUG_DMA_LOG_TOGGLE),
    mkinfo(ACTION_DEBUG_FLASH_LOG_TOGGLE),
#endif

    /* Help items */
    mkinfo(ACTION_HELP_MANUAL),
    mkinfo(ACTION_HELP_COMMAND_LINE),
    mkinfo(ACTION_HELP_COMPILE_TIME),
    mkinfo(ACTION_HELP_HOTKEYS),
    mkinfo(ACTION_HELP_ABOUT),

    { NULL, NULL, 0 }
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
 * \return  list of (name,desc) tuples for UI actions
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
            list[index].name = action->name;
            list[index].desc = action->desc;
            index++;
        }
        action++;
    }
    /* terminate list */
    list[index].name = NULL;
    list[index].desc = NULL;

    return list;
}
