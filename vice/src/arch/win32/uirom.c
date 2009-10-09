/*
 * uirom.c - Implementation of the ROM settings dialog box.
 *
 * Written by
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

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>

#ifndef DUMMYUNIONNAME
#define DUMMYUNIONNAME  u1
#endif

#include "intl.h"
#include "lib.h"
#include "machine.h"
#include "res.h"
#include "resources.h"
#include "romset.h"
#include "system.h"
#include "translate.h"
#include "ui.h"
#include "uiapi.h"
#include "uilib.h"
#include "uirom.h"
#include "winmain.h"

#ifdef _WIN64
#define _ANONYMOUS_UNION
#endif

static const uirom_settings_t *settings;

static const unsigned int *romset_dialog_resources;

static uilib_localize_dialog_param *main_trans;
static uilib_localize_dialog_param *drive_trans;
static uilib_localize_dialog_param *main_res_trans;

static uilib_dialog_group *main_left_group;
static uilib_dialog_group *main_middle_group;
static uilib_dialog_group *main_right_group;

static uilib_dialog_group *drive_left_group;
static uilib_dialog_group *drive_middle_group;
static uilib_dialog_group *drive_right_group;

static void init_rom_dialog(HWND hwnd, unsigned int type)
{
    unsigned int n = 0;
    int xpos;

    if (type == UIROM_TYPE_MAIN) {
        /* translate all dialog items */
        uilib_localize_dialog(hwnd, main_trans);

        /* adjust the size of the elements in the main left group */
        uilib_adjust_group_width(hwnd, main_left_group);

        /* get the max x of the main left group */
        uilib_get_group_max_x(hwnd, main_left_group, &xpos);

        /* move the main middle group to the correct position */
        uilib_move_group(hwnd, main_middle_group, xpos + 10);

        /* get the max x of the main middle group */
        uilib_get_group_max_x(hwnd, main_middle_group, &xpos);

        /* move the main right group to the correct position */
        uilib_move_group(hwnd, main_right_group, xpos + 10);

    } else if (type == UIROM_TYPE_DRIVE) {
        /* translate all dialog items */
        uilib_localize_dialog(hwnd, drive_trans);

        /* adjust the size of the elements in the drive left group */
        uilib_adjust_group_width(hwnd, drive_left_group);

        /* get the max x of the drive left group */
        uilib_get_group_max_x(hwnd, drive_left_group, &xpos);

        /* move the drive middle group to the correct position */
        uilib_move_group(hwnd, drive_middle_group, xpos + 10);

        /* get the max x of the drive middle group */
        uilib_get_group_max_x(hwnd, drive_middle_group, &xpos);

        /* move the drive right group to the correct position */
        uilib_move_group(hwnd, drive_right_group, xpos + 10);
    }

    while (settings[n].realname != NULL) {
        if (settings[n].type == type) {
            const char *filename;
            TCHAR *st_filename;

            resources_get_string(settings[n].resname, &filename);
            st_filename = system_mbstowcs_alloc(filename);
            SetDlgItemText(hwnd, settings[n].idc_filename,
                           st_filename != NULL ? st_filename : TEXT(""));
            system_mbstowcs_free(st_filename);
        }
        n++;
    }
}

static void set_dialog_proc(HWND hwnd, unsigned int type)
{
    unsigned int n = 0;

    while (settings[n].realname != NULL) {
        if (settings[n].type == type) {
            char filename[MAX_PATH];
            TCHAR st_filename[MAX_PATH];

            GetDlgItemText(hwnd, settings[n].idc_filename, st_filename,
                           MAX_PATH);
            system_wcstombs(filename, st_filename, MAX_PATH);
            resources_set_string(settings[n].resname, filename);
        }
        n++;
    }
}

static BOOL browse_command(HWND hwnd, unsigned int command)
{
    unsigned int n = 0;

    while (settings[n].realname != NULL) {
        if ((unsigned int)command == settings[n].idc_browse) {
            TCHAR st_realname[100];

            _stprintf(st_realname, translate_text(IDS_LOAD_S_ROM_IMAGE),
                      settings[n].realname);

            uilib_select_browse(hwnd, st_realname,
                                UILIB_FILTER_ALL,
                                UILIB_SELECTOR_TYPE_FILE_LOAD,
                                settings[n].idc_filename);
            return TRUE;
        }
        n++;
    }

    return FALSE;
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam, unsigned int type)
{
    int command;

    switch (msg) {
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_rom_dialog(hwnd, type);
        return TRUE;
      case WM_NOTIFY:
        switch (((NMHDR FAR *)lparam)->code) {
          case PSN_KILLACTIVE:
            set_dialog_proc(hwnd, type);
            return TRUE;
        }
        return FALSE;
      case WM_COMMAND:
        command = LOWORD(wparam);
        return browse_command(hwnd, command);
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
    }
    return FALSE;
}

