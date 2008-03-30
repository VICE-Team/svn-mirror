/*
 * uisound.c - Sound device configuration user interface for Win32 platforms.
 *
 * Written by
 *  Tibor Biczo  (crown@mail.matav.hu)
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

#include <windows.h>
#include "res.h"
#include "resources.h"
#include "sound.h"
#include "winmain.h"

static int ui_sound_freq[]={
    8000,
    11025,
    22050,
    44100
};

static int ui_sound_buffer[]={
    50,
    100,
    150,
    200,
    250,
    300,
    350
};

static int ui_sound_adjusting[]={
    SOUND_ADJUST_FLEXIBLE,
    SOUND_ADJUST_ADJUSTING,
    SOUND_ADJUST_EXACT
};

static void init_sound_dialog(HWND hwnd)
{
HWND    snd_hwnd;
int     res_value;

    snd_hwnd=GetDlgItem(hwnd,IDC_SOUND_FREQ);
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"8000 Hz");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"11025 Hz");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"22050 Hz");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"44100 Hz");
    resources_get_value("SoundSampleRate",(resource_value_t *)&res_value);
    switch (res_value) {
        case 8000:
            res_value=0;
            break;
        case 11025:
            res_value=1;
            break;
        case 22050:
            res_value=2;
            break;
        case 44100:
        default:
            res_value=3;
            break;
    }
    SendMessage(snd_hwnd,CB_SETCURSEL,(WPARAM)res_value,0);

    snd_hwnd=GetDlgItem(hwnd,IDC_SOUND_BUFFER);
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"50 msec");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"100 msec");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"150 msec");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"200 msec");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"250 msec");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"300 msec");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"350 msec");
    resources_get_value("SoundBufferSize",(resource_value_t *)&res_value);
    switch (res_value) {
        case 50:
            res_value=0;
            break;
        case 100:
            res_value=1;
            break;
        case 150:
            res_value=2;
            break;
        case 200:
            res_value=3;
            break;
        case 250:
            res_value=4;
            break;
        case 300:
            res_value=5;
            break;
        case 350:
        default:
            res_value=6;
            break;
    }
    SendMessage(snd_hwnd,CB_SETCURSEL,(WPARAM)res_value,0);

    snd_hwnd=GetDlgItem(hwnd,IDC_SOUND_OVERSAMPLE);
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"None");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"2x");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"4x");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"8x");
    resources_get_value("SoundOversample",(resource_value_t *)&res_value);
    SendMessage(snd_hwnd,CB_SETCURSEL,(WPARAM)res_value,0);

    snd_hwnd=GetDlgItem(hwnd,IDC_SOUND_SYNCH);
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"Flexible");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"Adjusting");
    SendMessage(snd_hwnd,CB_ADDSTRING,0,(LPARAM)"Exact");
    resources_get_value("SoundSpeedAdjustment",(resource_value_t *)&res_value);
    switch (res_value) {
        case SOUND_ADJUST_FLEXIBLE:
        default:
            res_value=0;
            break;
        case SOUND_ADJUST_ADJUSTING:
            res_value=1;
            break;
        case SOUND_ADJUST_EXACT:
            res_value=2;
            break;
    }
    SendMessage(snd_hwnd,CB_SETCURSEL,(WPARAM)res_value,0);
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
int     command;

    switch (msg) {
        case WM_CLOSE:
            EndDialog(hwnd,0);
            return TRUE;
        case WM_INITDIALOG:
            init_sound_dialog(hwnd);
            return TRUE;
        case WM_COMMAND:
            command=LOWORD(wparam);
            switch (command) {
                case IDOK:
                    resources_set_value("SoundSampleRate",(resource_value_t)ui_sound_freq[SendMessage(GetDlgItem(hwnd,IDC_SOUND_FREQ),CB_GETCURSEL,0,0)]);
                    resources_set_value("SoundBufferSize",(resource_value_t)ui_sound_buffer[SendMessage(GetDlgItem(hwnd,IDC_SOUND_BUFFER),CB_GETCURSEL,0,0)]);
                    resources_set_value("SoundOversample",(resource_value_t)SendMessage(GetDlgItem(hwnd,IDC_SOUND_OVERSAMPLE),CB_GETCURSEL,0,0));
                    resources_set_value("SoundSpeedAdjustment",(resource_value_t)ui_sound_adjusting[SendMessage(GetDlgItem(hwnd,IDC_SOUND_SYNCH),CB_GETCURSEL,0,0)]);
                case IDC_CANCEL:
                    EndDialog(hwnd,0);
                    return TRUE;
            }
            break;
    }
    return FALSE;
}

void ui_sound_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance,(LPCTSTR)IDD_SOUND_SETTINGS_DIALOG,hwnd,dialog_proc);
}
