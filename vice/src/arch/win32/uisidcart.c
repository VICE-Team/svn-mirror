/*
 * uisidcart.c - Implementation of the SID cartridge settings dialog box.
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

#include "hardsid.h"
#include "res.h"
#include "resources.h"
#include "sidcart.h"
#include "system.h"
#include "translate.h"
#include "uilib.h"
#include "uisidcart.h"
#include "winmain.h"

static const TCHAR *ui_sid_engine[] = 
{
    TEXT("Fast SID"),
#ifdef HAVE_CATWEASELMKIII
    TEXT("Catweasel MK3"),
#endif
#ifdef HAVE_HARDSID
    TEXT("HardSID"),
#endif
#ifdef HAVE_PARSID
    TEXT("ParSID on Port 1"),
    TEXT("ParSID on Port 2"),
    TEXT("ParSID on Port 3"),
#endif
    NULL
};

static void enable_sidcart_hardsid_controls(HWND hwnd)
{
  int engine;
  int is_enabled;

  resources_get_int("SidEngine", &engine);
  is_enabled = (engine == SID_ENGINE_HARDSID) && (hardsid_available() > 0);

  EnableWindow(GetDlgItem(hwnd, IDC_SIDCART_HARDSID_MAIN_DEVICE), is_enabled);
}

static void enable_sidcart_controls(HWND hwnd)
{
  int is_enabled;

  is_enabled = (IsDlgButtonChecked(hwnd, IDC_SIDCART_ENABLE) == BST_CHECKED) ? 1 : 0;

  EnableWindow(GetDlgItem(hwnd, IDC_SIDCART_ENGINE), is_enabled);
  EnableWindow(GetDlgItem(hwnd, IDC_SIDCART_MODEL), is_enabled);
  EnableWindow(GetDlgItem(hwnd, IDC_SIDCART_FILTERS), is_enabled);
  EnableWindow(GetDlgItem(hwnd, IDC_SIDCART_ADDRESS), is_enabled);
  EnableWindow(GetDlgItem(hwnd, IDC_SIDCART_CLOCK), is_enabled);

  enable_sidcart_hardsid_controls(hwnd);
}

static void init_sidcart_dialog(HWND hwnd)
{
  HWND temp_hwnd;
  int res_value;
  int res_value_loop;
  unsigned int available, device;

  resources_get_int("SidCart", &res_value);
  CheckDlgButton(hwnd, IDC_SIDCART_ENABLE, res_value ? BST_CHECKED : BST_UNCHECKED);
    
  resources_get_int("SidEngine", &res_value);
  temp_hwnd = GetDlgItem(hwnd, IDC_SIDCART_ENGINE);
  for (res_value_loop = 0; ui_sid_engine[res_value_loop]; res_value_loop++)
  {
    SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)ui_sid_engine[res_value_loop]);
  }
  SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

  temp_hwnd = GetDlgItem(hwnd, IDC_SIDCART_MODEL);
  SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("6581"));
  SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("8580"));
  resources_get_int("SidModel", &res_value);
  SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

  resources_get_int("SidFilters", &res_value);
  CheckDlgButton(hwnd, IDC_SIDCART_FILTERS, res_value ? BST_CHECKED : BST_UNCHECKED);

  temp_hwnd = GetDlgItem(hwnd, IDC_SIDCART_ADDRESS);
  SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)native_primary_sid_address);
  SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)native_secondary_sid_address);
  resources_get_int("SidAddress", &res_value);
  SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

  temp_hwnd = GetDlgItem(hwnd, IDC_SIDCART_CLOCK);
  SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("C64"));
  SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)native_sid_clock);
  resources_get_int("SidClock", &res_value);
  SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

  available = hardsid_available();
  device = 0;

  resources_get_int("SidHardSIDMain", &res_value);
  temp_hwnd = GetDlgItem(hwnd, IDC_SIDCART_HARDSID_MAIN_DEVICE);

  while (available > 0)
  {
    TCHAR item[10];

    _stprintf(item, TEXT("%d"), device++);
    SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)item);
    available--;
  }

  SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

  enable_sidcart_controls(hwnd);
}

static void end_sidcart_dialog(HWND hwnd)
{
  resources_set_int("SidCart", (IsDlgButtonChecked(hwnd,
                    IDC_SIDCART_ENABLE) == BST_CHECKED ? 1 : 0 ));
 
  resources_set_int("SidEngine",SendMessage(GetDlgItem(
                    hwnd, IDC_SIDCART_ENGINE), CB_GETCURSEL, 0, 0));

  resources_set_int("SidModel",SendMessage(GetDlgItem(
                    hwnd, IDC_SIDCART_MODEL), CB_GETCURSEL, 0, 0));

  resources_set_int("SidFilters", (IsDlgButtonChecked(hwnd,
                    IDC_SIDCART_FILTERS) == BST_CHECKED ? 1 : 0 ));

  resources_set_int("SidAddress",SendMessage(GetDlgItem(
                    hwnd, IDC_SIDCART_ADDRESS), CB_GETCURSEL, 0, 0));

  resources_set_int("SidClock",SendMessage(GetDlgItem(
                    hwnd, IDC_SIDCART_CLOCK), CB_GETCURSEL, 0, 0));

  resources_set_int("SidHardSIDMain",SendMessage(GetDlgItem(
                    hwnd, IDC_SIDCART_HARDSID_MAIN_DEVICE), CB_GETCURSEL, 0, 0));
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
  int command;

  switch (msg)
  {
    case WM_COMMAND:
      command = LOWORD(wparam);
      switch (command)
      {
        case IDC_SIDCART_ENGINE:
          enable_sidcart_hardsid_controls(hwnd);
          break;
        case IDC_SIDCART_ENABLE:
          enable_sidcart_controls(hwnd);
          break;
        case IDOK:
          end_sidcart_dialog(hwnd);
        case IDCANCEL:
          EndDialog(hwnd, 0);
          return TRUE;
      }
      return FALSE;
    case WM_CLOSE:
      EndDialog(hwnd, 0);
      return TRUE;
    case WM_INITDIALOG:
      init_sidcart_dialog(hwnd);
      return TRUE;
  }
  return FALSE;
}

void ui_sidcart_settings_dialog(HWND hwnd)
{
  DialogBox(winmain_instance, (LPCTSTR)translate_res(IDD_SIDCART_SETTINGS_DIALOG), hwnd,
            dialog_proc);
}
