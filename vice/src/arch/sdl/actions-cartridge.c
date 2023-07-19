/** \file   actions-cartridge.c
 * \brief   UI action implementations for cartridge-related dialogs and settings (SDL)
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
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

#include "cartridge.h"
#include "keyboard.h"
#include "uiactions.h"
#include "uimenu.h"

#include "actions-cartridge.h"


/** \brief  Attach cartridge (.crt) action
 *
 * Show file selector to attach a .crt image file.
 *
 * \param[in]   unused  unused
 */
static void cart_attach_crt_action(void *unused)
{
    sdl_ui_menu_item_activate_by_action(ACTION_CART_ATTACH);
}

/** \brief  Attach raw cartridge action
 *
 * Show submenu to attach raw cartridge images.
 *
 * \param[in]   unused  unused
 */
static void cart_attach_raw_action(void *unused)
{
    sdl_ui_menu_item_activate_by_action(ACTION_CART_ATTACH_RAW);
}

/** \brief  Detach cartridge action
 *
 * \param[in]   unused  unused
 */
static void cart_detach_action(void *unused)
{
    cartridge_detach_image(-1);
}

/** \brief  Trigger freeze action
 *
 * \param[in]   unused  unused
 */
static void cart_freeze_action(void *unused)
{
    keyboard_clear_keymatrix();
    cartridge_trigger_freeze();
}


/** \brief  List of mappings for machine-related actions
 *
 * XXX:     Might have to split this list into machine-specific lists with a
 *          `machine_class` check in `action_cartridge_register()`.
 */
static const ui_action_map_t cartridge_actions[] = {
    {   .action  = ACTION_CART_ATTACH,
        .handler = cart_attach_crt_action,
        .dialog  = true
    },
    {   .action  = ACTION_CART_ATTACH_RAW,
        .handler = cart_attach_raw_action,
    },
    {   .action  = ACTION_CART_DETACH,
        .handler = cart_detach_action
    },
    {   .action  = ACTION_CART_FREEZE,
        .handler = cart_freeze_action
    },
    UI_ACTION_MAP_TERMINATOR
};


/** \brief  Register cartridge actions */
void actions_cartridge_register(void)
{
    ui_actions_register(cartridge_actions);
}
