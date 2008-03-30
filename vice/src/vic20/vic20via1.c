/*
 *
 * via1.c - VIA1 emulation in the VIC20.
 *
 * Written by
 *   André Fachat (fachat@physik.tu-chemnitz.de)
 * Patches by
 *   Ettore Perazzoli (ettore@comm2000.it)
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
#define myvia via1
#define myvia_init via1_init

#define I_MYVIAFL I_VIA1FL
#define MYVIA_INT VIA1_INT
#define	MYVIA_NAME "Via1"

#define mycpu_rmw_flag rmw_flag
#define mycpu_int_status maincpu_int_status
#define mycpu_alarm_context maincpu_alarm_context
#define mycpu_clk_guard maincpu_clk_guard

#define reset_myvia reset_via1
#define store_myvia store_via1
#define read_myvia read_via1
#define peek_myvia peek_via1

#define myvia_log via1_log
#define myvia_signal via1_signal
#define myvia_prevent_clk_overflow via1_prevent_clk_overflow
#define myvia_read_snapshot_module via1_read_snapshot_module
#define myvia_write_snapshot_module via1_write_snapshot_module

#define int_myviat1 int_via1t1
#define int_myviat2 int_via1t2

#include "vice.h"
#include "viacore.h"

#include "maincpu.h"

#include "drive.h"
#include "kbd.h"
#include "vic20iec.h"
#include "vic20via.h"

#define VIA_SET_CA2(a)
#define VIA_SET_CB2(a)

#define	via_set_int		maincpu_set_irq
#define	VIA1_INT		IK_IRQ

/* #define VIA1_TIMER_DEBUG */

static char snap_module_name[] = "VIA1";

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
}

inline static void store_prb(BYTE byte, BYTE oldpb, ADDRESS addr)
{
}

static void undump_pcr(BYTE byte)
{
}

static void res_via(void)
{
	iec_pcr_write(0x22);
}

inline static BYTE store_pcr(BYTE byte, ADDRESS addr)
{
    /* FIXME: this should use VIA_SET_CA2() and VIA_SET_CB2() */
    if(byte != via1[VIA_PCR]) {
        register BYTE tmp = byte;
        /* first set bit 1 and 5 to the real output values */
        if((tmp & 0x0c) != 0x0c)
            tmp |= 0x02;
        if((tmp & 0xc0) != 0xc0) tmp |= 0x20;
           iec_pcr_write(tmp);
    }
    return byte;
}

inline static BYTE read_pra(ADDRESS addr)
{
    BYTE byte;
    /* FIXME: not 100% sure about this... */
    BYTE val = ~via1[VIA_DDRA];
    BYTE msk = oldpb;
    BYTE m;
    int i;

    for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
        if (!(msk & m))
            val &= ~rev_keyarr[i];

    byte = val | (via1[VIA_PRA] & via1[VIA_DDRA]); 
    return byte;
}

inline static BYTE read_prb(void)
{
    BYTE byte;
    /* FIXME: not 100% sure about this... */
    BYTE val = ~via1[VIA_DDRB];
    BYTE msk = oldpa;
    int m, i;

    for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
        if (!(msk & m))
            val &= ~keyarr[i];

    /* Bit 7 is mapped to the right direction of the joystick (bit
       3 in `joystick_value[]'). */
    if ((joystick_value[1] | joystick_value[2]) & 0x8)
        val &= 0x7f;

    byte = val | (via1[VIA_PRB] & via1[VIA_DDRB]) ;
    return byte;
}

#include "viacore.c"

