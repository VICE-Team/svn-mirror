/*
 * ui.c - A (very) simple user interface for MS-DOS.
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

#include <sys/farptr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <go32.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <allegro.h>
#include <math.h>
#include <io.h>

#include "tui.h"
#include "tuiview.h"

#include "ui.h"
#include "types.h"
#include "interrupt.h"
#include "video.h"
#include "serial.h"
#include "patchlevel.h"
#include "mem.h"
#include "tuimenu.h"
#include "sid.h"
#include "vmidas.h"
#include "utils.h"
#include "info.h"
#include "vsync.h"
#include "resources.h"
#include "mon.h"
#include "joystick.h"
#include "drive.h"
#include "tape.h"

#ifdef MOUSE
#include "mouse.h"
#endif

#ifdef HAVE_TRUE1541
#include "true1541.h"
#endif

#ifdef REU
#include "reu.h"
#endif

#ifdef AUTOSTART
#include "autostart.h"
#endif

/* ------------------------------------------------------------------------- */

static tui_menu_t main_menu;
static tui_menu_t attach_submenu;
static tui_menu_t detach_submenu;
static tui_menu_t refresh_rate_submenu;
static tui_menu_t video_submenu;
static tui_menu_t sound_buffer_size_submenu;
static tui_menu_t sound_sample_rate_submenu;
static tui_menu_t sound_submenu;
static tui_menu_t speed_limit_submenu;
static tui_menu_t special_submenu;
static tui_menu_t reset_submenu;
static tui_menu_t quit_submenu;
static tui_menu_t info_submenu;

#ifndef PET
static tui_menu_t resolution_submenu;
#endif

#ifdef HAVE_TRUE1541
static tui_menu_t drive_submenu;
#endif

#ifdef CBM64
static tui_menu_t joystick_submenu;
#endif

/* ------------------------------------------------------------------------- */

static int log_fd = -1;

static void enable_log(int new)
{
    char *path = concat(boot_path, "vice.log", NULL);

    printf("Writing log to `%s'\n", path);

    if (new)
	log_fd = open(path, O_TEXT | O_WRONLY | O_CREAT | O_TRUNC, 0666);
    else
	log_fd = open(path, O_TEXT | O_WRONLY | O_APPEND);
    if (log_fd == -1) {
	perror(path);
	exit(-1);
    }

    free(path);

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    dup2(log_fd, STDOUT_FILENO);
    dup2(log_fd, STDERR_FILENO);
    close(log_fd);
}

static void disable_log(void)
{
    if (log_fd != -1) {
	close(log_fd);
	freopen("CON", "wt", stdout);
	freopen("CON", "wt", stderr);
    }
}

/* ------------------------------------------------------------------------- */

static char *attach_disk_callback(int been_activated, void *param)
{
    char *s;

    if (been_activated) {
	char *name = tui_file_selector("Attach a disk image", "*.[dx]6[4z]",
				       read_disk_image_contents);

	if (name != NULL
	    && serial_select_file(DT_DISK | DT_1541, (int)param, name) < 0) {
	    tui_error("Invalid disk image.");
	    return NULL;
	}
	tui_menu_update(main_menu);
    }

    s = (char *)image_file_name((int)param);
    if (s == NULL || *s == '\0')
	return "(none)";
    else
	return s;
}

#ifdef CBM64
static char *attach_tape_callback(int been_activated, void *param_unused)
{
    char *s;

    if (been_activated) {
	char *name = tui_file_selector("Attach a tape image", "*.t6[4z]",
				       read_tape_image_contents);

	if (name != NULL && serial_select_file(DT_TAPE, 1, name) < 0) {
	    tui_error("Invalid tape image.");
	    return NULL;
	}
	tui_menu_update(main_menu);
    }

    s = (char *)image_file_name(1);
    if (s == NULL || *s == '\0')
	return "(none)";
    else
	return s;
}
#endif

#ifdef AUTOSTART
static char *autostart_callback(int been_activated, void *param)
{
    if (been_activated) {
	if (autostart_device((int)param) < 0)
	    tui_error("Cannot autostart device #%d", (int)param);
    }

    return NULL;
}
#endif

