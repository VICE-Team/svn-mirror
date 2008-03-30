/*
 * menudefs.c - Definition of menu commands.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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
#include <unistd.h>
#include <dir.h>

#include "menudefs.h"
#include "tuiview.h"
#include "ui.h"
#include "tui.h"

/* FIXME: argh!!  megahack!  <dir.h> #defines `DRIVE', which we need
   in "drive.h".  */
#undef DRIVE
#include "drive.h"
#include "tapeunit.h"
#include "serial.h"
#include "autostart.h"
#include "utils.h"
#include "true1541.h"
#include "video.h"
#include "info.h"
#include "machine.h"

/* ------------------------------------------------------------------------- */

tui_menu_t ui_main_menu;
tui_menu_t ui_attach_submenu;
tui_menu_t ui_detach_submenu;
tui_menu_t ui_video_submenu;
tui_menu_t ui_drive_submenu;
tui_menu_t ui_sound_buffer_size_submenu;
tui_menu_t ui_sound_sample_rate_submenu;
tui_menu_t ui_sound_submenu;
tui_menu_t ui_special_submenu;
tui_menu_t ui_reset_submenu;
tui_menu_t ui_quit_submenu;
tui_menu_t ui_info_submenu;

/* ------------------------------------------------------------------------ */

static TUI_MENU_CALLBACK(attach_disk_callback)
{
    char *s;

    if (been_activated) {
        char *default_item, *directory;
	char *name;

	s = (char *)serial_get_file_name((int)param);
	fname_split(s, &directory, &default_item);

	name = tui_file_selector("Attach a disk image", directory,
				 "*.[dx]6[4z]", default_item,
				 read_disk_image_contents);

	if (name != NULL
	    && (s == NULL || strcasecmp(name, s) != 0)
	    && serial_select_file(DT_DISK | DT_1541, (int)param, name) < 0) {
	    tui_error("Invalid disk image.");
	}
	ui_update_menus();

	free(directory), free(default_item);
    }

    s = (char *)serial_get_file_name((int)param);
    if (s == NULL || *s == '\0')
	return "(none)";
    else
	return s;
}

static TUI_MENU_CALLBACK(attach_tape_callback)
{
    char *s;

    if (been_activated) {
        char *directory, *default_item;
	char *name;

	s = (char *)serial_get_file_name(1);
	fname_split(s, &directory, &default_item);

	name = tui_file_selector("Attach a tape image", directory,
				 "*.t6[4z]", default_item,
				 read_tape_image_contents);

	if (name != NULL
	    && (s == NULL || strcasecmp(s, name) != 0)
	    && serial_select_file(DT_TAPE, 1, name) < 0) {
	    tui_error("Invalid tape image.");
	}
	ui_update_menus();
    }

    s = (char *)serial_get_file_name(1);
    if (s == NULL || *s == '\0')
	return "(none)";
    else
	return s;
}

static TUI_MENU_CALLBACK(autostart_callback)
{
    if (been_activated) {
	if (autostart_device((int)param) < 0)
	    tui_error("Cannot autostart device #%d", (int)param);
    }

    return NULL;
}

TUI_MENU_CALLBACK(detach_disk_callback)
{
    char *s;

    if (been_activated) {
	serial_remove((int)param);
	ui_update_menus();
    }

    s = (char *)serial_get_file_name((int)param);
    if (s == NULL || *s == '\0')
	return "(none)";
    else
	return s;
}

static TUI_MENU_CALLBACK(detach_tape_callback)
{
    char *s;

    if (been_activated) {
	serial_remove(1);
	ui_update_menus();
    }

    s = (char *)serial_get_file_name(1);
    if (s == NULL || *s == '\0')
	return "(none)";
    else
	return s;
}

/* ------------------------------------------------------------------------ */

static char *change_workdir_callback(int been_activated, void *param_unused)
{
    char s[256];

    if (!been_activated)
        return NULL;

    *s = '\0';

    if (tui_input_string("Change working directory",
    			 "New directory:", s, 255) == -1)
        return NULL;

    remove_spaces(s);
    if (*s == '\0')
        return NULL;

    if (chdir(s) == -1)
    	tui_error("Invalid directory.");

    return NULL;
}

