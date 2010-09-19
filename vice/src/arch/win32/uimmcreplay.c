/*
 * uimmcreplay.c - Implementation of the MMC Replay settings dialog box.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "types.h"
#include "uilib.h"
#include "uimmcreplay.h"
#include "winmain.h"

static char *ui_mmcreplay_sdtype[] = {
    "Auto",
    "MMC",
    "SD",
    "SDHC",
    NULL
};

static uilib_localize_dialog_param mmcreplay_dialog_trans[] = {
    { 0, IDS_MMCREPLAY_CAPTION, -1 },
    { IDC_MMCREPLAY_CARDIMAGE_LABEL, IDS_MMCREPLAY_CARDIMAGE_LABEL, 0 },
    { IDC_MMCREPLAY_CARDIMAGE_BROWSE, IDS_BROWSE, 0 },
    { IDC_MMCREPLAY_CARDRW, IDS_MMCREPLAY_READ_WRITE, 0 },
    { IDC_MMCREPLAY_EEPROMIMAGE_LABEL, IDS_MMCREPLAY_EEPROMIMAGE_LABEL, 0 },
    { IDC_MMCREPLAY_EEPROMIMAGE_BROWSE, IDS_BROWSE, 0 },
    { IDC_MMCREPLAY_EEPROMRW, IDS_MMCREPLAY_READ_WRITE, 0 },
    { IDC_MMCREPLAY_RESCUEMODE, IDS_MMCREPLAY_RESCUEMODE, 0 },
    { IDC_MMCREPLAY_SDTYPE_LABEL, IDS_MMCREPLAY_SDTYPE_LABEL, 0 },
    { IDOK, IDS_OK, 0 },
    { IDCANCEL, IDS_CANCEL, 0 },
    { 0, 0, 0 }
};

#if 0
static void enable_mmcreplay_controls(HWND hwnd)
{
    int is_enabled;

    is_enabled = (IsDlgButtonChecked(hwnd, IDC_MMC64_ENABLE) == BST_CHECKED) ? 1 : 0;

    EnableWindow(GetDlgItem(hwnd, IDC_MMC64_REVISION), is_enabled);

    EnableWindow(GetDlgItem(hwnd, IDC_MMC64_FLASHJUMPER), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_MMC64_BIOS_SAVE), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_MMC64_BIOS_BROWSE), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_MMC64_BIOS_FILE), is_enabled);

    EnableWindow(GetDlgItem(hwnd, IDC_MMC64_IMAGE_RO), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_MMC64_IMAGE_BROWSE), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_MMC64_IMAGE_FILE), is_enabled);
}

static uilib_localize_dialog_param mmc64_dialog_trans[] = {
    { IDC_MMC64_ENABLE, IDS_MMC64_ENABLE, 0 },
    { IDC_MMC64_REVISION_LABEL, IDS_MMC64_REVISION_LABEL, 0 },
    { IDC_MMC64_FLASHJUMPER, IDS_MMC64_FLASHJUMPER, 0 },
    { IDC_MMC64_BIOS_SAVE, IDS_MMC64_BIOS_SAVE, 0 },
    { IDC_MMC64_BIOS_FILE_LABEL, IDS_MMC64_BIOS_FILE_LABEL, 0 },
    { IDC_MMC64_BIOS_BROWSE, IDS_BROWSE, 0 },
    { IDC_MMC64_IMAGE_RO, IDS_MMC64_IMAGE_RO, 0 },
    { IDC_MMC64_IMAGE_FILE_LABEL, IDS_MMC64_IMAGE_FILE_LABEL, 0 },
    { IDC_MMC64_IMAGE_BROWSE, IDS_BROWSE, 0 },
    { IDOK, IDS_OK, 0 },
    { IDCANCEL, IDS_CANCEL, 0 },
    { 0, 0, 0 }
};

static uilib_dialog_group mmc64_main_group[] = {
    { IDC_MMC64_ENABLE, 1 },
    { IDC_MMC64_REVISION_LABEL, 0 },
    { IDC_MMC64_FLASHJUMPER, 1 },
    { IDC_MMC64_BIOS_SAVE, 1 },
    { IDC_MMC64_BIOS_FILE_LABEL, 0 },
    { IDC_MMC64_IMAGE_RO, 1 },
    { IDC_MMC64_IMAGE_FILE_LABEL, 0 },
    { 0, 0 }
};

static uilib_dialog_group mmc64_left_group[] = {
    { IDC_MMC64_REVISION_LABEL, 0 },
    { IDC_MMC64_BIOS_FILE_LABEL, 0 },
    { IDC_MMC64_IMAGE_FILE_LABEL, 0 },
    { 0, 0 }
};

static uilib_dialog_group mmc64_right_group[] = {
    { IDC_MMC64_REVISION, 0 },
    { IDC_MMC64_BIOS_BROWSE, 0 },
    { IDC_MMC64_IMAGE_BROWSE, 0 },
    { 0, 0 }
};

static uilib_dialog_group mmc64_window_group[] = {
    { IDC_MMC64_ENABLE, 0 },
    { IDC_MMC64_REVISION, 0 },
    { IDC_MMC64_FLASHJUMPER, 0 },
    { IDC_MMC64_BIOS_SAVE, 0 },
    { IDC_MMC64_BIOS_BROWSE, 0 },
    { IDC_MMC64_BIOS_FILE, 0 },
    { IDC_MMC64_IMAGE_RO, 0 },
    { IDC_MMC64_IMAGE_BROWSE, 0 },
    { IDC_MMC64_IMAGE_FILE, 0 },
    { 0, 0 }
};

static int move_buttons_group[] = {
    IDOK,
    IDCANCEL,
    0
};
#endif

static void init_mmcreplay_dialog(HWND hwnd)
{
    HWND temp_hwnd;
    int res_value;
    int res_value_loop;
    const char *mmcreplay_cardimage_file;
    TCHAR *st_mmcreplay_cardimage_file;
    const char *mmcreplay_eeprom_file;
    TCHAR *st_mmcreplay_eeprom_file;

    /* translate all dialog items */
    uilib_localize_dialog(hwnd, mmcreplay_dialog_trans);

    resources_get_string("MMCRCardImage", &mmcreplay_cardimage_file);
    st_mmcreplay_cardimage_file = system_mbstowcs_alloc(mmcreplay_cardimage_file);
    SetDlgItemText(hwnd, IDC_MMCREPLAY_CARDIMAGE, mmcreplay_cardimage_file != NULL ? st_mmcreplay_cardimage_file : TEXT(""));
    system_mbstowcs_free(st_mmcreplay_cardimage_file);

    resources_get_int("MMCRCardRW", &res_value);
    CheckDlgButton(hwnd, IDC_MMCREPLAY_CARDRW, res_value ? BST_CHECKED : BST_UNCHECKED);

    resources_get_string("MMCREEPROMImage", &mmcreplay_eeprom_file);
    st_mmcreplay_eeprom_file = system_mbstowcs_alloc(mmcreplay_eeprom_file);
    SetDlgItemText(hwnd, IDC_MMCREPLAY_EEPROMIMAGE, mmcreplay_eeprom_file != NULL ? st_mmcreplay_eeprom_file : TEXT(""));
    system_mbstowcs_free(st_mmcreplay_eeprom_file);

    resources_get_int("MMCREEPROMRW", &res_value);
    CheckDlgButton(hwnd, IDC_MMCREPLAY_EEPROMRW, res_value ? BST_CHECKED : BST_UNCHECKED);

    resources_get_int("MMCRRescueMode", &res_value);
    CheckDlgButton(hwnd, IDC_MMCREPLAY_RESCUEMODE, res_value ? BST_CHECKED : BST_UNCHECKED);

    resources_get_int("MMCRSDType", &res_value);
    temp_hwnd = GetDlgItem(hwnd, IDC_MMCREPLAY_SDTYPE);
    for (res_value_loop = 0; ui_mmcreplay_sdtype[res_value_loop]; res_value_loop++) {
        SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)ui_mmcreplay_sdtype[res_value_loop]);
    }
    SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);
}

