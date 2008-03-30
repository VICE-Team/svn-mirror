
/*
 * ../../../src/vic20/vic20via2.c
 * This file is generated from ../../../src/via-tmpl.c and ../../../src/vic20/vic20via2.def,
 * Do not edit!
 */
/*
 * via-tmpl.c - Template file for VIA emulation.
 *
 * Written by
 *  André Fachat (fachat@physik.tu-chemnitz.de)
 *
 * Patch by
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

/*
 * 24jan97 a.fachat
 * new interrupt handling, hopefully according to the specs now.
 * All interrupts (note: not timer events (i.e. alarms) are put
 * into one interrupt flag, I_VIA2FL.
 * if an interrupt condition changes, the function (i.e. cpp macro)
 * update_via2irq() id called, that checks the IRQ line state.
 * This is now possible, as ettore has decoupled A_* alarm events
 * from I_* interrupts for performance reasons.
 *
 * A new function for signaling rising/falling edges on the
 * control lines is introduced:
 *      via2_signal(VIA_SIG_[CA1|CA2|CB1|CB2], VIA_SIG_[RISE|FALL])
 * which signals the corresponding edge to the VIA. The constants
 * are defined in via.h.
 *
 * Except for shift register and input latching everything should be ok now.
 */

#include "vice.h"

#ifdef STDC_HEADERS
#include <stdio.h>
#include <time.h>
#endif

#include "vmachine.h"
#include "via.h"
#include "resources.h"
#include "snapshot.h"


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

#include "interrupt.h"
				/* Timer debugging */
/*#define VIA2_TIMER_DEBUG */
				/* when PB7 is really used, set this
				   to enable pulse output from the timer.
				   Otherwise PB7 state is computed only
				   when port B is read -
				   not yet implemented */
/*#define VIA2_NEED_PB7 */
				/* When you really need latching, define this.
				   It implies additional READ_PR* when
				   writing the snapshot. When latching is 
				   enabled: it reads the port when enabling,
				   and when an active C*1 transition occurs. 
				   It does not read the port when reading the
				   port register. Side-effects beware! */
/*#define VIA2_NEED_LATCHING */

/* global */

BYTE via2[16];


    static int tape_sense = 0;

/*
 * Local variables
 */

static int via2ifr;		/* Interrupt Flag register for via2 */
static int via2ier;		/* Interrupt Enable register for via2 */

static unsigned int via2tal;	/* current timer A latch value */
static unsigned int via2tbl;	/* current timer B latch value */

static CLOCK via2tau;		/* time when via2 timer A is updated */
static CLOCK via2tbu;		/* time when via2 timer B is updated */
static CLOCK via2tai;		/* time when next timer A alarm is */
static CLOCK via2tbi;		/* time when next timer A alarm is */

static int via2pb7;		/* state of PB7 for pulse output... */
static int via2pb7x;		/* to be xored herewith  */
static int via2pb7o;		/* to be ored herewith  */
static int via2pb7xx;
static int via2pb7sx;

static BYTE oldpa;		/* the actual output on PA (input = high) */
static BYTE oldpb;		/* the actual output on PB (input = high) */

static BYTE via2_ila;		/* input latch A */
static BYTE via2_ilb;		/* input latch B */

static int ca2_state;
static int cb2_state;

/*
 * local functions
 */

#define IS_CA2_OUTPUT()          ((via2[VIA_PCR] & 0x0c) == 0x0c)
#define IS_CA2_INDINPUT()        ((via2[VIA_PCR] & 0x0a) == 0x02)
#define IS_CA2_HANDSHAKE()       ((via2[VIA_PCR] & 0x0c) == 0x08)
#define IS_CA2_PULSE_MODE()      ((via2[VIA_PCR] & 0x0e) == 0x09)
#define IS_CA2_TOGGLE_MODE()     ((via2[VIA_PCR] & 0x0e) == 0x08)

#define IS_CB2_OUTPUT()          ((via2[VIA_PCR] & 0xc0) == 0xc0)
#define IS_CB2_INDINPUT()        ((via2[VIA_PCR] & 0xa0) == 0x20)
#define IS_CB2_HANDSHAKE()       ((via2[VIA_PCR] & 0xc0) == 0x80)
#define IS_CB2_PULSE_MODE()      ((via2[VIA_PCR] & 0xe0) == 0x90)
#define IS_CB2_TOGGLE_MODE()     ((via2[VIA_PCR] & 0xe0) == 0x80)

