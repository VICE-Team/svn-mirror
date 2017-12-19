/*
 * uivsidmenu.c - Native GTK3 menus for the SID player, vsid.
 *
 * Written by
 *  Marcus Sutton <loggedoubt@gmail.com>
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

#include <stdlib.h>
#include <gtk/gtk.h>

#include "debug.h"
#include "machine.h"
#include "ui.h"
#include "uiabout.h"
#include "uicmdline.h"
#include "uicommands.h"
#include "uicompiletimefeatures.h"
#include "uidebug.h"
#include "uimedia.h"
#include "uimenu.h"
#include "uimonarch.h"
#include "uisettings.h"
#include "uismartattach.h"
#include "uivsidmenu.h"


/** \brief  File->Reset submenu
 */
static ui_menu_item_t reset_submenu[] = {
    { "Soft reset", UI_MENU_TYPE_ITEM_ACTION,
        "reset-soft", ui_machine_reset_callback, GINT_TO_POINTER(MACHINE_RESET_MODE_SOFT),
        GDK_KEY_F9, VICE_MOD_MASK },
    { "Hard reset", UI_MENU_TYPE_ITEM_ACTION,
        "reset-hard", ui_machine_reset_callback, GINT_TO_POINTER(MACHINE_RESET_MODE_HARD),
        GDK_KEY_F12, VICE_MOD_MASK },

    UI_MENU_TERMINATOR
};


/** \brief  'File' menu
 */
static ui_menu_item_t file_menu[] = {
    { "Load PSID file ...", UI_MENU_TYPE_ITEM_ACTION,
        "load-psid", NULL, NULL,
        GDK_KEY_L, VICE_MOD_MASK },

    UI_MENU_SEPARATOR,

    /* XXX: this item might need its own dialog that only
     *      contains sound recording options
     */
    { "Record sound file ...", UI_MENU_TYPE_ITEM_ACTION,
        "sound-save", uimedia_dialog_show, NULL,
        GDK_KEY_R, VICE_MOD_MASK|GDK_SHIFT_MASK },

    { "Stop sound recording", UI_MENU_TYPE_ITEM_ACTION,
        "sound-stop", (void *)uimedia_stop_recording, NULL,
        GDK_KEY_S, VICE_MOD_MASK|GDK_SHIFT_MASK },

    UI_MENU_SEPARATOR,

    /* monitor */
    { "Activate monitor", UI_MENU_TYPE_ITEM_ACTION,
        "monitor", ui_monitor_activate_callback, NULL,
        GDK_KEY_H, VICE_MOD_MASK | GDK_SHIFT_MASK },
    { "Monitor settings ...", UI_MENU_TYPE_ITEM_ACTION,
        NULL, NULL, NULL,
        0, 0 },

    UI_MENU_SEPARATOR,

    { "Reset", UI_MENU_TYPE_SUBMENU,
        NULL, NULL, reset_submenu,
        0, 0 },
    { "Action on CPU JAM ...", UI_MENU_TYPE_ITEM_ACTION,
        NULL, NULL, NULL,
        0, 0 },

    UI_MENU_SEPARATOR,

    { "Exit emulator", UI_MENU_TYPE_ITEM_ACTION,
        "exit", ui_close_callback, NULL,
        GDK_KEY_Q, VICE_MOD_MASK },

    UI_MENU_TERMINATOR
};


/** \brief  'Tunes' menu
 */
#if 0
static ui_menu_item_t tune_menu[] = {

    UI_MENU_TERMINATOR
};
#endif


/** \brief  'Settings' menu
 */
static ui_menu_item_t settings_menu[] = {
    /* XXX: this item should perhaps be removed and its functionality
     *      added to the settings dialog
     */
    { "Override PSID settings", UI_MENU_TYPE_ITEM_CHECK,
        "psid-keep-env", (void *)(ui_toggle_resource), (void *)"PSIDKeepEnv",
        0, 0 },

    UI_MENU_SEPARATOR,

    /* the settings dialog */
    { "Settings", UI_MENU_TYPE_ITEM_ACTION,
        "settings", ui_settings_dialog_create, NULL,
        0, 0 },

    UI_MENU_TERMINATOR
};


/** \brief  'Debug' menu items
 */
#ifdef DEBUG
static ui_menu_item_t debug_menu[] = {
    { "Trace mode ...", UI_MENU_TYPE_ITEM_ACTION,
        "tracemode", uidebug_trace_mode_callback, NULL,
        0, 0 },

    UI_MENU_SEPARATOR,

    { "Main CPU trace", UI_MENU_TYPE_ITEM_CHECK,
        "trace-maincpu", (void *)(ui_toggle_resource), (void *)"MainCPU_TRACE",
        0, 0 },

    UI_MENU_SEPARATOR,


    { "Autoplay playback frames ...", UI_MENU_TYPE_ITEM_ACTION,
        "playframes", uidebug_playback_frames_callback, NULL,
        0, 0 },
    { "Save core dump", UI_MENU_TYPE_ITEM_CHECK,
        "coredump", (void *)(ui_toggle_resource), (void *)"DoCoreDump",
        0, 0 },

    UI_MENU_TERMINATOR
};
#endif


/** \brief  'Help' menu items
 */
static ui_menu_item_t help_menu[] = {
    { "Browse manual", UI_MENU_TYPE_ITEM_ACTION,
        "manual", NULL, NULL,
        0, 0 },
    { "Command line options ...", UI_MENU_TYPE_ITEM_ACTION,
        "cmdline", uicmdline_dialog_show, NULL,
        0, 0 },
    { "Compile time features ...", UI_MENU_TYPE_ITEM_ACTION,
        "features", uicompiletimefeatures_dialog_show, NULL,
        0, 0 },
    { "About VICE", UI_MENU_TYPE_ITEM_ACTION,
        "about", ui_about_dialog_callback, NULL,
        0, 0 },

    UI_MENU_TERMINATOR
};


/** \brief  Create the top menu bar with standard submenus
 *
 * \return  GtkMenuBar
 */
GtkWidget *ui_vsid_menu_bar_create(void)
{
    GtkWidget *menu_bar;

    menu_bar = ui_menu_bar_create();

    /* generate File menu */
    ui_menu_file_add(file_menu);

    /* generate Settings menu */
    ui_menu_settings_add(settings_menu);

#ifdef DEBUG
    /* generate Debug menu */
    ui_menu_debug_add(debug_menu);
#endif

    /* generate Help menu */
    ui_menu_help_add(help_menu);

    return menu_bar;
}