static void end_mmcreplay_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    GetDlgItemText(hwnd, IDC_MMCREPLAY_CARDIMAGE, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_string("MMCRCardImage", s);

    GetDlgItemText(hwnd, IDC_MMCREPLAY_EEPROMIMAGE, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_string("MMCREEPROMImage", s);

    resources_set_int("MMCRCardRW", (IsDlgButtonChecked(hwnd, IDC_MMCREPLAY_CARDRW) == BST_CHECKED ? 1 : 0 ));
    resources_set_int("MMCREEPROMRW", (IsDlgButtonChecked(hwnd, IDC_MMCREPLAY_EEPROMRW) == BST_CHECKED ? 1 : 0 ));
    resources_set_int("MMCRRescueMode", (IsDlgButtonChecked(hwnd, IDC_MMCREPLAY_RESCUEMODE) == BST_CHECKED ? 1 : 0 ));
    resources_set_int("MMCRSDType", (int)SendMessage(GetDlgItem(hwnd, IDC_MMCREPLAY_SDTYPE), CB_GETCURSEL, 0, 0));
}

static void browse_mmcreplay_cardimage_file(HWND hwnd)
{
    uilib_select_browse(hwnd, TEXT("Select file for MMC Replay Card Image"), UILIB_FILTER_ALL, UILIB_SELECTOR_TYPE_FILE_SAVE, IDC_MMCREPLAY_CARDIMAGE);
}

static void browse_mmcreplay_eeprom_file(HWND hwnd)
{
    uilib_select_browse(hwnd, TEXT("Select file for MMC Replay EEPROM image"), UILIB_FILTER_ALL, UILIB_SELECTOR_TYPE_FILE_SAVE, IDC_MMCREPLAY_EEPROMIMAGE);
}


static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    int command;

    switch (msg) {
        case WM_COMMAND:
            command = LOWORD(wparam);
            switch (command) {
                case IDC_MMCREPLAY_CARDIMAGE_BROWSE:
                    browse_mmcreplay_cardimage_file(hwnd);
                    break;
                case IDC_MMCREPLAY_EEPROMIMAGE_BROWSE:
                    browse_mmcreplay_eeprom_file(hwnd);
                    break;
                case IDOK:
                    end_mmcreplay_dialog(hwnd);
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    return TRUE;
            }
            return FALSE;
        case WM_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;
        case WM_INITDIALOG:
            init_mmcreplay_dialog(hwnd);
            return TRUE;
    }
    return FALSE;
}

void ui_mmcreplay_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)IDD_MMCREPLAY_SETTINGS_DIALOG, hwnd, dialog_proc);
}
