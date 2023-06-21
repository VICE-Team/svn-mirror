/** \file   actions-debug.c
 * \brief   UI action implementations for debug-related dialogs and settings
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

/* Resources altered by this file:
 *
 * $VICERES MainCPU_TRACE   all
 * $VICERES IEC_TRACE       -vsid
 * $VICERES IEEE_TRACE      -vsid
 * $VICERES Drive0CPU_TRACE -vsid
 * $VICERES Drive1CPU_TRACE -vsid
 * $VICERES Drive2CPU_TRACE -vsid
 * $VICERES Drive3CPU_TRACE -vsid
 * $VICERES DoCoreDump      -vsid
 * $VICERES DtvBlitterLog   x64dtv
 * $VICERES DtvDMALog       x64dtv
 * $VICERES DtvFlashLog     x64dtv
 */

#include "vice.h"
#include "debug.h"

#ifdef DEBUG

#include <gtk/gtk.h>
#include <stddef.h>
#include <stdbool.h>

#include "hotkeys.h"
#include "machine.h"
#include "resources.h"
#include "uiactions.h"
#include "uidebug.h"
#include "uimenu.h"

#include "actions-debug.h"


/** \brief  Object mapping action IDs to resource names
 */
typedef struct action_resource_s {
    int action;
    const char *resource;
} action_resource_t;



/** \brief  Toggle boolean resource and update its check menu item
 *
 * \param[in]   resource    resource name
 * \param[in]   action      UI action ID
 */
static void toggle_resource_and_update_menu(const char *resource, int action)
{
    int enabled;

    resources_get_int(resource, &enabled);
    enabled = !enabled;
    resources_set_int(resource, enabled);
    vhk_gtk_set_check_item_blocked_by_action(action, enabled);
}

/** \brief  Pop up dialog to change trace mode */
static void debug_trace_mode_action(void *unused)
{
    ui_debug_trace_mode_dialog_show();
}

/** \brief  Toggle CPU trace */
static void debug_trace_cpu_toggle_action(void *unused)
{
    toggle_resource_and_update_menu("MainCPU_TRACE",
                                    ACTION_DEBUG_TRACE_CPU_TOGGLE);
}

/** \brief  Toggle IEC bus trace */
static void debug_trace_iec_toggle_action(void *unused)
{
    toggle_resource_and_update_menu("IEC_TRACE",
                                    ACTION_DEBUG_TRACE_IEC_TOGGLE);
}

/** \brief  Toggle IEEE-488 bus trace */
static void debug_trace_ieee488_toggle_action(void *unused)
{
    toggle_resource_and_update_menu("IEEE_TRACE",
                                    ACTION_DEBUG_TRACE_IEEE488_TOGGLE);
}

/** \brief  Pop up dialog to change number of auto-playback frames */
static void debug_autoplayback_frames_action(void *unused)
{
    ui_debug_playback_frames_dialog_show();
}

/** \brief  Toggle Drive 8 CPU trace */
static void debug_trace_drive_8_toggle_action(void *unused)
{
    toggle_resource_and_update_menu("Drive0CPU_TRACE",
                                    ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE);
}

/** \brief  Toggle Drive 9 CPU trace */
static void debug_trace_drive_9_toggle_action(void *unused)
{
    toggle_resource_and_update_menu("Drive1CPU_TRACE",
                                    ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE);
}

/** \brief  Toggle Drive 10 CPU trace */
static void debug_trace_drive_10_toggle_action(void *unused)
{
    toggle_resource_and_update_menu("Drive2CPU_TRACE",
                                    ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE);
}

/** \brief  Toggle Drive 11 CPU trace */
static void debug_trace_drive_11_toggle_action(void *unused)
{
    toggle_resource_and_update_menu("Drive3CPU_TRACE",
                                    ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE);
}

/** \brief  Toggle core dump */
static void debug_core_dump_toggle_action(void *unused)
{
    toggle_resource_and_update_menu("DoCoreDump",
                                    ACTION_DEBUG_CORE_DUMP_TOGGLE);
}

/** \brief  Toggle DTV blitter log */
static void debug_blitter_log_toggle_action(void *unused)
{
    toggle_resource_and_update_menu("DtvBlitterLog",
                                    ACTION_DEBUG_BLITTER_LOG_TOGGLE);
}

/** \brief  Toggle DTV DMA log */
static void debug_dma_log_toggle_action(void *unused)
{
    toggle_resource_and_update_menu("DtvDMALog",
                                    ACTION_DEBUG_DMA_LOG_TOGGLE);
}

/** \brief  Toggle DTV flash log */
static void debug_flash_log_toggle_action(void *unused)
{
    toggle_resource_and_update_menu("DtvFlashLog",
                                    ACTION_DEBUG_FLASH_LOG_TOGGLE);
}


