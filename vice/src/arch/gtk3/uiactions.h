/** \file   uiactions.h
 * \brief   Gtk3 UI action names and descriptions - header
 *
 * Provides names and descriptions for Gtk3 UI actions.
 *
 * Used by menu items and custom hotkeys. There will be no Doxygen docblocks
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

#ifndef VICE_UIACTIONS_H
#define VICE_UIACTIONS_H

/* this header is required if the macro IS_ACTION_NAME_CHAR() is used: */
#include <ctype.h>

/* for the VICE_MACHINE_* masks */
#include "machine.h"


/** \brief  Mapping of action names to descriptions
 */
typedef struct ui_action_info_s {
    const char *name;       /**< action name */
    const char *desc;       /**< action description */
} ui_action_info_t;


/** \brief  Check for valid action name character
 *
 * Check if \a ch is a valid character in an action name.
 *
 * Supported characters are:
 * * a-z
 * * A-Z
 * * 0-9
 * * '_', '-' and ':'
 */
#define IS_ACTION_NAME_CHAR(ch) \
    (isalpha(ch) || isdigit(ch) || ch == '_' || ch == '-' || ch == ':')


/*
 * List of symbolic constants for action names, descriptions and emu support
 */

#define ACTION_QUIT                     "quit"
#define ACTION_QUIT_DESC                "Quit emulator"
#define ACTION_QUIT_MASK                VICE_MACHINE_ALL

#define ACTION_SETTINGS_DIALOG          "settings-dialog"
#define ACTION_SETTINGS_DIALOG_DESC     "Open settings"
#define ACTION_SETTINGS_DIALOG_MASK     VICE_MACHINE_ALL

#define ACTION_SETTINGS_LOAD            "settings-load"
#define ACTION_SETTINGS_LOAD_DESC       "Load settings"
#define ACTION_SETTINGS_LOAD_MASK       VICE_MACHINE_ALL

#define ACTION_SETTINGS_LOAD_FROM       "settings-load-from"
#define ACTION_SETTINGS_LOAD_FROM_DESC  "Load settings from alternate file"
#define ACTION_SETTINGS_LOAD_FROM_MASK  VICE_MACHINE_ALL

#define ACTION_SETTINGS_LOAD_EXTRA      "settings-load-extra"
#define ACTION_SETTINGS_LOAD_EXTRA_DESC "Load additional settings"
#define ACTION_SETTINGS_LOAD_EXTRA_MASK VICE_MACHINE_ALL

#define ACTION_SETTINGS_SAVE            "settings-save"
#define ACTION_SETTINGS_SAVE_DESC       "Save settings"
#define ACTION_SETTINGS_SAVE_MASK       VICE_MACHINE_ALL

#define ACTION_SETTINGS_SAVE_TO         "settings-save-to"
#define ACTION_SETTINGS_SAVE_TO_DESC    "Save settings to alternate file"
#define ACTION_SETTINGS_SAVE_TO_MASK    VICE_MACHINE_ALL

#define ACTION_SETTINGS_DEFAULT         "settings-default"
#define ACTION_SETTINGS_DEFAULT_DESC    "Restore default settings"
#define ACTION_SETTINGS_DEFAULT_MASK    VICE_MACHINE_ALL


/* emulator run settings: pause, warp, speed(?) */
#define ACTION_PAUSE_TOGGLE             "pause-toggle"
#define ACTION_PAUSE_TOGGLE_DESC        "Toggle Pause"
#define ACTION_PAUSE_TOGGLE_MASK        VICE_MACHINE_ALL

#define ACTION_WARP_MODE_TOGGLE         "warp-mode-toggle"
#define ACTION_WARP_MODE_TOGGLE_DESC    "Toggle Warp Mode"
#define ACTION_WARP_MODE_TOGGLE_MASK    VICE_MACHINE_ALL

#define ACTION_ADVANCE_FRAME            "advance-frame"
#define ACTION_ADVANCE_FRAME_DESC       "Advance emulation one frame"
#define ACTION_ADVANCE_FRAME_MASK       VICE_MACHINE_ALL

#define ACTION_SPEED_CPU_10             "speed-cpu-10"
#define ACTION_SPEED_CPU_10_DESC        "Set CPU speed to 10%"
#define ACTION_SPEED_CPU_10_MASK        VICE_MACHINE_ALL

#define ACTION_SPEED_CPU_20             "speed-cpu-20"
#define ACTION_SPEED_CPU_20_DESC        "Set CPU speed to 20%"
#define ACTION_SPEED_CPU_20_MASK        VICE_MACHINE_ALL