static char *detach_disk_callback(int been_activated, void *param)
{
    char *s;

    if (been_activated) {
	remove_serial((int)param);
	tui_menu_update(main_menu);
    }

    s = (char *)image_file_name((int)param);
    if (s == NULL || *s == '\0')
	return "(none)";
    else
	return s;
}

#ifdef CBM64
static char *detach_tape_callback(int been_activated, void *param_unused)
{
    char *s;

    if (been_activated) {
	remove_serial(1);
	tui_menu_update(main_menu);
    }

    s = (char *)image_file_name(1);
    if (s == NULL || *s == '\0')
	return "(none)";
    else
	return s;
}
#endif

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

#ifndef PET

static char *resolution_submenu_callback(int been_activated,
					 void *param_unused)
{
    return vga_modes[app_resources.vgaMode].description;
}

static char *change_resolution_callback(int been_activated, void *param)
{
    if (been_activated) {
	app_resources.vgaMode = (int)param;
	video_ack_vga_mode();
    }

    return NULL;
}

#endif

static char *refresh_rate_submenu_callback(int been_activated, void *param)
{
    if (app_resources.refreshRate == 0) {
	return "Auto";
    } else {
	static char s[256];

	sprintf(s, "1/%d", app_resources.refreshRate);
	return s;
    }
}

static char *change_refresh_rate_callback(int been_activated, void *param)
{
    if (been_activated)
	app_resources.refreshRate = (int)param;

    return NULL;
}

static char *toggle_video_cache_callback(int been_activated, void *param_unused)
{
    if (been_activated)
	app_resources.videoCache = !app_resources.videoCache;
    return app_resources.videoCache ? "On" : "Off";
}

#ifdef CBM64
static char *toggle_ss_collisions_callback(int been_activated,
					   void *param_unused)
{
    if (been_activated)
	app_resources.checkSsColl = !app_resources.checkSsColl;
    return app_resources.checkSsColl ? "On" : "Off";
}

static char *toggle_sb_collisions_callback(int been_activated,
					   void *param_unused)
{
    if (been_activated)
	app_resources.checkSbColl = !app_resources.checkSbColl;
    return app_resources.checkSbColl ? "On" : "Off";
}
#endif

#ifdef HAVE_TRUE1541

static char *toggle_true1541_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
	app_resources.true1541 = !app_resources.true1541;
	true1541_ack_switch();
    }
    return app_resources.true1541 ? "On" : "Off";
}

static char *change_true1541_sync_factor_callback(int been_activated,
						  void *param_unused)
{
    if (been_activated) {
	if (app_resources.true1541SyncFactor == TRUE1541_PAL_SYNC_FACTOR)
	    app_resources.true1541SyncFactor = TRUE1541_NTSC_SYNC_FACTOR;
	else
	    app_resources.true1541SyncFactor = TRUE1541_PAL_SYNC_FACTOR;
    }

    switch (app_resources.true1541SyncFactor) {
      case TRUE1541_PAL_SYNC_FACTOR:
	return "PAL";
      case TRUE1541_NTSC_SYNC_FACTOR:
	return "NTSC";
      default:
	return "(Custom)";
    }
}

static char *change_true1541_idle_method_callback(int been_activated,
						  void *param_unused)
{
    if (been_activated) {
	if (app_resources.true1541IdleMethod == TRUE1541_IDLE_SKIP_CYCLES)
	    app_resources.true1541IdleMethod = TRUE1541_IDLE_TRAP_IDLE;
	else
	    app_resources.true1541IdleMethod = TRUE1541_IDLE_SKIP_CYCLES;
    }

    switch (app_resources.true1541IdleMethod) {
      case TRUE1541_IDLE_TRAP_IDLE:
	return "Trap idle";
      case TRUE1541_IDLE_SKIP_CYCLES:
	return "Skip cycles";
      default:
	return "(Unknown)";
    }
}

#ifdef CBM64
static char *toggle_true1541_parallel_cable_callback(int been_activated,
						     void *param_unused)
{
    if (been_activated)
	app_resources.true1541ParallelCable =
	    !app_resources.true1541ParallelCable;

    return app_resources.true1541ParallelCable ? "On" : "Off";
}
#endif

#endif

