/** \file   uimachinemenu.c
 * \brief   Native GTK3 menus for machine emulators (not vsid)
 *
 * \author  Marcus Sutton <loggedoubt@gmail.com>
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES WarpMode        all
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
#include "uihotkeys.h"
#include "uimedia.h"
#include "uimenu.h"
#include "uimonarch.h"
#include "uisettings.h"
#include "uismartattach.h"
#include "uisnapshot.h"
#include "uitapeattach.h"
#include "uitapecreate.h"

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



#if 0
/** \brief  File submenu
 */
static GtkWidget *file_submenu = NULL;


/** \brief  Edit submenu
 */
static GtkWidget *edit_submenu = NULL;


/** \brief  Snapshot submenu
 */
static GtkWidget *snapshot_submenu = NULL;


/** \brief  Settings submenu
 */
static GtkWidget *settings_submenu = NULL;


#ifdef DEBUG
/** \brief  Debug submenu, only available when --enable-debug was specified
 */
static GtkWidget *debug_submenu = NULL;
#endif


/** \brief  Help submenu
 */
static GtkWidget *help_submenu = NULL;
#endif


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

            path = archdep_join_paths(cwd, vice_config_file, NULL);
            g_free(cwd);
        } else {
            path = lib_strdup(vice_config_file);
        }
    } else {
        /* default vicerc or vice.ini */
        path = archdep_join_paths(archdep_user_config_path(),
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
        ui_disk_detach_callback, UNIT_DRIVE_TO_PTR(8, 0),
        GDK_KEY_8, VICE_MOD_MASK|GDK_CONTROL_MASK, false },
    { "Drive 8:1", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_8_1,
        ui_disk_detach_callback, UNIT_DRIVE_TO_PTR(8, 1),
        0, 0, false },
    { "Drive 9:0", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_9_0,
        ui_disk_detach_callback, UNIT_DRIVE_TO_PTR(9, 0),
        GDK_KEY_9, VICE_MOD_MASK|GDK_CONTROL_MASK, false },
    { "Drive 9:1", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_9_1,
        ui_disk_detach_callback, UNIT_DRIVE_TO_PTR(9, 1),
        0, 0, false },
    { "Drive 10:0", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_10_0,
        ui_disk_detach_callback, UNIT_DRIVE_TO_PTR(10, 0),
        GDK_KEY_0, VICE_MOD_MASK|GDK_CONTROL_MASK, false },
    { "Drive 10:1", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_10_1,
        ui_disk_detach_callback, UNIT_DRIVE_TO_PTR(10, 1),
        0, 0, false },
    { "Drive 11:0", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_11_0,
        ui_disk_detach_callback, UNIT_DRIVE_TO_PTR(11, 0),
        GDK_KEY_1, VICE_MOD_MASK|GDK_CONTROL_MASK },
    { "Drive 11:1", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_11_1,
        ui_disk_detach_callback, UNIT_DRIVE_TO_PTR(11, 1),
        0, 0, false },
    { "Detach all", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_DRIVE_DETACH_ALL,
        ui_disk_detach_all_callback, NULL,
        0, 0, false },
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ disk_attach_submenu[] */
/** \brief  File->Attach disk submenu
 */
static ui_menu_item_t disk_attach_submenu[] = {
    { "Drive #8", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_ATTACH_8_0,
      ui_disk_attach_dialog_show, GINT_TO_POINTER(8),
      GDK_KEY_8, VICE_MOD_MASK, true },
    { "Drive #9", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_ATTACH_9_0,
      ui_disk_attach_dialog_show, GINT_TO_POINTER(9),
      GDK_KEY_9, VICE_MOD_MASK, true },
    { "Drive #10", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_ATTACH_10_0,
      ui_disk_attach_dialog_show, GINT_TO_POINTER(10),
      GDK_KEY_0, VICE_MOD_MASK, true },
    { "Drive #11", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_ATTACH_11_0,
      ui_disk_attach_dialog_show, GINT_TO_POINTER(11),
      GDK_KEY_1, VICE_MOD_MASK, true },
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ disk_fliplist_submenu[] */
/** \brief  File->Flip list submenu
 */
static ui_menu_item_t disk_fliplist_submenu[] = {
    { "Add current image (Unit #8)", UI_MENU_TYPE_ITEM_ACTION,
        ACTION_FLIPLIST_ADD,
        ui_fliplist_add_current_cb, GINT_TO_POINTER(8),
        GDK_KEY_I, VICE_MOD_MASK, false },
    { "Remove current image (Unit #8)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_REMOVE,
      ui_fliplist_remove_current_cb, GINT_TO_POINTER(8),
      GDK_KEY_K, VICE_MOD_MASK, false },
    { "Attach next image (Unit #8)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_NEXT,
      ui_fliplist_next_cb, GINT_TO_POINTER(8),
      GDK_KEY_N, VICE_MOD_MASK, false },
    { "Attach previous image (Unit #8)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_PREVIOUS,
      ui_fliplist_prev_cb, GINT_TO_POINTER(8),
      GDK_KEY_N, VICE_MOD_MASK | GDK_SHIFT_MASK, false },
    { "Clear fliplist (Unit #8)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_CLEAR,
      ui_fliplist_clear_cb, GINT_TO_POINTER(8), 0, false },
    { "Load flip list file...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_LOAD,
      ui_fliplist_load_callback, GINT_TO_POINTER(8),
      0, 0, true },
    { "Save flip list file...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_FLIPLIST_SAVE,
      ui_fliplist_save_callback, GINT_TO_POINTER(8),
      0, 0, true },
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ datasette_1_control_submenu[] */
/** \brief  File->Datasette control submenu for port #1
 */
static ui_menu_item_t datasette_1_control_submenu[] = {
    { "Stop", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_STOP_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_STOP | (1 << 8)),
      0, 0, false },
    { "Start", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_PLAY_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_START | (1 << 8)),
      0, 0, false },
    { "Forward", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_FFWD_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_FORWARD | (1 << 8)),
      0, 0, false },
    { "Rewind", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_REWIND_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_REWIND | (1 << 8)),
      0, 0, false },
    { "Record", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RECORD_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_RECORD | (1 << 8)),
      0, 0, false },
    { "Reset", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RESET_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_RESET | (1 << 8)),
      0, 0, false },
    { "Reset Counter", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RESET_COUNTER_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_RESET_COUNTER | ( 1<< 8)),
      0, 0, false },
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ datasette_2_control_submenu[] */
/** \brief  File->Datasette control submenu for port #2
 */
