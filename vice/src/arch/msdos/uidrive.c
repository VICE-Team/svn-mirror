/*
 * uidrive.c
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "attach.h"
#include "drive.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "tui.h"
#include "tuimenu.h"
#include "uidrive.h"

/* machine masks */
#define MM_NONE   (1 << VICE_MACHINE_NONE)
#define MM_C64    (1 << VICE_MACHINE_C64)
#define MM_C128   (1 << VICE_MACHINE_C128)
#define MM_VIC20  (1 << VICE_MACHINE_VIC20)
#define MM_PET    (1 << VICE_MACHINE_PET)
#define MM_CBM5x0 (1 << VICE_MACHINE_CBM5x0)
#define MM_CBM6x0 (1 << VICE_MACHINE_CBM6x0)
#define MM_PLUS4  (1 << VICE_MACHINE_PLUS4)
#define MM_C64DTV (1 << VICE_MACHINE_C64DTV)
#define MM_C64SC  (1 << VICE_MACHINE_C64SC)

/* special case masks */
#define MM_C64ASC (MM_C64 | MM_C64SC)
#define MM_C64ALL (MM_C64ASC | MM_C64DTV)
#define MM_IEC    (MM_C64ALL | MM_C128 | MM_VIC20 | MM_PLUS4)
#define MM_IEEE   (MM_PET | MM_CBM5x0 | MM_CBM6x0 | MM_C64ASC | MM_C128 | MM_VIC20)
#define MM_ALL    (MM_IEC | MM_IEEE)

typedef struct drive_caps_s {
    int type;
    char *name;
    int machines;
    int ram;
    int ram2000;
    int ram4000;
    int ram6000;
    int ram8000;
    int rama000;
    int idle;
    int par;
} drive_caps_t;

typedef struct drive_menu_s {
    tui_menu_t menu;
    char *name;
} drive_menu_t;

typedef struct radio_items_s {
    char *name;
    int value;
} radio_items_t;

static drive_caps_t uidrives[] = {
    { DRIVE_TYPE_NONE, "None", MM_ALL,
      0, 0, 0, 0, 0, 0,
      0, 0 },
    { ATTACH_DEVICE_FS, "Directory", MM_IEC,
      0, 0, 0, 0, 0, 0,
      0, 0 },
/* no real iec device support yet */
#if 0
    { ATTACH_DEVICE_REAL, "Real IEC device", NM_IEC,
      0, 0, 0, 0, 0, 0,
      0, 0 },
#endif
    { ATTACH_DEVICE_RAW, "Raw access", MM_IEC,
      0, 0, 0, 0, 0, 0,
      0, 0 },
    { DRIVE_TYPE_1541, "1541", MM_IEC,
      1, 1, 1, 1, 1, 1,
      1, 1 },
    { DRIVE_TYPE_1541II, "1541-II", MM_IEC,
      1, 1, 1, 1, 1, 1,
      1, 1 },
    { DRIVE_TYPE_1551, "1551", MM_PLUS4,
      0, 0, 0, 0, 0, 0,
      1, 0 },
    { DRIVE_TYPE_1570, "1570", MM_IEC,
      1, 0, 0, 1, 1, 0,
      0, 1 },
    { DRIVE_TYPE_1571, "1571", MM_IEC,
      1, 0, 0, 1, 1, 0,
      0, 1 },
    { DRIVE_TYPE_1571CR, "1571CR", MM_C128,
      1, 0, 0, 1, 1, 0,
      0, 1 },
    { DRIVE_TYPE_1581, "1581", MM_IEC,
      0, 0, 0, 0, 0, 0,
      0, 0 },
    { DRIVE_TYPE_2031, "2031", MM_IEEE,
      0, 0, 0, 0, 0, 0,
      0, 0 },
    { DRIVE_TYPE_2040, "2040", MM_IEEE,
      0, 0, 0, 0, 0, 0,
      0, 0 },
    { DRIVE_TYPE_3040, "3040", MM_IEEE,
      0, 0, 0, 0, 0, 0,
      0, 0 },
    { DRIVE_TYPE_4040, "4040", MM_IEEE,
      0, 0, 0, 0, 0, 0,
      0, 0 },
    { DRIVE_TYPE_1001, "1001", MM_IEEE,
      0, 0, 0, 0, 0, 0,
      0, 0 },
    { DRIVE_TYPE_8050, "8050", MM_IEEE,
      0, 0, 0, 0, 0, 0,
      0, 0 },
    { DRIVE_TYPE_8250, "8250", MM_IEEE,
      0, 0, 0, 0, 0, 0,
      0, 0 },
    { -1, NULL, MM_NONE,
      0, 0, 0, 0, 0, 0,
      0, 0 }
};

