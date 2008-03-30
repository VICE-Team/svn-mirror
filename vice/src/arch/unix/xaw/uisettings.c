/*
 * uisettings.c - Implementation of common UI settings.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Andreas Boose (boose@unixserv.rz.fh-hannover.de)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <X11/Intrinsic.h>

#include "uisettings.h"

#include "fsdevice.h"
#include "kbd.h"
#include "resources.h"
#include "sound.h"
#include "drive.h"
#include "vsync.h"
#include "utils.h"
#include "iecdrive.h"
#include "romset.h"
#include "mem.h"

/* ------------------------------------------------------------------------- */

/* Big kludge to get the ticks right in the refresh rate submenu.  This only
   works if the callback for the "custom" setting is the last one to be
   called, and the "Auto" one is the first one.  */
static int have_custom_refresh_rate;

static UI_CALLBACK(set_refresh_rate)
{
    int current_refresh_rate;

    resources_get_value("RefreshRate",
                        (resource_value_t *) &current_refresh_rate);
    if (!call_data) {
	if (current_refresh_rate != (int) client_data) {
	    resources_set_value("RefreshRate", (resource_value_t) client_data);
	    ui_update_menus();
	}
    } else {
        if ((int)client_data == 0)
            have_custom_refresh_rate = 1;
        if ((int)client_data == current_refresh_rate) {
            ui_menu_set_tick(w, 1);
            have_custom_refresh_rate = 0;
        } else {
            ui_menu_set_tick(w, 0);
        }
	if (client_data == 0) {
            int speed;

            resources_get_value("Speed", (resource_value_t *) &speed);
            if (speed == 0) {
                /* Cannot enable the `automatic' setting if a speed limit is
                   not specified. */
                ui_menu_set_sensitive(w, False);
            } else {
                ui_menu_set_sensitive(w, True);
            }
        }
    }
}

static UI_CALLBACK(set_custom_refresh_rate)
{
    static char input_string[32];
    char msg_string[256];
    ui_button_t button;
    int i;
    int current_refresh_rate;

    resources_get_value("RefreshRate",
                        (resource_value_t *) &current_refresh_rate);

    if (!*input_string)
	sprintf(input_string, "%d", current_refresh_rate);

    if (call_data) {
        if (have_custom_refresh_rate)
            ui_menu_set_tick(w, 1);
        else
            ui_menu_set_tick(w, 0);
        have_custom_refresh_rate = 0;
    } else {
        int current_speed;

	suspend_speed_eval();
	sprintf(msg_string, "Enter refresh rate");
	button = ui_input_string("Refresh rate", msg_string, input_string, 32);
	if (button == UI_BUTTON_OK) {
	    i = atoi(input_string);
            resources_get_value("Speed", (resource_value_t *) &current_speed);
	    if (!(current_speed <= 0 && i <= 0) && i >= 0
		&& current_refresh_rate != i) {
                resources_set_value("RefreshRate", (resource_value_t) i);
		ui_update_menus();
	    }
	}
    }
}

/* ------------------------------------------------------------------------- */

/* Big kludge to get the ticks right in the maximum speed submenu.  This only
   works if the callback for the "custom" setting is the last one to be
   called, and the "100%" one is the first one.  */
static int have_custom_maximum_speed;

static UI_CALLBACK(set_maximum_speed)
{
    int current_speed;

    resources_get_value("Speed", (resource_value_t *) &current_speed);

    if (!call_data) {
	if (current_speed != (int)client_data) {
            resources_set_value("Speed", (resource_value_t) client_data);
	    ui_update_menus();
	}
    } else {
        if ((int) client_data == 100)
            have_custom_maximum_speed = 1;
        if (current_speed == (int) client_data) {
            ui_menu_set_tick(w, 1);
            have_custom_maximum_speed = 0;
        } else {
            ui_menu_set_tick(w, 0);
        }
	if (client_data == 0) {
            int current_refresh_rate;

            resources_get_value("RefreshRate",
                                (resource_value_t *) &current_refresh_rate);
	    ui_menu_set_sensitive(w, current_refresh_rate != 0);
	}
    }
}

static UI_CALLBACK(set_custom_maximum_speed)
{
    static char input_string[32];
    char msg_string[256];
    ui_button_t button;
    int i;
    int current_speed;

    resources_get_value("Speed", (resource_value_t *) &current_speed);
    if (!*input_string)
	sprintf(input_string, "%d", current_speed);

    if (call_data) {
        if (have_custom_maximum_speed)
            ui_menu_set_tick(w, 1);
        else
            ui_menu_set_tick(w, 0);
        have_custom_maximum_speed = 0;
    } else {
	suspend_speed_eval();
	sprintf(msg_string, "Enter speed");
	button = ui_input_string("Maximum run speed", msg_string, input_string,
				 32);
	if (button == UI_BUTTON_OK) {
            int current_refresh_rate;

            resources_get_value("RefreshRate",
                                (resource_value_t *) &current_refresh_rate);
	    i = atoi(input_string);
	    if (!(current_refresh_rate <= 0 && i <= 0) && i >= 0
		&& current_speed != i) {
                resources_set_value("Speed", (resource_value_t) i);
		ui_update_menus();
	    } else
		ui_error("Invalid speed value");
	}
    }
}

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(save_resources)
{
    suspend_speed_eval();
    if (resources_save(NULL) < 0)
	ui_error("Cannot save settings.");
    else {
	if (w != NULL)
	    ui_message("Settings saved successfully.");
    }
    ui_update_menus();
}

static UI_CALLBACK(load_resources)
{
    int r;

    suspend_speed_eval();
    r = resources_load(NULL);

    if (r < 0) {
        if (r == RESERR_FILE_INVALID)
            ui_error("Cannot load settings:\nresource file not valid.");
        else
            ui_error("Cannot load settings:\nresource file not found.");
    }
#if 0
    else if (w != NULL)
            ui_message("Settings loaded.");
#endif

    ui_update_menus();
}

