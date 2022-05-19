/** \file   uimachinemenu.c
 * \brief   Native GTK3 menus for machine emulators (not vsid)
 *
 * \author  Marcus Sutton <loggedoubt@gmail.com>
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES KeySetEnable    -vsid
 * $VICERES Mouse           -vsid
 * $VICERES DtvBlitterLog   x64dtv
 * $VICERES DtvDMALog       x64dtv
 * $VICERES DtvFlashLog     x64dtv
 *
 * These resources are only available with --enable-debug
 *
 * $VICERES MainCPU_TRACE   all
 * $VICERES IEC_TRACE       -vsid
 * $VICERES IEEE_TRACE      -vsid
 * $VICERES Drive0CPU_TRACE -vsid
 * $VICERES Drive1CPU_TRACE -vsid
 * $VICERES Drive2CPU_TRACE -vsid
 * $VICERES Drive3CPU_TRACE -vsid
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

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "vice_gtk3.h"

#include "archdep.h"
#include "archdep_defs.h"
#include "datasette.h"
#include "debug.h"
#include "debug_gtk3.h"
#include "lib.h"
#include "machine.h"
#include "mainlock.h"
#include "resources.h"
#include "tapeport.h"
#include "ui.h"
#include "uiabout.h"
#include "uiactions.h"
#include "uicart.h"
#include "uicmdline.h"
#include "uicommands.h"
#include "uicompiletimefeatures.h"
#include "uidatasette.h"
#include "uidebug.h"
#include "uidiskattach.h"
#include "uidiskcreate.h"
#include "uiedit.h"
#include "uifliplist.h"
#include "uimedia.h"
#include "uimenu.h"
#include "uimonarch.h"
#include "uisettings.h"
#include "uismartattach.h"
#include "uisnapshot.h"
#include "uitapeattach.h"
#include "uitapecreate.h"
#include "util.h"
#include "vsync.h"

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



/** \brief  Generate full path and name of the current vice config file
 *
 * \return  heap-allocated path to the current config file, free with lib_free()
 */
static char *get_config_file_path(void)
{
    char *path;

    if (vice_config_file != NULL) {
        /* -config used */

        /* check for relative path */
        if (archdep_path_is_relative(vice_config_file)) {
            char *cwd = g_get_current_dir();

            path = util_join_paths(cwd, vice_config_file, NULL);
            g_free(cwd);
        } else {
            path = lib_strdup(vice_config_file);
        }
    } else {
        /* default vicerc or vice.ini */
        path = util_join_paths(archdep_user_config_path(),
                               ARCHDEP_VICERC_NAME,
                               NULL);
    }
    return path;
}




/** \brief  Load settings from default file
 *
 * \param[in]   widget  menu item triggering the event (ignored)
 * \param[in]   data    extra even data (ignored)
 *
 * \return  TRUE
 */
static gboolean settings_load_callback(GtkWidget *widget, gpointer data)
{
    if (resources_reset_and_load(NULL) != 0) {
        vice_gtk3_message_error("VICE core error",
                "Failed to load default settings file");
    }
    return TRUE;
}


/** \brief  Calllback for the load-settings dialog
 *
 * \param[in,out]   dialog      dialog
 * \param[in,out]   filename    filename
 * \param[in]       data        mode (0: reset and load, 1: add extra settings)
 */
static void settings_load_filename_callback(GtkDialog *dialog,
                                            gchar *filename,
                                            gpointer data)
{
    int res;
    if (filename!= NULL) {
        mainlock_obtain();
        res = (data == NULL) ? resources_reset_and_load(filename)
                             : resources_load(filename);
        if (res != 0) {
            vice_gtk3_message_error("VICE core error",
                    "Failed to load settings from '%s'", filename);
        }
        mainlock_release();
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}


/** \brief  Load settings from user-specified file
 *
 * \param[in]   widget  menu item triggering the event (ignored)
 * \param[in]   data    mode (0: reset and load, 1: add extra settings)
 *
 * \return  TRUE
 */
static gboolean settings_load_custom_callback(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    char *path;

    path = get_config_file_path();

    dialog = vice_gtk3_open_file_dialog(
            "Load settings file",
            NULL, NULL, NULL,
            settings_load_filename_callback,
            data);

    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), path);
    lib_free(path);

    return TRUE;
}


