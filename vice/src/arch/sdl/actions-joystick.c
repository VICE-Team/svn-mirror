/** \file   actions-joystick.c
 * \brief   UI action implementations for joystick (and mouse) settings (SDL)
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

#include <stddef.h>
#include <stdbool.h>

#include "joy.h"
#include "menu_common.h"
#include "resources.h"
#include "uiactions.h"
#include "uimenu.h"

#include "actions-joystick.h"

#ifdef HAVE_MOUSE
/** \brief  Toggle mouse grab action
 *
 * \param[in]   unused  unused
 */
static void mouse_grab_toggle_action(void *unused)
{
    int mouse = 0;

    resources_get_int("Mouse", &mouse);
    resources_set_int("Mouse", !mouse);
}
#endif

/** \brief  Toggle control port devices swap action
 *
 * \param[in]   unused  unused
 */
static void swap_controlport_toggle_action(void *unused)
{
    sdljoy_swap_ports();
}

/** \brief  Toggle keyset joysticks action
 *
 * \param[in]   unused  unused
 */
static void keyset_joystick_toggle_action(void *unused)
{
    int keyset = 0;

    resources_get_int("KeySetEnable", &keyset);
    resources_set_int("KeySetEnable", !keyset);
}


/** \brief  List of mappings for joystick and mouse actions
 */
static const ui_action_map_t joystick_actions[] = {
#ifdef HAVE_MOUSE
    {   .action  = ACTION_MOUSE_GRAB_TOGGLE,
        .handler = mouse_grab_toggle_action
    },
#endif
    {   .action  = ACTION_SWAP_CONTROLPORT_TOGGLE,
        .handler = swap_controlport_toggle_action
    },
    {   .action  = ACTION_KEYSET_JOYSTICK_TOGGLE,
        .handler = keyset_joystick_toggle_action
    },
    UI_ACTION_MAP_TERMINATOR
};


/** \brief  Register joystick/mouse actions */
void actions_joystick_register(void)
{
    ui_actions_register(joystick_actions);
}


/** \brief  Custom display callback for "swap-controlport-toggle"
 *
 * \param[in]   item    menu item (unused)
 */
const char *swap_controlport_toggle_display(ui_menu_entry_t *item)
{
    return sdljoy_get_swap_ports() ? MENU_CHECKMARK_CHECKED_STRING : NULL;
}