/** \brief  List of debugging-related actions */
static const ui_action_map_t debug_actions[] = {
    {
        .action  = ACTION_DEBUG_TRACE_MODE,
        .handler = debug_trace_mode_action,
        .blocks  = true,
        .dialog  = true
    },
    {
        .action   = ACTION_DEBUG_TRACE_CPU_TOGGLE,
        .handler  = debug_trace_cpu_toggle_action,
        .uithread = true
    },
    {
        .action   = ACTION_DEBUG_TRACE_IEC_TOGGLE,
        .handler  = debug_trace_iec_toggle_action,
        .uithread = true
    },
    {
        .action   = ACTION_DEBUG_TRACE_IEEE488_TOGGLE,
        .handler  = debug_trace_ieee488_toggle_action,
        .uithread = true
    },
    {
        .action   = ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE,
        .handler  = debug_trace_drive_8_toggle_action,
        .uithread = true
    },
    {
        .action   = ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE,
        .handler  = debug_trace_drive_9_toggle_action,
        .uithread = true
    },
    {
        .action   = ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE,
        .handler  = debug_trace_drive_10_toggle_action,
        .uithread = true
    },
    {
        .action   = ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE,
        .handler  = debug_trace_drive_11_toggle_action,
        .uithread = true
    },
    {
        .action  = ACTION_DEBUG_AUTOPLAYBACK_FRAMES,
        .handler = debug_autoplayback_frames_action,
        .blocks  = true,
        .dialog  = true
    },
    {
        .action   = ACTION_DEBUG_CORE_DUMP_TOGGLE,
        .handler  = debug_core_dump_toggle_action,
        .uithread = true
    },

    UI_ACTION_MAP_TERMINATOR
};


/** \brief  List of debugging-related actions (C64DTV only) */
static const ui_action_map_t debug_actions_dtv[] = {
    /* DTV-specific actions */
    {
        .action = ACTION_DEBUG_BLITTER_LOG_TOGGLE,
        .handler = debug_blitter_log_toggle_action
    },
    {
        .action = ACTION_DEBUG_DMA_LOG_TOGGLE,
        .handler = debug_dma_log_toggle_action
    },
    {
        .action = ACTION_DEBUG_FLASH_LOG_TOGGLE,
        .handler = debug_flash_log_toggle_action
    },

    UI_ACTION_MAP_TERMINATOR
};

static const ui_action_map_t debug_actions_vsid[] = {
    {
        .action = ACTION_DEBUG_TRACE_MODE,
        .handler = debug_trace_mode_action,
        .blocks = true,
        .dialog = true
    },
    {
        .action = ACTION_DEBUG_TRACE_CPU_TOGGLE,
        .handler = debug_trace_cpu_toggle_action,
        .uithread = true
    },
    {
        .action = ACTION_DEBUG_AUTOPLAYBACK_FRAMES,
        .handler = debug_autoplayback_frames_action,
        .blocks = true,
        .dialog = true
    },
    {
        .action = ACTION_DEBUG_CORE_DUMP_TOGGLE,
        .handler = debug_core_dump_toggle_action,
        .uithread = true
    },

    UI_ACTION_MAP_TERMINATOR
};



/** \brief  Register debugging actions */
void actions_debug_register(void)
{
    if (machine_class == VICE_MACHINE_VSID) {
        /* VSID */
        ui_actions_register(debug_actions_vsid);
    } else {
        /* non-VSID */
        ui_actions_register(debug_actions);
        /* C64DTV-specific */
        if (machine_class == VICE_MACHINE_C64DTV) {
            ui_actions_register(debug_actions_dtv);
        }
    }
}


/** \brief  List of actions and resources to set up check buttons
 */
static const action_resource_t actions_list[] = {
    { ACTION_DEBUG_TRACE_CPU_TOGGLE,        "MainCPU_TRACE" },
    { ACTION_DEBUG_TRACE_IEC_TOGGLE,        "IEC_TRACE" },
    { ACTION_DEBUG_TRACE_IEEE488_TOGGLE,    "IEEE_TRACE" },
    { ACTION_DEBUG_TRACE_DRIVE_8_TOGGLE,    "Drive0CPU_TRACE" },
    { ACTION_DEBUG_TRACE_DRIVE_9_TOGGLE,    "Drive1CPU_TRACE" },
    { ACTION_DEBUG_TRACE_DRIVE_10_TOGGLE,   "Drive2CPU_TRACE" },
    { ACTION_DEBUG_TRACE_DRIVE_11_TOGGLE,   "Drive3CPU_TRACE" },
    { ACTION_DEBUG_CORE_DUMP_TOGGLE,        "DoCoreDump" },
    { ACTION_DEBUG_BLITTER_LOG_TOGGLE,      "DtvBlitterLog" },
    { ACTION_DEBUG_DMA_LOG_TOGGLE,          "DtvDMALog" },
    { ACTION_DEBUG_FLASH_LOG_TOGGLE,        "DtvFlashLog" }
};


/** \brief  Set debug-related check menu items
 */
void actions_debug_setup_ui(void)
{
    size_t i;

    for (i = 0; i < sizeof actions_list / sizeof actions_list[0]; i++) {
        if (ui_action_is_valid(actions_list[i].action)) {
            int enabled;

            resources_get_int(actions_list[i].resource, &enabled);
            vhk_gtk_set_check_item_blocked_by_action(actions_list[i].action,
                                                     (gboolean)enabled);
        }
    }
}

#endif
