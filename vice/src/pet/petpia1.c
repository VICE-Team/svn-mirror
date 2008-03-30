/*
 * petpia1.c -- PIA#1 chip emulation.
 *
 * Written by
 *  Jouko Valta (jopi@stekt.oulu.fi)
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

#include "vice.h"
#include "types.h"

#include "piacore.h"

#include "cmdline.h"
#include "resources.h"

#include "crtc.h"
#include "kbd.h"
#include "parallel.h"
#include "drive.h"
#include "pets.h"
#include "petmem.h"
#include "petpia.h"
#include "maincpu.h"

/* ------------------------------------------------------------------------- */
/* Renaming exported functions */

#define	MYPIA_NAME	"PIA1"

#define mypia_init pia1_init
#define reset_mypia reset_pia1
#define store_mypia store_pia1
#define read_mypia read_pia1
#define peek_mypia peek_pia1
#define mypia_write_snapshot_module pia1_write_snapshot_module
#define mypia_read_snapshot_module pia1_read_snapshot_module
#define	signal_mypia signal_pia1

static piareg mypia;

/* ------------------------------------------------------------------------- */
/* CPU binding */

#define	my_set_int(a)							\
	maincpu_set_irq(I_PIA1, (a)? IK_IRQ : IK_NONE)

#define	my_restore_int(a)						\
	set_int_noclk(&maincpu_int_status, I_PIA1, (a) ? IK_IRQ : IK_NONE)

#define mycpu_rmw_flag  rmw_flag
#define myclk           clk

/* ------------------------------------------------------------------------- */
/* PIA resources.  */

/* Flag: is the diagnostic pin enabled?  */
static int diagnostic_pin_enabled;

static int set_diagnostic_pin_enabled(resource_value_t v)
{
    diagnostic_pin_enabled = (int) v;
    return 0;
}

static resource_t resources[] = {
    { "DiagPin", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &diagnostic_pin_enabled, set_diagnostic_pin_enabled },
    { NULL }
};

int pia1_init_resources(void)
{
    return resources_register(resources);
}


static cmdline_option_t cmdline_options[] = {
    { "-diagpin", SET_RESOURCE, 0, NULL, NULL, "DiagPin", (resource_value_t) 1,
      NULL, "Enable userport diagnostic pin" },
    { "+diagpin", SET_RESOURCE, 0, NULL, NULL, "DiagPin", (resource_value_t) 1,
      NULL, "Disable userport diagnostic pin" },
    { NULL }
};

int pia1_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

static int tape1_sense = 0;

void mem_set_tape_sense(int v)
{
    tape1_sense = v;
}

/* ------------------------------------------------------------------------- */
/* I/O */

_PIA_FUNC void pia_set_ca2(int a)
{
    parallel_cpu_set_eoi((a)?0:1); 
    if(petres.pet2k) 
	crtc_screen_enable((a)?1:0);
}

_PIA_FUNC void pia_set_cb2(a)
{
}

_PIA_FUNC void pia_reset(void)
{
}


/*
E810	PORT A	7   Diagnostic sense (pin 5 on the user port)
		6   IEEE EOI in
		5   Cassette sense #2
		4   Cassette sense #1
		3-0 Keyboard row select (through 4->10 decoder)
E811	CA2	    output to blank the screen (old PETs only)
		    IEEE EOI out
	CA1	    cassette #1 read line
E812	PORT B	7-0 Contents of keyboard row
		    Usually all or all but one bits set.
E813	CB2	    output to cassette #1 motor: 0=on, 1=off
	CB1	    screen retrace detection in


	 Control

 7    CA1 active transition flag. 1= 0->1, 0= 1->0
 6    CA2 active transition flag. 1= 0->1, 0= 1->0
 5    CA2 direction	      1 = out	     | 0 = in
                    ------------+------------+---------------------
 4    CA2 control   Handshake=0 | Manual=1   | Active: High=1 Low=0
 3    CA2 control   On Read=0	| CA2 High=1 | IRQ on=1, IRQ off=0
		    Pulse  =1	| CA2 Low=0  |

 2    Port A control: DDRA = 0, IORA = 1
 1    CA1 control: Active High = 1, Low = 0
 0    CA1 control: IRQ on=1, off = 0
*/

_PIA_FUNC void store_pa(BYTE byte)
{
}

_PIA_FUNC void store_pb(BYTE byte)
{
}

_PIA_FUNC void undump_pa(BYTE byte)
{
}

_PIA_FUNC void undump_pb(BYTE byte)
{
}


_PIA_FUNC BYTE read_pa(void)
{
    BYTE byte;

    if (drive[0].enable)
        drive0_cpu_execute(clk);
    if (drive[1].enable)
        drive1_cpu_execute(clk);

    byte = 0xff
	- (tape1_sense ? 16 : 0)
	- (parallel_eoi ? 64 : 0)
	- ((diagnostic_pin_enabled || superpet_diag()) ? 128 : 0);
    byte = ((byte & ~mypia.ddr_a) | (mypia.port_a & mypia.ddr_a));

    return byte;
}


_PIA_FUNC BYTE read_pb(void)
{
    int     row;
    BYTE    j = 0xFF;

    row = mypia.port_a & 15;

    if (row < KBD_ROWS)
	j = ~keyarr[row];

#if (defined(DEBUG_PIA) || defined(KBDBUG))
    if (j < 255)
        log_message(mypia_log,
         "%02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X - row %d  %02x",
         keyarr[0], keyarr[1], keyarr[2], keyarr[3], keyarr[4],
         keyarr[5], keyarr[6], keyarr[7], keyarr[8], keyarr[9],
         row, j);
#endif

    return j;
}

#include "piacore.c"