static int has_fs(void)
{
    if (machine_class == VICE_MACHINE_CBM5x0 || machine_class == VICE_MACHINE_CBM6x0 || machine_class == VICE_MACHINE_PET) {
        return 0;
    }
    return 1;
}

static int is_fs(int type)
{
    return ((type == 0 || type == ATTACH_DEVICE_FS || type == ATTACH_DEVICE_REAL || type == ATTACH_DEVICE_RAW) && has_fs());
}

static int get_drive_type(int drive)
{
    int iecdevice = 0;
    int fsdevice;
    int drivetype;

    if (has_fs()) {
        resources_get_int_sprintf("IECDevice%i", &iecdevice, drive);
        resources_get_int_sprintf("FileSystemDevice%i", &fsdevice, drive);
    }
    resources_get_int_sprintf("Drive%iType", &drivetype, drive);
    if (iecdevice) {
        return fsdevice;
    } else {
        return drivetype;
    }
}

static int check_current_drive_type(int type, int drive)
{
    int iecdevice = 0;
    int fsdevice;
    int drivetype;

    if (has_fs()) {
        resources_get_int_sprintf("IECDevice%i", &iecdevice, drive);
        resources_get_int_sprintf("FileSystemDevice%i", &fsdevice, drive);
    }
    resources_get_int_sprintf("Drive%iType", &drivetype, drive);
    if (iecdevice) {
        if (type == fsdevice) {
            return 1;
        }
    } else {
        if (type == drivetype) {
            return 1;
        }
    }
    return 0;
}

static TUI_MENU_CALLBACK(current_type_string_callback)
{
    int drive = (int)param;
    int type = get_drive_type(drive);
    int i;

    for (i = 0; uidrives[i].type != type; i++) {}

    return uidrives[i].name;
}

static TUI_MENU_CALLBACK(radio_type_check_callback)
{
    int drive = (int)param >> 16;
    int type = (int)param & 0xffff;
    int value;
    int i;
    int support = (is_fs(type) || drive_check_type(type, drive - 8));

    if (!support) {
        return "N/A";
    }

    if (been_activated) {
        if (is_fs(type)) {
            resources_set_int_sprintf("IECDevice%i", 1, drive);
            resources_set_int_sprintf("FileSystemDevice%i", type, drive);
        } else {
            if (has_fs()) {
                resources_set_int_sprintf("IECDevice%i", 0, drive);
            }
            resources_set_int_sprintf("Drive%iType", type, drive);
        }
        *become_default = 1;
        ui_update_menus();
    } else {
        if (check_current_drive_type(type, drive)) {
            *become_default = 1;
        }
    }
}

static TUI_MENU_CALLBACK(set_directory_callback)
{
    char s[256];
    int drive = (int)param;
    const char *v;

    if (!(check_current_drive_type(ATTACH_DEVICE_FS, drive))) {
        return "N/A";
    }

    if (been_activated) {

        *s = '\0';

        if (tui_input_string("Change directory name", "New name:", s, 255) == -1) {
            return NULL;
        }

        if (*s == '\0') {
            return NULL;
        }

        resources_set_string_sprintf("FSDevice%iDir", s, drive);
        ui_update_menus();
    }

    resources_get_string_sprintf("FSDevice%iDir", &v, drive);

    return v;
}

static TUI_MENU_CALLBACK(toggle_read_p00_callback)
{
    int drive = (int)param;
    int read_p00;

    if (been_activated) {
        resources_get_int_sprintf("FSDevice%iConvertP00", &read_p00, drive);
        resources_set_int_sprintf("FSDevice%iConvertP00", !read_p00, drive);
        ui_update_menus();
    }
    resources_get_int_sprintf("FSDevice%iConvertP00", &read_p00, drive);

    return (read_p00) ? "On" : "Off";
}

static TUI_MENU_CALLBACK(toggle_write_p00_callback)
{
    int drive = (int)param;
    int write_p00;

    if (been_activated) {
        resources_get_int_sprintf("FSDevice%iSaveP00", &write_p00, drive);
        resources_set_int_sprintf("FSDevice%iSaveP00", !write_p00, drive);
        ui_update_menus();
    }
    resources_get_int_sprintf("FSDevice%iSaveP00", &write_p00, drive);

    return (write_p00) ? "On" : "Off";
}