static ui_menu_item_t datasette_2_control_submenu[] = {
    { "Stop", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_STOP_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_STOP | (2 << 8)),
      0, 0, false },
    { "Start", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_PLAY_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_START | (2 << 8)),
      0, 0, false },
    { "Forward", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_FFWD_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_FORWARD | (2 << 8)),
      0, 0, false },
    { "Rewind", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_REWIND_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_REWIND | (2 << 8)),
      0, 0, false },
    { "Record", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RECORD_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_RECORD | (2 << 8)),
      0, 0, false },
    { "Reset", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RESET_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_RESET | (2 << 8)),
      0, 0, false },
    { "Reset Counter", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_RESET_COUNTER_1, ui_datasette_tape_action_cb,
      GINT_TO_POINTER(DATASETTE_CONTROL_RESET_COUNTER | (2 << 8)),
      0, 0, false },
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ reset_submenu[] */
/** \brief  File->Reset submenu
 */
static ui_menu_item_t reset_submenu[] = {
    { "Soft reset", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_SOFT, ui_machine_reset_callback,
      GINT_TO_POINTER(MACHINE_RESET_MODE_SOFT),
      GDK_KEY_F9, VICE_MOD_MASK, false },
    { "Hard reset", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_HARD, ui_machine_reset_callback,
      GINT_TO_POINTER(MACHINE_RESET_MODE_HARD),
      GDK_KEY_F12, VICE_MOD_MASK, false },
    UI_MENU_SEPARATOR,
    { "Reset drive #8", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_DRIVE_8, ui_drive_reset_callback,GINT_TO_POINTER(8),
      0, 0, false },
    { "Reset drive #9", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_DRIVE_9, ui_drive_reset_callback, GINT_TO_POINTER(9),
      0, 0, false },
    { "Reset drive #10", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_DRIVE_10, ui_drive_reset_callback, GINT_TO_POINTER(10),
      0, 0, false },
    { "Reset drive #11", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESET_DRIVE_11, ui_drive_reset_callback, GINT_TO_POINTER(11),
      0, 0, false },
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ file_menu_head[] */
/** \brief  'File' menu - head section
 */
static ui_menu_item_t file_menu_head[] = {
    { "Smart attach ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SMART_ATTACH, ui_smart_attach_dialog_show, NULL,
      GDK_KEY_A, VICE_MOD_MASK, true },
    UI_MENU_SEPARATOR,
    { "Attach disk image", UI_MENU_TYPE_SUBMENU,
      "attach-disk-submenu", NULL, disk_attach_submenu,
      0, 0, false },
    { "Create and attach an empty disk image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DRIVE_CREATE, ui_disk_create_dialog_show, GINT_TO_POINTER(8),
      0, 0, true },
    { "Detach disk image", UI_MENU_TYPE_SUBMENU,
      NULL, NULL, disk_detach_submenu,
      0, 0, false },
    { "Flip list", UI_MENU_TYPE_SUBMENU,
      NULL, NULL, disk_fliplist_submenu,
      0, 0, false },
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
      ui_tape_attach_callback, (void*)1,
      GDK_KEY_T, VICE_MOD_MASK, true },
    { "Create and attach datasette image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_CREATE_1,
        ui_tape_create_dialog_show, (void*)1,
        0, 0, true },
    { "Detach datasette image", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_DETACH_1,
      ui_tape_detach_callback, (void*)1,
      0, 0, false },
    { "Datasette controls", UI_MENU_TYPE_SUBMENU,
      NULL, NULL, datasette_1_control_submenu,
      0, 0, false },
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
      ui_tape_attach_callback, (void*)1,
      GDK_KEY_T, VICE_MOD_MASK, true },
    { "Create and attach datasette #1 image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_CREATE_1,
      ui_tape_create_dialog_show, (void*)1,
      0, 0, true },
    { "Detach datasette #1 image", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_DETACH_1,
      ui_tape_detach_callback, (void*)1,
      0, 0, false },
    /* TODO: fix the submenus: */
    { "Datasette #1 controls", UI_MENU_TYPE_SUBMENU,
      NULL, NULL, datasette_1_control_submenu,
      0, 0, false },

    UI_MENU_SEPARATOR,

    { "Attach datasette #2 image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_ATTACH_2,
      ui_tape_attach_callback, (void*)2,
      GDK_KEY_T, VICE_MOD_MASK, true },
   { "Create and attach datasette #2 image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_CREATE_2,
      ui_tape_create_dialog_show, (void*)2,
      0, 0, true },
   { "Detach datasette #2 image", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_TAPE_DETACH_2,
      ui_tape_detach_callback, (void*)2,
      0, 0, false },
   { "Datasette #2 controls", UI_MENU_TYPE_SUBMENU,
      NULL, NULL, datasette_2_control_submenu,
      0, 0, false },

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
      ui_cart_show_dialog, GINT_TO_POINTER(0),
      GDK_KEY_C, VICE_MOD_MASK, true },
    { "Detach cartridge image(s)", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_CART_DETACH,
      (void *)ui_cart_detach, NULL,
      GDK_KEY_C, VICE_MOD_MASK|GDK_SHIFT_MASK, false },
    { "Cartridge freeze", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_CART_FREEZE,
      (void *)ui_cart_trigger_freeze, NULL,
      GDK_KEY_Z, VICE_MOD_MASK, false },
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
      ui_monitor_activate_callback, NULL,
