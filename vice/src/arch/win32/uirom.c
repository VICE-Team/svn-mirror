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

#include <string.h>
#include <windows.h>
#include <tchar.h>

#include "lib.h"
#include "machine.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "ui.h"
#include "uilib.h"
#include "uirom.h"
#include "winmain.h"


static const uirom_settings_t *settings;


static void init_rom_dialog(HWND hwnd, unsigned int type)
{
    unsigned int n = 0;

    while (settings[n].realname != NULL) {
        if (settings[n].type == type) {
            const char *filename;
            TCHAR *st_filename;

            resources_get_value(settings[n].resname, (void *)&filename);
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
            resources_set_value(settings[n].resname,
                                (resource_value_t)filename);
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

            _stprintf(st_realname, TEXT("Load %s ROM image"),
                      settings[n].realname);

            uilib_select_browse(hwnd, st_realname,
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

static BOOL CALLBACK dialog_proc_main(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    return dialog_proc(hwnd, msg, wparam, lparam, UIROM_TYPE_MAIN);
}

static BOOL CALLBACK dialog_proc_drive(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    return dialog_proc(hwnd, msg, wparam, lparam, UIROM_TYPE_DRIVE);
}

static void enable_controls_for_romset(HWND hwnd, int idc_active)
{
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_NAME),
                 idc_active == IDC_ROMSET_SELECT_ARCHIVE);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_BROWSE),
                 idc_active == IDC_ROMSET_SELECT_ARCHIVE);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_ACTIVE),
                 idc_active == IDC_ROMSET_SELECT_ARCHIVE);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_SAVEACTIVE),
                 idc_active == IDC_ROMSET_SELECT_ARCHIVE);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_SAVENEW),
                 idc_active == IDC_ROMSET_SELECT_ARCHIVE);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_DELETE),
                 idc_active == IDC_ROMSET_SELECT_ARCHIVE);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_FILE_NAME),
                 idc_active == IDC_ROMSET_SELECT_FILE);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_FILE_BROWSE),
                 idc_active == IDC_ROMSET_SELECT_FILE);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_FILE_SAVE),
                 idc_active == IDC_ROMSET_SELECT_FILE);
}

static void update_romset_dialog(HWND hwnd, int idc_active)
{
    char *list;
    TCHAR *st_list;

    if (idc_active == IDC_ROMSET_SELECT_ARCHIVE
        || idc_active == IDC_ROMSET_SELECT_FILE) {
        CheckRadioButton(hwnd, IDC_ROMSET_SELECT_ARCHIVE,
                         IDC_ROMSET_SELECT_FILE, idc_active);
        enable_controls_for_romset(hwnd, idc_active);
    }

    list = machine_romset_file_list("\r\n");

    st_list = system_mbstowcs_alloc(list);
    SetDlgItemText(hwnd, IDC_ROMSET_PREVIEW, st_list);
    system_mbstowcs_free(st_list);

    lib_free(list);
}

static void init_romset_dialog(HWND hwnd)
{
    int res_value, idc_active;
    char *name;
    TCHAR *st_name;

    resources_get_value("RomsetSourceFile", (void *)&res_value);
    idc_active = IDC_ROMSET_SELECT_ARCHIVE + res_value;
    update_romset_dialog(hwnd, idc_active);

    resources_get_value("RomsetArchiveName", (void *)&name);
    st_name = system_mbstowcs_alloc(name);
    SetDlgItemText(hwnd, IDC_ROMSET_ARCHIVE_NAME,
                   name != NULL ? st_name : TEXT(""));
    system_mbstowcs_free(st_name);

    resources_get_value("RomsetFileName", (void *)&name);
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
        resources_set_value("RomsetSourceFile", (resource_value_t)0);
    if (IsDlgButtonChecked(hwnd, IDC_ROMSET_SELECT_FILE) == BST_CHECKED)
        resources_set_value("RomsetSourceFile", (resource_value_t)1);

    GetDlgItemText(hwnd, IDC_ROMSET_ARCHIVE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_value("RomsetArchiveName", (resource_value_t)s);

    GetDlgItemText(hwnd, IDC_ROMSET_FILE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_value("RomsetfileName", (resource_value_t)s);
}

static void saveactive_archive_romset_dialog(HWND hwnd)
{

}

static void savenew_archive_romset_dialog(HWND hwnd)
{

}

static void delete_archive_romset_dialog(HWND hwnd)
{

}

static void save_file_romset_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    GetDlgItemText(hwnd, IDC_ROMSET_FILE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    machine_romset_save(s);
}

static BOOL CALLBACK dialog_proc_romset(HWND hwnd, UINT msg, WPARAM wparam,
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
            uilib_select_browse(hwnd, TEXT("Select romset archive"),
                                UILIB_SELECTOR_TYPE_FILE_SAVE,
                                IDC_ROMSET_ARCHIVE_NAME);
            break;
          case IDC_ROMSET_ARCHIVE_SAVEACTIVE:
            saveactive_archive_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_ARCHIVE_SAVENEW:
            savenew_archive_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_ARCHIVE_DELETE:
            delete_archive_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_FILE_BROWSE:
            uilib_select_browse(hwnd, TEXT("Select romset file"),
                                UILIB_SELECTOR_TYPE_FILE_SAVE,
                                IDC_ROMSET_FILE_NAME);
            break;
          case IDC_ROMSET_FILE_SAVE:
            save_file_romset_dialog(hwnd);
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
                           const uirom_settings_t *uirom_settings)
{
    PROPSHEETPAGE psp[3];
    PROPSHEETHEADER psh;

    settings = uirom_settings;

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
    psp[0].pszTitle = TEXT("Romset");
    psp[1].pfnDlgProc = dialog_proc_main;
    psp[1].pszTitle = TEXT("Computer");
    psp[2].pfnDlgProc = dialog_proc_drive;
    psp[2].pszTitle = TEXT("Drive");

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = TEXT("ROM settings");
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

