/*
 * menudefs.c - Definition of menu commands and settings.
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

#include <dir.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <unistd.h>

#include "menudefs.h"

#include "datasette.h"
#include "grabkey.h"
#include "imagecontents.h"
#include "joystick.h"
#include "kbd.h"
#include "log.h"
#include "maincpu.h"
#include "romset.h"
#include "sound.h"
#include "tape.h"
#include "tui.h"
#include "tuiview.h"
#include "ui.h"
#include "uisnapshot.h"

/* FIXME: Argh!!  Megakludge!  <dir.h> #defines `DRIVE', which we need in
   "vdrive.h".  */
#undef DRIVE

#include "attach.h"
#include "autostart.h"
#include "vdrive.h"
#include "fliplist.h"
#include "fsdevice.h"
#include "info.h"
#include "machine.h"
#include "serial.h"
#include "drive.h"
#include "utils.h"
#include "video.h"

/* ------------------------------------------------------------------------- */

tui_menu_t ui_attach_submenu;
tui_menu_t ui_detach_submenu;
tui_menu_t ui_datasette_submenu;
tui_menu_t ui_drive_submenu;
tui_menu_t ui_flip_submenu;
tui_menu_t ui_info_submenu;
tui_menu_t ui_joystick_settings_submenu;
tui_menu_t ui_main_menu;
tui_menu_t ui_quit_submenu;
tui_menu_t ui_reset_submenu;
tui_menu_t ui_rom_submenu;
tui_menu_t ui_sound_buffer_size_submenu;
tui_menu_t ui_sound_sample_rate_submenu;
tui_menu_t ui_sound_submenu;
tui_menu_t ui_special_submenu;
tui_menu_t ui_snapshot_submenu;
tui_menu_t ui_video_submenu;
tui_menu_t ui_settings_submenu;

/* ------------------------------------------------------------------------ */

static TUI_MENU_CALLBACK(attach_disk_callback)
{
    char *s;

    if (been_activated) {
        char *default_item, *directory;
        char *name, *file;

        s = (char *)serial_get_file_name((int)param);
        fname_split(s, &directory, &default_item);

        name = tui_file_selector("Attach a disk image", directory,
                                 "*.d64;*.d71;*.d81;*.g64;*.g41;*.x64",
                                 default_item, image_contents_read_disk, &file);

        if (file != NULL) {
            if (autostart_disk(name, file) < 0)
                tui_error("Cannot autostart disk image.");
            else
                *behavior = TUI_MENU_BEH_RESUME;
            free(file);
        } else	if (name != NULL
                    && (s == NULL || strcasecmp(name, s) != 0)
                    && file_system_attach_disk((int)param, name) < 0) {
            tui_error("Invalid disk image.");
        }

        ui_update_menus();

        free(directory), free(default_item);
        if (name != NULL)
            free(name);
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
	char *name, *file;

	s = tape_get_file_name();
	fname_split(s, &directory, &default_item);

	name = tui_file_selector("Attach a tape image", directory,
				 "*.t64;*.tap", default_item,
				 image_contents_read_tape, &file);

        if (file != NULL) {
            if (autostart_tape(name, file) < 0)
                tui_error("Cannot autostart tape image.");
            else
                *behavior = TUI_MENU_BEH_RESUME;
            free(file);
        } else if (name != NULL
                   && (s == NULL || strcasecmp(s, name) != 0)
                   && tape_attach_image(name) < 0) {
	    tui_error("Invalid tape image.");
	}
	ui_update_menus();
	free(directory), free(default_item);
        if (name != NULL)
            free(name);
    }

    s = tape_get_file_name();
    if (s == NULL || *s == '\0')
	return "(none)";
    else
	return s;
}

static TUI_MENU_CALLBACK(datasette_callback)
{
    if (been_activated)
        datasette_control((int)param);
    return NULL;
}

static TUI_MENU_CALLBACK(autostart_callback)
{
    if (been_activated) {
	if (autostart_device((int)param) < 0)
	    tui_error("Cannot autostart device #%d", (int)param);
    }

    return NULL;
}

static TUI_MENU_CALLBACK(detach_disk_callback)
{
    char *s;

    if (been_activated) {
	file_system_detach_disk((int)param);
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
	tape_detach_image();
	ui_update_menus();
    }

    s = tape_get_file_name();
    if (s == NULL || *s == '\0')
	return "(none)";
    else
	return s;
}

static TUI_MENU_CALLBACK(flip_add_callback)
{
    if (been_activated)
        flip_add_image();
    return NULL;
}

static TUI_MENU_CALLBACK(flip_remove_callback)
{
    if (been_activated)
        flip_remove(-1, NULL);
    return NULL;
}

static TUI_MENU_CALLBACK(flip_next_callback)
{
    if (been_activated)
        flip_attach_head(1);
    return NULL;
}

static TUI_MENU_CALLBACK(flip_previous_callback)
{
    if (been_activated)
        flip_attach_head(0);
    return NULL;
}

/* ------------------------------------------------------------------------ */

static TUI_MENU_CALLBACK(change_workdir_callback)
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

#ifndef USE_MIDAS_SOUND
TUI_MENU_DEFINE_TOGGLE(TripleBuffering)
#endif

/* ------------------------------------------------------------------------- */

TUI_MENU_DEFINE_RADIO(Drive8ExtendImagePolicy)
TUI_MENU_DEFINE_RADIO(Drive9ExtendImagePolicy)

static TUI_MENU_CALLBACK(drive_extend_image_policy_submenu_callback)
{
    int unit = (int) param;
    char rname[256];
    int v;

    sprintf(rname, "Drive%dExtendImagePolicy", unit);
    resources_get_value(rname, (resource_value_t *) &v);

    switch (v) {
      case DRIVE_EXTEND_NEVER:
        return "Never extend";
      case DRIVE_EXTEND_ASK:
        return "Ask on extend";
      case DRIVE_EXTEND_ACCESS:
        return "Extend on access";
      default:
        return "Unknown";
    }
}

