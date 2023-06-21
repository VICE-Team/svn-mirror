/** \file   actions-snapshot.c
 * \brief   UI action implementations for snapshots and event recording
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
 */

#include "vice.h"

#include <gtk/gtk.h>
#include <stddef.h>
#include <stdbool.h>

#include "uiactions.h"
#include "uiapi.h"
#include "uisnapshot.h"
#include "vice-event.h"

#include "actions-snapshot.h"


/* {{{ Snapshit actions */
/** \brief  Pop up dialog to load a snapshot */
static void snapshot_load_action(void *unused)
{
    ui_snapshot_load_snapshot();
}

/** \brief  Pop up dialog to save a snapshot */
static void snapshot_save_action(void *unused)
{
    ui_snapshot_save_snapshot();
}

/** \brief  Quickload snapshot */
static void snapshot_quickload_action(void *unused)
{
    ui_snapshot_quickload_snapshot();
}

/** \brief  Quicksave snapshot */
static void snapshot_quicksave_action(void *unused)
{
    ui_snapshot_quicksave_snapshot();
}
/* }}} */

/* {{{ History actions */
/** \brief  Start recording history */
static void history_record_start_action(void *unused)
{
    event_record_start();
    ui_display_recording(1);

}

/** \brief  Stop recording history */
static void history_record_stop_action(void *unused)
{
    event_record_stop();
    ui_display_recording(0);
}


/** \brief  Start history playback */
static void history_playback_start_action(void *unused)
{
    event_playback_start();
}

/** \brief  Stop history playback */
static void history_playback_stop_action(void *unused)
{
    event_playback_stop();
}

/** \brief  Set history milestone */
static void history_milestone_set_action(void *unused)
{
    event_record_set_milestone();
}

/** \brief  Rewind to history milestone */
static void history_milestone_reset_action(void *unused)
{
    event_record_reset_milestone();
}
/* }}} */


/** \brief  List of snapshot and event-related actions */
static const ui_action_map_t snapshot_actions[] = {
    /* Snapshot actions */
    {
        .action  = ACTION_SNAPSHOT_LOAD,
        .handler = snapshot_load_action,
        .blocks  = true,
        .dialog  = true
    },
    {
        .action  = ACTION_SNAPSHOT_SAVE,
        .handler = snapshot_save_action,
        .blocks  = true,
        .dialog  = true
    },
    {
        .action  = ACTION_SNAPSHOT_QUICKLOAD,
        .handler = snapshot_quickload_action
    },
    {
        .action  = ACTION_SNAPSHOT_QUICKSAVE,
        .handler = snapshot_quicksave_action
    },

    /* History actions */
    {
        .action   = ACTION_HISTORY_RECORD_START,
        .handler  = history_record_start_action,
        .uithread = true
    },
    {
        .action   = ACTION_HISTORY_RECORD_STOP,
        .handler  = history_record_stop_action,
        .uithread = true
    },
    {
        .action   = ACTION_HISTORY_PLAYBACK_START,
        .handler  = history_playback_start_action,
        .uithread = true
    },
    {
        .action   = ACTION_HISTORY_PLAYBACK_STOP,
        .handler  = history_playback_stop_action,
        .uithread = true
    },
    {
        .action  = ACTION_HISTORY_MILESTONE_SET,
        .handler = history_milestone_set_action
    },
    {
        .action  = ACTION_HISTORY_MILESTONE_RESET,
        .handler = history_milestone_reset_action
    },

    UI_ACTION_MAP_TERMINATOR
};


/** \brief  Register snapshot and history actions */
void actions_snapshot_register(void)
{
    ui_actions_register(snapshot_actions);
}