static INT_PTR CALLBACK dialog_proc_main(HWND hwnd, UINT msg, WPARAM wparam,
                                         LPARAM lparam)
{
    return dialog_proc(hwnd, msg, wparam, lparam, UIROM_TYPE_MAIN);
}

static INT_PTR CALLBACK dialog_proc_drive(HWND hwnd, UINT msg, WPARAM wparam,
                                          LPARAM lparam)
{
    return dialog_proc(hwnd, msg, wparam, lparam, UIROM_TYPE_DRIVE);
}

static void enable_controls_for_romset(HWND hwnd, int idc_active)
{
    int res;

    res = idc_active == IDC_ROMSET_SELECT_ARCHIVE;

    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_NAME), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_BROWSE), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_LOAD), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_SAVE), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_ACTIVE), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_APPLY),  res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_NEW), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_DELETE), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_FILE_NAME), !res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_FILE_BROWSE), !res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_FILE_LOAD), !res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_FILE_SAVE), !res);
}

static void update_romset_list(HWND hwnd)
{
    char *list;
    TCHAR *st_list;

    if (IsDlgButtonChecked(hwnd, IDC_ROMSET_SELECT_ARCHIVE) == BST_CHECKED)
        list = romset_archive_list();
    else
        list = machine_romset_file_list();

    st_list = system_mbstowcs_alloc(list);
    SetDlgItemText(hwnd, IDC_ROMSET_PREVIEW, st_list);
    system_mbstowcs_free(st_list);

    lib_free(list);
}

static void update_romset_archive(HWND hwnd)
{
    HWND temp_hwnd;
    int num, index, active;
    const char *conf;

    active = 0;
    num = romset_archive_get_number();

    resources_get_string("RomsetArchiveActive", &conf);

    temp_hwnd = GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_ACTIVE);
    SendMessage(temp_hwnd, CB_RESETCONTENT, 0, 0);
    for (index = 0; index < num; index++) {
        TCHAR *st_name;
        char *name;

        name = romset_archive_get_item(index);
        if (!strcmp(conf, name))
            active = index;
        st_name = system_mbstowcs_alloc(name);
        SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)st_name);
        system_mbstowcs_free(st_name);
    }
    SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)active, 0);

    update_romset_list(hwnd);
}

static void update_romset_dialog(HWND hwnd, int idc_active)
{
    if (idc_active == IDC_ROMSET_SELECT_ARCHIVE
        || idc_active == IDC_ROMSET_SELECT_FILE) {
        CheckRadioButton(hwnd, IDC_ROMSET_SELECT_ARCHIVE,
                         IDC_ROMSET_SELECT_FILE, idc_active);
        enable_controls_for_romset(hwnd, idc_active);

        update_romset_archive(hwnd);
    }
}

static uilib_localize_dialog_param romset_dialog_trans[] = {
    {IDC_ROMSET_SELECT_ARCHIVE, IDS_ROMSET_SELECT_ARCHIVE, 0},
    {IDC_ROMSET_ARCHIVE_BROWSE, IDS_BROWSE, 0},
    {IDC_ROMSET_ARCHIVE_LOAD, IDS_ROMSET_ARCHIVE_LOAD, 0},
    {IDC_ROMSET_ARCHIVE_SAVE, IDS_ROMSET_ARCHIVE_SAVE, 0},
    {IDC_ROMSET_CONFIGURATION, IDS_ROMSET_CONFIGURATION, 0},
    {IDC_ROMSET_ARCHIVE_APPLY, IDS_ROMSET_ARCHIVE_APPLY, 0},
    {IDC_ROMSET_ARCHIVE_NEW, IDS_ROMSET_ARCHIVE_NEW, 0},
    {IDC_ROMSET_ARCHIVE_DELETE, IDS_ROMSET_ARCHIVE_DELETE, 0},
    {IDC_ROMSET_SELECT_FILE, IDS_ROMSET_SELECT_FILE, 0},
    {IDC_ROMSET_FILE_BROWSE, IDS_BROWSE, 0},
    {IDC_ROMSET_FILE_LOAD, IDS_ROMSET_FILE_LOAD, 0},
    {IDC_ROMSET_FILE_SAVE, IDS_ROMSET_FILE_SAVE, 0},
    {IDC_ROMSET_RESOURCE_COMPUTER, IDS_COMPUTER, 0},
    {IDC_ROMSET_RESOURCE_DRIVE, IDS_DRIVE, 0},
    {0, 0, 0}
};