static UI_CALLBACK(set_default_resources)
{
    suspend_speed_eval();
    resources_set_defaults();
    ui_update_menus();
}

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_TOGGLE(VideoCache)

UI_MENU_DEFINE_TOGGLE(UseXSync)

UI_MENU_DEFINE_TOGGLE(SaveResourcesOnExit)

UI_MENU_DEFINE_TOGGLE(WarpMode)

UI_MENU_DEFINE_TOGGLE(DoubleSize)

UI_MENU_DEFINE_TOGGLE(DoubleScan)


/* ------------------------------------------------------------------------- */

/* this is modelled after the toggle_* calls */

static UI_CALLBACK(set_keymap_type)
{
     int kindex, newindex = (int) client_data;

     if (resources_get_value("KeymapIndex", (resource_value_t*) &kindex) < 0)
         return;

     if (!call_data) {
	if ((kindex & 1) != newindex) {
            resources_set_value("KeymapIndex", (resource_value_t)
                                ((kindex & ~1) + newindex));
            ui_update_menus();
        }
     } else {
        ui_menu_set_tick(w, (kindex & 1) == newindex);
     }
}

static ui_menu_entry_t keyboard_maptype_submenu[] = {
    { "*Symbolic mapping", (ui_callback_t) set_keymap_type,
	(ui_callback_data_t) 0, NULL },
    { "*Positional mapping (US)", (ui_callback_t) set_keymap_type,
	(ui_callback_data_t) 1, NULL },
    { NULL }
};

