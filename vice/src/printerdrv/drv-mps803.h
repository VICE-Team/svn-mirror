/*
 * drv-mps803.h - MPS803 printer driver.
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

#ifndef _DRV_MPS803_H
#define _DRV_MPS803_H

#include <stdio.h>

#define MAX_COL 480

struct mps_s
{
    BYTE line[MAX_COL][7];
    int  bitcnt;
    int  repeatn;
    int  pos;
    int  tab;
    BYTE tabc[3];
    int  mode;
};
typedef struct mps_s mps_t;

#define MPS_REVERSE 0x01
#define MPS_CRSRUP  0x02
#define MPS_BITMODE 0x04
#define MPS_DBLWDTH 0x08
#define MPS_REPEAT  0x10
#define MPS_ESC     0x20

/* We will make this dynamic later.  */
extern mps_t drv_mps803[3];

extern int drv_mps803_init_resources(void);
extern void drv_mps803_init(void);
extern int is_mode(mps_t *mps, int m);

#endif