/* ------------------------------------------------------------------------- */

static TUI_MENU_CALLBACK(resolution_submenu_callback)
{
    int mode;

    resources_get_value("VGAMode", (resource_value_t *) &mode);
    return vga_modes[mode].description;
}

TUI_MENU_DEFINE_RADIO(VGAMode)

static TUI_MENU_CALLBACK(refresh_rate_submenu_callback)
{
    int v;

    resources_get_value("RefreshRate", (resource_value_t *) &v);
    if (v == 0) {
	return "Auto";
    } else {
	static char s[256];

	sprintf(s, "1/%d", v);
	return s;
    }
}

TUI_MENU_DEFINE_RADIO(RefreshRate)

TUI_MENU_DEFINE_TOGGLE(VideoCache)

/* ------------------------------------------------------------------------- */

TUI_MENU_DEFINE_TOGGLE(True1541)

static char *toggle_True1541SyncFactor_callback(int been_activated,
                                                void *param_unused)
{
    int value;

    resources_get_value("True1541SyncFactor", (resource_value_t *) &value);

    if (been_activated) {
	if (value == TRUE1541_SYNC_PAL)
	    value = TRUE1541_SYNC_NTSC;
	else
	    value = TRUE1541_SYNC_PAL;
        resources_set_value("True1541SyncFactor", (resource_value_t) value);
    }

    switch (value) {
      case TRUE1541_SYNC_PAL:
	return "PAL";
      case TRUE1541_SYNC_NTSC:
	return "NTSC";
      default:
	return "(Custom)";
    }
}

static char *toggle_True1541IdleMethod_callback(int been_activated,
                                                void *param_unused)
{
    int value;

    resources_get_value("True1541IdleMethod", (resource_value_t *) &value);

    if (been_activated) {
	if (value == TRUE1541_IDLE_SKIP_CYCLES)
	    value = TRUE1541_IDLE_TRAP_IDLE;
	else
	    value = TRUE1541_IDLE_SKIP_CYCLES;
        resources_set_value("True1541IdleMethod", (resource_value_t) value);
    }

    switch (value) {
      case TRUE1541_IDLE_TRAP_IDLE:
	return "Trap idle";
      case TRUE1541_IDLE_SKIP_CYCLES:
	return "Skip cycles";
      default:
	return "(Unknown)";
    }
}

TUI_MENU_DEFINE_TOGGLE(True1541ParallelCable)

