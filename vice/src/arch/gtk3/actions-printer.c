/** \file   actions-printer.c
 * \brief   UI action implementations for printers
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

#include "debug_gtk3.h"
#include "printer.h"
#include "uiactions.h"

#include "actions-printer.h"


/** \brief  Send formfeed to printer #4 */
static void printer_formfeed_4_action(void)
{
    debug_gtk3("sending formfeed to printer #4.");
    printer_formfeed(4);
}

/** \brief  Send formfeed to printer #5 */
static void printer_formfeed_5_action(void)
{
    debug_gtk3("sending formfeed to printer #5.");
    printer_formfeed(5);
}

/** \brief  Send formfeed to plotter #6 */
static void printer_formfeed_6_action(void)
{
    debug_gtk3("sending formfeed to plotter #6.");
    printer_formfeed(6);
}

/** \brief  Send formfeed to userport printer */
static void printer_formfeed_userport_action(void)
{
    debug_gtk3("sending formfeed to userport printer.");
    printer_formfeed(3);
}

/** \brief  Printer actions */
static const ui_action_map_t printer_actions[] = {
    {
        .action  = ACTION_PRINTER_FORMFEED_4,
        .handler = printer_formfeed_4_action
    },
    {
        .action  = ACTION_PRINTER_FORMFEED_5,
        .handler = printer_formfeed_5_action
    },
    {
        .action  = ACTION_PRINTER_FORMFEED_6,
        .handler = printer_formfeed_6_action
    },
    {
        .action  = ACTION_PRINTER_FORMFEED_USERPORT,
        .handler = printer_formfeed_userport_action
    },

    UI_ACTION_MAP_TERMINATOR
};


/** \brief  Register printer-related UI action handlers
 */
void actions_printer_register(void)
{
    ui_actions_register(printer_actions);
}
