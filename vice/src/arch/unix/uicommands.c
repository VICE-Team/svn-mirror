/*
 * uicommands.c - Implementation of common UI commands.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archdep.h"
#include "attach.h"
#include "autostart.h"
#include "drivecpu.h"
#include "event.h"
#include "fullscreenarch.h"
#include "imagecontents.h"
#include "interrupt.h"
#include "ioutil.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "monitor.h"
#include "resources.h"
#include "sound.h"
#include "tape.h"
#include "types.h"
#include "uicommands.h"
#include "uiedisk.h"
#include "uifliplist.h"
#include "uimenu.h"
#include "uisnapshot.h"
#include "uidrive.h"
#include "util.h"
#include "vsync.h"


static int selection_from_image = 0;

/* ------------------------------------------------------------------------- */
void ui_set_selected_file(int num)
{
    selection_from_image = num;
}

/* ------------------------------------------------------------------------- */

static char *read_disk_image_contents(const char *name, unsigned int unit)
{
#ifdef USE_GNOMEUI
    return image_contents_read_string(IMAGE_CONTENTS_DISK, name, unit,
                                      IMAGE_CONTENTS_STRING_PETSCII);
#else
    return image_contents_read_string(IMAGE_CONTENTS_DISK, name, unit,
                                      IMAGE_CONTENTS_STRING_ASCII);
#endif
}

static char *attach_disk_last_dir = NULL;

