/** \file   uiactions.c
 * \brief   UI action names and descriptions
 *
 * Provides names and descriptions for UI actions.
 *
 * Used by menu structs, hotkeys and joystick mappings. There will be no Doxygen
 * docblocks for most of the defines, since they're self-explanatory. And
 * obviously I will not bitch about keeping the text within 80 columns here :D
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
#include <stdbool.h>

#include "lib.h"
#include "machine.h"

#include "uiactions.h"


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
    { ACTION_SMART_ATTACH,      "smart-attach",         "Attach a medium to the emulator inspecting its type", VICE_MACHINE_ALL },

    /* disk image attach */
    { ACTION_DRIVE_ATTACH_8_0,  "drive-attach-8:0",     "Attach disk to unit 8, drive 0",   VICE_MACHINE_ALL },
    { ACTION_DRIVE_ATTACH_8_1,  "drive-attach-8:1",     "Attach disk to unit 8, drive 1",   VICE_MACHINE_ALL },
    { ACTION_DRIVE_ATTACH_9_0,  "drive-attach-9:0",     "Attach disk to unit 9, drive 0",   VICE_MACHINE_ALL },
    { ACTION_DRIVE_ATTACH_9_1,  "drive-attach-9:1",     "Attach disk to unit 9, drive 1",   VICE_MACHINE_ALL },
    { ACTION_DRIVE_ATTACH_10_0, "drive-attach-10:0",    "Attach disk to unit 10, drive 0",  VICE_MACHINE_ALL },
    { ACTION_DRIVE_ATTACH_10_1, "drive-attach-10:1",    "Attach disk to unit 10, drive 1",  VICE_MACHINE_ALL },
    { ACTION_DRIVE_ATTACH_11_0, "drive-attach-11:0",    "Attach disk to unit 11, drive 0",  VICE_MACHINE_ALL },
    { ACTION_DRIVE_ATTACH_11_1, "drive-attach-11:1",    "Attach disk to unit 11, drive 1",  VICE_MACHINE_ALL },

    /* disk image create & attach */
    { ACTION_DRIVE_CREATE,      "drive-create",         "Create and attach empty disk image",   VICE_MACHINE_ALL },

    /* disk image detach */
    { ACTION_DRIVE_DETACH_8_0,  "drive-detach-8:0",     "Detach disk from unit 8, drive 0",     VICE_MACHINE_ALL },
    { ACTION_DRIVE_DETACH_8_1,  "drive-detach-8:1",     "Detach disk from unit 8, drive 1",     VICE_MACHINE_ALL },
    { ACTION_DRIVE_DETACH_9_0,  "drive-detach-9:0",     "Detach disk from unit 9, drive 0",     VICE_MACHINE_ALL },
    { ACTION_DRIVE_DETACH_9_1,  "drive-detach-9:1",     "Detach disk from unit 9, drive 1",     VICE_MACHINE_ALL },
    { ACTION_DRIVE_DETACH_10_0, "drive-detach-10:0",    "Detach disk from unit 10, drive 0",    VICE_MACHINE_ALL },
    { ACTION_DRIVE_DETACH_10_1, "drive-detach-10:1",    "Detach disk from unit 10, drive 1",    VICE_MACHINE_ALL },
    { ACTION_DRIVE_DETACH_11_0, "drive-detach-11:0",    "Detach disk from unit 11, drive 0",    VICE_MACHINE_ALL },
    { ACTION_DRIVE_DETACH_11_1, "drive-detach-11:1",    "Detach disk from unit 11, drive 1",    VICE_MACHINE_ALL },

    /* fliplist */
    { ACTION_FLIPLIST_ADD,      "fliplist-add",         "Add current disk to fliplist",         VICE_MACHINE_ALL },
    { ACTION_FLIPLIST_REMOVE,   "fliplist-remove",      "Remove current disk from fliplist",    VICE_MACHINE_ALL },
    { ACTION_FLIPLIST_NEXT,     "fliplist-next",        "Attach next disk in fliplist",         VICE_MACHINE_ALL },
    { ACTION_FLIPLIST_PREVIOUS, "fliplist-previous",    "Attach previous disk in fliplist",     VICE_MACHINE_ALL },
    { ACTION_FLIPLIST_LOAD,     "fliplist-load",        "Load fliplist",                        VICE_MACHINE_ALL },
    { ACTION_FLIPLIST_SAVE,     "fliplist-save",        "Save fliplist",                        VICE_MACHINE_ALL },
    { ACTION_FLIPLIST_CLEAR,    "fliplist-clear",       "Clear fliplist",                       VICE_MACHINE_ALL },

    /* datasette image */
    { ACTION_TAPE_ATTACH_1,     "tape-attach-1",        "Attach tape to datasette 1",           (VICE_MACHINE_ALL^VICE_MACHINE_C64DTV^VICE_MACHINE_SCPU64) },
    { ACTION_TAPE_ATTACH_2,     "tape-attach-2",        "Attach tape to datasette 2",           VICE_MACHINE_PET },
    { ACTION_TAPE_DETACH_1,     "tape-detach-1",        "Detach tape from datasette 1",         (VICE_MACHINE_ALL^VICE_MACHINE_C64DTV^VICE_MACHINE_SCPU64) },
    { ACTION_TAPE_DETACH_2,     "tape-detach-2",        "Detach tape from datasette 2",         VICE_MACHINE_PET },
    { ACTION_TAPE_CREATE_1,     "tape-create-1",        "Create tape and attach to datasette 1", (VICE_MACHINE_ALL^VICE_MACHINE_C64DTV^VICE_MACHINE_SCPU64) },
    { ACTION_TAPE_CREATE_2,     "tape-create-2",        "Create tape and attach to datasette 2", VICE_MACHINE_PET },

    /* datasette controls */
    { ACTION_TAPE_RECORD_1,         "tape-record-1",        "Press RECORD on datasette 1",  (VICE_MACHINE_ALL^VICE_MACHINE_C64DTV^VICE_MACHINE_SCPU64) },
    { ACTION_TAPE_RECORD_2,         "tape-record-2",        "Press RECORD on datasette 2",  VICE_MACHINE_PET },
    { ACTION_TAPE_PLAY_1,           "tape-play-1",          "Press PLAY on datasette 1",    (VICE_MACHINE_ALL^VICE_MACHINE_C64DTV^VICE_MACHINE_SCPU64) },
    { ACTION_TAPE_PLAY_2,           "tape-play-2",          "Press PLAY on datasette 2",    VICE_MACHINE_PET },
    { ACTION_TAPE_REWIND_1,         "tape-rewind-1",        "Press REWIND on datasette 1",  (VICE_MACHINE_ALL^VICE_MACHINE_C64DTV^VICE_MACHINE_SCPU64) },
    { ACTION_TAPE_REWIND_2,         "tape-rewind-2",        "Press REWIND on datasette 2",  VICE_MACHINE_PET },
    { ACTION_TAPE_FFWD_1,           "tape-ffwd-1",          "Press FFWD on datasette 1",    (VICE_MACHINE_ALL^VICE_MACHINE_C64DTV^VICE_MACHINE_SCPU64) },
    { ACTION_TAPE_FFWD_2,           "tape-ffwd-2",          "Press FFWD on datasette 2",    VICE_MACHINE_PET },
    { ACTION_TAPE_STOP_1,           "tape-stop-1",          "Press STOP on datasette 1",    (VICE_MACHINE_ALL^VICE_MACHINE_C64DTV^VICE_MACHINE_SCPU64) },
    { ACTION_TAPE_STOP_2,           "tape-stop-2",          "Press STOP on datasette 2",    VICE_MACHINE_PET },
    { ACTION_TAPE_RESET_1,          "tape-reset-1",         "Reset datasette 1",            (VICE_MACHINE_ALL^VICE_MACHINE_C64DTV^VICE_MACHINE_SCPU64) },
    { ACTION_TAPE_RESET_2,          "tape-reset-2",         "Reset datasette 2",            VICE_MACHINE_PET },
    { ACTION_TAPE_RESET_COUNTER_1,  "tape-reset-counter-1", "Reset datasette 1 counter",    (VICE_MACHINE_ALL^VICE_MACHINE_C64DTV^VICE_MACHINE_SCPU64) },
    { ACTION_TAPE_RESET_COUNTER_2,  "tape-reset-counter-2", "Reset datasette 2 counter",    VICE_MACHINE_PET },

    /* cartridge items */
    { ACTION_CART_ATTACH,   "cart-attach",  "Attach cartridge",                 (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_SCPU64|VICE_MACHINE_VIC20|VICE_MACHINE_PLUS4|VICE_MACHINE_CBM6x0) },
    { ACTION_CART_DETACH,   "cart-detach",  "Detach cartridge",                 (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_SCPU64|VICE_MACHINE_VIC20|VICE_MACHINE_PLUS4|VICE_MACHINE_CBM6x0) },
    { ACTION_CART_FREEZE,   "cart-freeze",  "Press cartridge freeze button",    (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_SCPU64|VICE_MACHINE_VIC20|VICE_MACHINE_PLUS4|VICE_MACHINE_CBM6x0) },

    /* open monitor */
    { ACTION_MONITOR_OPEN,      "monitor-open",         "Open monitor",                         VICE_MACHINE_ALL },

    /* reset items */
    { ACTION_RESET_SOFT,        "reset-soft",           "Soft-reset the machine",               VICE_MACHINE_ALL },
    { ACTION_RESET_HARD,        "reset-hard",           "Hard-reset the machine",               VICE_MACHINE_ALL },
    { ACTION_RESET_DRIVE_8,     "reset-drive-8",        "Reset drive 8",                        VICE_MACHINE_ALL },
    { ACTION_RESET_DRIVE_9,     "reset-drive-9",        "Reset drive 9",                        VICE_MACHINE_ALL },
    { ACTION_RESET_DRIVE_10,    "reset-drive-10",       "Reset drive 10",                       VICE_MACHINE_ALL },
    { ACTION_RESET_DRIVE_11,    "reset-drive-11",       "Reset drive 11",                       VICE_MACHINE_ALL },

    /* quit emulator */
    { ACTION_QUIT,              "quit",                 "Quit emulator",                        VICE_MACHINE_ALL },

    /* edit items */
    { ACTION_EDIT_COPY,         "edit-copy",            "Copy screen content to clipboard",     VICE_MACHINE_ALL },
    { ACTION_EDIT_PASTE,        "edit-paste",           "Paste clipboard content into machine", VICE_MACHINE_ALL },

    /* pause, warp, advance-frame */
    { ACTION_PAUSE_TOGGLE,      "pause-toggle",         "Toggle Pause",                         VICE_MACHINE_ALL },
    { ACTION_ADVANCE_FRAME,     "advance-frame",        "Advance emulation one frame",          VICE_MACHINE_ALL },
    { ACTION_WARP_MODE_TOGGLE,  "warp-mode-toggle",     "Toggle Warp Mode",                     VICE_MACHINE_ALL },

    /* CPU speed presets and custom speed */
    { ACTION_SPEED_CPU_10,      "speed-cpu-10",         "Set CPU speed to 10%",                 VICE_MACHINE_ALL },
    { ACTION_SPEED_CPU_20,      "speed-cpu-20",         "Set CPU speed to 20%",                 VICE_MACHINE_ALL },
    { ACTION_SPEED_CPU_50,      "speed-cpu-50",         "Set CPU speed to 50%",                 VICE_MACHINE_ALL },
    { ACTION_SPEED_CPU_100,     "speed-cpu-100",        "Set CPU speed to 100%",                VICE_MACHINE_ALL },
    { ACTION_SPEED_CPU_200,     "speed-cpu-200",        "Set CPU speed to 200%",                VICE_MACHINE_ALL },
    { ACTION_SPEED_CPU_CUSTOM,  "speed-cpu-custom",     "Set custom CPU speed",                 VICE_MACHINE_ALL },

    /* video clock */
    { ACTION_SPEED_FPS_50,      "speed-fps-50",         "Set video clock to 50Hz",              VICE_MACHINE_ALL },
    { ACTION_SPEED_FPS_60,      "speed-fps-60",         "Set video clock to 60Hz",              VICE_MACHINE_ALL },
    { ACTION_SPEED_FPS_CUSTOM,  "speed-fps-custom",     "Set custom video clock",               VICE_MACHINE_ALL },
    { ACTION_SPEED_FPS_REAL,    "speed-fps-50",         "Set real video clock",                 VICE_MACHINE_ALL },

    /* fullscreen, fullscreen decs, restore display */
    { ACTION_FULLSCREEN_TOGGLE,             "fullscreen-toggle",                "Toggle fullscreen",                        VICE_MACHINE_ALL },
    { ACTION_FULLSCREEN_DECORATIONS_TOGGLE, "fullscreen-decorations-toggle",    "Show menu/status in fullscreen",           VICE_MACHINE_ALL },
    { ACTION_SHOW_STATUSBAR_TOGGLE,         "show-statusbar-toggle",            "Show status bar",                          VICE_MACHINE_ALL },
    { ACTION_RESTORE_DISPLAY,               "restore-display",                  "Resize application window to fit content", VICE_MACHINE_ALL },

    /* joystick, mouse etc */
    { ACTION_SWAP_CONTROLPORT_TOGGLE,   "swap-controlport-toggle",  "Swap controlport joysticks",   (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_C64DTV|VICE_MACHINE_SCPU64|VICE_MACHINE_PLUS4|VICE_MACHINE_CBM5x0) },
    { ACTION_MOUSE_GRAB_TOGGLE,         "mouse-grab-toggle",        "Toggle Mouse Grab",            VICE_MACHINE_ALL },
    { ACTION_KEYSET_JOYSTICK_TOGGLE,    "keyset-joystick-toggle",   "Allow keyset joysticks",       VICE_MACHINE_ALL },

    /* settings items */
    { ACTION_SETTINGS_DIALOG,       "settings-dialog",      "Open settings",                        VICE_MACHINE_ALL },
    { ACTION_SETTINGS_LOAD,         "settings-load",        "Load settings",                        VICE_MACHINE_ALL },
    { ACTION_SETTINGS_LOAD_FROM,    "settings-load-from",   "Load settings from alternate file",    VICE_MACHINE_ALL },
    { ACTION_SETTINGS_LOAD_EXTRA,   "settings-load-extra",  "Load additional settings",             VICE_MACHINE_ALL },
    { ACTION_SETTINGS_SAVE,         "settings-save",        "Save settings",                        VICE_MACHINE_ALL },
    { ACTION_SETTINGS_SAVE_TO,      "settings-save-to",     "Save settings to alternate file",      VICE_MACHINE_ALL },
    { ACTION_SETTINGS_DEFAULT,      "settings-default",     "Restore default settings",             VICE_MACHINE_ALL },

    /* snapshots, media recording, events */
    { ACTION_SNAPSHOT_LOAD,             "snapshot-load",            "Load snapshot file",               VICE_MACHINE_ALL },
    { ACTION_SNAPSHOT_SAVE,             "snapshot-save",            "Save snapshot file",               VICE_MACHINE_ALL },
    { ACTION_SNAPSHOT_QUICKLOAD,        "snapshot-quickload",       "Quickload snapshot",               VICE_MACHINE_ALL },
    { ACTION_SNAPSHOT_QUICKSAVE,        "snapshot-quicksave",       "Quicksave snapshot",               VICE_MACHINE_ALL },
    { ACTION_HISTORY_RECORD_START,      "history-record-start",     "Start recording events",           VICE_MACHINE_ALL },
    { ACTION_HISTORY_RECORD_STOP,       "history-record-stop",      "Stop recording events",            VICE_MACHINE_ALL },
    { ACTION_HISTORY_PLAYBACK_START,    "history-playback-start",   "Start playing back events",        VICE_MACHINE_ALL },
    { ACTION_HISTORY_PLAYBACK_STOP,     "history-playback-stop",    "Stop playing back events",         VICE_MACHINE_ALL },
    { ACTION_HISTORY_MILESTONE_SET,     "history-milestone-set",    "Set recording milestone",          VICE_MACHINE_ALL },
    { ACTION_HISTORY_MILESTONE_RESET,   "history-milestone-reset",  "Return to recording milestone",    VICE_MACHINE_ALL },
    { ACTION_MEDIA_RECORD,              "media-record",             "Record media",                     VICE_MACHINE_ALL },
    { ACTION_MEDIA_STOP,                "media-stop",               "Stop media recording",             VICE_MACHINE_ALL },
    { ACTION_SCREENSHOT_QUICKSAVE,      "screenshot-quicksave",     "Quiksave screenshot",              VICE_MACHINE_ALL },

    /* debug items */
