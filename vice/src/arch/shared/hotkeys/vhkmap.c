/** \file   vhkmap.c
 * \brief   Mapping of hotkeys to actions and menu items
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
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "hotkeystypes.h"
#include "vhkkeysyms.h"
#include "uiactions.h"
#include "uiapi.h"

#include "vhkmap.h"

#define DEBUG_VHK
#include "vhkdebug.h"

#define ARRAY_LEN(arr)  (sizeof (arr) / sizeof (arr[0]) )


/** \brief  Mappings of hotkeys to actions
 *
 * Array with one element per UI action, the index into the array is the action
 * ID for fast access.
 */
static vhk_map_t vhk_mappings[ACTION_ID_COUNT];


/** \brief  Initialize mapping to default values
 *
 * Set default values (no hotkey assigned, no menu items or data) on \a map.
 *
 * \param[in]   map hotkey mapping
 */
static void init_map(vhk_map_t *map)
{
    map->action       = ACTION_NONE;
    map->vice_keysym  = 0;
    map->vice_modmask = VHK_MOD_NONE;
    map->arch_keysym  = 0;
    map->arch_modmask = 0;
    map->menu_item[0] = NULL;
    map->menu_item[1] = NULL;
    map->user_data    = NULL;
}

/** \brief  Remove registered hotkey from map
 *
 * Clears hotkey keysyms and modifier masks in \a map.
 *
 * \param[in]   map hotkey mapping
 */
static void unset_map(vhk_map_t *map)
{
    map->vice_keysym  = 0;
    map->vice_modmask = VHK_MOD_NONE;
    map->arch_keysym  = 0;
    map->arch_modmask = 0;
}


/** \brief  Test if action ID is a valid index in the mappings array
 *
 * \param[in]   action  action ID
 *
 * \return  `true` if \a action is a valid index
 *
 * \note    Returns `true` for `ACTION_NONE` (0) since that's a valid index
 */
static bool is_valid_index(int action)
{
    if (action < ACTION_NONE || action >= (int)ARRAY_LEN(vhk_mappings)) {
        log_warning(vhk_log,
                    "Hotkeys: %s(): invalid action map index %d.",
                    __func__, action);
        return false;
    }
    return true;
}


/** \brief  Initialize mappings array
 *
 * This needs to happen early in the boot sequence, before the menu items are
 * generated (and their references added to the array) and the hotkeys are
 * parsed.
 *
 * Order (in the Gtk3 UI):
 *  * Initialize table with `vhk_map_init_mappings()` (currently in main.c)
 *  * Initialize Gtk accelerators group (in arch/gtk3/ui.c, during window
 *    creation)
 *  * Generate menu items from `ui_menu_item_t` declarations, storing references
 *    to the items, the signal handler IDs for the items and their locked/unlocked
 *    connection property in the array (in arch/gtk3/ui.c, during window
 *    creation)
 *    See `ui_menu_add()` in arch/gtk3/uimenu.c
 *  * Parse hotkeys files and set accelerators to trigger UI actions, using
 *    the locked/unlocked property to connect the handlers using the correct
 *    locking wrapper (`g_signal_connect()` vs `g_signal_connect_unlocked()`)
 */
void vhk_map_init_mappings(void)
{
    size_t i;

    debug_vhk("initializing vhk_mappings[]");

    for (i = 0; i < sizeof vhk_mappings / sizeof vhk_mappings[0]; i++) {
        vhk_map_t *map = &vhk_mappings[i];

        /* although the mappings are indexible by action ID, when passing a
         * pointer to a mapping this index will be lost, so we store the action
         * ID as well */
        init_map(map);
        vhk_mappings[i].action = (int)i;
    }
}


/** \brief  Set hotkey mapping for action
 *
 * \param[in]   action          action ID
 * \param[in]   vice_keysym     VICE keysym
 * \param[in]   vice_modmask    VICE modifier mask
 * \param[in]   arch_keysym     arch keysym
 * \param[in]   arch_modmask    arch modifier mask
 * \param[in]   primary_item    menu item for primary display
 * \param[in]   secondary_item  menu item for secondary display
 *
 * \return  pointer to mapping or `NULL` on error
 */
