/*
 *
 * via2.c - VIA2 emulation in the VIC20.
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
#define myrmwf rmw_flag
#define myvia via2

#define I_MYVIAFL I_VIA2FL
#define MYVIA_INT VIA2_INT
#define A_MYVIAT1 A_VIA2T1
#define A_MYVIAT2 A_VIA2T2

#define mycpu_int_status maincpu_int_status
#define mycpu_unset_alarm maincpu_unset_alarm
#define mycpu_set_alarm_clk maincpu_set_alarm_clk

#define reset_myvia reset_via2
#define store_myvia store_via2
#define read_myvia read_via2
#define peek_myvia peek_via2

#define myvia_log via2_log
#define myvia_signal via2_signal
#define myvia_prevent_clk_overflow via2_prevent_clk_overflow
#define myvia_read_snapshot_module via2_read_snapshot_module
#define myvia_write_snapshot_module via2_write_snapshot_module

#define int_myviat1 int_via2t1
#define int_myviat2 int_via2t2

#include "vice.h"
#include "viacore.h"

#include "drive.h"
#include "kbd.h"
#include "vic20iec.h"
#include "vic20via.h"
#include "mem.h"

#ifdef HAVE_PRINTER
#include "pruser.h"
#endif
#ifdef HAVE_RS232
#include "rsuser.h"
#endif

#define VIA_SET_CB2(a)
#define VIA_SET_CA2(a)

static int tape_sense = 0;

#define	via_set_int		maincpu_set_nmi
#define	VIA2_INT		IK_NMI

/* #define VIA2_TIMER_DEBUG */

static char snap_module_name[] = "VIA2";

void mem_set_tape_sense(int v)
{
    tape_sense = v;
}

static void undump_pra(BYTE byte)
{
    iec_pa_write(byte);
}

inline static void store_pra(BYTE byte, BYTE oldpa, ADDRESS addr)
{
    iec_pa_write(byte);
}

static void undump_prb(BYTE byte)
{
#ifdef HAVE_PRINTER
    pruser_write_data(byte);
#endif
}

inline static void store_prb(BYTE byte, BYTE oldpb, ADDRESS addr)
{
#ifdef HAVE_PRINTER
    pruser_write_data(byte);
#endif
#ifdef HAVE_RS232
    rsuser_write_ctrl(byte);
#endif
}

static void undump_pcr(BYTE byte)
{
}

static void res_via(void)
{
    iec_pa_write(0xff);

#ifdef HAVE_PRINTER
    pruser_write_data(0xff);
    pruser_write_strobe(1);
#endif
#ifdef HAVE_RS232
    rsuser_write_ctrl(0xff);
    rsuser_set_tx_bit(1);
#endif
}

inline static BYTE store_pcr(BYTE byte, ADDRESS addr)
{
    /* FIXME: should use VIA_SET_CA2() and VIA_SET_CB2() */
    if (byte != via2[VIA_PCR]) {
	register BYTE tmp = byte;
	/* first set bit 1 and 5 to the real output values */
	if ((tmp & 0x0c) != 0x0c)
	    tmp |= 0x02;
	if ((tmp & 0xc0) != 0xc0)
	    tmp |= 0x20;
	/* switching userport strobe with CB2 */
#ifdef HAVE_RS232
	if(rsuser_enabled) {
	    rsuser_set_tx_bit(byte & 0x20);
	}
#endif
#ifdef HAVE_PRINTER
	pruser_write_strobe(byte & 0x20);
#endif
    }
    return byte;
}

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

inline static BYTE read_pra(ADDRESS addr)
{
    BYTE byte;
    BYTE joy_bits;

    /*
        Port A is connected this way:

        bit 0  IEC clock
        bit 1  IEC data
        bit 2  joystick switch 0 (up)
        bit 3  joystick switch 1 (down)
        bit 4  joystick switch 2 (left)
        bit 5  joystick switch 4 (fire)
        bit 6  tape sense
        bit 7  IEC ATN
    */

    /* Setup joy bits (2 through 5).  Use the `or' of the values
       of both joysticks so that it works with every joystick
       setting.  This is a bit slow... we might think of a
       faster method.  */
    joy_bits = ~(joy[1] | joy[2]);
    joy_bits = ((joy_bits & 0x7) << 2) | ((joy_bits & 0x10) << 1);

    joy_bits |= tape_sense ? 0 : 0x40;

    /* We assume `iec_pa_read()' returns the non-IEC bits
       as zeroes. */
    byte = ((via2[VIA_PRA] & via2[VIA_DDRA])
           | ((iec_pa_read() | joy_bits) & ~via2[VIA_DDRA])); 
    return byte;
}

inline static BYTE read_prb(void)
{
    BYTE byte;
    byte = via2[VIA_PRB] | ~via2[VIA_DDRB];
#ifdef HAVE_RS232
    byte = rsuser_read_ctrl();
#else
    byte = 0xff;
#endif
    return byte;
}

#ifdef HAVE_PRINTER
void pruser_set_busy(int b)
{
    via2_signal(VIA_SIG_CB1, b ? VIA_SIG_RISE : VIA_SIG_FALL);
}
#endif

#include "viacore.c"