/** \brief  Save settings to default file
 *
 * \param[in]   widget  menu item triggering the event (ignored)
 * \param[in]   data    extra even datat (ignored)
 *
 * \return  TRUE to signal the event has been handled
 */
static gboolean settings_save_callback(GtkWidget *widget, gpointer data)
{
    if (resources_save(NULL) != 0) {
        vice_gtk3_message_error("VICE core error",
                "Failed to save default settings file");
    }
    return TRUE;
}


/** \brief  Callback for the save-custom-settings dialog
 *
 * \param[in,out]   dialog      save-file dialog
 * \param[in,out]   filename    filename
 * \param[in]       data        extra data (unused)
 */
static void on_settings_save_custom_filename(GtkDialog *dialog,
                                             gchar *filename,
                                             gpointer data)
{
    if (filename!= NULL) {
        mainlock_obtain();
        if (resources_save(filename) != 0) {
            vice_gtk3_message_error("VICE core error",
                    "Failed to save settings as '%s'", filename);
        }
        mainlock_release();
        g_free(filename);
    }
    mainlock_release();
    gtk_widget_destroy(GTK_WIDGET(dialog));
    mainlock_obtain();

}


/** \brief  Save settings to user-specified file
 *
 * \param[in]   widget  menu item triggering the event (ignored)
 * \param[in]   data    extra even data (ignored)
 *
 * \return  TRUE
 */
static gboolean settings_save_custom_callback(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    char *path;

    path = get_config_file_path();

    dialog = vice_gtk3_save_file_dialog(
            "Save settings as ...",
            NULL, TRUE, NULL,
            on_settings_save_custom_filename,
            NULL);
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), path);
    lib_free(path);

    return TRUE;
}


/** \brief  Encode unit and drive number for a detach callback argument
 *
 * Encodes unit and drive number into a value to be used in the
 * ui_disk_detach_callback() calls.
 *
 * \param[in]   U   unit number (8-11)
 * \param[in]   D   drive number (0 or 1)
 */
#define UNIT_DRIVE_TO_PTR(U, D) GINT_TO_POINTER(((U) << 8) | ((D) & 0xff))


/* {{{ disk_detach_submenu[] */
/** \brief  File->Detach disk submenu
 */
