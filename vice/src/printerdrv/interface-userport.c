/*
 * interface-userport.c - Userport printer interface.
 *
 * Written by
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include "types.h"

static BYTE value;              /* userport value */
static int strobe;
static int fd;

/***********************************************************************
 * resource handling
 */

static int userport_printer_enabled = 0;
static int userport_printer_device;

static int set_up_enabled(resource_value_t v, void *param)
{
    int newval = ((int)v) ? 1 : 0;

    if (newval && !userport_printer_enabled) {
        /* Switch printer on.  */
        fd = driver_select_open(userport_printer_device);
        if (fd >= 0) {
            userport_printer_enabled = 1;
        }
    }
    if (userport_printer_enabled && !newval) {
        driver_select_close(fd);
        userport_printer_enabled = 0;
    }

    return 0;
}

static int set_up_device(resource_value_t v, void *param)
{
    userport_printer_device = (int)v;
    return 0;
}

static resource_t resources[] = {
    { "PrUser", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&userport_printer_enabled,
      set_up_enabled, NULL },
    { "PrUserDev", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&userport_printer_device,
      set_up_device, NULL },
    { NULL }
};

int interface_userport_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-pruser", SET_RESOURCE, 0, NULL, NULL, "PrUser",
      (resource_value_t) 1, NULL,
      "Enable the userport printer emulation" },
    { "+pruser", SET_RESOURCE, 0, NULL, NULL, "PrUser",
      (resource_value_t) 0, NULL,
      "Disable the userport printer emulation" },
    { "-pruserdev", SET_RESOURCE, 1, NULL, NULL, "PrUserDevice",
      (resource_value_t) 0,
      "<0-2>", "Specify VICE printer device for userport" },
    { NULL }
};

int interface_userport_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/*********************************************************************/


void printer_interface_userport_write_data(BYTE b)
{
    value = b;
}

void printer_interface_userport_write_strobe(int s)
{
    if (userport_printer_enabled && strobe && !s) {     /* hi->lo on strobe */
        driver_select_putc(fd, (BYTE)value);

        printer_interface_userport_set_busy(1); /* signal lo->hi */
        printer_interface_userport_set_busy(0); /* signal hi->lo */
    }

    strobe = s;
}

