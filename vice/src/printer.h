/*
 * printer.h - Common external printer interface.
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

#ifndef _PRINTER_H
#define _PRINTER_H

#include "types.h"

/* Generic interface.  */
extern int printer_init_resources(void);
extern int printer_init_cmdline_options(void);
extern void printer_init(void);
extern void printer_reset(void);

/* Serial interface.  */
extern int printer_interface_serial_close(unsigned int unit);
extern void printer_interface_serial_late_init(void);

/* Userport interface.  */
extern void printer_interface_userport_write_data(BYTE b);
extern void printer_interface_userport_write_strobe(int s);
extern void printer_interface_userport_set_busy(int);

#endif