UI_CALLBACK(attach_disk)
{
    int unit = (int)UI_MENU_CB_PARAM;
    char *filename, *title;
    ui_button_t button;
    int attach_wp = 0;

    vsync_suspend_speed_eval();
    title = lib_msprintf(_("Attach Disk Image as unit #%d"), unit);
    filename = ui_select_file(title, read_disk_image_contents, unit,
                              unit == 8 ? True : False, attach_disk_last_dir,
                              "*.[gdxGDX]*", &button, True, &attach_wp);

    lib_free(title);
    if (attach_wp) {
        printf("Write protect attach requested.\n");
        resources_set_sprintf("AttachDevice%dReadonly",
                              (resource_value_t)attach_wp, unit);
    }

    switch (button) {
      case UI_BUTTON_OK:
        if (file_system_attach_disk(unit, filename) < 0)
            ui_error(_("Invalid Disk Image"));
        if (attach_disk_last_dir)
            lib_free(attach_disk_last_dir);
        util_fname_split(filename, &attach_disk_last_dir, NULL);
        break;
      case UI_BUTTON_AUTOSTART:
        if (autostart_disk(filename, NULL, selection_from_image,
            AUTOSTART_MODE_RUN) < 0)
            ui_error(_("Invalid Disk Image or Filename"));
        if (attach_disk_last_dir)
            lib_free(attach_disk_last_dir);
        util_fname_split(filename, &attach_disk_last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }

    if (filename != NULL)
        lib_free(filename);
}

static UI_CALLBACK(attach_empty_disk)
{
    int unit = (int)UI_MENU_CB_PARAM;
    /* Where does the 1024 come from?  */
    char filename[1024];

    vsync_suspend_speed_eval();

    /* The following code depends on a zeroed filename.  */
    memset(filename, 0, 1024);

    if (ui_empty_disk_dialog(filename) < 0)
        return;

    if (file_system_attach_disk(unit, filename) < 0)
        ui_error(_("Invalid Disk Image"));
}

UI_CALLBACK(detach_disk)
{
    int unit = (int)UI_MENU_CB_PARAM;

    vsync_suspend_speed_eval();
    file_system_detach_disk(unit);
}

/* ------------------------------------------------------------------------- */

static char *read_tape_image_contents(const char *name, unsigned int unit)
{
#ifdef USE_GNOMEUI
    return image_contents_read_string(IMAGE_CONTENTS_TAPE, name, unit,
                                      IMAGE_CONTENTS_STRING_PETSCII);
#else
    return image_contents_read_string(IMAGE_CONTENTS_TAPE, name, unit,
                                      IMAGE_CONTENTS_STRING_ASCII);
#endif
}

static char *attach_tape_last_dir = NULL;

static UI_CALLBACK(attach_tape)
{
    char *filename;
    ui_button_t button;

    vsync_suspend_speed_eval();

    filename = ui_select_file(_("Attach a tape image"),
                              read_tape_image_contents, 0,
                              True, attach_tape_last_dir, "*.[tT]*",
                              &button, True, NULL);

    switch (button) {
      case UI_BUTTON_OK:
        if (tape_image_attach(1, filename) < 0)
            ui_error(_("Invalid Tape Image"));
        if (attach_tape_last_dir)
            lib_free(attach_tape_last_dir);
        util_fname_split(filename, &attach_tape_last_dir, NULL);
        break;
      case UI_BUTTON_AUTOSTART:
        if (autostart_tape(filename, NULL, selection_from_image,
            AUTOSTART_MODE_RUN) < 0)
            ui_error(_("Invalid Tape Image"));
        if (attach_tape_last_dir)
            lib_free(attach_tape_last_dir);
        util_fname_split(filename, &attach_tape_last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    if (filename != NULL)
        lib_free(filename);
}

static UI_CALLBACK(detach_tape)
{
    tape_image_detach(1);
}

/* ------------------------------------------------------------------------- */

static char *read_disk_or_tape_image_contents(const char *fname,
                                              unsigned int unit)
{
    char *tmp;

    tmp = read_disk_image_contents(fname, unit);
    if (tmp)
        return tmp;
    return read_tape_image_contents(fname, unit);
}

static char *smart_attach_last_dir = NULL;

static UI_CALLBACK(smart_attach)
{
    char *filename;
    ui_button_t button;

    vsync_suspend_speed_eval();

    filename = ui_select_file(_("Smart-attach a file"),
                              read_disk_or_tape_image_contents, 0,
                              True, smart_attach_last_dir, NULL, &button,
                              True, NULL);

    switch (button) {
      case UI_BUTTON_OK:
        if (file_system_attach_disk(8, filename) < 0
            && tape_image_attach(1, filename) < 0) {
            ui_error(_("Unknown image type"));
        }
        if (smart_attach_last_dir)
            lib_free(smart_attach_last_dir);
        util_fname_split(filename, &smart_attach_last_dir, NULL);
        break;
      case UI_BUTTON_AUTOSTART:
        if (autostart_autodetect(filename, NULL, selection_from_image,
            AUTOSTART_MODE_RUN) < 0)
            ui_error(_("Unknown image type"));
        if (smart_attach_last_dir)
            lib_free(smart_attach_last_dir);
        util_fname_split(filename, &smart_attach_last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    if (filename != NULL)
        lib_free(filename);
}

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(change_working_directory)
{
    char *wd;
    wd = lib_malloc(MAXPATHLEN);

    ioutil_getcwd(wd, MAXPATHLEN);
    vsync_suspend_speed_eval();
    if (ui_input_string(_("VICE setting"),
                        _("Change current working directory"),
                        wd, MAXPATHLEN) == UI_BUTTON_OK) {
        if (ioutil_chdir(wd) < 0)
            ui_error(_("Directory not found"));
    }
    lib_free(wd);
}

static UI_CALLBACK(activate_monitor)
{
#ifdef USE_XF86_EXTENSIONS
    fullscreen_suspend(0);
#endif
    vsync_suspend_speed_eval();
    ui_dispatch_events();               /* popdown the menu */
    ui_autorepeat_on();

    if (!ui_emulation_is_paused())
        monitor_startup_trap();
    else
        monitor_startup();
}

static UI_CALLBACK(run_c1541)
{
#ifdef USE_XF86_EXTENSIONS
    fullscreen_suspend(0);
#endif
    vsync_suspend_speed_eval();
    sound_close();
    switch (system("xterm -sb -e c1541 &")) {
      case 127:
        ui_error(_("Couldn't run /bin/sh???"));
        break;
      case -1:
        ui_error(_("Couldn't run xterm"));
        break;
      case 0:
        break;
      default:
        ui_error(_("Unknown error while running c1541"));
    }
}

static UI_CALLBACK(drive_reset)
{
    vsync_suspend_speed_eval();
    drivecpu_trigger_reset((unsigned int)UI_MENU_CB_PARAM);
}

static UI_CALLBACK(reset)
{
    vsync_suspend_speed_eval();
    machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
}

static UI_CALLBACK(powerup_reset)
{
    vsync_suspend_speed_eval();
    machine_trigger_reset(MACHINE_RESET_MODE_HARD);
}

static UI_CALLBACK(browse_manual)
{
    const char *bcommand = NULL;

    resources_get_value("HTMLBrowserCommand", (void *)&bcommand);

    if (bcommand == NULL ||
        *bcommand == '\0') {
        ui_error(_("No HTML browser is defined."));
    } else {
        /* FIXME: Argh.  Ugly!  */
#define BROWSE_CMD_BUF_MAX 16384
        char buf[BROWSE_CMD_BUF_MAX];
        static const char manual_path[] = DOCDIR "/vice_toc.html";
        char *res_ptr;
        int manual_path_len, cmd_len;

#ifdef USE_XF86_EXTENSIONS
        fullscreen_suspend(0);
#endif
        cmd_len = strlen(bcommand);
        manual_path_len = strlen(manual_path);

        res_ptr = strstr(bcommand, "%s");
        if (res_ptr == NULL) {
            /* No substitution. */
            if (cmd_len + 2 > BROWSE_CMD_BUF_MAX - 1) {
                ui_error(_("Browser command too long."));
                return;
            }
            sprintf(buf, "%s &", bcommand);
        } else {
            char *tmp_ptr, *cmd_ptr;
            int offs;

            /* Replace each occurrence of "%s" with the path of the HTML
               manual. */

            cmd_len += manual_path_len - 2;
            cmd_len += 2;       /* Trailing " &". */
            if (cmd_len > BROWSE_CMD_BUF_MAX - 1) {
                ui_error(_("Browser command too long."));
                return;
            }

            offs = res_ptr - bcommand;
            memcpy(buf, bcommand, offs);
            strcpy(buf + offs, manual_path);
            cmd_ptr = buf + offs + manual_path_len;
            res_ptr += 2;

            while ((tmp_ptr = strstr(res_ptr, "%s")) != NULL) {
                cmd_len += manual_path_len - 2;
                if (cmd_len > BROWSE_CMD_BUF_MAX - 1) {
                    ui_error(_("Browser command too long."));
                    return;
                }
                offs = tmp_ptr - res_ptr;
                memcpy(cmd_ptr, res_ptr, offs);
                strcpy(cmd_ptr + offs, manual_path);
                cmd_ptr += manual_path_len + offs;
                res_ptr = tmp_ptr + 2;
            }

            sprintf(cmd_ptr, "%s &", res_ptr);
        }

        log_debug(_("Executing `%s'..."), buf);
        if (system(buf) != 0)
            ui_error(_("Cannot run HTML browser."));
    }
}

static UI_CALLBACK(do_exit)
{
    ui_exit();
}

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(toggle_pause)
{
    static int pause = 0;

    if (!CHECK_MENUS) {
        pause = !pause;
        ui_update_menus();
        ui_pause_emulation(pause);
        return;
    }

    ui_menu_set_tick(w, pause);
}

/* ------------------------------------------------------------------------- */

/* Snapshot commands.  */

static char *load_snapshot_last_dir = NULL;

static void load_snapshot_trap(WORD unused_addr, void *data)
{
    ui_button_t button;
    char *filename;

    vsync_suspend_speed_eval();

    if (data) {
        log_debug(_("Quickloading file %s."), (char *)data);
        filename = (char *)data;
    } else {
        filename = ui_select_file(_("Load snapshot"), NULL, 0, False,
                                  load_snapshot_last_dir,
                                  "*.vsf", &button, False, NULL);
        if (button != UI_BUTTON_OK) {
            if (filename)
                lib_free(filename);
            return;
        }
    }
    if (load_snapshot_last_dir)
        lib_free(load_snapshot_last_dir);
    util_fname_split(filename, &load_snapshot_last_dir, NULL);

    if (machine_read_snapshot(filename, 0) < 0)
        ui_error(_("Cannot load snapshot file\n`%s'"), filename);
    ui_update_menus();

    if (filename != NULL)
        lib_free(filename);
}

static UI_CALLBACK(load_snapshot)
{
    if (!ui_emulation_is_paused())
        interrupt_maincpu_trigger_trap(load_snapshot_trap, (void *)0);
    else
        load_snapshot_trap(0, 0);
}

static UI_CALLBACK(load_quicksnap)
{
    char *fname = util_concat(archdep_home_path(), "/", VICEUSERDIR, "/",
        machine_name, ".vsf", NULL);

    if (!ui_emulation_is_paused())
        interrupt_maincpu_trigger_trap(load_snapshot_trap, (void *)fname);
    else
        load_snapshot_trap(0, (void *)fname);
}

static void save_snapshot_trap(WORD unused_addr, void *data)
{
    if (data) {
        /* quick snapshot, save ROMs & disks (??) */
        log_debug(_("Quicksaving file %s."), (char *)data);
        if (machine_write_snapshot(data, 1, 1, 0) < 0) {
            ui_error(_("Cannot write snapshot file\n`%s'\n"), data);
        }
        lib_free(data);
    } else {
        ui_snapshot_dialog();
    }

    vsync_suspend_speed_eval();
}

static UI_CALLBACK(save_snapshot)
{
    interrupt_maincpu_trigger_trap(save_snapshot_trap, (void *)0);
}

static UI_CALLBACK(save_quicksnap)
{
    char *fname = util_concat(archdep_home_path(), "/", VICEUSERDIR, "/",
        machine_name, ".vsf", NULL);

    interrupt_maincpu_trigger_trap(save_snapshot_trap, (void *)fname);
}

static UI_CALLBACK(events_select_dir)
{
    char *wd;
    int i, is_dir;
    wd = lib_malloc(MAXPATHLEN);

    ioutil_getcwd(wd, MAXPATHLEN);
    vsync_suspend_speed_eval();
    if (ui_input_string(_("VICE setting"),
                        _("Select history directory"),
                        wd, MAXPATHLEN) == UI_BUTTON_OK) {
        ioutil_stat(wd, &i, &is_dir);
	if (!is_dir)
            ui_error(_("Directory not found"));
	else
	    resources_set_value("EventSnapshotDir", (resource_value_t) wd);
    }
    lib_free(wd);
    
}

static UI_CALLBACK(record_events_start)
{
    event_record_start();
}

static UI_CALLBACK(record_events_stop)
{
    event_record_stop();
}

static UI_CALLBACK(playback_events_start)
{
    event_playback_start();
}

static UI_CALLBACK(playback_events_stop)
{
    event_playback_stop();
}

static UI_CALLBACK(events_set_ms)
{
    event_record_set_milestone();
}

static UI_CALLBACK(events_return_ms)
{
    event_record_reset_milestone();
}

static UI_CALLBACK(sound_record)
{
    ui_button_t button;
    char *s;
    const char *devicename;

    vsync_suspend_speed_eval();

    resources_get_value("SoundRecordDeviceName",(void *) &devicename);
    if (devicename && !strcmp(devicename,"wav")) {
        /* the recording is active; stop it  */
        resources_set_value("SoundRecordDeviceName", "");
/*        ui_display_statustext("");*/
    } else {
        s = ui_select_file(_("Record sound to file"), NULL, 0, False, NULL,
                              "*.wav", &button, False, NULL);
        if (button == UI_BUTTON_OK && s != NULL) {
            util_add_extension(&s, "wav");
            resources_set_value("SoundRecordDeviceArg", s);
            resources_set_value("SoundRecordDeviceName", "wav");
            resources_set_value("Sound", (resource_value_t)1);
            lib_free(s);
/*            ui_display_statustext("Recording wav...");*/
        }
    }
}

#ifdef USE_GNOMEUI
UI_MENU_DEFINE_TOGGLE(AttachDevice8Readonly)
UI_MENU_DEFINE_TOGGLE(AttachDevice9Readonly)
#endif

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t attach_empty_disk_image_submenu[] = {
    { N_("Unit #8..."),
      (ui_callback_t)attach_empty_disk, (ui_callback_data_t)8, NULL, },
    { N_("Unit #9..."),
      (ui_callback_t)attach_empty_disk, (ui_callback_data_t)9, NULL, },
    { N_("Unit #10..."),
      (ui_callback_t)attach_empty_disk, (ui_callback_data_t)10, NULL, },
    { N_("Unit #11..."),
      (ui_callback_t)attach_empty_disk, (ui_callback_data_t)11, NULL, },
    { NULL }
};

static ui_menu_entry_t attach_disk_image_submenu[] = {
    { N_("Unit #8..."),
      (ui_callback_t)attach_disk, (ui_callback_data_t)8, NULL,
      XK_8, UI_HOTMOD_META },
    { N_("Unit #9..."),
      (ui_callback_t)attach_disk, (ui_callback_data_t)9, NULL,
      XK_9, UI_HOTMOD_META },
    { N_("Unit #10..."),
      (ui_callback_t)attach_disk, (ui_callback_data_t)10, NULL,
      XK_0, UI_HOTMOD_META },
    { N_("Unit #11..."),
      (ui_callback_t)attach_disk, (ui_callback_data_t)11, NULL,
      XK_1, UI_HOTMOD_META },
    { NULL }
};

static ui_menu_entry_t detach_disk_image_submenu[] = {
    { N_("Unit #8"),
      (ui_callback_t)detach_disk, (ui_callback_data_t)8, NULL },
    { N_("Unit #9"),
      (ui_callback_t)detach_disk, (ui_callback_data_t)9, NULL },
    { N_("Unit #10"),
      (ui_callback_t)detach_disk, (ui_callback_data_t)10, NULL },
    { N_("Unit #11"),
      (ui_callback_t)detach_disk, (ui_callback_data_t)11, NULL },
    { "--" },
    { N_("All"),
      (ui_callback_t)detach_disk, (ui_callback_data_t)-1, NULL },
    { NULL }
};

static ui_menu_entry_t reset_submenu[] = {
    { N_("Soft"),
      (ui_callback_t)reset, NULL, NULL,
      XK_F9, UI_HOTMOD_META },
    { N_("Hard"),
      (ui_callback_t)powerup_reset, NULL, NULL,
      XK_F12, UI_HOTMOD_META },
    { "--" },
    { N_("Unit #8"),
      (ui_callback_t)drive_reset, (ui_callback_data_t)0, NULL },
    { N_("Unit #9"),
      (ui_callback_t)drive_reset, (ui_callback_data_t)1, NULL },
    { N_("Unit #10"),
      (ui_callback_t)drive_reset, (ui_callback_data_t)2, NULL },
    { N_("Unit #11"),
      (ui_callback_t)drive_reset, (ui_callback_data_t)3, NULL },
    { NULL }
};

ui_menu_entry_t ui_disk_commands_menu[] = {
    { N_("Attach a disk image"),
      NULL, NULL, attach_disk_image_submenu },
    { N_("Create and attach an empty disk"),
      NULL, NULL, attach_empty_disk_image_submenu },
    { N_("Detach disk image"),
      NULL, NULL, detach_disk_image_submenu },
    { N_("Fliplist"),
      NULL, NULL, fliplist_submenu },
    { NULL }
};

ui_menu_entry_t ui_tape_commands_menu[] = {
    { N_("Attach a tape image..."),
      (ui_callback_t)attach_tape, NULL, NULL,
      XK_t, UI_HOTMOD_META},
    { N_("Detach tape image"),
      (ui_callback_t)detach_tape, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_smart_attach_commands_menu[] = {
    { N_("Smart-attach disk/tape..."),
      (ui_callback_t)smart_attach, NULL, NULL,
      XK_a, UI_HOTMOD_META },
    { NULL }
};

ui_menu_entry_t ui_directory_commands_menu[] = {
    { N_("Change working directory..."),
      (ui_callback_t)change_working_directory, NULL, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(EventStartMode)

static ui_menu_entry_t set_event_start_mode_submenu[] = {
    { N_("*Save new snapshot"), (ui_callback_t)radio_EventStartMode,
      (ui_callback_data_t)EVENT_START_MODE_FILE_SAVE, NULL },
    { N_("*Load existing snapshot"), (ui_callback_t)radio_EventStartMode,
      (ui_callback_data_t)EVENT_START_MODE_FILE_LOAD, NULL },
    { N_("*Start with reset"), (ui_callback_t)radio_EventStartMode,
      (ui_callback_data_t)EVENT_START_MODE_RESET, NULL },
    { NULL }
};

ui_menu_entry_t ui_snapshot_commands_submenu[] = {
    { N_("Load snapshot..."),
      (ui_callback_t)load_snapshot, NULL, NULL,
      XK_l, UI_HOTMOD_META },
    { N_("Save snapshot..."),
      (ui_callback_t)save_snapshot, NULL, NULL,
      XK_s, UI_HOTMOD_META },
    { "--" },
    { N_("Quickload snapshot"),
      (ui_callback_t)load_quicksnap, NULL, NULL,
      XK_F10, UI_HOTMOD_META },
    { N_("Quicksave snapshot"),
      (ui_callback_t)save_quicksnap, NULL, NULL,
      XK_F11, UI_HOTMOD_META },
    { "--" },
    { N_("Select history directory"),
      (ui_callback_t)events_select_dir, NULL, NULL },
    { N_("Start recording events"),
      (ui_callback_t)record_events_start, NULL, NULL },
    { N_("Stop recording events"),
      (ui_callback_t)record_events_stop, NULL, NULL },
    { N_("Start playing back events"),
      (ui_callback_t)playback_events_start, NULL, NULL },
    { N_("Stop playing back events"),
      (ui_callback_t)playback_events_stop, NULL, NULL },
    { N_("Set recording milestone"),
      (ui_callback_t)events_set_ms, NULL, NULL, XK_e, UI_HOTMOD_META },
    { N_("Return to milestone"),
      (ui_callback_t)events_return_ms, NULL, NULL, XK_u, UI_HOTMOD_META },
    { "--" },
    { N_("Recording start mode"),
      NULL, NULL, set_event_start_mode_submenu },
    { NULL }
};

ui_menu_entry_t ui_snapshot_commands_menu[] = {
    { N_("Snapshot commands"),
      NULL,  NULL, ui_snapshot_commands_submenu },
    { NULL }
};

ui_menu_entry_t ui_sound_record_commands_menu[] = {
    { N_("Sound record..."),
      (ui_callback_t)sound_record, NULL, NULL,
      XK_r, UI_HOTMOD_META },
    { NULL }
};

ui_menu_entry_t ui_tool_commands_menu[] = {
    { N_("Activate monitor"),
      (ui_callback_t)activate_monitor, NULL, NULL,
      XK_h, UI_HOTMOD_META },
    { N_("Run C1541"),
      (ui_callback_t)run_c1541, NULL, NULL },
    { NULL }
};

extern ui_callback_t about;

ui_menu_entry_t ui_help_commands_menu[] = {
    { N_("Browse manuals"),
      (ui_callback_t)browse_manual, NULL, NULL },
    { N_("About VICE..."),
      (ui_callback_t)ui_about, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_run_commands_menu[] = {
    { N_("Reset"),
      NULL, NULL, reset_submenu },
    { N_("*Pause"),
      (ui_callback_t)toggle_pause, NULL, NULL,
      XK_p, UI_HOTMOD_META },
    { NULL }
};

ui_menu_entry_t ui_exit_commands_menu[] = {
    { N_("Exit emulator"),
      (ui_callback_t)do_exit, NULL, NULL,
      XK_q, UI_HOTMOD_META },
    { NULL }
};

void uicommands_shutdown(void)
{
    lib_free(attach_disk_last_dir);
    lib_free(attach_tape_last_dir);
    lib_free(smart_attach_last_dir);
    lib_free(load_snapshot_last_dir);
}

