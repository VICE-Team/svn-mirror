/*
 * menu_drive.c - Implementation of the common drive settings menu for the SDL UI.
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

#include "attach.h"
#include "autostart-prg.h"
#include "drive.h"
#include "diskimage.h"
#include "fliplist.h"
#include "lib.h"
#include "machine.h"
#include "menu_common.h"
#include "menu_drive.h"
#include "resources.h"
#include "ui.h"
#include "uifilereq.h"
#include "uimenu.h"
#include "uimsgbox.h"
#include "util.h"
#include "vdrive-internal.h"

enum {
    UI_FLIP_ADD,
    UI_FLIP_REMOVE,
    UI_FLIP_NEXT,
    UI_FLIP_PREVIOUS,
    UI_FLIP_LOAD,
    UI_FLIP_SAVE
};

static int check_memory_expansion(int memory, int type)
{
    switch (memory) {
        case 0x2000:
            return drive_check_expansion2000(type);
            break;
        case 0x4000:
            return drive_check_expansion4000(type);
            break;
        case 0x6000:
            return drive_check_expansion6000(type);
            break;
        case 0x8000:
            return drive_check_expansion8000(type);
            break;
        default:
            return drive_check_expansionA000(type);
    }
    return 0;
}

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

static char *get_drive_type_string(int drive)
{
    int type;

    type = get_drive_type(drive);
    switch (type) {
        case 0:
            return "-> none";
            break;
        case ATTACH_DEVICE_FS:
            return "-> directory";
            break;
#ifdef HAVE_OPENCBM
        case ATTACH_DEVICE_REAL:
            return "-> real drive";
            break;
#endif
#ifdef HAVE_RAWDRIVE
        case ATTACH_DEVICE_RAW:
            return "-> block device";
            break;
#endif
        case DRIVE_TYPE_1541II:
            return "-> 1541-II";
            break;
        case DRIVE_TYPE_1551:
            return "-> 1551";
            break;
        case DRIVE_TYPE_1570:
            return "-> 1570";
            break;
        case DRIVE_TYPE_1571:
            return "-> 1571";
            break;
        case DRIVE_TYPE_1571CR:
            return "-> 1571CR";
            break;
        case DRIVE_TYPE_1581:
            return "-> 1581";
            break;
        case DRIVE_TYPE_2031:
            return "-> 2031";
            break;
        case DRIVE_TYPE_2040:
            return "-> 2040";
            break;
        case DRIVE_TYPE_3040:
            return "-> 3040";
            break;
        case DRIVE_TYPE_4040:
            return "-> 4040";
            break;
        case DRIVE_TYPE_1001:
            return "-> 1001";
            break;
        case DRIVE_TYPE_8050:
            return "-> 8050";
            break;
        case DRIVE_TYPE_8250:
            return "-> 8250";
            break;
        default:
            return "-> 1541";
            break;
    }
}

UI_MENU_DEFINE_TOGGLE(DriveTrueEmulation)
UI_MENU_DEFINE_TOGGLE(VirtualDevices)

static UI_MENU_CALLBACK(set_hide_p00_files_callback)
{
    int drive;
    int hide_p00;

    drive = vice_ptr_to_int(param);
    if (activated) {
        resources_get_int_sprintf("FSDevice%iHideCBMFiles", &hide_p00, drive);
        resources_set_int_sprintf("FSDevice%iHideCBMFiles", !hide_p00, drive);
    } else {
        resources_get_int_sprintf("FSDevice%iHideCBMFiles", &hide_p00, drive);
        if (hide_p00) {
            return sdl_menu_text_tick;
        }
    }
    return NULL;
}

static UI_MENU_CALLBACK(set_write_p00_files_callback)
{
    int drive;
    int write_p00;

    drive = vice_ptr_to_int(param);
    if (activated) {
        resources_get_int_sprintf("FSDevice%iSaveP00", &write_p00, drive);
        resources_set_int_sprintf("FSDevice%iSaveP00", !write_p00, drive);
    } else {
        resources_get_int_sprintf("FSDevice%iSaveP00", &write_p00, drive);
        if (write_p00) {
            return sdl_menu_text_tick;
        }
    }
    return NULL;
}

static UI_MENU_CALLBACK(set_read_p00_files_callback)
{
    int drive;
    int read_p00;

    drive = vice_ptr_to_int(param);
    if (activated) {
        resources_get_int_sprintf("FSDevice%iConvertP00", &read_p00, drive);
        resources_set_int_sprintf("FSDevice%iConvertP00", !read_p00, drive);
    } else {
        resources_get_int_sprintf("FSDevice%iConvertP00", &read_p00, drive);
        if (read_p00) {
            return sdl_menu_text_tick;
        }
    }
    return NULL;
}

static UI_MENU_CALLBACK(set_directory_callback)
{
    char *name;
    int drive;

    drive = vice_ptr_to_int(param);
    if (activated) {
        if (check_current_drive_type(ATTACH_DEVICE_FS, drive)) {
            name = sdl_ui_file_selection_dialog("Select directory", FILEREQ_MODE_CHOOSE_DIR);
            if (name != NULL) {
                resources_set_string_sprintf("FSDevice%iDir", name, drive);
                lib_free(name);
            }
        }
    } else {
        if (!check_current_drive_type(ATTACH_DEVICE_FS, drive)) {
            return "(N/A)";
        }
    }
    return NULL;
}

static UI_MENU_CALLBACK(attach_disk_callback)
{
    char *name;

    if (activated) {
        name = sdl_ui_file_selection_dialog("Select disk image", FILEREQ_MODE_CHOOSE_FILE);
        if (name != NULL) {
            if (file_system_attach_disk(vice_ptr_to_int(param), name) < 0) {
                ui_error("Cannot attach disk image.");
            }
            lib_free(name);
        }
    }
    return NULL;
}

static UI_MENU_CALLBACK(detach_disk_callback)
{
    int parameter;

    if (activated) {
        parameter = vice_ptr_to_int(param);
        if (parameter == 0) {
           file_system_detach_disk(8);
           file_system_detach_disk(9);
           file_system_detach_disk(10);
           file_system_detach_disk(11);
        } else {
           file_system_detach_disk(parameter);
        }
    }
    return NULL;
}

static UI_MENU_CALLBACK(fliplist_callback)
{
    char *name;

    if (activated) {
        switch (vice_ptr_to_int(param)) {
            case UI_FLIP_ADD:
               fliplist_add_image(8);
               break;
            case UI_FLIP_REMOVE:
               fliplist_remove(8, NULL);
               break;
            case UI_FLIP_NEXT:
               fliplist_attach_head(8, 1);
               break;
            case UI_FLIP_PREVIOUS:
               fliplist_attach_head(8, 0);
               break;
            case UI_FLIP_LOAD:
                name = sdl_ui_file_selection_dialog("Select fliplist to load", FILEREQ_MODE_CHOOSE_FILE);
                if (name != NULL) {
                    if (fliplist_load_list((unsigned int)-1, name, 0) != 0) {
                        ui_error("Cannot load fliplist.");
                    }
                    lib_free(name);
               }
               break;
            case UI_FLIP_SAVE:
            default:
                name = sdl_ui_file_selection_dialog("Select fliplist to save", FILEREQ_MODE_CHOOSE_FILE);
                if (name != NULL) {
                    util_add_extension(&name, "vfl");
                    if (fliplist_save_list((unsigned int)-1, name) != 0) {
                        ui_error("Cannot save fliplist.");
                    }
                    lib_free(name);
               }
               break;
        }
    }
    return NULL;
}

static UI_MENU_CALLBACK(drive_8_show_idle_callback)
{
    int type;

    type = get_drive_type(8);

    if (drive_check_idle_method(type)) {
        return "->";
    }
    return "(N/A)";
}

static UI_MENU_CALLBACK(drive_9_show_idle_callback)
{
    int type;

    type = get_drive_type(9);

    if (drive_check_idle_method(type)) {
        return "->";
    }
    return "(N/A)";
}

static UI_MENU_CALLBACK(drive_10_show_idle_callback)
{
    int type;

    type = get_drive_type(10);

    if (drive_check_idle_method(type)) {
        return "->";
    }
    return "(N/A)";
}

static UI_MENU_CALLBACK(drive_11_show_idle_callback)
{
    int type;

    type = get_drive_type(11);

    if (drive_check_idle_method(type)) {
        return "->";
    }
    return "(N/A)";
}

static UI_MENU_CALLBACK(drive_8_show_extend_callback)
{
    int type;

    type = get_drive_type(8);

    if (drive_check_extend_policy(type)) {
        return "->";
    }
    return "(N/A)";
}

static UI_MENU_CALLBACK(drive_9_show_extend_callback)
{
    int type;

    type = get_drive_type(9);

    if (drive_check_extend_policy(type)) {
        return "->";
    }
    return "(N/A)";
}

static UI_MENU_CALLBACK(drive_10_show_extend_callback)
{
    int type;

    type = get_drive_type(10);

    if (drive_check_extend_policy(type)) {
        return "->";
    }
    return "(N/A)";
}

static UI_MENU_CALLBACK(drive_11_show_extend_callback)
{
    int type;

    type = get_drive_type(11);

    if (drive_check_extend_policy(type)) {
        return "->";
    }
    return "(N/A)";
}

static UI_MENU_CALLBACK(drive_8_show_expand_callback)
{
    int type;

    type = get_drive_type(8);

    if (drive_check_expansion(type)) {
        return "->";
    }
    return "(N/A)";
}

static UI_MENU_CALLBACK(drive_9_show_expand_callback)
{
    int type;

    type = get_drive_type(9);

    if (drive_check_expansion(type)) {
        return "->";
    }
    return "(N/A)";
}

static UI_MENU_CALLBACK(drive_10_show_expand_callback)
{
    int type;

    type = get_drive_type(10);

    if (drive_check_expansion(type)) {
        return "->";
    }
    return "(N/A)";
}

static UI_MENU_CALLBACK(drive_11_show_expand_callback)
{
    int type;

    type = get_drive_type(11);

    if (drive_check_expansion(type)) {
        return "->";
    }
    return "(N/A)";
}

static UI_MENU_CALLBACK(drive_8_show_type_callback)
{
    return get_drive_type_string(8);
}

static UI_MENU_CALLBACK(drive_9_show_type_callback)
{
    return get_drive_type_string(9);
}

static UI_MENU_CALLBACK(drive_10_show_type_callback)
{
    return get_drive_type_string(10);
}

static UI_MENU_CALLBACK(drive_11_show_type_callback)
{
    return get_drive_type_string(11);
}

static UI_MENU_CALLBACK(set_idle_callback)
{
    int drive;
    int parameter;
    int current;
    int idle = 0;

    drive = (int)(vice_ptr_to_int(param) >> 8);
    parameter = (int)(vice_ptr_to_int(param) & 0xff);
    current = get_drive_type(drive);

    if (activated) {
        if (drive_check_idle_method(current)) {
            resources_set_int_sprintf("Drive%iIdleMethod", parameter, drive);
        }
    } else {
        if (!drive_check_idle_method(current)) {
            return "(N/A)";
        } else {
            resources_get_int_sprintf("Drive%iIdleMethod", &idle, drive);
            if (idle == parameter) {
                return sdl_menu_text_tick;
            }
        }
    }
    return NULL;
}

static UI_MENU_CALLBACK(set_extend_callback)
{
    int drive;
    int parameter;
    int current;
    int extend = 0;

    drive = (int)(vice_ptr_to_int(param) >> 8);
    parameter = (int)(vice_ptr_to_int(param) & 0xff);
    current = get_drive_type(drive);

    if (activated) {
        if (drive_check_extend_policy(current)) {
            resources_set_int_sprintf("Drive%iExtendImagePolicy", parameter, drive);
        }
    } else {
        if (!drive_check_extend_policy(current)) {
            return "(N/A)";
        } else {
            resources_get_int_sprintf("Drive%iExtendImagePolicy", &extend, drive);
            if (extend == parameter) {
                return sdl_menu_text_tick;
            }
        }
    }
    return NULL;
}

static UI_MENU_CALLBACK(set_par_callback)
{
    int drive;
    int current;
    int par;

    drive = vice_ptr_to_int(param);

    current = get_drive_type(drive);

    if (activated) {
        if (machine_class != VICE_MACHINE_VIC20 && drive_check_parallel_cable(current)) {
            resources_get_int_sprintf("Drive%iParallelCable", &par, drive);
            resources_set_int_sprintf("Drive%iParallelCable", !par, drive);
        }
    } else {
        if (machine_class == VICE_MACHINE_VIC20 || !drive_check_parallel_cable(current)) {
            return "(N/A)";
        } else {
            resources_get_int_sprintf("Drive%iParallelCable", &par, drive);
            if (par) {
                return sdl_menu_text_tick;
            }
        }
    }
    return NULL;
}

static UI_MENU_CALLBACK(set_expand_callback)
{
    int drive;
    int parameter;
    int current;
    int memory;
    
    drive = (int)(vice_ptr_to_int(param) >> 16);
    parameter = (int)(vice_ptr_to_int(param) & 0xffff);

    current = get_drive_type(drive);

    if (activated) {
        if (drive_check_expansion(current) && check_memory_expansion(parameter, current)) {
            resources_get_int_sprintf("Drive%iRAM%X", &memory, drive, parameter);
            resources_set_int_sprintf("Drive%iRAM%X", !memory, drive, parameter);
        }
    } else {
        if (!drive_check_extend_policy(current) || !check_memory_expansion(parameter, current)) {
            return "(N/A)";
        } else {
            resources_get_int_sprintf("Drive%iRAM%X", &memory, drive, parameter);
            if (memory) {
                return sdl_menu_text_tick;
            }
        }
    }
    return NULL;
}

static UI_MENU_CALLBACK(set_drive_type_callback)
{
    int drive;
    int parameter;
    int support;
    int current;

    drive = (int)(vice_ptr_to_int(param) >> 16);
    parameter = (int)(vice_ptr_to_int(param) & 0xffff);
    support = (is_fs(parameter) || drive_check_type(parameter, drive - 8));
    current = check_current_drive_type(parameter, drive);

    if (activated) {
        if (support) {
            if (is_fs(parameter)) {
                resources_set_int_sprintf("IECDevice%i", 1, drive);
                resources_set_int_sprintf("FileSystemDevice%i", parameter, drive);
            } else {
                if (has_fs()) {
                    resources_set_int_sprintf("IECDevice%i", 0, drive);
                }
                resources_set_int_sprintf("Drive%iType", parameter, drive);
            }
        }
    } else {
        if (!support) {
            return "(N/A)";
        } else {
            if (current) {
                return sdl_menu_text_tick;
            }
        }
    }
    return NULL;
}

static int new_disk_image_type = DISK_IMAGE_TYPE_D64;

static UI_MENU_CALLBACK(set_disk_type_callback)
{
    int disk_type;

    disk_type = (int)(vice_ptr_to_int(param));

    if (activated) {
        new_disk_image_type = disk_type;
    } else {
        if (disk_type == new_disk_image_type) {
            return sdl_menu_text_tick;
        }
    }
    return NULL;
}

UI_MENU_CALLBACK(create_disk_image_callback)
{
    char *name = NULL;
    char *format_name;
    int overwrite = 1;

    if (activated) {
        name = sdl_ui_file_selection_dialog("Select diskimage name", FILEREQ_MODE_CHOOSE_FILE);
        if (name != NULL) {
            if (util_file_exists(name)) {
                if (message_box("VICE QUESTION","File exists, do you want to overwrite?", MESSAGE_YESNO) == 1) {
                    overwrite = 0;
                }
            }
            if (overwrite == 1) {
                format_name = lib_msprintf("%s,dsk", name);
                if (vdrive_internal_create_format_disk_image(name, format_name, new_disk_image_type) < 0) {
                    ui_error("Cannot create disk image");
                }
                lib_free(format_name);
            }
            lib_free(name);
        }
    }
    return NULL;
}

static const ui_menu_entry_t create_disk_image_type_menu[] = {
    { "D64",
      MENU_ENTRY_RESOURCE_RADIO,
      set_disk_type_callback,
      (ui_callback_data_t)DISK_IMAGE_TYPE_D64},
    { "D71",
      MENU_ENTRY_RESOURCE_RADIO,
      set_disk_type_callback,
      (ui_callback_data_t)DISK_IMAGE_TYPE_D71},
    { "D80",
      MENU_ENTRY_RESOURCE_RADIO,
      set_disk_type_callback,
      (ui_callback_data_t)DISK_IMAGE_TYPE_D80},
    { "D81",
      MENU_ENTRY_RESOURCE_RADIO,
      set_disk_type_callback,
      (ui_callback_data_t)DISK_IMAGE_TYPE_D81},
    { "D82",
      MENU_ENTRY_RESOURCE_RADIO,
      set_disk_type_callback,
      (ui_callback_data_t)DISK_IMAGE_TYPE_D82},
    { "G64",
      MENU_ENTRY_RESOURCE_RADIO,
      set_disk_type_callback,
      (ui_callback_data_t)DISK_IMAGE_TYPE_G64},
    { "X64",
      MENU_ENTRY_RESOURCE_RADIO,
      set_disk_type_callback,
      (ui_callback_data_t)DISK_IMAGE_TYPE_X64},
    { NULL }
};

static const ui_menu_entry_t create_disk_image_menu[] = {
   { "Image type",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)create_disk_image_type_menu },
    { "Create",
      MENU_ENTRY_DIALOG,
      create_disk_image_callback,
      NULL },
    { NULL }
};

static const ui_menu_entry_t drive_8_type_menu[] = {
    { "None",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(8 * 0x10000)},
    { "Directory",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(ATTACH_DEVICE_FS + (8 * 0x10000))},
#ifdef HAVE_OPENCBM
    { "Real drive",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(ATTACH_DEVICE_REAL + (8 * 0x10000))},
#endif
#ifdef HAVE_RAWDRIVE
    { "Block device",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(ATTACH_DEVICE_RAW + (8 * 0x10000))},
#endif
    { "1541",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1541 + (8 * 0x10000))},
    { "1541-II",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1541II + (8 * 0x10000))},
    { "1551",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1551 + (8 * 0x10000))},
    { "1570",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1570 + (8 * 0x10000))},
    { "1571",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1571 + (8 * 0x10000))},
    { "1571CR",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1571CR + (8 * 0x10000))},
    { "1581",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1581 + (8 * 0x10000))},
    { "2031",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_2031 + (8 * 0x10000))},
    { "2040",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_2040 + (8 * 0x10000))},
    { "3040",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_3040 + (8 * 0x10000))},
    { "4040",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_4040 + (8 * 0x10000))},
    { "1001",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1001 + (8 * 0x10000))},
    { "8050",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_8050 + (8 * 0x10000))},
    { "8250",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_8250 + (8 * 0x10000))},
    { NULL }
};

static const ui_menu_entry_t drive_9_type_menu[] = {
    { "None",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(9 * 0x10000)},
    { "Directory",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(ATTACH_DEVICE_FS + (9 * 0x10000))},
#ifdef HAVE_OPENCBM
    { "Real drive",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(ATTACH_DEVICE_REAL + (9 * 0x10000))},
#endif
#ifdef HAVE_RAWDRIVE
    { "Block device",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(ATTACH_DEVICE_RAW + (9 * 0x10000))},
#endif
    { "1541",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1541 + (9 * 0x10000))},
    { "1541-II",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1541II + (9 * 0x10000))},
    { "1551",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1551 + (9 * 0x10000))},
    { "1570",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1570 + (9 * 0x10000))},
    { "1571",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1571 + (9 * 0x10000))},
    { "1571CR",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1571CR + (9 * 0x10000))},
    { "1581",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1581 + (9 * 0x10000))},
    { "2031",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_2031 + (9 * 0x10000))},
    { "2040",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_2040 + (9 * 0x10000))},
    { "3040",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_3040 + (9 * 0x10000))},
    { "4040",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_4040 + (9 * 0x10000))},
    { "1001",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1001 + (9 * 0x10000))},
    { "8050",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_8050 + (9 * 0x10000))},
    { "8250",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_8250 + (9 * 0x10000))},
    { NULL }
};

static const ui_menu_entry_t drive_10_type_menu[] = {
    { "None",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(10 * 0x10000)},
    { "Directory",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(ATTACH_DEVICE_FS + (10 * 0x10000))},
#ifdef HAVE_OPENCBM
    { "Real drive",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(ATTACH_DEVICE_REAL + (10 * 0x10000))},
#endif
#ifdef HAVE_RAWDRIVE
    { "Block device",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(ATTACH_DEVICE_RAW + (10 * 0x10000))},
#endif
    { "1541",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1541 + (10 * 0x10000))},
    { "1541-II",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1541II + (10 * 0x10000))},
    { "1551",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1551 + (10 * 0x10000))},
    { "1570",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1570 + (10 * 0x10000))},
    { "1571",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1571 + (10 * 0x10000))},
    { "1571CR",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1571CR + (10 * 0x10000))},
    { "1581",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1581 + (10 * 0x10000))},
    { "2031",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_2031 + (10 * 0x10000))},
    { "2040",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_2040 + (10 * 0x10000))},
    { "3040",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_3040 + (10 * 0x10000))},
    { "4040",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_4040 + (10 * 0x10000))},
    { "1001",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1001 + (10 * 0x10000))},
    { "8050",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_8050 + (10 * 0x10000))},
    { "8250",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_8250 + (10 * 0x10000))},
    { NULL }
};

static const ui_menu_entry_t drive_11_type_menu[] = {
    { "None",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(11 * 0x10000)},
    { "Directory",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(ATTACH_DEVICE_FS + (11 * 0x10000))},
#ifdef HAVE_OPENCBM
    { "Real drive",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(ATTACH_DEVICE_REAL + (11 * 0x10000))},
#endif
#ifdef HAVE_RAWDRIVE
    { "Block device",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(ATTACH_DEVICE_RAW + (11 * 0x10000))},
#endif
    { "1541",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1541 + (11 * 0x10000))},
    { "1541-II",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1541II + (11 * 0x10000))},
    { "1551",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1551 + (11 * 0x10000))},
    { "1570",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1570 + (11 * 0x10000))},
    { "1571",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1571 + (11 * 0x10000))},
    { "1571CR",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1571CR + (11 * 0x10000))},
    { "1581",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1581 + (11 * 0x10000))},
    { "2031",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_2031 + (11 * 0x10000))},
    { "2040",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_2040 + (11 * 0x10000))},
    { "3040",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_3040 + (11 * 0x10000))},
    { "4040",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_4040 + (11 * 0x10000))},
    { "1001",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_1001 + (11 * 0x10000))},
    { "8050",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_8050 + (11 * 0x10000))},
    { "8250",
      MENU_ENTRY_OTHER,
      set_drive_type_callback,
      (ui_callback_data_t)(DRIVE_TYPE_8250 + (11 * 0x10000))},
    { NULL }
};

static const ui_menu_entry_t drive_8_fsdir_menu[] = {
    { "Choose directory",
      MENU_ENTRY_DIALOG,
      set_directory_callback,
      (ui_callback_data_t)8},
    { "Read P00 files",
      MENU_ENTRY_OTHER,
      set_read_p00_files_callback,
      (ui_callback_data_t)8},
    { "Write P00 files",
      MENU_ENTRY_OTHER,
      set_write_p00_files_callback,
      (ui_callback_data_t)8},
    { "Hide P00 files",
      MENU_ENTRY_OTHER,
      set_hide_p00_files_callback,
      (ui_callback_data_t)8},
    { NULL }
};

static const ui_menu_entry_t drive_9_fsdir_menu[] = {
    { "Choose directory",
      MENU_ENTRY_DIALOG,
      set_directory_callback,
      (ui_callback_data_t)9},
    { "Read P00 files",
      MENU_ENTRY_OTHER,
      set_read_p00_files_callback,
      (ui_callback_data_t)9},
    { "Write P00 files",
      MENU_ENTRY_OTHER,
      set_write_p00_files_callback,
      (ui_callback_data_t)9},
    { "Hide P00 files",
      MENU_ENTRY_OTHER,
      set_hide_p00_files_callback,
      (ui_callback_data_t)9},
    { NULL }
};

static const ui_menu_entry_t drive_10_fsdir_menu[] = {
    { "Choose directory",
      MENU_ENTRY_DIALOG,
      set_directory_callback,
      (ui_callback_data_t)10},
    { "Read P00 files",
      MENU_ENTRY_OTHER,
      set_read_p00_files_callback,
      (ui_callback_data_t)10},
    { "Write P00 files",
      MENU_ENTRY_OTHER,
      set_write_p00_files_callback,
      (ui_callback_data_t)10},
    { "Hide P00 files",
      MENU_ENTRY_OTHER,
      set_hide_p00_files_callback,
      (ui_callback_data_t)10},
    { NULL }
};

static const ui_menu_entry_t drive_11_fsdir_menu[] = {
    { "Choose directory",
      MENU_ENTRY_DIALOG,
      set_directory_callback,
      (ui_callback_data_t)11},
    { "Read P00 files",
      MENU_ENTRY_OTHER,
      set_read_p00_files_callback,
      (ui_callback_data_t)11},
    { "Write P00 files",
      MENU_ENTRY_OTHER,
      set_write_p00_files_callback,
      (ui_callback_data_t)11},
    { "Hide P00 files",
      MENU_ENTRY_OTHER,
      set_hide_p00_files_callback,
      (ui_callback_data_t)11},
    { NULL }
};

static const ui_menu_entry_t drive_8_extend_menu[] = {
    { "Never extend",
      MENU_ENTRY_OTHER,
      set_extend_callback,
      (ui_callback_data_t)(DRIVE_EXTEND_NEVER + (8 * 256))},
    { "Ask on extend",
      MENU_ENTRY_OTHER,
      set_extend_callback,
      (ui_callback_data_t)(DRIVE_EXTEND_ASK + (8 * 256))},
    { "Extend on access",
      MENU_ENTRY_OTHER,
      set_extend_callback,
      (ui_callback_data_t)(DRIVE_EXTEND_ACCESS + (8 * 256))},
    { NULL }
};

static const ui_menu_entry_t drive_9_extend_menu[] = {
    { "Never extend",
      MENU_ENTRY_OTHER,
      set_extend_callback,
      (ui_callback_data_t)(DRIVE_EXTEND_NEVER + (9 * 256))},
    { "Ask on extend",
      MENU_ENTRY_OTHER,
      set_extend_callback,
      (ui_callback_data_t)(DRIVE_EXTEND_ASK + (9 * 256))},
    { "Extend on access",
      MENU_ENTRY_OTHER,
      set_extend_callback,
      (ui_callback_data_t)(DRIVE_EXTEND_ACCESS + (9 * 256))},
    { NULL }
};

static const ui_menu_entry_t drive_10_extend_menu[] = {
    { "Never extend",
      MENU_ENTRY_OTHER,
      set_extend_callback,
      (ui_callback_data_t)(DRIVE_EXTEND_NEVER + (10 * 256))},
    { "Ask on extend",
      MENU_ENTRY_OTHER,
      set_extend_callback,
      (ui_callback_data_t)(DRIVE_EXTEND_ASK + (10 * 256))},
    { "Extend on access",
      MENU_ENTRY_OTHER,
      set_extend_callback,
      (ui_callback_data_t)(DRIVE_EXTEND_ACCESS + (10 * 256))},
    { NULL }
};

static const ui_menu_entry_t drive_11_extend_menu[] = {
    { "Never extend",
      MENU_ENTRY_OTHER,
      set_extend_callback,
      (ui_callback_data_t)(DRIVE_EXTEND_NEVER + (11 * 256))},
    { "Ask on extend",
      MENU_ENTRY_OTHER,
      set_extend_callback,
      (ui_callback_data_t)(DRIVE_EXTEND_ASK + (11 * 256))},
    { "Extend on access",
      MENU_ENTRY_OTHER,
      set_extend_callback,
      (ui_callback_data_t)(DRIVE_EXTEND_ACCESS + (11 * 256))},
    { NULL }
};

static const ui_menu_entry_t drive_8_expand_menu[] = {
    { "RAM at $2000-$3FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x2000 + (8 * 0x10000))},
    { "RAM at $4000-$5FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x4000 + (8 * 0x10000))},
    { "RAM at $6000-$7FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x6000 + (8 * 0x10000))},
    { "RAM at $8000-$9FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x8000 + (8 * 0x10000))},
    { "RAM at $A000-$BFFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0xa000 + (8 * 0x10000))},
    { NULL }
};

static const ui_menu_entry_t drive_9_expand_menu[] = {
    { "RAM at $2000-$3FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x2000 + (9 * 0x10000))},
    { "RAM at $4000-$5FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x4000 + (9 * 0x10000))},
    { "RAM at $6000-$7FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x6000 + (9 * 0x10000))},
    { "RAM at $8000-$9FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x8000 + (9 * 0x10000))},
    { "RAM at $A000-$BFFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0xa000 + (9 * 0x10000))},
    { NULL }
};

static const ui_menu_entry_t drive_10_expand_menu[] = {
    { "RAM at $2000-$3FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x2000 + (10 * 0x10000))},
    { "RAM at $4000-$5FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x4000 + (10 * 0x10000))},
    { "RAM at $6000-$7FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x6000 + (10 * 0x10000))},
    { "RAM at $8000-$9FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x8000 + (10 * 0x10000))},
    { "RAM at $A000-$BFFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0xa000 + (10 * 0x10000))},
    { NULL }
};

static const ui_menu_entry_t drive_11_expand_menu[] = {
    { "RAM at $2000-$3FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x2000 + (11 * 0x10000))},
    { "RAM at $4000-$5FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x4000 + (11 * 0x10000))},
    { "RAM at $6000-$7FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x6000 + (11 * 0x10000))},
    { "RAM at $8000-$9FFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0x8000 + (11 * 0x10000))},
    { "RAM at $A000-$BFFF",
      MENU_ENTRY_OTHER,
      set_expand_callback,
      (ui_callback_data_t)(0xa000 + (11 * 0x10000))},
    { NULL }
};

static const ui_menu_entry_t drive_8_idle_menu[] = {
    { "None",
      MENU_ENTRY_OTHER,
      set_idle_callback,
      (ui_callback_data_t)(DRIVE_IDLE_NO_IDLE + (8 * 256))},
    { "Trap idle",
      MENU_ENTRY_OTHER,
      set_idle_callback,
      (ui_callback_data_t)(DRIVE_IDLE_SKIP_CYCLES + (8 * 256))},
    { "Skip cycles",
      MENU_ENTRY_OTHER,
      set_idle_callback,
      (ui_callback_data_t)(DRIVE_IDLE_TRAP_IDLE + (8 * 256))},
    { NULL }
};


static const ui_menu_entry_t drive_9_idle_menu[] = {
    { "None",
      MENU_ENTRY_OTHER,
      set_idle_callback,
      (ui_callback_data_t)(DRIVE_IDLE_NO_IDLE + (9 * 256))},
    { "Trap idle",
      MENU_ENTRY_OTHER,
      set_idle_callback,
      (ui_callback_data_t)(DRIVE_IDLE_SKIP_CYCLES + (9 * 256))},
    { "Skip cycles",
      MENU_ENTRY_OTHER,
      set_idle_callback,
      (ui_callback_data_t)(DRIVE_IDLE_TRAP_IDLE + (9 * 256))},
    { NULL }
};

static const ui_menu_entry_t drive_10_idle_menu[] = {
    { "None",
      MENU_ENTRY_OTHER,
      set_idle_callback,
      (ui_callback_data_t)(DRIVE_IDLE_NO_IDLE + (10 * 256))},
    { "Trap idle",
      MENU_ENTRY_OTHER,
      set_idle_callback,
      (ui_callback_data_t)(DRIVE_IDLE_SKIP_CYCLES + (10 * 256))},
    { "Skip cycles",
      MENU_ENTRY_OTHER,
      set_idle_callback,
      (ui_callback_data_t)(DRIVE_IDLE_TRAP_IDLE + (10 * 256))},
    { NULL }
};

static const ui_menu_entry_t drive_11_idle_menu[] = {
    { "None",
      MENU_ENTRY_OTHER,
      set_idle_callback,
      (ui_callback_data_t)(DRIVE_IDLE_NO_IDLE + (11 * 256))},
    { "Trap idle",
      MENU_ENTRY_OTHER,
      set_idle_callback,
      (ui_callback_data_t)(DRIVE_IDLE_SKIP_CYCLES + (11 * 256))},
    { "Skip cycles",
      MENU_ENTRY_OTHER,
      set_idle_callback,
      (ui_callback_data_t)(DRIVE_IDLE_TRAP_IDLE + (11 * 256))},
    { NULL }
};

#ifdef HAVE_RAWDRIVE
UI_MENU_DEFINE_FILE_STRING(RawDriveDriver)
#endif

static const ui_menu_entry_t drive_8_menu[] = {
   { "Drive 8 type",
      MENU_ENTRY_SUBMENU,
      drive_8_show_type_callback,
      (ui_callback_data_t)drive_8_type_menu },
   { "Drive 8 dir settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)drive_8_fsdir_menu },
   { "Drive 8 40 track handling",
      MENU_ENTRY_SUBMENU,
      drive_8_show_extend_callback,
      (ui_callback_data_t)drive_8_extend_menu },
   { "Drive 8 expansion memory",
      MENU_ENTRY_SUBMENU,
      drive_8_show_expand_callback,
      (ui_callback_data_t)drive_8_expand_menu },
   { "Drive 8 idle method",
      MENU_ENTRY_SUBMENU,
      drive_8_show_idle_callback,
      (ui_callback_data_t)drive_8_idle_menu },
   { "Drive 8 parallel cable",
      MENU_ENTRY_OTHER,
      set_par_callback,
      (ui_callback_data_t)8},
#ifdef HAVE_RAWDRIVE
   { "Blockdevice",
      MENU_ENTRY_DIALOG,
      file_string_RawDriveDriver_callback,
      (ui_callback_data_t)"Select device file to use as drive" },
#endif
    { NULL }
};

static const ui_menu_entry_t drive_9_menu[] = {
   { "Drive 9 type",
      MENU_ENTRY_SUBMENU,
      drive_9_show_type_callback,
      (ui_callback_data_t)drive_9_type_menu },
   { "Drive 9 dir settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)drive_9_fsdir_menu },
   { "Drive 9 40 track handling",
      MENU_ENTRY_SUBMENU,
      drive_9_show_extend_callback,
      (ui_callback_data_t)drive_9_extend_menu },
   { "Drive 9 expansion memory",
      MENU_ENTRY_SUBMENU,
      drive_9_show_expand_callback,
      (ui_callback_data_t)drive_9_expand_menu },
   { "Drive 9 idle method",
      MENU_ENTRY_SUBMENU,
      drive_9_show_idle_callback,
      (ui_callback_data_t)drive_9_idle_menu },
   { "Drive 9 parallel cable",
      MENU_ENTRY_OTHER,
      set_par_callback,
      (ui_callback_data_t)9},
#ifdef HAVE_RAWDRIVE
   { "Blockdevice",
      MENU_ENTRY_DIALOG,
      file_string_RawDriveDriver_callback,
      (ui_callback_data_t)"Select device file to use as drive" },
#endif
    { NULL }
};

static const ui_menu_entry_t drive_10_menu[] = {
   { "Drive 10 type",
      MENU_ENTRY_SUBMENU,
      drive_10_show_type_callback,
      (ui_callback_data_t)drive_10_type_menu },
   { "Drive 10 dir settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)drive_10_fsdir_menu },
   { "Drive 10 40 track handling",
      MENU_ENTRY_SUBMENU,
      drive_10_show_extend_callback,
      (ui_callback_data_t)drive_10_extend_menu },
   { "Drive 10 expansion memory",
      MENU_ENTRY_SUBMENU,
      drive_10_show_expand_callback,
      (ui_callback_data_t)drive_10_expand_menu },
   { "Drive 10 idle method",
      MENU_ENTRY_SUBMENU,
      drive_10_show_idle_callback,
      (ui_callback_data_t)drive_10_idle_menu },
   { "Drive 10 parallel cable",
      MENU_ENTRY_OTHER,
      set_par_callback,
      (ui_callback_data_t)10},
#ifdef HAVE_RAWDRIVE
   { "Blockdevice",
      MENU_ENTRY_DIALOG,
      file_string_RawDriveDriver_callback,
      (ui_callback_data_t)"Select device file to use as drive" },
#endif
    { NULL }
};

static const ui_menu_entry_t drive_11_menu[] = {
   { "Drive 11 type",
      MENU_ENTRY_SUBMENU,
      drive_11_show_type_callback,
      (ui_callback_data_t)drive_11_type_menu },
   { "Drive 11 dir settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)drive_11_fsdir_menu },
   { "Drive 11 40 track handling",
      MENU_ENTRY_SUBMENU,
      drive_11_show_extend_callback,
      (ui_callback_data_t)drive_11_extend_menu },
   { "Drive 11 expansion memory",
      MENU_ENTRY_SUBMENU,
      drive_11_show_expand_callback,
      (ui_callback_data_t)drive_11_expand_menu },
   { "Drive 11 idle method",
      MENU_ENTRY_SUBMENU,
      drive_11_show_idle_callback,
      (ui_callback_data_t)drive_11_idle_menu },
   { "Drive 11 parallel cable",
      MENU_ENTRY_OTHER,
      set_par_callback,
      (ui_callback_data_t)11},
#ifdef HAVE_RAWDRIVE
   { "Blockdevice",
      MENU_ENTRY_DIALOG,
      file_string_RawDriveDriver_callback,
      (ui_callback_data_t)"Select device file to use as drive" },
#endif
    { NULL }
};

static const ui_menu_entry_t fliplist_menu[] = {
    { "Add current image to fliplist",
      MENU_ENTRY_OTHER,
      fliplist_callback,
      (ui_callback_data_t)UI_FLIP_ADD },
    { "Remove current image from fliplist",
      MENU_ENTRY_OTHER,
      fliplist_callback,
      (ui_callback_data_t)UI_FLIP_REMOVE },
    { "Attach next image in fliplist",
      MENU_ENTRY_OTHER,
      fliplist_callback,
      (ui_callback_data_t)UI_FLIP_NEXT },
    { "Attach previous image in fliplist",
      MENU_ENTRY_OTHER,
      fliplist_callback,
      (ui_callback_data_t)UI_FLIP_PREVIOUS },
    { "Load fliplist",
      MENU_ENTRY_DIALOG,
      fliplist_callback,
      (ui_callback_data_t)UI_FLIP_LOAD },
    { "Save fliplist",
      MENU_ENTRY_DIALOG,
      fliplist_callback,
      (ui_callback_data_t)UI_FLIP_SAVE },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(AutostartHandleTrueDriveEmulation)
UI_MENU_DEFINE_TOGGLE(AutostartWarp)
UI_MENU_DEFINE_TOGGLE(AutostartRunWithColon)
UI_MENU_DEFINE_RADIO(AutostartPrgMode)
UI_MENU_DEFINE_STRING(AutostartPrgDiskImage)

static const ui_menu_entry_t autostart_settings_menu[] = {
    { "Handle TDE on autostart",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_AutostartHandleTrueDriveEmulation_callback,
      NULL },
    { "Autostart warp",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_AutostartWarp_callback,
      NULL },
    { "Use ':' with RUN",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_AutostartRunWithColon_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Autostart PRG mode"),
    { "VirtualFS",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_AutostartPrgMode_callback,
      (ui_callback_data_t)AUTOSTART_PRG_MODE_VFS },
    { "Inject",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_AutostartPrgMode_callback,
      (ui_callback_data_t)AUTOSTART_PRG_MODE_INJECT },
    { "Disk image",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_AutostartPrgMode_callback,
      (ui_callback_data_t)AUTOSTART_PRG_MODE_DISK },
    SDL_MENU_ITEM_SEPARATOR,
    { "Autostart disk image",
      MENU_ENTRY_RESOURCE_STRING,
      string_AutostartPrgDiskImage_callback,
      (ui_callback_data_t)"Disk image for autostarting PRG files" },
    { NULL }
};

const ui_menu_entry_t drive_menu[] = {
    { "Attach disk image to drive 8",
      MENU_ENTRY_DIALOG,
      attach_disk_callback,
      (ui_callback_data_t)8 },
    { "Attach disk image to drive 9",
      MENU_ENTRY_DIALOG,
      attach_disk_callback,
      (ui_callback_data_t)9 },
    { "Attach disk image to drive 10",
      MENU_ENTRY_DIALOG,
      attach_disk_callback,
      (ui_callback_data_t)10 },
    { "Attach disk image to drive 11",
      MENU_ENTRY_DIALOG,
      attach_disk_callback,
      (ui_callback_data_t)11 },
    { "Detach disk image from drive 8",
      MENU_ENTRY_OTHER,
      detach_disk_callback,
      (ui_callback_data_t)8 },
    { "Detach disk image from drive 9",
      MENU_ENTRY_OTHER,
      detach_disk_callback,
      (ui_callback_data_t)9 },
    { "Detach disk image from drive 10",
      MENU_ENTRY_OTHER,
      detach_disk_callback,
      (ui_callback_data_t)10 },
    { "Detach disk image from drive 11",
      MENU_ENTRY_OTHER,
      detach_disk_callback,
      (ui_callback_data_t)11 },
    { "Detach all disk images",
      MENU_ENTRY_OTHER,
      detach_disk_callback,
      (ui_callback_data_t)0 },
   { "Create new disk image",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)create_disk_image_menu },
    SDL_MENU_ITEM_SEPARATOR,
   { "Drive 8 settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)drive_8_menu },
   { "Drive 9 settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)drive_9_menu },
   { "Drive 10 settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)drive_10_menu },
   { "Drive 11 settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)drive_11_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "True drive emulation",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_DriveTrueEmulation_callback,
      NULL },
    { "Virtual device traps",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VirtualDevices_callback,
      NULL },
   { "Autostart settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)autostart_settings_menu },
   { "Fliplist settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)fliplist_menu },
    { NULL }
};
