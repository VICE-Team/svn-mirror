/*
 * prdevice.h - Printer device.
 *
 * Written by
 *  André Fachat        (a.fachat@physik.tu-chemnitz.de)
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

#ifndef _PRDEVICE_H_
#define _PRDEVICE_H_

typedef int PRINTER;

extern int prdevice_init_resources(void);
extern int prdevice_init_cmdline_options(void);

extern int detach_prdevice(PRINTER *info);
extern int attach_prdevice(PRINTER *info, const char *file, int mode);

/* this is needed because not all CLOSE are sent to the IEC bus... :-( */
extern int prdevice_close_printer(int device);

/* this is called at the end of serial_init to enable possibly 
   attached printers */
extern void prdevice_late_init(void);

#endif