#define ACTION_SPEED_CPU_50             "speed-cpu-50"
#define ACTION_SPEED_CPU_50_DESC        "Set CPU speed to 50%"
#define ACTION_SPEED_CPU_50_MASK        VICE_MACHINE_ALL

#define ACTION_SPEED_CPU_100            "speed-cpu-100"
#define ACTION_SPEED_CPU_100_DESC       "Set CPU speed to 100%"
#define ACTION_SPEED_CPU_100_MASK       VICE_MACHINE_ALL

#define ACTION_SPEED_CPU_200            "speed-cpu-200"
#define ACTION_SPEED_CPU_200_DESC       "Set CPU speed to 200%"
#define ACTION_SPEED_CPU_200_MASK       VICE_MACHINE_ALL

#define ACTION_SPEED_CPU_CUSTOM         "speed-cpu-custom"
#define ACTION_SPEED_CPU_CUSTOM_DESC    "Set custom CPU speed"
#define ACTION_SPEED_CPU_CUSTOM_MASK    VICE_MACHINE_ALL

#define ACTION_SPEED_FPS_50             "speed-fps-50"
#define ACTION_SPEED_FPS_50_DESC        "Set video clock to 50Hz"
#define ACTION_SPEED_FPS_50_MASK        VICE_MACHINE_ALL

#define ACTION_SPEED_FPS_60             "speed-fps-60"
#define ACTION_SPEED_FPS_60_DESC        "Set video clock to 60Hz"
#define ACTION_SPEED_FPS_60_MASK        VICE_MACHINE_ALL

#define ACTION_SPEED_FPS_CUSTOM         "speed-fps-custom"
#define ACTION_SPEED_FPS_CUSTOM_DESC    "Set custom video clock"
#define ACTION_SPEED_FPS_CUSTOM_MASK    VICE_MACHINE_ALL

#define ACTION_SPEED_FPS_REAL           "speed-fps-real"
#define ACTION_SPEED_FPS_REAL_DESC      "Set real video clock"
#define ACTION_SPEED_FPS_REAL_MASK      VICE_MACHINE_ALL



/* host display */
#define ACTION_FULLSCREEN_TOGGLE        "fullscreen-toggle"
#define ACTION_FULLSCREEN_TOGGLE_DESC   "Toggle fullscreen"
#define ACTION_FULLSCREEN_TOGGLE_MASK   VICE_MACHINE_ALL


#define ACTION_FULLSCREEN_DECORATIONS_TOGGLE        "fullscreen-decorations-toggle"
#define ACTION_FULLSCREEN_DECORATIONS_TOGGLE_DESC   "Show menu/status in fullscreen"
#define ACTION_FULLSCREEN_DECORATIONS_TOGGLE_MASK   VICE_MACHINE_ALL

#define ACTION_RESTORE_DISPLAY          "restore-display"
#define ACTION_RESTORE_DISPLAY_DESC     "Resize application window to fit contents"
#define ACTION_RESTORE_DISPLAY_MASK     VICE_MACHINE_ALL

/* joystick and mouse */
#define ACTION_MOUSE_GRAB_TOGGLE            "mouse-grab-toggle"
#define ACTION_MOUSE_GRAB_TOGGLE_DESC       "Toggle Mouse Grab"
#define ACTION_MOUSE_GRAB_TOGGLE_MASK       VICE_MACHINE_ALL

#define ACTION_SWAP_CONTROLPORT_TOGGLE      "swap-controlport-toggle"
#define ACTION_SWAP_CONTROLPORT_TOGGLE_DESC "Swap controlport joysticks"
#define ACTION_SWAP_CONTROLPORT_TOGGLE_MASK (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_C64DTV|VICE_MACHINE_SCPU64|VICE_MACHINE_PLUS4|VICE_MACHINE_CBM5x0)

#define ACTION_KEYSET_JOYSTICK_TOGGLE       "keyset-joystick-toggle"
#define ACTION_KEYSET_JOYSTICK_TOGGLE_DESC  "Allow keyset joystick"
#define ACTION_KEYSET_JOYSTICK_TOGGLE_MASK  VICE_MACHINE_ALL

/* 'smart' attach */
#define ACTION_SMART_ATTACH                 "smart-attach"
#define ACTION_SMART_ATTACH_DESC            "Attach a medium to the emulator inspecting its type"
#define ACTION_SMART_ATTACH_MASK            VICE_MACHINE_ALL