static uilib_dialog_group romset_main_group[] = {
    {IDC_ROMSET_SELECT_ARCHIVE, 1},
    {IDC_ROMSET_ARCHIVE_BROWSE, 1},
    {IDC_ROMSET_ARCHIVE_LOAD, 1},
    {IDC_ROMSET_ARCHIVE_SAVE, 1},
    {IDC_ROMSET_ARCHIVE_APPLY, 1},
    {IDC_ROMSET_ARCHIVE_NEW, 1},
    {IDC_ROMSET_ARCHIVE_DELETE, 1},
    {IDC_ROMSET_SELECT_FILE, 1},
    {IDC_ROMSET_FILE_BROWSE, 1},
    {IDC_ROMSET_FILE_LOAD, 1},
    {IDC_ROMSET_FILE_SAVE, 1},
    {IDC_ROMSET_RESOURCE_COMPUTER, 1},
    {IDC_ROMSET_RESOURCE_DRIVE, 1},
    {0, 0}
};

static void init_romset_dialog(HWND hwnd)
{
    int res_value, idc_active;
    const char *name;
    TCHAR *st_name;

    /* translate all dialog items */
    uilib_localize_dialog(hwnd, romset_dialog_trans);

    /* adjust the size of the elements in the main group */
    uilib_adjust_group_width(hwnd, romset_main_group);

    resources_get_int("RomsetSourceFile", &res_value);
    idc_active = IDC_ROMSET_SELECT_ARCHIVE + res_value;
    update_romset_dialog(hwnd, idc_active);

    resources_get_string("RomsetArchiveName", &name);
    st_name = system_mbstowcs_alloc(name);
    SetDlgItemText(hwnd, IDC_ROMSET_ARCHIVE_NAME,
                   name != NULL ? st_name : TEXT(""));
    system_mbstowcs_free(st_name);

    resources_get_string("RomsetFileName", &name);
    st_name = system_mbstowcs_alloc(name);
    SetDlgItemText(hwnd, IDC_ROMSET_FILE_NAME,
                   name != NULL ? st_name : TEXT(""));
    system_mbstowcs_free(st_name);
}

static void end_romset_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    if (IsDlgButtonChecked(hwnd, IDC_ROMSET_SELECT_ARCHIVE) == BST_CHECKED)
        resources_set_int("RomsetSourceFile", 0);
    if (IsDlgButtonChecked(hwnd, IDC_ROMSET_SELECT_FILE) == BST_CHECKED)
        resources_set_int("RomsetSourceFile", 1);

    GetDlgItemText(hwnd, IDC_ROMSET_ARCHIVE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_string("RomsetArchiveName", s);

    GetDlgItemText(hwnd, IDC_ROMSET_FILE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_string("RomsetFileName", s);
}

/*
static void browse_archive_romset_dialog(HWND hwnd)
{
    uilib_select_browse(hwnd, translate_text(IDS_SELECT_ROMSET_ARCHIVE),
                        UILIB_FILTER_ALL, UILIB_SELECTOR_TYPE_FILE_LOAD,
                        IDC_ROMSET_ARCHIVE_NAME);
}
*/

static void load_archive_romset_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    GetDlgItemText(hwnd, IDC_ROMSET_ARCHIVE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    if (romset_archive_load(s, 0) < 0)
        ui_error(translate_text(IDS_CANNOT_LOAD_ROMSET_ARCH));

    update_romset_archive(hwnd);
}

static void save_archive_romset_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    GetDlgItemText(hwnd, IDC_ROMSET_ARCHIVE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    if (romset_archive_save(s) < 0)
        ui_error(translate_text(IDS_CANNOT_SAVE_ROMSET_ARCH));
}

static TCHAR *active_archive_name(HWND hwnd)
{
    HWND temp_hwnd;
    int active, len;
    TCHAR *st_name;

    temp_hwnd = GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_ACTIVE);
    active = (int)SendMessage(temp_hwnd, CB_GETCURSEL, 0, 0);
    len = (int)SendMessage(temp_hwnd, CB_GETLBTEXTLEN, active, 0);
    st_name = lib_malloc((len + 1) * sizeof(TCHAR));
    SendMessage(temp_hwnd, CB_GETLBTEXT, active, (LPARAM)st_name);

    return st_name;
}

