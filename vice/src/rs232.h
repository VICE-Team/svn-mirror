/*
 * rs232.h - RS232 emulation.
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
 * This is the header for the RS232 emulation.
 *
 * The RS232 emulation captures the bytes sent to the RS232 interfaces
 * available (currently only ACIA 6551, later UART 16550A, std C64,
 * and Daniel Dallmanns fast RS232 with 9600 Baud).
 * The characters captured are displayed in a special terminal window.
 * Characters typed in the terminal window are sent back to the
 * chip emulations.
 */

#ifndef _RS232_H
#define _RS232_H

#include "types.h"

/* initializes all RS232 stuff */
extern void rs232_reset(void);

/* opens a rs232 window, returns handle to give to functions below. */
extern int rs232_open(int device);

/* closes the rs232 window again */
extern void rs232_close(int fd);

/* sends a byte to the RS232 line */
extern int rs232_putc(int fd, BYTE b);

/* gets a byte to the RS232 line, returns !=1 if byte received, byte in *b. */
extern int rs232_getc(int fd, BYTE *b);

extern int rs232_init_resources(void);
extern int rs232_init_cmdline_options(void);

#endif