static tui_menu_item_def_t true1541_settings_submenu[] = {
    { "True 1541 _Emulation:",
      "Enable hardware-level floppy drive emulation",
      toggle_True1541_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "True 1541 _Sync Factor:",
      "Select 1541/machine clock ratio",
      toggle_True1541SyncFactor_callback, NULL, 8,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "True 1541 _Idle Method:",
      "Select method for disk drive idle",
      toggle_True1541IdleMethod_callback, NULL, 11,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Enable _Parallel Cable:",
      "Enable a SpeedDOS-compatible parallel cable.",
      toggle_True1541ParallelCable_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

TUI_MENU_DEFINE_TOGGLE(Sound)

static char *sound_sample_rate_submenu_callback(int been_activated, void *param)
{
    static char s[256];
    int value;

    resources_get_value("SoundSampleRate", (resource_value_t *) &value);
    sprintf(s, "%d Hz", value);
    return s;
}

TUI_MENU_DEFINE_RADIO(SoundSampleRate)

TUI_MENU_DEFINE_RADIO(SoundBufferSize)

static char *sound_buffer_size_submenu_callback(int been_activated, void *param)
{
    static char s[256];
    int value;

    resources_get_value("SoundBufferSize", (resource_value_t *) &value);
    sprintf(s, "%d msec", value);
    return s;
}

static tui_menu_item_def_t sample_rate_submenu[] = {
    { "8000 Hz",
      "Set sampling rate to 8000 Hz",
      radio_SoundSampleRate_callback, (void *) 8000, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "11025 Hz",
      "Set sampling rate to 11025 Hz",
      radio_SoundSampleRate_callback, (void *) 11025, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "22050 Hz",
      "Set sampling rate to 22050 Hz",
      radio_SoundSampleRate_callback, (void *) 22050, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "44100 Hz",
      "Set sampling rate to 44100 Hz",
      radio_SoundSampleRate_callback, (void *) 44100, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t sound_buffer_size_submenu[] = {
    { "50 msec",
      "Set sound buffer size to 50 msec",
      radio_SoundBufferSize_callback, (void *) 50, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "75 msec",
      "Set sound buffer size to 100 msec",
      radio_SoundBufferSize_callback, (void *) 100, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "150 msec",
      "Set sound buffer size to 150 msec",
      radio_SoundBufferSize_callback, (void *) 150, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "200 msec",
      "Set sound buffer size to 200 msec",
      radio_SoundBufferSize_callback, (void *) 200, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "250 msec",
      "Set sound buffer size to 250 msec",
      radio_SoundBufferSize_callback, (void *) 250, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "300 msec",
      "Set sound buffer size to 300 msec",
      radio_SoundBufferSize_callback, (void *) 300, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "350 msec",
      "Set sound buffer size to 350 msec",
      radio_SoundBufferSize_callback, (void *) 350, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t sound_submenu[] = {
    { "Sound _Playback:",
      "Enable sound output",
      toggle_Sound_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_Sample Frequency:",
      "Choose sound output sampling rate",
      sound_sample_rate_submenu_callback, NULL, 10,
      TUI_MENU_BEH_CONTINUE, sample_rate_submenu, "Sample rate" },
    { "Sound _Buffer Size:",
      "Specify playback latency",
      sound_buffer_size_submenu_callback, NULL, 10,
      TUI_MENU_BEH_CONTINUE, sound_buffer_size_submenu, "Latency" },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static char *toggle_joystick_callback(int been_activated, void *param)
{
#if 0
    if (been_activated) {
	if (app_resources.joyPort == (int)param)
	    app_resources.joyPort = (int)param == 1 ? 2 : 1;
	else
	    app_resources.joyPort = (int)param;
	ui_update_menus();
    }

    if (app_resources.joyPort == (int)param) {
	return "Numpad + RightCtrl";
    } else {
	return joystick_available ? "Joystick" : "None";
    }
#endif
    return NULL;
}

static char *save_settings_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
	if (resources_save(NULL) < 0)
	    tui_error("Cannot save settings.");
	else
	    tui_message("Settings saved successfully.");
    }

    return NULL;
}

static char *load_settings_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
	if (resources_load(NULL) < 0)
	    tui_error("Cannot load settings.");
	else
	    tui_message("Settings loaded successfully.");
    }

    return NULL;
}

static char *restore_default_settings_callback(int been_activated,
					       void *param_unused)
{
    if (been_activated) {
	resources_set_defaults();
	tui_message("Default settings restored.");
    }

    return NULL;
}

static char *quit_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
	_setcursortype(_NORMALCURSOR);
	normvideo();
	clrscr();
#ifdef HAVE_TRUE1541
        true1541_detach_floppy();
#endif
	/* disable_log(); FIXME */
#ifdef UNSTABLE
	printf("VICE version %s (unstable).\n", VERSION);
#else
	printf("VICE version %s.\n", VERSION);
#endif
	printf("\nOfficial VICE homepage: http://www.tu-chemnitz.de/~fachat/vice/vice.html\n\n");
#if 0
	printf("Report problems, bugs and suggestions to ettore@comm2000.it\n\n");
#endif
	exit(0);
    }

    return NULL;
}

static tui_menu_item_def_t quit_submenu[] = {
    { "_Not really!",
      "Go back to the menu",
      NULL, NULL, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_Yes, exit emulator",
      "Leave the emulator completely",
      quit_callback, NULL, 0,
      TUI_MENU_BEH_RESUME, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------ */

static void mon_trap(ADDRESS addr)
{
    /* FIXME */
#if 0
    int old_mode;

    disable_log();
    enable_text();
    clrscr();
    _set_screen_lines(43);
    _setcursortype(_SOLIDCURSOR);
    old_mode = setmode(STDIN_FILENO, O_TEXT);

    mon(addr);

    setmode(STDIN_FILENO, old_mode);
    enable_log(0);
    disable_text();
#endif
}

static char *monitor_callback(int been_activated, void *param_unused)
{
    if (been_activated)
	maincpu_trigger_trap(mon_trap);

    return NULL;
}

/* ------------------------------------------------------------------------- */

static char *soft_reset_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
	maincpu_trigger_reset();
    }

    return NULL;
}

static char *hard_reset_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
	mem_powerup();
	maincpu_trigger_reset();
    }

    return NULL;
}

