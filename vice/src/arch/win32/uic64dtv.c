/*
 * uic64dtv.c - Implementation of the C64DTV specific settings dialog box.
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
#include <string.h>
#include <windows.h>
#include <tchar.h>

#include "c64dtvflash.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "uilib.h"
#include "uic64dtv.h"
#include "winmain.h"

static void enable_c64dtv_controls(HWND hwnd)
{
  EnableWindow(GetDlgItem(hwnd, IDC_DTV_REVISION), 1);
  EnableWindow(GetDlgItem(hwnd, IDC_HUMMER_USERPORT_DEVICE), 1);
  EnableWindow(GetDlgItem(hwnd, IDC_C64DTV_HUMMER_JOY_PORT), 1);
}

static void enable_c64dtv_attach_flash_controls(HWND hwnd)
{
  EnableWindow(GetDlgItem(hwnd, IDC_C64DTV_ROM_WRITE_ENABLE), 1);
  EnableWindow(GetDlgItem(hwnd, IDC_C64DTV_ROM_IMAGE_BROWSE), 1);
  EnableWindow(GetDlgItem(hwnd, IDC_C64DTV_ROM_IMAGE_FILE), 1);
}

static void enable_c64dtv_create_flash_controls(HWND hwnd)
{
  EnableWindow(GetDlgItem(hwnd, IDC_C64DTV_ROM_COPY_C64), 1);
  EnableWindow(GetDlgItem(hwnd, IDC_C64DTV_ROM_IMAGE_BROWSE), 1);
  EnableWindow(GetDlgItem(hwnd, IDC_C64DTV_ROM_IMAGE_FILE), 1);
}

static void init_c64dtv_dialog(HWND hwnd)
{
  HWND temp_hwnd;
  int res_value;

  temp_hwnd = GetDlgItem(hwnd, IDC_DTV_REVISION);
  SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"DTV2");
  SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"DTV3");
  resources_get_int("DtvRevision", &res_value);
  res_value-=2;
  SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

  temp_hwnd = GetDlgItem(hwnd, IDC_HUMMER_USERPORT_DEVICE);
  SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)translate_text(IDS_NONE));
  SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"ADC");
  SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)translate_text(IDS_JOYSTICK));

  resources_get_int("HummerUserportDevice", &res_value);
  SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

  temp_hwnd = GetDlgItem(hwnd, IDC_C64DTV_HUMMER_JOY_PORT);
  SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"Joy1");
  SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"Joy2");
  resources_get_int("HummerUserportJoyPort", &res_value);
  res_value--;
  SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

  enable_c64dtv_controls(hwnd);
}

static void init_c64dtv_attach_flash_dialog(HWND hwnd)
{
  int res_value;
  const char *c64dtv_bios_file;
  TCHAR *st_c64dtv_bios_file;

  resources_get_string("c64dtvromfilename", &c64dtv_bios_file);
  st_c64dtv_bios_file = system_mbstowcs_alloc(c64dtv_bios_file);
  SetDlgItemText(hwnd, IDC_C64DTV_ROM_IMAGE_FILE,
                 c64dtv_bios_file != NULL ? st_c64dtv_bios_file : TEXT(""));
  system_mbstowcs_free(st_c64dtv_bios_file);

  resources_get_int("c64dtvromrw", &res_value);
  CheckDlgButton(hwnd, IDC_C64DTV_ROM_WRITE_ENABLE, res_value ? BST_CHECKED : BST_UNCHECKED);

  enable_c64dtv_attach_flash_controls(hwnd);
}

static void init_c64dtv_create_flash_dialog(HWND hwnd)
{
  SetDlgItemText(hwnd, IDC_C64DTV_ROM_IMAGE_FILE, TEXT(""));

  CheckDlgButton(hwnd, IDC_C64DTV_ROM_COPY_C64, BST_CHECKED);

  enable_c64dtv_create_flash_controls(hwnd);
}

static void end_c64dtv_dialog(HWND hwnd)
{
  resources_set_int("DtvRevision",(int)SendMessage(GetDlgItem(
                    hwnd, IDC_DTV_REVISION), CB_GETCURSEL, 0, 0)+2);

  resources_set_int("HummerUserportDevice",(int)SendMessage(GetDlgItem(
                    hwnd, IDC_C64DTV_HUMMER_JOY_PORT), CB_GETCURSEL, 0, 0));

  resources_set_int("HummerUserportJoyPort",(int)SendMessage(GetDlgItem(
                    hwnd, IDC_C64DTV_HUMMER_JOY_PORT), CB_GETCURSEL, 0, 0)+1);
}

static void end_c64dtv_attach_flash_dialog(HWND hwnd)
{
  TCHAR st[MAX_PATH];
  char s[MAX_PATH];

  GetDlgItemText(hwnd, IDC_C64DTV_ROM_IMAGE_FILE, st, MAX_PATH);
  system_wcstombs(s, st, MAX_PATH);
  resources_set_string("c64dtvromfilename", s);

  resources_set_int("c64dtvromrw", (IsDlgButtonChecked(hwnd,
                    IDC_C64DTV_ROM_WRITE_ENABLE) == BST_CHECKED ? 1 : 0 ));
}

static void end_c64dtv_create_flash_dialog(HWND hwnd)
{
  TCHAR st[MAX_PATH];
  char s[MAX_PATH];

  GetDlgItemText(hwnd, IDC_C64DTV_ROM_IMAGE_FILE, st, MAX_PATH);
  system_wcstombs(s, st, MAX_PATH);
  c64dtvflash_create_blank_image(s, (IsDlgButtonChecked(hwnd,
                                     IDC_C64DTV_ROM_COPY_C64) == BST_CHECKED ? 1 : 0 ));
}

static void browse_c64dtv_bios_file(HWND hwnd)
{
    uilib_select_browse(hwnd, TEXT("Select file for C64DTV ROM"),
                        UILIB_FILTER_ALL, UILIB_SELECTOR_TYPE_FILE_SAVE,
                        IDC_C64DTV_ROM_IMAGE_FILE);
}

static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                    LPARAM lparam)
{
  TCHAR st[MAX_PATH];
  char s[MAX_PATH];

  int command;

  switch (msg)
  {
    case WM_COMMAND:
      command = LOWORD(wparam);
      switch (command)
      {
        case IDC_C64DTV_ROM_CREATE:
          GetDlgItemText(hwnd, IDC_C64DTV_ROM_IMAGE_FILE, st, MAX_PATH);
          system_wcstombs(s, st, MAX_PATH);
          c64dtvflash_create_blank_image(s, (IsDlgButtonChecked(hwnd,
                                             IDC_C64DTV_ROM_COPY_C64) == BST_CHECKED ? 1 : 0 ));
          break;
        case IDOK:
          end_c64dtv_dialog(hwnd);
        case IDCANCEL:
          EndDialog(hwnd, 0);
          return TRUE;
      }
      return FALSE;
    case WM_CLOSE:
      EndDialog(hwnd, 0);
      return TRUE;
    case WM_INITDIALOG:
      init_c64dtv_dialog(hwnd);
      return TRUE;
  }
  return FALSE;
}

static INT_PTR CALLBACK dialog_attach_flash_proc(HWND hwnd, UINT msg,
                                                 WPARAM wparam, LPARAM lparam)
{
  int command;

  switch (msg)
  {
    case WM_COMMAND:
      command = LOWORD(wparam);
      switch (command)
      {
        case IDC_C64DTV_ROM_IMAGE_BROWSE:
          browse_c64dtv_bios_file(hwnd);
          break;
        case IDOK:
          end_c64dtv_attach_flash_dialog(hwnd);
        case IDCANCEL:
          EndDialog(hwnd, 0);
          return TRUE;
      }
      return FALSE;
    case WM_CLOSE:
      EndDialog(hwnd, 0);
      return TRUE;
    case WM_INITDIALOG:
      init_c64dtv_attach_flash_dialog(hwnd);
      return TRUE;
  }
  return FALSE;
}

static INT_PTR CALLBACK dialog_create_flash_proc(HWND hwnd, UINT msg,
                                                 WPARAM wparam, LPARAM lparam)
{
  int command;

  switch (msg)
  {
    case WM_COMMAND:
      command = LOWORD(wparam);
      switch (command)
      {
        case IDC_C64DTV_ROM_IMAGE_BROWSE:
          browse_c64dtv_bios_file(hwnd);
          break;
        case IDOK:
          end_c64dtv_create_flash_dialog(hwnd);
        case IDCANCEL:
          EndDialog(hwnd, 0);
          return TRUE;
      }
      return FALSE;
    case WM_CLOSE:
      EndDialog(hwnd, 0);
      return TRUE;
    case WM_INITDIALOG:
      init_c64dtv_create_flash_dialog(hwnd);
      return TRUE;
  }
  return FALSE;
}

void ui_c64dtv_settings_dialog(HWND hwnd)
{
  DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)translate_res(IDD_C64DTV_SETTINGS_DIALOG), hwnd,
            dialog_proc);
}

void ui_c64dtv_attach_flash_dialog(HWND hwnd)
{
  DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)translate_res(IDD_C64DTV_ATTACH_FLASH_IMAGE_DIALOG), hwnd,
            dialog_attach_flash_proc);
}

void ui_c64dtv_create_flash_dialog(HWND hwnd)
{
  DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)translate_res(IDD_C64DTV_CREATE_FLASH_IMAGE_DIALOG), hwnd,
            dialog_create_flash_proc);
}
