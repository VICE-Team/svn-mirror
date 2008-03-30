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

/* WD1770 register.  */
#define WD1770_STATUS  0
#define WD1770_COMMAND 0
#define WD1770_TRACK   1
#define WD1770_SECTOR  2
#define WD1770_DATA    3

typedef struct wd1770_s {
    /* WD1770 register.  */
    BYTE reg[4];
    /* Busy bit clock counter.  */
    CLOCK busy_clk;
    /* Clock counter to control motor spinup.  */
    CLOCK motor_spinup_clk;
    /* Current track of the r/w head.  */
    int current_track;
    /* Current disk side.  */
    int side;
    /* Data register buffer.  */
    BYTE data_buffer[512];
    /* Data register buffer index.  */
    int data_buffer_index;
    /* LED delay.  */
    CLOCK led_delay_clk;
    /* Interrupt line.  */
    CLOCK set_drq;
} wd1770_t;

extern void wd1770d0_init(void);
extern void REGPARM2 store_wd1770d0(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_wd1770d0(ADDRESS addr);
extern void reset_wd1770d0(void);

extern void wd1770d1_init(void);
extern void REGPARM2 store_wd1770d1(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_wd1770d1(ADDRESS addr);
extern void reset_wd1770d1(void);

extern void wd1770_handle_job_code(int dnr);
extern void wd1770_vsync_hook(void);

#endif                          /* _WD1770_H */

