/*
 * uinetwork.c
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
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

#include "network.h"
#include "uinetwork.h"
#include "resources.h"
#include "translate.h"
#include "ui.h"
#include "vsync.h"
#include "mui.h"

#if 0
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
#endif

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_INTEGER, "NetworkServerPort", NULL, NULL },
  { NULL, MUI_TYPE_TEXT, "NetworkServerName", NULL, NULL },
  UI_END /* mandatory */
};

#define BTN_START_SERVER (256+0)
#define BTN_CONNECT_TO   (256+1)
#define BTN_DISCONNECT   (256+2)

static APTR build_gui(void)
{
  APTR app, ui, mode, start_server, connect_to, disconnect, cancel;

  app = mui_get_app();

  ui = GroupObject,
    Child, mode = StringObject,
      MUIA_Frame, MUIV_Frame_String,
      MUIA_FrameTitle, "Current mode",
    End,

    Child, GroupObject,
      MUIA_Frame, MUIV_Frame_Group,
      MUIA_Group_Horiz, TRUE,
      Child, ui_to_from[0].object = StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_FrameTitle, "TCP-Port",
        MUIA_String_Accept, "0123456789",
        MUIA_String_MaxLen, 5+1,
      End,
      Child, start_server = TextObject,
        ButtonFrame,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, "Start Server",
        MUIA_Text_PreParse, "\033c",
        MUIA_InputMode, MUIV_InputMode_RelVerify,
      End,
    End,

    Child, GroupObject,
      MUIA_Frame, MUIV_Frame_Group,
      MUIA_Group_Horiz, TRUE,
      Child, connect_to = TextObject,
        ButtonFrame,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, "Connect to",
        MUIA_Text_PreParse, "\033c",
        MUIA_InputMode, MUIV_InputMode_RelVerify,
      End,
      Child, ui_to_from[1].object = StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_Accept, ".0123456789",
        MUIA_String_MaxLen, 15+1,
      End,
    End,

    Child, GroupObject,
      MUIA_Frame, MUIV_Frame_Group,
      MUIA_Group_Horiz, TRUE,
      Child, disconnect = TextObject,
        ButtonFrame,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, "Disconnect",
        MUIA_Text_PreParse, "\033c",
        MUIA_InputMode, MUIV_InputMode_RelVerify,
      End,
      Child, cancel = TextObject,
        ButtonFrame,
        MUIA_Background, MUII_ButtonBack,
        MUIA_Text_Contents, "Cancel",
        MUIA_Text_PreParse, "\033c",
        MUIA_InputMode, MUIV_InputMode_RelVerify,
      End,
    End,
  End;

  if (ui != NULL) {
    DoMethod(start_server, MUIM_Notify, MUIA_Pressed, FALSE,
      app, 2, MUIM_Application_ReturnID, BTN_START_SERVER);

    DoMethod(connect_to, MUIM_Notify, MUIA_Pressed, FALSE,
      app, 2, MUIM_Application_ReturnID, BTN_CONNECT_TO);

    DoMethod(disconnect, MUIM_Notify, MUIA_Pressed, FALSE,
      app, 2, MUIM_Application_ReturnID, BTN_DISCONNECT);

    DoMethod(cancel,
      MUIM_Notify, MUIA_Pressed, FALSE,
      app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    switch(network_get_mode()) {
      case NETWORK_IDLE:
        set(mode, MUIA_String_Contents, "Idle");
        break;
      case NETWORK_SERVER:
        set(mode, MUIA_String_Contents, "Server listening");
        break;
      case NETWORK_SERVER_CONNECTED:
        set(mode, MUIA_String_Contents, "Connected server");
        break;
      case NETWORK_CLIENT:
        set(mode, MUIA_String_Contents, "Connected client");
        break;
    }
  }

  return ui;
}

void ui_network_dialog(void)
{
  APTR window = mui_make_simple_window(build_gui(), "Network Settings");

/* FIXME: Bad workaround */
  resources_set_value("EventSnapshotDir", (resource_value_t)"");

  if (window != NULL) {
    mui_add_window(window);
    ui_get_to(ui_to_from);
    set(window, MUIA_Window_Open, TRUE);
    switch (mui_run()) {
      case BTN_START_SERVER:
        ui_get_from(ui_to_from);
        if (network_start_server() < 0)
          ui_error("An error occured starting the server.");
        break;
      case BTN_CONNECT_TO:
        ui_get_from(ui_to_from);
        if (network_connect_client() < 0)
          ui_error("An error occured connecting the client.");
        break;
      case BTN_DISCONNECT:
        network_disconnect();
        break;
    }
    set(window, MUIA_Window_Open, FALSE);
    mui_rem_window(window);
    MUI_DisposeObject(window);
  }
}

