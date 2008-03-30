/*
 * riot2drive0.c - RIOT1 emulation in the SFD1001, 8050 and 8250 disk drive.
 *
 * Written by
 *  Andre' Fachat (fachat@physik.tu-chemnitz.de)
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

#include "riotcore.h"

#undef RIOT_TIMER_DEBUG

/*************************************************************************
 * Renaming exported functions
 */

#define myriot_init riot2d0_init
#define myriot_signal riot2d0_signal
#define myriot_reset riot2d0_reset
#define store_myriot store_riot2d0
#define read_myriot read_riot2d0
#define peek_myriot peek_riot2d0
#define myriot_set_flag riot2d0_set_flag
#define myriot_write_snapshot_module riot2d0_write_snapshot_module
#define myriot_read_snapshot_module riot2d0_read_snapshot_module

#define MYRIOT_NAME "RIOT2D0"

/*************************************************************************
 * CPU binding
 */

#include "interrupt.h"

#define myclk           drive_clk[0]
#define mycpu_clk_guard drive0_clk_guard
#define mycpu_rmw_flag  drive0_rmw_flag
#define mycpu_alarm_context drive0_alarm_context

/*
#define my_set_irq(fl, clk)	\
	do { \
	printf("set_int_d0(%d)\n",(fl)); \
        set_int(&drive0_int_status,I_RIOTD0FL,(fl) ? IK_IRQ : 0, (clk)) \
	; } while(0)
*/
#define my_set_irq(fl, clk)	\
        set_int(&drive0_int_status,I_RIOTD0FL,(fl) ? IK_IRQ : 0, (clk))

#define my_restore_irq(fl)	\
        set_int_noclk(&drive0_int_status,I_RIOTD0FL,(fl) ? IK_IRQ : 0)

/*************************************************************************
 * I/O
 */

#ifdef STDC_HEADERS
#include <stdio.h>
#include <time.h>
#endif

#include "drive.h"
#include "drivecpu.h"
#include "iecdrive.h"
#include "riotd.h"
#include "parallel.h"
#include "riot.h"

static int atn_active = 0;

_RIOT_FUNC void set_handshake(BYTE pa)
{
    parallel_drv0_set_nrfd(
	((pa & 0x4)==0) 
	|| ((pa & 1) && !atn_active)
	|| (((pa & 1)==0) && atn_active)
	);
    parallel_drv0_set_ndac(
	(pa & 0x2) 
	|| (((pa & 0x1)==0) && atn_active)
	);
}
 
void drive0_riot_set_atn(int state) 
{
    if (drive[0].type == DRIVE_TYPE_1001) {
	if (atn_active && !state) {
	    riot2d0_signal(RIOT_SIG_PA7, RIOT_SIG_FALL);
	} else
	if (state && !atn_active) {
	    riot2d0_signal(RIOT_SIG_PA7, RIOT_SIG_RISE);
	}
        atn_active = state;
	riot1d0_set_pardata();
	set_handshake(oldpa);
    }
}

_RIOT_FUNC void undump_pra(BYTE byte)
{
    /* bit 0 = atna */
    set_handshake(byte);
    parallel_drv0_set_eoi(!(byte & 0x08));
    parallel_drv0_set_dav(!(byte & 0x10));
}

_RIOT_FUNC void store_pra(BYTE byte)
{
    /* bit 0 = atna */
    set_handshake(byte);
    parallel_drv0_set_eoi(!(byte & 0x08));
    parallel_drv0_set_dav(!(byte & 0x10));
}

_RIOT_FUNC void undump_prb(BYTE byte)
{
    /* bit 3 Act LED 1 */
    /* bit 4 Act LED 0 */
    /* bit 5 Error LED */

    /* 1001 only needs LED 0 and Error LED */
    drive[0].led_status = (byte >> 4) & 0x03;
}

_RIOT_FUNC void store_prb(BYTE byte)
{
    /* bit 3 Act LED 1 */
    /* bit 4 Act LED 0 */
    /* bit 5 Error LED */

    /* 1001 only needs LED 0 and Error LED */
    drive[0].led_status = (byte >> 4) & 0x03;
}

_RIOT_FUNC void riot_reset(void)
{
    atn_active = 0;

    parallel_drv0_set_atn(0);
    parallel_drv0_set_dav(0);
    parallel_drv0_set_eoi(0);

    set_handshake(oldpa);

    /* 1001 only needs LED 0 and Error LED */
    drive[0].led_status = 3;
}

_RIOT_FUNC BYTE read_pra(void)
{
    BYTE byte = 0xff;
    if (!parallel_atn) byte -= 0x80;
    if (parallel_dav) byte -= 0x40;
    if (parallel_eoi) byte -= 0x20;
    return (byte & ~riotio[1]) | (riotio[0] & riotio[1]);
}

_RIOT_FUNC BYTE read_prb(void)
{
    BYTE byte = 0xff;
    if (parallel_nrfd) byte -= 0x80;
    if (parallel_ndac) byte -= 0x40;

    byte -= 1;		/* device address bit 0 */
    byte -= 2;		/* device address bit 1 */
    byte -= 4;		/* device address bit 2 */

    return (byte & ~riotio[3]) | (riotio[2] & riotio[3]);
}

#include "riotcore.c"

