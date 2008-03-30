/*
 * uispeed.c - Implementation of the custom speed dialog box.
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
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "ui.h"
#include "uilib.h"
#include "uispeed.h"
#include "winmain.h"


int ui_speed_current(void)
{
    int res_value;

    resources_get_value("Speed", (void *)&res_value);

    return res_value;
}
#if 0
static void init_speed_dialog(HWND hwnd)
{
    int res_value;
    char *speedstr;
    TCHAR *st_speedstr;

    resources_get_value("Speed", (void *)&res_value);

    speedstr = lib_msprintf("%i", res_value);
    st_speedstr = system_mbstowcs_alloc(speedstr);
    SetDlgItemText(hwnd, IDC_CUSTOM_SPEED, st_speedstr);
    system_mbstowcs_free(st_speedstr);
    lib_free(speedstr);
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    int command;
    int speed;
    TCHAR st[20];

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDOK:
            GetDlgItemText(hwnd, IDC_CUSTOM_SPEED, st, 20);
            speed = _ttoi(st);
            if (speed > 0 && speed < 1000000)
                resources_set_value("Speed", (resource_value_t)speed);
          case IDCANCEL:
            EndDialog(hwnd,0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        init_speed_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}
#endif

void ui_speed_settings_dialog(HWND hwnd)
{
/*
    DialogBox(winmain_instance, (LPCTSTR)translate_res(IDD_CUSTOM_SPEED_DIALOG), hwnd,
              dialog_proc);
*/
    uilib_dialogbox_param_t param;
    int speed;

    resources_get_value("Speed", (void *)&speed);

    param.hwnd = hwnd;
    param.idd_dialog = translate_res(IDD_CUSTOM_SPEED_DIALOG);
    param.idc_dialog = IDC_CUSTOM_SPEED;
    _itot(speed, param.string, 10);

    uilib_dialogbox(&param);

    if (param.updated > 0) {
        speed = _ttoi(param.string);
        if (speed > 0 && speed < 1000000)
            resources_set_value("Speed", (resource_value_t)speed);

    }
}