static void apply_archive_romset_dialog(HWND hwnd)
{
    char *name;
    TCHAR *st_name;

    st_name = active_archive_name(hwnd);
    name = system_wcstombs_alloc(st_name);
    romset_archive_item_select(name);
    system_wcstombs_free(name);
    lib_free(st_name);
}

static void new_archive_romset_dialog(HWND hwnd)
{
    uilib_dialogbox_param_t param;

    param.hwnd = hwnd;
    param.idd_dialog = IDD_ROMSET_ENTER_NAME_DIALOG;
    param.idc_dialog = IDC_ROMSET_ENTER_NAME;
    _tcscpy(param.string, TEXT(""));
    param.idc_dialog_trans = IDC_ENTER_CONFIGURATION_NAME;
    param.idc_dialog_trans_text = translate_text(IDS_ENTER_CONFIGURATION_NAME);
    param.idd_dialog_caption = translate_text(IDS_NEW_CONFIGURATION);

    uilib_dialogbox(&param);

    if (param.updated > 0) {
        machine_romset_archive_item_create(param.string);
        update_romset_archive(hwnd);
    }
}

static void delete_archive_romset_dialog(HWND hwnd)
{
    char *name;
    TCHAR *st_name;

    st_name = active_archive_name(hwnd);
    name = system_wcstombs_alloc(st_name);
    romset_archive_item_delete(name);
    system_wcstombs_free(name);
    lib_free(st_name);

    update_romset_archive(hwnd);
}

static void load_file_romset_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    GetDlgItemText(hwnd, IDC_ROMSET_FILE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);

    if (machine_romset_file_load(s) < 0)
        ui_error(translate_text(IDS_CANNOT_LOAD_ROMSET_FILE));

    update_romset_list(hwnd);
}

static void save_file_romset_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    GetDlgItemText(hwnd, IDC_ROMSET_FILE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    if (machine_romset_file_save(s) < 0)
        ui_error(translate_text(IDS_CANNOT_SAVE_ROMSET_FILE));
}

static uilib_localize_dialog_param drive_res_trans[] = {
    {0, IDS_DRIVE_RESOURCES_CAPTION, -1},
    {IDC_DRIVE_RESOURCES, IDS_DRIVE_RESOURCES, 0},
    {IDOK, IDS_OK, 0},
    {IDCANCEL, IDS_CANCEL, 0},
    {0, 0, 0}
};


static int move_buttons_group[] = {
    IDOK,
    IDCANCEL,
    0
};

static void init_resources_dialog(HWND hwnd, unsigned int type)
{
    unsigned int n = 0;
    int xpos1, xpos2;
    int size;
    RECT rect;
    int idc;

    if (type == UIROM_TYPE_MAIN) {
        /* translate all dialog items */
        uilib_localize_dialog(hwnd, main_res_trans);

        idc = IDC_COMPUTER_RESOURCES;
    } else if (type == UIROM_TYPE_DRIVE) {
        /* translate all dialog items */
        uilib_localize_dialog(hwnd, drive_res_trans);

        idc = IDC_DRIVE_RESOURCES;
    }

    /* get the max x of the group element */
    uilib_get_element_max_x(hwnd, idc, &xpos1);

    /* get the size of the group element */
    uilib_get_element_size(hwnd, idc, &size);

    /* adjust the size of the group element */
    uilib_adjust_element_width(hwnd, idc);

    /* get the max x of the group element */
    uilib_get_element_max_x(hwnd, idc, &xpos2);

    if (xpos2 < xpos1) {
        /* restore the size of the group element */
        uilib_set_element_width(hwnd, idc, size);
    } else {
        /* resize the dialog window */
        GetWindowRect(hwnd, &rect);
        MoveWindow(hwnd, rect.left, rect.top, xpos2 + 10, rect.bottom - rect.top, TRUE);

        /* recenter the buttons in the newly resized dialog window */
        uilib_center_buttons(hwnd, move_buttons_group, 0);
    }

    while (settings[n].realname != NULL) {
        if (settings[n].type == type) {
            int enable;

            resources_get_int_sprintf("Romset%s", &enable,
                                      settings[n].resname);

            CheckDlgButton(hwnd, settings[n].idc_resource,
                           enable ? BST_CHECKED : BST_UNCHECKED);
        }
        n++;
    }
}