/* Drive->Attach items */
#define ACTION_DRIVE_ATTACH_8_0         "drive-attach-8:0"
#define ACTION_DRIVE_ATTACH_8_0_DESC    "Attach disk to unit 8, drive 0"
#define ACTION_DRIVE_ATTACH_8_0_MASK    VICE_MACHINE_ALL

#define ACTION_DRIVE_ATTACH_8_1         "drive-attach-8:1"
#define ACTION_DRIVE_ATTACH_8_1_DESC    "Attach disk to unit 8, drive 1"
#define ACTION_DRIVE_ATTACH_8_1_MASK    VICE_MACHINE_ALL

#define ACTION_DRIVE_ATTACH_9_0         "drive-attach-9:0"
#define ACTION_DRIVE_ATTACH_9_0_DESC    "Attach disk to unit 9, drive 0"
#define ACTION_DRIVE_ATTACH_9_0_MASK    VICE_MACHINE_ALL

#define ACTION_DRIVE_ATTACH_9_1         "drive-attach-9:1"
#define ACTION_DRIVE_ATTACH_9_1_DESC    "Attach disk to unit 9, drive 1"
#define ACTION_DRIVE_ATTACH_9_1_MASK        VICE_MACHINE_ALL

#define ACTION_DRIVE_ATTACH_10_0        "drive-attach-10:0"
#define ACTION_DRIVE_ATTACH_10_0_DESC   "Attach disk to unit 10, drive 0"
#define ACTION_DRIVE_ATTACH_10_0_MASK   VICE_MACHINE_ALL

#define ACTION_DRIVE_ATTACH_10_1        "drive-attach-10:1"
#define ACTION_DRIVE_ATTACH_10_1_DESC   "Attach disk to unit 10, drive 1"
#define ACTION_DRIVE_ATTACH_10_1_MASK   VICE_MACHINE_ALL

#define ACTION_DRIVE_ATTACH_11_0        "drive-attach-11:0"
#define ACTION_DRIVE_ATTACH_11_0_DESC   "Attach disk to unit 11, drive 0"
#define ACTION_DRIVE_ATTACH_11_0_MASK   VICE_MACHINE_ALL

#define ACTION_DRIVE_ATTACH_11_1        "drive-attach-11:1"
#define ACTION_DRIVE_ATTACH_11_1_DESC   "Attach disk to unit 11, drive 1"
#define ACTION_DRIVE_ATTACH_11_1_MASK   VICE_MACHINE_ALL

/* create disk image */
#define ACTION_DRIVE_CREATE             "drive-create"
#define ACTION_DRIVE_CREATE_DESC        "Create and attach empty disk image"
#define ACTION_DRIVE_CREATE_MASK        VICE_MACHINE_ALL

/* Drive->Detach items */
#define ACTION_DRIVE_DETACH_8_0         "drive-detach-8:0"
#define ACTION_DRIVE_DETACH_8_0_DESC    "Detach disk from unit 8, drive 0"
#define ACTION_DRIVE_DETACH_8_0_MASK    VICE_MACHINE_ALL

#define ACTION_DRIVE_DETACH_8_1         "drive-detach-8:1"
#define ACTION_DRIVE_DETACH_8_1_DESC    "Detach disk from unit 8, drive 1"
#define ACTION_DRIVE_DETACH_8_1_MASK    VICE_MACHINE_ALL

#define ACTION_DRIVE_DETACH_9_0         "drive-detach-9:0"
#define ACTION_DRIVE_DETACH_9_0_DESC    "Detach disk from unit 9, drive 0"
#define ACTION_DRIVE_DETACH_9_0_MASK    VICE_MACHINE_ALL

#define ACTION_DRIVE_DETACH_9_1         "drive-detach-9:1"
#define ACTION_DRIVE_DETACH_9_1_DESC    "Detach disk from unit 9, drive 1"
#define ACTION_DRIVE_DETACH_9_1_MASK    VICE_MACHINE_ALL

#define ACTION_DRIVE_DETACH_10_0        "drive-detach-10:0"
#define ACTION_DRIVE_DETACH_10_0_DESC   "Detach disk from unit 10, drive 0"
#define ACTION_DRIVE_DETACH_10_0_MASK   VICE_MACHINE_ALL

#define ACTION_DRIVE_DETACH_10_1        "drive-detach-10:1"
#define ACTION_DRIVE_DETACH_10_1_DESC   "Detach disk from unit 10, drive 1"
#define ACTION_DRIVE_DETACH_10_1_MASK   VICE_MACHINE_ALL

