/*
 * menudefs.c - Definition of menu commands and settings.
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

#include <conio.h>
#include <dir.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "attach.h"
#include "autostart.h"
#include "datasette.h"
#include "drivecpu.h"
#include "imagecontents.h"
#include "info.h"
#include "interrupt.h"
#include "ioutil.h"
#include "kbd.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "menudefs.h"
#include "monitor.h"
#include "resources.h"
#include "tape.h"
#include "tui.h"
#include "tui_backend.h"
#include "tuifs.h"
#include "tuiview.h"
#include "ui.h"
#include "uiattach.h"
#include "uidrive.h"
#include "uijoystick.h"
#include "uiperipherial.h"
#include "uiprinter.h"
#include "uisnapshot.h"
#include "uisound.h"
#include "util.h"
#include "version.h"
#include "video.h"
#include "videoarch.h"


/* ------------------------------------------------------------------------- */

tui_menu_t ui_attach_submenu;
tui_menu_t ui_datasette_submenu;
tui_menu_t ui_datasette_settings_submenu;
tui_menu_t ui_detach_submenu;
tui_menu_t ui_info_submenu;
tui_menu_t ui_main_menu;
tui_menu_t ui_quit_submenu;
tui_menu_t ui_reset_submenu;
tui_menu_t ui_rom_submenu;
tui_menu_t ui_settings_submenu;
tui_menu_t ui_snapshot_submenu;
tui_menu_t ui_special_submenu;
tui_menu_t ui_video_submenu;

/* ------------------------------------------------------------------------ */