static void end_resources_dialog(HWND hwnd, unsigned int type)
{
    unsigned int n = 0;

    while (settings[n].realname != NULL) {
        if (settings[n].type == type) {
            int enable;

            enable = (IsDlgButtonChecked(hwnd, settings[n].idc_resource)
                     == BST_CHECKED) ? 1 : 0;

            resources_set_int_sprintf("Romset%s", enable, settings[n].resname);
        }
        n++;
    }
}

static BOOL CALLBACK resources_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                           LPARAM lparam, unsigned int type)
{
    int command;

    switch (msg) {
      case WM_INITDIALOG:
        init_resources_dialog(hwnd, type);
        return TRUE;
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDOK:
            end_resources_dialog(hwnd, type);
          case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
    }
    return FALSE;
}

static INT_PTR CALLBACK resources_computer_dialog_proc(HWND hwnd, UINT msg,
                                                       WPARAM wparam,
                                                       LPARAM lparam)
{
    return resources_dialog_proc(hwnd, msg, wparam, lparam, UIROM_TYPE_MAIN);
}

static INT_PTR CALLBACK resources_drive_dialog_proc(HWND hwnd, UINT msg,
                                                    WPARAM wparam,
                                                    LPARAM lparam)
{
    return resources_dialog_proc(hwnd, msg, wparam, lparam, UIROM_TYPE_DRIVE);
}

static INT_PTR CALLBACK resources_other_dialog_proc(HWND hwnd, UINT msg,
                                                    WPARAM wparam,
                                                    LPARAM lparam)
{
    return resources_dialog_proc(hwnd, msg, wparam, lparam, UIROM_TYPE_OTHER);
}

static void uirom_resources_computer(HWND hwnd)
{
    DialogBox(winmain_instance,
              (LPCTSTR)(UINT_PTR)romset_dialog_resources[UIROM_TYPE_MAIN], hwnd,
              resources_computer_dialog_proc);
    update_romset_list(hwnd);
}

static void uirom_resources_drive(HWND hwnd)
{
    DialogBox(winmain_instance,
              (LPCTSTR)(UINT_PTR)romset_dialog_resources[UIROM_TYPE_DRIVE], hwnd,
              resources_drive_dialog_proc);
    update_romset_list(hwnd);
}

static void uirom_resources_other(HWND hwnd)
{
    DialogBox(winmain_instance,
              (LPCTSTR)(UINT_PTR)romset_dialog_resources[UIROM_TYPE_OTHER], hwnd,
              resources_other_dialog_proc);
    update_romset_list(hwnd);
}

static INT_PTR CALLBACK dialog_proc_romset(HWND hwnd, UINT msg, WPARAM wparam,
                                           LPARAM lparam)
{
    switch (msg) {
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_romset_dialog(hwnd);
        return TRUE;
      case WM_NOTIFY:
        switch (((NMHDR FAR *)lparam)->code) {
          case PSN_KILLACTIVE:
            end_romset_dialog(hwnd);
            return TRUE;
        }
        return FALSE;
      case WM_COMMAND:
        switch (LOWORD(wparam)) {
          case IDC_ROMSET_SELECT_ARCHIVE:
          case IDC_ROMSET_SELECT_FILE:
            update_romset_dialog(hwnd, LOWORD(wparam));
            break;
          case IDC_ROMSET_ARCHIVE_BROWSE:
            uilib_select_browse(hwnd, translate_text(IDS_SELECT_ROMSET_ARCHIVE),
                                UILIB_FILTER_ROMSET_ARCHIVE,
                                UILIB_SELECTOR_TYPE_FILE_SAVE,
                                IDC_ROMSET_ARCHIVE_NAME);
            break;
          case IDC_ROMSET_ARCHIVE_LOAD:
            load_archive_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_ARCHIVE_SAVE:
            save_archive_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_ARCHIVE_APPLY:
            apply_archive_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_ARCHIVE_NEW:
            new_archive_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_ARCHIVE_DELETE:
            delete_archive_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_FILE_BROWSE:
            uilib_select_browse(hwnd, translate_text(IDS_SELECT_ROMSET_FILE),
                                UILIB_FILTER_ROMSET_FILE,
                                UILIB_SELECTOR_TYPE_FILE_SAVE,
                                IDC_ROMSET_FILE_NAME);
            break;
          case IDC_ROMSET_FILE_LOAD:
            load_file_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_FILE_SAVE:
            save_file_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_RESOURCE_COMPUTER:
            uirom_resources_computer(hwnd);
            break;
          case IDC_ROMSET_RESOURCE_DRIVE:
            uirom_resources_drive(hwnd);
            break;
          case IDC_ROMSET_RESOURCE_OTHER:
            uirom_resources_other(hwnd);
            break;
        }
        return TRUE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
    }
    return FALSE;
}