#ifdef DEBUG
    { ACTION_DEBUG_TRACE_MODE,              "debug-trace-mode",             "Select machine/drive CPU trace mode",  VICE_MACHINE_ALL },
    { ACTION_DEBUG_TRACE_CPU_TOGGLE,        "debug-trace-cpu-toggle",       "Toggle CPU trace",                     VICE_MACHINE_ALL },
    { ACTION_DEBUG_TRACE_IEC_TOGGLE,        "debug-trace-iec-toggle",       "Toggle IEC bus trace",                 VICE_MACHINE_ALL },
    { ACTION_DEBUG_TRACE_IEEE488_TOGGLE,    "debug-trace-ieee488-toggle",   "Toggle IEEE-488 bus trace",            (VICE_MACHINE_ALL^VICE_MACHINE_C64DTV) },
    { ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE,    "debug-trace-drive-8-toggle",   "Toggle drive 8 CPU trace",             VICE_MACHINE_ALL },
    { ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE,    "debug-trace-drive-9-toggle",   "Toggle drive 9 CPU trace",             VICE_MACHINE_ALL },
    { ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE,   "debug-trace-drive-10-toggle",  "Toggle drive 10 CPU trace",            VICE_MACHINE_ALL },
    { ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE,   "debug-trace-drive-11-toggle",  "Toggle drive 11 CPU trace",            VICE_MACHINE_ALL },
    { ACTION_DEBUG_AUTOPLAYBACK_FRAMES,     "debug-autoplayback-frames",    "Set autoplayback frames",              VICE_MACHINE_ALL },
    { ACTION_DEBUG_CORE_DUMP_TOGGLE,        "debug-core-dump-toggle",       "Toggle saving core dump",              VICE_MACHINE_ALL },
    /* DTV-specific */
    { ACTION_DEBUG_BLITTER_LOG_TOGGLE,      "debug-blitter-log-toggle",     "Toggle blitter logging",               VICE_MACHINE_C64DTV },
    { ACTION_DEBUG_DMA_LOG_TOGGLE,          "debug-dma-log-toggle",         "Toggle DMA logging",                   VICE_MACHINE_C64DTV },
    { ACTION_DEBUG_FLASH_LOG_TOGGLE,        "debug-flash-log-toggle",       "Toggle Flash logging",                 VICE_MACHINE_C64DTV },