static tui_menu_item_def_t reset_submenu[] = {
    { "_Not Really!",
      "Go back to the menu",
      NULL, NULL, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Do a _Soft Reset",
      "Do a soft reset without resetting the memory",
      soft_reset_callback, NULL, 0,
      TUI_MENU_BEH_RESUME, NULL, NULL },
    { "Do a _Hard Reset",
      "Clear memory and reset as after a power-up",
      hard_reset_callback, NULL, 0,
      TUI_MENU_BEH_RESUME, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static char *show_copyright_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
	static char *str_list[] = {
	    "",
	    "V I C E",
	    "Version " VERSION,
#ifdef UNSTABLE
	    "(unstable)",
#endif
	    "",
            "Copyright (c) 1996-1998 Ettore Perazzoli",
            "Copyright (c) 1996-1998 Andre' Fachat",
            "Copyright (c) 1993-1994, 1997-1998 Teemu Rantanen",
            "Copyright (c) 1997-1998 Daniel Sladic",
            "Copyright (c) 1998 Andreas Boose",
            "Copyright (c) 1993-1996 Jouko Valta",
            "Copyright (c) 1993-1994 Jarkko Sonninen",
            "",
	    "Official VICE homepage:",
	    "http://www.tu-chemnitz.de/~fachat/vice/vice.html",
#ifdef UNSTABLE
	    "",
	    "WARNING: this is an *unstable* test version!",
	    "Please check out the homepage for the latest updates.",
#endif
	    ""
	};
	int num_items = sizeof(str_list) / sizeof(*str_list);
	tui_area_t backing_store = NULL;
	int height, width;
	int y, x, i;

	for (width = i = 0; i < num_items; i++) {
	    int l = strlen(str_list[i]);
	    if (l > width)
		width = l;
	}
	width += 4;
	height = num_items + 2;
	x = CENTER_X(width);
	y = CENTER_Y(height);

	tui_display_window(x, y, width, height,
			   MESSAGE_BORDER, MESSAGE_BACK, "About VICE",
			   &backing_store);
	tui_set_attr(MESSAGE_FORE, MESSAGE_BACK, 0);
	for (i = 0; i < num_items; i++)
	    tui_display(CENTER_X(strlen(str_list[i])), y + i + 1, 0, "%s",
			str_list[i]);

	getkey();

	tui_area_put(backing_store, x, y);
	tui_area_free(backing_store);
    }
    return NULL;
}

static char *show_info_callback(int been_activated, void *param)
{
    if (been_activated)
	tui_view_text(70, 20, NULL, (const char *)param);
    return NULL;
}

static tui_menu_item_def_t info_submenu[] = {
    { "_Copyright",
      "VICE copyright information",
      show_copyright_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_Who made what?",
      "VICE contributors",
      show_info_callback, (void *)contrib_text, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_License",
      "VICE license (GNU General Public License)",
      show_info_callback, (void *)license_text, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_No warranty!",
      "VICE is distributed WITHOUT ANY WARRANTY!",
      show_info_callback, (void *)warranty_text, 0,
      TUI_MENU_BEH_CONTINUE },
    { NULL }
};

