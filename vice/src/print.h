
/*
 * print.h - Printer interface.
 *
 * Written by
 *  André Fachat (a.fachat@physik.tu-chemnitz.de)
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

/*
 * The printer emulation captures the bytes sent to device 4 on the
 * IEC bus and/or the bytes sent to an emulated userport interface
 */

#ifndef _PRINT_H
#define _PRINT_H

#include "types.h"

/* initializes all print stuff */
extern void print_init(void);

/* called when doing a system reset */
extern void print_reset(void);

/* opens a print window, returns handle to give to functions below. */
extern int print_open(int device);

/* closes the print window again */
extern void print_close(int fd);

/* sends a byte to the print line */
extern int print_putc(int fd, BYTE b);

/* flushes buffer to printer */
extern int print_flush(int fd);

extern int print_init_resources(void);
extern int print_init_cmdline_options(void);

#endif

