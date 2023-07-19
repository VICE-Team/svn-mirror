/** \file   actions-tape.c
 * \brief   UI action implementations for tape-related dialogs and settings (SDL)
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

#include "datasette.h"
#include "tape.h"
#include "tapeport.h"
#include "types.h"
#include "uiactions.h"
#include "uimenu.h"

#include "actions-tape.h"


/** \brief  Encode port number and button number as pointer value
 *
 * \param[in]   port    port number
 * \param[in]   button  button number
 *
 * \return  void*
 *
 * \see src/datasette/datasette.h for the values for \a button
 */
#define PORT_BUTTON_TO_PTR(port, button) int_to_void_ptr(((port) << 8) | (button))

/** \brief  Decode port number from pointer value
 *
 * \param[in]   ptr pointer value encoded with PORT_BUTTON_TO_PTR()
 *
 * \return  port number
 */
#define PORT_FROM_PTR(ptr)      (vice_ptr_to_int(ptr) >> 8)

/** \brief  Decode button number from pointer value
 *
 * \param[in]   ptr pointer value encoded with PORT_BUTTON_TO_PTR()
 *
 * \return  button number
 */
#define BUTTON_FROM_PTR(ptr)    (vice_ptr_to_int(ptr) & 0xff)

/** \brief  Unpack \a ptr into assignments to `int port` and `int button`
 *
 * \param[in]   ptr pointer value encodded with PORT_BUTTON_TO_PTR()
 */
#define UNPACK_P_B(ptr) \
    int port   = PORT_FROM_PTR(ptr); \
    int button = BUTTON_FROM_PTR(ptr);


/** \brief  Attach tape dialog action
 *
 * \param[in]   action  UI action ID to look up callback with
 */
static void tape_attach_action(void *action)
{
    sdl_ui_menu_item_activate_by_action(vice_ptr_to_int(action));
}

/** \brief  Detach tape dialog action
 *
 * \param[in]   port    tape port number (1 or 2)
 */
static void tape_detach_action(void *port)
{
    tape_image_detach(vice_ptr_to_int(port));
}

/** \brief  Create tape dialog action
 *
 * \param[in]   unused  unused
 */
static void tape_create_action(void *unused)
{
    sdl_ui_menu_item_activate_by_action(ACTION_TAPE_CREATE_1);
}

/** \brief  Datasette button action
 *
 * Emulate a button push on a datasette.
 *
 * \param[in]   port_button port number and button number (encoded with
 *                          PORT_BUTTON_TO_PTR()
 */
static void tape_control_action(void *port_button)
{
    UNPACK_P_B(port_button);
    datasette_control(port - 1, button);
}


/** \brief  List of mappings for tape-related actions */
static const ui_action_map_t tape_actions[] = {
    /* datasette 1 image operations */
    {   .action  = ACTION_TAPE_ATTACH_1,
        .handler = tape_attach_action,
        .param   = int_to_void_ptr(ACTION_TAPE_ATTACH_1),
        .dialog  = true
    },
    {   .action  = ACTION_TAPE_DETACH_1,
        .handler = tape_detach_action,
        .param   = int_to_void_ptr(ACTION_TAPE_DETACH_1),
    },
    {   .action  = ACTION_TAPE_CREATE_1,
        .handler = tape_create_action,
        .dialog  = true
    },
    /* datasette 1 controls */
    {   .action  = ACTION_TAPE_STOP_1,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(1, DATASETTE_CONTROL_STOP)
    },
    {   .action  = ACTION_TAPE_PLAY_1,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(1, DATASETTE_CONTROL_START)
    },
    {   .action  = ACTION_TAPE_FFWD_1,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(1, DATASETTE_CONTROL_FORWARD)
    },
    {   .action  = ACTION_TAPE_REWIND_1,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(1, DATASETTE_CONTROL_REWIND)
    },
    {   .action  = ACTION_TAPE_RECORD_1,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(1, DATASETTE_CONTROL_RECORD)
    },
    {   .action  = ACTION_TAPE_RESET_1,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(1, DATASETTE_CONTROL_RESET)
    },
    {   .action  = ACTION_TAPE_RESET_COUNTER_1,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(1, DATASETTE_CONTROL_RESET_COUNTER)
    },

    /* datasette 2 image operations */
    {   .action  = ACTION_TAPE_ATTACH_2,
        .handler = tape_attach_action,
        .param   = int_to_void_ptr(ACTION_TAPE_ATTACH_2),
        .dialog  = true
    },
    {   .action  = ACTION_TAPE_DETACH_2,
        .handler = tape_detach_action,
        .param   = int_to_void_ptr(ACTION_TAPE_DETACH_2),
    },
    /* datasette 2 controls */
    {   .action  = ACTION_TAPE_STOP_2,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(2, DATASETTE_CONTROL_STOP)
    },
    {   .action  = ACTION_TAPE_PLAY_2,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(2, DATASETTE_CONTROL_START)
    },
    {   .action  = ACTION_TAPE_FFWD_2,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(2, DATASETTE_CONTROL_FORWARD)
    },
    {   .action  = ACTION_TAPE_REWIND_2,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(2, DATASETTE_CONTROL_REWIND)
    },
    {   .action  = ACTION_TAPE_RECORD_2,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(2, DATASETTE_CONTROL_RECORD)
    },
    {   .action  = ACTION_TAPE_RESET_2,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(2, DATASETTE_CONTROL_RESET)
    },
    {   .action  = ACTION_TAPE_RESET_COUNTER_2,
        .handler = tape_control_action,
        .param   = PORT_BUTTON_TO_PTR(2, DATASETTE_CONTROL_RESET_COUNTER)
    },


    UI_ACTION_MAP_TERMINATOR

};


/** \brief  Register tape actions */
void actions_tape_register(void)
{
    ui_actions_register(tape_actions);
}
