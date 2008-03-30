/*
 * printer.c - Common printer interface.
 *
 * Written by
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

#include "driver-select.h"
#include "drv-ascii.h"
#include "drv-mps803.h"
#include "interface-serial.h"
#include "interface-userport.h"
#include "output-file.h"

int printer_init_resources(void)
{
    if (output_file_init_resources() < 0
        || drv_ascii_init_resources() < 0
        || drv_mps803_init_resources() < 0
        || driver_select_init_resources() < 0
        || interface_serial_init_resources() < 0
        || interface_userport_init_resources() < 0)
        return -1;
    return 0;
}

int printer_init_cmdline_options(void)
{
    if (output_file_init_cmdline_options() < 0
        || driver_select_init_cmdline_options() < 0
        || interface_serial_init_cmdline_options() < 0
        || interface_userport_init_cmdline_options() < 0)
        return -1;
    return 0;
}

void printer_init(void)
{
    output_file_init();
    drv_ascii_init();
    drv_mps803_init();
    driver_select_init();
    interface_serial_init();
}

void printer_reset(void)
{
    output_file_reset();
}

