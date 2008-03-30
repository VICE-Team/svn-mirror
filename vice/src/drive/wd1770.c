/*
 * wd1770.c - WD1770 emulation for the 1571 and 1581 disk drives.
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

/* FIXME: wd1770 support is far from being complete.  */

#undef WD1770_DEBUG

#include "vice.h"

#include <stdio.h>

#include "wd1770.h"

static void store_wd1770(ADDRESS addr, BYTE byte, int dnr);
static BYTE read_wd1770(ADDRESS addr, int dnr);
static void reset_wd1770(int dnr);

/* Register of both wd1770 disk controller. */
static BYTE wd1770[4][2];

void REGPARM2 store_wd1770d0(ADDRESS addr, BYTE byte)
{
    store_wd1770(addr & 3, byte, 0);
}

BYTE REGPARM1 read_wd1770d0(ADDRESS addr)
{
    return read_wd1770(addr & 3, 0);
}

void reset_wd1770d0(void)
{
    reset_wd1770(0);
}

void REGPARM2 store_wd1770d1(ADDRESS addr, BYTE byte)
{
    store_wd1770(addr & 3, byte, 1);
}

BYTE REGPARM1 read_wd1770d1(ADDRESS addr)
{
    return read_wd1770(addr & 3, 1);
}

void reset_wd1770d1(void)
{
    reset_wd1770(1);
}

static void store_wd1770(ADDRESS addr, BYTE byte, int dnr)
{
#if WD_DEBUG
    printf("WD COMMAND: %x\n",byte);
#endif
    wd1770[addr][dnr] = byte;
}

static BYTE read_wd1770(ADDRESS addr, int dnr)
{
    BYTE tmp;

    switch (addr) {
      case WD1770_STATUS:
        wd1770[addr][dnr] ^= 1;
        tmp = wd1770[addr][dnr];
        break; 
      default:
        tmp = wd1770[addr][dnr];
        break;
    }
#if WD_DEBUG
    printf("WD READ %x: %x\n",tmp,wd1770[addr][dnr]);
#endif
    return tmp;
}

static void reset_wd1770(int dnr)
{
    int i;

    for (i = 0; i < 4; i++)
        /* FIXME: Just a wild guess.  */
        wd1770[i][dnr] = 0xff;
}