#define DEFINE_DRIVE_EXTEND_IMAGE_POLICY_SUBMENU(num)                           \
static tui_menu_item_def_t drive##num##_extend_image_policy_submenu[] = {       \
    { "_Never extend",                                                          \
      "Never create more than 35 tracks",                                       \
      radio_Drive##num##ExtendImagePolicy_callback,                             \
      (void *) DRIVE_EXTEND_NEVER, 0,                                           \
      TUI_MENU_BEH_CLOSE, NULL, NULL },                                         \
    { "_Ask on extend",                                                         \
      "Ask the user before creating extra tracks",                              \
      radio_Drive##num##ExtendImagePolicy_callback,                             \
      (void *) DRIVE_EXTEND_ASK, 0,                                             \
      TUI_MENU_BEH_CLOSE, NULL, NULL },                                         \
    { "_Extend on access",                                                      \
      "Automagically extend the disk image if extra (>35) tracks are accessed", \
      radio_Drive##num##ExtendImagePolicy_callback,                             \
      (void *) DRIVE_EXTEND_ACCESS, 0,                                          \
      TUI_MENU_BEH_CLOSE, NULL, NULL },                                         \
    { NULL }                                                                    \
};

DEFINE_DRIVE_EXTEND_IMAGE_POLICY_SUBMENU(8)
DEFINE_DRIVE_EXTEND_IMAGE_POLICY_SUBMENU(9)

TUI_MENU_DEFINE_TOGGLE(DriveTrueEmulation)

static TUI_MENU_CALLBACK(toggle_DriveSyncFactor_callback)
{
    int value;

    resources_get_value("DriveSyncFactor", (resource_value_t *) &value);

    if (been_activated) {
	if (value == DRIVE_SYNC_PAL)
	    value = DRIVE_SYNC_NTSC;
	else
	    value = DRIVE_SYNC_PAL;
        resources_set_value("DriveSyncFactor", (resource_value_t) value);
    }

    switch (value) {
      case DRIVE_SYNC_PAL:
	return "PAL";
      case DRIVE_SYNC_NTSC:
	return "NTSC";
      default:
	return "(Custom)";
    }
}

TUI_MENU_DEFINE_RADIO(Drive8Type)
TUI_MENU_DEFINE_RADIO(Drive9Type)

static TUI_MENU_CALLBACK(drive_type_submenu_callback)
{
    int value;

    if ((int) param == 8)
        resources_get_value("Drive8Type", (resource_value_t *) &value);
    else
        resources_get_value("Drive9Type", (resource_value_t *) &value);

    switch (value) {
      case 0:
        return "None";
      case DRIVE_TYPE_1541:
        return "1541, 5\"1/4 SS";
      case DRIVE_TYPE_1541II:
        return "1541-II, 5\"1/4 SS";
      case DRIVE_TYPE_1571:
        return "1571, 5\"1/4 DS";
      case DRIVE_TYPE_1581:
        return "1581, 3\"1/2 DS";
      case DRIVE_TYPE_2031:
        return "2031, 5\"1/4 SS, IEEE488";
      default:
	return "(Unknown)";
    }
}

#define DEFINE_DRIVE_MODEL_SUBMENU(num)                                   \
static tui_menu_item_def_t drive##num##_type_submenu[] = {                \
    { "_None",                                                            \
      "Disable hardware-level emulation of drive #" #num,                 \
      radio_Drive##num##Type_callback, (void *) 0, 0,                     \
      TUI_MENU_BEH_CLOSE, NULL, NULL },                                   \
    { "_1541, 5\"1/4 SS",                                                 \
      "Emulate a 1541 5\"1/4 single-sided disk drive as unit #" #num,     \
      radio_Drive##num##Type_callback, (void *) DRIVE_TYPE_1541, 0,       \
      TUI_MENU_BEH_CLOSE, NULL, NULL },                                   \
    { "1541-_II, 5\"1/4 SS",                                              \
      "Emulate a 1541-II 5\"1/4 single-sided disk drive as unit #" #num,  \
      radio_Drive##num##Type_callback, (void *) DRIVE_TYPE_1541II, 0,     \
      TUI_MENU_BEH_CLOSE, NULL, NULL },                                   \
    { "15_71, 5\"1/4 DS",                                                 \
      "Emulate a 1571 5\"1/4 double-sided disk drive as unit #" #num,     \
      radio_Drive##num##Type_callback, (void *) DRIVE_TYPE_1571, 0,       \
      TUI_MENU_BEH_CLOSE, NULL, NULL },                                   \
    { "15_81, 3\"1/2 DS",                                                 \
      "Emulate a 1581 3\"1/2 double-sided disk drive as unit #" #num,     \
      radio_Drive##num##Type_callback, (void *) DRIVE_TYPE_1581, 0,       \
      TUI_MENU_BEH_CLOSE, NULL, NULL },                                   \
    { "_2031, 5\"1/4 SS IEEE488",                                         \
      "Emulate a 2031 5\"1/4 single-sided IEEE disk drive as unit #" #num,\
      radio_Drive##num##Type_callback, (void *) DRIVE_TYPE_2031, 0,       \
      TUI_MENU_BEH_CLOSE, NULL, NULL },                                   \
    { NULL }                                                              \
};

DEFINE_DRIVE_MODEL_SUBMENU(8)
DEFINE_DRIVE_MODEL_SUBMENU(9)

TUI_MENU_DEFINE_RADIO(Drive8IdleMethod)
TUI_MENU_DEFINE_RADIO(Drive9IdleMethod)

static TUI_MENU_CALLBACK(drive_idle_method_submenu_callback)
{
    int value;

    if ((int) param == 8)
        resources_get_value("Drive8IdleMethod", (resource_value_t *) &value);
    else
        resources_get_value("Drive9IdleMethod", (resource_value_t *) &value);

    switch (value) {
      case DRIVE_IDLE_NO_IDLE:
        return "None";
      case DRIVE_IDLE_TRAP_IDLE:
	return "Trap idle";
      case DRIVE_IDLE_SKIP_CYCLES:
	return "Skip cycles";
      default:
	return "(Unknown)";
    }
}