#define ACTION_DRIVE_DETACH_11_0        "drive-detach-11:0"
#define ACTION_DRIVE_DETACH_11_0_DESC   "Detach disk from unit 11, drive 0"
#define ACTION_DRIVE_DETACH_11_0_MASK   VICE_MACHINE_ALL

#define ACTION_DRIVE_DETACH_11_1        "drive-detach-11:1"
#define ACTION_DRIVE_DETACH_11_1_DESC   "Detach disk from unit 11, drive 1"
#define ACTION_DRIVE_DETACH_11_1_MASK   VICE_MACHINE_ALL

#define ACTION_DRIVE_DETACH_ALL         "drive-detach-all"
#define ACTION_DRIVE_DETACH_ALL_DESC    "Detach all disks"
#define ACTION_DRIVE_DETACH_ALL_MASK    VICE_MACHINE_ALL

/* Fliplist items */
#define ACTION_FLIPLIST_ADD             "fliplist-add"
#define ACTION_FLIPLIST_ADD_DESC        "Add current disk to fliplist"
#define ACTION_FLIPLIST_ADD_MASK        VICE_MACHINE_ALL

#define ACTION_FLIPLIST_REMOVE          "fliplist-remove"
#define ACTION_FLIPLIST_REMOVE_DESC     "Remove current disk from fliplist"
#define ACTION_FLIPLIST_REMOVE_MASK     VICE_MACHINE_ALL

#define ACTION_FLIPLIST_NEXT            "fliplist-next"
#define ACTION_FLIPLIST_NEXT_DESC       "Attach next disk in fliplist"
#define ACTION_FLIPLIST_NEXT_MASK       VICE_MACHINE_ALL

#define ACTION_FLIPLIST_PREVIOUS        "fliplist-previous"
#define ACTION_FLIPLIST_PREVIOUS_DESC   "Attach previous disk in fliplist"
#define ACTION_FLIPLIST_PREVIOUS_MASK   VICE_MACHINE_ALL

#define ACTION_FLIPLIST_LOAD            "fliplist-load"
#define ACTION_FLIPLIST_LOAD_DESC       "Load fliplist"
#define ACTION_FLIPLIST_LOAD_MASK       VICE_MACHINE_ALL

#define ACTION_FLIPLIST_SAVE            "fliplist-save"
#define ACTION_FLIPLIST_SAVE_DESC       "Save fliplist"
#define ACTION_FLIPLIST_SAVE_MASK       VICE_MACHINE_ALL

#define ACTION_FLIPLIST_CLEAR           "fliplist-clear"
#define ACTION_FLIPLIST_CLEAR_DESC      "Clear fliplist"
#define ACTION_FLIPLIST_CLEAR_MASK      VICE_MACHINE_ALL

/* Datasette items
 *
 * Currently only one datasette is supported, but PETs actually (can) have two,
 * so let's plan ahead.
 */
#define ACTION_TAPE_ATTACH_1        "tape-attach-1"
#define ACTION_TAPE_ATTACH_1_DESC   "Attach tape to datasette 1"
#define ACTION_TAPE_ATTACH_1_MASK   (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_C128|VICE_MACHINE_PLUS4|VICE_MACHINE_VIC20|VICE_MACHINE_CBM5x0|VICE_MACHINE_CBM6x0|VICE_MACHINE_PET)

#define ACTION_TAPE_ATTACH_2        "tape-attach-2"
#define ACTION_TAPE_ATTACH_2_DESC   "Attach tape to datasette 2"
#define ACTION_TAPE_ATTACH_2_MASK   VICE_MACHINE_PET

#define ACTION_TAPE_DETACH_1        "tape-detach-1"
#define ACTION_TAPE_DETACH_1_DESC   "Detach tape from datasette 1"
#define ACTION_TAPE_DETACH_1_MASK   (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_C128|VICE_MACHINE_PLUS4|VICE_MACHINE_VIC20|VICE_MACHINE_CBM5x0|VICE_MACHINE_CBM6x0|VICE_MACHINE_PET)

#define ACTION_TAPE_DETACH_2        "tape-detach-2"
#define ACTION_TAPE_DETACH_2_DESC   "Detach tape from datasette 2"
#define ACTION_TAPE_DETACH_2_MASK   VICE_MACHINE_PET

#define ACTION_TAPE_CREATE_1        "tape-create-1"
#define ACTION_TAPE_CREATE_1_DESC   "Create tape and attach to datasette 1"
#define ACTION_TAPE_CREATE_1_MASK   (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_C128|VICE_MACHINE_PLUS4|VICE_MACHINE_VIC20|VICE_MACHINE_CBM5x0|VICE_MACHINE_CBM6x0|VICE_MACHINE_PET)