static char *toggle_sound_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
	app_resources.sound = !app_resources.sound;

	if (!app_resources.sound) {
	    close_sound();
	}

	tui_menu_update(main_menu);
    }

    return app_resources.sound ? "On" : "Off";
}

#ifdef CBM64

static char *toggle_sid_filters_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
	app_resources.sidFilters = !app_resources.sidFilters;
	close_sound();
	tui_menu_update(main_menu);
    }

    return app_resources.sidFilters ? "On" : "Off";
}

static char *toggle_sid_model_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
	app_resources.sidModel = !app_resources.sidModel;
	close_sound();
	tui_menu_update(main_menu);
    }

    return app_resources.sidModel ? "8580 (New)" : "6581 (Old)";
}

#endif

static char *sound_sample_rate_submenu_callback(int been_activated, void *param)
{
    static char s[256];

    sprintf(s, "%d Hz", app_resources.soundSampleRate);
    return s;
}

static char *change_sound_sample_rate_callback(int been_activated, void *param)
{
    if (been_activated) {
	app_resources.soundSampleRate = (int)param;
	close_sound();
    }

    return NULL;
}

static char *sound_buffer_size_submenu_callback(int been_activated, void *param)
{
    static char s[256];

    sprintf(s, "%d msec", app_resources.soundBufferSize);
    return s;
}

static char *change_sound_buffer_size_callback(int been_activated, void *param)
{
    if (been_activated) {
	app_resources.soundBufferSize = (int)param;
	close_sound();
    }

    return NULL;
}

#if 0
/* FIXME: This is currently not working... */
static char *toggle_sound_adjustment_callback(int been_activated,
					      void *param_unused)
{
    if (been_activated) {
	app_resources.soundSpeedAdjustment =
	    !app_resources.soundSpeedAdjustment;
    }

    return app_resources.soundSpeedAdjustment ? "On" : "Off";
}
#endif

#if 0
/* FIXME: This is currently not working... */
static char *sound_card_setup_callback(int been_activated,
				       void *param_unused)
{
    if (been_activated) {
	tui_area_t backing_store = NULL;

	tui_area_get(&backing_store, 0, 0, tui_num_cols(), tui_num_lines());
        vmidas_config();
        vmidas_init();
        close_sound();
	/* This is annoying... */
	_setcursortype(_NOCURSOR);
	tui_area_put(backing_store, 0, 0);
	tui_area_free(backing_store);
    }

    return NULL;
}
#endif

static char *speed_limit_submenu_callback(int been_activated,
					  void *param_unused)
{
    static char s[1024];

    if (app_resources.speed != 0) {
	sprintf(s, "%d%%", app_resources.speed);
	return s;
    } else
	return "None";
}

static char *speed_limit_callback(int been_activated, void *param)
{
    if (been_activated) {
	if ((int)param < 0) {
	    char buf[25];

	    if (tui_input_string("Maximum Speed",
				 "Enter maximum speed (%%):",
				 buf, 25) == 0) {
		app_resources.speed = atoi(buf);
		if (app_resources.speed > 1000)
		    app_resources.speed = 1000;
		else if (app_resources.speed < 0)
		    app_resources.speed = 0;
	    }
	} else {
	    app_resources.speed = (int)param;
	}
    }
    return NULL;
}

#ifdef CBM64

static char *toggle_reu_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
	app_resources.reu = !app_resources.reu;
	/* The REU and the IEEE488 interface share the same address space, so
	   they cannot be enabled at the same time. */
	if (app_resources.reu) {
	    app_resources.ieee488 = 0;
	    activate_reu();
	}
	UiUpdateMenus();
    }

    return app_resources.reu ? "On" : "Off";
}

static char *toggle_emulator_id_callback(int been_activated, void *param_unused)
{
    if (been_activated)
	app_resources.emuID = !app_resources.emuID;

    return app_resources.emuID ? "On" : "Off";
}
#endif

#if 0
static char *toggle_mouse_callback(int been_activated, void *param_unused)
{
    if (been_activated)
	app_resources.mouse = !app_resources.mouse;

    return app_resources.mouse ? "On" : "Off";
}
#endif

