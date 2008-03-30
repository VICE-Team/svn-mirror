/*
 * wd1770.h - WD1770 emulation for the 1571 and 1581 disk drives.
 *
 * Written by
 *  Andreas Boose (boose@linux.rz.fh-hannover.de)
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

#ifndef _WD1770_H
#define _WD1770_H

#include "types.h"

/* wd1770 register.  */
#define WD1770_STATUS  0
#define WD1770_COMMAND 0
#define WD1770_TRACK   1
#define WD1770_SECTOR  2
#define WD1770_DATA    3

void REGPARM2 store_wd1770d0(ADDRESS addr, BYTE byte);
BYTE REGPARM1 read_wd1770d0(ADDRESS addr);
void reset_wd1770d0(void);

void REGPARM2 store_wd1770d1(ADDRESS addr, BYTE byte);
BYTE REGPARM1 read_wd1770d1(ADDRESS addr);
void reset_wd1770d1(void);

#endif                          /* _WD1770_H */