#ifdef MACOSX_SUPPORT
      /* use Command-Option-M on Mac */
      GDK_KEY_M, VICE_MOD_MASK | GDK_MOD1_MASK,
#else
      GDK_KEY_H, VICE_MOD_MASK,
#endif
      false
    },
    UI_MENU_SEPARATOR,
    { "Reset", UI_MENU_TYPE_SUBMENU,
      NULL, NULL, reset_submenu,
      0, 0, false },
    UI_MENU_SEPARATOR,
    { "Exit emulator", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_QUIT, ui_close_callback, NULL,
      GDK_KEY_Q, VICE_MOD_MASK, true },
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ edit_menu[] */
/** \brief  'Edit' menu
 */
static ui_menu_item_t edit_menu[] = {
    { "Copy", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_EDIT_COPY, (void *)ui_copy_callback, NULL,
      GDK_KEY_Delete, VICE_MOD_MASK, false },
    { "Paste", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_EDIT_PASTE, (void *)ui_paste_callback, NULL,
      GDK_KEY_Insert, VICE_MOD_MASK, false },
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ snapshot_menu[] */
/** \brief  'Snapshot' menu
 */
static ui_menu_item_t snapshot_menu[] = {
    { "Load snapshot image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SNAPSHOT_LOAD, ui_snapshot_open_file, NULL,
      GDK_KEY_L, VICE_MOD_MASK, false },
    { "Save snapshot image ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SNAPSHOT_SAVE, ui_snapshot_save_file, NULL,
      GDK_KEY_S, VICE_MOD_MASK, false },
    UI_MENU_SEPARATOR,
    { "Quickload snapshot", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SNAPSHOT_QUICKLOAD, ui_snapshot_quickload_snapshot, NULL,
      GDK_KEY_F10, VICE_MOD_MASK, false },
    { "Quicksave snapshot", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SNAPSHOT_QUICKSAVE, ui_snapshot_quicksave_snapshot, NULL,
      GDK_KEY_F11, VICE_MOD_MASK, false },
    UI_MENU_SEPARATOR,
#if 0
    { "Select history directory ...", UI_MENU_TYPE_ITEM_ACTION,
        "history-select-dir", ui_snapshot_history_select_dir, "0:3",
        0, 0, false },
#endif
    { "Start recording events", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_RECORD_START, ui_snapshot_history_record_start, NULL,
      0, 0, false },
    { "Stop recording events", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_RECORD_STOP, ui_snapshot_history_record_stop, NULL,
      0, 0, false },
    { "Start playing back events", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_PLAYBACK_START, ui_snapshot_history_playback_start, NULL,
      0, 0, false },
    { "Stop playing back events", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_PLAYBACK_STOP, ui_snapshot_history_playback_stop, NULL,
      0, 0, false },
    { "Set recording milestone", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_MILESTONE_SET, ui_snapshot_history_milestone_set, NULL,
      GDK_KEY_E, VICE_MOD_MASK, false },
    { "Return to milestone", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HISTORY_MILESTONE_RESET, ui_snapshot_history_milestone_reset, NULL,
      GDK_KEY_U, VICE_MOD_MASK, false },
    UI_MENU_SEPARATOR,
#if 0
    { "Recording start mode ...", UI_MENU_TYPE_ITEM_ACTION,
        "history-recording-start-mode", ui_settings_dialog_create, "20,0",
        0, 0, true },

    UI_MENU_SEPARATOR,
#endif
    { "Save/Record media ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_MEDIA_RECORD, ui_media_dialog_show, NULL,
      GDK_KEY_R, VICE_MOD_MASK | GDK_SHIFT_MASK, false },

    { "Stop media recording", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_MEDIA_STOP, (void *)ui_media_stop_recording, NULL,
      GDK_KEY_S, VICE_MOD_MASK | GDK_SHIFT_MASK, false },

    UI_MENU_SEPARATOR,
    { "Quicksave screenshot", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SCREENSHOT_QUICKSAVE,
      (void *)ui_media_auto_screenshot, NULL,
      0, 0, false },
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ settings_menu_head[] */
/** \brief  'Settings' menu - head section
 */
static ui_menu_item_t settings_menu_head[] = {
    { "Fullscreen", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_FULLSCREEN_TOGGLE,
      (void *)ui_action_toggle_fullscreen, NULL,
      GDK_KEY_D, VICE_MOD_MASK, true },
    { "Restore display state", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_RESTORE_DISPLAY,
      (void *)ui_restore_display, NULL,
      GDK_KEY_r, VICE_MOD_MASK, true },
#if 1
    { "Show menu/status in fullscreen", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_FULLSCREEN_DECORATIONS_TOGGLE,
      (void *)ui_action_toggle_fullscreen_decorations, NULL,
      GDK_KEY_B, VICE_MOD_MASK, true },
#else
    /* Mac menubar version */
    { "Show statusbar in fullscreen", UI_MENU_TYPE_ITEM_ACTION,
        "fullscreen-widgets", ui_fullscreen_decorations_callback, NULL,
        GDK_KEY_B, VICE_MOD_MASK, false },
#endif

    UI_MENU_SEPARATOR,

    { "Warp mode", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_WARP_MODE_TOGGLE,
      (void *)ui_action_toggle_warp, NULL,
      GDK_KEY_W, VICE_MOD_MASK, false },
    { "Pause emulation", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_PAUSE_TOGGLE,
      (void *)ui_action_toggle_pause, NULL,
      GDK_KEY_P, VICE_MOD_MASK, false },
    { "Advance frame", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_ADVANCE_FRAME,
      (void *)ui_action_advance_frame, NULL,
      GDK_KEY_P, VICE_MOD_MASK | GDK_SHIFT_MASK, false },

    UI_MENU_SEPARATOR,

    { "Mouse grab", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_MOUSE_GRAB_TOGGLE,
      (void *)ui_action_toggle_mouse_grab, NULL,
      GDK_KEY_M, VICE_MOD_MASK, false },
#if 0
    { "Swap controlport joysticks", UI_MENU_TYPE_ITEM_CHECK,
        ACTION_TOGGLE_CONTROLPORT_SWAP,
        (void *)ui_action_toggle_controlport_swap, NULL,
        GDK_KEY_J, VICE_MOD_MASK, false },
    { "Swap userport joysticks", UI_MENU_TYPE_ITEM_CHECK,
        ACTION_TOGGLE_USERPORT_SWAP,
        (void *)ui_action_toggle_userport_swap, NULL,
        GDK_KEY_U, VICE_MOD_MASK|GDK_SHIFT_MASK, false },
#endif
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ settings_menu_joy_both[] */

/** \brief  Settings menu - joystick - with controlport swap
 *
 * Has controlport, userport and keyset items
 *
 * Vaalid for x64/x64sc/x64dtv/xscpu64/x128/xplus4/xcbm5x0
 */
static ui_menu_item_t settings_menu_joy_with_swap[] = {
    { "Swap joysticks", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_SWAP_CONTROLPORT_TOGGLE, (void *)(ui_action_toggle_controlport_swap), NULL,
      GDK_KEY_J, VICE_MOD_MASK, false },
    { "Allow keyset joystick", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_KEYSET_JOYSTICK_TOGGLE, (void *)(ui_toggle_resource), (void *)"KeySetEnable",
      GDK_KEY_J, VICE_MOD_MASK | GDK_SHIFT_MASK, false },
    UI_MENU_TERMINATOR
};
/* }}} */


/* {{{ settings_menu_joy_userport[] */
/** \brief  Settings menu - joystick - without controlport swap
 *
 * Only valid for xvic/xpet/xcbm2
 */
static ui_menu_item_t settings_menu_joy_without_swap[] = {
    { "Allow keyset joystick", UI_MENU_TYPE_ITEM_CHECK,
        "keyset", (void *)(ui_toggle_resource), (void *)"KeySetEnable",
        GDK_KEY_J, VICE_MOD_MASK | GDK_SHIFT_MASK, false },
    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ settings_menu_tail[] */
/** \brief  'Settings' menu tail section
 */
static ui_menu_item_t settings_menu_tail[] = {
    UI_MENU_SEPARATOR,
    /* the settings dialog */
    { "Settings ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_DIALOG, ui_settings_dialog_create_and_activate_node_callback, NULL,
      GDK_KEY_O, VICE_MOD_MASK, true },
    { "Load settings", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_LOAD, settings_load_callback, NULL,
      0, 0, false },
    { "Load settings from ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_LOAD_FROM, settings_load_custom_callback, NULL,
        0, 0, true },
    { "Load extra settings from ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_LOAD_EXTRA, settings_load_custom_callback, (void*)1,
      0, 0, true },
    { "Save settings", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_SAVE, settings_save_callback, NULL,
      0, 0, false },
    { "Save settings to ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_SAVE_TO, settings_save_custom_callback, NULL,
      0, 0, true },
    { "Restore default settings", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_SETTINGS_DEFAULT, (void *)ui_restore_default_settings, NULL,
      0, 0, true },
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
      ACTION_DEBUG_TRACE_MODE, ui_debug_trace_mode_dialog_show, NULL,
      0, 0, true },

    UI_MENU_SEPARATOR,

    { "Main CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_CPU_TOGGLE, (void *)(ui_toggle_resource), (void *)"MainCPU_TRACE",
      0, 0, false },

    UI_MENU_SEPARATOR,

    { "IEC bus trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_IEC_TOGGLE, (void *)(ui_toggle_resource), (void *)"IEC_TRACE",
      0, 0, false },

    { "IEEE-488 bus trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_IEEE488_TOGGLE, (void *)(ui_toggle_resource), (void *)"IEEE_TRACE",
        0, 0, false },

    UI_MENU_SEPARATOR,

    { "Drive #8 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE, (void *)(ui_toggle_resource), (void *)"Drive0CPU_TRACE",
      0, 0, false },
    { "Drive #9 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE, (void *)(ui_toggle_resource), (void *)"Drive1CPU_TRACE",
      0, 0, false },
    { "Drive #10 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE, (void *)(ui_toggle_resource), (void *)"Drive2CPU_TRACE",
      0, 0, false },
    { "Drive #11 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE, (void *)(ui_toggle_resource), (void *)"Drive3CPU_TRACE",
      0, 0, false },

    UI_MENU_SEPARATOR,

    { "Autoplay playback frames ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DEBUG_AUTOPLAYBACK_FRAMES, ui_debug_playback_frames_dialog_show, NULL,
      0, 0, true },
    { "Save core dump", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_CORE_DUMP_TOGGLE, (void *)(ui_toggle_resource), (void *)"DoCoreDump",
      0, 0, false },

    UI_MENU_TERMINATOR
};
/* }}} */

/* {{{ debug_menu_c64dtv[] */
/** \brief  'Debug' menu items for x64dtv
 */
static ui_menu_item_t debug_menu_c64dtv[] = {
    { "Trace mode ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DEBUG_TRACE_MODE, ui_debug_trace_mode_dialog_show, NULL,
      0, 0, true },

    UI_MENU_SEPARATOR,

    { "Main CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_CPU_TOGGLE, (void *)(ui_toggle_resource), (void *)"MainCPU_TRACE",
      0, 0, false },

    UI_MENU_SEPARATOR,

    { "IEC bus trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_IEC_TOGGLE, (void *)(ui_toggle_resource), (void *)"IEC_TRACE",
        0, 0, false },

    { "Drive #8 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE, (void *)(ui_toggle_resource), (void *)"Drive0CPU_TRACE",
      0, 0, false },
    { "Drive #9 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE, (void *)(ui_toggle_resource), (void *)"Drive1CPU_TRACE",
      0, 0, false },
    { "Drive #10 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE, (void *)(ui_toggle_resource), (void *)"Drive2CPU_TRACE",
      0, 0, false },
    { "Drive #11 CPU trace", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE, (void *)(ui_toggle_resource), (void *)"Drive3CPU_TRACE",
      0, 0, false },

    UI_MENU_SEPARATOR,

    { "Blitter log", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_BLITTER_LOG_TOGGLE, ui_toggle_resource, (void *)"DtvBlitterLog",
      0, 0, false },
    { "DMA log", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_DMA_LOG_TOGGLE, ui_toggle_resource, (void *)"DtvDMALog",
      0, 0, false },
    { "Flash log", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_FLASH_LOG_TOGGLE, ui_toggle_resource, (void*)"DtvFlashLog",
      0, 0, false },

    UI_MENU_SEPARATOR,

    { "Autoplay playback frames ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_DEBUG_AUTOPLAYBACK_FRAMES, ui_debug_playback_frames_dialog_show, NULL,
      0, 0, true },
    { "Save core dump", UI_MENU_TYPE_ITEM_CHECK,
      ACTION_DEBUG_CORE_DUMP_TOGGLE, ui_toggle_resource, (void *)"DoCoreDump",
      0, 0, false },

    UI_MENU_TERMINATOR
};
/* }}} */
#endif


/* {{{ help_menu[] */
/** \brief  'Help' menu items
 */
static ui_menu_item_t help_menu[] = {
    { "Browse manual", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_MANUAL, ui_open_manual_callback, NULL,
      0, 0, true },
    { "Command line options ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_COMMAND_LINE, uicmdline_dialog_show, NULL,
      0, 0, true },
    { "Compile time features ...", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_COMPILE_TIME, uicompiletimefeatures_dialog_show, NULL,
      0, 0, true },
    { "Hotkeys", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_HOTKEYS, uihotkeys_dialog_show, NULL,
      0, 0, true },
    { "About VICE", UI_MENU_TYPE_ITEM_ACTION,
      ACTION_HELP_ABOUT, ui_about_dialog_callback, NULL,
      0, 0, true },

    UI_MENU_TERMINATOR
};
/* }}} */


/** \brief  References to menus (and submenus)
 *
 * This array contains references to the (sub)menus registered with the UI.
 *
 * The hotkeys interface will use this array to look up items by action name
 * and set/unset their hotkey ID and modifiers. This happens once on emulator
 * init, before the menus are created.
 */
static ui_menu_ref_t menu_references[] = {
    /* File */
    { "file-section-head",                  file_menu_head },
    { "file-submenu-attach-disk",           disk_attach_submenu },
    { "file-submenu-detach-disk",           disk_detach_submenu },
    { "file-submenu-disk-fliplist",         disk_fliplist_submenu },
    { "file-section-tape",                  file_menu_tape },
    { "file-section-tape-xpet",             file_menu_tape_xpet },
    { "file-submenu-tape1-controls",        datasette_1_control_submenu },
    { "file-submenu-tape2-controls",        datasette_2_control_submenu },
    { "file-section-cart",                  file_menu_cart },
    { "file-submenu-reset",                 reset_submenu },
    { "file-section-tail",                  file_menu_tail },

    /* Edit */
    { "edit",                               edit_menu },

    /* Snapshots, events, video/audio/screenshot */
    { "snapshot",                           snapshot_menu },

    /* Settings */
    { "settings-section-head",              settings_menu_head },
    { "settings-section-joy-with-swap",     settings_menu_joy_with_swap },
    { "settings-section-joy-without-swap",  settings_menu_joy_without_swap },
    { "settings-section-tail",              settings_menu_tail },

    /* Debug */
#ifdef DEBUG
    { "debug",                              debug_menu },
    { "debug-c64dtv",                       debug_menu_c64dtv },
#endif

    /* Help */
    { "help",                               help_menu },

    { NULL,                                 NULL },
};


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
 * \return  GtkMenuBar
 */
GtkWidget *ui_machine_menu_bar_create(void)
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

#if 0
    /* Test looking up a menu item via name */
    ui_menu_item_t *item = ui_get_vice_menu_item_by_name("reset-soft");
    if (item != NULL) {
        ui_set_vice_menu_item_hotkey(item, "z", GDK_MOD1_MASK | GDK_SHIFT_MASK);
    }
#endif

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
            settings_menu_joy_section = settings_menu_joy_with_swap;
            break;

        case VICE_MACHINE_C64DTV:
            settings_menu_joy_section = settings_menu_joy_with_swap;
            break;

        case VICE_MACHINE_SCPU64:
            file_menu_cart_section = file_menu_cart;
            settings_menu_joy_section = settings_menu_joy_with_swap;
            break;

        case VICE_MACHINE_C128:
            file_menu_tape_section = file_menu_tape;
            file_menu_cart_section = file_menu_cart;
            settings_menu_joy_section = settings_menu_joy_with_swap;
            break;

        case VICE_MACHINE_VIC20:
            file_menu_tape_section = file_menu_tape;
            file_menu_cart_section = file_menu_cart;
            settings_menu_joy_section = settings_menu_joy_without_swap;
            break;

        case VICE_MACHINE_PLUS4:
            file_menu_tape_section = file_menu_tape;
            file_menu_cart_section = file_menu_cart;
            settings_menu_joy_section = settings_menu_joy_with_swap;
            break;

        case VICE_MACHINE_CBM5x0:
            file_menu_tape_section = file_menu_tape;
            file_menu_cart_section = file_menu_cart;
            settings_menu_joy_section = settings_menu_joy_with_swap;
            break;

        case VICE_MACHINE_CBM6x0:
            file_menu_tape_section = file_menu_tape;
            file_menu_cart_section = file_menu_cart;
            settings_menu_joy_section = settings_menu_joy_without_swap;
            break;

        case VICE_MACHINE_PET:
            file_menu_tape_section = file_menu_tape_xpet;
            settings_menu_joy_section = settings_menu_joy_without_swap;
            break;

        case VICE_MACHINE_VSID:
            archdep_vice_exit(1);
            break;
        default:
            break;
    }

    /* add items to the File menu */
    ui_menu_add(file_submenu, file_menu_head);
    if (file_menu_tape_section != NULL) {
        ui_menu_add(file_submenu, file_menu_tape_section);
    }
    if (file_menu_cart_section != NULL) {
        ui_menu_add(file_submenu, file_menu_cart_section);
    }
    ui_menu_add(file_submenu, file_menu_tail);

    /* add items to the Edit menu */
    ui_menu_add(edit_submenu, edit_menu);
    /* add items to the Snapshot menu */
    ui_menu_add(snapshot_submenu, snapshot_menu);

    /* add items to the Settings menu */
    ui_menu_add(settings_submenu, settings_menu_head);
    if (settings_menu_joy_section != NULL) {
        ui_menu_add(settings_submenu, settings_menu_joy_section);
    }
    ui_menu_add(settings_submenu, settings_menu_tail);

#ifdef DEBUG
    /* add items to the Debug menu */
    if (machine_class == VICE_MACHINE_C64DTV) {
        ui_menu_add(debug_submenu, debug_menu_c64dtv);
    } else {
        ui_menu_add(debug_submenu, debug_menu);
    }
#endif

    /* add items to the Help menu */
    ui_menu_add(help_submenu, help_menu);

    main_menu_bar = menu_bar;    /* XXX: do I need g_object_ref()/g_object_unref()
                                         for this */
    return menu_bar;
}


/** \brief  Add missing settings load/save items
 *
 * \param[in,out]   menu    GtkMenu
 */
void ui_machine_menu_bar_vsid_patch(GtkWidget *menu)
{
    ui_menu_add(menu, settings_menu_tail);
}


/*
 * API for the hotkeys
 */


/* Iterator API */

/** \brief  Initialize VICE menu item iterator
 *
 * \param[in]   iter    VICE menu item iterator
 */
void ui_vice_menu_iter_init(ui_vice_menu_iter_t *iter)
{
    ui_menu_ref_t ref;

    iter->menu_index = 0;
    ref = menu_references[0];
    iter->menu_item = ref.items;
}


/** \brief  Move VICE menu item iterator to the next item
 *
 * \param[in]   iter    VICE menu item iterator
 *
 * \return  TRUE when there was a next item
 *
 * \note    Please note that this also returns menu items of types 'submenu'
 *          (UI_MENU_TYPE_SUBMENU) and 'separator' (UI_MENU_TYPE_SEPARATOR).
 * \see     ui_vice_menu_iter_get_type()
 */
gboolean ui_vice_menu_iter_next(ui_vice_menu_iter_t *iter)
{
    /* guard against calling this function on invalid iter */
    if (iter->menu_item == NULL) {
        debug_gtk3("\nWARNING: called on invalid iterator, fix code!'\n");
        return FALSE;
    }

    iter->menu_item++;
    /* while loop to handle empty menus */
    while (iter->menu_item->type == UI_MENU_TYPE_GUARD) {
        /* next menu in list */
        ui_menu_ref_t ref;

        iter->menu_index++;
        ref = menu_references[iter->menu_index];
        iter->menu_item = ref.items;
        if (iter->menu_item == NULL) {
            return FALSE;
        }
    }
    return TRUE;
}


/** \brief  Get VICE menu item type via iterator
 *
 * \param[in]   iter    VICE menu item iterator
 * \param[out]  type    object to store item type
 *
 * \return  TRUE when iter was valid
 *
*/
gboolean ui_vice_menu_iter_get_type(ui_vice_menu_iter_t *iter,
                                    ui_menu_item_type_t *type)
{
    if (iter->menu_item != NULL) {
        *type = iter->menu_item->type;
        return TRUE;
    } else {
        *type = UI_MENU_TYPE_GUARD;    /* -1 */
        return FALSE;
    }
}


/** \brief  Get VICE menu item action name via iterator
 *
 * \param[in]   iter    VICE menu item iterator
 * \param[out]  name    object to store action name
 *
 * \return  TRUE when iter was valid
 *
 * \note    name can be `NULL` for submenus and separators
 */
gboolean ui_vice_menu_iter_get_name(ui_vice_menu_iter_t *iter,
                                    const char **name)
{
    if (iter->menu_item != NULL) {
        *name = iter->menu_item->action_name;
        return TRUE;
    } else {
        *name = NULL;
        return FALSE;
    }
}


/** \brief  Get VICE menu item hotkey modifier mask and keysym via iterator
 *
 * \param[in]   iter    VICE menu item iterator
 * \param[out]  mask    Gdk modifier mask
 * \param[out]  keysym  Gdk keysym
 *
 * \return  TRUE when iter was valid
 */
gboolean ui_vice_menu_iter_get_hotkey(ui_vice_menu_iter_t *iter,
                                    GdkModifierType *mask,
                                    guint *keysym)
{
    if (iter->menu_item != NULL) {
        *mask = iter->menu_item->modifier;
        *keysym = iter->menu_item->keysym;
        return TRUE;
    } else {
        *mask = 0;
        *keysym = 0;
        return FALSE;
    }
}


/* Menu item API */

/** \brief  Scan menu items for an action called \a name
 *
 * \param[in]   name    item action name
 *
 * \return  pointer to menu item or `NULL` when not found
 */
ui_menu_item_t *ui_get_vice_menu_item_by_name(const char *name)
{
    ui_vice_menu_iter_t iter;
    ui_menu_item_type_t type;
    const char *item_name;

    ui_vice_menu_iter_init(&iter);
    do {
        if (ui_vice_menu_iter_get_type(&iter, &type) &&
                (type == UI_MENU_TYPE_ITEM_ACTION ||
                 type == UI_MENU_TYPE_ITEM_CHECK)) {
            if (ui_vice_menu_iter_get_name(&iter, &item_name) &&
                    item_name != NULL) {
                //debug_gtk3("Checking '%s'.", item_name);
                if (strcmp(item_name, name) == 0) {
                    return iter.menu_item;
                }
            }
        }
    } while (ui_vice_menu_iter_next(&iter));
    return NULL;
}


/** \brief  Scan menu items for hotkey
 *
 * \param[in]   mask    Gdk modifier(s)
 * \param[in]   keysym  Gdk keysym
 *
 * \return  pointer to menu item or `NULL` when not found
 */
ui_menu_item_t* ui_get_vice_menu_item_by_hotkey(GdkModifierType mask,
                                                guint keysym)
{
    ui_vice_menu_iter_t iter;
    ui_menu_item_type_t type;
    GdkModifierType item_mask;
    guint item_keysym;

    ui_vice_menu_iter_init(&iter);
    do {
        if (ui_vice_menu_iter_get_type(&iter, &type) &&
                (type == UI_MENU_TYPE_ITEM_ACTION ||
                 type == UI_MENU_TYPE_ITEM_CHECK)) {
            bool result = ui_vice_menu_iter_get_hotkey(&iter, &item_mask, &item_keysym);
#if 0
            debug_gtk3("iter keysym: %04x, iter mask: %04x (%s)", item_keysym, item_mask,
                    iter.menu_item->action_name);
#endif
            if (result) {
                if (mask == item_mask && keysym == item_keysym) {
                    return iter.menu_item;
                }
            }
        }
    } while (ui_vice_menu_iter_next(&iter));
    return NULL;
}


/** \brief  Set key and modifiers for \a item
 *
 * \param[in,out]   item        menu item
 * \param[in]       keyval_name GDK key name without 'GDK_KEY_' prefix
 * \param[in]       modifier    bitmask with GDK modifiers
 *
 * \return  TRUE if set
 */
gboolean ui_set_vice_menu_item_hotkey(ui_menu_item_t *item,
                                      const char *keyval_name,
                                      GdkModifierType modifier)
{
    guint keysym = gdk_keyval_from_name(keyval_name);

    if (keysym != GDK_KEY_VoidSymbol) {
        item->keysym = keysym;
        item->modifier = modifier;
        return TRUE;
    }
    return FALSE;
}


/** \brief  Set key and modifiers for a menu item by \a name
 *
 * \param[in,out]   name        menu item name
 * \param[in]       keyval_name GDK key name without 'GDK_KEY_' prefix
 * \param[in]       modifier    bitmask with GDK modifiers
 *
 * \return  TRUE if set
 */
gboolean ui_set_vice_menu_item_hotkey_by_name(const char *name,
                                              const char *keyval_name,
                                              GdkModifierType modifier)
{
    ui_menu_item_t *item = ui_get_vice_menu_item_by_name(name);

    if (item != NULL) {
        return ui_set_vice_menu_item_hotkey(item, keyval_name, modifier);
    }
    return FALSE;
}


/** \brief  Get (sub)menu item from the Gtk menu bar by \a name
 *
 * Try to look up a menu item for the action \a name.
 *
 * \param[in]   name    menu action name
 *
 * \return  GtkMenuItem reference or `NULL` when not found
 */
GtkWidget *ui_get_gtk_menu_item_by_name(const char *name)
{
    GList *node = gtk_container_get_children(GTK_CONTAINER(main_menu_bar));

#if 0
    debug_gtk3("Iterating menu main bar children.");
#endif
    while (node != NULL) {
        GtkWidget *item = node->data;
        if (item != NULL && GTK_IS_CONTAINER(item)) {
#if 0
            debug_gtk3("Item != NULL, getting submenu.");
#endif
            GtkWidget *submenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(item));
            if (submenu != NULL) {
                item = ui_get_gtk_submenu_item_by_name(submenu, name);
                if (item != NULL) {
                    return item;
                }
            }
        }
        node = node->next;
    }
    return NULL;
}


