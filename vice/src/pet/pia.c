/*
 * pia.c -- PIA chip emulation.
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


#include <stdio.h>
#include <time.h>

#ifndef PET
#define	PET		/* to get PET definitions in include files as
			   pia.c is non of the files that are different
			   for different platforms - it just exists in
			   xpet only...! */
#endif

#include "vice.h"

#include "types.h"
#include "pia.h"
#include "vmachine.h"
#include "parallel.h"
#include "kbd.h"
#include "interrupt.h"
#include "pets.h"
#include "resources.h"
#include "cmdline.h"

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

int pia_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static cmdline_option_t cmdline_options[] = {
    { "-diagpin", SET_RESOURCE, 0, NULL, NULL, "DiagPin", (resource_value_t) 1,
      NULL, "Enable userport diagnostic pin" },
    { "+diagpin", SET_RESOURCE, 0, NULL, NULL, "DiagPin", (resource_value_t) 1,
      NULL, "Disable userport diagnostic pin" },
    { NULL }
};

int pia_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

#define P_PORT_A	0
#define P_CTRL_A	1
#define P_PORT_B	2
#define P_CTRL_B	3

typedef struct {
    BYTE  port_a;	/* output register, i.e. what has been written by
			   the CPU. input is assembled at read time */
    BYTE  ddr_a;	/* PIA Port A DDR */
    BYTE  ctrl_a;

    BYTE  port_b;
    BYTE  ddr_b;	/* PIA Port B DDR */
    BYTE  ctrl_b;
} piareg;


/*
 * Local variables
 */

static piareg  pia1, pia2;

/* ------------------------------------------------------------------------- */


void    reset_pia1(void)
{
   /* clear _all_ internal registers */

   pia1.ctrl_a = 0;	/* PIA 1 Port A Control */
   pia1.ctrl_b = 0;	/* PIA 1 Port B Control */
   pia1.ddr_a = 0;	/* PIA 1 Port A DDR */
   pia1.ddr_b = 0;	/* PIA 1 Port B DDR */
   pia1.port_a = 255;	/* PIA 1 Port A input; nothing to read from keyboard */
   pia1.port_b = 255;	/* PIA 1 Port B input; nothing to read from keyboard */

   par_set_eoi(0);	/* CA2 input mode -> pin goes high -> EOI not set */
   crtc_screen_enable(1);

   maincpu_set_irq(I_PIA1, IK_NONE);
}

void	reset_pia2(void)
{
   /* clear _all_ internal registers, set input lines */

   pia2.ctrl_a = 0;	/* PIA 2 Port A Control */
   pia2.ctrl_b = 0;	/* PIA 2 Port B Control */
   pia2.ddr_a = 0;	/* PIA 2 Port A DDR */
   pia2.ddr_b = 0;	/* PIA 2 Port B DDR */
			/* PIA 2 Port A input = iec bus */
   pia2.port_a = par_bus;
   pia2.port_b = 255;	/* PIA 2 Port B input; */

   par_set_ndac(0);	/* CA2 input mode */
   par_set_dav(0);	/* CA1 input mode */

   par_set_bus(0xff);	/* all data lines high, because of input mode */

   maincpu_set_irq(I_PIA2, IK_NONE);
}

static void pia1_update_irq(void) {
    if( ((pia1.ctrl_a & 0x81) == 0x81)
	    || ((pia1.ctrl_a & 0x68) == 0x48)
	    || ((pia1.ctrl_b & 0x81) == 0x81)
	    || ((pia1.ctrl_b & 0x68) == 0x48)
	) {
        maincpu_set_irq(I_PIA1, IK_IRQ);
    } else {
        maincpu_set_irq(I_PIA1, IK_NONE);
    }
}

static void pia2_update_irq(void) {
    if( ((pia2.ctrl_a & 0x81) == 0x81)
	    || ((pia2.ctrl_a & 0x68) == 0x48)
	    || ((pia2.ctrl_b & 0x81) == 0x81)
	    || ((pia2.ctrl_b & 0x68) == 0x48)
	) {
        maincpu_set_irq(I_PIA2, IK_IRQ);
    } else {
        maincpu_set_irq(I_PIA2, IK_NONE);
    }
}