static TUI_MENU_CALLBACK(toggle_hide_p00_callback)
{
    int drive = (int)param;
    int hide_p00;

    if (been_activated) {
        resources_get_int_sprintf("FSDevice%iHideCBMFiles", &hide_p00, drive);
        resources_set_int_sprintf("FSDevice%iHideCBMFiles", !hide_p00, drive);
        ui_update_menus();
    }
    resources_get_int_sprintf("FSDevice%iHideCBMFiles", &hide_p00, drive);

    return (hide_p00) ? "On" : "Off";
}

static TUI_MENU_CALLBACK(toggle_par_callback)
{
    int drive = (int)param;
    int type = get_drive_type(drive);
    int par_support = drive_check_parallel_cable(type);
    int par;

    if (!par_support || machine_class == VICE_MACHINE_VIC20) {
        return "N/A";
    }

    if (been_activated) {
        resources_get_int_sprintf("Drive%iParallelCable", &par, drive);
        resources_set_int_sprintf("Drive%iParallelCable", !par, drive);
        ui_update_menus();
    }
    resources_get_int_sprintf("Drive%iParallelCable", &par, drive);

    return (par) ? "On" : "Off";
}

static TUI_MENU_CALLBACK(current_extend_string_callback)
{
    int drive = (int)param;
    int type = get_drive_type(drive);
    int extend_support =  drive_check_extend_policy(type);
    int res_value;
    char *s;

    if (!extend_support) {
        return "N/A";
    }

    resources_get_int_sprintf("Drive%iExtendImagePolicy", &res_value, drive);

    switch (res_value) {
        case DRIVE_EXTEND_NEVER:
        default:
            s = "Never extend";
            break;
        case DRIVE_EXTEND_ASK:
            s = "Ask to extend";
            break;
        case DRIVE_EXTEND_ACCESS:
            s = "Extend on access";
            break;
    }
    return s;
}

static TUI_MENU_CALLBACK(radio_extend_check_callback)
{
    int drive = (int)param >> 16;
    int value = (int)param & 0xffff;
    int type = get_drive_type(drive);
    int extend_support =  drive_check_extend_policy(type);
    int res_value;

    if (!extend_support) {
        return "N/A";
    }

    if (been_activated) {
        resources_set_int_sprintf("Drive%iExtendImagePolicy", value, drive);
        *become_default = 1;
        ui_update_menus();
    } else {
        resources_get_int_sprintf("Drive%iExtendImagePolicy", &res_value, drive);
        if (res_value == value) {
            *become_default = 1;
        }
    }
}

static TUI_MENU_CALLBACK(current_expansion_string_callback)
{
    int drive = (int)param;
    int type = get_drive_type(drive);
    int i;
    char *s = NULL;

    for (i = 0; uidrives[i].type != type; i++) {}
    if (!uidrives[i].ram) {
        s = "N/A";
    }
    return s;
}

static TUI_MENU_CALLBACK(radio_expansion_check_callback)
{
    int drive = (int)param >> 16;
    int value = (int)param & 0xffff;
    int type = get_drive_type(drive);
    int ram_support;
    int res_value;
    int i;

    for (i = 0; uidrives[i].type != type; i++) {}
    switch (value) {
        default:
        case 0x2000:
            ram_support = uidrives[i].ram2000;
            break;
        case 0x4000:
            ram_support = uidrives[i].ram4000;
            break;
        case 0x6000:
            ram_support = uidrives[i].ram6000;
            break;
        case 0x8000:
            ram_support = uidrives[i].ram8000;
            break;
        case 0xA000:
            ram_support = uidrives[i].rama000;
            break;
    }

    if (!ram_support) {
        return "N/A";
    }

    if (been_activated) {
        resources_get_int_sprintf("Drive%iRAM%X", &res_value, drive, value);
        resources_set_int_sprintf("Drive%iRAM%X", !res_value, drive, value);
        ui_update_menus();
    }

    resources_get_int_sprintf("Drive%iRAM%X", &res_value, drive, value);

    return (res_value) ? "On" : "Off";
}