#endif

    /* Help items */
    { ACTION_HELP_MANUAL,       "help-manual",          "Browse VICE manual",           VICE_MACHINE_ALL },
    { ACTION_HELP_COMMAND_LINE, "help-command-line",    "Show command line options",    VICE_MACHINE_ALL },
    { ACTION_HELP_COMPILE_TIME, "help-compile-time",    "Show compile time features",   VICE_MACHINE_ALL },
    /* XXX: Is this still valid? We have a hotkeys editor now. */
    { ACTION_HELP_HOTKEYS,      "help-hotkeys",         "Show hotkeys",                 VICE_MACHINE_ALL },
    { ACTION_HELP_ABOUT,        "help-about",           "Show About dialog",            VICE_MACHINE_ALL },

    { ACTION_INVALID, NULL, NULL, 0 }
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


/** \brief  Get "internal" info about a UI action
 *
 * \param[in]   id  action ID
 *
 * \return  pointer to info or `NULL` on failure
 */
static const ui_action_info_internal_t *get_info_internal(int id)
{
    if (id > ACTION_NONE) {
        int i = 0;

        while (action_info_list[i].id > ACTION_NONE) {
            if (action_info_list[i].id == id) {
                return &action_info_list[i];
            }
            i++;
        }
    }
    return NULL;
}


/** \brief  Get action ID by name
 *
 * Get the action name as used by the vhk files.
 *
 * \param[in]   name    action name
 *
 * \return  ID or -1 when not found
 */
int ui_action_get_id(const char *name)
{
    if (name != NULL && *name != '\0') {
        int i = 0;

        while (action_info_list[i].id > ACTION_NONE) {
            if (strcmp(action_info_list[i].name, name) == 0) {
                return action_info_list[i].id;
            }
            i++;
        }
    }
    return ACTION_INVALID;
}


/** \brief  Get action name by ID
 *
 * \param[in]   id  action ID
 *
 * \return  action name or NULL when not found
 *
 * \note    Also returns NULL for NONE and INVALID
 */
const char *ui_action_get_name(int id)
{
    const ui_action_info_internal_t *action = get_info_internal(id);

    return action != NULL ? action->name : NULL;
}


/** \brief  Get description of an action
 *
 * Looks up the description for action \a id.
 *
 * \param[in]   name    action name
 *
 * \return  description or `NULL` when \a id not found
 */
const char *ui_action_get_desc(int id)
{
    const ui_action_info_internal_t *action = get_info_internal(id);

    return action != NULL ? action->desc : NULL;
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