/* unfinished control line flag support. Used for PET IRQ input. */
void signal_pia1(int line, int edge) {
    switch(line) {
    case PIA_SIG_CB1:
	/* this currently relies on each edge being called only once,
	 * otherwise multiple IRQs could occur. */
	if( ((pia1.ctrl_b & 0x02) ? PIA_SIG_RISE : PIA_SIG_FALL) == edge) {
	    pia1.ctrl_b |= 0x80;
	    pia1_update_irq();
            /* printf("clk=%d line=%d, edge=%d\n",clk, line, edge); */
	}
	break;
    }
}


/* ------------------------------------------------------------------------- */
/* PIA 1 */

void REGPARM2 store_pia1(ADDRESS addr, BYTE byte)
{

    addr &= 3;

#if 0
    printf("store pia1 [%x] %x\n", (int) addr, (int) byte);
#endif

    switch (addr) {

      case P_PORT_A: /* port A */
	if (pia1.ctrl_a & 4) {
	    pia1.port_a = byte;
		/*(pia1.port_a & ~pia1.ddr_a) | (byte & pia1.ddr_a);*/
	} else
	    pia1.ddr_a = byte;
	break;

      case P_PORT_B: /* port B */
	if (pia1.ctrl_b & 4)
	    pia1.port_b = byte;
		/*(pia1.port_b & ~pia1.ddr_b) | (byte & pia1.ddr_b);*/
	else
	    pia1.ddr_b = byte;
	break;


	/* Control */

      case P_CTRL_A: /* Control A */
	pia1.ctrl_a = (pia1.ctrl_a & 0xc0) | (byte & 0x3f);
	if(pia1.ctrl_a & 0x20) pia1.ctrl_a &= 0xbf;
	pia1_update_irq();

if(pardebug) printf("write pia1.ctrl_a(%x)\n",byte);
	if( (byte & 0x38) == 0x30 ) {
	  par_set_eoi(1);
	  if(pet.pet2k) crtc_screen_enable(0);
	} else {
	  par_set_eoi(0);	/* toggle mode still to be implemented */
	  if(pet.pet2k) crtc_screen_enable(1);
	}
	break;

      case P_CTRL_B: /* Control B */
	pia1.ctrl_b = (pia1.ctrl_b & 0xc0) | (byte & 0x3f);
	if(pia1.ctrl_b & 0x20) pia1.ctrl_b &= 0xbf;
	pia1_update_irq();
	break;


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

    }  /* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_pia1(ADDRESS addr)
{
    static BYTE inbyte;

    addr &= 3;

#if 0
    printf("read pia1 [%d]  [%02x %02x] [%02x] [%02x %02x] [%02x]\n",
           addr,
           pia1.port_a, pia1.ddr_a, pia1.ctrl_a,
           pia1.port_b, pia1.ddr_b, pia1.ctrl_b);
#endif

    switch (addr) {

      case P_PORT_A: /* port A */
	if (pia1.ctrl_a & 4) {
	    pia1.ctrl_a &= 0x3f;		/* Clear CA1,CA2 IRQ */
	    pia1_update_irq();

	    inbyte = 0xff
			- (par_eoi ? 64 : 0)
			- (diagnostic_pin_enabled ? 128 : 0);
	    return ((inbyte & ~pia1.ddr_a) | (pia1.port_a & pia1.ddr_a));
	}
	return (pia1.ddr_a);

/*
E810	PORT A	7   Diagnostic sense (pin 5 on the user port)
		6   IEEE EOI in
		5   Cassette sense #2
		4   Cassette sense #1
		3-0 Keyboard row select (through 4->10 decoder)

E812	PORT B	7-0 Contents of keyboard row
		    Usually all or all but one bits set.
*/
      case P_PORT_B: /* port B */
	if (pia1.ctrl_b & 4) {
	    int     row;
	    BYTE    j = 0xFF;

	    pia1.ctrl_b &= 0x3f;		/* Clear CB1,CB2 IRQ */
	    pia1_update_irq();

	    row = pia1.port_a & 15;

	    if (row < KBD_ROWS)
		j = ~keyarr[row];

#if 0
            printf("read pia1 port B %d\n", j);
            printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                   (int) pia1.port_a, (int) j,
                   (int) pia1.ddr_a, (int) pia1.ddr_b, joy[2]);
#endif
#if (defined(DEBUG_PIA) || defined(KBDBUG))
	    if (j < 255)
                printf("%02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X - row %d  %02x\n",
                       keyarr[0], keyarr[1], keyarr[2], keyarr[3], keyarr[4],
                       keyarr[5], keyarr[6], keyarr[7], keyarr[8], keyarr[9],
                       row, j);
#endif

	    return (j);
	}
	return (pia1.ddr_a);


	/* Control */

      case P_CTRL_A: /* Control A */
	return (pia1.ctrl_a);

      case P_CTRL_B: /* Control B */
	return (pia1.ctrl_b);
	break;

    }  /* switch */

    return (0xFF);
}


