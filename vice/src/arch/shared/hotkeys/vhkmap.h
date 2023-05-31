/** \file   vhkmap.h
 * \brief   Mapping of hotkeys to actions and menu items - header
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

#ifndef VICE_HOTKEYS_VHKMAP_H
#define VICE_HOTKEYS_VHKMAP_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "hotkeystypes.h"

void        vhk_map_init_mappings  (void);

vhk_map_t * vhk_map_set            (int       action,
                                    uint32_t  vice_keysym,
                                    uint32_t  vice_modmask,
                                    uint32_t  arch_keysym,
                                    uint32_t  arch_modmask,
                                    void     *primary_item,
                                    void     *secondary_item);
vhk_map_t * vhk_map_get            (int action);
vhk_map_t * vhk_map_get_by_hotkey  (uint32_t vice_keysym, uint32_t vice_modmask);
vhk_map_t * vhk_map_get_by_arch_hotkey(uint32_t arch_keysym, uint32_t arch_modmask);
void        vhk_map_unset          (int action);
void        vhk_map_unset_by_hotkey(uint32_t vice_keysym, uint32_t vice_modmask);
void        vhk_map_unset_by_map   (vhk_map_t *map);



void        vhk_map_set_hotkey_by_map(vhk_map_t *map,
                                      uint32_t vice_keysym,
                                      uint32_t vice_modmask,
                                      uint32_t arch_keysym,
                                      uint32_t arch_modmask);
#endif
