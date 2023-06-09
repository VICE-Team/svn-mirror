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