#define DEFINE_DRIVE_IDLE_METHOD_SUBMENU(num)                             \
static tui_menu_item_def_t drive##num##_idle_method_submenu[] = {         \
    { "_None",                                                            \
      "Always run the drive CPU as on the real thing",                    \
      radio_Drive##num##IdleMethod_callback,                              \
      (void *) DRIVE_IDLE_NO_IDLE, 0,                                     \
      TUI_MENU_BEH_CLOSE, NULL, NULL },                                   \
    { "_Trap Idle",                                                       \
      "Stop running the drive CPU when entering the idle DOS loop",       \
      radio_Drive##num##IdleMethod_callback,                              \
      (void *) DRIVE_IDLE_TRAP_IDLE, 0,                                   \
      TUI_MENU_BEH_CLOSE, NULL, NULL },                                   \
    { "_Skip Cycles",                                                     \
      "Skip drive CPU cycles when the IEC bus is not used for a while",   \
      radio_Drive##num##IdleMethod_callback,                              \
      (void *) DRIVE_IDLE_SKIP_CYCLES, 0,                                 \
      TUI_MENU_BEH_CLOSE, NULL, NULL },                                   \
    { NULL }                                                              \
};

DEFINE_DRIVE_IDLE_METHOD_SUBMENU(8)
DEFINE_DRIVE_IDLE_METHOD_SUBMENU(9)

TUI_MENU_DEFINE_TOGGLE(Drive8ParallelCable)
TUI_MENU_DEFINE_TOGGLE(Drive9ParallelCable)