#define ACTION_TAPE_CREATE_2        "tape-create-2"
#define ACTION_TAPE_CREATE_2_DESC   "Create tape and attach to datasette 2"
#define ACTION_TAPE_CREATE_2_MASK   VICE_MACHINE_PET

/* Datasette control items */
#define ACTION_TAPE_RECORD_1        "tape-record-1"
#define ACTION_TAPE_RECORD_1_DESC   "Press RECORD on datasette 1"
#define ACTION_TAPE_RECORD_1_MASK   (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_C128|VICE_MACHINE_PLUS4|VICE_MACHINE_VIC20|VICE_MACHINE_CBM5x0|VICE_MACHINE_CBM6x0|VICE_MACHINE_PET)

#define ACTION_TAPE_RECORD_2        "tape-record-2"
#define ACTION_TAPE_RECORD_2_DESC   "Press RECORD on datasette 2"
#define ACTION_TAPE_RECORD_2_MASK   VICE_MACHINE_PET

#define ACTION_TAPE_PLAY_1          "tape-play-1"
#define ACTION_TAPE_PLAY_1_DESC     "Press PLAY on datasette 1"
#define ACTION_TAPE_PLAY_1_MASK     (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_C128|VICE_MACHINE_PLUS4|VICE_MACHINE_VIC20|VICE_MACHINE_CBM5x0|VICE_MACHINE_CBM6x0|VICE_MACHINE_PET)

#define ACTION_TAPE_PLAY_2          "tape-play-2"
#define ACTION_TAPE_PLAY_2_DESC     "Press PLAY on datasette 2"
#define ACTION_TAPE_PLAY_2_MASK     VICE_MACHINE_PET

#define ACTION_TAPE_REWIND_1        "tape-rewind-1"
#define ACTION_TAPE_REWIND_1_DESC   "Press REWIND on datasette 1"
#define ACTION_TAPE_REWIND_1_MASK   (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_C128|VICE_MACHINE_PLUS4|VICE_MACHINE_VIC20|VICE_MACHINE_CBM5x0|VICE_MACHINE_CBM6x0|VICE_MACHINE_PET)

#define ACTION_TAPE_REWIND_2        "tape-rewind-2"
#define ACTION_TAPE_REWIND_2_DESC   "Press REWIND on datasette 2"
#define ACTION_TAPE_REWIND_2_MASK   VICE_MACHINE_PET

#define ACTION_TAPE_FFWD_1          "tape-ffwd-1"
#define ACTION_TAPE_FFWD_1_DESC     "Press FFWD on datasette 1"
#define ACTION_TAPE_FFWD_1_MASK     (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_C128|VICE_MACHINE_PLUS4|VICE_MACHINE_VIC20|VICE_MACHINE_CBM5x0|VICE_MACHINE_CBM6x0|VICE_MACHINE_PET)

#define ACTION_TAPE_FFWD_2          "tape-ffwd-2"
#define ACTION_TAPE_FFWD_2_DESC     "Press FFWD on datasette 2"
#define ACTION_TAPE_FFWD_2_MASK     VICE_MACHINE_PET

#define ACTION_TAPE_STOP_1          "tape-stop-1"
#define ACTION_TAPE_STOP_1_DESC     "Press STOP on datasette 1"
#define ACTION_TAPE_STOP_1_MASK     (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_C128|VICE_MACHINE_PLUS4|VICE_MACHINE_VIC20|VICE_MACHINE_CBM5x0|VICE_MACHINE_CBM6x0|VICE_MACHINE_PET)

#define ACTION_TAPE_STOP_2          "tape-stop-2"
#define ACTION_TAPE_STOP_2_DESC     "Press STOP on datasette 2"
#define ACTION_TAPE_STOP_2_MASK     VICE_MACHINE_PET

/* XXX: no items for EJECT */

#define ACTION_TAPE_RESET_1         "tape-reset-1"
#define ACTION_TAPE_RESET_1_DESC    "Reset datasette 1"
#define ACTION_TAPE_RESET_1_MASK    (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_C128|VICE_MACHINE_PLUS4|VICE_MACHINE_VIC20|VICE_MACHINE_CBM5x0|VICE_MACHINE_CBM6x0|VICE_MACHINE_PET)

