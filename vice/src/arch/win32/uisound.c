/*
 * uisound.c - Sound device configuration user interface for Win32 platforms.
 *
 * Written by
 *  Tibor Biczo <crown@mail.matav.hu>
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

#include <string.h>
#include <windows.h>

#include "res.h"
#include "resources.h"
#include "sound.h"
#include "system.h"
#include "ui.h"
#include "winmain.h"


static int ui_sound_freq[] = {
    8000,
    11025,
    22050,
    44100
};

static int ui_sound_buffer[] = {
    100,
    150,
    200,
    250,
    300,
    350
};

static int ui_sound_adjusting[] = {
    SOUND_ADJUST_FLEXIBLE,
    SOUND_ADJUST_ADJUSTING,
    SOUND_ADJUST_EXACT
};

static void init_sound_dialog(HWND hwnd)
{
    HWND snd_hwnd;
    int res_value;
    char *devicename;

    snd_hwnd = GetDlgItem(hwnd, IDC_SOUND_FREQ);
    SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("8000 Hz"));
    SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("11025 Hz"));
    SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("22050 Hz"));
    SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("44100 Hz"));
    resources_get_value("SoundSampleRate", (void *)&res_value);
    switch (res_value) {
      case 8000:
        res_value = 0;
        break;
      case 11025:
        res_value = 1;
        break;
      case 22050:
        res_value = 2;
        break;
      case 44100:
      default:
        res_value = 3;
        break;
    }
    SendMessage(snd_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    snd_hwnd = GetDlgItem(hwnd, IDC_SOUND_BUFFER);
    SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("100 msec"));
    SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("150 msec"));
    SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("200 msec"));
    SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("250 msec"));
    SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("300 msec"));
    SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("350 msec"));
    resources_get_value("SoundBufferSize", (void *)&res_value);
    switch (res_value) {
      case 100:
        res_value = 0;
        break;
      case 150:
        res_value = 1;
        break;
      case 200:
        res_value = 2;
        break;
      case 250:
        res_value = 3;
        break;
      case 300:
        res_value = 4;
        break;
       case 350:
       default:
        res_value = 5;
        break;
    }
    SendMessage(snd_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    snd_hwnd = GetDlgItem(hwnd, IDC_SOUND_OVERSAMPLE);
    SendMessage(snd_hwnd, CB_ADDSTRING,0, (LPARAM)TEXT("None"));
    SendMessage(snd_hwnd, CB_ADDSTRING,0, (LPARAM)TEXT("2x"));
    SendMessage(snd_hwnd, CB_ADDSTRING,0, (LPARAM)TEXT("4x"));
    SendMessage(snd_hwnd, CB_ADDSTRING,0, (LPARAM)TEXT("8x"));
    resources_get_value("SoundOversample", (void *)&res_value);
    SendMessage(snd_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    snd_hwnd=GetDlgItem(hwnd, IDC_SOUND_SYNCH);
    SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("Flexible"));
    SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("Adjusting"));
    SendMessage(snd_hwnd, CB_ADDSTRING, 0, (LPARAM)TEXT("Exact"));
    resources_get_value("SoundSpeedAdjustment", (void *)&res_value);
    switch (res_value) {
      case SOUND_ADJUST_FLEXIBLE:
      default:
        res_value = 0;
        break;
      case SOUND_ADJUST_ADJUSTING:
        res_value = 1;
        break;
      case SOUND_ADJUST_EXACT:
        res_value = 2;
        break;
    }
    SendMessage(snd_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    resources_get_value("SoundDeviceName", (void *)&devicename);
    if (devicename && !strcasecmp("wmm", devicename))
        res_value = IDC_SOUND_WMM;
    else
        res_value = IDC_SOUND_DIRECTX;
        
    CheckRadioButton(hwnd, IDC_SOUND_DIRECTX, IDC_SOUND_WMM, res_value);
}

static void end_sound_dialog(HWND hwnd)
{
    resources_set_value("SoundSampleRate",
                        (resource_value_t)ui_sound_freq[SendMessage(
                        GetDlgItem(hwnd,IDC_SOUND_FREQ),
                        CB_GETCURSEL, 0, 0)]);
    resources_set_value("SoundBufferSize",
                        (resource_value_t)ui_sound_buffer[SendMessage(
                        GetDlgItem(hwnd,IDC_SOUND_BUFFER),
                        CB_GETCURSEL, 0, 0)]);
    resources_set_value("SoundOversample",
                        (resource_value_t)SendMessage(
                        GetDlgItem(hwnd,IDC_SOUND_OVERSAMPLE),
                        CB_GETCURSEL, 0, 0));
    resources_set_value("SoundSpeedAdjustment",
                        (resource_value_t)ui_sound_adjusting[SendMessage(
                        GetDlgItem(hwnd, IDC_SOUND_SYNCH),
                        CB_GETCURSEL, 0, 0)]);
}

static void select_dx(void)
{
    resources_set_value("SoundDeviceName",(resource_value_t)"dx");
    ui_display_statustext("Sound driver: DirectX");
}

static void select_wmm(void)
{
    resources_set_value("SoundDeviceName",(resource_value_t)"wmm");
    ui_display_statustext("Sound driver: WMM");
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_sound_dialog(hwnd);
        return TRUE;
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_SOUND_DIRECTX:
            select_dx();
            break;
          case IDC_SOUND_WMM:
            select_wmm();
            break;
          case IDOK:
            end_sound_dialog(hwnd);
          case IDCANCEL:
            EndDialog(hwnd,0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void ui_sound_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, MAKEINTRESOURCE(IDD_SOUND_SETTINGS_DIALOG),
              hwnd, dialog_proc);
}

