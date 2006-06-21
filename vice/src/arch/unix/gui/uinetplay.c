/*
 *  * uinetplay.c - Generic UI controls for netplay
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

#ifdef HAVE_NETWORK

#include <stdio.h>
#include <stdlib.h>

#include "lib.h"
#include "network.h"
#include "resources.h"
#include "uilib.h"
#include "uimenu.h"
#include "uinetplay.h"
#include "vsync.h"

#ifdef HAVE_IPV6
UI_MENU_DEFINE_TOGGLE(NetworkIPV6)
#endif

#ifdef USE_GNOMEUI

static UI_CALLBACK(netplay)
{
    vsync_suspend_speed_eval();
    ui_netplay_dialog();
}

#else
static UI_CALLBACK(ui_netplay_set_port)
{
  static char input_string[32];
  char *msg_string;
  ui_button_t button;
  int i;
  int current_port;

  resources_get_value("NetworkServerPort", (void *)&current_port);
  if (!*input_string)
    sprintf(input_string, "%d", current_port);

  vsync_suspend_speed_eval();
  msg_string = lib_stralloc(_("Enter port"));
  button = ui_input_string(_("Netplay TCP port"), msg_string, input_string, 32);
  lib_free(msg_string);
  if (button == UI_BUTTON_OK)
  {
    i = atoi(input_string);
    if (i>0 && i<65536)
    {
      resources_set_value("NetworkServerPort", (resource_value_t)i);
      ui_update_menus();
    }
    else
    {
      ui_error(_("Invalid TCP port"));
    }
  }
}

static UI_CALLBACK(ui_netplay_start_server)
{
  if (network_start_server() < 0)
    ui_error(_("Error starting the netplay server."));
}

UI_CALLBACK(ui_netplay_set_host)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM, _("Netplay Server name"),
                        _("Hostname:"));
}

static UI_CALLBACK(ui_netplay_connect_to_server)
{
  if (network_connect_client() < 0)
    ui_error(_("Error connecting to server."));
}

static UI_CALLBACK(ui_netplay_disconnect)
{
  network_disconnect();
}
#endif /* USE_GNOMEUI */

ui_menu_entry_t netplay_submenu[] = {
#ifdef HAVE_IPV6
    { N_("*Enable IPV6"),
      (ui_callback_t)toggle_NetworkIPV6, NULL, NULL },
#endif
#ifdef USE_GNOMEUI
    { N_("Netplay (experimental)"),
      (ui_callback_t)netplay, NULL, NULL },
#else
    { N_("TCP port"),
      (ui_callback_t)ui_netplay_set_port, NULL, NULL },
    { N_("Start server"),
      (ui_callback_t)ui_netplay_start_server, NULL, NULL },
    { N_("Server to connect to"),
      (ui_callback_t)ui_netplay_set_host,
      (ui_callback_data_t)"NetworkServerName", NULL },
    { N_("Connect to server"),
      (ui_callback_t)ui_netplay_connect_to_server, NULL, NULL },
    { N_("Disconnect"),
      (ui_callback_t)ui_netplay_disconnect, NULL, NULL },
#endif /* USE_GNOMEUI */
    { NULL }
};

#endif	/* HAVE_NETWORK */