static TUI_MENU_CALLBACK(current_idle_string_callback)
{
    int drive = (int)param;
    int type = get_drive_type(drive);
    int idle_support = drive_check_idle_method(type);
    int res_value;
    char *s;

    if (!idle_support) {
        return "N/A";
    }

    resources_get_int_sprintf("Drive%iIdleMethod", &res_value, drive);

    switch (res_value) {
        default:
        case DRIVE_IDLE_NO_IDLE:
            s = "None";
            break;
        case DRIVE_IDLE_SKIP_CYCLES:
            s = "Skip cycles";
            break;
        case DRIVE_IDLE_TRAP_IDLE:
            s = "Trap idle";
            break;
    }
    return s;
}

static TUI_MENU_CALLBACK(radio_idle_check_callback)
{
    int drive = (int)param >> 16;
    int value = (int)param & 0xffff;
    int type = get_drive_type(drive);
    int idle_support = drive_check_idle_method(type);
    int res_value;

    if (!idle_support) {
        return "N/A";
    }

    if (been_activated) {
        resources_set_int_sprintf("Drive%iIdleMethod", value, drive);
        *become_default = 1;
        ui_update_menus();
    } else {
        resources_get_int_sprintf("Drive%iIdleMethod", &res_value, drive);
        if (res_value == value) {
            *become_default = 1;
        }
    }
}