static ui_menu_item_t disk_detach_submenu[] = {
    { "Drive 8:0", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_8_0,
        ui_disk_detach_callback, NULL, UNIT_DRIVE_TO_PTR(8, 0),
        false },
    { "Drive 8:1", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_8_1,
        ui_disk_detach_callback, NULL, UNIT_DRIVE_TO_PTR(8, 1),
        false },
    { "Drive 9:0", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_9_0,
        ui_disk_detach_callback, NULL, UNIT_DRIVE_TO_PTR(9, 0),
        false },
    { "Drive 9:1", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_9_1,
        ui_disk_detach_callback, NULL, UNIT_DRIVE_TO_PTR(9, 1),
        false },
    { "Drive 10:0", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_10_0,
        ui_disk_detach_callback, NULL, UNIT_DRIVE_TO_PTR(10, 0),
        false },
    { "Drive 10:1", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_10_1,
        ui_disk_detach_callback, NULL, UNIT_DRIVE_TO_PTR(10, 1),
        false },
    { "Drive 11:0", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_11_0,
        ui_disk_detach_callback, NULL, UNIT_DRIVE_TO_PTR(11, 0),
        false },
    { "Drive 11:1", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_11_1,
        ui_disk_detach_callback, NULL, UNIT_DRIVE_TO_PTR(11, 1),
        false },
    { "Detach all", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_ALL,
        ui_disk_detach_all_callback, NULL, NULL,
        false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ disk_attach_submenu[] */
/** \brief  File->Attach disk submenu
 */
static ui_menu_item_t disk_attach_submenu[] = {
    { "Drive #8", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_ATTACH_8_0,
      ui_disk_attach_dialog_show, NULL, GINT_TO_POINTER(8),
      true },
    { "Drive #9", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_ATTACH_9_0,
      ui_disk_attach_dialog_show, NULL, GINT_TO_POINTER(9),
      true },
    { "Drive #10", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_ATTACH_10_0,
      ui_disk_attach_dialog_show, NULL, GINT_TO_POINTER(10),
      true },
    { "Drive #11", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_ATTACH_11_0,
      ui_disk_attach_dialog_show, NULL, GINT_TO_POINTER(11),
      true },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ disk_fliplist_submenu[] */
/** \brief  File->Flip list submenu
 */
static ui_menu_item_t disk_fliplist_submenu[] = {
    { "Add current image (Unit #8)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_ADD,
      ui_fliplist_add_current_cb, NULL, GINT_TO_POINTER(8),
      false },
    { "Remove current image (Unit #8)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_REMOVE,
      ui_fliplist_remove_current_cb, NULL, GINT_TO_POINTER(8),
      false },
    { "Attach next image (Unit #8)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_NEXT,
      ui_fliplist_next_cb, NULL, GINT_TO_POINTER(8),
      false },
    { "Attach previous image (Unit #8)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_PREVIOUS,
      ui_fliplist_prev_cb, NULL, GINT_TO_POINTER(8),
      false },
    { "Clear fliplist (Unit #8)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_CLEAR,
      ui_fliplist_clear_cb, NULL, GINT_TO_POINTER(8),
      false },
    { "Load flip list file...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_LOAD,
      ui_fliplist_load_callback, NULL, GINT_TO_POINTER(8),
      true },
    { "Save flip list file...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_SAVE,
      ui_fliplist_save_callback, NULL, GINT_TO_POINTER(8),
      true },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ datasette_1_control_submenu[] */
/** \brief  File->Datasette control submenu for port #1
 */
static ui_menu_item_t datasette_1_control_submenu[] = {
    { "Stop", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_STOP_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_STOP | (TAPEPORT_UNIT_1 << 8)),
      false },
    { "Start", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_PLAY_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_START | (TAPEPORT_UNIT_1 << 8)),
      false },
    { "Forward", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_FFWD_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_FORWARD | (TAPEPORT_UNIT_1 << 8)),
      false },
    { "Rewind", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_REWIND_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_REWIND | (TAPEPORT_UNIT_1 << 8)),
      false },
    { "Record", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RECORD_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_RECORD | (TAPEPORT_UNIT_1 << 8)),
      false },
    { "Reset", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RESET_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_RESET | (TAPEPORT_UNIT_1 << 8)),
      false },
    { "Reset Counter", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RESET_COUNTER_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_RESET_COUNTER | (TAPEPORT_UNIT_1 << 8)),
      false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ datasette_2_control_submenu[] */
/** \brief  File->Datasette control submenu for port #2
 */