#define ACTION_TAPE_RESET_2         "tape-reset-1"
#define ACTION_TAPE_RESET_2_DESC    "Reset datasette 2"
#define ACTION_TAPE_RESET_2_MASK    VICE_MACHINE_PET

#define ACTION_TAPE_RESET_COUNTER_1         "tape-reset-counter-1"
#define ACTION_TAPE_RESET_COUNTER_1_DESC    "Reset datasette 1 counter"
#define ACTION_TAPE_RESET_COUNTER_1_MASK    (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_C128|VICE_MACHINE_PLUS4|VICE_MACHINE_VIC20|VICE_MACHINE_CBM5x0|VICE_MACHINE_CBM6x0|VICE_MACHINE_PET)

#define ACTION_TAPE_RESET_COUNTER_2         "tape-reset-counter-2"
#define ACTION_TAPE_RESET_COUNTER_2_DESC    "Reset datasette 2 counter"
#define ACTION_TAPE_RESET_COUNTER_2_MASK    VICE_MACHINE_PET


/* Cartridge items */
#define ACTION_CART_ATTACH          "cart-attach"
#define ACTION_CART_ATTACH_DESC     "Attach cartridge"
#define ACTION_CART_ATTACH_MASK     (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_SCPU64|VICE_MACHINE_VIC20|VICE_MACHINE_PLUS4|VICE_MACHINE_CBM6x0)

#define ACTION_CART_DETACH          "cart-detach"
#define ACTION_CART_DETACH_DESC     "Detach cartridge"
#define ACTION_CART_DETACH_MASK     (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_SCPU64|VICE_MACHINE_VIC20|VICE_MACHINE_PLUS4|VICE_MACHINE_CBM6x0)

#define ACTION_CART_FREEZE          "cart-freeze"
#define ACTION_CART_FREEZE_DESC     "Press cartridge freeze button"
#define ACTION_CART_FREEZE_MASK     (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_SCPU64|VICE_MACHINE_VIC20|VICE_MACHINE_PLUS4|VICE_MACHINE_CBM6x0)

/* Monitor */
#define ACTION_MONITOR_OPEN         "monitor-open"
#define ACTION_MONITOR_OPEN_DESC    "Open monitor"
#define ACTION_MONITOR_OPEN_MASK    VICE_MACHINE_ALL

/* Reset */
#define ACTION_RESET_SOFT           "reset-soft"
#define ACTION_RESET_SOFT_DESC      "Soft reset the machine"
#define ACTION_RESET_SOFT_MASK      VICE_MACHINE_ALL

#define ACTION_RESET_HARD           "reset-hard"
#define ACTION_RESET_HARD_DESC      "Hard reset the machine"
#define ACTION_RESET_HARD_MASK      VICE_MACHINE_ALL

#define ACTION_RESET_DRIVE_8        "reset-drive-8"
#define ACTION_RESET_DRIVE_8_DESC   "Reset drive 8"
#define ACTION_RESET_DRIVE_8_MASK   VICE_MACHINE_ALL

#define ACTION_RESET_DRIVE_9        "reset-drive-9"
#define ACTION_RESET_DRIVE_9_DESC   "Reset drive 9"
#define ACTION_RESET_DRIVE_9_MASK   VICE_MACHINE_ALL

#define ACTION_RESET_DRIVE_10       "reset-drive-10"
#define ACTION_RESET_DRIVE_10_DESC  "Reset drive 10"
#define ACTION_RESET_DRIVE_10_MASK  VICE_MACHINE_ALL

#define ACTION_RESET_DRIVE_11       "reset-drive-11"
#define ACTION_RESET_DRIVE_11_DESC  "Reset drive 11"
#define ACTION_RESET_DRIVE_11_MASK  VICE_MACHINE_ALL


/* Edit items */
#define ACTION_EDIT_COPY            "edit-copy"
#define ACTION_EDIT_COPY_DESC       "Copy screen content to clipboard"
#define ACTION_EDIT_COPY_MASK       VICE_MACHINE_ALL

#define ACTION_EDIT_PASTE           "edit-paste"
#define ACTION_EDIT_PASTE_DESC      "Paste clipboard content into machine"
#define ACTION_EDIT_PASTE_MASK      VICE_MACHINE_ALL

/* Snapshots */
#define ACTION_SNAPSHOT_LOAD            "snapshot-load"
#define ACTION_SNAPSHOT_LOAD_DESC       "Load snapshot file"
#define ACTION_SNAPSHOT_LOAD_MASK       VICE_MACHINE_ALL