static tui_menu_item_def_t drive_settings_submenu[] = {
    { "True Drive _Emulation:",
      "Enable hardware-level floppy drive emulation",
      toggle_DriveTrueEmulation_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "--" },
    { "Drive #_8 model:",
      "Specify model for drive #8",
      drive_type_submenu_callback, (void *) 8, 26,
      TUI_MENU_BEH_CONTINUE, drive8_type_submenu,
      "Drive 8 model" },
    { "Drive #8 idle method:",
      "Specify idle method for drive #8",
      drive_idle_method_submenu_callback, (void *) 8, 12,
      TUI_MENU_BEH_CONTINUE, drive8_idle_method_submenu,
      "Drive 8 idle method" },
    { "Drive #8 Parallel Cable:",
      "Enable a SpeedDOS-compatible parallel cable for drive #8",
      toggle_Drive8ParallelCable_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Drive #8 40-Track Image Support:",
      "Settings for dealing with 40-track disk images in drive #8",
      drive_extend_image_policy_submenu_callback, (void *) 8, 16,
      TUI_MENU_BEH_CONTINUE, drive8_extend_image_policy_submenu, "" },
    { "--" },
    { "Drive #_9 model:",
      "Specify model for drive #9",
      drive_type_submenu_callback, (void *) 9, 26,
      TUI_MENU_BEH_CONTINUE, drive9_type_submenu,
      "Drive 9 model" },
    { "Drive #9 idle method:",
      "Specify idle method for drive #9",
      drive_idle_method_submenu_callback, (void *) 9, 12,
      TUI_MENU_BEH_CONTINUE, drive9_idle_method_submenu,
      "Drive 9 idle method" },
    { "Drive #9 Parallel Cable:",
      "Enable a SpeedDOS-compatible parallel cable for drive #9",
      toggle_Drive9ParallelCable_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Drive #9 40-Track Image Support:",
      "Settings for dealing with 40-track disk images in drive #9",
      drive_extend_image_policy_submenu_callback, (void *) 9, 16,
      TUI_MENU_BEH_CONTINUE, drive9_extend_image_policy_submenu, "" },
    { "--" },
    { "Drive _Sync Factor:",
      "Select drive/machine clock ratio",
      toggle_DriveSyncFactor_callback, NULL, 8,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

TUI_MENU_DEFINE_TOGGLE(Sound)

static TUI_MENU_CALLBACK(sound_sample_rate_submenu_callback)
{
    static char s[256];
    int value;

    resources_get_value("SoundSampleRate", (resource_value_t *) &value);
    sprintf(s, "%d Hz", value);
    return s;
}

TUI_MENU_DEFINE_RADIO(SoundSampleRate)

TUI_MENU_DEFINE_RADIO(SoundBufferSize)

static TUI_MENU_CALLBACK(sound_buffer_size_submenu_callback)
{
    static char s[256];
    int value;

    resources_get_value("SoundBufferSize", (resource_value_t *) &value);
    sprintf(s, "%d msec", value);
    return s;
}

TUI_MENU_DEFINE_RADIO(SoundOversample)

static TUI_MENU_CALLBACK(sound_oversample_submenu_callback)
{
    static char s[40];
    int value;

    resources_get_value("SoundOversample", (resource_value_t *) &value);
    if (value != 0) {
        int n = 1, i;

        for (i = 0; i < value; i++)
            n *= 2;
        sprintf(s, "%dx", n);
        return s;
    } else
        return "None";
}

TUI_MENU_DEFINE_RADIO(SoundSpeedAdjustment)

static TUI_MENU_CALLBACK(sound_synchronization_submenu_callback)
{
    int value;

    resources_get_value("SoundSpeedAdjustment", (resource_value_t *) &value);

    switch (value) {
      case SOUND_ADJUST_FLEXIBLE:
        return "Flexible";
      case SOUND_ADJUST_ADJUSTING:
        return "Adjusting";
      case SOUND_ADJUST_EXACT:
        return "Exact";
      default:
        return "Unknown";
    }
}

static tui_menu_item_def_t sample_rate_submenu[] = {
    { "_0: 8000 Hz",
      "Set sampling rate to 8000 Hz",
      radio_SoundSampleRate_callback, (void *) 8000, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_1: 11025 Hz",
      "Set sampling rate to 11025 Hz",
      radio_SoundSampleRate_callback, (void *) 11025, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_2: 22050 Hz",
      "Set sampling rate to 22050 Hz",
      radio_SoundSampleRate_callback, (void *) 22050, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_3: 44100 Hz",
      "Set sampling rate to 44100 Hz",
      radio_SoundSampleRate_callback, (void *) 44100, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t sound_buffer_size_submenu[] = {
    { "_1: 50 msec",
      "Set sound buffer size to 50 msec",
      radio_SoundBufferSize_callback, (void *) 50, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_2: 100 msec",
      "Set sound buffer size to 100 msec",
      radio_SoundBufferSize_callback, (void *) 100, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_3: 150 msec",
      "Set sound buffer size to 150 msec",
      radio_SoundBufferSize_callback, (void *) 150, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_4: 200 msec",
      "Set sound buffer size to 200 msec",
      radio_SoundBufferSize_callback, (void *) 200, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_5: 250 msec",
      "Set sound buffer size to 250 msec",
      radio_SoundBufferSize_callback, (void *) 250, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_6: 300 msec",
      "Set sound buffer size to 300 msec",
      radio_SoundBufferSize_callback, (void *) 300, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_7: 350 msec",
      "Set sound buffer size to 350 msec",
      radio_SoundBufferSize_callback, (void *) 350, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t sound_oversample_submenu[] = {
    { "_None",
      "Disable oversampling",
      radio_SoundOversample_callback, (void *) 0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_2x",
      "Enable 2x oversampling",
      radio_SoundOversample_callback, (void *) 1, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_4x",
      "Enable 4x oversampling",
      radio_SoundOversample_callback, (void *) 2, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8x",
      "Enable 8x oversampling",
      radio_SoundOversample_callback, (void *) 3, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t sound_synchronization_submenu[] = {
    { "_Flexible",
      "Slightly adapt sound playback speed to the speed of the emulator",
      radio_SoundSpeedAdjustment_callback, (void *) SOUND_ADJUST_FLEXIBLE, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_Adjusting",
      "Fully adapt the playback speed to the emulator, avoiding clicks when it's slower",
      radio_SoundSpeedAdjustment_callback, (void *) SOUND_ADJUST_ADJUSTING, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_Exact",
      "Don't adapt sound playback: make the emulator finetune its speed to the playback",
      radio_SoundSpeedAdjustment_callback, (void *) SOUND_ADJUST_EXACT, 0,
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
    { "_Oversampling Factor:",
      "Specify amount of oversampling on sound output",
      sound_oversample_submenu_callback, NULL, 4,
      TUI_MENU_BEH_CONTINUE, sound_oversample_submenu, "Oversample" },
    { "S_ynchronization Method:",
      "Specify method used to synchronize the sound playback with the emulator",
      sound_synchronization_submenu_callback, NULL, 9,
      TUI_MENU_BEH_CONTINUE, sound_synchronization_submenu, "Synchronization" },
    { NULL }
};

/* ------------------------------------------------------------------------- */

/* Joystick settings.  */

static TUI_MENU_CALLBACK(get_joystick_device_callback)
{
    int port = (int) param;
    char *resource = port == 1 ? "JoyDevice1" : "JoyDevice2";
    int value;

    resources_get_value(resource, (resource_value_t *) &value);
    switch (value) {
      case JOYDEV_NONE:
        return "None";
      case JOYDEV_NUMPAD:
        return "Numpad + Right Ctrl";
      case JOYDEV_KEYSET1:
        return "Keyset A";
      case JOYDEV_KEYSET2:
        return "Keyset B";
      case JOYDEV_HW1:
        return "Joystick #1";
      case JOYDEV_HW2:
        return "Joystick #2";
    }

    return "Unknown";
}

static TUI_MENU_CALLBACK(set_joy_device_callback)
{
    int port = (int) param >> 8;
    char *resource = port == 1 ? "JoyDevice1" : "JoyDevice2";

    if (been_activated) {
        resources_set_value(resource, (resource_value_t) ((int) param & 0xff));
        ui_update_menus();
    } else {
        int value;

        resources_get_value(resource, (resource_value_t *) &value);
        if (value == ((int) param & 0xff))
            *become_default = 1;
    }

    return NULL;
}

static TUI_MENU_CALLBACK(swap_joysticks_callback)
{
    int value1, value2, tmp;

    if (been_activated) {
        resources_get_value("JoyDevice1", (resource_value_t *) &value1);
        resources_get_value("JoyDevice2", (resource_value_t *) &value2);

        tmp = value1;
        value1 = value2;
        value2 = tmp;

        resources_set_value("JoyDevice1", (resource_value_t) value1);
        resources_set_value("JoyDevice2", (resource_value_t) value2);

        ui_update_menus();
    }

    return NULL;
}

static tui_menu_item_def_t joy_device_1_submenu[] = {
    { "N_one",
      "No joystick device attached",
      set_joy_device_callback, (void *) (0x100 | JOYDEV_NONE), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "_Numpad + Right Ctrl",
      "Use numeric keypad for movement and right Ctrl for fire",
      set_joy_device_callback, (void *) (0x100 | JOYDEV_NUMPAD), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Keyset _A",
      "Use keyset A",
      set_joy_device_callback, (void *) (0x100 | JOYDEV_KEYSET1), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Keyset _B",
      "Use keyset B",
      set_joy_device_callback, (void *) (0x100 | JOYDEV_KEYSET2), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "PC Joystick #_1",
      "Use real PC joystick #1",
      set_joy_device_callback, (void *) (0x100 | JOYDEV_HW1), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "PC Joystick #_2",
      "Use real PC joystick #2",
      set_joy_device_callback, (void *) (0x100 | JOYDEV_HW2), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t joy_device_2_submenu[] = {
    { "N_one",
      "No joystick device attached",
      set_joy_device_callback, (void *) (0x200 | JOYDEV_NONE), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "_Numpad + Right Ctrl",
      "Use numeric keypad for movement and right Ctrl for fire",
      set_joy_device_callback, (void *) (0x200 | JOYDEV_NUMPAD), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Keyset _A",
      "Use keyset A",
      set_joy_device_callback, (void *) (0x200 | JOYDEV_KEYSET1), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Keyset _B",
      "Use keyset B",
      set_joy_device_callback, (void *) (0x200 | JOYDEV_KEYSET2), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "Joystick #_1",
      "Use real joystick #1",
      set_joy_device_callback, (void *) (0x200 | JOYDEV_HW1), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Joystick #_2",
      "Use real joystick #2",
      set_joy_device_callback, (void *) (0x200 | JOYDEV_HW2), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static TUI_MENU_CALLBACK(keyset_callback)
{
    int direction, number;
    int value;
    char s[256];

    number = (int) param >> 8;
    direction = (int) param & 0xff;
    sprintf(s, "KeySet%d%s", number, joystick_direction_to_string(direction));

    if (been_activated) {
        kbd_code_t key;
        int width = 60, height = 5;
        int x = CENTER_X(width), y = CENTER_Y(height);
        tui_area_t backing_store = NULL;
        char msg[80];

        sprintf(msg, "Press key for %s%s (Esc for none)...",
                direction == KEYSET_FIRE ? "" : "direction ",
                joystick_direction_to_string(direction));
        tui_display_window(x, y, width, height, MESSAGE_BORDER, MESSAGE_BACK,
                           NULL, &backing_store);
        tui_set_attr(MESSAGE_FORE, MESSAGE_BACK, 0);
        tui_display(CENTER_X(strlen(msg)), y + 2, 0, msg);

        /* Do not allow Alt as we need it for hotkeys.  */
        do
            key = grab_key();
        while (key == K_LEFTALT || key == K_RIGHTALT);

        tui_area_put(backing_store, x, y);
        tui_area_free(backing_store);

        if (key == K_ESC)
            key = K_NONE;
        resources_set_value(s, (resource_value_t) key);
    }

    resources_get_value(s, (resource_value_t *) &value);
    return kbd_code_to_string((kbd_code_t) value);
}

#define DEFINE_KEYSET_MENU(num)                                         \
    static tui_menu_item_def_t keyset_##num##_submenu[] = {             \
        { "_1: South West:",                                            \
          "Specify key for diagonal down-left direction",               \
          keyset_callback, (void *) ((num << 8) | KEYSET_SW), 12,       \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_2: South:",                                                 \
          "Specify key for the down direction",                         \
          keyset_callback, (void *) ((num << 8) | KEYSET_S), 12,        \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_3: South East:",                                            \
          "Specify key for the diagonal down-right direction",          \
          keyset_callback, (void *) ((num << 8) | KEYSET_SE), 12,       \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_4: West:",                                                  \
          "Specify key for the left direction",                         \
          keyset_callback, (void *) ((num << 8) | KEYSET_W), 12,        \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_6: East:",                                                  \
          "Specify key for the right direction",                        \
          keyset_callback, (void *) ((num << 8) | KEYSET_E), 12,        \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_7: North West:",                                            \
          "Specify key for the diagonal up-left direction",             \
          keyset_callback, (void *) ((num << 8) | KEYSET_NW), 12,       \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_8: North:",                                                 \
          "Specify key for the up direction",                           \
          keyset_callback, (void *) ((num << 8) | KEYSET_N), 12,        \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_9: North East:",                                            \
          "Specify key for the diagonal up-right direction",            \
          keyset_callback, (void *) ((num << 8) | KEYSET_NE), 12,       \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_0: Fire",                                                   \
          "Specify key for the fire button",                            \
          keyset_callback, (void *) ((num << 8) | KEYSET_FIRE), 12,     \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { NULL }                                                        \
    };

DEFINE_KEYSET_MENU(1)
DEFINE_KEYSET_MENU(2)

static tui_menu_item_def_t single_joystick_submenu[] = {
    { "Joystick _Device:",
      "Specify device for joystick emulation",
      get_joystick_device_callback, (void *) 1, 19,
      TUI_MENU_BEH_CONTINUE, joy_device_1_submenu, "Joystick" },
    { "--" },
    { "Configure Keyset _A...",
      "Configure keyboard set A for joystick emulation",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, keyset_1_submenu, "Keyset A" },
    { "Configure Keyset _B...",
      "Configure keyboard set B for joystick emulation",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, keyset_2_submenu, "Keyset B" },
    { NULL }
};

static tui_menu_item_def_t double_joystick_submenu[] = {
    { "_Swap",
      "Swap joystick ports",
      swap_joysticks_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "--" },
    { "Port #_1:",
      "Specify device for emulation of joystick in port #1",
      get_joystick_device_callback, (void *) 1, 19,
      TUI_MENU_BEH_CONTINUE, joy_device_1_submenu, "Joystick #1" },
    { "Port #_2:",
      "Specify device for emulation of joystick in port #2",
      get_joystick_device_callback, (void *) 2, 19,
      TUI_MENU_BEH_CONTINUE, joy_device_2_submenu, "Joystick #2" },
    { "--" },
    { "Configure Keyset _A...",
      "Configure keyboard set A for joystick emulation",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, keyset_1_submenu, "Keyset A" },
    { "Configure Keyset _B...",
      "Configure keyboard set B for joystick emulation",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, keyset_2_submenu, "Keyset B" },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static TUI_MENU_CALLBACK(set_romset_callback)
{
    if (been_activated) {
        romset_load((char *)param);
    }
    return NULL;
}

static TUI_MENU_CALLBACK(load_romset_callback)
{
    if (been_activated) {
        char *name;

        name = tui_file_selector("Load custom ROM set definition",
                                 NULL, "*.vrs", NULL, NULL, NULL);

        if (name != NULL) {
            if (romset_load(name) < 0)
                ui_error("Could not load ROM set file '%s'", name);
            free(name);
        }
    }
    return NULL;
}

static TUI_MENU_CALLBACK(dump_romset_callback)
{
    if (been_activated) {
        char name[GET_PATH_MAX];
        memset(name, 0, GET_PATH_MAX);

        tui_input_string("Dump ROM set definition", "Enter file name:",
                         name, GET_PATH_MAX);
        remove_spaces(name);

        romset_dump(name, mem_romset_resources_list);
    }
    return NULL;
}

static tui_menu_item_def_t rom_submenu[] = {
    { "_Default ROM set",
      "Load default ROM set file",
      set_romset_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_Custom ROM set",
      "Load custom ROM set from a *.vrs file",
      load_romset_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_Save ROM set",
      "Save ROM set definition to a *.vrs file",
      dump_romset_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static TUI_MENU_CALLBACK(save_settings_callback)
{
    if (been_activated) {
	if (resources_save(NULL) < 0)
	    tui_error("Cannot save settings.");
	else
	    tui_message("Settings saved successfully.");
    }

    return NULL;
}

static TUI_MENU_CALLBACK(load_settings_callback)
{
    if (been_activated) {
	if (resources_load(NULL) < 0) {
	    tui_error("Cannot load settings.");
	} else {
	    tui_message("Settings loaded successfully.");
            ui_update_menus();
        }
    }

    return NULL;
}

static TUI_MENU_CALLBACK(restore_default_settings_callback)
{
    if (been_activated) {
	resources_set_defaults();
	tui_message("Default settings restored.");
        ui_update_menus();
    }

    return NULL;
}

static TUI_MENU_CALLBACK(quit_callback)
{
    if (been_activated) {
	_setcursortype(_NORMALCURSOR);
	normvideo();
	clrscr();
        serial_remove_file(-1);
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

static TUI_MENU_CALLBACK(monitor_callback)
{
    if (been_activated)
        mon(maincpu_regs.reg_pc);

    return NULL;
}

/* ------------------------------------------------------------------------- */

static TUI_MENU_CALLBACK(soft_reset_callback)
{
    if (been_activated) {
	maincpu_trigger_reset();
    }

    /* This way, the "Not Really!" item is always the default one.  */
    *become_default = 0;

    return NULL;
}

static TUI_MENU_CALLBACK(hard_reset_callback)
{
    if (been_activated)
        machine_powerup();

    /* This way, the "Not Really!" item is always the default one.  */
    *become_default = 0;

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

static TUI_MENU_CALLBACK(show_copyright_callback)
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
            "Copyright (c) 1996-1999 Ettore Perazzoli",
            "Copyright (c) 1996-1999 Andre' Fachat",
            "Copyright (c) 1993-1994, 1997-1999 Teemu Rantanen",
            "Copyright (c) 1997-1999 Daniel Sladic",
            "Copyright (c) 1998-1999 Andreas Boose",
            "Copyright (c) 1998-1999 Tibor Biczo",
            "Copyright (c) 1993-1996 Jouko Valta",
            "Copyright (c) 1993-1994 Jarkko Sonninen",
#ifdef USE_RESID
            "",
            "reSID engine:"
            "Copyright (c) 1998 Dag Lem",
#endif
            "",
	    "Official VICE homepage:",
	    "http://www.cs.cmu.edu/~dsladic/vice/vice.html",
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

static TUI_MENU_CALLBACK(show_info_callback)
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
		      "Enable screen cache (disabled when using triple buffering)",
		      toggle_VideoCache_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);

#ifndef USE_MIDAS_SOUND
    tui_menu_add_item(ui_video_submenu, "_Triple Buffering:",
		      "Enable triple buffering for smoother animations (when available)",
		      toggle_TripleBuffering_callback, NULL, 3,
		      TUI_MENU_BEH_CONTINUE);
#endif
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

TUI_MENU_DEFINE_TOGGLE(FSDevice8HideCBMFiles)
TUI_MENU_DEFINE_TOGGLE(FSDevice9HideCBMFiles)
TUI_MENU_DEFINE_TOGGLE(FSDevice10HideCBMFiles)
TUI_MENU_DEFINE_TOGGLE(FSDevice11HideCBMFiles)

static TUI_MENU_CALLBACK(set_fsdevice_directory_callback)
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

#define DEFINE_FSDEVICE_SUBMENU(num)                                    \
    tui_menu_item_def_t fsdevice##num##_submenu[] = {                   \
        { "_Directory:",                                                \
          "Specify access directory for device" #num,                   \
          set_fsdevice_directory_callback, (void *) (num), 40,          \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "--" },                                                       \
        { "_Allow access:",                                             \
          "Allow device" #num " to access the MS-DOS file system",      \
          toggle_FileSystemDevice##num##_callback, NULL, 3,             \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_Convert P00 names:",                                        \
          "Handle P00 names on device " #num,                           \
          toggle_FSDevice##num##ConvertP00_callback, NULL, 3,           \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_Save P00 files:",                                           \
          "Create P00 files on device " #num,                           \
          toggle_FSDevice##num##SaveP00_callback, NULL, 3,              \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_Hide non-P00 files: ",                                      \
          "Display only P00 files on device " #num,                     \
          toggle_FSDevice##num##HideCBMFiles_callback, NULL, 3,         \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { NULL }                                                        \
    };

DEFINE_FSDEVICE_SUBMENU(8)
DEFINE_FSDEVICE_SUBMENU(9)
DEFINE_FSDEVICE_SUBMENU(10)
DEFINE_FSDEVICE_SUBMENU(11)

tui_menu_item_def_t fsdevice_submenu[] = {
    { "Drive _8...",
      "Settings for drive #8",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, fsdevice8_submenu, "Drive 8 directory access" },
    { "Drive _9...",
      "Settings for drive #9",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, fsdevice9_submenu, "Drive 9 directory access" },
    { "Drive 1_0...",
      "Settings for drive #10",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, fsdevice10_submenu, "Drive 10 directory access" },
    { "Drive 1_1...",
      "Settings for drive #11",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, fsdevice11_submenu, "Drive 11 directory access" },
    { NULL }
};

TUI_MENU_DEFINE_TOGGLE(NoTraps)

/* ------------------------------------------------------------------------- */

static TUI_MENU_CALLBACK(speed_submenu_callback)
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

static TUI_MENU_CALLBACK(speed_callback)
{
    if (been_activated) {
        int value = (int)param;

	if (value < 0) {
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
	    } else
                return NULL;
	}

        resources_set_value("Speed", (resource_value_t) value);
    }
    return NULL;
}

TUI_MENU_DEFINE_TOGGLE(WarpMode)
TUI_MENU_DEFINE_TOGGLE(UseLeds)

static void create_special_submenu(int has_serial_traps)
{
    static tui_menu_t speed_submenu;

    ui_special_submenu = tui_menu_create("Other Settings", 1);

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
			      speed_callback, (void *)speed[i], 5,
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

    tui_menu_add_item(ui_special_submenu, "Enable _Warp Mode:",
                      "Make the emulator run as fast as possible skipping lots of frames",
                      toggle_WarpMode_callback, NULL, 3,
                      TUI_MENU_BEH_CONTINUE);

    /* File system access.  */
    {
	tui_menu_t tmp = tui_menu_create("MS-DOS Directory Access", 1);

        tui_menu_add_separator(ui_special_submenu);
	tui_menu_add(tmp, fsdevice_submenu);
	tui_menu_add_submenu(ui_special_submenu,
			     "MS-DOS _Directory Access...",
			     "Options to access MS-DOS directories from within the emulator",
			     tmp, NULL, NULL, 0);
    }

    if (has_serial_traps)
        tui_menu_add_item(ui_special_submenu, "Disable Kernal _Traps:",
                          "Disable the Kernal ROM patches used by tape and fast drive emulation",
                          toggle_NoTraps_callback, NULL, 4,
                          TUI_MENU_BEH_CONTINUE);

    tui_menu_add_separator(ui_special_submenu);
    tui_menu_add_item(ui_special_submenu,
                      "Use _Keyboard LEDs:",
                      "Use PC keyboard LEDs for the disk drive and Warp Mode",
                      toggle_UseLeds_callback, NULL, 4,
                      TUI_MENU_BEH_CONTINUE);
}

/* ------------------------------------------------------------------------- */

void ui_create_main_menu(int has_tape, int has_drive, int has_serial_traps,
                         int number_joysticks, int has_datasette)
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
    tui_menu_add_item(ui_attach_submenu, "Drive #1_1:",
		      "Attach disk image for disk drive #11",
		      attach_disk_callback, (void *)11, 30,
		      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(ui_attach_submenu, "Autostart _Drive #8",
		      "Reset the emulator and run the first program in the disk in drive 8",
		      autostart_callback, (void *)8, 0,
		      TUI_MENU_BEH_RESUME);
    ui_flip_submenu = tui_menu_create("Fliplist for drive #8", 1);
    tui_menu_add_submenu(ui_attach_submenu, "_Fliplist for Drive #8...",
                         "Select, add or remove disk images from the flip list",
                         ui_flip_submenu, NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(ui_flip_submenu,"_Add current image",
              "Add current disk image to flip list (ALT-F3)",
              flip_add_callback, NULL, 0,
              TUI_MENU_BEH_CLOSE);
    tui_menu_add_item(ui_flip_submenu,"_Remove current image",
              "Remove current disk image from flip list (ALT-F4)",
              flip_remove_callback, NULL, 0,
              TUI_MENU_BEH_CLOSE);
    tui_menu_add_item(ui_flip_submenu,"Attach _next image",
              "Attach next disk image from flip list (ALT-F1)",
              flip_next_callback, NULL, 0,
              TUI_MENU_BEH_RESUME);
    tui_menu_add_item(ui_flip_submenu,"Attach _previous image",
              "Attach previous disk image from flip list (ALT-F2)",
              flip_previous_callback, NULL, 0,
              TUI_MENU_BEH_RESUME);

    if (has_tape) {
	tui_menu_add_separator(ui_attach_submenu);
	tui_menu_add_item(ui_attach_submenu,"_Tape:",
			  "Attach tape image for cassette player (device #1)",
			  attach_tape_callback, NULL, 30,
			  TUI_MENU_BEH_CONTINUE);
	tui_menu_add_item(ui_attach_submenu, "Autostart Ta_pe",
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
    tui_menu_add_item(ui_detach_submenu, "Drive #1_1:",
		      "Remove disk from disk drive #11",
		      detach_disk_callback, (void *)11, 30,
		      TUI_MENU_BEH_CONTINUE);

    if (has_tape) {
	tui_menu_add_separator(ui_detach_submenu);
	tui_menu_add_item(ui_detach_submenu, "_Tape:",
			  "Remove tape from cassette player (device #1)",
			  detach_tape_callback, NULL, 30,
			  TUI_MENU_BEH_CONTINUE);
    }

    tui_menu_add_submenu(ui_main_menu, "_Attach Image...",
                         "Insert virtual disks, tapes or cartridges in the emulated machine",
                         ui_attach_submenu, NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
    tui_menu_add_submenu(ui_main_menu, "_Detach Image...",
                         "Remove virtual disks, tapes or cartridges from the emulated machine",
                         ui_detach_submenu, NULL, 0,
                         TUI_MENU_BEH_CONTINUE);

    if (has_datasette) {
        ui_datasette_submenu = tui_menu_create("Datassette control", 1);
        tui_menu_add_item(ui_datasette_submenu, "S_top",
                  "Press the STOP key of the datassette",
                  datasette_callback, (void *)DATASETTE_CONTROL_STOP, 0,
                  TUI_MENU_BEH_RESUME);
        tui_menu_add_item(ui_datasette_submenu, "_Start",
                  "Press the START key of the datassette",
                  datasette_callback, (void *)DATASETTE_CONTROL_START, 0,
                  TUI_MENU_BEH_RESUME);
        tui_menu_add_item(ui_datasette_submenu, "_Forward",
                  "Press the FORWARD key of the datassette",
                  datasette_callback, (void *)DATASETTE_CONTROL_FORWARD, 0,
                  TUI_MENU_BEH_RESUME);
        tui_menu_add_item(ui_datasette_submenu, "_Rewind",
                  "Press the REWIND key of the datassette",
                  datasette_callback, (void *)DATASETTE_CONTROL_REWIND, 0,
                  TUI_MENU_BEH_RESUME);
#if 0
        tui_menu_add_item(ui_datasette_submenu, "Re_cord",
                  "Press the REWIND key of the datassette",
                  datasette_callback, (void *)DATASETTE_CONTROL_RECORD, 0,
                  TUI_MENU_BEH_RESUME);
        tui_menu_add_item(ui_datasette_submenu, "R_eset",
                  "Press the REWIND key of the datassette",
                  datasette_callback, (void *)DATASETTE_CONTROL_RESET, 0,
                  TUI_MENU_BEH_RESUME);
#endif

        tui_menu_add_submenu(ui_main_menu, "Datassett_e Control...",
                             "Press some buttons on the emulated datassette",
                             ui_datasette_submenu, NULL, 0,
                             TUI_MENU_BEH_CONTINUE);
    }

    tui_menu_add_item(ui_main_menu, "Change _Working Directory...",
		      "Change the current working directory",
		      change_workdir_callback, NULL, 0,
		      TUI_MENU_BEH_CONTINUE);

    tui_menu_add_separator(ui_main_menu);

    create_ui_video_submenu();
    tui_menu_add_submenu(ui_main_menu, "_Video Settings...",
			 "Screen parameters",
			 ui_video_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);

    if (has_drive) {
        ui_drive_submenu = tui_menu_create("Disk Drive Settings", 1);
	tui_menu_add(ui_drive_submenu, drive_settings_submenu);
	tui_menu_add_submenu(ui_main_menu, "Dis_k Drive Settings...",
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

    if (number_joysticks > 0) {
        ui_joystick_settings_submenu = tui_menu_create("Joystick Settings", 1);
        tui_menu_add_submenu(ui_main_menu, "_Joystick Settings...",
                             "Joystick settings",
                             ui_joystick_settings_submenu, NULL, 0,
                             TUI_MENU_BEH_CONTINUE);
        if (number_joysticks == 2)
            tui_menu_add(ui_joystick_settings_submenu,
                         double_joystick_submenu);
        else                    /* Just one joystick.  */
            tui_menu_add(ui_joystick_settings_submenu,
                         single_joystick_submenu);
    }

/*
    ui_rom_submenu = tui_menu_create("Firmware ROM Settings", 1);

    tui_menu_add(ui_rom_submenu, rom_submenu);
    tui_menu_add_submenu(ui_main_menu, "_ROM Settings...",
             "Firmware ROMs the emulator is using",
             ui_rom_submenu, NULL, 0,
             TUI_MENU_BEH_CONTINUE);
*/

    create_special_submenu(has_serial_traps);

    tui_menu_add_submenu(ui_main_menu, "_Other Settings...",
			 "Extra emulation features",
			 ui_special_submenu, NULL, 0,
			 TUI_MENU_BEH_CONTINUE);

    tui_menu_add_separator(ui_main_menu);

    ui_snapshot_submenu = tui_menu_create("Freeze Commands", 1);
    tui_menu_add(ui_snapshot_submenu, ui_snapshot_menu_def);

    tui_menu_add_submenu(ui_main_menu, "_Freeze Commands...",
                         "Commands for loading/saving the machine state",
                         ui_snapshot_submenu, NULL, 0,
                         TUI_MENU_BEH_CONTINUE);

    tui_menu_add_separator(ui_main_menu);

    ui_settings_submenu = tui_menu_create("Configuration Commands", 1);

    tui_menu_add_item(ui_settings_submenu, "_Write Configuration",
		      "Save current settings as default for next session",
		      save_settings_callback, NULL, 0,
		      TUI_MENU_BEH_CLOSE);
    tui_menu_add_item(ui_settings_submenu, "_Load Configuration",
		      "Load saved settings from previous session",
		      load_settings_callback, NULL, 0,
		      TUI_MENU_BEH_CLOSE);
    tui_menu_add_item(ui_settings_submenu, "Restore _Factory Defaults",
		      "Set default settings",
		      restore_default_settings_callback, NULL, 0,
		      TUI_MENU_BEH_CLOSE);

    tui_menu_add_submenu(ui_main_menu, "_Configuration Commands...",
                         "Commands to save, retrieve and restore settings",
                         ui_settings_submenu, NULL, 0,
                         TUI_MENU_BEH_CONTINUE);

    tui_menu_add_separator(ui_main_menu);

    tui_menu_add_item(ui_main_menu, "_Monitor",
		      "Enter the built-in machine language monitor",
		      monitor_callback, NULL, 0,
		      TUI_MENU_BEH_RESUME);

    ui_reset_submenu = tui_menu_create("Reset?", 1);
    tui_menu_add(ui_reset_submenu, reset_submenu);
    tui_menu_add_submenu(ui_main_menu, "_Reset ",
			 "Reset the machine",
			 ui_reset_submenu,
                         NULL, NULL, 0);

    ui_quit_submenu = tui_menu_create("Quit", 1);
    tui_menu_add(ui_quit_submenu, quit_submenu);
    tui_menu_add_submenu(ui_main_menu, "_Quit",
			 "Quit emulator",
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
