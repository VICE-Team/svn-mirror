/*
 * riot1drive0.c - RIOT1 emulation in the SFD1001, 8050 and 8250 disk drive.
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

#define myriot_init riot1d0_init
#define myriot_signal riot1d0_signal
#define myriot_reset riot1d0_reset
#define store_myriot store_riot1d0
#define read_myriot read_riot1d0
#define peek_myriot peek_riot1d0
#define myriot_set_flag riot1d0_set_flag
#define myriot_write_snapshot_module riot1d0_write_snapshot_module
#define myriot_read_snapshot_module riot1d0_read_snapshot_module

#define MYRIOT_NAME "RIOT1"

/*************************************************************************
 * CPU binding
 */

#include "interrupt.h"

#define myclk           drive_clk[0]
#define mycpu_clk_guard drive0_clk_guard
#define mycpu_rmw_flag  drive0_rmw_flag
#define mycpu_alarm_context drive0_alarm_context

#define	my_set_irq(fl, clk)	

#define	my_restore_irq(fl)

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

static void undump_pra(BYTE byte)
{
}

inline static void store_pra(BYTE byte)
{
}

static void undump_prb(BYTE byte)
{
    parallel_drv0_set_bus(byte ^ 0xff);
}

inline static void store_prb(BYTE byte)
{
printf("store par data=%02x (atn=%d)\n",byte, parallel_atn);
    /* hack! */
    parallel_drv0_set_bus(parallel_atn ? 0 : byte ^ 0xff);
}

void riot1_set_pardata(void)
{
    store_prb(oldpb);
}

static void riot_reset(void)
{
    store_prb(0xff);
}

inline static BYTE read_pra(void)
{
    return ((parallel_bus ^ 0xff) & ~riotio[1]) | (riotio[0] & riotio[1]);
}

inline static BYTE read_prb(void)
{
    return (0xff & ~riotio[3]) | (riotio[2] & riotio[3]);
}

#include "riotcore.c"