static ui_menu_item_t datasette_2_control_submenu[] = {
    { "Stop", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_STOP_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_STOP | (TAPEPORT_UNIT_2 << 8)),
      false },
    { "Start", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_PLAY_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_START | (TAPEPORT_UNIT_2 << 8)),
      false },
    { "Forward", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_FFWD_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_FORWARD | (TAPEPORT_UNIT_2 << 8)),
      false },
    { "Rewind", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_REWIND_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_REWIND | (TAPEPORT_UNIT_2 << 8)),
      false },
    { "Record", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RECORD_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_RECORD | (TAPEPORT_UNIT_2 << 8)),
      false },
    { "Reset", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RESET_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_RESET | (TAPEPORT_UNIT_2 << 8)),
      false },
    { "Reset Counter", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RESET_COUNTER_1,
      ui_datasette_tape_action_cb, NULL,
      GINT_TO_POINTER(DATASETTE_CONTROL_RESET_COUNTER | (TAPEPORT_UNIT_2 << 8)),
      false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ reset_submenu[] */
/** \brief  File->Reset submenu
 */
static ui_menu_item_t reset_submenu[] = {
    { "Soft reset", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_SOFT,
      ui_machine_reset_callback, NULL, GINT_TO_POINTER(MACHINE_RESET_MODE_SOFT),
      false },
    { "Hard reset", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_HARD,
      ui_machine_reset_callback, NULL, GINT_TO_POINTER(MACHINE_RESET_MODE_HARD),
      false },

    UI_MENU_SEPARATOR,

    { "Reset drive #8", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_DRIVE_8,
      ui_drive_reset_callback, NULL, GINT_TO_POINTER(8),
      false },
    { "Reset drive #9", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_DRIVE_9,
      ui_drive_reset_callback, NULL, GINT_TO_POINTER(9),
      false },
    { "Reset drive #10", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_DRIVE_10,
      ui_drive_reset_callback, NULL, GINT_TO_POINTER(10),
      false },
    { "Reset drive #11", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_DRIVE_11,
      ui_drive_reset_callback, NULL, GINT_TO_POINTER(11),
      false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ file_menu_head[] */
/** \brief  'File' menu - head section
 */
static ui_menu_item_t file_menu_head[] = {
    { "Smart attach ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SMART_ATTACH,
      ui_smart_attach_dialog_show, NULL, NULL,
      true },

    UI_MENU_SEPARATOR,

    { "Attach disk image", UI_MENU_TYPE_SUBMENU,
      0,
      NULL, NULL, disk_attach_submenu,
      false },
    { "Create and attach an empty disk image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_CREATE,
      ui_disk_create_dialog_show, NULL, GINT_TO_POINTER(8),
      true },
    { "Detach disk image", UI_MENU_TYPE_SUBMENU,
      0,
      NULL, NULL, disk_detach_submenu,
      false },
    { "Flip list", UI_MENU_TYPE_SUBMENU,
      0,
      NULL, NULL, disk_fliplist_submenu,
      false },

    UI_MENU_SEPARATOR,

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ file_menu_tape[] */
/** \brief  'File' menu - tape section
 */
static ui_menu_item_t file_menu_tape[] = {
    { "Attach datasette image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_ATTACH_1,
      ui_tape_attach_callback, NULL, (void*)1,
      true },
    { "Create and attach datasette image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_CREATE_1,
      ui_tape_create_dialog_show, NULL, (void*)1,
      true },
    { "Detach datasette image", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_DETACH_1,
      ui_tape_detach_callback, NULL, (void*)1,
      false },
    { "Datasette controls", UI_MENU_TYPE_SUBMENU,
      0,
      NULL, NULL, datasette_1_control_submenu,
      false },

    UI_MENU_SEPARATOR,  /* Required since this menu gets inserted between
                           disk menu items and cartridge items on emulators
                           that have a datasette port. */
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ file_menu_tape_xpet[] */
/** \brief  'File' menu - tape section for xpet
 */
static ui_menu_item_t file_menu_tape_xpet[] = {
    { "Attach datasette #1 image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_ATTACH_1,
      ui_tape_attach_callback, NULL, (void*)TAPEPORT_UNIT_1,
      true },
    { "Create and attach datasette #1 image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_CREATE_1,
      ui_tape_create_dialog_show, NULL, (void*)TAPEPORT_UNIT_1,
      true },
    { "Detach datasette #1 image", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_DETACH_1,
      ui_tape_detach_callback, NULL, (void*)TAPEPORT_UNIT_1,
      false },
    /* TODO: fix the submenus: */
    { "Datasette #1 controls", UI_MENU_TYPE_SUBMENU,
      0,
      NULL, NULL, datasette_1_control_submenu,
      false },

    UI_MENU_SEPARATOR,

    { "Attach datasette #2 image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_ATTACH_2,
      ui_tape_attach_callback, NULL, (void*)TAPEPORT_UNIT_2,
      true },
    { "Create and attach datasette #2 image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_CREATE_2,
      ui_tape_create_dialog_show, NULL, (void*)TAPEPORT_UNIT_2,
      true },
    { "Detach datasette #2 image", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_DETACH_2,
      ui_tape_detach_callback, NULL, (void*)TAPEPORT_UNIT_2,
      false },
    { "Datasette #2 controls", UI_MENU_TYPE_SUBMENU,
      0,
      NULL, NULL, datasette_2_control_submenu,
      false },

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
static ui_menu_item_t file_menu_cart[] = {
    /* cart */
    { "Attach cartridge image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_CART_ATTACH,
      ui_cart_show_dialog, NULL, GINT_TO_POINTER(0),
      true },
    { "Detach cartridge image(s)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_CART_DETACH,
      (void *)ui_cart_detach, NULL, NULL,
      false },
    { "Cartridge freeze", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_CART_FREEZE,
      (void *)ui_cart_trigger_freeze, NULL, NULL,
      false },

    UI_MENU_SEPARATOR,

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ file_menu_tail */
/** \brief  'File' menu - tail section
 */
static ui_menu_item_t file_menu_tail[] = {
    /* monitor */
    { "Activate monitor", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_MONITOR_OPEN,
      ui_monitor_activate_callback, NULL, NULL,
      false },

    UI_MENU_SEPARATOR,

    { "Reset", UI_MENU_TYPE_SUBMENU,
      0,
      NULL, NULL, reset_submenu,
      false },

    UI_MENU_SEPARATOR,

    { "Exit emulator", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_QUIT,
      ui_close_callback, NULL, NULL,
      true },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ edit_menu[] */
/** \brief  'Edit' menu
 */
static ui_menu_item_t edit_menu[] = {
    { "Copy", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_EDIT_COPY,
      (void*)ui_copy_callback, NULL, NULL,
      false },
    { "Paste", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_EDIT_PASTE,
      (void*)ui_paste_callback, NULL, NULL,
      false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ snapshot_menu[] */
/** \brief  'Snapshot' menu
 */
static ui_menu_item_t snapshot_menu[] = {
    { "Load snapshot image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SNAPSHOT_LOAD,
      ui_snapshot_open_file, NULL, NULL,
      false },
    { "Save snapshot image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SNAPSHOT_SAVE,
      ui_snapshot_save_file, NULL, NULL,
      false },

    UI_MENU_SEPARATOR,

    { "Quickload snapshot", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SNAPSHOT_QUICKLOAD,
      ui_snapshot_quickload_snapshot, NULL, NULL,
      false },
    { "Quicksave snapshot", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SNAPSHOT_QUICKSAVE,
      ui_snapshot_quicksave_snapshot, NULL, NULL,
      false },

    UI_MENU_SEPARATOR,

    { "Start recording events", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_RECORD_START,
      ui_snapshot_history_record_start, NULL, NULL,
      false },
    { "Stop recording events", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_RECORD_STOP,
      ui_snapshot_history_record_stop, NULL, NULL,
      false },
    { "Start playing back events", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_PLAYBACK_START,
      ui_snapshot_history_playback_start, NULL, NULL,
      false },
    { "Stop playing back events", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_PLAYBACK_STOP,
      ui_snapshot_history_playback_stop, NULL, NULL,
      false },
    { "Set recording milestone", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_MILESTONE_SET,
      ui_snapshot_history_milestone_set, NULL, NULL,
      false },
    { "Return to milestone", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_MILESTONE_RESET,
      ui_snapshot_history_milestone_reset, NULL, NULL,
      false },

    UI_MENU_SEPARATOR,

    { "Save/Record media ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_MEDIA_RECORD,
      ui_media_dialog_show, NULL, NULL,
      false },
    { "Stop media recording", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_MEDIA_STOP,
      (void *)ui_media_stop_recording, NULL, NULL,
      false },

    UI_MENU_SEPARATOR,

    { "Quicksave screenshot", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SCREENSHOT_QUICKSAVE,
      (void *)ui_media_auto_screenshot, NULL, NULL,
      false },

    UI_MENU_TERMINATOR
};
/* }}} */

/** \brief  Index in the speed submenu for the "$MACHINE_NAME FPS" item
 *
 * Bit of a hack since the menu item labels are reused for all emus and we can't
 * dynamically set them via some printf()-ish construct
 */
#define MACHINE_FPS_INDEX   7

static ui_menu_item_t speed_submenu[] = {
    { "200% CPU", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_CPU_200,
      ui_cpu_speed_callback, "Speed", (void*)200,
      false },
    { "100% CPU", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_CPU_100,
      ui_cpu_speed_callback, "Speed", (void*)100,
      false },
    { "50% CPU", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_CPU_50,
      ui_cpu_speed_callback, "Speed", (void*)50,
      false },
    { "20% CPU", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_CPU_20,
      ui_cpu_speed_callback, "Speed", (void*)20,
      false },
    { "10% CPU", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_CPU_10,
      ui_cpu_speed_callback, "Speed", (void*)10,
      false },
    { "Custom CPU speed ...", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_CPU_CUSTOM,
      ui_speed_custom_toggled, "Speed", NULL,
      false },

    UI_MENU_SEPARATOR,

    { "MACHINE_NAME FPS", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_FPS_REAL,
      ui_cpu_speed_callback, "Speed", (void*)100,
      false },
    { "50 FPS", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_FPS_50,
      ui_fps_callback, "Speed", (void*)50,
      false },
    { "60 FPS", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_FPS_60,
      ui_fps_callback, "Speed", (void*)60,
      false },
    { "Custom FPS", UI_MENU_TYPE_ITEM_RADIO_INT,
      ACTION_SPEED_FPS_CUSTOM,
      ui_fps_custom_toggled, "Speed", NULL,
      false },

    UI_MENU_TERMINATOR
};


/* {{{ settings_menu_head[] */
/** \brief  'Settings' menu - head section
 */
static ui_menu_item_t settings_menu_head[] = {
    { "Fullscreen", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_FULLSCREEN_TOGGLE,
      (void *)ui_action_toggle_fullscreen, NULL, NULL,
      true },
    { "Restore display state", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESTORE_DISPLAY,
      (void *)ui_restore_display, NULL, NULL,
      true },
    { "Show menu/status in fullscreen", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_FULLSCREEN_DECORATIONS_TOGGLE,
      (void *)ui_action_toggle_fullscreen_decorations, "FullscreenDecorations", NULL,
      true },
    { "Show status bar", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_SHOW_STATUSBAR_TOGGLE,
      (void* )ui_action_toggle_show_statusbar, NULL, NULL,
      true },

    UI_MENU_SEPARATOR,

    { "Warp mode", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_WARP_MODE_TOGGLE,
      (void *)ui_action_toggle_warp, NULL, NULL,
      false },
    { "Pause emulation", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_PAUSE_TOGGLE,
      (void *)ui_action_toggle_pause, NULL, NULL,
      false },
    { "Advance frame", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_ADVANCE_FRAME,
      (void *)ui_action_advance_frame, NULL, NULL,
      false },

    UI_MENU_SEPARATOR,

    { "Emulation speed", UI_MENU_TYPE_SUBMENU,
      0,
      NULL, NULL, speed_submenu,
      false },

    UI_MENU_SEPARATOR,

    { "Mouse grab", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_MOUSE_GRAB_TOGGLE,
      (void *)ui_action_toggle_mouse_grab, "Mouse", NULL,
      false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ settings_menu_joy_swap[] */

/** \brief  Settings menu - joystick controlport swap
 *
 * Valid for x64/x64sc/x64dtv/xscpu64/x128/xplus4/xcbm5x0
 */
static ui_menu_item_t settings_menu_joy_swap[] = {
    { "Swap joysticks", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_SWAP_CONTROLPORT_TOGGLE,
      (void *)(ui_action_toggle_controlport_swap), NULL, NULL,
      false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ settings_menu_non_vsid[] */
/** \brief  'Settings' menu section before the tail section
 *
 * Only valid for non-VSID
 */
static ui_menu_item_t settings_menu_non_vsid[] = {
    /* Needs to go here to avoid duplicate action names */
    { "Allow keyset joysticks", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_KEYSET_JOYSTICK_TOGGLE,
      (void *)(ui_action_toggle_keyset_joystick), "KeySetEnable", NULL,
      false },

    UI_MENU_SEPARATOR,

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ settings_menu_tail[] */
/** \brief  'Settings' menu tail section
 */
static ui_menu_item_t settings_menu_tail[] = {
   /* the settings dialog */
    { "Settings ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_DIALOG,
      ui_settings_dialog_create_and_activate_node_callback, NULL, NULL,
      true },
    { "Load settings", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_LOAD,
      settings_load_callback, NULL, NULL,
      false },
    { "Load settings from ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_LOAD_FROM,
      settings_load_custom_callback, NULL, (void*)false,
      true },
    { "Load extra settings from ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_LOAD_EXTRA,
      settings_load_custom_callback, NULL, (void*)true,
      true },
    { "Save settings", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_SAVE,
      settings_save_callback, NULL, NULL,
      false },
    { "Save settings to ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_SAVE_TO,
      settings_save_custom_callback, NULL, NULL,
      true },
    { "Restore default settings", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_DEFAULT,
      (void *)ui_restore_default_settings, NULL, NULL,
      true },

    UI_MENU_TERMINATOR
};
/* }}} */

#ifdef DEBUG

/* {{{ debug_menu[] */
/** \brief  'Debug' menu items for emu's except x64dtv
 *
 * TODO: Make the check items use the proper API.
 */
static ui_menu_item_t debug_menu[] = {
    { "Trace mode ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DEBUG_TRACE_MODE,
      ui_debug_trace_mode_dialog_show, NULL, NULL,
      true },

    UI_MENU_SEPARATOR,

    { "Main CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_CPU_TOGGLE,
      ui_toggle_resource, (void*)"MainCPU_TRACE", NULL,
      false },

    UI_MENU_SEPARATOR,

    { "IEC bus trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_IEC_TOGGLE,
      ui_toggle_resource, (void*)"IEC_TRACE", NULL,
      false },
    { "IEEE-488 bus trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_IEEE488_TOGGLE,
      ui_toggle_resource, (void*)"IEEE_TRACE", NULL,
      false },

    UI_MENU_SEPARATOR,

    { "Drive #8 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE,
      ui_toggle_resource, (void*)"Drive0CPU_TRACE", NULL,
      false },
    { "Drive #9 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE,
      ui_toggle_resource, (void*)"Drive1CPU_TRACE", NULL,
      false },
    { "Drive #10 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE,
      ui_toggle_resource, (void*)"Drive2CPU_TRACE", NULL,
      false },
    { "Drive #11 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE,
      ui_toggle_resource, (void*)"Drive3CPU_TRACE", NULL,
      false },

    UI_MENU_SEPARATOR,

    { "Autoplay playback frames...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DEBUG_AUTOPLAYBACK_FRAMES,
      ui_debug_playback_frames_dialog_show, NULL, NULL,
      true },
    { "Save core dump", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_CORE_DUMP_TOGGLE,
      ui_toggle_resource, (void*)"DoCoreDump", NULL,
      false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ debug_menu_c64dtv[] */
/** \brief  'Debug' menu items for x64dtv
 */
static ui_menu_item_t debug_menu_c64dtv[] = {
    { "Trace mode ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DEBUG_TRACE_MODE,
      ui_debug_trace_mode_dialog_show, NULL, NULL,
      true },

    UI_MENU_SEPARATOR,

    { "Main CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_CPU_TOGGLE,
      ui_toggle_resource, (void*)"MainCPU_TRACE", NULL,
      false },

    UI_MENU_SEPARATOR,

    { "IEC bus trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_IEC_TOGGLE,
      ui_toggle_resource, (void*)"IEC_TRACE", NULL,
      false },
    { "Drive #8 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE,
      ui_toggle_resource, (void*)"Drive0CPU_TRACE", NULL,
      false },
    { "Drive #9 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE,
      ui_toggle_resource, (void*)"Drive1CPU_TRACE", NULL,
      false },
    { "Drive #10 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE,
      ui_toggle_resource, (void*)"Drive2CPU_TRACE", NULL,
      false },
    { "Drive #11 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE,
      ui_toggle_resource, (void*)"Drive3CPU_TRACE", NULL,
      false },

    UI_MENU_SEPARATOR,

    { "Blitter log", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_BLITTER_LOG_TOGGLE,
      ui_toggle_resource, (void*)"DtvBlitterLog", NULL,
      false },
    { "DMA log", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_DMA_LOG_TOGGLE,
      ui_toggle_resource, (void*)"DtvDMALog", NULL,
      false },
    { "Flash log", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_FLASH_LOG_TOGGLE,
      ui_toggle_resource, (void*)"DtvFlashLog", NULL,
      false },

    UI_MENU_SEPARATOR,

    { "Autoplay playback frames ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DEBUG_AUTOPLAYBACK_FRAMES,
      ui_debug_playback_frames_dialog_show, NULL, NULL,
      true },
    { "Save core dump", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_CORE_DUMP_TOGGLE,
      ui_toggle_resource, (void*)"DoCoreDump", NULL,
      false },

    UI_MENU_TERMINATOR
};
/* }}} */
#endif


/* {{{ help_menu[] */
/** \brief  'Help' menu items
 */
static ui_menu_item_t help_menu[] = {
    { "Browse manual", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_MANUAL,
      ui_open_manual_callback, NULL, NULL,
      true },
    { "Command line options...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_COMMAND_LINE,
      uicmdline_dialog_show, NULL, NULL,
      true },
    { "Compile time features...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_COMPILE_TIME,
      uicompiletimefeatures_dialog_show, NULL, NULL,
      true },
    { "Hotkeys ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_HOTKEYS,
      ui_popup_hotkeys_settings, NULL, NULL,
      true },
    { "About VICE", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_ABOUT,
      ui_about_dialog_callback, NULL, NULL,
      true },

    UI_MENU_TERMINATOR
};
/* }}} */

/** \brief  'File' menu - tape section pointer
 *
 * Set by ui_machine_menu_bar_create().
 */
static ui_menu_item_t *file_menu_tape_section = NULL;

/** \brief  'File' menu - cart section pointer
 *
 * Set by ui_machine_menu_bar_create().
 */
static ui_menu_item_t *file_menu_cart_section = NULL;

/** \brief  'Settings' menu - joystick section pointer
 *
 * Set by ui_machine_menu_bar_create().
 */
static ui_menu_item_t *settings_menu_joy_section = NULL;


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
            file_menu_tape_section = file_menu_tape;
            file_menu_cart_section = file_menu_cart;
            settings_menu_joy_section = settings_menu_joy_swap;
            speed_submenu[MACHINE_FPS_INDEX].label = "C64 FPS";
            break;

        case VICE_MACHINE_C64DTV:
            settings_menu_joy_section = settings_menu_joy_swap;
            speed_submenu[MACHINE_FPS_INDEX].label = "C64DTV FPS";
            break;

        case VICE_MACHINE_SCPU64:
            file_menu_cart_section = file_menu_cart;
            settings_menu_joy_section = settings_menu_joy_swap;
            break;

        case VICE_MACHINE_C128:
            file_menu_tape_section = file_menu_tape;
            file_menu_cart_section = file_menu_cart;
            settings_menu_joy_section = settings_menu_joy_swap;
            speed_submenu[MACHINE_FPS_INDEX].label = "C128 FPS";
            break;

        case VICE_MACHINE_VIC20:
            file_menu_tape_section = file_menu_tape;
            file_menu_cart_section = file_menu_cart;
            speed_submenu[MACHINE_FPS_INDEX].label = "VIC20 FPS";
            break;

        case VICE_MACHINE_PLUS4:
            file_menu_tape_section = file_menu_tape;
            file_menu_cart_section = file_menu_cart;
            settings_menu_joy_section = settings_menu_joy_swap;
            speed_submenu[MACHINE_FPS_INDEX].label = "Plus4 FPS";
            break;

        case VICE_MACHINE_CBM5x0:
            file_menu_tape_section = file_menu_tape;
            file_menu_cart_section = file_menu_cart;
            settings_menu_joy_section = settings_menu_joy_swap;
            speed_submenu[MACHINE_FPS_INDEX].label = "CBM-II FPS";
            break;

        case VICE_MACHINE_CBM6x0:
            file_menu_tape_section = file_menu_tape;
            file_menu_cart_section = file_menu_cart;
            speed_submenu[MACHINE_FPS_INDEX].label = "CBM-II FPS";
            break;

        case VICE_MACHINE_PET:
            file_menu_tape_section = file_menu_tape_xpet;
            speed_submenu[MACHINE_FPS_INDEX].label = "PET FPS";
            break;

        case VICE_MACHINE_VSID:
            archdep_vice_exit(1);
            speed_submenu[MACHINE_FPS_INDEX].label = "VSID FPS";
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
