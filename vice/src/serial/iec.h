/*
 * iec.h - Common IEC bus emulation.
 *
 * Written by
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

#ifndef _IEC_H
#define _IEC_H

#include "types.h"

extern void iec_init(void);
extern void iec_reset(void);
extern void iec_open(unsigned int device, BYTE secondary);
extern void iec_close(unsigned int device, BYTE secondary);
extern void iec_listentalk(unsigned int device, BYTE secondary);
extern void iec_unlisten(unsigned int device, BYTE secondary);
extern void iec_untalk(unsigned int device, BYTE secondary);
extern void iec_write(unsigned int device, BYTE secondary, BYTE data);
extern BYTE iec_read(unsigned int device, BYTE secondary);

#endif