/* ------------------------------------------------------------------------- */
/* PIA 2 */

void REGPARM2 store_pia2(ADDRESS addr, BYTE byte)
{
    addr &= 3;

#if 0
    printf("store pia2 [%x] %x\n", (int) addr, (int) byte);
#endif

/*
E820	PORT A	    Input buffer for IEEE data lines
E821	CA2	    IEEE NDAC out
	CA1	    IEEE ATN in
E822	PORT B	    Output buffer for IEEE data lines
E823	CB2	    IEEE DAV out
	CB1	    IEEE SRQ in
*/

    switch (addr) {

      case P_PORT_A: /* port A */
	if (pia2.ctrl_a & 4) {
	    pia2.port_a = byte;
		/*(pia2.port_a & ~pia2.ddr_a) | (byte & pia2.ddr_a);*/
	} else
	    pia2.ddr_a = byte;
	break;

      case P_PORT_B: /* port B */
	if (pia2.ctrl_b & 4) {
if(pardebug) printf("write pia2.port_b(%x)\n",byte);
	    par_set_bus(byte^255); /* ignoring ddr_b, hmm... */
	    pia2.port_b = byte;
		/*(pia2.port_b & ~pia2.ddr_b) | (byte & pia2.ddr_b);*/
	} else
	    pia2.ddr_b = byte;
	break;


	/* Control */

      case P_CTRL_A: /* Control A */
	pia2.ctrl_a = (pia2.ctrl_a & 0xc0) | (byte & 0x3f);
	if(pia2.ctrl_a & 0x20) pia2.ctrl_a &= 0xbf;
	pia2_update_irq();

if(pardebug) printf("write pia2.ctrl_a(%x)\n",byte);
	if( (byte & 0x38) == 0x30 ) par_set_ndac(1);
	else par_set_ndac(0);
	break;

      case P_CTRL_B: /* Control B */
	pia2.ctrl_b = (pia2.ctrl_b & 0xc0) | (byte & 0x3f);
	if(pia2.ctrl_b & 0x20) pia2.ctrl_b &= 0xbf;
	pia2_update_irq();

if(pardebug) printf("write pia2.ctrl_b(%x)\n",byte);
	if( (byte & 0x38) == 0x30 ) par_set_dav(1);
	else par_set_dav(0);
	break;

    }  /* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_pia2(ADDRESS addr)
{
    addr &= 3;

#if 0
    printf("read pia2 [%d]\n", addr);
#endif

    switch (addr) {

      case P_PORT_A: /* port A */
	if (pia2.ctrl_a & 4) {
	    pia2.ctrl_a &= 0x3f;		/* Clear CA1,CA2 IRQ */
	    pia2_update_irq();

	    if (pardebug)
		printf("read pia2 port A %x, par_bus=%x, gives %x\n",
			pia2.port_a, par_bus,
			(par_bus & ~pia2.ddr_a) |(pia2.port_a & pia2.ddr_a));

	    return (par_bus & ~pia2.ddr_a) |(pia2.port_a & pia2.ddr_a);
	}
	return (pia2.ddr_a);

      case P_PORT_B: /* port B */
	if (pia2.ctrl_b & 4) {
	    pia2.ctrl_b &= 0x3f;		/* Clear CB1,CB2 IRQ */
	    pia2_update_irq();

	    if (pardebug)
		printf("read pia2 port B %x, par_bus=%x, gives %x\n",
			pia2.port_b, par_bus,
			(~(par_bus|pia2.ddr_b)) |(pia2.port_b & pia2.ddr_b));

	    return (~pia2.ddr_b) |(pia2.port_b & pia2.ddr_b);
	}
	else
	    return (pia2.ddr_b);


	/* Control */

      case P_CTRL_A: /* Control A */
	return (pia2.ctrl_a);

      case P_CTRL_B: /* Control B */
	return (pia2.ctrl_b);
	break;

    }  /* switch */

    return (0xFF);
}