static UI_CALLBACK(select_user_keymap)
{
    char *filename;
    const char *resname;
    ui_button_t button;
    int kindex;

    resources_get_value("KeymapIndex", (resource_value_t)&kindex);
    kindex = (kindex & ~1) + (int)client_data;
    resname = keymap_res_name_list[kindex];

    suspend_speed_eval();
    filename = ui_select_file("Read Keymap File", NULL, False, NULL,
							"*.vkm", &button);

    switch (button) {
      case UI_BUTTON_OK:
	resources_set_value(resname, (resource_value_t)filename);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
}

static UI_CALLBACK(dump_keymap)
{
    PATH_VAR(wd);
    int path_max = GET_PATH_MAX;

    getcwd(wd, path_max);
    suspend_speed_eval();
    if (ui_input_string("VICE setting", "Write to Keymap File:",
			wd, path_max) != UI_BUTTON_OK)
	return;
    else if (kbd_dump_keymap(wd) < 0)
	ui_error(strerror(errno));
}

static ui_menu_entry_t keyboard_settings_submenu[] = {
    { "Keyboard mapping type",
      NULL, NULL, keyboard_maptype_submenu },
    { "--" },
    { "Set symbolic keymap file", (ui_callback_t)select_user_keymap,
		(ui_callback_data_t)0, NULL},
    { "Set positional keymap file", (ui_callback_t)select_user_keymap,
		(ui_callback_data_t)1, NULL},
    { "--" },
    { "Dump keymap to file",
      (ui_callback_t) dump_keymap, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_CALLBACK(ui_load_palette)
{
    char *filename;
    char title[1024];
    ui_button_t button;

    suspend_speed_eval();
    sprintf(title, "Load custom palette");
    filename = ui_select_file(title, NULL, False, NULL, "*.vpl", &button);

    switch (button) {
      case UI_BUTTON_OK:
        if (resources_set_value("PaletteFile", 
		(resource_value_t) filename) < 0)
            ui_error("Could not load palette file\n'%s'",filename);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    ui_update_menus();
}

/* ------------------------------------------------------------------------- */

/* ROM set stuff */

UI_CALLBACK(ui_set_romset)
{
    romset_load(client_data);
    ui_update_menus();
}

UI_CALLBACK(ui_load_romset)
{
    char *filename;
    char title[1024];
    ui_button_t button;

    suspend_speed_eval();
    sprintf(title, "Load custom ROM set definition");
    filename = ui_select_file(title, NULL, False, NULL, "*.vrs", &button);

    switch (button) {
      case UI_BUTTON_OK:
        if (romset_load(filename) < 0)
            ui_error("Could not load ROM set file\n'%s'",filename);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    ui_update_menus();
}

UI_CALLBACK(ui_dump_romset)
{
    char title[1024];

    suspend_speed_eval();
    sprintf(title, "File to dump ROM set definition to");
    {
        char *new_value;
        int len = 512;

        new_value = alloca(len + 1);
        strcpy(new_value, "");

        if (ui_input_string(title, "ROM set file:", new_value, len)
                != UI_BUTTON_OK)
            return;

        romset_dump(new_value, mem_romset_resources_list);
    }
}

UI_CALLBACK(ui_load_rom_file)
{
    char *filename;
    char title[1024];
    ui_button_t button;

    suspend_speed_eval();
    sprintf(title, "Load ROM file");
    filename = ui_select_file(title, NULL, False, NULL, "*", &button);

    switch (button) {
      case UI_BUTTON_OK:
        if (resources_set_value(client_data, (resource_value_t)filename) < 0)
            ui_error("Could not load ROM file\n'%s'",filename);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    ui_update_menus();
}

UI_CALLBACK(ui_unload_rom_file)
{
    resources_set_value((char*)client_data, (resource_value_t) NULL);
}

/* ------------------------------------------------------------------------- */

/* RS232 stuff */


UI_MENU_DEFINE_RADIO(RsUserDev)

ui_menu_entry_t rsuser_device_submenu[] = {
    { "*Serial 1",
      (ui_callback_t) radio_RsUserDev, (ui_callback_data_t) 0, NULL },
    { "*Serial 2",
      (ui_callback_t) radio_RsUserDev, (ui_callback_data_t) 1, NULL },
    { "*Dump to file",
      (ui_callback_t) radio_RsUserDev, (ui_callback_data_t) 2, NULL },
    { "*Exec process",
      (ui_callback_t) radio_RsUserDev, (ui_callback_data_t) 3, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(Acia1Dev)

ui_menu_entry_t acia1_device_submenu[] = {
    { "*Serial 1",
      (ui_callback_t) radio_Acia1Dev, (ui_callback_data_t) 0, NULL },
    { "*Serial 2",
      (ui_callback_t) radio_Acia1Dev, (ui_callback_data_t) 1, NULL },
    { "*Dump to file",
      (ui_callback_t) radio_Acia1Dev, (ui_callback_data_t) 2, NULL },
    { "*Exec process",
      (ui_callback_t) radio_Acia1Dev, (ui_callback_data_t) 3, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(Acia1Irq)

ui_menu_entry_t acia1_irq_submenu[] = {
    { "*No IRQ/NMI",
      (ui_callback_t) radio_Acia1Irq, (ui_callback_data_t) 0, NULL },
    { "*IRQ",
      (ui_callback_t) radio_Acia1Irq, (ui_callback_data_t) 1, NULL },
    { "*NMI",
      (ui_callback_t) radio_Acia1Irq, (ui_callback_data_t) 2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(RsDevice1Baud)

ui_menu_entry_t ser1_baud_submenu[] = {
  { "*300",
      (ui_callback_t) radio_RsDevice1Baud, (ui_callback_data_t)   300, NULL },
  { "*1200",
      (ui_callback_t) radio_RsDevice1Baud, (ui_callback_data_t)  1200, NULL },
  { "*2400",
      (ui_callback_t) radio_RsDevice1Baud, (ui_callback_data_t)  2400, NULL },
  { "*9600",
      (ui_callback_t) radio_RsDevice1Baud, (ui_callback_data_t)  9600, NULL },
  { "*19200",
      (ui_callback_t) radio_RsDevice1Baud, (ui_callback_data_t) 19200, NULL },
  { NULL }
};

UI_MENU_DEFINE_RADIO(RsDevice2Baud)

ui_menu_entry_t ser2_baud_submenu[] = {
  { "*300",
      (ui_callback_t) radio_RsDevice2Baud, (ui_callback_data_t)   300, NULL },
  { "*1200",
      (ui_callback_t) radio_RsDevice2Baud, (ui_callback_data_t)  1200, NULL },
  { "*2400",
      (ui_callback_t) radio_RsDevice2Baud, (ui_callback_data_t)  2400, NULL },
  { "*9600",
      (ui_callback_t) radio_RsDevice2Baud, (ui_callback_data_t)  9600, NULL },
  { "*19200",
      (ui_callback_t) radio_RsDevice2Baud, (ui_callback_data_t) 19200, NULL },
  { NULL }
};

UI_CALLBACK(set_rs232_device_file)
{
    char *resource = (char*) client_data;
    char *filename;
    ui_button_t button;

    suspend_speed_eval();

    filename = ui_select_file("Select RS232 device file",
                              NULL, False, "/dev", "ttyS*", &button);
    switch (button) {
      case UI_BUTTON_OK:
        resources_set_value(resource, (resource_value_t) filename);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
}

UI_CALLBACK(set_rs232_exec_file)
{
    char *resname = (char*) client_data;
    char title[1024];

    suspend_speed_eval();
    sprintf(title, "Command to execute for RS232 (preceed with '|')");
    {
        char *value;
        char *new_value;
        int len;

        resources_get_value(resname, (resource_value_t *) &value);
        len = strlen(value) * 2;
        if (len < 255)
            len = 255;
        new_value = alloca(len + 1);
        strcpy(new_value, value);

        if (ui_input_string(title, "Command:", new_value, len) != UI_BUTTON_OK)
            return;

        resources_set_value(resname, (resource_value_t) new_value);
    }
}

UI_CALLBACK(set_rs232_dump_file)
{
    char *resname = (char*) client_data;
    char title[1024];

    suspend_speed_eval();
    sprintf(title, "File to dump RS232 to");
    {
        char *value;
        char *new_value;
        int len;

        resources_get_value(resname, (resource_value_t *) &value);
        len = strlen(value) * 2;
        if (len < 255)
            len = 255;
        new_value = alloca(len + 1);
        strcpy(new_value, value);

        if (ui_input_string(title, "Command:", new_value, len) != UI_BUTTON_OK)
            return;

        resources_set_value(resname, (resource_value_t) new_value);
    }
}


UI_MENU_DEFINE_TOGGLE(AciaDE)
UI_MENU_DEFINE_RADIO(RsUser)

ui_menu_entry_t rs232_submenu[] = {
    { "*ACIA $DExx RS232 interface emulation",
      (ui_callback_t) toggle_AciaDE, NULL, NULL },
    { "ACIA $DExx device",
      NULL, NULL, acia1_device_submenu },
    { "ACIA $DExx Interrupt",
      NULL, NULL, acia1_irq_submenu },
    { "--" },
    { "*No Userport RS232 emulation",
      (ui_callback_t) radio_RsUser, (ui_callback_data_t) 0, NULL },
    { "*Userport 300 baud RS232 emulation",
      (ui_callback_t) radio_RsUser, (ui_callback_data_t) 300, NULL },
    { "*Userport 1200 baud RS232 emulation",
      (ui_callback_t) radio_RsUser, (ui_callback_data_t) 1200, NULL },
    { "*CIA 9600 baud RS232 emulation",
      (ui_callback_t) radio_RsUser, (ui_callback_data_t) 9600, NULL },
    { "Userport RS232 device",
      NULL, NULL, rsuser_device_submenu },
    { "--" },
    { "Serial 1 device...", (ui_callback_t) set_rs232_device_file,
      (ui_callback_data_t) "RsDevice1", NULL },
    { "Serial 1 baudrate",
      NULL, NULL, ser1_baud_submenu },
    { "--" },
    { "Serial 2 device...", (ui_callback_t) set_rs232_device_file,
      (ui_callback_data_t) "RsDevice2", NULL },
    { "Serial 2 baudrate",
      NULL, NULL, ser2_baud_submenu },
    { "--" },
    { "Dump filename...", (ui_callback_t) set_rs232_dump_file,
      (ui_callback_data_t) "RsDevice3", NULL },
    { "--" },
    { "Program name to exec...", (ui_callback_t) set_rs232_exec_file,
      (ui_callback_data_t) "RsDevice4", NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

/* Drive emulation support items.  */

UI_MENU_DEFINE_TOGGLE(DriveTrueEmulation)
UI_MENU_DEFINE_TOGGLE(Drive8ParallelCable)
UI_MENU_DEFINE_TOGGLE(Drive9ParallelCable)

static UI_CALLBACK(set_custom_drive_sync_factor)
{
    static char input_string[256];
    char msg_string[256];
    ui_button_t button;
    int sync_factor;

    resources_get_value("DriveSyncFactor",
                        (resource_value_t *) &sync_factor);
    if (!*input_string)
	sprintf(input_string, "%d", sync_factor);

    if (call_data) {
	if (sync_factor != DRIVE_SYNC_PAL
            && sync_factor != DRIVE_SYNC_NTSC)
	    ui_menu_set_tick(w, 1);
	else
	    ui_menu_set_tick(w, 0);
    } else {
	suspend_speed_eval();
	sprintf(msg_string, "Enter factor (PAL %d, NTSC %d)",
		DRIVE_SYNC_PAL, DRIVE_SYNC_NTSC);
	button = ui_input_string("1541 Sync Factor", msg_string, input_string,
				 256);
	if (button == UI_BUTTON_OK) {
	    int v;

	    v = atoi(input_string);
	    if (v != sync_factor) {
                resources_set_value("DriveSyncFactor",
                                    (resource_value_t) v);
		ui_update_menus();
	    }
	}
    }
}

UI_MENU_DEFINE_RADIO(Drive8ExtendImagePolicy)
UI_MENU_DEFINE_RADIO(Drive9ExtendImagePolicy)
UI_MENU_DEFINE_RADIO(DriveSyncFactor)
UI_MENU_DEFINE_RADIO(Drive8IdleMethod)
UI_MENU_DEFINE_RADIO(Drive9IdleMethod)

/* ------------------------------------------------------------------------- */

/* Peripheral settings.  */

UI_MENU_DEFINE_TOGGLE(NoTraps)
UI_MENU_DEFINE_TOGGLE(FileSystemDevice8)
UI_MENU_DEFINE_TOGGLE(FileSystemDevice9)
UI_MENU_DEFINE_TOGGLE(FileSystemDevice10)
UI_MENU_DEFINE_TOGGLE(FileSystemDevice11)
UI_MENU_DEFINE_TOGGLE(FSDevice8ConvertP00)
UI_MENU_DEFINE_TOGGLE(FSDevice9ConvertP00)
UI_MENU_DEFINE_TOGGLE(FSDevice10ConvertP00)
UI_MENU_DEFINE_TOGGLE(FSDevice11ConvertP00)
UI_MENU_DEFINE_TOGGLE(FSDevice8SaveP00)
UI_MENU_DEFINE_TOGGLE(FSDevice9SaveP00)
UI_MENU_DEFINE_TOGGLE(FSDevice10SaveP00)
UI_MENU_DEFINE_TOGGLE(FSDevice11SaveP00)
UI_MENU_DEFINE_TOGGLE(FSDevice8HideCBMFiles)
UI_MENU_DEFINE_TOGGLE(FSDevice9HideCBMFiles)
UI_MENU_DEFINE_TOGGLE(FSDevice10HideCBMFiles)
UI_MENU_DEFINE_TOGGLE(FSDevice11HideCBMFiles)

static UI_CALLBACK(set_fsdevice_directory)
{
    int unit = (int)client_data;
    char title[1024];

    suspend_speed_eval();
    sprintf(title, "Attach file system directory to device #%d", unit);

    /* FIXME: We need a real directory browser here.  */
    {
        char resname[256];
        char *value;
        char *new_value;
        int len;

        sprintf(resname, "FSDevice%dDir", unit);

        resources_get_value(resname, (resource_value_t *) &value);
        len = strlen(value) * 2;
        if (len < 255)
            len = 255;
        new_value = alloca(len + 1);
        strcpy(new_value, value);

        if (ui_input_string(title, "Path:", new_value, len) != UI_BUTTON_OK)
            return;

        resources_set_value(resname, (resource_value_t) new_value);
    }
}

/* ------------------------------------------------------------------------- */

/* Sound support. */

UI_MENU_DEFINE_TOGGLE(Sound)
UI_MENU_DEFINE_RADIO(SoundSpeedAdjustment)
UI_MENU_DEFINE_RADIO(SoundSampleRate)
UI_MENU_DEFINE_RADIO(SoundBufferSize)
UI_MENU_DEFINE_RADIO(SoundSuspendTime)
UI_MENU_DEFINE_RADIO(SoundOversample)

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t set_refresh_rate_submenu[] = {
    { "*Auto",
      (ui_callback_t) set_refresh_rate, (ui_callback_data_t) 0, NULL },
    { "*1/1",
      (ui_callback_t) set_refresh_rate, (ui_callback_data_t) 1, NULL },
    { "*1/2",
      (ui_callback_t) set_refresh_rate, (ui_callback_data_t) 2, NULL },
    { "*1/3",
      (ui_callback_t) set_refresh_rate, (ui_callback_data_t) 3, NULL },
    { "*1/4",
      (ui_callback_t) set_refresh_rate, (ui_callback_data_t) 4, NULL },
    { "*1/5",
      (ui_callback_t) set_refresh_rate, (ui_callback_data_t) 5, NULL },
    { "*1/6",
      (ui_callback_t) set_refresh_rate, (ui_callback_data_t) 6, NULL },
    { "*1/7",
      (ui_callback_t) set_refresh_rate, (ui_callback_data_t) 7, NULL },
    { "*1/8",
      (ui_callback_t) set_refresh_rate, (ui_callback_data_t) 8, NULL },
    { "*1/9",
      (ui_callback_t) set_refresh_rate, (ui_callback_data_t) 9, NULL },
    { "*1/10",
      (ui_callback_t) set_refresh_rate, (ui_callback_data_t) 10, NULL },
    { "--" },
    { "*Custom...",
      (ui_callback_t) set_custom_refresh_rate, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t set_maximum_speed_submenu[] = {
    { "*200%",
      (ui_callback_t) set_maximum_speed, (ui_callback_data_t) 200, NULL },
    { "*100%",
      (ui_callback_t) set_maximum_speed, (ui_callback_data_t) 100, NULL },
    { "*50%",
      (ui_callback_t) set_maximum_speed, (ui_callback_data_t) 50, NULL },
    { "*20%",
      (ui_callback_t) set_maximum_speed, (ui_callback_data_t) 20, NULL },
    { "*10%",
      (ui_callback_t) set_maximum_speed, (ui_callback_data_t) 10, NULL },
    { "*No limit",
      (ui_callback_t) set_maximum_speed, (ui_callback_data_t) 0, NULL },
    { "--" },
    { "*Custom...",
      (ui_callback_t) set_custom_maximum_speed, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(radio_Drive8Type)
{
    int current_value;

    resources_get_value("Drive8Type", (resource_value_t *) &current_value);
    if (!call_data) {
        if (current_value != (int) client_data) {
            resources_set_value("Drive8Type",
                                (resource_value_t) client_data);
            ui_update_menus();
        } 
    } else {
        ui_menu_set_tick(w, current_value == (int) client_data);
	if (drive_match_bus((int) client_data, iec_available_busses())) {
            ui_menu_set_sensitive(w, True);
	} else {
            ui_menu_set_sensitive(w, False);
	}
    } 
}

static UI_CALLBACK(radio_Drive9Type)
{
    int current_value;

    resources_get_value("Drive9Type", (resource_value_t *) &current_value);
    if (!call_data) {
        if (current_value != (int) client_data) {
            resources_set_value("Drive9Type",
                                (resource_value_t) client_data);
            ui_update_menus();
        } 
    } else {
        ui_menu_set_tick(w, current_value == (int) client_data);
	if (drive_match_bus((int) client_data, iec_available_busses())) {
            ui_menu_set_sensitive(w, True);
	} else {
            ui_menu_set_sensitive(w, False);
	}
    } 
}

static ui_menu_entry_t set_drive0_type_submenu[] = {
    { "*None", (ui_callback_t) radio_Drive8Type,
      (ui_callback_data_t) DRIVE_TYPE_NONE, NULL },
    { "*1541", (ui_callback_t) radio_Drive8Type,
      (ui_callback_data_t) DRIVE_TYPE_1541, NULL },
    { "*1541-II", (ui_callback_t) radio_Drive8Type,
      (ui_callback_data_t) DRIVE_TYPE_1541II, NULL },
    { "*1571", (ui_callback_t) radio_Drive8Type,
      (ui_callback_data_t) DRIVE_TYPE_1571, NULL },
    { "*1581", (ui_callback_t) radio_Drive8Type,
      (ui_callback_data_t) DRIVE_TYPE_1581, NULL },
    { "*2031", (ui_callback_t) radio_Drive8Type,
      (ui_callback_data_t) DRIVE_TYPE_2031, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive1_type_submenu[] = {
    { "*None", (ui_callback_t) radio_Drive9Type,
      (ui_callback_data_t) DRIVE_TYPE_NONE, NULL },
    { "*1541", (ui_callback_t) radio_Drive9Type,
      (ui_callback_data_t) DRIVE_TYPE_1541, NULL },
    { "*1541-II", (ui_callback_t) radio_Drive9Type,
      (ui_callback_data_t) DRIVE_TYPE_1541II, NULL },
    { "*1571", (ui_callback_t) radio_Drive9Type,
      (ui_callback_data_t) DRIVE_TYPE_1571, NULL },
    { "*1581", (ui_callback_t) radio_Drive9Type,
      (ui_callback_data_t) DRIVE_TYPE_1581, NULL },
    { "*2031", (ui_callback_t) radio_Drive9Type,
      (ui_callback_data_t) DRIVE_TYPE_2031, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive0_extend_image_policy_submenu[] = {
    { "*Never extend", (ui_callback_t) radio_Drive8ExtendImagePolicy,
      (ui_callback_data_t) DRIVE_EXTEND_NEVER, NULL },
    { "*Ask on extend", (ui_callback_t) radio_Drive8ExtendImagePolicy,
      (ui_callback_data_t) DRIVE_EXTEND_ASK, NULL },
    { "*Extend on access", (ui_callback_t) radio_Drive8ExtendImagePolicy,
      (ui_callback_data_t) DRIVE_EXTEND_ACCESS, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive1_extend_image_policy_submenu[] = {
    { "*Never extend", (ui_callback_t) radio_Drive9ExtendImagePolicy,
      (ui_callback_data_t) DRIVE_EXTEND_NEVER, NULL },
    { "*Ask on extend", (ui_callback_t) radio_Drive9ExtendImagePolicy,
      (ui_callback_data_t) DRIVE_EXTEND_ASK, NULL },
    { "*Extend on access", (ui_callback_t) radio_Drive9ExtendImagePolicy,
      (ui_callback_data_t) DRIVE_EXTEND_ACCESS, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive_sync_factor_submenu[] = {
    { "*PAL", (ui_callback_t) radio_DriveSyncFactor,
      (ui_callback_data_t) DRIVE_SYNC_PAL, NULL },
    { "*NTSC", (ui_callback_t) radio_DriveSyncFactor,
      (ui_callback_data_t) DRIVE_SYNC_NTSC, NULL },
    { "*Custom...", (ui_callback_t) set_custom_drive_sync_factor,
      NULL, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive0_idle_method_submenu[] = {
    { "*No traps", (ui_callback_t) radio_Drive8IdleMethod,
      (ui_callback_data_t) DRIVE_IDLE_NO_IDLE, NULL },
    { "*Skip cycles", (ui_callback_t) radio_Drive8IdleMethod,
      (ui_callback_data_t) DRIVE_IDLE_SKIP_CYCLES, NULL },
    { "*Trap idle", (ui_callback_t) radio_Drive8IdleMethod,
      (ui_callback_data_t) DRIVE_IDLE_TRAP_IDLE, NULL },
    { NULL }
};

static ui_menu_entry_t set_drive1_idle_method_submenu[] = {
    { "*No traps", (ui_callback_t) radio_Drive9IdleMethod,
      (ui_callback_data_t) DRIVE_IDLE_NO_IDLE, NULL },
    { "*Skip cycles", (ui_callback_t) radio_Drive9IdleMethod,
      (ui_callback_data_t) DRIVE_IDLE_SKIP_CYCLES, NULL },
    { "*Trap idle", (ui_callback_t) radio_Drive9IdleMethod,
      (ui_callback_data_t) DRIVE_IDLE_TRAP_IDLE, NULL },
    { NULL }
};

static ui_menu_entry_t set_sound_sample_rate_submenu[] = {
    { "*8000Hz", (ui_callback_t) radio_SoundSampleRate,
      (ui_callback_data_t) 8000, NULL },
    { "*11025Hz", (ui_callback_t) radio_SoundSampleRate,
      (ui_callback_data_t) 11025, NULL },
    { "*22050Hz", (ui_callback_t) radio_SoundSampleRate,
      (ui_callback_data_t) 22050, NULL },
    { "*44100Hz", (ui_callback_t) radio_SoundSampleRate,
      (ui_callback_data_t) 44100, NULL },
    { "*48000Hz", (ui_callback_t) radio_SoundSampleRate,
      (ui_callback_data_t) 48000, NULL },
    { NULL }
};

static ui_menu_entry_t set_sound_buffer_size_submenu[] = {
    { "*1.00 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 1000, NULL },
    { "*0.75 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 750, NULL },
    { "*0.50 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 500, NULL },
    { "*0.35 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 350, NULL },
    { "*0.30 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 300, NULL },
    { "*0.25 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 250, NULL },
    { "*0.20 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 200, NULL },
    { "*0.15 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 150, NULL },
    { "*0.10 sec", (ui_callback_t) radio_SoundBufferSize,
      (ui_callback_data_t) 100, NULL },
    { NULL }
};

static ui_menu_entry_t set_sound_suspend_time_submenu[] = {
    { "*Keep going", (ui_callback_t) radio_SoundSuspendTime,
      (ui_callback_data_t) 0, NULL },
    { "*1 sec suspend", (ui_callback_t) radio_SoundSuspendTime,
      (ui_callback_data_t) 1, NULL },
    { "*2 sec suspend", (ui_callback_t) radio_SoundSuspendTime,
      (ui_callback_data_t) 2, NULL },
    { "*5 sec suspend", (ui_callback_t) radio_SoundSuspendTime,
      (ui_callback_data_t) 5, NULL },
    { "*10 sec suspend", (ui_callback_t) radio_SoundSuspendTime,
      (ui_callback_data_t) 10, NULL },
    { NULL }
};

static ui_menu_entry_t set_sound_oversample_submenu [] = {
    { "*1x",
      (ui_callback_t) radio_SoundOversample, (ui_callback_data_t) 0, NULL },
    { "*2x",
      (ui_callback_t) radio_SoundOversample, (ui_callback_data_t) 1, NULL },
    { "*4x",
      (ui_callback_t) radio_SoundOversample, (ui_callback_data_t) 2, NULL },
    { "*8x",
      (ui_callback_t) radio_SoundOversample, (ui_callback_data_t) 3, NULL },
    { NULL }
};

static ui_menu_entry_t set_sound_adjustment_submenu [] = {
    { "*Flexible",
      (ui_callback_t) radio_SoundSpeedAdjustment,
      (ui_callback_data_t) SOUND_ADJUST_FLEXIBLE, NULL },
    { "*Adjusting",
      (ui_callback_t) radio_SoundSpeedAdjustment,
      (ui_callback_data_t) SOUND_ADJUST_ADJUSTING, NULL },
    { "*Exact",
      (ui_callback_t) radio_SoundSpeedAdjustment,
      (ui_callback_data_t) SOUND_ADJUST_EXACT, NULL },
    { NULL }
};

static ui_menu_entry_t sound_settings_submenu[] = {
    { "*Enable sound playback",
      (ui_callback_t) toggle_Sound, NULL, NULL },
    { "--" },
    { "*Sound synchronization",
      NULL, NULL, set_sound_adjustment_submenu },
    { "--" },
    { "*Sample rate",
      NULL, NULL, set_sound_sample_rate_submenu },
    { "*Buffer size",
      NULL, NULL, set_sound_buffer_size_submenu },
    { "*Suspend time",
      NULL, NULL, set_sound_suspend_time_submenu },
    { "*Oversample",
      NULL, NULL, set_sound_oversample_submenu },
    { NULL },
};

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(PrUserDev)

static ui_menu_entry_t pruser_device_submenu[] = {
    { "*Printer 1 (file dump)",
      (ui_callback_t) radio_PrUserDev, (ui_callback_data_t) 0, NULL },
    { "*Printer 2 (exec)",
      (ui_callback_t) radio_PrUserDev, (ui_callback_data_t) 1, NULL },
    { "*Printer 3 (exec)",
      (ui_callback_t) radio_PrUserDev, (ui_callback_data_t) 2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(Printer4Dev)

static ui_menu_entry_t pr4_device_submenu[] = {
    { "*Printer 1 (file dump)",
      (ui_callback_t) radio_Printer4Dev, (ui_callback_data_t) 0, NULL },
    { "*Printer 2 (exec)",
      (ui_callback_t) radio_Printer4Dev, (ui_callback_data_t) 1, NULL },
    { "*Printer 3 (exec)",
      (ui_callback_t) radio_Printer4Dev, (ui_callback_data_t) 2, NULL },
    { NULL }
};

#if 0
/* The file selector cannot select a non-existing file -> does not work */
static UI_CALLBACK(set_printer_dump_file)
{
    char *resource = (char*) client_data;
    char *filename;
    ui_button_t button;

    suspend_speed_eval();

    filename = ui_select_file("Select printer dump file",
                              NULL, False, NULL, NULL, &button);
    switch (button) {
      case UI_BUTTON_OK:
        resources_set_value(resource, (resource_value_t) filename);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
}
#endif

static UI_CALLBACK(set_printer_exec_file)
{
    char *resname = (char*) client_data;
    char title[1024];

    suspend_speed_eval();
    sprintf(title, "Command to execute for printing (preceed with '|')");
    {
        char *value;
        char *new_value;
        int len;

        resources_get_value(resname, (resource_value_t *) &value);
        len = strlen(value) * 2;
        if (len < 255)
            len = 255;
        new_value = alloca(len + 1);
        strcpy(new_value, value);

        if (ui_input_string(title, "Command:", new_value, len) != UI_BUTTON_OK)
            return;

        resources_set_value(resname, (resource_value_t) new_value);
    }
}

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_TOGGLE(Printer4)
UI_MENU_DEFINE_TOGGLE(PrUser)

static ui_menu_entry_t printer_settings_menu[] = {
    { "*IEC device 4 printer emulation",
      (ui_callback_t) toggle_Printer4, NULL, NULL },
    { "IEC printer device",
      NULL, NULL, pr4_device_submenu  },
    { "--" },
    { "*Userport printer emulation",
      (ui_callback_t) toggle_PrUser, NULL, NULL },
    { "Userport printer device",
      NULL, NULL, pruser_device_submenu  },
    { "--" },
    { "Printer device 1...", (ui_callback_t) /* set_printer_dump_file */
					     set_printer_exec_file,
      (ui_callback_data_t) "PrDevice1", NULL },
    { "Printer device 2...", (ui_callback_t) set_printer_exec_file,
      (ui_callback_data_t) "PrDevice2", NULL },
    { "Printer device 3...", (ui_callback_t) set_printer_exec_file,
      (ui_callback_data_t) "PrDevice3", NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t fsdevice_drive8_submenu[] = {
    { "*File system access", (ui_callback_t) toggle_FileSystemDevice8,
      NULL, NULL },
    { "--" },
    { "File system directory...", (ui_callback_t) set_fsdevice_directory,
      (ui_callback_data_t) 8, NULL },
    { "*Convert P00 file names", (ui_callback_t) toggle_FSDevice8ConvertP00,
      NULL, NULL },
    { "*Create P00 files on save", (ui_callback_t) toggle_FSDevice8SaveP00,
      NULL, NULL },
    { "*Hide raw CBM files", (ui_callback_t) toggle_FSDevice8HideCBMFiles,
      NULL, NULL },
    { NULL }
};

static ui_menu_entry_t fsdevice_drive9_submenu[] = {
    { "*File system access", (ui_callback_t) toggle_FileSystemDevice9,
      NULL, NULL },
    { "--" },
    { "File system directory...", (ui_callback_t) set_fsdevice_directory,
      (ui_callback_data_t) 9, NULL },
    { "*Convert P00 file names", (ui_callback_t) toggle_FSDevice9ConvertP00,
      NULL, NULL },
    { "*Create P00 files on save", (ui_callback_t) toggle_FSDevice9SaveP00,
      NULL, NULL },
    { "*Hide raw CBM files", (ui_callback_t) toggle_FSDevice9HideCBMFiles,
      NULL, NULL },
    { NULL }
};

static ui_menu_entry_t fsdevice_drive10_submenu[] = {
    { "*File system access", (ui_callback_t) toggle_FileSystemDevice10,
      NULL, NULL },
    { "--" },
    { "File system directory...", (ui_callback_t) set_fsdevice_directory,
      (ui_callback_data_t) 10, NULL },
    { "*Convert P00 file names", (ui_callback_t) toggle_FSDevice10ConvertP00,
      NULL, NULL },
    { "*Create P00 files on save", (ui_callback_t) toggle_FSDevice10SaveP00,
      NULL, NULL },
    { "*Hide raw CBM files", (ui_callback_t) toggle_FSDevice10HideCBMFiles,
      NULL, NULL },
    { NULL }
};

static ui_menu_entry_t fsdevice_drive11_submenu[] = {
    { "*File system access", (ui_callback_t) toggle_FileSystemDevice11,
      NULL, NULL },
    { "--" },
    { "File system directory...", (ui_callback_t) set_fsdevice_directory,
      (ui_callback_data_t) 11, NULL },
    { "*Convert P00 file names", (ui_callback_t) toggle_FSDevice11ConvertP00,
      NULL, NULL },
    { "*Create P00 files on save", (ui_callback_t) toggle_FSDevice11SaveP00,
      NULL, NULL },
    { "*Hide raw CBM files", (ui_callback_t) toggle_FSDevice11HideCBMFiles,
      NULL, NULL },
    { NULL }
};

static ui_menu_entry_t peripheral_settings_submenu[] = {
    { "Device #8", NULL, NULL, fsdevice_drive8_submenu },
    { "Device #9", NULL, NULL, fsdevice_drive9_submenu },
    { "Device #10", NULL, NULL, fsdevice_drive10_submenu },
    { "Device #11", NULL, NULL, fsdevice_drive11_submenu },
    { "--" },
    { "Printer settings", NULL, NULL, printer_settings_menu },
    { "--" },
    { "*Disable Kernal traps", (ui_callback_t) toggle_NoTraps, NULL, NULL },
    { NULL }
};

/* This menu is for the C64 */
static ui_menu_entry_t drive_settings_submenu[] = {
    { "*Enable true drive emulation",
      (ui_callback_t) toggle_DriveTrueEmulation, NULL, NULL },
    { "--" },
    { "Drive #8 model",
      NULL, NULL, set_drive0_type_submenu },
    { "*Drive #8 enable parallel cable",
      (ui_callback_t) toggle_Drive8ParallelCable, NULL, NULL },
    { "Drive #8 40-track image support",
      NULL, NULL, set_drive0_extend_image_policy_submenu },
    { "Drive #8 idle method",
      NULL, NULL, set_drive0_idle_method_submenu },
    { "--" },
    { "Drive #9 model",
      NULL, NULL, set_drive1_type_submenu },
    { "*Drive #9 enable parallel cable",
      (ui_callback_t) toggle_Drive9ParallelCable, NULL, NULL },
    { "Drive #9 40-track image support",
      NULL, NULL, set_drive1_extend_image_policy_submenu },
    { "Drive #9 idle method",
      NULL, NULL, set_drive1_idle_method_submenu },
    { "--" },
    { "Drive sync factor",
      NULL, NULL, set_drive_sync_factor_submenu },
    { NULL }
};

/* This menu is for the PET/C610 */
static ui_menu_entry_t par_drive_settings_submenu[] = {
    { "*Enable true drive emulation",
      (ui_callback_t) toggle_DriveTrueEmulation, NULL, NULL },
    { "--" },
    { "Drive #8 floppy disk type",
      NULL, NULL, set_drive0_type_submenu },
    { "Drive #8 40-track image support",
      NULL, NULL, set_drive0_extend_image_policy_submenu },
#if 0
    { "Drive #8 idle method",
      NULL, NULL, set_drive0_idle_method_submenu },
#endif
    { "--" },
    { "Drive #9 floppy disk type",
      NULL, NULL, set_drive1_type_submenu },
    { "Drive #9 40-track image support",
      NULL, NULL, set_drive1_extend_image_policy_submenu },
#if 0
    { "Drive #9 idle method",
      NULL, NULL, set_drive1_idle_method_submenu },
#endif
    { "--" },
    { "Drive sync factor",
      NULL, NULL, set_drive_sync_factor_submenu },
    { NULL }
};

/* ------------------------------------------------------------------------- */

#ifdef USE_VIDMODE_EXTENSION

UI_MENU_DEFINE_TOGGLE(UseFullscreen)

UI_MENU_DEFINE_TOGGLE(FullscreenDoubleSize)

UI_MENU_DEFINE_TOGGLE(FullscreenDoubleScan)

ui_menu_entry_t ui_fullscreen_settings_submenu[] = {
    { "*Enable",
      (ui_callback_t) toggle_UseFullscreen, NULL, NULL, XK_d, UI_HOTMOD_META },
    { "--"},
    { "*Double size",
      (ui_callback_t) toggle_FullscreenDoubleSize, NULL, NULL },
    { "*Double scan",
      (ui_callback_t) toggle_FullscreenDoubleScan, NULL, NULL },
    { "--"},
    { "Resolutions",
      (ui_callback_t) NULL, NULL, NULL },
    { NULL }
};
#endif 

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t video_settings_submenu[] = {
    { "*Video cache",
      (ui_callback_t) toggle_VideoCache, NULL, NULL },
    { "*Double size",
      (ui_callback_t) toggle_DoubleSize, NULL, NULL },
    { "*Double scan",
      (ui_callback_t) toggle_DoubleScan, NULL, NULL },
    { "*Use XSync()",
      (ui_callback_t) toggle_UseXSync, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

ui_menu_entry_t ui_performance_settings_menu[] = {
    { "Refresh rate",
      NULL, NULL, set_refresh_rate_submenu },
    { "Maximum speed",
      NULL, NULL, set_maximum_speed_submenu },
    { "*Enable warp mode",
      (ui_callback_t) toggle_WarpMode, NULL, NULL, XK_w, UI_HOTMOD_META },
    { NULL }
};

#ifdef USE_VIDMODE_EXTENSION

static UI_CALLBACK(FullscreenMenu)
{
    if (call_data) {
        ui_menu_set_sensitive(w, ui_is_fullscreen_available());
    }
}

ui_menu_entry_t ui_fullscreen_settings_menu[] = {
    { "*Fullscreen settings",
      (ui_callback_t) FullscreenMenu, NULL, ui_fullscreen_settings_submenu },
    { NULL }
};
#endif 


ui_menu_entry_t ui_video_settings_menu[] = {
    { "Video settings",
      NULL, NULL, video_settings_submenu },
    { NULL }
};

ui_menu_entry_t ui_keyboard_settings_menu[] = {
    { "Keyboard settings",
      NULL, NULL, keyboard_settings_submenu },
    { NULL }
};

ui_menu_entry_t ui_sound_settings_menu[] = {
    { "Sound settings",
      NULL, NULL, sound_settings_submenu },
    { NULL }
};

/* c64 */
ui_menu_entry_t ui_drive_settings_menu[] = {
    { "Drive settings",
      NULL, NULL, drive_settings_submenu },
    { NULL }
};

/* PET/C610 */
ui_menu_entry_t ui_par_drive_settings_menu[] = {
    { "Drive settings",
      NULL, NULL, par_drive_settings_submenu },
    { NULL }
};

ui_menu_entry_t ui_peripheral_settings_menu[] = {
    { "Peripheral settings",
      NULL, NULL, peripheral_settings_submenu },
    { NULL }
};

ui_menu_entry_t ui_settings_settings_menu[] = {
    { "Save settings",
      (ui_callback_t) save_resources, NULL, NULL },
    { "Load settings",
      (ui_callback_t) load_resources, NULL, NULL },
    { "Restore default settings",
      (ui_callback_t) set_default_resources, NULL, NULL },
    { "*Save settings on exit",
      (ui_callback_t) toggle_SaveResourcesOnExit, NULL, NULL },
    { NULL }
};