#define	IS_PA_INPUT_LATCH()	 (via2[VIA_ACR] & 0x01)
#define	IS_PB_INPUT_LATCH()	 (via2[VIA_ACR] & 0x02)

/*
 * 01apr98 a.fachat
 *
 * One-shot Timing (partly from 6522-VIA.txt):

                     +-+ +-+ +-+ +-+ +-+ +-+   +-+ +-+ +-+ +-+ +-+ +-+
                02 --+ +-+ +-+ +-+ +-+ +-+ +-#-+ +-+ +-+ +-+ +-+ +-+ +-
                       |   |                           |
                       +---+                           |
       WRITE T1C-H ----+   +-----------------#-------------------------
        ___                |                           |
        IRQ OUTPUT --------------------------#---------+
                           |                           +---------------
                           |                           |
        PB7 OUTPUT --------+                           +---------------
                           +-----------------#---------+
         T1                | N |N-1|N-2|N-3|     | 0 | -1|N  |N-1|N-2|
         T2                | N |N-1|N-2|N-3|     | 0 | -1| -2| -3| -4|
                           |                           |
                           |<---- N + 1.5 CYCLES ----->|<--- N + 2 cycles --->
                                                         +---+
 via2t*u* clk ------------------------------------------+   +--------
                                                     |
                                                     |
                                                  call of
			                        int_via2*
                                                   here

   real via2tau value = via2tau* + TAUOFFSET
   via2tbu = via2tbu* + 0

 *
 * IRQ and PB7 are set/toggled at the low-high transition of Phi2,
 * but int_* is called a half-cycle before that. Does that matter?
 *
 * PB7 output is still to be implemented
 */

/* timer values do not depend on a certain value here, but PB7 does... */
#define	TAUOFFSET	(-1)


#ifndef via_restore_int	/* if VIA reports to other chip (TPI) for IRQ */
#define	via_restore_int(a)  set_int_noclk(&maincpu_int_status, I_VIA2FL, \
		(a) ? IK_NMI : 0)
#endif


inline static void update_via2irq(void)
{
#if 0	/* DEBUG */
    static int irq = 0;
    if(irq && !(via2ifr & via2ier & 0x7f)) {
       printf("via2: clk=%d, IRQ off\n", clk);
    }
    if(!irq && (via2ifr & via2ier & 0x7f)) {
       printf("via2: clk=%d, IRQ on\n", clk);
    }
    irq = (via2ifr & via2ier & 0x7f);
#endif
    maincpu_set_nmi(I_VIA2FL, (via2ifr & via2ier & 0x7f) ? IK_NMI : 0);
}

/* the next two are used in read_via2() */

inline static unsigned int via2ta(void)
{
    if (clk < via2tau - TAUOFFSET)
        return via2tau - TAUOFFSET - clk - 2;
    else
	return (via2tal - (clk - via2tau + TAUOFFSET) % (via2tal + 2));
}

inline static unsigned int via2tb(void)
{
    return via2tbu - clk - 2;
}

inline static void update_via2tal(CLOCK rclk)
{
    via2pb7x = 0;
    via2pb7xx = 0;

    if (rclk > via2tau) {
	int nuf = (via2tal + 1 + rclk - via2tau) / (via2tal + 2);

	if (!(via2[VIA_ACR] & 0x40)) {
	    if (((nuf - via2pb7sx) > 1) || (!via2pb7)) {
		via2pb7o = 1;
		via2pb7sx = 0;
	    }
	}
	via2pb7 ^= (nuf & 1);

	via2tau = TAUOFFSET + via2tal + 2 + (rclk - (rclk - via2tau + TAUOFFSET) % (via2tal + 2));
	if (rclk == via2tau - via2tal - 1) {
	    via2pb7xx = 1;
	}
    }

    if (via2tau == rclk)
	via2pb7x = 1;

    via2tal = via2[VIA_T1LL] + (via2[VIA_T1LH] << 8);
}

inline static void update_via2tbl(void)
{
    via2tbl = via2[VIA_T2CL] + (via2[VIA_T2CH] << 8);
}


