/** \file   actions-datasette.c
 * \brief   UI action implementations for datasette-related dialogs and settings
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

#include "datasette.h"
#include "tape.h"
#include "uiactions.h"
#include "uitapeattach.h"
#include "uitapecreate.h"

#include "actions-datasette.h"


/* XXX: The datasette functions use 0/1 for "port", while the UI code uses
 *      1/2 for the port!
 */


/* {{{ Datasette #1 actions */
static void tape_attach_1_action(void)
{
    ui_tape_attach_show_dialog(1);
}

static void tape_create_1_action(void)
{
    ui_tape_create_dialog_show(1);
}

static void tape_detach_1_action(void)
{
    tape_image_detach(1);
}

static void tape_stop_1_action(void)
{
    datasette_control(0, DATASETTE_CONTROL_STOP);
}

static void tape_play_1_action(void)
{
    datasette_control(0, DATASETTE_CONTROL_START);
}

static void tape_ffwd_1_action(void)
{
    datasette_control(0, DATASETTE_CONTROL_FORWARD);
}

static void tape_rewind_1_action(void)
{
    datasette_control(0, DATASETTE_CONTROL_REWIND);
}

static void tape_record_1_action(void)
{
    datasette_control(0, DATASETTE_CONTROL_RECORD);
}

static void tape_reset_1_action(void)
{
    datasette_control(0, DATASETTE_CONTROL_RESET);
}

static void tape_reset_counter_1_action(void)
{
    datasette_control(0, DATASETTE_CONTROL_RESET_COUNTER);
}
/* }}} */

/* {{{ Datasette #2 actions */
static void tape_attach_2_action(void)
{
    ui_tape_attach_show_dialog(2);
}

static void tape_create_2_action(void)
{
    ui_tape_create_dialog_show(2);
}

static void tape_detach_2_action(void)
{
    tape_image_detach(2);
}

static void tape_stop_2_action(void)
{
    datasette_control(1, DATASETTE_CONTROL_STOP);
}

static void tape_play_2_action(void)
{
    datasette_control(1, DATASETTE_CONTROL_START);
}

static void tape_ffwd_2_action(void)
{
    datasette_control(1, DATASETTE_CONTROL_FORWARD);
}

static void tape_rewind_2_action(void)
{
    datasette_control(1, DATASETTE_CONTROL_REWIND);
}

static void tape_record_2_action(void)
{
    datasette_control(1, DATASETTE_CONTROL_RECORD);
}

static void tape_reset_2_action(void)
{
    datasette_control(1, DATASETTE_CONTROL_RESET);
}

static void tape_reset_counter_2_action(void)
{
    datasette_control(1, DATASETTE_CONTROL_RESET_COUNTER);
}
/* }}} */


/** \brief  List of actions for datasettes */
static const ui_action_map_t datasette_actions[] = {
    /* Datasette #1 image actions */
    {
        .action = ACTION_TAPE_ATTACH_1,
        .handler = tape_attach_1_action,
        .blocks = true,
        .dialog = true
    },
    {
        .action = ACTION_TAPE_CREATE_1,
        .handler = tape_create_1_action,
        .blocks = true,
        .dialog = true
    },
    {
        .action = ACTION_TAPE_DETACH_1,
        .handler = tape_detach_1_action
    },

    /* Datasette #1 command actions */
    {
        .action = ACTION_TAPE_STOP_1,
        .handler = tape_stop_1_action
    },
    {
        .action = ACTION_TAPE_PLAY_1,
        .handler = tape_play_1_action
    },
    {
        .action = ACTION_TAPE_FFWD_1,
        .handler = tape_ffwd_1_action
    },
    {
        .action = ACTION_TAPE_REWIND_1,
        .handler = tape_rewind_1_action
    },
    {
        .action = ACTION_TAPE_RECORD_1,
        .handler = tape_record_1_action
    },
    {
        .action = ACTION_TAPE_RESET_1,
        .handler = tape_reset_1_action
    },
    {
        .action = ACTION_TAPE_RESET_COUNTER_1,
        .handler = tape_reset_counter_1_action
    },

    /* Datasette #2 image actions */
    {
        .action = ACTION_TAPE_ATTACH_2,
        .handler = tape_attach_2_action,
        .blocks = true,
        .dialog = true
    },
    {
        .action = ACTION_TAPE_CREATE_2,
        .handler = tape_create_2_action,
        .blocks = true,
        .dialog = true
    },
    {
        .action = ACTION_TAPE_DETACH_2,
        .handler = tape_detach_2_action
    },

    /* Datasette #2 command actions */
    {
        .action = ACTION_TAPE_STOP_2,
        .handler = tape_stop_2_action
    },
    {
        .action = ACTION_TAPE_PLAY_2,
        .handler = tape_play_2_action
    },
    {
        .action = ACTION_TAPE_FFWD_2,
        .handler = tape_ffwd_2_action
    },
    {
        .action = ACTION_TAPE_REWIND_2,
        .handler = tape_rewind_2_action
    },
    {
        .action = ACTION_TAPE_RECORD_2,
        .handler = tape_record_2_action
    },
    {
        .action = ACTION_TAPE_RESET_2,
        .handler = tape_reset_2_action
    },
    {
        .action = ACTION_TAPE_RESET_COUNTER_2,
        .handler = tape_reset_counter_2_action
    },
    UI_ACTION_MAP_TERMINATOR
};


/** \brief  Register datasette-related actions */
void actions_datasette_register(void)
{
    ui_actions_register(datasette_actions);
}
