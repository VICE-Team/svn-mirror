/*
 *
 * ieeevia1.c - IEEE488 interface VIA1 emulation in the VIC-1112.
 *
 * Written by
 *   André Fachat <a.fachat@physik.tu-chemnitz.de>
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

#define mycpu maincpu
#define myclk clk
#define myvia ieeevia1
#define myvia_init ieeevia1_init

#define	via_set_int maincpu_set_irq
#define I_MYVIAFL I_IEEEVIA1FL
#define MYVIA_INT IK_IRQ
#define	MYVIA_NAME "IeeeVia1"

#define mycpu_rmw_flag rmw_flag
#define mycpu_int_status maincpu_int_status
#define mycpu_alarm_context maincpu_alarm_context
#define mycpu_clk_guard maincpu_clk_guard

#define myvia_reset ieeevia1_reset
#define myvia_store ieeevia1_store
#define myvia_read ieeevia1_read
#define myvia_peek ieeevia1_peek

#define myvia_log ieeevia1_log
#define myvia_signal ieeevia1_signal
#define myvia_prevent_clk_overflow ieeevia1_prevent_clk_overflow
#define myvia_read_snapshot_module ieeevia1_read_snapshot_module
#define myvia_write_snapshot_module ieeevia1_write_snapshot_module

#include "vice.h"
#include "viacore.h"

#include "maincpu.h"
#include "parallel.h"
#include "drive.h"

#define VIA_SET_CA2(a)
#define VIA_SET_CB2(a)


/* #define VIA1_TIMER_DEBUG */

static char snap_module_name[] = MYVIA_NAME;

static void undump_acr(BYTE byte)
{
}

inline void static store_acr(BYTE byte)
{
}

inline void static store_sr(BYTE byte)
{
}

inline void static store_t2l(BYTE byte)
{
}

static void undump_pra(BYTE byte)
{
}

inline static void store_pra(BYTE byte, BYTE oldpa, ADDRESS addr)
{
}

static void undump_prb(BYTE byte)
{
    parallel_cpu_set_dav(!(byte & 0x01));
    parallel_cpu_set_nrfd(!(byte & 0x02));
    parallel_cpu_set_ndac(!(byte & 0x04));
}

inline static void store_prb(BYTE byte, BYTE oldpb, ADDRESS addr)
{
    parallel_cpu_set_dav(!(byte & 0x01));
    parallel_cpu_set_nrfd(!(byte & 0x02));
    parallel_cpu_set_ndac(!(byte & 0x04));
}

static void undump_pcr(BYTE byte)
{
}

static void res_via(void)
{
    parallel_cpu_set_dav(0);
    parallel_cpu_set_nrfd(0);
    parallel_cpu_set_ndac(0);
}

inline static BYTE store_pcr(BYTE byte, ADDRESS addr)
{
    return byte;
}

inline static BYTE read_pra(ADDRESS addr)
{
    return 0xff;
}

inline static BYTE read_prb(void)
{
    BYTE byte;

    if (drive[0].enable)
        drive0_cpu_execute(clk);
    if (drive[1].enable)
        drive1_cpu_execute(clk);

    byte = 255
	- (parallel_atn  ? 0x80 : 0)
	- (parallel_ndac ? 0x40 : 0)
	- (parallel_nrfd ? 0x20 : 0)
	- (parallel_dav  ? 0x10 : 0)
	- (parallel_eoi  ? 0x08 : 0);

    /* none of the load changes output register value -> std. masking */
    byte = ((byte & ~myvia[VIA_DDRB]) | (myvia[VIA_PRB] & myvia[VIA_DDRB]));
    return byte;
}

#include "viacore.c"