/* ------------------------------------------------------------------------- */
/* VIA2 */



    void mem_set_tape_sense(int v) {
	tape_sense = v;
    }

/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void reset_via2(void)
{
    int i;
#ifdef VIA2_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("VIA2: reset\n");
#endif
    /* clear registers */
    for (i = 0; i < 4; i++)
	via2[i] = 0;
    for (i = 4; i < 10; i++)
        via2[i] = 0xff;        /* AB 98.08.23 */
    for (i = 11; i < 16; i++)
	via2[i] = 0;

    via2tal = 0;
    via2tbl = 0;
    via2tau = clk;
    via2tbu = clk;

    via2ier = 0;
    via2ifr = 0;

    /* disable vice interrupts */
    via2tai = 0;
    via2tbi = 0;
    maincpu_unset_alarm(A_VIA2T1);
    maincpu_unset_alarm(A_VIA2T2);
    update_via2irq();

    oldpa = 0xff;
    oldpb = 0xff;

    ca2_state = 1;
    cb2_state = 1;
    VIA_SET_CA2( ca2_state )	/* input = high */
    VIA_SET_CB2( cb2_state )	/* input = high */


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

void via2_signal(int line, int edge)
{
    switch (line) {
      case VIA_SIG_CA1:
	if ( (edge ? 1 : 0) == (via2[VIA_PCR] & 0x01) ) {
	    if (IS_CA2_TOGGLE_MODE() && !ca2_state) {
		ca2_state = 1;
		VIA_SET_CA2( ca2_state )
	    }
            via2ifr |= VIA_IM_CA1;
            update_via2irq();
#ifdef VIA2_NEED_LATCHING
	    if (IS_PA_INPUT_LATCH()) {
		BYTE byte;

    {
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
    }
		via2_ila = byte;
	    }
#endif
	}
        break;
      case VIA_SIG_CA2:
        if (!(via2[VIA_PCR] & 0x08)) {
            via2ifr |= (((edge << 2) ^ via2[VIA_PCR]) & 0x04) ?
                0 : VIA_IM_CA2;
            update_via2irq();
        }
        break;
      case VIA_SIG_CB1:
	if ( (edge ? 0x10 : 0) == (via2[VIA_PCR] & 0x10) ) {
	    if (IS_CB2_TOGGLE_MODE() && !cb2_state) {
		cb2_state = 1;
		VIA_SET_CB2( cb2_state )
	    }
            via2ifr |= VIA_IM_CB1;
            update_via2irq();
#ifdef VIA2_NEED_LATCHING
	    if (IS_PB_INPUT_LATCH()) {
		BYTE byte;

#ifdef HAVE_RS232
    byte = rsuser_read_ctrl();
#else
    byte = 0xff;
#endif
		via2_ilb = byte;
	    }
#endif	
	}
        break;
      case VIA_SIG_CB2:
        if (!(via2[VIA_PCR] & 0x80)) {
            via2ifr |= (((edge << 6) ^ via2[VIA_PCR]) & 0x40) ?
                0 : VIA_IM_CB2;
            update_via2irq();
        }
        break;
    }
}

void REGPARM2 store_via2(ADDRESS addr, BYTE byte)
{
    CLOCK rclk = clk - 1;	/* stores have a one-cylce offset */

    addr &= 0xf;
#ifdef VIA2_TIMER_DEBUG
    if ((addr < 10 && addr > 3) || (addr == VIA_ACR))
	printf("store via2[%x] %x, rmwf=%d, clk=%d, rclk=%d\n",
	       (int) addr, (int) byte, rmw_flag, clk, rclk);
#endif

    switch (addr) {

      /* these are done with saving the value */
      case VIA_PRA:		/* port A */
        via2ifr &= ~VIA_IM_CA1;
        if (!IS_CA2_INDINPUT()) {
            via2ifr &= ~VIA_IM_CA2;
        }
	if(IS_CA2_HANDSHAKE()) {
	    ca2_state = 0;
	    VIA_SET_CA2( ca2_state )
	    if(IS_CA2_PULSE_MODE()) {
	  	ca2_state = 1;
	    	VIA_SET_CA2( ca2_state )
	    }
	}
        update_via2irq();

      case VIA_PRA_NHS:	/* port A, no handshake */
        via2[VIA_PRA_NHS] = byte;
        addr = VIA_PRA;
      case VIA_DDRA:
	via2[addr] = byte;
	byte = via2[VIA_PRA] | ~via2[VIA_DDRA];

    iec_pa_write(byte);
	oldpa = byte;
        break;

      case VIA_PRB:		/* port B */
        via2ifr &= ~VIA_IM_CB1;
        if ((via2[VIA_PCR] & 0xa0) != 0x20) {
            via2ifr &= ~VIA_IM_CB2;
        }
        if(IS_CB2_HANDSHAKE()) {
            cb2_state = 0;
            VIA_SET_CB2( cb2_state )
            if(IS_CB2_PULSE_MODE()) {
                cb2_state = 1;
                VIA_SET_CB2( cb2_state )
            }
        }
        update_via2irq();

      case VIA_DDRB:
	via2[addr] = byte;
	byte = via2[VIA_PRB] | ~via2[VIA_DDRB];

#ifdef HAVE_PRINTER
    pruser_write_data(byte);
#endif
#ifdef HAVE_RS232
    rsuser_write_ctrl(byte);
#endif
	oldpb = byte;
        break;

      case VIA_SR:		/* Serial Port output buffer */
        via2[addr] = byte;
        
            break;

        /* Timers */

      case VIA_T1CL:
      case VIA_T1LL:
        via2[VIA_T1LL] = byte;
        update_via2tal(rclk);
        break;

      case VIA_T1CH /*TIMER_AH */ :	/* Write timer A high */
#ifdef VIA2_TIMER_DEBUG
        if (app_resources.debugFlag)
            printf("Write timer A high: %02x\n", byte);
#endif
        via2[VIA_T1LH] = byte;
        update_via2tal(rclk);
        /* load counter with latch value */
        via2tau = rclk + via2tal + 3 + TAUOFFSET;
        via2tai = rclk + via2tal + 2;
        maincpu_set_alarm_clk(A_VIA2T1, via2tai);

        /* set pb7 state */
        via2pb7 = 0;
        via2pb7o = 0;

        /* Clear T1 interrupt */
        via2ifr &= ~VIA_IM_T1;
        update_via2irq();
        break;

      case VIA_T1LH:		/* Write timer A high order latch */
        via2[addr] = byte;
        update_via2tal(rclk);

        /* Clear T1 interrupt */
        via2ifr &= ~VIA_IM_T1;
        update_via2irq();
        break;

      case VIA_T2LL:		/* Write timer 2 low latch */
        via2[VIA_T2LL] = byte;
        update_via2tbl();
        
            break;

      case VIA_T2CH:		/* Write timer 2 high */
        via2[VIA_T2CH] = byte;
        update_via2tbl();
        via2tbu = rclk + via2tbl + 3;
        via2tbi = rclk + via2tbl + 2;
        maincpu_set_alarm_clk(A_VIA2T2, via2tbi);

        /* Clear T2 interrupt */
        via2ifr &= ~VIA_IM_T2;
        update_via2irq();
        break;

        /* Interrupts */

      case VIA_IFR:		/* 6522 Interrupt Flag Register */
        via2ifr &= ~byte;
        update_via2irq();
        break;

      case VIA_IER:		/* Interrupt Enable Register */
#if defined (VIA2_TIMER_DEBUG)
        printf("Via#1 set VIA_IER: 0x%x\n", byte);
#endif
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
            via2ier |= byte & 0x7f;
        } else {
            /* clear interrupts */
            via2ier &= ~byte;
        }
        update_via2irq();
        break;

        /* Control */

      case VIA_ACR:
        /* bit 7 timer 1 output to PB7 */
        update_via2tal(rclk);
        if ((via2[VIA_ACR] ^ byte) & 0x80) {
            if (byte & 0x80) {
                via2pb7 = 1 ^ via2pb7x;
            }
        }
        if ((via2[VIA_ACR] ^ byte) & 0x40) {
            via2pb7 ^= via2pb7sx;
            if ((byte & 0x40)) {
                if (via2pb7x || via2pb7xx) {
                    if (via2tal) {
                        via2pb7o = 1;
                    } else {
                        via2pb7o = 0;
                        if ((via2[VIA_ACR] & 0x80) && via2pb7x && (!via2pb7xx))
                            via2pb7 ^= 1;
                    }
                }
            }
        }
        via2pb7sx = via2pb7x;

        /* bit 1, 0  latch enable port B and A */
#ifdef VIA2_NEED_LATCHING
	/* switch on port A latching - FIXME: is this ok? */
	if ( (!(via2[addr] & 1)) && (byte & 1)) {

    {
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
    }
	    via2_ila = byte;
	}
	/* switch on port B latching - FIXME: is this ok? */
	if ( (!(via2[addr] & 2)) && (byte & 2)) {

#ifdef HAVE_RS232
    byte = rsuser_read_ctrl();
#else
    byte = 0xff;
#endif
	    via2_ilb = byte;
	}
#endif

        via2[addr] = byte;

        

        /* bit 5 timer 2 count mode */
        if (byte & 32) {
            /* TODO */
            /* update_via2tb(0); *//* stop timer if mode == 1 */
        }

        /* bit 4, 3, 2 shift register control */

        break;

      case VIA_PCR:

        /* if(viadebug) printf("VIA1: write %02x to PCR\n",byte); */

        /* bit 7, 6, 5  CB2 handshake/interrupt control */
        /* bit 4  CB1 interrupt control */

        /* bit 3, 2, 1  CA2 handshake/interrupt control */
        /* bit 0  CA1 interrupt control */

	if ( (byte & 0x0e) == 0x0c ) {	/* set output low */
	    ca2_state = 0;
	} else 
	if ( (byte & 0x0e) == 0x0e ) {	/* set output high */
	    ca2_state = 1;
	} else {			/* set to toggle/pulse/input */
	    /* FIXME: is this correct if handshake is already active? */
	    ca2_state = 1;
	}
	VIA_SET_CA2( ca2_state )

	if ( (byte & 0xe0) == 0xc0 ) {	/* set output low */
	    cb2_state = 0;
	} else 
	if ( (byte & 0xe0) == 0xe0 ) {	/* set output high */
	    cb2_state = 1;
	} else {			/* set to toggle/pulse/input */
	    /* FIXME: is this correct if handshake is already active? */
	    cb2_state = 1;
	}
	VIA_SET_CB2( cb2_state )


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

        via2[addr] = byte;

        break;

      default:
        via2[addr] = byte;

    }				/* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_via2(ADDRESS addr)
{
#ifdef VIA2_TIMER_DEBUG
    BYTE read_via2_(ADDRESS);
    BYTE retv = read_via2_(addr);
    addr &= 0x0f;
    if ((addr > 3 && addr < 10) || app_resources.debugFlag)
	printf("read_via2(%x) -> %02x, clk=%d\n", addr, retv, clk);
    return retv;
}
BYTE REGPARM1 read_via2_(ADDRESS addr)
{
#endif
    BYTE byte = 0xff;
    CLOCK rclk = clk;

    addr &= 0xf;

    if (via2tai && (via2tai <= clk))
	int_via2t1(clk - via2tai);
    if (via2tbi && (via2tbi <= clk))
	int_via2t2(clk - via2tbi);

    switch (addr) {

      case VIA_PRA:		/* port A */
        via2ifr &= ~VIA_IM_CA1;
        if ((via2[VIA_PCR] & 0x0a) != 0x02) {
            via2ifr &= ~VIA_IM_CA2;
        }
        if(IS_CA2_HANDSHAKE()) {
            ca2_state = 0;
            VIA_SET_CA2( ca2_state )
            if(IS_CA2_PULSE_MODE()) {
                ca2_state = 1;
                VIA_SET_CA2( ca2_state )
            }
        }
        update_via2irq();

      case VIA_PRA_NHS:	/* port A, no handshake */
        /* WARNING: this pin reads the voltage of the output pins, not
           the ORA value as the other port. Value read might be different
           from what is expected due to excessive load. */
#ifdef VIA2_NEED_LATCHING
	if (IS_PA_INPUT_LATCH()) {
	    byte = via2_ila;
	} else {

    {
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
    }
	}
#else

    {
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
    }
#endif
	via2_ila = byte;
	return byte;

      case VIA_PRB:		/* port B */
        via2ifr &= ~VIA_IM_CB1;
        if ((via2[VIA_PCR] & 0xa0) != 0x20)
            via2ifr &= ~VIA_IM_CB2;
        update_via2irq();

        /* WARNING: this pin reads the ORA for output pins, not
           the voltage on the pins as the other port. */
#ifdef VIA2_NEED_LATCHING
	if (IS_PB_INPUT_LATCH()) {
	    byte = via2_ilb;
	} else {

#ifdef HAVE_RS232
    byte = rsuser_read_ctrl();
#else
    byte = 0xff;
#endif
	}
#else

#ifdef HAVE_RS232
    byte = rsuser_read_ctrl();
#else
    byte = 0xff;
#endif
#endif
	via2_ilb = byte;
        byte = (byte & ~via2[VIA_DDRB]) | (via2[VIA_PRB] & via2[VIA_DDRB]);

        if (via2[VIA_ACR] & 0x80) {
            update_via2tal(rclk);
            byte = (byte & 0x7f) | (((via2pb7 ^ via2pb7x) | via2pb7o) ? 0x80 : 0);
        }
        return byte;

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        via2ifr &= ~VIA_IM_T1;
        update_via2irq();
        return via2ta() & 0xff;

      case VIA_T1CH /*TIMER_AH */ :	/* timer A high */
        return (via2ta() >> 8) & 0xff;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        via2ifr &= ~VIA_IM_T2;
        update_via2irq();
        return via2tb() & 0xff;

      case VIA_T2CH /*TIMER_BH */ :	/* timer B high */
        return (via2tb() >> 8) & 0xff;

      case VIA_SR:		/* Serial Port Shift Register */
        return (via2[addr]);

        /* Interrupts */

      case VIA_IFR:		/* Interrupt Flag Register */
        {
            BYTE t = via2ifr;
            if (via2ifr & via2ier /*[VIA_IER] */ )
                t |= 0x80;
            return (t);
        }

      case VIA_IER:		/* 6522 Interrupt Control Register */
        return (via2ier /*[VIA_IER] */  | 0x80);

    }				/* switch */

    return (via2[addr]);
}