static TUI_MENU_CALLBACK(attach_tape_callback)
{
    const char *s;

    if (been_activated) {
        char *directory, *default_item;
        char *name, *file;

        s = tape_get_file_name();
        util_fname_split(s, &directory, &default_item);

        name = tui_file_selector("Attach a tape image", directory,
                                 "*.t64;*.tap;*.t6z;*.taz;*.zip;*.gz;*.lzh",
                                 default_item, IMAGE_CONTENTS_TAPE,
                                 image_contents_read, 0, &file, NULL);

        if (file != NULL) {
            if (autostart_tape(name, file, 0, AUTOSTART_MODE_RUN) < 0)
                tui_error("Cannot autostart tape image.");
            else
                *behavior = TUI_MENU_BEH_RESUME;
            lib_free(file);
        } else if (name != NULL
                   && (s == NULL || strcasecmp(s, name) != 0)
                   && tape_image_attach(1, name) < 0) {
            tui_error("Invalid tape image.");
        }
        ui_update_menus();
        lib_free(directory);
        lib_free(default_item);
        if (name != NULL)
            lib_free(name);
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
    const char *s;

    if (been_activated) {
        file_system_detach_disk((int)param);
        ui_update_menus();
    }

    s = file_system_get_disk_name((unsigned int)param);
    if (s == NULL || *s == '\0')
        return "(none)";
    else
        return s;
}

static TUI_MENU_CALLBACK(detach_tape_callback)
{
    const char *s;

    if (been_activated) {
        tape_image_detach(1);
        ui_update_menus();
    }

    s = tape_get_file_name();
    if (s == NULL || *s == '\0')
        return "(none)";
    else
        return s;
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

    util_remove_spaces(s);
    if (*s == '\0')
        return NULL;

    if (ioutil_chdir(s) == -1)
        tui_error("Invalid directory.");

    return NULL;
}

/* ------------------------------------------------------------------------- */

static TUI_MENU_CALLBACK(resolution_submenu_callback)
{
    int mode;

    resources_get_value("VGAMode", (void *)&mode);
    return vga_modes[mode].description;
}

TUI_MENU_DEFINE_RADIO(VGAMode)

static TUI_MENU_CALLBACK(refresh_rate_submenu_callback)
{
    int v;

    resources_get_value("RefreshRate", (void *)&v);
    if (v == 0) {
        return "Auto";
    } else {
        static char s[256];

        sprintf(s, "1/%d", v);
        return s;
    }
}

TUI_MENU_DEFINE_RADIO(RefreshRate)

#ifndef USE_MIDAS_SOUND
TUI_MENU_DEFINE_TOGGLE(TripleBuffering)
#endif

/* ------------------------------------------------------------------------- */
/* Datasette settings */

TUI_MENU_DEFINE_TOGGLE(DatasetteResetWithCPU)
TUI_MENU_DEFINE_RADIO(DatasetteSpeedTuning)
TUI_MENU_DEFINE_RADIO(DatasetteZeroGapDelay)

static TUI_MENU_CALLBACK(datasette_speedtuning_submenu_callback)
{
    int value;
    static char s[100];

    resources_get_value("DatasetteSpeedTuning", (void *)&value);
    sprintf(s, "%d",value);
    return s;
}

static TUI_MENU_CALLBACK(datasette_zerogapdelay_submenu_callback)
{
    int value;
    static char s[100];

    resources_get_value("DatasetteZeroGapDelay", (void *)&value);
    sprintf(s, "%8d",value);
    return s;
}

static tui_menu_item_def_t datasette_speedtuning_submenu[] = {
    { "_0 cycles",
      NULL,
      radio_DatasetteSpeedTuning_callback, (void *)0, 5,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_1 cycle",
      NULL,
      radio_DatasetteSpeedTuning_callback, (void *)1, 5,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_2 cycles",
      NULL,
      radio_DatasetteSpeedTuning_callback, (void *)2, 5,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_3 cycles",
      NULL,
      radio_DatasetteSpeedTuning_callback, (void *)3, 5,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_4 cycles",
      NULL,
      radio_DatasetteSpeedTuning_callback, (void *)4, 5,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_5 cycles",
      NULL,
      radio_DatasetteSpeedTuning_callback, (void *)5, 5,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_6 cycles",
      NULL,
      radio_DatasetteSpeedTuning_callback, (void *)6, 5,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_7 cycles",
      NULL,
      radio_DatasetteSpeedTuning_callback, (void *)7, 5,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t datasette_zerogapdelay_submenu[] = {
    { "_1:   1000 cycles",
      NULL,
      radio_DatasetteZeroGapDelay_callback, (void *)1000, 8,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_2:   2000 cycles",
      NULL,
      radio_DatasetteZeroGapDelay_callback, (void *)2000, 8,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_3:   5000 cycles",
      NULL,
      radio_DatasetteZeroGapDelay_callback, (void *)5000, 8,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_4:  10000 cycles",
      NULL,
      radio_DatasetteZeroGapDelay_callback, (void *)10000, 8,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_5:  20000 cycles",
      NULL,
      radio_DatasetteZeroGapDelay_callback, (void *)20000, 8,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_6:  50000 cycles",
      NULL,
      radio_DatasetteZeroGapDelay_callback, (void *)50000, 8,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_7: 100000 cycles",
      NULL,
      radio_DatasetteZeroGapDelay_callback, (void *)100000, 8,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t datasette_settings_submenu[] = {
    { "_Reset Datasette with CPU:",
      "Reset the datasette when main CPU resets",
      toggle_DatasetteResetWithCPU_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Cycles _delay each trigger:",
      "Finetuning for the speed of datasette",
      datasette_speedtuning_submenu_callback, NULL, 2,
      TUI_MENU_BEH_CONTINUE, datasette_speedtuning_submenu,
      "Delay each trigger:" },
    { "A _zero in the tap is:",
      "How many cyles delay are represented by a zero in the tap?",
      datasette_zerogapdelay_submenu_callback, NULL, 8,
      TUI_MENU_BEH_CONTINUE, datasette_zerogapdelay_submenu,
      "A zero in tap is..." },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static TUI_MENU_CALLBACK(set_romset_callback)
{
    if (been_activated) {
        if (machine_romset_file_load((char *)param) < 0)
            tui_error("Could not load ROM set '%s'", param);
        else
            tui_message("ROM set loaded successfully.");
    }
    return NULL;
}

static TUI_MENU_CALLBACK(load_romset_callback)
{
    if (been_activated) {
        char *name;

        name = tui_file_selector("Load custom ROM set definition",
                                 NULL, "*.vrs", NULL, 0, NULL, 0, NULL, NULL);

        if (name != NULL) {
            if (machine_romset_file_load(name) < 0)
                ui_error("Could not load ROM set file '%s'", name);
            lib_free(name);
        }
    }
    return NULL;
}

static TUI_MENU_CALLBACK(dump_romset_callback)
{
    if (been_activated) {
        char name[PATH_MAX];
        memset(name, 0, PATH_MAX);

        tui_input_string("Dump ROM set definition", "Enter file name:",
                         name, PATH_MAX);
        util_remove_spaces(name);

        machine_romset_file_save(name);
    }
    return NULL;
}

static tui_menu_item_def_t rom_submenu[] = {
    { "_Default ROM set",
      "Load default ROM set file",
      set_romset_callback, "default.vrs", 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "C_ustom ROM set",
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
        file_system_detach_disk(-1);
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
        monitor_startup();

    return NULL;
}

/* ------------------------------------------------------------------------- */

static TUI_MENU_CALLBACK(soft_reset_callback)
{
    if (been_activated)
        machine_trigger_reset(MACHINE_RESET_MODE_SOFT);

    /* This way, the "Not Really!" item is always the default one.  */
    *become_default = 0;

    return NULL;
}

static TUI_MENU_CALLBACK(hard_reset_callback)
{
    if (been_activated)
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);

    *become_default = 0;

    return NULL;
}

static TUI_MENU_CALLBACK(reset_drive_callback)
{
    if (been_activated)
        drivecpu_trigger_reset((unsigned int)param);

    *become_default = 0;

    return NULL;
}

static tui_menu_item_def_t reset_submenu[] = {
    { "_Not Really!", "Go back to the menu",
      NULL, NULL, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Do a _Soft Reset", "Do a soft reset without resetting the memory",
      soft_reset_callback, NULL, 0,
      TUI_MENU_BEH_RESUME, NULL, NULL },
    { "Do a _Hard Reset", "Clear memory and reset as after a power-up",
      hard_reset_callback, NULL, 0,
      TUI_MENU_BEH_RESUME, NULL, NULL },
    { "Reset drive #_8", "Reset drive #8 separately",
      reset_drive_callback, (void *)0, 0,
      TUI_MENU_BEH_RESUME, NULL, NULL },
    { "Reset drive #_9", "Reset drive #9 separately",
      reset_drive_callback, (void *)1, 0,
      TUI_MENU_BEH_RESUME, NULL, NULL },
    { "Reset drive #1_0", "Reset drive #10 separately",
      reset_drive_callback, (void *)2, 0,
      TUI_MENU_BEH_RESUME, NULL, NULL },
    { "Reset drive #_11", "Reset drive #11 separately",
      reset_drive_callback, (void *)3, 0,
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
            "Copyright (c) 1998-2005 Andreas Boose",
            "Copyright (c) 1998-2005 Tibor Biczo",
            "Copyright (c) 1998-2005 Dag Lem",
            "Copyright (c) 1999-2005 Andreas Dehmel",
            "Copyright (c) 1999-2005 Thomas Bretz",
            "Copyright (c) 1999-2005 Andreas Matthies",
            "Copyright (c) 1999-2005 Martin Pottendorfer",
            "Copyright (c) 2000-2005 Spiro Trikaliotis",
            "Copyright (c) 2003-2005 David Hansel",
            "Copyright (c) 2000-2004 Markus Brenner",
            "Copyright (c) 2005      Marco van den Heuvel",
            "",
            "Official VICE homepage:",
            "http://www.viceteam.org/",
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
      show_info_callback, (void *)info_contrib_text, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_License",
      "VICE license (GNU General Public License)",
      show_info_callback, (void *)info_license_text, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_No warranty!",
      "VICE is distributed WITHOUT ANY WARRANTY!",
      show_info_callback, (void *)info_warranty_text, 0,
      TUI_MENU_BEH_CONTINUE },
    { NULL }
};

/* ------------------------------------------------------------------------- */

/* This is a bit of a hack, but I prefer this way instead of writing 1,000
   menu entries...  */
static void create_ui_video_submenu(void)
{
    static tui_menu_t refresh_rate_submenu, vga_mode_submenu;
    int i;

    refresh_rate_submenu = tui_menu_create("Refresh", 1);

    for (i = 1; i <= 10; i++) {
        char *label, *desc;
        if (i != 10)
            label = lib_msprintf("1/_%d", i);
        else
            label = lib_stralloc("1/1_0");
        if (i == 1)
            desc = lib_msprintf("Set refresh rate to 1/%d (update every frame)",
                                i);
        else
            desc = lib_msprintf("Set refresh rate to 1/%d (update once every %d frames)",
                                i, i);
        tui_menu_add_item(refresh_rate_submenu, label, desc,
                          radio_RefreshRate_callback, (void *)i, 0,
                          TUI_MENU_BEH_CLOSE);
        lib_free(label);
        lib_free(desc);
    }

    tui_menu_add_separator(refresh_rate_submenu);
    tui_menu_add_item(refresh_rate_submenu, "_Automatic",
                      "Let the emulator select an appropriate refresh rate automagically",
                       radio_RefreshRate_callback, NULL, 0,
                       TUI_MENU_BEH_CLOSE);

    vga_mode_submenu = tui_menu_create("VGA Resolution", 1);

    for (i = 0; i < NUM_VGA_MODES; i++) {
        char *s1, *s2;

        /* FIXME: hotkeys work only for less than 11 elements. */
        s1 = lib_msprintf("Mode %s%d: %s",(i<10?" _":""), i,
                          vga_modes[i].description);
        s2 = lib_msprintf("Set VGA resolution to %s", vga_modes[i].description);
        tui_menu_add_item(vga_mode_submenu, s1, s2,
                          radio_VGAMode_callback, (void *)i, 0,
                          TUI_MENU_BEH_CLOSE);
        lib_free(s1);
        lib_free(s2);
    }

    ui_video_submenu = tui_menu_create("Video Settings", 1);

    tui_menu_add_submenu(ui_video_submenu,"_VGA Resolution:",
                         "Choose screen resolution for video emulation",
                         vga_mode_submenu,
                         resolution_submenu_callback, NULL, 15);

    tui_menu_add_submenu(ui_video_submenu, "_Refresh Rate:",
                         "Choose frequency of screen refresh",
                         refresh_rate_submenu,
                         refresh_rate_submenu_callback, NULL, 4);

#ifndef USE_MIDAS_SOUND
    tui_menu_add_item(ui_video_submenu, "_Triple Buffering:",
                      "Enable triple buffering for smoother animations (when available)",
                      toggle_TripleBuffering_callback, NULL, 3,
                      TUI_MENU_BEH_CONTINUE);
#endif
}

/* ------------------------------------------------------------------------- */

TUI_MENU_DEFINE_TOGGLE(VirtualDevices)

/* ------------------------------------------------------------------------- */

static TUI_MENU_CALLBACK(speed_submenu_callback)
{
    static char s[1024];
    int value;

    resources_get_value("Speed", (void *)&value);
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

        resources_set_value("Speed", (resource_value_t)value);
    }
    return NULL;
}

TUI_MENU_DEFINE_TOGGLE(WarpMode)
TUI_MENU_DEFINE_TOGGLE(UseLeds)

static TUI_MENU_CALLBACK(toggle_ShowStatusbar_callback)
{
    int value;

    resources_get_value("ShowStatusbar", (void *)&value);

    if (been_activated) {
            if (value == STATUSBAR_MODE_OFF)
                value = STATUSBAR_MODE_ON;
            else if (value == STATUSBAR_MODE_ON)
                value = STATUSBAR_MODE_AUTO;
        else
                value = STATUSBAR_MODE_OFF;

        resources_set_value("ShowStatusbar", (resource_value_t)value);
    }

    switch (value) {
      case STATUSBAR_MODE_OFF:
        return "Off";
      case STATUSBAR_MODE_ON:
        return "On";
      case STATUSBAR_MODE_AUTO:
        return "Auto";
      default:
        return "(Custom)";
    }
}


static void create_speed_limit_submenu(void)
{
    int i;
    int speed[4] = { 100, 50, 20, 10 };
    char *s1, *s2;
    static tui_menu_t speed_submenu;

    ui_special_submenu = tui_menu_create("Other Settings", 1);

    speed_submenu = tui_menu_create("Speed Limit", 1);
    for (i = 0; i < 4; i++) {
        if (speed[i] == 100)
            s1 = lib_msprintf("Limit speed to the one of the real %s",
                           machine_name);
        else
            s1 = lib_msprintf("Limit speed to %d%% of the real %s",
                           speed[i], machine_name);
        s2 = lib_msprintf("_%d%%", speed[i]);
        tui_menu_add_item(speed_submenu, s2, s1,
                          speed_callback, (void *)speed[i], 5,
                          TUI_MENU_BEH_CLOSE);
        lib_free(s1);
        lib_free(s2);
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

    tui_menu_add_submenu(ui_special_submenu, "_Speed Limit:",
                         "Specify a custom speed limit",
                         speed_submenu, speed_submenu_callback,
                         NULL, 5);
}

static void create_special_submenu(int has_serial_traps)
{
    create_speed_limit_submenu();

    tui_menu_add_item(ui_special_submenu, "Enable _Warp Mode:",
                      "Make the emulator run as fast as possible skipping lots of frames",
                      toggle_WarpMode_callback, NULL, 3,
                      TUI_MENU_BEH_CONTINUE);

    tui_menu_add_separator(ui_special_submenu);
    uiperipherial_init(ui_special_submenu);
    uiprinter_init(ui_special_submenu);

    if (has_serial_traps)
        tui_menu_add_item(ui_special_submenu, "Enable virtual device _traps:",
                          "Enable virtual device traps for fast tape and drive emulation",
                          toggle_VirtualDevices_callback, NULL, 4,
                          TUI_MENU_BEH_CONTINUE);

    tui_menu_add_item(ui_special_submenu, "_Change Working Directory...",
                      "Change the current working directory",
                      change_workdir_callback, NULL, 0,
                      TUI_MENU_BEH_CONTINUE);

    tui_menu_add_separator(ui_special_submenu);
    tui_menu_add_item(ui_special_submenu,
                      "Use _Keyboard LEDs:",
                      "Use PC keyboard LEDs for the disk drive and Warp Mode",
                      toggle_UseLeds_callback, NULL, 4,
                      TUI_MENU_BEH_CONTINUE);
    tui_menu_add_item(ui_special_submenu,
                      "Show Status_bar:",
                      "Statusbar to display Speed, Tape and Drive properties; toggle with ALT-F5",
                      toggle_ShowStatusbar_callback, NULL, 10,
                      TUI_MENU_BEH_CONTINUE);
}

/* ------------------------------------------------------------------------- */

void ui_create_main_menu(int has_tape, int has_drive, int has_serial_traps,
                         int number_joysticks, int has_datasette)
{
    /* Main menu. */
    ui_main_menu = tui_menu_create(NULL, 1);

    ui_attach_submenu = tui_menu_create("Attach Images", 1);
    tui_menu_add(ui_attach_submenu, ui_attach_menu_def);

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
        tui_menu_add_item(ui_datasette_submenu, "Re_cord",
                  "Press the RECORD key of the datassette",
                  datasette_callback, (void *)DATASETTE_CONTROL_RECORD, 0,
                  TUI_MENU_BEH_RESUME);
        tui_menu_add_item(ui_datasette_submenu, "R_eset",
                  "Rewind the tape and stop the datasette",
                  datasette_callback, (void *)DATASETTE_CONTROL_RESET, 0,
                  TUI_MENU_BEH_RESUME);
        tui_menu_add_item(ui_datasette_submenu, "Reset C_ounter",
                  "Set the datasette counter to '000'",
                  datasette_callback, (void *)DATASETTE_CONTROL_RESET_COUNTER, 0,
                  TUI_MENU_BEH_RESUME);

        tui_menu_add_submenu(ui_main_menu, "Datassett_e Control...",
                             "Press some buttons on the emulated datassette",
                             ui_datasette_submenu, NULL, 0,
                             TUI_MENU_BEH_CONTINUE);
    }

    tui_menu_add_separator(ui_main_menu);

    create_ui_video_submenu();
    tui_menu_add_submenu(ui_main_menu, "_Video Settings...",
                         "Screen parameters",
                         ui_video_submenu, NULL, 0,
                         TUI_MENU_BEH_CONTINUE);

    if (has_drive)
        uidrive_init(ui_main_menu);

    if (has_datasette) {
        ui_datasette_settings_submenu = tui_menu_create("Datasette Settings", 1);
        tui_menu_add(ui_datasette_settings_submenu,datasette_settings_submenu);
        tui_menu_add_submenu(ui_main_menu, "Da_tasette Settings...",
                             "Datasette settings",
                             ui_datasette_settings_submenu, NULL, 0,
                             TUI_MENU_BEH_CONTINUE);
    }

    uisound_init(ui_main_menu);

    uijoystick_init(ui_main_menu, number_joysticks);
#if 0
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
#endif

    ui_rom_submenu = tui_menu_create("Firmware ROM Settings", 1);
    tui_menu_add(ui_rom_submenu, rom_submenu);
    tui_menu_add_submenu(ui_main_menu, "Firm_ware ROM Settings...",
             "Firmware ROMs the emulator is using",
             ui_rom_submenu, NULL, 0,
             TUI_MENU_BEH_CONTINUE);

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
                         "VICE is Free Software distributed under"
             " the GNU General Public License!",
                         ui_info_submenu, NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}