vhk_map_t *vhk_map_set(int       action,
                       uint32_t  vice_keysym,
                       uint32_t  vice_modmask,
                       uint32_t  arch_keysym,
                       uint32_t  arch_modmask,
                       void     *primary_item,
                       void     *secondary_item)
{
    vhk_map_t *map;

    if (!is_valid_index(action)) {
        return NULL;
    }

    map = &vhk_mappings[action];
    map->action       = action;
    map->vice_keysym  = vice_keysym;
    map->vice_modmask = vice_modmask;
    map->arch_keysym  = arch_keysym;
    map->arch_modmask = arch_modmask;
    if (primary_item != NULL) {
        map->menu_item[0] = primary_item;
    }
    if (secondary_item != NULL) {
        map->menu_item[1] = secondary_item;
    }
    return map;
}


/** \brief  Get hotkey mapping by action ID
 *
 * \param[in]   action  action ID
 *
 * \return  pointer to mapping or `NULL` when \a action isn't valid
 */
vhk_map_t *vhk_map_get(int action)
{
    if (!is_valid_index(action)) {
        return NULL;
    }
    return &vhk_mappings[action];
}


/** \brief  Get hotkey mapping by VICE keysym and modifier mask
 *
 * \param[in]   vice_keysym     VICE keysym
 * \param[in]   vice_modmask    VICE modifier mask
 *
 * \return  pointer to mapping or `NULL` not found
 *
 * \note    Returns `NULL` when \a keysym is 0.
 */
vhk_map_t *vhk_map_get_by_hotkey(uint32_t vice_keysym, uint32_t vice_modmask)
{
    if (vice_keysym != 0) {
        size_t action;

        for (action = 0; action < ARRAY_LEN(vhk_mappings); action++) {
            vhk_map_t *map = &vhk_mappings[action];

            if (map->vice_keysym == vice_keysym && map->vice_modmask == vice_modmask) {
                return map;
            }
        }
    }
    return NULL;
}


/** \brief  Get hotkey mapping arch by keysym and modifier mask
 *
 * \param[in]   arch_keysym     arch keysym
 * \param[in]   arch_modmask    arch modifier mask
 *
 * \return  pointer to mapping or `NULL` not found
 *
 * \note    Returns `NULL` when \a arch_keysym is 0.
 */
vhk_map_t *vhk_map_get_by_arch_hotkey(uint32_t arch_keysym, uint32_t arch_modmask)
{
    uint32_t vice_keysym  = ui_hotkeys_arch_keysym_from_arch(arch_keysym);
    uint32_t vice_modmask = ui_hotkeys_arch_modmask_from_arch(arch_modmask);

    debug_vhk("looking up vhk map for (%04x,%08x == %04x,%08x)",
              vice_keysym, vice_modmask, arch_keysym, arch_modmask);
    return vhk_map_get_by_hotkey(vice_keysym, vice_modmask);
}

/** \brief  Unset mapping by action ID
 *
 * \param[in]   action ID
 */
void vhk_map_unset(int action)
{
    vhk_map_t *map = vhk_map_get(action);
    if (map != NULL) {
        unset_map(map);
    }
}


/** \brief  Unset mapping by hotkey
 *
 * \param[in]   keysym      VICE keysym
 * \param[in]   modifiers   VICE modifier mask
 */
void vhk_map_unset_by_hotkey(uint32_t keysym, uint32_t modifiers)
{
    vhk_map_t *map = vhk_map_get_by_hotkey(keysym, modifiers);
    if (map != NULL) {
       unset_map(map);
    }
}


/** \brief  Unset mapping by reference
 *
 * \param[in]   map hotkey mapping
 */
void vhk_map_unset_by_map(vhk_map_t *map)
{
    if (map != NULL) {
        unset_map(map);
    }
}


/** \brief  Set new hotkey by map
 *
 * \param[in]   map             hotkey mapping
 * \param[in]   vice_keysym     VICE keysym
 * \param[in]   vice_modmask    VICE modifier mask
 * \param[in]   arch_keysym     arch keysym
 * \param[in]   arch_modmask    arch modifier mask
 */
void vhk_map_set_hotkey_by_map(vhk_map_t *map,
                               uint32_t   vice_keysym,
                               uint32_t   vice_modmask,
                               uint32_t   arch_keysym,
                               uint32_t   arch_modmask)
{
    if (map != NULL) {
        map->vice_keysym  = vice_keysym;
        map->vice_modmask = vice_modmask;
        map->arch_keysym  = arch_keysym;
        map->arch_modmask = arch_modmask;
    }
}
