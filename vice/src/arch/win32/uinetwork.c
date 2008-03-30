/*
 * uinetwork.c - Implementation of the network dialog.
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
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include <commdlg.h>

#include "network.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "ui.h"
#include "vsync.h"
#include "winmain.h"


static void init_network_dialog(HWND hwnd)
{
    int port;
    char *server_name;
    TCHAR st[256];
    int connected;

    resources_get_value("NetworkServerPort", (void *)&port);
    resources_get_value("NetworkServerName", (void *)&server_name);

    _stprintf(st, TEXT("%d"), port);
    SetDlgItemText(hwnd, IDC_NETWORK_PORT, st);
    SetDlgItemText(hwnd, IDC_NETWORK_SERVERNAME, TEXT(server_name));

    switch(network_get_mode()) {
        case NETWORK_IDLE:
            SetDlgItemText(hwnd, IDC_NETWORK_MODE, "Idle");
            break;
        case NETWORK_SERVER:
            SetDlgItemText(hwnd, IDC_NETWORK_MODE, "Server listening");
            break;
        case NETWORK_SERVER_CONNECTED:
            SetDlgItemText(hwnd, IDC_NETWORK_MODE, "Connected server");
            break;
        case NETWORK_CLIENT:
            SetDlgItemText(hwnd, IDC_NETWORK_MODE, "Connected client");
            break;
    }

    connected = ((network_get_mode() != NETWORK_IDLE) ? 1 : 0);
    EnableWindow(GetDlgItem(hwnd, IDC_NETWORK_PORT), !connected);
    EnableWindow(GetDlgItem(hwnd, IDC_NETWORK_SERVERNAME), !connected);
    EnableWindow(GetDlgItem(hwnd, IDC_NETWORK_CLIENT), !connected);
    EnableWindow(GetDlgItem(hwnd, IDC_NETWORK_SERVER), !connected);
    EnableWindow(GetDlgItem(hwnd, IDC_NETWORK_DISCONNECT), connected);
    EnableWindow(GetDlgItem(hwnd, IDC_NETWORK_MODE), 0);

    SetFocus(connected ? GetDlgItem(hwnd, IDC_NETWORK_DISCONNECT)
                        : GetDlgItem(hwnd, IDC_NETWORK_SERVER));
}

static int set_resources(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    int port;

    GetDlgItemText(hwnd, IDC_NETWORK_PORT, st, MAX_PATH);
    port = atoi(st);
    if (port < 1 || port > 0xFFFF) {
        ui_error("Invalid port number");
        return -1;
    }

    resources_set_value("NetworkServerPort",
                        (resource_value_t)port);

    GetDlgItemText(hwnd, IDC_NETWORK_SERVERNAME, st, MAX_PATH);
    resources_set_value("NetworkServerName", (resource_value_t)st);

    return 0;
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command=LOWORD(wparam);
        switch (command) {
          case IDCANCEL:
            EndDialog(hwnd,0);
            return TRUE;
          case IDC_NETWORK_SERVER:
            if (set_resources(hwnd) == 0)
                if (network_start_server() < 0)
                    ui_error("An error occured starting the server.");
            EndDialog(hwnd,0);
            return TRUE;
          case IDC_NETWORK_CLIENT:
            if (set_resources(hwnd) == 0)
                if (network_connect_client() < 0)
                    ui_error("An error occured connecting the client.");
            EndDialog(hwnd,0);
            return TRUE;
          case IDC_NETWORK_DISCONNECT:
            network_disconnect();
            EndDialog(hwnd,0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd,0);
        return TRUE;
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_network_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}


void ui_network_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, MAKEINTRESOURCE(translate_res(IDD_NETWORK_SETTINGS_DIALOG)),
              hwnd, dialog_proc);
}