/* ------------------------------------------------------------------------- */

/* This is a bit of a hack, but I prefer this way instead of writing 1,000
   menu entries...  */
static void create_ui_video_submenu(void)
{
    static tui_menu_t refresh_rate_submenu, vga_mode_submenu;

    refresh_rate_submenu = tui_menu_create("Refresh", 1);
    {
	int i;
	char label[256], desc[256];

	for (i = 1; i <= 10; i++) {
	    if (i != 10)
		sprintf(label, "1/_%d", i);
	    else
		strcpy(label, "1/1_0");
	    if (i == 1)
		sprintf(desc, "Set refresh rate to 1/%d (update every frame)",
			i);
	    else
		sprintf(desc,
			"Set refresh rate to 1/%d (update once every %d frames)",
			i, i);
	    tui_menu_add_item(refresh_rate_submenu, label, desc,
			      radio_RefreshRate_callback, (void *)i, 0,
			      TUI_MENU_BEH_CLOSE);
	}
	tui_menu_add_separator(refresh_rate_submenu);
	tui_menu_add_item(refresh_rate_submenu, "_Automatic",
			  "Let the emulator select an appropriate refresh rate automagically",
			  radio_RefreshRate_callback, NULL, 0,
			  TUI_MENU_BEH_CLOSE);
    }

    vga_mode_submenu = tui_menu_create("VGA Resolution", 1);
    {
	int i;

	for (i = 0; i < NUM_VGA_MODES; i++) {
	    char s1[256], s2[256];

	    /* FIXME: hotkeys work only for less than 11 elements. */
	    sprintf(s1, "Mode _%d: %s", i, vga_modes[i].description);
	    sprintf(s2, "Set VGA resolution to %s", vga_modes[i].description);
	    tui_menu_add_item(vga_mode_submenu, s1, s2,
			      radio_VGAMode_callback, (void *)i, 0,
			      TUI_MENU_BEH_CLOSE);
	}
    }

    ui_video_submenu = tui_menu_create("Video Settings", 1);

    tui_menu_add_submenu(ui_video_submenu,"_VGA Resolution:",
			 "Choose screen resolution for video emulation",
			 vga_mode_submenu,
			 resolution_submenu_callback, NULL, 8);

    tui_menu_add_submenu(ui_video_submenu, "_Refresh Rate:",
			 "Choose frequency of screen refresh",
			 refresh_rate_submenu,
			 refresh_rate_submenu_callback, NULL, 4);
    tui_menu_add_item(ui_video_submenu, "Video _Cache:",
		      "Enable screen cache",
		      toggle_VideoCache_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);
}

/* ------------------------------------------------------------------------- */

TUI_MENU_DEFINE_TOGGLE(FileSystemDevice8)
TUI_MENU_DEFINE_TOGGLE(FileSystemDevice9)
TUI_MENU_DEFINE_TOGGLE(FileSystemDevice10)
TUI_MENU_DEFINE_TOGGLE(FileSystemDevice11)

TUI_MENU_DEFINE_TOGGLE(FSDevice8ConvertP00)
TUI_MENU_DEFINE_TOGGLE(FSDevice9ConvertP00)
TUI_MENU_DEFINE_TOGGLE(FSDevice10ConvertP00)
TUI_MENU_DEFINE_TOGGLE(FSDevice11ConvertP00)

TUI_MENU_DEFINE_TOGGLE(FSDevice8SaveP00)
TUI_MENU_DEFINE_TOGGLE(FSDevice9SaveP00)
TUI_MENU_DEFINE_TOGGLE(FSDevice10SaveP00)
TUI_MENU_DEFINE_TOGGLE(FSDevice11SaveP00)

