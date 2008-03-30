/*
 * uic64cart.c - Implementation of the C64/C128 cart-specific part of the UI.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Nathan Huizinga <nathan.huizinga@chess.nl>
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

#include "c64cart.h"
#include "cartridge.h"
#include "lib.h"
#include "ui.h"
#include "uic64cart.h"
#include "uicartridge.h"
#include "uimenu.h"
#include "util.h"
#include "vsync.h"


static UI_CALLBACK(attach_cartridge)
{
    int type = (int)UI_MENU_CB_PARAM;

    vsync_suspend_speed_eval();

    switch (type) {
      case CARTRIDGE_EXPERT:
        /*
         * Expert cartridge has *no* image file.
         * It's only emulation that should be enabled!
         */
        if (cartridge_attach_image(type, NULL) < 0)
            ui_error(_("Cannot attach cartridge"));
        break;

      default:
        {
            char *filename;
            ui_button_t button;
            static char *last_dir;

            filename = ui_select_file(_("Attach cartridge image"),
                                      NULL, 0, False, last_dir,
                                      "*.[cCbB][rRiI][tTnN]",
                                      &button, False, NULL);

            switch (button) {
              case UI_BUTTON_OK:
                if (cartridge_attach_image(type, filename) < 0)
                    ui_error(_("Cannot attach cartridge"));
                if (last_dir)
                    lib_free(last_dir);
                util_fname_split(filename, &last_dir, NULL);
                break;
              default:
                /* Do nothing special. */
                break;
            }
            if (filename != NULL)
                lib_free(filename);
        }
    }
    ui_update_menus();
}

static UI_CALLBACK(detach_cartridge)
{
    cartridge_detach_image();
    ui_update_menus();
}

static UI_CALLBACK(default_cartridge)
{
    cartridge_set_default();
}

static UI_CALLBACK(freeze_cartridge)
{
    cartridge_trigger_freeze();
}

static UI_CALLBACK(control_cartridge)
{
    if (!CHECK_MENUS) {
        ui_update_menus();
    } else {
        switch (mem_cartridge_type) {
          case CARTRIDGE_EXPERT:
            ui_menu_set_sensitive(w, True);
            break;
          default:
            ui_menu_set_sensitive(w, False);
            break;
        }
    }
}

static UI_CALLBACK(save_cartridge)
{
    ui_cartridge_dialog();
}

UI_MENU_DEFINE_RADIO(CartridgeMode)

static ui_menu_entry_t cartridge_control_submenu[] = {
    { "*Prg", (ui_callback_t)radio_CartridgeMode,
      (ui_callback_data_t)CARTRIDGE_MODE_PRG, NULL },
    { "*Off", (ui_callback_t)radio_CartridgeMode,
      (ui_callback_data_t)CARTRIDGE_MODE_OFF, NULL },
    { "*On", (ui_callback_t)radio_CartridgeMode,
      (ui_callback_data_t)CARTRIDGE_MODE_ON, NULL },
    { "--" },
    { N_("Save cartridge image..."),
      (ui_callback_t)save_cartridge, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t attach_cartridge_image_submenu[] = {
    { N_("Smart attach CRT image..."),
      (ui_callback_t)attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_CRT, NULL },
    { "--" },
    { N_("Attach generic 8KB image..."),
      (ui_callback_t)attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_GENERIC_8KB, NULL },
    { N_("Attach generic 16KB image..."),
      (ui_callback_t)attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_GENERIC_16KB, NULL },
    { N_("Attach Action Replay image..."),
      (ui_callback_t)attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_ACTION_REPLAY, NULL },
    { N_("Attach Atomic Power image..."),
      (ui_callback_t)attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_ATOMIC_POWER, NULL },
    { N_("Attach Epyx fastload image..."),
      (ui_callback_t)attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_EPYX_FASTLOAD, NULL },
    { N_("Attach IEEE488 interface image..."),
      (ui_callback_t)attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_IEEE488, NULL },
    { N_("Attach Retro Replay image..."),
      (ui_callback_t)attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_RETRO_REPLAY, NULL },
    { N_("Attach IDE64 interface image..."),
      (ui_callback_t)attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_IDE64, NULL },
    { N_("Attach Super Snapshot 4 image..."),
      (ui_callback_t)attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_SUPER_SNAPSHOT, NULL },
    { N_("Attach Super Snapshot 5 image..."),
      (ui_callback_t)attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_SUPER_SNAPSHOT_V5, NULL },
    { N_("Attach Structured Basic image..."),
      (ui_callback_t)attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_STRUCTURED_BASIC, NULL },
    { "--" },
    { N_("Enable Expert Cartridge..."),
      (ui_callback_t)attach_cartridge, (ui_callback_data_t)
      CARTRIDGE_EXPERT, NULL },
    { "--" },
    { N_("Set cartridge as default"), (ui_callback_t)
      default_cartridge, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_c64cart_commands_menu[] = {
    { N_("Attach a cartridge image"),
      NULL, NULL, attach_cartridge_image_submenu },
    { N_("Detach cartridge image"),
      (ui_callback_t)detach_cartridge, NULL, NULL },
    { N_("Cartridge freeze"),
      (ui_callback_t)freeze_cartridge, NULL, NULL, XK_z, UI_HOTMOD_META },
    { N_("*Cartridge control"),
      (ui_callback_t)control_cartridge,
      (ui_callback_data_t)0, cartridge_control_submenu },
    { NULL }
};