/** \brief  Get (sub)menu item from the Gtk menu bar by hotkey
 *
 * Try to look up a menu item via keyval and modifier(s)
 *
 * \param[in]   mask    modifiers
 * \param[in]   keyval  keyvalue
 *
 * \return  GtkMenuItem reference or `NULL` when not found
 */
GtkWidget *ui_get_gtk_menu_item_by_hotkey(GdkModifierType mask, guint keyval)
{
    GList *node = gtk_container_get_children(GTK_CONTAINER(main_menu_bar));

#if 0
    debug_gtk3("Iterating menu main bar children.");
#endif
    while (node != NULL) {
        GtkWidget *item = node->data;
        if (item != NULL && GTK_IS_CONTAINER(item)) {
#if 0
            debug_gtk3("Item != NULL, getting submenu.");
#endif
            GtkWidget *submenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(item));
            if (submenu != NULL) {

                item = ui_get_gtk_submenu_item_by_hotkey(submenu, mask, keyval);
                if (item != NULL) {
                    return item;
                }
            }
        }
        node = node->next;
    }
    return NULL;
}


/** \brief  Clear hotkeys of all the menu items
 *
 * Iterates all menu item declarations and sets `keysym` and `modifier` to 0.
 */
void ui_clear_vice_menu_item_hotkeys(void)
{
    ui_vice_menu_iter_t iter;

    ui_vice_menu_iter_init(&iter);
    do {
        /* just do 'em all, simpler and probably faster ;) */
        iter.menu_item->modifier = 0;
        iter.menu_item->keysym = 0;
    } while (ui_vice_menu_iter_next(&iter));
}