#define ACTION_SNAPSHOT_SAVE            "snapshot-save"
#define ACTION_SNAPSHOT_SAVE_DESC       "Save snapshot file"
#define ACTION_SNAPSHOT_SAVE_MASK       VICE_MACHINE_ALL

#define ACTION_SNAPSHOT_QUICKLOAD       "snapshot-quickload"
#define ACTION_SNAPSHOT_QUICKLOAD_DESC  "Quickload snapshot"
#define ACTION_SNAPSHOT_QUICKLOAD_MASK  VICE_MACHINE_ALL

#define ACTION_SNAPSHOT_QUICKSAVE       "snapshot-quicksave"
#define ACTION_SNAPSHOT_QUICKSAVE_DESC  "Quicksave snapshot"
#define ACTION_SNAPSHOT_QUICKSAVE_MASK  VICE_MACHINE_ALL


/* History recording */
#define ACTION_HISTORY_RECORD_START         "history-record-start"
#define ACTION_HISTORY_RECORD_START_DESC    "Start recording events"
#define ACTION_HISTORY_RECORD_START_MASK    VICE_MACHINE_ALL

#define ACTION_HISTORY_RECORD_STOP          "history-record-stop"
#define ACTION_HISTORY_RECORD_STOP_DESC     "Stop recording events"
#define ACTION_HISTORY_RECORD_STOP_MASK     VICE_MACHINE_ALL

#define ACTION_HISTORY_PLAYBACK_START       "history-playback-start"
#define ACTION_HISTORY_PLAYBACK_START_DESC  "Start playing back events"
#define ACTION_HISTORY_PLAYBACK_START_MASK  VICE_MACHINE_ALL

#define ACTION_HISTORY_PLAYBACK_STOP        "history-playback-stop"
#define ACTION_HISTORY_PLAYBACK_STOP_DESC   "Stop playing back events"
#define ACTION_HISTORY_PLAYBACK_STOP_MASK   VICE_MACHINE_ALL

#define ACTION_HISTORY_MILESTONE_SET        "history-milestone-set"
#define ACTION_HISTORY_MILESTONE_SET_DESC   "Set recording milestone"
#define ACTION_HISTORY_MILESTONE_SET_MASK   VICE_MACHINE_ALL

#define ACTION_HISTORY_MILESTONE_RESET      "history-milestone-reset"
#define ACTION_HISTORY_MILESTONE_RESET_DESC "Return to recording milestone"
#define ACTION_HISTORY_MILESTONE_RESET_MASK VICE_MACHINE_ALL


/* Media recording, screenshits */
#define ACTION_MEDIA_RECORD                 "media-record"
#define ACTION_MEDIA_RECORD_DESC            "Record media"
#define ACTION_MEDIA_RECORD_MASK            VICE_MACHINE_ALL

#define ACTION_MEDIA_STOP                   "media-stop"
#define ACTION_MEDIA_STOP_DESC              "Stop media recording"
#define ACTION_MEDIA_STOP_MASK              VICE_MACHINE_ALL

#define ACTION_SCREENSHOT_QUICKSAVE         "screenshot-quicksave"
#define ACTION_SCREENSHOT_QUICKSAVE_DESC    "Save screenshot in current working directory"
#define ACTION_SCREENSHOT_QUICKSAVE_MASK    VICE_MACHINE_ALL


/* Debugging items */

#define ACTION_DEBUG_TRACE_MODE             "debug-trace-mode"
#define ACTION_DEBUG_TRACE_MODE_DESC        "Select CPU/Drive trac mode"
#define ACTION_DEBUG_TRACE_MODE_MASK        VICE_MACHINE_ALL

#define ACTION_DEBUG_TRACE_CPU_TOGGLE       "debug-trace-cpu-toggle"
#define ACTION_DEBUG_TRACE_CPU_TOGGLE_DESC  "Toggle CPU trace"
#define ACTION_DEBUG_TRACE_CPU_TOGGLE_MASK  VICE_MACHINE_ALL

#define ACTION_DEBUG_TRACE_IEC_TOGGLE       "debug-trace-iec-toggle"
#define ACTION_DEBUG_TRACE_IEC_TOGGLE_DESC  "Toggle IEC bus trace"
#define ACTION_DEBUG_TRACE_IEC_TOGGLE_MASK  VICE_MACHINE_ALL

