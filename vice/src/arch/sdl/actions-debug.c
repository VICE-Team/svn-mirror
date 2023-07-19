/** \file   actions-debug.c
 * \brief   UI action implementations for debugging settings (SDL)
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
#include "debug.h"
#ifdef DEBUG

#include "resources.h"
#include "types.h"
#include "uiactions.h"
#include "uimenu.h"

#include "actions-debug.h"


static void autoplayback_frames_action(void *unused)
{
    sdl_ui_menu_item_activate_by_action(ACTION_DEBUG_AUTOPLAYBACK_FRAMES);
}

/** \brief  Toggle resource action
 *
 * \param[in]   resource    resource name
 */
static void resource_toggle_action(void *resource)
{
    const char *name  = resource;
    int         value = 0;

    resources_get_int(name, &value);
    resources_set_int(name, value ? 0 : 1);
}


/** \brief  List of mappings for debugging actions */
static const ui_action_map_t debug_actions[] = {
    {   .action  = ACTION_DEBUG_AUTOPLAYBACK_FRAMES,
        .handler = autoplayback_frames_action,
        .dialog  = true
    },
    {   .action  = ACTION_DEBUG_TRACE_CPU_TOGGLE,
        .handler = resource_toggle_action,
        .param   = "MainCPU_TRACE"
    },
    {   .action  = ACTION_DEBUG_TRACE_IEC_TOGGLE,
        .handler = resource_toggle_action,
        .param   = "IEC_TRACE"
    },
    {   .action  = ACTION_DEBUG_TRACE_IEEE488_TOGGLE,
        .handler = resource_toggle_action,
        .param   = "IEEE_TRACE"
    },
    {   .action  = ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE,
        .handler = resource_toggle_action,
        .param   = "Drive0CPU_TRACE"
    },
    {   .action  = ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE,
        .handler = resource_toggle_action,
        .param   = "Drive1CPU_TRACE"
    },
    {   .action  = ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE,
        .handler = resource_toggle_action,
        .param   = "Drive2CPU_TRACE"
    },
    {   .action  = ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE,
        .handler = resource_toggle_action,
        .param   = "Drive3CPU_TRACE"
    },

    /* DTV-specific actions */
    {   .action  = ACTION_DEBUG_BLITTER_LOG_TOGGLE,
        .handler = resource_toggle_action,
        .param   = "DtvBlitterLog"
    },
    {   .action  = ACTION_DEBUG_DMA_LOG_TOGGLE,
        .handler = resource_toggle_action,
        .param   = "DtvDmaLog"
    },

    {   .action  = ACTION_DEBUG_FLASH_LOG_TOGGLE,
        .handler = resource_toggle_action,
        .param   = "DtvFlashLog"
    },

    UI_ACTION_MAP_TERMINATOR
};


/** \brief  Register debugging actions */
void actions_debug_register(void)
{
    ui_actions_register(debug_actions);
}

#endif