BYTE REGPARM1 peek_via2(ADDRESS addr)
{
    CLOCK rclk = clk;

    addr &= 0xf;

    if (via2tai && (via2tai <= clk))
	int_via2t1(clk - via2tai);
    if (via2tbi && (via2tbi <= clk))
	int_via2t2(clk - via2tbi);

    switch (addr) {
      case VIA_PRA:
        return read_via2(VIA_PRA_NHS);

      case VIA_PRB:		/* port B */
        {
            BYTE byte;
#ifdef VIA2_NEED_LATCHING
	    if (IS_PB_INPUT_LATCH()) {
	        byte = via2_ilb;
	    } else {

#ifdef HAVE_RS232
    byte = rsuser_read_ctrl();
#else
    byte = 0xff;
#endif
	    }
#else

#ifdef HAVE_RS232
    byte = rsuser_read_ctrl();
#else
    byte = 0xff;
#endif
#endif
            byte = (byte & ~via2[VIA_DDRB]) | (via2[VIA_PRB] & via2[VIA_DDRB]);
            if (via2[VIA_ACR] & 0x80) {
                update_via2tal(rclk);
                byte = (byte & 0x7f) | (((via2pb7 ^ via2pb7x) | via2pb7o) ? 0x80 : 0);
            }
            return byte;
        }

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        return via2ta() & 0xff;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        return via2tb() & 0xff;

      default:
        break;
    }				/* switch */

    return read_via2(addr);
}