static char *set_fsdevice_directory_callback(int been_activated,
					     void *param)
{
    int unit = (int) param;
    char *v;
    char rname[256];

    sprintf(rname, "FSDevice%dDir", unit);

    if (been_activated) {
	char *path;
	int len = 255;

	resources_get_value(rname, (resource_value_t *) &v);
	if (len < strlen(v) * 2)
	    len = strlen(v) * 2;
	path = alloca(len + 1);
	strcpy(path, v);
	if (tui_input_string("Insert path",
			     "Path:", path, len) != -1) {
	    remove_spaces(path);
	    fsdevice_set_directory(path, unit);
	}
    }

    resources_get_value(rname, (resource_value_t *) &v);
    return v;
}

tui_menu_item_def_t fsdevice_submenu[] = {
    { "Device _8:  Allow access:",
      "Allow device 8 to access the MS-DOS file system",
      toggle_FileSystemDevice8_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "           Directory:",
      "Specify access directory for device 8",
      set_fsdevice_directory_callback, (void *) 8, 40,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "           Convert P00 names:",
      "Handle P00 names on device 8",
      toggle_FSDevice8ConvertP00_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "           Save P00 files:",
      "Create P00 files on device 8",
      toggle_FSDevice8SaveP00_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "--" },
    { "Device _9:  Allow access:",
      "Allow device 9 to access the MS-DOS file system",
      toggle_FileSystemDevice9_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "           Directory:",
      "Specify access directory for device 9",
      set_fsdevice_directory_callback, (void *) 9, 40,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "           Convert P00 names:",
      "Handle P00 names on device 9",
      toggle_FSDevice9ConvertP00_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "           Save P00 files:",
      "Create P00 files on device 9",
      toggle_FSDevice9SaveP00_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "--" },
    { "Device 1_0: Allow access:",
      "Allow device 10 to access the MS-DOS file system",
      toggle_FileSystemDevice10_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "           Directory:",
      "Specify access directory for device 10",
      set_fsdevice_directory_callback, (void *) 10, 40,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "           Convert P00 names:",
      "Handle P00 names on device 10",
      toggle_FSDevice10ConvertP00_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "           Save P00 files:",
      "Create P00 files on device 10",
      toggle_FSDevice10SaveP00_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "--" },
    { "Device 1_1: Allow access:",
      "Allow device 11 to access the MS-DOS file system",
      toggle_FileSystemDevice11_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "           Directory:",
      "Specify access directory for device 11",
      set_fsdevice_directory_callback, (void *) 11, 40,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "           Convert P00 names:",
      "Handle P00 names on device 11",
      toggle_FSDevice11ConvertP00_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "           Save P00 files:",
      "Create P00 files on device 11",
      toggle_FSDevice11SaveP00_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static char *speed_submenu_callback(int been_activated,
				    void *param_unused)
{
    static char s[1024];
    int value;

    resources_get_value("Speed", (resource_value_t *) &value);
    if (value) {
	sprintf(s, "%d%%", value);
	return s;
    } else
	return "None";
}

static char *speed_callback(int been_activated, void *param)
{
    if (been_activated) {
        int value;

	if ((int)param < 0) {
	    char buf[25];

	    *buf = '\0';
	    if (tui_input_string("Maximum Speed",
				 "Enter maximum speed (%%):",
				 buf, 25) == 0) {
		value = atoi(buf);
		if (value > 1000)
		    value = 1000;
		else if (value < 0)
		    value = 0;
	    }
	} else {
	    value = (int)param;
	}

        resources_set_value("Speed", (resource_value_t) value);
    }
    return NULL;
}

static void create_special_submenu(void)
{
    static tui_menu_t speed_submenu;

    ui_special_submenu = tui_menu_create("Special Features", 1);

    /* Speed limit submenu.  */
    {
	int i;
	int speed[4] = { 100, 50, 20, 10 };
	char s1[256], s2[256];

	speed_submenu = tui_menu_create("Speed Limit", 1);
	for (i = 0; i < 4; i++) {
	    if (speed[i] == 100)
	        sprintf(s1, "Limit speed to the one of the real %s",
			machine_name);
	    else
	        sprintf(s1, "Limit speed to %d%% of the real %s",
			speed[i], machine_name);
	    sprintf(s2, "_%d%%", speed[i]);
	    tui_menu_add_item(speed_submenu, s2, s1,
			      speed_callback, (void *)100, 5,
			      TUI_MENU_BEH_CLOSE);
	}
	tui_menu_add_item(speed_submenu, "_No Limit",
			  "Run the emulator as fast as possible",
			  speed_callback, (void *)0, 5,
			  TUI_MENU_BEH_CLOSE);
	tui_menu_add_separator(speed_submenu);
	    tui_menu_add_item(speed_submenu, "_Custom...",
			      "Specify a custom relative speed value",
			      speed_callback, (void *)-1, 5,
			      TUI_MENU_BEH_CLOSE);
    }

    tui_menu_add_submenu(ui_special_submenu, "_Speed Limit:",
			 "Specify a custom speed limit",
			 speed_submenu, speed_submenu_callback,
			 NULL, 5);


    /* File system access.  */
    {
	tui_menu_t tmp = tui_menu_create("MS-DOS directory access", 1);

	tui_menu_add(tmp, fsdevice_submenu);
	tui_menu_add_submenu(ui_special_submenu,
			     "MS-DOS _Directory Access",
			     "Options to access MS-DOS directories from within the emulator",
			     tmp, NULL, NULL, 0);
    }
}

/* ------------------------------------------------------------------------- */

void ui_create_main_menu(int has_tape, int has_true1541)
{
    /* Main menu. */
    ui_main_menu = tui_menu_create(NULL, 1);

    ui_attach_submenu = tui_menu_create("Attach Images", 1);
    tui_menu_add_item(ui_attach_submenu, "Drive #_8:",
		      "Attach disk image for disk drive #8",
		      attach_disk_callback, (void *)8, 30,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(ui_attach_submenu, "Drive #_9:",
		      "Attach disk image for disk drive #9",
		      attach_disk_callback, (void *)9, 30,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(ui_attach_submenu, "Drive #1_0:",
		      "Attach disk image for disk drive #10",
		      attach_disk_callback, (void *)10, 30,
		      TUI_MENU_BEH_CONTINUE);

    if (has_tape) {
	tui_menu_add_separator(ui_attach_submenu);
	tui_menu_add_item(ui_attach_submenu,"Tape #_1:",
			  "Attach tape image for cassette player (device #1)",
			  attach_tape_callback, NULL, 30,
			  TUI_MENU_BEH_CONTINUE);
    }

    tui_menu_add_separator(ui_attach_submenu);
    tui_menu_add_item(ui_attach_submenu, "Autostart _Drive 8",
		      "Reset the emulator and run the first program in the disk in drive 8",
		      autostart_callback, (void *)8, 0,
		      TUI_MENU_BEH_RESUME);

    if (has_tape) {
	tui_menu_add_item(ui_attach_submenu, "Autostart _Tape",
			  "Reset the emulator and run the first program on the tape image",
			  autostart_callback, (void *)1, 0,
			  TUI_MENU_BEH_RESUME);
    }


    ui_detach_submenu = tui_menu_create("Detach Images", 1);
    tui_menu_add_item(ui_detach_submenu, "Drive #_8:",
		      "Remove disk from disk drive #8",
		      detach_disk_callback, (void *)8, 30,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(ui_detach_submenu,"Drive #_9:",
		      "Remove disk from disk drive #9",
		      detach_disk_callback, (void *)9, 30,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(ui_detach_submenu, "Drive #1_0:",
		      "Remove disk from disk drive #10",
		      detach_disk_callback, (void *)10, 30,
		      TUI_MENU_BEH_CONTINUE);

    if (has_tape) {
	tui_menu_add_separator(ui_detach_submenu);
	tui_menu_add_item(ui_detach_submenu, "Tape #_1:",
			  "Remove tape from cassette player (device #1)",
			  detach_tape_callback, NULL, 30,
			  TUI_MENU_BEH_CONTINUE);
    }

    if (has_tape) {
	tui_menu_add_submenu(ui_main_menu, "_Attach Disk/Tape Image...",
			     "Specify disk/tape images for virtual floppy/cassette drives",
			     ui_attach_submenu, NULL, 0,
			     TUI_MENU_BEH_CONTINUE);
	tui_menu_add_submenu(ui_main_menu, "_Detach Disk/Tape Image...",
			     "Remove disks/tapes from virtual drives",
			     ui_detach_submenu, NULL, 0,
			     TUI_MENU_BEH_CONTINUE);
    } else {
	tui_menu_add_submenu(ui_main_menu, "_Attach Disk Image...",
			     "Specify disk images for virtual floppy drives",
			     ui_attach_submenu, NULL, 0,
			     TUI_MENU_BEH_CONTINUE);
	tui_menu_add_submenu(ui_main_menu, "_Detach Disk Image...",
			     "Remove disks from virtual drives",
			     ui_detach_submenu, NULL, 0,
			     TUI_MENU_BEH_CONTINUE);
    }

    tui_menu_add_item(ui_main_menu, "_Change Working Directory...",
		      "Change the current working directory",
		      change_workdir_callback, NULL, 0,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_separator(ui_main_menu);


    create_ui_video_submenu();
    tui_menu_add_submenu(ui_main_menu, "_Video Settings...",
			 "Screen parameters",
			 ui_video_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);

    if (has_true1541) {
        ui_drive_submenu = tui_menu_create("Drive Settings", 1);
	tui_menu_add(ui_drive_submenu, true1541_settings_submenu);
	tui_menu_add_submenu(ui_main_menu, "_Drive Settings...",
			     "Drive emulation settings",
			     ui_drive_submenu, NULL, 0,
			     TUI_MENU_BEH_CONTINUE);
    }

    ui_sound_submenu = tui_menu_create("Audio Settings", 1);
    tui_menu_add(ui_sound_submenu, sound_submenu);
    tui_menu_add_submenu(ui_main_menu, "_Sound Settings...",
			 "Sampling rate, sound output, soundcard settings",
			 ui_sound_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);

    create_special_submenu();

    tui_menu_add_submenu(ui_main_menu, "Special _Options...",
			 "Extra emulation features",
			 ui_special_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);

    tui_menu_add_separator(ui_main_menu);

    tui_menu_add_item(ui_main_menu, "_Write Settings",
		      "Save current settings as default for next session",
		      save_settings_callback, NULL, 0,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(ui_main_menu, "_Load Settings",
		      "Load saved settings from previous session",
		      load_settings_callback, NULL, 0,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(ui_main_menu, "Restore _Factory Defaults",
		      "Set default settings",
		      restore_default_settings_callback, NULL, 0,
		      TUI_MENU_BEH_CONTINUE);

    tui_menu_add_separator(ui_main_menu);

    tui_menu_add_item(ui_main_menu, "_Monitor",
		      "Enter the built-in machine language monitor",
		      monitor_callback, NULL, 0,
		      TUI_MENU_BEH_RESUME);

    ui_reset_submenu = tui_menu_create("Reset?", 1);
    tui_menu_add(ui_reset_submenu, reset_submenu);
    tui_menu_add_submenu(ui_main_menu, "_Reset " /* EMULATOR */,
			 "Reset the machine",
			 ui_reset_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);

    ui_quit_submenu = tui_menu_create("Quit", 1);
    tui_menu_add(ui_quit_submenu, quit_submenu);
    tui_menu_add_submenu(ui_main_menu, "_Quit",
			 "Quit " /* EMULATOR */ " emulator",
			 ui_quit_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);
    tui_menu_add_separator(ui_main_menu);

    ui_info_submenu = tui_menu_create("Info", 1);
    tui_menu_add(ui_info_submenu, info_submenu);
    tui_menu_add_submenu(ui_main_menu, "VICE _Info...",
			 "VICE is Free Software distributed under the GNU General Public License!",
			 ui_info_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);
}
