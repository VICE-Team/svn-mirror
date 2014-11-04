/*
 * uitfe.c
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

#ifdef HAVE_TFE

#include <stdio.h>

#include "lib.h"
#include "uilib.h"
#include "uimenu.h"
#include "uitfe.h"
#include "util.h"

UI_MENU_DEFINE_TOGGLE(ETHERNET_ACTIVE)
UI_MENU_DEFINE_TOGGLE(ETHERNET_AS_RR)
UI_MENU_DEFINE_TOGGLE(TFEIOSwap)

UI_CALLBACK(set_interface_name)
{
    char *name = util_concat(_("Name"), ":", NULL);

    /* FIXME: might actually make some sense to use the file browser on *nix systems which
              actually DO have /dev/eth0. linux doesn't however */
    /* uilib_select_dev((char *)UI_MENU_CB_PARAM, _("Ethernet interface"), UILIB_FILTER_ETH); */
    uilib_select_string((char *)UI_MENU_CB_PARAM, _("Ethernet interface"), name);
    lib_free(name);
}

ui_menu_entry_t tfe_c64_submenu[] = {
    { N_("Enable"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_ETHERNET_ACTIVE, NULL, NULL },
    /* Translators: "RR-Net" is the name of the network addon and should not get translated */
    { N_("RR-Net compatibility mode"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_ETHERNET_AS_RR, NULL, NULL },
    { N_("Interface"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_interface_name,
      (ui_callback_data_t)"ETHERNET_INTERFACE", NULL },
    { NULL }
};

ui_menu_entry_t tfe_vic20_submenu[] = {
    { N_("Enable"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_ETHERNET_ACTIVE, NULL, NULL },
    /* Translators: "RR-Net" is the name of the network addon and should not get translated */
    { N_("MasC=uerade I/O swap"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_TFEIOSwap, NULL, NULL },
    { N_("RR-Net compatibility mode"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_ETHERNET_AS_RR, NULL, NULL },
    { N_("Interface"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_interface_name,
      (ui_callback_data_t)"ETHERNET_INTERFACE", NULL },
    { NULL }
};

#endif
