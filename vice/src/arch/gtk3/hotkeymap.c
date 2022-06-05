/** \file   hotkeymap.c
 * \brief   Mapping of hotkeys to actions and menu items
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

#include <gtk/gtk.h>
#include "lib.h"
#include "ui.h"
#include "uiactions.h"

#include "hotkeymap.h"


/** \brief  Mappings list head */
static hotkey_map_t *maps_head = NULL;

/** \brief  Mappings list tail */
static hotkey_map_t *maps_tail = NULL;

/** \brief  Number of registered hotkey mappings */
static int maps_count = 0;



/** \brief  Free memory used by all hotkey maps */
void hotkey_map_shutdown(void)
{
    hotkey_map_t *node = maps_head;

    while (node != NULL) {
        hotkey_map_t *next = node->next;
        lib_free(node);
        node = next;
    }
    maps_head = NULL;
    maps_tail = NULL;
}


/** \brief  Get number of hotkey maps
 */
int hotkey_map_count(void)
{
    return maps_count;
}


/** \brief  Get hotkey map list head node
 *
 * \return  head node
 */
hotkey_map_t *hotkey_map_get_head(void)
{
    return maps_head;
}


/** \brief  Get hotkey map list tail node
 *
 * \return  tail node
 */
hotkey_map_t *hotkey_map_get_tail(void)
{
    return maps_tail;
}


/** \brief  Clear hotkeys map object
 *
 * Sets all members of \a map to 0/NULL.
 *
 * \param[in]   map hotkey map
 */
void hotkey_map_clear(hotkey_map_t *map)
{
    map->keysym = 0;
    map->modifier = 0;
    map->action = ACTION_NONE;
    map->item[PRIMARY_WINDOW] = NULL;
    map->item[SECONDARY_WINDOW] = NULL;
    map->next = NULL;
    map->prev = NULL;
}


/** \brief  Allocate and initialize new hotkey map object
 *
 * \return  new hotkey map
 */
hotkey_map_t *hotkey_map_new(void)
{
    hotkey_map_t *map = lib_malloc(sizeof *map);
    hotkey_map_clear(map);
    return map;
}


/** \brief  Append hotkey map to list of maps
 *
 */
void hotkey_map_append(hotkey_map_t *map)
{
    if (maps_tail == NULL) {
        map->next = map->prev = NULL;
        maps_head = maps_tail = map;
    } else {
        maps_tail->next = map;
        map->prev = maps_tail;
        map->next = NULL;
        maps_tail = map;
    }
    maps_count++;
}


/** \brief  Delete hotkey map from the list and free it
 *
 * \param[in]   map hotkey map
 */
void hotkey_map_delete(hotkey_map_t *map)
{
    if (map->next == NULL && map->prev == NULL) {
        if (map == maps_head) {
            /* the only node in the list */
            lib_free(map);
            maps_head = NULL;
            maps_tail = NULL;
        }
        /* node isn't in the list, ignore */
        return;
    } else if (map->next == NULL) {
        /* tail node */
        map->prev->next = NULL;
        maps_tail = map->prev;
    } else if (map->prev == NULL) {
        /* head node */
        map->next->prev = NULL;
        maps_head = map->next;
    } else {
        /* inside node */
        map->prev->next = map->next;
        map->next->prev = map->prev;
    }
    lib_free(map);
    maps_count--;
}


/** \brief  Delete hotkey map for given UI action from list and free it
 *
 * Look up hotkey map for \a action and if found remove it from list and free
 * it.
 *
 * \param[in]   action  UI action ID
 */
void hotkey_map_delete_by_action(int action)
{
    hotkey_map_t *map = hotkey_map_get_by_action(action);
    if (map != NULL) {
        hotkey_map_delete(map);
    }
}


/** \brief  Delete hotkey map for given hotkey from list and free it
 *
 * Look up hotkey map for \a keysym and \a modifier and if found remove it from
 * the list and free it.
 *
 * \param[in]   keysym      Gdk keysym
 * \param[in]   modifier    Gdk modifier mask
 */
void hotkey_map_delete_by_hotkey(guint keysym, GdkModifierType modifier)
{
    hotkey_map_t *map = hotkey_map_get_by_hotkey(keysym, modifier);
    if (map != NULL) {
        hotkey_map_delete(map);
    }
}


/** \brief  Look up hotkey map by UI action ID
 *
 * \param[in]   action  UI action ID
 *
 * \return  hotkey map or `NULL` when not found
 */
hotkey_map_t *hotkey_map_get_by_action(int action)
{
    if (action <= ACTION_NONE) {
        return NULL;
    } else {
        hotkey_map_t *node;

        for (node = maps_head; node != NULL; node = node->next) {
            if (node->action == action) {
                return node;
            }
        }
    }
    return NULL;
}


/** \brief  Look up hotkey map by hotkey
 *
 * \param[in]   keysym      Gdk keysym
 * \param[in]   modifier    Gdk modifier mask
 *
 * \return  hotkey map or `NULL` when not found
 */

hotkey_map_t *hotkey_map_get_by_hotkey(guint keysym, GdkModifierType modifier)
{
    hotkey_map_t *node;

    for (node = maps_head; node != NULL; node = node->next) {
        if (node->keysym == keysym && node->modifier == modifier) {
            return node;
        }
    }
    return NULL;
}


/** \brief  Get accelerator label for hotkey in \a map
 *
 * \param[in]   map hotkey map
 *
 * \return  accelerator label or `NULL` when no hotkey defined
 *
 * \note    The value returned is allocated by Gtk and should be freed after
 *          use with g_free()
 */
gchar *hotkey_map_get_accel_label(const hotkey_map_t *map)
{
    if (map->keysym > 0) {
        return gtk_accelerator_get_label(map->keysym, map->modifier);
    }
    return NULL;
}
