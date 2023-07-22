/** \file   actions-help.c
 * \brief   UI action implementations for help dialogs (SDL)
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

#include "uiactions.h"
#include "uimenu.h"

#include "actions-help.h"


/** \brief  About dialog action
 *
 * \param[in]   self    action map
 */
static void help_about_action(ui_action_map_t *self)
{
    sdl_ui_menu_item_activate_by_action(self->action);
}

/** \brief  Command line options dialog action
 *
 * \param[in]   self    action map
 */
static void help_command_line_action(ui_action_map_t *self)
{
    sdl_ui_menu_item_activate_by_action(self->action);
}

/** \brief  Compile time features dialog action
 *
 * \param[in]   self    action map
 */
static void help_compile_time_action(ui_action_map_t *self)
{
    sdl_ui_menu_item_activate_by_action(self->action);
}


/** \brief  List of mappings for help actions */
static const ui_action_map_t help_actions[] = {
    {   .action  = ACTION_HELP_ABOUT,
        .handler = help_about_action,
        .dialog  = true
    },
    {   .action  = ACTION_HELP_COMMAND_LINE,
        .handler = help_command_line_action,
        .dialog  = true
    },
    {   .action  = ACTION_HELP_COMPILE_TIME,
        .handler = help_compile_time_action,
        .dialog  = true
    },
    UI_ACTION_MAP_TERMINATOR
};


/** \brief  Register help actions */
void actions_help_register(void)
{
    ui_actions_register(help_actions);
}