#define ACTION_DEBUG_TRACE_IEEE488_TOGGLE       "debug-trace-ieee488-toggle"
#define ACTION_DEBUG_TRACE_IEEE488_TOGGLE_DESC  "Toggle IEEE-488 bus trace"
#define ACTION_DEBUG_TRACE_IEEE488_TOGGLE_MASK  (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_SCPU64|VICE_MACHINE_C128|VICE_MACHINE_PLUS4|VICE_MACHINE_VIC20|VICE_MACHINE_CBM5x0|VICE_MACHINE_CBM6x0|VICE_MACHINE_PET)

#define ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE       "debug-trace-drive-8-toggle"
#define ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE_DESC  "Toggle Drive 8 CPU trace"
#define ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE_MASK  VICE_MACHINE_ALL

#define ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE       "debug-trace-drive-9-toggle"
#define ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE_DESC  "Toggle Drive 9 CPU trace"
#define ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE_MASK  VICE_MACHINE_ALL

#define ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE      "debug-trace-drive-10-toggle"
#define ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE_DESC "Toggle Drive 10 CPU trace"
#define ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE_MASK VICE_MACHINE_ALL

#define ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE      "debug-trace-drive-11-toggle"
#define ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE_DESC "Toggle Drive 11 CPU trace"
#define ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE_MASK VICE_MACHINE_ALL

#define ACTION_DEBUG_AUTOPLAYBACK_FRAMES        "debug-autoplayback-frames"
#define ACTION_DEBUG_AUTOPLAYBACK_FRAMES_DESC   "Set autoplayback frames"
#define ACTION_DEBUG_AUTOPLAYBACK_FRAMES_MASK   VICE_MACHINE_ALL

#define ACTION_DEBUG_CORE_DUMP_TOGGLE           "debug-core-dump-toggle"
#define ACTION_DEBUG_CORE_DUMP_TOGGLE_DESC      "Toggle saving core dump"
#define ACTION_DEBUG_CORE_DUMP_TOGGLE_MASK      VICE_MACHINE_ALL

/* x64dtv-specific debug items */
#define ACTION_DEBUG_BLITTER_LOG_TOGGLE         "debug-blitter-log-toggle"
#define ACTION_DEBUG_BLITTER_LOG_TOGGLE_DESC    "Toggle blitter logging"
#define ACTION_DEBUG_BLITTER_LOG_TOGGLE_MASK     VICE_MACHINE_C64DTV

#define ACTION_DEBUG_DMA_LOG_TOGGLE             "debug-dma-log-toggle"
#define ACTION_DEBUG_DMA_LOG_TOGGLE_DESC        "Toggle DMA logging"
#define ACTION_DEBUG_DMA_LOG_TOGGLE_MASK        VICE_MACHINE_C64DTV

#define ACTION_DEBUG_FLASH_LOG_TOGGLE           "debug-flash-log-toggle"
#define ACTION_DEBUG_FLASH_LOG_TOGGLE_DESC      "Toggle Flash logging"
#define ACTION_DEBUG_FLASH_LOG_TOGGLE_MASK      VICE_MACHINE_C64DTV



/* Help items */

#define ACTION_HELP_MANUAL                      "help-manual"
#define ACTION_HELP_MANUAL_DESC                 "Browse VICE manual"
#define ACTION_HELP_MANUAL_MASK                 VICE_MACHINE_ALL

#define ACTION_HELP_COMMAND_LINE                "help-command-line"
#define ACTION_HELP_COMMAND_LINE_DESC           "Command line options"
#define ACTION_HELP_COMMAND_LINE_MASK           VICE_MACHINE_ALL

#define ACTION_HELP_COMPILE_TIME                "help-compile-time"
#define ACTION_HELP_COMPILE_TIME_DESC           "Compile time features"
#define ACTION_HELP_COMPILE_TIME_MASK           VICE_MACHINE_ALL

/* (This one should probably go since we have a GUI hotkeys list/editor now) */
#define ACTION_HELP_HOTKEYS                     "help-hotkeys"
#define ACTION_HELP_HOTKEYS_DESC                "Hotkeys"
#define ACTION_HELP_HOTKEYS_MASK                VICE_MACHINE_ALL

#define ACTION_HELP_ABOUT                       "help-about"
#define ACTION_HELP_ABOUT_DESC                  "Show about dialog"
#define ACTION_HELP_ABOUT_MASK                  VICE_MACHINE_ALL



/* API */

const char *        ui_action_get_desc(const char *name);
ui_action_info_t *  ui_action_get_info_list(void);

/* TODO: implement the following: */
bool                ui_action_def(const char *name, const char *hotkey);
bool                ui_action_undef(const char *name);
bool                ui_action_redef(const char *name, const char *hotkey);

#endif