/* ------------------------------------------------------------------------- */

int int_via2t1(long offset)
{
/*    CLOCK rclk = clk - offset; */
#ifdef VIA2_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("via2 timer A interrupt\n");
#endif

    if (!(via2[VIA_ACR] & 0x40)) {	/* one-shot mode */
#if 0				/* defined (VIA2_TIMER_DEBUG) */
	printf("VIA2 Timer A interrupt -- one-shot mode: next int won't happen\n");
#endif
	maincpu_unset_alarm(A_VIA2T1);	/*int_clk[I_VIA2T1] = 0; */
	via2tai = 0;
    } else {			/* continuous mode */
	/* load counter with latch value */
	via2tai += via2tal + 2;
	maincpu_set_alarm_clk(A_VIA2T1, via2tai);
    }
    via2ifr |= VIA_IM_T1;
    update_via2irq();

    /* TODO: toggle PB7? */
    return 0;			/*(viaier & VIA_IM_T1) ? 1:0; */
}

/*
 * Timer B is always in one-shot mode
 */

int int_via2t2(long offset)
{
#ifdef VIA2_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("VIA2 timer B interrupt\n");
#endif
    maincpu_unset_alarm(A_VIA2T2);	/*int_clk[I_VIA2T2] = 0; */
    via2tbi = 0;

    via2ifr |= VIA_IM_T2;
    update_via2irq();

    return 0;
}

