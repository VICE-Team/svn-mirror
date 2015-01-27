/*
 * uic64memoryhacks.c
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

#include "c64-memory-hacks.h"
#include "uilib.h"
#include "uimenu.h"
#include "uic64memoryhacks.h"

UI_MENU_DEFINE_RADIO(MemoryHack)
UI_MENU_DEFINE_RADIO(C64_256Kbase)
UI_MENU_DEFINE_RADIO(PLUS60Kbase)

UI_CALLBACK(set_c64_256k_image_name)
{
    uilib_select_file((char *)UI_MENU_CB_PARAM, _("C64 256K file"), UILIB_FILTER_ALL);
}

UI_CALLBACK(set_plus60k_image_name)
{
    uilib_select_file((char *)UI_MENU_CB_PARAM, _("+60K file"), UILIB_FILTER_ALL);
}

UI_CALLBACK(set_plus256k_image_name)
{
    uilib_select_file((char *)UI_MENU_CB_PARAM, _("+256K file"), UILIB_FILTER_ALL);
}

static ui_menu_entry_t c64_memory_hacks_device_submenu[] = {
    { N_("None"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_MemoryHack,
      (ui_callback_data_t)MEMORY_HACK_NONE, NULL },
    { "C64 256K", UI_MENU_TYPE_TICK, (ui_callback_t)radio_MemoryHack,
      (ui_callback_data_t)MEMORY_HACK_C64_256K, NULL },
    { "+60K", UI_MENU_TYPE_TICK, (ui_callback_t)radio_MemoryHack,
      (ui_callback_data_t)MEMORY_HACK_PLUS60K, NULL },
    { "+256K", UI_MENU_TYPE_TICK, (ui_callback_t)radio_MemoryHack,
      (ui_callback_data_t)MEMORY_HACK_PLUS256K, NULL },
    { NULL }
};

static ui_menu_entry_t c64_256k_base_submenu[] = {
    { "$DE00-$DE7F", UI_MENU_TYPE_TICK, (ui_callback_t)radio_C64_256Kbase,
      (ui_callback_data_t)0xde00, NULL },
    { "$DE80-$DEFF", UI_MENU_TYPE_TICK, (ui_callback_t)radio_C64_256Kbase,
      (ui_callback_data_t)0xde80, NULL },
    { "$DF00-$DF7F", UI_MENU_TYPE_TICK, (ui_callback_t)radio_C64_256Kbase,
      (ui_callback_data_t)0xdf00, NULL },
    { "$DF80-$DFFF", UI_MENU_TYPE_TICK, (ui_callback_t)radio_C64_256Kbase,
      (ui_callback_data_t)0xdf80, NULL },
    { NULL }
};

static ui_menu_entry_t plus60k_base_submenu[] = {
    { "$D040", UI_MENU_TYPE_TICK, (ui_callback_t)radio_PLUS60Kbase,
      (ui_callback_data_t)0xd040, NULL },
    { "$D100", UI_MENU_TYPE_TICK, (ui_callback_t)radio_PLUS60Kbase,
      (ui_callback_data_t)0xd100, NULL },
    { NULL }
};

ui_menu_entry_t c64_memory_hacks_submenu[] = {
    { N_("C64 memory expansions hack device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c64_memory_hacks_device_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("C64 256K base address"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, c64_256k_base_submenu },
    { N_("C64 256K image name"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_c64_256k_image_name,
      (ui_callback_data_t)"C64_256Kfilename", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("+60K Base address"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, plus60k_base_submenu },
    { N_("+60K Image name"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_plus60k_image_name,
      (ui_callback_data_t)"PLUS60Kfilename", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("+256K image name"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_plus256k_image_name,
      (ui_callback_data_t)"PLUS256Kfilename", NULL },
    { NULL }
};
