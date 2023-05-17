/** \file   uimachinemenu.c
 * \brief   Native GTK3 menus for machine emulators (not vsid)
 *
 * \author  Marcus Sutton <loggedoubt@gmail.com>
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
#include <stdbool.h>
#include <stddef.h>

#include "archdep.h"
#include "debug.h"
#include "machine.h"
#include "uiactions.h"
#include "uimenu.h"

#include "uimachinemenu.h"

/*
 * The following are translation unit local so we can create functions that
 * modify menu contents or even functions that alter the top bar itself.
 */

/** \brief  Main menu bar widget
 *
 * Contains the submenus on the menu main bar
 *
 * This one lives until ui_exit() or thereabouts
 */
static GtkWidget *main_menu_bar = NULL;


/* {{{ disk_detach_submenu[] */
/** \brief  File->Detach disk submenu
 */
static const ui_menu_item_t disk_detach_submenu[] = {
    { "Drive 8:0", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_DETACH_8_0,
      NULL, false },
    { "Drive 8:1", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_DETACH_8_1,
      NULL, false },
    { "Drive 9:0", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_DETACH_9_0,
      NULL, false },
    { "Drive 9:1", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_DETACH_9_1,
      NULL,false },
    { "Drive 10:0", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_DETACH_10_0,
      NULL, false },
    { "Drive 10:1", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_DETACH_10_1,
      NULL, false },
    { "Drive 11:0", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_DETACH_11_0,
      NULL, false },
    { "Drive 11:1", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_DETACH_11_1,
      NULL, false },
    { "Detach all", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_DETACH_ALL,
      NULL, false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ disk_attach_submenu[] */
/** \brief  File->Attach disk submenu
 */
static const ui_menu_item_t disk_attach_submenu[] = {
    { "Drive #8", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_ATTACH_8_0,
      NULL, true },
    { "Drive #9", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_ATTACH_9_0,
      NULL, true },
    { "Drive #10", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_ATTACH_10_0,
      NULL, true },
    { "Drive #11", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_ATTACH_11_0,
      NULL, true },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ disk_fliplist_submenu[] */
/** \brief  File->Flip list submenu
 */
static const ui_menu_item_t disk_fliplist_submenu[] = {
    { "Add current image (unit #8, drive 0)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_ADD_8_0,
      NULL, false },
    { "Remove current image (unit #8, drive 0)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_REMOVE_8_0,
      NULL, false },
    { "Attach next image (unit #8, drive 0)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_NEXT_8_0,
      NULL, false },
    { "Attach previous image (unit #8, drive 0)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_PREVIOUS_8_0,
      NULL, false },
    { "Clear fliplist (unit #8, drive 0)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_CLEAR_8_0,
      NULL, false },
    { "Load flip list file for unit #8, drive 0...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_LOAD_8_0,
      NULL, true },
    { "Save flip list file of unit #8, drive 0...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_SAVE_8_0,
      NULL, true },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ datasette_1_control_submenu[] */
/** \brief  File->Datasette control submenu for port #1
 */
static const ui_menu_item_t datasette_1_control_submenu[] = {
    { "Stop", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_STOP_1,
      NULL, false },
    { "Start", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_PLAY_1,
      NULL, false },
    { "Forward", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_FFWD_1,
      NULL, false },
    { "Rewind", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_REWIND_1,
      NULL, false },
    { "Record", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RECORD_1,
      NULL, false },
    { "Reset", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RESET_1,
      NULL, false },
    { "Reset Counter", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RESET_COUNTER_1,
      NULL, false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ datasette_2_control_submenu[] */
/** \brief  File->Datasette control submenu for port #2
 */
static const ui_menu_item_t datasette_2_control_submenu[] = {
    { "Stop", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_STOP_1,
      NULL, false },
    { "Start", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_PLAY_1,
      NULL, false },
    { "Forward", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_FFWD_1,
      NULL, false },
    { "Rewind", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_REWIND_1,
      NULL, false },
    { "Record", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RECORD_1,
      NULL, false },
    { "Reset", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RESET_1,
      NULL, false },
    { "Reset Counter", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RESET_COUNTER_1,
      NULL, false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ reset_submenu[] */
/** \brief  File->Reset submenu
 */
static const ui_menu_item_t reset_submenu[] = {
    { "Soft reset", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_SOFT,
      NULL, false },
    { "Hard reset", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_HARD,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "Reset drive #8", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_DRIVE_8,
      NULL, false },
    { "Reset drive #9", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_DRIVE_9,
      NULL, false },
    { "Reset drive #10", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_DRIVE_10,
      NULL, false },
    { "Reset drive #11", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_DRIVE_11,
      NULL, false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ file_menu_head[] */
/** \brief  'File' menu - head section
 */
static const ui_menu_item_t file_menu_head[] = {
    { "Smart attach ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SMART_ATTACH,
      NULL, true },

    UI_MENU_SEPARATOR,

    { "Attach disk image", UI_MENU_TYPE_SUBMENU,
      0,
      disk_attach_submenu, false },
    { "Create and attach an empty disk image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_CREATE,
      NULL, false },
    { "Detach disk image", UI_MENU_TYPE_SUBMENU,
      0,
      disk_detach_submenu, false },
    { "Flip list", UI_MENU_TYPE_SUBMENU,
      0,
      disk_fliplist_submenu, false },

    UI_MENU_SEPARATOR,

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ file_menu_tape[] */
/** \brief  'File' menu - tape section
 */
static const ui_menu_item_t file_menu_tape[] = {
    { "Attach datasette image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_ATTACH_1,
      NULL, true },
    { "Create and attach datasette image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_CREATE_1,
      NULL, true },
    { "Detach datasette image", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_DETACH_1,
      NULL, false },
    { "Datasette controls", UI_MENU_TYPE_SUBMENU,
      0,
      datasette_1_control_submenu, false },

    UI_MENU_SEPARATOR,  /* Required since this menu gets inserted between
                           disk menu items and cartridge items on emulators
                           that have a datasette port. */
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ file_menu_tape_xpet[] */
/** \brief  'File' menu - tape section for xpet
 */
static const ui_menu_item_t file_menu_tape_xpet[] = {
    { "Attach datasette #1 image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_ATTACH_1,
      NULL, true },
    { "Create and attach datasette #1 image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_CREATE_1,
      NULL, true },
    { "Detach datasette #1 image", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_DETACH_1,
      NULL,  false },
    { "Datasette #1 controls", UI_MENU_TYPE_SUBMENU,
      0,
      datasette_1_control_submenu, false },

    UI_MENU_SEPARATOR,

    { "Attach datasette #2 image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_ATTACH_2,
      NULL, true },
    { "Create and attach datasette #2 image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_CREATE_2,
      NULL, true },
    { "Detach datasette #2 image", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_DETACH_2,
      NULL, false },
    { "Datasette #2 controls", UI_MENU_TYPE_SUBMENU,
      0,
      datasette_2_control_submenu, false },

    UI_MENU_SEPARATOR,  /* Required since this menu gets inserted between
                           disk menu items and cartridge items on emulators
                           that have a datasette port. */
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ file_menu_cart */
/** \brief  'File' menu - cartridge section
 *
 * All machines except C64DTV and PET.
 */
static const ui_menu_item_t file_menu_cart[] = {
    /* cart */
    { "Attach cartridge image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_CART_ATTACH,
      NULL, true },
    { "Detach cartridge image(s)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_CART_DETACH,
      NULL, false },
    { "Cartridge freeze", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_CART_FREEZE,
      NULL, false },

    UI_MENU_SEPARATOR,

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ printer_submenu */
/** \brief  'File' menu - printer submenu (with userport printer)
 *
 * C64, C64SC, SCPU64, C128, VIC20, PET, CBM6x0.
 */
static const ui_menu_item_t printer_submenu[] = {
    { "Send formfeed to printer #4", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_PRINTER_FORMFEED_4,
      NULL, false },
    { "Send formfeed to printer #5", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_PRINTER_FORMFEED_5,
      NULL, false },
    { "Send formfeed to plotter #6", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_PRINTER_FORMFEED_6,
      NULL, false },
    { "Send formfeed to userport printer", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_PRINTER_FORMFEED_USERPORT,
      NULL, false },
    UI_MENU_SEPARATOR,
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ file_menu_printer_no_userport */
/** \brief  'File' menu - printer submenu (without userport printer)
 *
 * C64DTV, PLUS4, CBM5x0.
 */
static const ui_menu_item_t printer_submenu_no_userport[] = {
    { "Send formfeed to printer #4", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_PRINTER_FORMFEED_4,
      NULL, false },
    { "Send formfeed to printer #5", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_PRINTER_FORMFEED_5,
      NULL, false },
    { "Send formfeed to plotter #6", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_PRINTER_FORMFEED_6,
      NULL, false },
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ file_menu_print */
/** \brief  'File' menu - printer section (with userport printer)
 *
 * C64, C64SC, SCPU64, C128, VIC20, PET, CBM6x0.
 */
static const ui_menu_item_t file_menu_printer[] = {
    { "Printer/plotter",    UI_MENU_TYPE_SUBMENU,
      0,
      printer_submenu, false },
    UI_MENU_SEPARATOR,
    UI_MENU_TERMINATOR,
};
/* }}} */

/* {{{ file_menu_printer_no_userport */
/** \brief  'File' menu - printer section (without userport printer)
 *
 * C64DTV, PLUS4, CBM5x0.
 */
static const ui_menu_item_t file_menu_printer_no_userport[] = {
    { "Printer/plotter",    UI_MENU_TYPE_SUBMENU,
      0,
      printer_submenu_no_userport, false },
    UI_MENU_SEPARATOR,
    UI_MENU_TERMINATOR,
};
/* }}} */

/* {{{ file_menu_tail */
/** \brief  'File' menu - tail section
 */
static const ui_menu_item_t file_menu_tail[] = {
    /* monitor */
    { "Activate monitor", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_MONITOR_OPEN,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "Reset", UI_MENU_TYPE_SUBMENU,
      0,
      reset_submenu, false },

    UI_MENU_SEPARATOR,

    { "Exit emulator", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_QUIT,
      NULL, true },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ edit_menu[] */
/** \brief  'Edit' menu
 */
static const ui_menu_item_t edit_menu[] = {
    { "Copy", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_EDIT_COPY,
      NULL, false },
    { "Paste", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_EDIT_PASTE,
      NULL, false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ snapshot_menu[] */
/** \brief  'Snapshot' menu
 */
static ui_menu_item_t snapshot_menu[] = {
    { "Load snapshot image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SNAPSHOT_LOAD,
      NULL, false },
    { "Save snapshot image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SNAPSHOT_SAVE,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "Quickload snapshot", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SNAPSHOT_QUICKLOAD,
      NULL, false },
    { "Quicksave snapshot", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SNAPSHOT_QUICKSAVE,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "Start recording events", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_RECORD_START,
      NULL, false },
    { "Stop recording events", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_RECORD_STOP,
      NULL, false },
    { "Start playing back events", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_PLAYBACK_START,
      NULL, false },
    { "Stop playing back events", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_PLAYBACK_STOP,
      NULL, false },
    { "Set recording milestone", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_MILESTONE_SET,
      NULL, false },
    { "Return to milestone", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_MILESTONE_RESET,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "Save/Record media ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_MEDIA_RECORD,
      NULL, false },
    { "Stop media recording", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_MEDIA_STOP,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "Quicksave screenshot", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SCREENSHOT_QUICKSAVE,
      NULL, false },

    UI_MENU_TERMINATOR
};
/* }}} */

/** \brief  Index in the speed submenu for the "$MACHINE_NAME FPS" item
 *
 * Bit of a hack since the menu item labels are reused for all emus and we can't
 * dynamically set them via some printf()-ish construct
 */
#define MACHINE_FPS_INDEX   7

static const ui_menu_item_t speed_submenu[] = {
    { "200% CPU", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_CPU_200,
      NULL, false },
    { "100% CPU", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_CPU_100,
      NULL, false },
    { "50% CPU", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_CPU_50,
      NULL, false },
    { "20% CPU", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_CPU_20,
      NULL, false },
    { "10% CPU", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_CPU_10,
      NULL, false },
    { "Custom CPU speed ...", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_CPU_CUSTOM,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "MACHINE_NAME FPS", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_FPS_REAL,
      NULL, false },
    { "50 FPS", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_FPS_50,
      NULL, false },
    { "60 FPS", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_FPS_60,
      NULL, false },
    { "Custom FPS", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_FPS_CUSTOM,
      NULL, false },

    UI_MENU_TERMINATOR
};

/* {{{ settings_menu_head[] */
/** \brief  'Settings' menu - head section
 */
static const ui_menu_item_t settings_menu_head[] = {
    { "Fullscreen", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_FULLSCREEN_TOGGLE,
      NULL, true },
    { "Restore display state", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESTORE_DISPLAY,
      NULL, true },
    { "Show menu/status in fullscreen", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_FULLSCREEN_DECORATIONS_TOGGLE,
      NULL, true },
    { "Show status bar", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_SHOW_STATUSBAR_TOGGLE,
      NULL, true },

    UI_MENU_SEPARATOR,

    { "Warp mode", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_WARP_MODE_TOGGLE,
      NULL, false },
    { "Pause emulation", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_PAUSE_TOGGLE,
      NULL, false },
    { "Advance frame", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_ADVANCE_FRAME,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "Emulation speed", UI_MENU_TYPE_SUBMENU,
      0,
      speed_submenu, false },

    UI_MENU_SEPARATOR,

    { "Mouse grab", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_MOUSE_GRAB_TOGGLE,
      NULL, false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ settings_menu_joy_swap[] */

/** \brief  Settings menu - joystick controlport swap
 *
 * Valid for x64/x64sc/x64dtv/xscpu64/x128/xplus4/xcbm5x0
 */
static const ui_menu_item_t settings_menu_joy_swap[] = {
    { "Swap joysticks", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_SWAP_CONTROLPORT_TOGGLE,
      NULL, false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ settings_menu_non_vsid[] */
/** \brief  'Settings' menu section before the tail section
 *
 * Only valid for non-VSID
 */
static const ui_menu_item_t settings_menu_non_vsid[] = {
    /* Needs to go here to avoid duplicate action names */
    { "Allow keyset joysticks", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_KEYSET_JOYSTICK_TOGGLE,
      NULL, false },

    UI_MENU_SEPARATOR,

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ settings_menu_tail[] */
/** \brief  'Settings' menu tail section
 */
static const ui_menu_item_t settings_menu_tail[] = {
   /* the settings dialog */
    { "Settings ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_DIALOG,
      NULL, true },
    { "Load settings", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_LOAD,
      NULL, false },
    { "Load settings from ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_LOAD_FROM,
      NULL, true },
    { "Load extra settings from ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_LOAD_EXTRA,
      NULL, true },
    { "Save settings", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_SAVE,
      NULL, false },
    { "Save settings to ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_SAVE_TO,
      NULL, true },
    { "Restore default settings", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_DEFAULT,
      NULL, true },

    UI_MENU_TERMINATOR
};
/* }}} */

#ifdef DEBUG

/* {{{ debug_menu[] */
/** \brief  'Debug' menu items for emu's except x64dtv
 */
static const ui_menu_item_t debug_menu[] = {
    { "Trace mode ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DEBUG_TRACE_MODE,
      NULL, true },

    UI_MENU_SEPARATOR,

    { "Main CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_CPU_TOGGLE,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "IEC bus trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_IEC_TOGGLE,
      NULL, false },
    { "IEEE-488 bus trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_IEEE488_TOGGLE,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "Drive #8 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE,
      NULL, false },
    { "Drive #9 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE,
      NULL, false },
    { "Drive #10 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE,
      NULL, false },
    { "Drive #11 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "Autoplay playback frames...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DEBUG_AUTOPLAYBACK_FRAMES,
      NULL, true },
    { "Save core dump", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_CORE_DUMP_TOGGLE,
      NULL, false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ debug_menu_c64dtv[] */
/** \brief  'Debug' menu items for x64dtv
 */
static const ui_menu_item_t debug_menu_c64dtv[] = {
    { "Trace mode ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DEBUG_TRACE_MODE,
      NULL, true },

    UI_MENU_SEPARATOR,

    { "Main CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_CPU_TOGGLE,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "IEC bus trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_IEC_TOGGLE,
      NULL, false },
    { "Drive #8 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE,
      NULL, false },
    { "Drive #9 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE,
      NULL, false },
    { "Drive #10 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE,
      NULL, false },
    { "Drive #11 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "Blitter log", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_BLITTER_LOG_TOGGLE,
      NULL, false },
    { "DMA log", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_DMA_LOG_TOGGLE,
      NULL, false },
    { "Flash log", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_FLASH_LOG_TOGGLE,
      NULL, false },

    UI_MENU_SEPARATOR,

    { "Autoplay playback frames ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DEBUG_AUTOPLAYBACK_FRAMES,
      NULL, true },
    { "Save core dump", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_CORE_DUMP_TOGGLE,
      NULL, false },

    UI_MENU_TERMINATOR
};
/* }}} */
#endif


/* {{{ help_menu[] */
/** \brief  'Help' menu items
 */
static const ui_menu_item_t help_menu[] = {
    { "Browse manual", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_MANUAL,
      NULL, true },
    { "Command line options...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_COMMAND_LINE,
      NULL, true },
    { "Compile time features...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_COMPILE_TIME,
      NULL, true },
    { "Hotkeys ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_HOTKEYS,
      NULL, true },
    { "About VICE", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_ABOUT,
      NULL, true },

    UI_MENU_TERMINATOR
};
/* }}} */

/** \brief  'File' menu - tape section pointer
 *
 * Set by ui_machine_menu_bar_create().
 */
static const ui_menu_item_t *file_menu_tape_section = NULL;

/** \brief  'File' menu - cart section pointer
 *
 * Set by ui_machine_menu_bar_create().
 */
static const ui_menu_item_t *file_menu_cart_section = NULL;

/** \brief  'File' menu - printer section pointer
 *
 * Set by ui_machine_menu_bar_create().
 */
static const ui_menu_item_t *file_menu_printer_section = NULL;

/** \brief  'Settings' menu - joystick section pointer
 *
 * Set by ui_machine_menu_bar_create().
 */
static const ui_menu_item_t *settings_menu_joy_section = NULL;


/** \brief  Create the top menu bar with standard submenus
 *
 * \param[in]   window_id   window ID (PRIMARY_WINDOW or SECONDARY_WINDOW)
 *
 * \return  GtkMenuBar
 */
GtkWidget *ui_machine_menu_bar_create(gint window_id)
{
    GtkWidget *menu_bar;
    GtkWidget *file_submenu;
    GtkWidget *snapshot_submenu;
    GtkWidget *edit_submenu;
    GtkWidget *settings_submenu;
#ifdef DEBUG
    GtkWidget *debug_submenu;
#endif
    GtkWidget *help_submenu;


    /* create the top menu bar */
    menu_bar = gtk_menu_bar_new();

    /* create the top-level 'File' menu */
    file_submenu = ui_menu_submenu_create(menu_bar, "File");

    /* create the top-level 'Edit' menu */
    edit_submenu = ui_menu_submenu_create(menu_bar, "Edit");

    /* create the top-level 'Snapshot' menu */
    snapshot_submenu = ui_menu_submenu_create(menu_bar, "Snapshot");

    /* create the top-level 'Settings' menu */
    settings_submenu = ui_menu_submenu_create(menu_bar, "Preferences");

#ifdef DEBUG
    /* create the top-level 'Debug' menu (when --enable-debug is used) */
    debug_submenu = ui_menu_submenu_create(menu_bar, "Debug");
#endif

    /* create the top-level 'Help' menu */
    help_submenu = ui_menu_submenu_create(menu_bar, "Help");

    /* determine which joystick swap, tape and cart menu items should be added */
    switch (machine_class) {

        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:
            file_menu_tape_section    = file_menu_tape;
            file_menu_cart_section    = file_menu_cart;
            file_menu_printer_section = file_menu_printer;
            settings_menu_joy_section = settings_menu_joy_swap;
            break;

        case VICE_MACHINE_C64DTV:
            file_menu_printer_section = file_menu_printer_no_userport;
            settings_menu_joy_section = settings_menu_joy_swap;
            break;

        case VICE_MACHINE_SCPU64:
            file_menu_cart_section    = file_menu_cart;
            file_menu_printer_section = file_menu_printer;
            settings_menu_joy_section = settings_menu_joy_swap;
            break;

        case VICE_MACHINE_C128:
            file_menu_tape_section    = file_menu_tape;
            file_menu_cart_section    = file_menu_cart;
            file_menu_printer_section = file_menu_printer;
            settings_menu_joy_section = settings_menu_joy_swap;
            break;

        case VICE_MACHINE_VIC20:
            file_menu_tape_section    = file_menu_tape;
            file_menu_cart_section    = file_menu_cart;
            file_menu_printer_section = file_menu_printer;
            break;

        case VICE_MACHINE_PLUS4:
            file_menu_tape_section    = file_menu_tape;
            file_menu_cart_section    = file_menu_cart;
            file_menu_printer_section = file_menu_printer_no_userport;
            settings_menu_joy_section = settings_menu_joy_swap;
            break;

        case VICE_MACHINE_CBM5x0:
            file_menu_tape_section    = file_menu_tape;
            file_menu_cart_section    = file_menu_cart;
            file_menu_printer_section = file_menu_printer_no_userport;
            settings_menu_joy_section = settings_menu_joy_swap;
            break;

        case VICE_MACHINE_CBM6x0:
            file_menu_tape_section    = file_menu_tape;
            file_menu_cart_section    = file_menu_cart;
            file_menu_printer_section = file_menu_printer;
            break;

        case VICE_MACHINE_PET:
            file_menu_tape_section    = file_menu_tape_xpet;
            file_menu_printer_section = file_menu_printer;
            break;

        case VICE_MACHINE_VSID:
            archdep_vice_exit(1);
            break;
        default:
            break;
    }


    /* add items to the File menu */
    ui_menu_add(file_submenu, file_menu_head, window_id);
    if (file_menu_tape_section != NULL) {
        ui_menu_add(file_submenu, file_menu_tape_section, window_id);
    }
    if (file_menu_cart_section != NULL) {
        ui_menu_add(file_submenu, file_menu_cart_section, window_id);
    }
    if (file_menu_printer_section != NULL) {
        ui_menu_add(file_submenu, file_menu_printer_section, window_id);
    }
    ui_menu_add(file_submenu, file_menu_tail, window_id);

    /* add items to the Edit menu */
    ui_menu_add(edit_submenu, edit_menu, window_id);
    /* add items to the Snapshot menu */
    ui_menu_add(snapshot_submenu, snapshot_menu, window_id);

    /* add items to the Settings menu */
    ui_menu_add(settings_submenu, settings_menu_head, window_id);
    if (settings_menu_joy_section != NULL) {
        ui_menu_add(settings_submenu, settings_menu_joy_section, window_id);
    }
    if (machine_class != VICE_MACHINE_VSID) {
        ui_menu_add(settings_submenu, settings_menu_non_vsid, window_id);
    }
    ui_menu_add(settings_submenu, settings_menu_tail, window_id);

#ifdef DEBUG
    /* add items to the Debug menu */
    if (machine_class == VICE_MACHINE_C64DTV) {
        ui_menu_add(debug_submenu, debug_menu_c64dtv, window_id);
    } else {
        ui_menu_add(debug_submenu, debug_menu, window_id);
    }
#endif

    /* add items to the Help menu */
    ui_menu_add(help_submenu, help_menu, window_id);

    main_menu_bar = menu_bar;    /* XXX: do I need g_object_ref()/g_object_unref()
                                         for this */

    return menu_bar;
}


/** \brief  Add missing settings load/save items
 *
 * \param[in,out]   menu        GtkMenu
 * \param[in]       window_id   window ID (currently only 0/PRIMARY_WINDOW)
 */
void ui_machine_menu_bar_vsid_patch(GtkWidget *menu, gint window_id)
{
    ui_menu_add(menu, settings_menu_tail, window_id);
}