#ifdef CBM64
static char *toggle_ieee488_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
        app_resources.ieee488 = !app_resources.ieee488;
	/* The REU and the IEEE488 interface share the same address space, so
	   they cannot be enabled at the same time. */
	if (app_resources.ieee488)
	    app_resources.reu = 0;
	UiUpdateMenus();
    }

    return app_resources.ieee488 ? "On" : "Off";
}

static char *toggle_joystick_callback(int been_activated, void *param)
{
    if (been_activated) {
	if (app_resources.joyPort == (int)param)
	    app_resources.joyPort = (int)param == 1 ? 2 : 1;
	else
	    app_resources.joyPort = (int)param;
	tui_menu_update(main_menu);
    }

    if (app_resources.joyPort == (int)param) {
	return "Numpad + RightCtrl";
    } else {
	return joystick_available ? "Joystick" : "None";
    }
}
#endif

static char *save_settings_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
	if (resources_save(NULL, EMULATOR) < 0)
	    tui_error("Cannot save settings.");
	else
	    tui_message("Settings saved successfully.");
    }

    return NULL;
}

static char *load_settings_callback(int been_activated, void *param_unused)
{
    if (been_activated) {
	if (resources_load(NULL, EMULATOR) < 0)
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
	tui_menu_update(main_menu);
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
	disable_log();
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

static void mon_trap(ADDRESS addr)
{
    int old_mode;

    disable_log();
    enable_text();
    clrscr();
    _set_screen_lines(43);
    _setcursortype(_SOLIDCURSOR);
    old_mode = setmode(STDIN_FILENO, O_TEXT);

    mon(addr);

    setmode(STDIN_FILENO, old_mode);
    disable_text();
    enable_log(0);
}

static char *monitor_callback(int been_activated, void *param_unused)
{
    if (been_activated)
	maincpu_trigger_trap(mon_trap);

    return NULL;
}

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
	    "Copyright (c) 1993-1998",
	    "E. Perazzoli, T. Rantanen, A. Fachat,",
	    "J. Valta, D. Sladic and J. Sonninen", "",
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

/* ------------------------------------------------------------------------- */

static void UiExit(void)
{
    _setcursortype(_NORMALCURSOR);
    normvideo();
}

int UiInit(int *argc, char **argv)
{
    /* This forces Ctrl-C and Ctrl-Break to be treated as normal key
       sequences. */
    _go32_want_ctrl_break(1);

    return 0;
}

int UiInitFinish(void)
{
    enable_log(1);
    tui_init();
    atexit(UiExit);

    attach_submenu = tui_menu_create("Attach Images", 1);
    tui_menu_add_item(attach_submenu, "Drive #_8:",
		      "Attach disk image for disk drive #8",
		      attach_disk_callback, (void *)8, 30,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(attach_submenu, "Drive #_9:",
		      "Attach disk image for disk drive #9",
		      attach_disk_callback, (void *)9, 30,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(attach_submenu, "Drive #1_0:",
		      "Attach disk image for disk drive #10",
		      attach_disk_callback, (void *)10, 30,
		      TUI_MENU_BEH_CONTINUE);

#ifdef CBM64
    tui_menu_add_separator(attach_submenu);
    tui_menu_add_item(attach_submenu,"Tape #_1:",
		      "Attach tape image for cassette player (device #1)",
		      attach_tape_callback, NULL, 30,
		      TUI_MENU_BEH_CONTINUE);
#endif

#ifdef AUTOSTART

    tui_menu_add_separator(attach_submenu);
    tui_menu_add_item(attach_submenu, "Autostart _Drive 8",
		      "Reset the emulator and run the first program in the disk in drive 8",
		      autostart_callback, (void *)8, 0,
		      TUI_MENU_BEH_RESUME);

#ifdef CBM64
    tui_menu_add_item(attach_submenu, "Autostart _Tape",
		      "Reset the emulator and run the first program on the tape image",
		      autostart_callback, (void *)1, 0,
		      TUI_MENU_BEH_RESUME);
#endif

#endif /* AUTOSTART */

    detach_submenu = tui_menu_create("Detach Images", 1);
    tui_menu_add_item(detach_submenu, "Drive #_8:",
		      "Remove disk from disk drive #8",
		      detach_disk_callback, (void *)8, 30,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(detach_submenu,"Drive #_9:",
		      "Remove disk from disk drive #9",
		      detach_disk_callback, (void *)9, 30,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(detach_submenu, "Drive #1_0:",
		      "Remove disk from disk drive #10",
		      detach_disk_callback, (void *)10, 30,
		      TUI_MENU_BEH_CONTINUE);

#ifdef CBM64
    tui_menu_add_separator(detach_submenu);
    tui_menu_add_item(detach_submenu, "Tape #_1:",
		      "Remove tape from cassette player (device #1)",
		      detach_tape_callback, NULL, 30,
		      TUI_MENU_BEH_CONTINUE);
#endif

#ifdef CBM64
    joystick_submenu = tui_menu_create("Joystick Settings", 1);
    tui_menu_add_item(joystick_submenu, "Joystick in port _1:",
		      "Choose joystick for port 1",
		      toggle_joystick_callback, (void *)1, 18,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(joystick_submenu, "Joystick in port _2:",
		      "Choose joystick for port 2",
		      toggle_joystick_callback, (void *)2, 18,
		      TUI_MENU_BEH_CONTINUE);
#endif

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
			      change_refresh_rate_callback, (void *)i, 0,
			      TUI_MENU_BEH_CLOSE);
	}
	tui_menu_add_separator(refresh_rate_submenu);
	tui_menu_add_item(refresh_rate_submenu, "_Automatic",
			  "Let the emulator select an appropriate refresh rate automagically",
			  change_refresh_rate_callback, NULL, 0,
			  TUI_MENU_BEH_CLOSE);
    }

#ifndef PET
    resolution_submenu = tui_menu_create("VGA Resolution", 1);
    {
	int i;

	for (i = 0; i < NUM_VGA_MODES; i++) {
	    char s1[256], s2[256];

	    /* FIXME: hotkeys work only for less than 11 elements. */
	    sprintf(s1, "Mode _%d: %s", i, vga_modes[i].description);
	    sprintf(s2, "Set VGA resolution to %s", vga_modes[i].description);
	    tui_menu_add_item(resolution_submenu, s1, s2,
			      change_resolution_callback, (void *)i, 0,
			      TUI_MENU_BEH_CLOSE);
	}
    }
#endif

    video_submenu = tui_menu_create("Video Settings", 1);

#ifndef PET
    tui_menu_add_submenu(video_submenu,"_VGA Resolution:",
			 "Choose screen resolution for video emulation",
			 resolution_submenu,
			 resolution_submenu_callback, NULL, 8);
#endif

    tui_menu_add_submenu(video_submenu, "_Refresh Rate:",
			 "Choose frequency of screen refresh",
			 refresh_rate_submenu,
			 refresh_rate_submenu_callback, NULL, 4);
    tui_menu_add_item(video_submenu, "Video _Cache:",
		      "Enable screen cache",
		      toggle_video_cache_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);

#ifdef CBM64
    tui_menu_add_separator(video_submenu);
    tui_menu_add_item(video_submenu, "Sprite-_Background Collisions:",
		      "Emulate sprite-background collision register",
		      toggle_sb_collisions_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(video_submenu, "Sprite-_Sprite Collisions:",
		      "Emulate sprite-sprite collision register",
		      toggle_ss_collisions_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);
#endif

#ifdef HAVE_TRUE1541
    drive_submenu = tui_menu_create("Drive Settings", 1);
    tui_menu_add_item(drive_submenu, "True 1541 _Emulation:",
		      "Enable hardware-level floppy drive emulation",
		      toggle_true1541_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(drive_submenu, "True 1541 _Sync Factor:",
		      "Select 1541 vs. " EMULATOR " clock ratio",
		      change_true1541_sync_factor_callback, NULL, 8,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(drive_submenu, "True 1541 _Idle Method:",
		      "Select method for disk drive idle",
		      change_true1541_idle_method_callback, NULL, 11,
		      TUI_MENU_BEH_CONTINUE);
#ifdef CBM64
    tui_menu_add_item(drive_submenu, "Enable _Parallel Cable:",
		      "Enable a SpeedDOS-compatible parallel cable.",
		      toggle_true1541_parallel_cable_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);
#endif
	
#endif /* HAVE_TRUE1541 */

    sound_submenu = tui_menu_create("Audio Settings", 1);
    tui_menu_add_item(sound_submenu, "Sound _Playback:",
		      "Enable sound output",
		      toggle_sound_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);

#ifdef CBM64
    tui_menu_add_item(sound_submenu, "Emulate SID _Filters:",
		      "Enable emulation of the built-in filters of the SID chip",
		      toggle_sid_filters_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(sound_submenu, "SID _Model:          ",
		      "Select the SID model to emulate",
		      toggle_sid_model_callback, NULL, 10,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_separator(sound_submenu);
#endif /* CBM64 */

    sound_sample_rate_submenu = tui_menu_create("Sample rate", 1);
    {
	static int rates[] = { 8000, 11025, 22050, 44100, 0 };
	int i;

	for (i = 0; rates[i] != 0; i++) {
	    char s1[256], s2[256];

	    sprintf(s1, "%d Hz", rates[i]);
	    sprintf(s2, "Set sampling rate to %d Hz", rates[i]);
	    tui_menu_add_item(sound_sample_rate_submenu, s1, s2,
			      change_sound_sample_rate_callback,
			      (void *)rates[i], 0,
			      TUI_MENU_BEH_CLOSE);
	}
    }

    tui_menu_add_submenu(sound_submenu, "_Sample Frequency:",
			 "Choose sound output sampling rate",
			 sound_sample_rate_submenu,
			 sound_sample_rate_submenu_callback, NULL, 8);

    sound_buffer_size_submenu = tui_menu_create("Latency", 1);
    {
	static int values[] = { 50, 75, 100, 150, 200, 250, 300,
				350, 500, 750, 1000, 0 };
	int i;

	for (i = 0; values[i] != 0; i++) {
	    char s1[256], s2[256];

	    sprintf(s1, "%d msec", values[i]);
	    sprintf(s2, "Set sound buffer size to %d msec", values[i]);
	    tui_menu_add_item(sound_buffer_size_submenu, s1, s2,
			      change_sound_buffer_size_callback,
			      (void *)values[i], 0,
			      TUI_MENU_BEH_CLOSE);
	}
    }
    tui_menu_add_submenu(sound_submenu, "Sound _Buffer Size:",
			 "Specify playback latency",
			 sound_buffer_size_submenu,
			 sound_buffer_size_submenu_callback, NULL, 9);

#if 0
    /* FIXME: This stuff is currently not working... */
    tui_menu_add_item(sound_submenu, "Speed _Adjustment:",
		      "Adapt sample speed to emulation speed to avoid clicks",
		      toggle_sound_adjustment_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_separator(sound_submenu);
    tui_menu_add_item(sound_submenu, "Sound _Card Setup...",
    		      "Run MIDAS sound driver setup",
    		      sound_card_setup_callback, NULL, 0,
    		      TUI_MENU_BEH_CONTINUE);
#endif

    speed_limit_submenu = tui_menu_create("Speed Limit", 1);
    tui_menu_add_item(speed_limit_submenu, "_100%",
		      "Limit speed to the one of the real " EMULATOR,
		      speed_limit_callback, (void *)100, 5,
		      TUI_MENU_BEH_CLOSE);
    tui_menu_add_item(speed_limit_submenu, "_50%",
		      "Limit relative speed to 50% of the real " EMULATOR,
		      speed_limit_callback, (void *)50, 5,
		      TUI_MENU_BEH_CLOSE);
    tui_menu_add_item(speed_limit_submenu, "_20%",
		      "Limit relative speed to 20% of the real " EMULATOR,
		      speed_limit_callback, (void *)20, 5,
		      TUI_MENU_BEH_CLOSE);
    tui_menu_add_item(speed_limit_submenu, "_10%",
		      "Limit relative speed to 10% of the real " EMULATOR,
		      speed_limit_callback, (void *)10, 5,
		      TUI_MENU_BEH_CLOSE);
    tui_menu_add_item(speed_limit_submenu, "_No Limit",
		      "Run the emulator as fast as possible",
		      speed_limit_callback, (void *)0, 5,
		      TUI_MENU_BEH_CLOSE);
    tui_menu_add_separator(speed_limit_submenu);
    tui_menu_add_item(speed_limit_submenu, "_Custom...",
		      "Specify a custom relative speed value",
		      speed_limit_callback, (void *)-1, 5,
		      TUI_MENU_BEH_CLOSE);

    special_submenu = tui_menu_create("Special Features", 1);
    tui_menu_add_submenu(special_submenu, "_Speed Limit:",
			 "Specify a custom speed limit",
			 speed_limit_submenu, speed_limit_submenu_callback,
			 NULL, 5);

#ifdef REU
    tui_menu_add_separator(special_submenu);
    tui_menu_add_item(special_submenu, "512K _RAM Expansion Unit (C1750):",
		      "Emulate auxiliary 512K RAM Expansion Unit",
		      toggle_reu_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);
#endif

#if 0
    tui_menu_add_item(special_submenu, "_Mouse emulation:",
		      "Emulate a Commodore 1351 proportional mouse",
		      toggle_mouse_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);
#endif

#ifdef CBM64
    tui_menu_add_item(special_submenu, "_IEEE488 Interface Emulation:",
		      "Emulate external IEEE488 interface",
		      toggle_ieee488_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(special_submenu, "_Emulator Identification:",
		      "Allow programs to identify the emulator they are running on",
		      toggle_emulator_id_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);
#endif

    reset_submenu = tui_menu_create("Reset?", 1);
    tui_menu_add_item(reset_submenu, "_Not Really!",
		      "Go back to the menu",
		      NULL, NULL, 0,
		      TUI_MENU_BEH_CLOSE);
    tui_menu_add_item(reset_submenu, "Do a _Soft Reset",
		      "Do a soft reset without resetting the memory",
		      soft_reset_callback, NULL, 0,
		      TUI_MENU_BEH_RESUME);
    tui_menu_add_item(reset_submenu, "Do a _Hard Reset",
		      "Clear memory and reset as after a power-up",
		      hard_reset_callback, NULL, 0,
		      TUI_MENU_BEH_RESUME);

    quit_submenu = tui_menu_create("Quit?", 1);
    tui_menu_add_item(quit_submenu, "_Not really!",
		      "Go back to the menu",
		      NULL, NULL, 0,
		      TUI_MENU_BEH_CLOSE);
    tui_menu_add_item(quit_submenu, "_Yes, exit " EMULATOR " emulator",
		      "Leave emulator completely",
		      quit_callback, NULL, 0,
		      TUI_MENU_BEH_RESUME);

    info_submenu = tui_menu_create("Info", 1);
    tui_menu_add_item(info_submenu, "_Copyright",
		      "VICE copyright information",
		      show_copyright_callback, NULL, 0,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(info_submenu, "_Who made what?",
		      "VICE contributors",
		      show_info_callback, (void *)contrib_text, 0,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(info_submenu, "_License",
		      "VICE license (GNU General Public License)",
		      show_info_callback, (void *)license_text, 0,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(info_submenu, "_No warranty!",
		      "VICE is distributed WITHOUT ANY WARRANTY!",
		      show_info_callback, (void *)warranty_text, 0,
		      TUI_MENU_BEH_CONTINUE);

    /* Main menu. */
    main_menu = tui_menu_create(NULL, 1);

#ifdef CBM64
    tui_menu_add_submenu(main_menu, "_Attach Disk/Tape Image...",
			 "Specify disk/tape images for virtual floppy/cassette drives",
			 attach_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);
    tui_menu_add_submenu(main_menu, "_Detach Disk/Tape Image...",
			 "Remove disks/tapes from virtual drives",
			 detach_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);
#else
    tui_menu_add_submenu(main_menu, "_Attach Disk Image...",
			 "Specify disk images for virtual floppy drives",
			 attach_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);
    tui_menu_add_submenu(main_menu, "_Detach Disk Image...",
			 "Remove disks from virtual drives",
			 detach_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);
#endif

    tui_menu_add_item(main_menu, "_Change Working Directory...",
                      "Change the current working directory",
                      change_workdir_callback, NULL, 0,
                      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_separator(main_menu);

#ifdef CBM64
    tui_menu_add_submenu(main_menu, "_Joystick Settings...",
			 "Setup joysticks",
			 joystick_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);
#endif

    tui_menu_add_submenu(main_menu, "_Video Settings...",
			 "Screen parameters",
			 video_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);

#ifdef HAVE_TRUE1541
    tui_menu_add_submenu(main_menu, "_1541 Settings...",
			 "Change disk-drive emulation parameters",
			 drive_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);
#endif

    tui_menu_add_submenu(main_menu, "_Sound Settings...",
			 "Sampling rate, sound output, soundcard settings",
			 sound_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);
    tui_menu_add_submenu(main_menu, "Special _Options...",
			 "REU, IEEE488, custom speed, emulation identification",
			 special_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);
    tui_menu_add_separator(main_menu);

    tui_menu_add_item(main_menu, "_Write Settings",
		      "Save current settings as default for next session",
		      save_settings_callback, NULL, 0,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(main_menu, "_Load Settings",
		      "Load saved settings from previous session",
		      load_settings_callback, NULL, 0,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(main_menu, "Restore _Factory Defaults",
		      "Set default settings",
		      restore_default_settings_callback, NULL, 0,
		      TUI_MENU_BEH_CONTINUE);

    tui_menu_add_separator(main_menu);

    tui_menu_add_item(main_menu, "_Monitor",
		      "Enter the built-in machine language monitor",
		      monitor_callback, NULL, 0,
		      TUI_MENU_BEH_RESUME);
    tui_menu_add_submenu(main_menu, "_Reset " EMULATOR,
			 "Reset the machine",
			 reset_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);
    tui_menu_add_submenu(main_menu, "_Quit",
			 "Quit " EMULATOR " emulator",
			 quit_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);
    tui_menu_add_separator(main_menu);
    tui_menu_add_submenu(main_menu, "VICE _Info...",
			 "VICE is Free Software distributed under the GNU General Public License!",
			 info_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);

    return 0;
}

void UiMain(ADDRESS addr)
{
    char s[256];
    double speed_index, frame_rate;
    int old_stdin_mode = setmode(STDIN_FILENO, O_BINARY);

    speed_index = vsync_get_avg_speed_index();
    frame_rate = vsync_get_avg_frame_rate();

    if (speed_index > 0.0 && frame_rate > 0.0)
	sprintf(s, "%s emulator at %d%% speed, %d fps",
		EMULATOR, (int)floor(speed_index), (int)floor(frame_rate));
    else
	strcpy(s, EMULATOR " emulator");

    enable_text();

    tui_clear_screen();
    tui_set_attr(FIRST_LINE_FORE, FIRST_LINE_BACK, 0);
    tui_display(tui_num_cols() - strlen(s), 0, 0, "%s", s);

    tui_menu_update(main_menu);
    tui_menu_handle(main_menu);

    /* FIXME: This is an ugly kludge.  I really think this should be handled
       transparently, i.e. without messing with the resource values, in the
       sound driver. */
    if (app_resources.sound) {
	/* With the current version of the sound driver, we must always
	   force 100% speed (no automatic sound speed adjustment is
	   implemented for MS-DOS). */
	app_resources.speed = 100;
    }

    disable_text();
    suspend_speed_eval();

    setmode(STDIN_FILENO, old_stdin_mode);
}

void UiError(const char *format,...)
{
    char tmp[1024];
    va_list ap;

    enable_text();
    tui_clear_screen();

    va_start(ap, format);
    vsprintf(tmp, format, ap);
    tui_error(tmp);

    disable_text();
}

int UiJamDialog(const char *format,...)
{
    char tmp[1024];
    va_list ap;

    enable_text();
    tui_clear_screen();

    va_start(ap, format);
    vsprintf(tmp, format, ap);
    tui_error(tmp);

    disable_text();

    /* Always reset. */
    return 0;
}

void UiShowText(const char *title, const char *text)
{
    /* Needs to be done... */
}

void UiUpdateMenus(void)
{
    if (main_menu != NULL)
        tui_menu_update(main_menu);
}

void UiAutoRepeatOn(void)
{
}

void UiAutoRepeatOff(void)
{
}