void via2_prevent_clk_overflow(CLOCK sub)
{
    unsigned int t;
    t = (via2tau - (clk + sub)) & 0xffff;
    via2tau = clk + t;
    t = (via2tbu - (clk + sub)) & 0xffff;
    via2tbu = clk + t;
    if (via2tai)
	via2tai -= sub;
}

/*------------------------------------------------------------------------*/

/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 0/0
 */

static char snap_module_name[] = "VIA2";
#define VIA_DUMP_VER_MAJOR      1
#define VIA_DUMP_VER_MINOR      0

/*
 * The dump data:
 *
 * UBYTE        ORA
 * UBYTE        DDRA
 * UBYTE        ORB
 * UBYTE        DDRB
 * UWORD	T1L
 * UWORD	T1C
 * UBYTE	T2L
 * UWORD	T2C
 * UBYTE	SR
 * UBYTE	ACR
 * UBYTE	PCR
 * UBYTE	IFR		 active interrupts
 * UBYTE	IER		 interrupt masks
 * UBYTE	PB7		 bit 7 = pb7 state
 * UBYTE	SRHBITS		 number of half bits to shift out on SR
 * UBYTE	CABSTATE	 bit 7 = ca2 state, bi 6 = cb2 state
 * UBYTE	ILA		 input latch port A
 * UBYTE	ILB		 input latch port B
 */