void uidrive_init(struct tui_menu *parent_submenu)
{
    drive_menu_t directory_submenu[4] = {
        { NULL, "Drive 8 directory settings" },
        { NULL, "Drive 9 directory settings" },
        { NULL, "Drive 10 directory settings" },
        { NULL, "Drive 11 directory settings" },
    };

    drive_menu_t type_submenu[4] = {
        { NULL, "Drive 8 type" },
        { NULL, "Drive 9 type" },
        { NULL, "Drive 10 type" },
        { NULL, "Drive 11 type" }
    };

    drive_menu_t drive_submenu[4] = {
        { NULL, "Drive 8 settings" },
        { NULL, "Drive 9 settings" },
        { NULL, "Drive 10 settings" },
        { NULL, "Drive 11 settings" }
    };

    drive_menu_t extend_submenu[4] = {
        { NULL, "Drive 8 40 track handling" },
        { NULL, "Drive 9 40 track handling" },
        { NULL, "Drive 10 40 track handling" },
        { NULL, "Drive 11 40 track handling" }
    };

    drive_menu_t expansion_submenu[4] = {
        { NULL, "Drive 8 RAM expansion" },
        { NULL, "Drive 9 RAM expansion" },
        { NULL, "Drive 10 RAM expansion" },
        { NULL, "Drive 11 RAM expansion" },
    };

    drive_menu_t idle_submenu[4] = {
        { NULL, "Drive 8 idle method" },
        { NULL, "Drive 9 idle method" },
        { NULL, "Drive 10 idle method" },
        { NULL, "Drive 11 idle method" },
    };

    drive_menu_t par_submenu[4] = {
        { NULL, "Drive 8 parallel cable" },
        { NULL, "Drive 9 parallel cable" },
        { NULL, "Drive 10 parallel cable" },
        { NULL, "Drive 11 parallel cable" },
    };

    radio_items_t extend_items[] = {
        { "Never extend", DRIVE_EXTEND_NEVER },
        { "Ask for extend", DRIVE_EXTEND_ASK },
        { "Extend on access", DRIVE_EXTEND_ACCESS },
        { NULL, -1 }
    };

    radio_items_t expansion_items[] = {
        { "RAM at $2000-$3FFF", 0x2000 },
        { "RAM at $4000-$5FFF", 0x4000 },
        { "RAM at $6000-$7FFF", 0x6000 },
        { "RAM at $8000-$9FFF", 0x8000 },
        { "RAM at $A000-$BFFF", 0xA000 },
        { NULL, -1 }
    };

    radio_items_t idle_items[] = {
        { "None", DRIVE_IDLE_NO_IDLE },
        { "Skip cycles", DRIVE_IDLE_SKIP_CYCLES },
        { "Trap idle", DRIVE_IDLE_TRAP_IDLE },
        { NULL, -1 }
    };

    tui_menu_t ui_drive_submenu;
    int i, j;

    ui_drive_submenu = tui_menu_create("Drive settings", 1);
    for (i = 0; i < 4; i++) {
        type_submenu[i].menu = tui_menu_create(type_submenu[i].name, 1);
        for (j = 0; uidrives[j].type != -1; j++) {
            if ((1 << machine_class) & uidrives[j].machines) {
                tui_menu_add_item(type_submenu[i].menu, uidrives[j].name,
                                  uidrives[j].name,
                                  radio_type_check_callback,
                                  (void *)(((i + 8) << 16) | uidrives[j].type), 3,
                                  TUI_MENU_BEH_CLOSE);
            }
        }

        directory_submenu[i].menu = tui_menu_create(directory_submenu[i].name, 1);

        tui_menu_add_item(directory_submenu[i].menu, "Directory",
                          "Choose directory",
                          set_directory_callback,
                          (void *)(i + 8), 20,
                          TUI_MENU_BEH_CONTINUE);

        tui_menu_add_item(directory_submenu[i].menu, "Read p00 files",
                          "Read p00 files",
                          toggle_read_p00_callback,
                          (void *)(i + 8), 3,
                          TUI_MENU_BEH_CONTINUE);

        tui_menu_add_item(directory_submenu[i].menu, "Write p00 files",
                          "Write p00 files",
                          toggle_write_p00_callback,
                          (void *)(i + 8), 3,
                          TUI_MENU_BEH_CONTINUE);

        tui_menu_add_item(directory_submenu[i].menu, "Hide non-p00 files",
                          "Hide non-p00 files",
                          toggle_hide_p00_callback,
                          (void *)(i + 8), 3,
                          TUI_MENU_BEH_CONTINUE);

        extend_submenu[i].menu = tui_menu_create(extend_submenu[i].name, 1);

        for (j = 0; extend_items[j].name != NULL; j++) {
            tui_menu_add_item(extend_submenu[i].menu, extend_items[j].name,
                              extend_items[j].name,
                              radio_extend_check_callback,
                              (void *)(((i + 8) << 16) | extend_items[j].value), 20,
                              TUI_MENU_BEH_CLOSE);
        }

        expansion_submenu[i].menu = tui_menu_create(expansion_submenu[i].name, 1);

        for (j = 0; expansion_items[j].name != NULL; j++) {
            tui_menu_add_item(expansion_submenu[i].menu, expansion_items[j].name,
                              expansion_items[j].name,
                              radio_expansion_check_callback,
                              (void *)(((i + 8) << 16) | expansion_items[j].value), 20,
                              TUI_MENU_BEH_CONTINUE);
        }

        idle_submenu[i].menu = tui_menu_create(idle_submenu[i].name, 1);

        for (j = 0; idle_items[j].name != NULL; j++) {
            tui_menu_add_item(idle_submenu[i].menu, idle_items[j].name,
                              idle_items[j].name,
                              radio_idle_check_callback,
                              (void *)(((i + 8) << 16) | idle_items[j].value), 20,
                              TUI_MENU_BEH_CLOSE);
        }

        drive_submenu[i].menu = tui_menu_create(drive_submenu[i].name, 1);

        tui_menu_add_submenu(drive_submenu[i].menu, type_submenu[i].name,
                             type_submenu[i].name,
                             type_submenu[i].menu,
                             current_type_string_callback,
                             (void *)i + 8, 20);

        tui_menu_add_submenu(drive_submenu[i].menu, directory_submenu[i].name,
                             directory_submenu[i].name,
                             directory_submenu[i].menu,
                             NULL, 0, 0);

        tui_menu_add_submenu(drive_submenu[i].menu, extend_submenu[i].name,
                             extend_submenu[i].name,
                             extend_submenu[i].menu,
                             current_extend_string_callback,
                             (void *)i + 8, 20);

        tui_menu_add_submenu(drive_submenu[i].menu, expansion_submenu[i].name,
                             expansion_submenu[i].name,
                             expansion_submenu[i].menu,
                             current_expansion_string_callback,
                             (void *)i + 8, 3);

        tui_menu_add_submenu(drive_submenu[i].menu, idle_submenu[i].name,
                             idle_submenu[i].name,
                             idle_submenu[i].menu,
                             current_idle_string_callback,
                             (void *)i + 8, 20);

        tui_menu_add_item(drive_submenu[i].menu, "Parallel cable",
                          "Parallel cable",
                          toggle_par_callback,
                          (void *)(i + 8), 3,
                          TUI_MENU_BEH_CONTINUE);

        tui_menu_add_submenu(ui_drive_submenu, drive_submenu[i].name,
                             drive_submenu[i].name,
                             drive_submenu[i].menu,
                             NULL, NULL, 0);
    }

    tui_menu_add_submenu(parent_submenu, "Drive settings...",
                         "Drive emulation settings",
                         ui_drive_submenu,
                         NULL, NULL, 0);
}
