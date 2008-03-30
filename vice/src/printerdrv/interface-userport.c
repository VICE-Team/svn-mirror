/*
 * interface-userport.c - Userport printer interface.
 *
 * Written by
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <viceteam@t-online.de>
 *
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

#include <stdio.h>

#include "cmdline.h"
#include "driver-select.h"
#include "interface-userport.h"
#include "printer.h"
#include "resources.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"


static int userport_printer_enabled = 0;

static void (*set_busy_func)(unsigned int b) = NULL;


static int set_up_enabled(resource_value_t v, void *param)
{
    int newval;

    newval = ((int)v) ? 1 : 0;

    if (newval && !userport_printer_enabled) {
        /* Switch printer on.  */
        if (driver_select_open(2, 4) >= 0) {
            userport_printer_enabled = 1;
        }
    }
    if (userport_printer_enabled && !newval) {
        driver_select_close(2, 4);
        userport_printer_enabled = 0;
    }

    return 0;
}

static const resource_t resources[] = {
    { "PrinterUserport", RES_INTEGER, (resource_value_t)0,
      (void *)&userport_printer_enabled, set_up_enabled, NULL },
    { NULL }
};

int interface_userport_init_resources(void)
{
    return resources_register(resources);
}

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] = {
    { "-pruser", SET_RESOURCE, 0, NULL, NULL, "PrinterUserport",
      (resource_value_t) 1, 0,
      IDCLS_ENABLE_USERPORT_PRINTER },
    { "+pruser", SET_RESOURCE, 0, NULL, NULL, "PrinterUserport",
      (resource_value_t) 0, 0,
      IDCLS_DISABLE_USERPORT_PRINTER },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] = {
    { "-pruser", SET_RESOURCE, 0, NULL, NULL, "PrinterUserport",
      (resource_value_t) 1, NULL,
      "Enable the userport printer emulation" },
    { "+pruser", SET_RESOURCE, 0, NULL, NULL, "PrinterUserport",
      (resource_value_t) 0, NULL,
      "Disable the userport printer emulation" },
    { NULL }
};
#endif

int interface_userport_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

static BYTE value; /* userport value */
static int strobe;

void interface_userport_write_data(BYTE b)
{
    value = b;
}

void interface_userport_write_strobe(int s)
{
    if (userport_printer_enabled && strobe && !s) {     /* hi->lo on strobe */
        driver_select_putc(2, 4, (BYTE)value);

        if (set_busy_func != NULL) {
            (*set_busy_func)(1); /* signal lo->hi */
            (*set_busy_func)(0); /* signal hi->lo */
        }
    }

    strobe = s;
}

void interface_userport_init(void (*set_busy)(unsigned int))
{
    set_busy_func = set_busy;
}

