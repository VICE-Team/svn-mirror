/*
 * uimidi.c - Implementation of the midi settings dialog box.
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
#include <mmsystem.h>
#include <tchar.h>

#include "midi.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "uilib.h"
#include "uimidi.h"
#include "winmain.h"

static void enable_midi_controls(HWND hwnd)
{
  int num_in, num_out;
  int is_enabled;

  num_in = midiInGetNumDevs();
  num_out = midiOutGetNumDevs();

  is_enabled = (IsDlgButtonChecked(hwnd, IDC_MIDI_ENABLE) == BST_CHECKED) ? 1 : 0;

  EnableWindow(GetDlgItem(hwnd, IDC_MIDI_TYPE), is_enabled && (num_in != 0 || num_out != 0));
  EnableWindow(GetDlgItem(hwnd, IDC_MIDI_IN_DEVICE), is_enabled && (num_in != 0));
  EnableWindow(GetDlgItem(hwnd, IDC_MIDI_OUT_DEVICE), is_enabled && (num_out != 0));
}

static void init_midi_dialog(HWND hwnd)
{
  HWND temp_hwnd;
  MMRESULT ret;
  MIDIINCAPS mic;
  MIDIOUTCAPS moc;
  int num, i;
  int res_value;
  int number;

  resources_get_int("MIDIEnable", &res_value);
  CheckDlgButton(hwnd, IDC_MIDI_ENABLE, res_value ? BST_CHECKED : BST_UNCHECKED);

  temp_hwnd = GetDlgItem(hwnd, IDC_MIDI_TYPE);
  for (i = 0; midi_interface[i].name != NULL; i++)
  {
    SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)midi_interface[i].name);
  }
  resources_get_int("MIDIMode", &res_value);
  SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

  temp_hwnd = GetDlgItem(hwnd, IDC_MIDI_IN_DEVICE);
  num = midiInGetNumDevs();
  if (num != 0)
  {
    for (i = 0; i < num; i++)
    {
      ret = midiInGetDevCaps(i, &mic, sizeof(MIDIINCAPS));
      if (ret == MMSYSERR_NOERROR)
      {
        SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)mic.szPname);
      }
      else
      {
        SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"Error getting name");
      }
    }
    resources_get_int("MIDIInDev", &number);
    SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)number, 0);
  }

  temp_hwnd = GetDlgItem(hwnd, IDC_MIDI_OUT_DEVICE);
  num = midiOutGetNumDevs();
  if (num != 0)
  {
    for (i = 0; i < num; i++)
    {
      ret = midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS));
      if (ret == MMSYSERR_NOERROR)
      {
        SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)moc.szPname);
      }
      else
      {
        SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)"Error getting name");
      }
    }
    resources_get_int("MIDIOutDev", &number);
    SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)number, 0);
  }

  enable_midi_controls(hwnd);
}

static void end_midi_dialog(HWND hwnd)
{
  resources_set_int("MIDIEnable", (IsDlgButtonChecked(hwnd,
                    IDC_MIDI_ENABLE) == BST_CHECKED ? 1 : 0 ));

  resources_set_int("MIDIMode",SendMessage(GetDlgItem(
                    hwnd, IDC_MIDI_TYPE), CB_GETCURSEL, 0, 0));

  resources_set_int("MIDIInDev",SendMessage(GetDlgItem(
                    hwnd, IDC_MIDI_IN_DEVICE), CB_GETCURSEL, 0, 0));

  resources_set_int("MIDIOutDev",SendMessage(GetDlgItem(
                    hwnd, IDC_MIDI_OUT_DEVICE), CB_GETCURSEL, 0, 0));
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
        case IDC_MIDI_ENABLE:
          enable_midi_controls(hwnd);
          break;
        case IDOK:
          end_midi_dialog(hwnd);
        case IDCANCEL:
          EndDialog(hwnd, 0);
          return TRUE;
      }
      return FALSE;
    case WM_CLOSE:
      EndDialog(hwnd, 0);
      return TRUE;
    case WM_INITDIALOG:
      init_midi_dialog(hwnd);
      return TRUE;
  }
  return FALSE;
}

void ui_midi_settings_dialog(HWND hwnd)
{
  DialogBox(winmain_instance, (LPCTSTR)translate_res(IDD_MIDI_SETTINGS_DIALOG), hwnd,
            dialog_proc);
}