/* FIXME!!!  Error check.  */

int via2_write_snapshot_module(snapshot_t * p)
{
    snapshot_module_t *m;

    if (via2tai && (via2tai <= clk))
        int_via2t1(clk - via2tai);
    if (via2tbi && (via2tbi <= clk))
        int_via2t2(clk - via2tbi);

    m = snapshot_module_create(p, snap_module_name,
                               VIA_DUMP_VER_MAJOR, VIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;
/*
printf("via2: write: clk=%d, tai=%d, tau=%d\n"
       "     : tbi=%d, tbu=%d\n",
		clk, via2tai, via2tau, via2tbi, via2tbu);
printf("     : ta=%d, tb=%d\n",via2ta() & 0xffff, via2tb() & 0xffff);
*/
    snapshot_module_write_byte(m, via2[VIA_PRA]);
    snapshot_module_write_byte(m, via2[VIA_DDRA]);
    snapshot_module_write_byte(m, via2[VIA_PRB]);
    snapshot_module_write_byte(m, via2[VIA_DDRB]);

    snapshot_module_write_word(m, via2tal);
    snapshot_module_write_word(m, via2ta());
    snapshot_module_write_byte(m, via2tbl);
    snapshot_module_write_word(m, via2tb());

    snapshot_module_write_byte(m, (via2tai ? 0x80 : 0)
					| (via2tbi ? 0x40 : 0) );

    snapshot_module_write_byte(m, via2[VIA_SR]);
    snapshot_module_write_byte(m, via2[VIA_ACR]);
    snapshot_module_write_byte(m, via2[VIA_PCR]);

    snapshot_module_write_byte(m, via2ifr);
    snapshot_module_write_byte(m, via2ier);

						/* FIXME! */
    snapshot_module_write_byte(m, (((via2pb7 ^ via2pb7x) | via2pb7o) ? 0x80 : 0));
    snapshot_module_write_byte(m, 0);		/* SRHBITS */

    snapshot_module_write_byte(m, (ca2_state ? 0x80 : 0) 
				| (cb2_state ? 0x40 : 0));

    snapshot_module_write_byte(m, via2_ila);
    snapshot_module_write_byte(m, via2_ilb);

    snapshot_module_close(m);

    return 0;
}

int via2_read_snapshot_module(snapshot_t * p)
{
    BYTE vmajor, vminor;
    BYTE byte;
    WORD word;
    ADDRESS addr;
    CLOCK rclk = clk;
    snapshot_module_t *m;

    m = snapshot_module_open(p, snap_module_name, &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (vmajor != VIA_DUMP_VER_MAJOR) {
        fprintf(stderr,
                "MEM: Snapshot module version (%d.%d) newer than %d.%d.\n",
                vmajor, vminor, VIA_DUMP_VER_MAJOR, VIA_DUMP_VER_MINOR);
        snapshot_module_close(m);
        return -1;
    }

    maincpu_unset_alarm(A_VIA2T1);
    maincpu_unset_alarm(A_VIA2T2);
    via2tai = 0;
    via2tbi = 0;

    snapshot_module_read_byte(m, &via2[VIA_PRA]);
    snapshot_module_read_byte(m, &via2[VIA_DDRA]);
    snapshot_module_read_byte(m, &via2[VIA_PRB]);
    snapshot_module_read_byte(m, &via2[VIA_DDRB]);
    {
        addr = VIA_DDRA;
	byte = via2[VIA_PRA] | ~via2[VIA_DDRA];

    iec_pa_write(byte);
	oldpa = byte;

	addr = VIA_DDRB;
	byte = via2[VIA_PRB] | ~via2[VIA_DDRB];

#ifdef HAVE_PRINTER
    pruser_write_data(byte);
#endif
	oldpb = byte;
    }

    snapshot_module_read_word(m, &word);
    via2tal = word;
    snapshot_module_read_word(m, &word);
    via2tau = rclk + word + 2 /* 3 */ + TAUOFFSET;
    via2tai = rclk + word + 1;

    snapshot_module_read_byte(m, &byte);
    via2tbl = byte;
    snapshot_module_read_word(m, &word);
    via2tbu = rclk + word + 2 /* 3 */;
    via2tbi = rclk + word + 1;

    snapshot_module_read_byte(m, &byte);
    if (byte & 0x80) {
    	maincpu_set_alarm_clk(A_VIA2T1, via2tai);
    } else {
	via2tai = 0;
    }
    if (byte & 0x40) {
    	maincpu_set_alarm_clk(A_VIA2T2, via2tbi);
    } else {
	via2tbi = 0;
    }

    snapshot_module_read_byte(m, &via2[VIA_SR]);
    {
	addr = via2[VIA_SR];
	byte = via2[addr];
	
    }
    snapshot_module_read_byte(m, &via2[VIA_ACR]);
    snapshot_module_read_byte(m, &via2[VIA_PCR]);
    {
	addr = via2[VIA_PCR];
	byte = via2[addr];
	
    }

    snapshot_module_read_byte(m, &byte);
    via2ifr = byte;
    snapshot_module_read_byte(m, &byte);
    via2ier = byte;

    via_restore_int(via2ifr & via2ier & 0x7f);

    /* FIXME! */
    snapshot_module_read_byte(m, &byte);
    via2pb7 = byte ? 1 : 0;
    via2pb7x = 0;
    via2pb7o = 0;
    snapshot_module_read_byte(m, &byte);	/* SRHBITS */

    snapshot_module_read_byte(m, &byte);	/* CABSTATE */
    ca2_state = byte & 0x80;
    cb2_state = byte & 0x40;

    snapshot_module_read_byte(m, &via2_ila);
    snapshot_module_read_byte(m, &via2_ilb);

/*
printf("via2: read: clk=%d, tai=%d, tau=%d\n"
       "     : tbi=%d, tbu=%d\n",
		clk, via2tai, via2tau, via2tbi, via2tbu);
printf("     : ta=%d, tb=%d\n",via2ta() & 0xffff, via2tb() & 0xffff);
*/
    return snapshot_module_close(m);
}


#ifdef HAVE_PRINTER
void pruser_set_busy(int b)
{
    via2_signal(VIA_SIG_CB1, b ? VIA_SIG_RISE : VIA_SIG_FALL);
}
#endif