void uirom_settings_dialog(HWND hwnd, unsigned int idd_dialog_main,
                           unsigned int idd_dialog_drive,
                           const unsigned int *idd_dialog_resources,
                           const uirom_settings_t *uirom_settings,
                           uilib_localize_dialog_param *uirom_main_trans,
                           uilib_localize_dialog_param *uirom_drive_trans,
                           uilib_dialog_group *uirom_main_left_group,
                           uilib_dialog_group *uirom_main_middle_group,
                           uilib_dialog_group *uirom_main_right_group,
                           uilib_dialog_group *uirom_drive_left_group,
                           uilib_dialog_group *uirom_drive_middle_group,
                           uilib_dialog_group *uirom_drive_right_group,
                           uilib_localize_dialog_param *uirom_main_res_trans)
{
    PROPSHEETPAGE psp[3];
    PROPSHEETHEADER psh;

    main_trans = uirom_main_trans;
    drive_trans = uirom_drive_trans;
    main_left_group = uirom_main_left_group;
    main_middle_group = uirom_main_middle_group;
    main_right_group = uirom_main_right_group;
    drive_left_group = uirom_drive_left_group;
    drive_middle_group = uirom_drive_middle_group;
    drive_right_group = uirom_drive_right_group;
    main_res_trans = uirom_main_res_trans;

    settings = uirom_settings;
    romset_dialog_resources = idd_dialog_resources;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
    psp[0].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_ROMSET_SETTINGS_DIALOG);
    psp[0].pszIcon = NULL;
#else
    psp[0].DUMMYUNIONNAME.pszTemplate
        = MAKEINTRESOURCE(IDD_ROMSET_SETTINGS_DIALOG);
    psp[0].u2.pszIcon = NULL;
#endif
    psp[0].lParam = 0;
    psp[0].pfnCallback = NULL;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
    psp[1].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
    psp[1].pszTemplate = MAKEINTRESOURCE(idd_dialog_main);
    psp[1].pszIcon = NULL;
#else
    psp[1].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(idd_dialog_main);
    psp[1].u2.pszIcon = NULL;
#endif
    psp[1].lParam = 0;
    psp[1].pfnCallback = NULL;

    psp[2].dwSize = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
    psp[2].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
    psp[2].pszTemplate = MAKEINTRESOURCE(idd_dialog_drive);
    psp[2].pszIcon = NULL;
#else
    psp[2].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(idd_dialog_drive);
    psp[2].u2.pszIcon = NULL;
#endif
    psp[2].lParam = 0;
    psp[2].pfnCallback = NULL;

    psp[0].pfnDlgProc = dialog_proc_romset;
    psp[0].pszTitle = translate_text(IDS_ROMSET);
    psp[1].pfnDlgProc = dialog_proc_main;
    psp[1].pszTitle = translate_text(IDS_COMPUTER);
    psp[2].pfnDlgProc = dialog_proc_drive;
    psp[2].pszTitle = translate_text(IDS_DRIVE);

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = translate_text(IDS_ROM_SETTINGS);
    psh.nPages = 3;
#ifdef _ANONYMOUS_UNION
    psh.pszIcon = NULL;
    psh.nStartPage = 0;
    psh.ppsp = psp;
#else
    psh.DUMMYUNIONNAME.pszIcon = NULL;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
#endif
    psh.pfnCallback = NULL;

    PropertySheet(&psh);
}

