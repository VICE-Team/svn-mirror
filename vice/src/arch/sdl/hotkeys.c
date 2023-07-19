/** \file   hotkeys.c
 * \brief   SDL custom hotkeys handling
 *
 * Provides custom keyboard shortcuts for the SDL UI.
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
 */

#include "vice.h"
#include "vice_sdl.h"
#include <stdbool.h>
#include <stdint.h>

#include "uiapi.h"
#include "uihotkeys.h"
#include "uimenu.h"

#include "hotkeys.h"


/******************************************************************************
 *      Hotkeys API virtual method implementations (currently just stubs)     *
 *****************************************************************************/

uint32_t ui_hotkeys_arch_modifier_to_arch(uint32_t vice_mod)
{
    return 0;
}

uint32_t ui_hotkeys_arch_modifier_from_arch(uint32_t arch_mod)
{
    return 0;
}

uint32_t ui_hotkeys_arch_keysym_to_arch(uint32_t vice_keysym)
{
    return 0;
}

uint32_t ui_hotkeys_arch_keysym_from_arch(uint32_t arch_keysym)
{
    return 0;
}

uint32_t ui_hotkeys_arch_modmask_to_arch(uint32_t vice_modmask)
{
    return 0;
}

uint32_t ui_hotkeys_arch_modmask_from_arch(uint32_t arch_modmask)
{
    return 0;
}

void     ui_hotkeys_arch_install_by_map(ui_action_map_t *map)
{
}

void     ui_hotkeys_arch_remove_by_map(ui_action_map_t *map)
{
}

void     ui_hotkeys_arch_init(void)
{
}

void     ui_hotkeys_arch_shutdown(void)
{
}


/* for debugging */
static int submenu_count = 0;
static int item_count    = 0;
static int action_count  = 0;

/** \brief  Iterate a (sub)menu and item references in the action mappings
 *
 * \param[in]   menu    (sub)menu
 * \param[in]   indent  initial indentation level for pretty printing (use \<;0
 *                      to disable printing entirely)
 */
static void iter_menu(ui_menu_entry_t *menu, int indent)
{
    while (menu != NULL && menu->string != NULL) {
        ui_menu_entry_type_t  type   = menu->type;
        char                 *string = menu->string;
        void                 *data   = menu->data;
        int                   action = menu->action;

        if (indent >= 0) {
            for (int i = 0; i < indent * 4; i++) {
                putchar(' ');
            }
        }

        /* we don't care about DYNAMIC_SUBMENU since the menu code doesn't
         * allow setting hotkeys for those */
        if (type == MENU_ENTRY_SUBMENU) {
            submenu_count++;
            if (indent >= 0) {
                printf("  %s >>\n", string);
                iter_menu(data, indent + 1);
            } else {
                iter_menu(data, indent);
            }
        } else {
            int c = ' ';
            if (type == MENU_ENTRY_RESOURCE_TOGGLE ||
                type == MENU_ENTRY_OTHER_TOGGLE) {
                c = 'v';
            } else if (type == MENU_ENTRY_RESOURCE_RADIO) {
                c = 'o';
            }

            if (*string == '\0') {
                /* empty string is a separator */
                if (indent >= 0) {
                    printf("  -  -  -  -  - - \n");
                }
            } else if (type == MENU_ENTRY_TEXT && data != NULL) {
                /* inverted text (header) */
                if (indent >= 0) {
                    printf("  [%s]\n", string);
                }
            } else {
                if (indent >= 0) {
                    printf("%c %s", c, string);
                    if (action > 0) {
                        printf("   (%d: %s)", action, ui_action_get_name(action));
                    }
                    printf("\n");
                }
            }
            item_count++;
            if (action > ACTION_NONE) {
                ui_action_map_t *map = ui_action_map_get(action);
                if (map != NULL) {
#if 0
                    printf("%s(): adding item reference for action %d (%s)\n",
                           __func__, action, ui_action_get_name(action));
#endif
                    map->menu_item[0] = menu;
                    action_count++;
                }
            }
        }
        menu++;
    }
}


/** \brief  Iterate menus and store references in the action mappings
 *
 * Iterate all (sub)menus and their items and store references to them in the
 * UI action mappings array if a valid action ID is found.
 */
void hotkeys_iterate_menu(void)
{
    /* disable printing of menu structure by passing <0 indentation */
    iter_menu(sdl_ui_get_main_menu(), -1);
#if 0
    printf("%s(): %d (sub)menus with %d items, found %d action IDs.\n",
           __func__, submenu_count, item_count, action_count);
#endif
}
