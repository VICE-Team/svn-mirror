/** \file   actions-vsid.c
 * \brief   UI action implementations for VSID
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

#include "resources.h"
#include "uiactions.h"
#include "uisidattach.h"

#include "actions-vsid.h"


/** \brief  Show PSID load dialog */
static void psid_load_action(void)
{
    /* FIXME: This triggers massive linker errors =) */
/*    uisidattach_show_dialog(); */
}


/** \brief  List of VSID-specific actions */
static const ui_action_map_t vsid_actions[] = {
    {
        .action = ACTION_PSID_LOAD,
        .handler = psid_load_action,
        .blocks = true,
        .dialog = true
    },

    UI_ACTION_MAP_TERMINATOR
};


/** \brief  Register VSID-specific actions */
void actions_vsid_register(void)
{
    ui_actions_register(vsid_actions);
}
