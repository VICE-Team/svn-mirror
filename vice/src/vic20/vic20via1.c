
/*
 * ../../../src/vic20/vic20via1.c
 * This file is generated from ../../../src/via-tmpl.c and ../../../src/vic20/vic20via1.def,
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
 * into one interrupt flag, I_VIA1FL.
 * if an interrupt condition changes, the function (i.e. cpp macro)
 * update_via1irq() id called, that checks the IRQ line state.
 * This is now possible, as ettore has decoupled A_* alarm events
 * from I_* interrupts for performance reasons.
 *
 * A new function for signaling rising/falling edges on the
 * control lines is introduced:
 *      via1_signal(VIA_SIG_[CA1|CA2|CB1|CB2], VIA_SIG_[RISE|FALL])
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

#define VIA_SET_CA2(a)
#define VIA_SET_CB2(a)

#include "interrupt.h"
				/* Timer debugging */
/*#define VIA1_TIMER_DEBUG */
				/* when PB7 is really used, set this
				   to enable pulse output from the timer.
				   Otherwise PB7 state is computed only
				   when port B is read -
				   not yet implemented */
/*#define VIA1_NEED_PB7 */
				/* When you really need latching, define this.
				   It implies additional READ_PR* when
				   writing the snapshot. When latching is 
				   enabled: it reads the port when enabling,
				   and when an active C*1 transition occurs. 
				   It does not read the port when reading the
				   port register. Side-effects beware! */
/*#define VIA1_NEED_LATCHING */

/* global */

BYTE via1[16];

extern int keyarr[KBD_ROWS];

/*
 * Local variables
 */

static int via1ifr;		/* Interrupt Flag register for via1 */
static int via1ier;		/* Interrupt Enable register for via1 */

static unsigned int via1tal;	/* current timer A latch value */
static unsigned int via1tbl;	/* current timer B latch value */

static CLOCK via1tau;		/* time when via1 timer A is updated */
static CLOCK via1tbu;		/* time when via1 timer B is updated */
static CLOCK via1tai;		/* time when next timer A alarm is */
static CLOCK via1tbi;		/* time when next timer A alarm is */

static int via1pb7;		/* state of PB7 for pulse output... */
static int via1pb7x;		/* to be xored herewith  */
static int via1pb7o;		/* to be ored herewith  */
static int via1pb7xx;
static int via1pb7sx;

static BYTE oldpa;		/* the actual output on PA (input = high) */
static BYTE oldpb;		/* the actual output on PB (input = high) */

static BYTE via1_ila;		/* input latch A */
static BYTE via1_ilb;		/* input latch B */

static int ca2_state;
static int cb2_state;

/*
 * local functions
 */

#define IS_CA2_OUTPUT()          ((via1[VIA_PCR] & 0x0c) == 0x0c)
#define IS_CA2_INDINPUT()        ((via1[VIA_PCR] & 0x0a) == 0x02)
#define IS_CA2_HANDSHAKE()       ((via1[VIA_PCR] & 0x0c) == 0x08)
#define IS_CA2_PULSE_MODE()      ((via1[VIA_PCR] & 0x0e) == 0x09)
#define IS_CA2_TOGGLE_MODE()     ((via1[VIA_PCR] & 0x0e) == 0x08)

#define IS_CB2_OUTPUT()          ((via1[VIA_PCR] & 0xc0) == 0xc0)
#define IS_CB2_INDINPUT()        ((via1[VIA_PCR] & 0xa0) == 0x20)
#define IS_CB2_HANDSHAKE()       ((via1[VIA_PCR] & 0xc0) == 0x80)
#define IS_CB2_PULSE_MODE()      ((via1[VIA_PCR] & 0xe0) == 0x90)
#define IS_CB2_TOGGLE_MODE()     ((via1[VIA_PCR] & 0xe0) == 0x80)

#define	IS_PA_INPUT_LATCH()	 (via1[VIA_ACR] & 0x01)
#define	IS_PB_INPUT_LATCH()	 (via1[VIA_ACR] & 0x02)

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
 via1t*u* clk ------------------------------------------+   +--------
                                                     |
                                                     |
                                                  call of
			                        int_via1*
                                                   here

   real via1tau value = via1tau* + TAUOFFSET
   via1tbu = via1tbu* + 0

 *
 * IRQ and PB7 are set/toggled at the low-high transition of Phi2,
 * but int_* is called a half-cycle before that. Does that matter?
 *
 * PB7 output is still to be implemented
 */

/* timer values do not depend on a certain value here, but PB7 does... */
#define	TAUOFFSET	(-1)


#ifndef via_restore_int	/* if VIA reports to other chip (TPI) for IRQ */
#define	via_restore_int(a)  set_int_noclk(&maincpu_int_status, I_VIA1FL, \
		(a) ? IK_IRQ : 0)
#endif


inline static void update_via1irq(void)
{
#if 0	/* DEBUG */
    static int irq = 0;
    if(irq && !(via1ifr & via1ier & 0x7f)) {
       printf("via1: clk=%d, IRQ off\n", clk);
    }
    if(!irq && (via1ifr & via1ier & 0x7f)) {
       printf("via1: clk=%d, IRQ on\n", clk);
    }
    irq = (via1ifr & via1ier & 0x7f);
#endif
    maincpu_set_irq(I_VIA1FL, (via1ifr & via1ier & 0x7f) ? IK_IRQ : 0);
}

/* the next two are used in read_via1() */

inline static unsigned int via1ta(void)
{
    if (clk < via1tau - TAUOFFSET)
        return via1tau - TAUOFFSET - clk - 2;
    else
	return (via1tal - (clk - via1tau + TAUOFFSET) % (via1tal + 2));
}

inline static unsigned int via1tb(void)
{
    return via1tbu - clk - 2;
}

inline static void update_via1tal(CLOCK rclk)
{
    via1pb7x = 0;
    via1pb7xx = 0;

    if (rclk > via1tau) {
	int nuf = (via1tal + 1 + rclk - via1tau) / (via1tal + 2);

	if (!(via1[VIA_ACR] & 0x40)) {
	    if (((nuf - via1pb7sx) > 1) || (!via1pb7)) {
		via1pb7o = 1;
		via1pb7sx = 0;
	    }
	}
	via1pb7 ^= (nuf & 1);

	via1tau = TAUOFFSET + via1tal + 2 + (rclk - (rclk - via1tau + TAUOFFSET) % (via1tal + 2));
	if (rclk == via1tau - via1tal - 1) {
	    via1pb7xx = 1;
	}
    }

    if (via1tau == rclk)
	via1pb7x = 1;

    via1tal = via1[VIA_T1LL] + (via1[VIA_T1LH] << 8);
}

inline static void update_via1tbl(void)
{
    via1tbl = via1[VIA_T2CL] + (via1[VIA_T2CH] << 8);
}


/* ------------------------------------------------------------------------- */
/* VIA1 */



/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void reset_via1(void)
{
    int i;
#ifdef VIA1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("VIA1: reset\n");
#endif
    /* clear registers */
    for (i = 0; i < 4; i++)
	via1[i] = 0;
    for (i = 4; i < 10; i++)
        via1[i] = 0xff;        /* AB 98.08.23 */
    for (i = 11; i < 16; i++)
	via1[i] = 0;

    via1tal = 0;
    via1tbl = 0;
    via1tau = clk;
    via1tbu = clk;

    via1ier = 0;
    via1ifr = 0;

    /* disable vice interrupts */
    via1tai = 0;
    via1tbi = 0;
    maincpu_unset_alarm(A_VIA1T1);
    maincpu_unset_alarm(A_VIA1T2);
    update_via1irq();

    oldpa = 0xff;
    oldpb = 0xff;

    ca2_state = 1;
    cb2_state = 1;
    VIA_SET_CA2( ca2_state )	/* input = high */
    VIA_SET_CB2( cb2_state )	/* input = high */


	iec_pcr_write(0x22);
}

void via1_signal(int line, int edge)
{
    switch (line) {
      case VIA_SIG_CA1:
	if ( (edge ? 1 : 0) == (via1[VIA_PCR] & 0x01) ) {
	    if (IS_CA2_TOGGLE_MODE() && !ca2_state) {
		ca2_state = 1;
		VIA_SET_CA2( ca2_state )
	    }
            via1ifr |= VIA_IM_CA1;
            update_via1irq();
#ifdef VIA1_NEED_LATCHING
	    if (IS_PA_INPUT_LATCH()) {
		BYTE byte;

    {
	/* FIXME: not 100% sure about this... */
	BYTE val = ~via1[VIA_DDRA];
	BYTE msk = oldpb;
	BYTE m;
	int i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
		val &= ~rev_keyarr[i];

	byte = val | (via1[VIA_PRA] & via1[VIA_DDRA]); 
    }
		via1_ila = byte;
	    }
#endif
	}
        break;
      case VIA_SIG_CA2:
        if (!(via1[VIA_PCR] & 0x08)) {
            via1ifr |= (((edge << 2) ^ via1[VIA_PCR]) & 0x04) ?
                0 : VIA_IM_CA2;
            update_via1irq();
        }
        break;
      case VIA_SIG_CB1:
	if ( (edge ? 0x10 : 0) == (via1[VIA_PCR] & 0x10) ) {
	    if (IS_CB2_TOGGLE_MODE() && !cb2_state) {
		cb2_state = 1;
		VIA_SET_CB2( cb2_state )
	    }
            via1ifr |= VIA_IM_CB1;
            update_via1irq();
#ifdef VIA1_NEED_LATCHING
	    if (IS_PB_INPUT_LATCH()) {
		BYTE byte;

    {
	/* FIXME: not 100% sure about this... */
        BYTE val = ~via1[VIA_DDRB];
	BYTE msk = oldpa;
	int m, i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
	        val &= ~keyarr[i];

	/* Bit 7 is mapped to the right direction of the joystick (bit
	   3 in `joy[]'). */
	if ((joy[1] | joy[2]) & 0x8)
	    val &= 0x7f;

	byte = val | (via1[VIA_PRB] & via1[VIA_DDRB]) ;
    }
		via1_ilb = byte;
	    }
#endif	
	}
        break;
      case VIA_SIG_CB2:
        if (!(via1[VIA_PCR] & 0x80)) {
            via1ifr |= (((edge << 6) ^ via1[VIA_PCR]) & 0x40) ?
                0 : VIA_IM_CB2;
            update_via1irq();
        }
        break;
    }
}

void REGPARM2 store_via1(ADDRESS addr, BYTE byte)
{
    CLOCK rclk = clk - 1;	/* stores have a one-cylce offset */

    addr &= 0xf;
#ifdef VIA1_TIMER_DEBUG
    if ((addr < 10 && addr > 3) || (addr == VIA_ACR))
	printf("store via1[%x] %x, rmwf=%d, clk=%d, rclk=%d\n",
	       (int) addr, (int) byte, rmw_flag, clk, rclk);
#endif

    switch (addr) {

      /* these are done with saving the value */
      case VIA_PRA:		/* port A */
        via1ifr &= ~VIA_IM_CA1;
        if (!IS_CA2_INDINPUT()) {
            via1ifr &= ~VIA_IM_CA2;
        }
	if(IS_CA2_HANDSHAKE()) {
	    ca2_state = 0;
	    VIA_SET_CA2( ca2_state )
	    if(IS_CA2_PULSE_MODE()) {
	  	ca2_state = 1;
	    	VIA_SET_CA2( ca2_state )
	    }
	}
        update_via1irq();

      case VIA_PRA_NHS:	/* port A, no handshake */
        via1[VIA_PRA_NHS] = byte;
        addr = VIA_PRA;
      case VIA_DDRA:
	via1[addr] = byte;
	byte = via1[VIA_PRA] | ~via1[VIA_DDRA];
        
	oldpa = byte;
        break;

      case VIA_PRB:		/* port B */
        via1ifr &= ~VIA_IM_CB1;
        if ((via1[VIA_PCR] & 0xa0) != 0x20) {
            via1ifr &= ~VIA_IM_CB2;
        }
        if(IS_CB2_HANDSHAKE()) {
            cb2_state = 0;
            VIA_SET_CB2( cb2_state )
            if(IS_CB2_PULSE_MODE()) {
                cb2_state = 1;
                VIA_SET_CB2( cb2_state )
            }
        }
        update_via1irq();

      case VIA_DDRB:
	via1[addr] = byte;
	byte = via1[VIA_PRB] | ~via1[VIA_DDRB];
        
	oldpb = byte;
        break;

      case VIA_SR:		/* Serial Port output buffer */
        via1[addr] = byte;
        
            break;

        /* Timers */

      case VIA_T1CL:
      case VIA_T1LL:
        via1[VIA_T1LL] = byte;
        update_via1tal(rclk);
        break;

      case VIA_T1CH /*TIMER_AH */ :	/* Write timer A high */
#ifdef VIA1_TIMER_DEBUG
        if (app_resources.debugFlag)
            printf("Write timer A high: %02x\n", byte);
#endif
        via1[VIA_T1LH] = byte;
        update_via1tal(rclk);
        /* load counter with latch value */
        via1tau = rclk + via1tal + 3 + TAUOFFSET;
        via1tai = rclk + via1tal + 2;
        maincpu_set_alarm_clk(A_VIA1T1, via1tai);

        /* set pb7 state */
        via1pb7 = 0;
        via1pb7o = 0;

        /* Clear T1 interrupt */
        via1ifr &= ~VIA_IM_T1;
        update_via1irq();
        break;

      case VIA_T1LH:		/* Write timer A high order latch */
        via1[addr] = byte;
        update_via1tal(rclk);

        /* Clear T1 interrupt */
        via1ifr &= ~VIA_IM_T1;
        update_via1irq();
        break;

      case VIA_T2LL:		/* Write timer 2 low latch */
        via1[VIA_T2LL] = byte;
        update_via1tbl();
        
            break;

      case VIA_T2CH:		/* Write timer 2 high */
        via1[VIA_T2CH] = byte;
        update_via1tbl();
        via1tbu = rclk + via1tbl + 3;
        via1tbi = rclk + via1tbl + 2;
        maincpu_set_alarm_clk(A_VIA1T2, via1tbi);

        /* Clear T2 interrupt */
        via1ifr &= ~VIA_IM_T2;
        update_via1irq();
        break;

        /* Interrupts */

      case VIA_IFR:		/* 6522 Interrupt Flag Register */
        via1ifr &= ~byte;
        update_via1irq();
        break;

      case VIA_IER:		/* Interrupt Enable Register */
#if defined (VIA1_TIMER_DEBUG)
        printf("Via#1 set VIA_IER: 0x%x\n", byte);
#endif
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
            via1ier |= byte & 0x7f;
        } else {
            /* clear interrupts */
            via1ier &= ~byte;
        }
        update_via1irq();
        break;

        /* Control */

      case VIA_ACR:
        /* bit 7 timer 1 output to PB7 */
        update_via1tal(rclk);
        if ((via1[VIA_ACR] ^ byte) & 0x80) {
            if (byte & 0x80) {
                via1pb7 = 1 ^ via1pb7x;
            }
        }
        if ((via1[VIA_ACR] ^ byte) & 0x40) {
            via1pb7 ^= via1pb7sx;
            if ((byte & 0x40)) {
                if (via1pb7x || via1pb7xx) {
                    if (via1tal) {
                        via1pb7o = 1;
                    } else {
                        via1pb7o = 0;
                        if ((via1[VIA_ACR] & 0x80) && via1pb7x && (!via1pb7xx))
                            via1pb7 ^= 1;
                    }
                }
            }
        }
        via1pb7sx = via1pb7x;

        /* bit 1, 0  latch enable port B and A */
#ifdef VIA1_NEED_LATCHING
	/* switch on port A latching - FIXME: is this ok? */
	if ( (!(via1[addr] & 1)) && (byte & 1)) {

    {
	/* FIXME: not 100% sure about this... */
	BYTE val = ~via1[VIA_DDRA];
	BYTE msk = oldpb;
	BYTE m;
	int i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
		val &= ~rev_keyarr[i];

	byte = val | (via1[VIA_PRA] & via1[VIA_DDRA]); 
    }
	    via1_ila = byte;
	}
	/* switch on port B latching - FIXME: is this ok? */
	if ( (!(via1[addr] & 2)) && (byte & 2)) {

    {
	/* FIXME: not 100% sure about this... */
        BYTE val = ~via1[VIA_DDRB];
	BYTE msk = oldpa;
	int m, i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
	        val &= ~keyarr[i];

	/* Bit 7 is mapped to the right direction of the joystick (bit
	   3 in `joy[]'). */
	if ((joy[1] | joy[2]) & 0x8)
	    val &= 0x7f;

	byte = val | (via1[VIA_PRB] & via1[VIA_DDRB]) ;
    }
	    via1_ilb = byte;
	}
#endif

        via1[addr] = byte;

        

        /* bit 5 timer 2 count mode */
        if (byte & 32) {
            /* TODO */
            /* update_via1tb(0); *//* stop timer if mode == 1 */
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


	/* FIXME: this should use VIA_SET_CA2() and VIA_SET_CB2() */
	if(byte != via1[VIA_PCR]) {
	  register BYTE tmp = byte;
	  /* first set bit 1 and 5 to the real output values */
	  if((tmp & 0x0c) != 0x0c) tmp |= 0x02;
	  if((tmp & 0xc0) != 0xc0) tmp |= 0x20;
	  iec_pcr_write(tmp);
	}

        via1[addr] = byte;

        break;

      default:
        via1[addr] = byte;

    }				/* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_via1(ADDRESS addr)
{
#ifdef VIA1_TIMER_DEBUG
    BYTE read_via1_(ADDRESS);
    BYTE retv = read_via1_(addr);
    addr &= 0x0f;
    if ((addr > 3 && addr < 10) || app_resources.debugFlag)
	printf("read_via1(%x) -> %02x, clk=%d\n", addr, retv, clk);
    return retv;
}
BYTE REGPARM1 read_via1_(ADDRESS addr)
{
#endif
    BYTE byte = 0xff;
    CLOCK rclk = clk;

    addr &= 0xf;

    if (via1tai && (via1tai <= clk))
	int_via1t1(clk - via1tai);
    if (via1tbi && (via1tbi <= clk))
	int_via1t2(clk - via1tbi);

    switch (addr) {

      case VIA_PRA:		/* port A */
        via1ifr &= ~VIA_IM_CA1;
        if ((via1[VIA_PCR] & 0x0a) != 0x02) {
            via1ifr &= ~VIA_IM_CA2;
        }
        if(IS_CA2_HANDSHAKE()) {
            ca2_state = 0;
            VIA_SET_CA2( ca2_state )
            if(IS_CA2_PULSE_MODE()) {
                ca2_state = 1;
                VIA_SET_CA2( ca2_state )
            }
        }
        update_via1irq();

      case VIA_PRA_NHS:	/* port A, no handshake */
        /* WARNING: this pin reads the voltage of the output pins, not
           the ORA value as the other port. Value read might be different
           from what is expected due to excessive load. */
#ifdef VIA1_NEED_LATCHING
	if (IS_PA_INPUT_LATCH()) {
	    byte = via1_ila;
	} else {

    {
	/* FIXME: not 100% sure about this... */
	BYTE val = ~via1[VIA_DDRA];
	BYTE msk = oldpb;
	BYTE m;
	int i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
		val &= ~rev_keyarr[i];

	byte = val | (via1[VIA_PRA] & via1[VIA_DDRA]); 
    }
	}
#else

    {
	/* FIXME: not 100% sure about this... */
	BYTE val = ~via1[VIA_DDRA];
	BYTE msk = oldpb;
	BYTE m;
	int i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
		val &= ~rev_keyarr[i];

	byte = val | (via1[VIA_PRA] & via1[VIA_DDRA]); 
    }
#endif
	via1_ila = byte;
	return byte;

      case VIA_PRB:		/* port B */
        via1ifr &= ~VIA_IM_CB1;
        if ((via1[VIA_PCR] & 0xa0) != 0x20)
            via1ifr &= ~VIA_IM_CB2;
        update_via1irq();

        /* WARNING: this pin reads the ORA for output pins, not
           the voltage on the pins as the other port. */
#ifdef VIA1_NEED_LATCHING
	if (IS_PB_INPUT_LATCH()) {
	    byte = via1_ilb;
	} else {

    {
	/* FIXME: not 100% sure about this... */
        BYTE val = ~via1[VIA_DDRB];
	BYTE msk = oldpa;
	int m, i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
	        val &= ~keyarr[i];

	/* Bit 7 is mapped to the right direction of the joystick (bit
	   3 in `joy[]'). */
	if ((joy[1] | joy[2]) & 0x8)
	    val &= 0x7f;

	byte = val | (via1[VIA_PRB] & via1[VIA_DDRB]) ;
    }
	}
#else

    {
	/* FIXME: not 100% sure about this... */
        BYTE val = ~via1[VIA_DDRB];
	BYTE msk = oldpa;
	int m, i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
	        val &= ~keyarr[i];

	/* Bit 7 is mapped to the right direction of the joystick (bit
	   3 in `joy[]'). */
	if ((joy[1] | joy[2]) & 0x8)
	    val &= 0x7f;

	byte = val | (via1[VIA_PRB] & via1[VIA_DDRB]) ;
    }
#endif
	via1_ilb = byte;
        byte = (byte & ~via1[VIA_DDRB]) | (via1[VIA_PRB] & via1[VIA_DDRB]);

        if (via1[VIA_ACR] & 0x80) {
            update_via1tal(rclk);
            byte = (byte & 0x7f) | (((via1pb7 ^ via1pb7x) | via1pb7o) ? 0x80 : 0);
        }
        return byte;

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        via1ifr &= ~VIA_IM_T1;
        update_via1irq();
        return via1ta() & 0xff;

      case VIA_T1CH /*TIMER_AH */ :	/* timer A high */
        return (via1ta() >> 8) & 0xff;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        via1ifr &= ~VIA_IM_T2;
        update_via1irq();
        return via1tb() & 0xff;

      case VIA_T2CH /*TIMER_BH */ :	/* timer B high */
        return (via1tb() >> 8) & 0xff;

      case VIA_SR:		/* Serial Port Shift Register */
        return (via1[addr]);

        /* Interrupts */

      case VIA_IFR:		/* Interrupt Flag Register */
        {
            BYTE t = via1ifr;
            if (via1ifr & via1ier /*[VIA_IER] */ )
                t |= 0x80;
            return (t);
        }

      case VIA_IER:		/* 6522 Interrupt Control Register */
        return (via1ier /*[VIA_IER] */  | 0x80);

    }				/* switch */

    return (via1[addr]);
}

BYTE REGPARM1 peek_via1(ADDRESS addr)
{
    CLOCK rclk = clk;

    addr &= 0xf;

    if (via1tai && (via1tai <= clk))
	int_via1t1(clk - via1tai);
    if (via1tbi && (via1tbi <= clk))
	int_via1t2(clk - via1tbi);

    switch (addr) {
      case VIA_PRA:
        return read_via1(VIA_PRA_NHS);

      case VIA_PRB:		/* port B */
        {
            BYTE byte;
#ifdef VIA1_NEED_LATCHING
	    if (IS_PB_INPUT_LATCH()) {
	        byte = via1_ilb;
	    } else {

    {
	/* FIXME: not 100% sure about this... */
        BYTE val = ~via1[VIA_DDRB];
	BYTE msk = oldpa;
	int m, i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
	        val &= ~keyarr[i];

	/* Bit 7 is mapped to the right direction of the joystick (bit
	   3 in `joy[]'). */
	if ((joy[1] | joy[2]) & 0x8)
	    val &= 0x7f;

	byte = val | (via1[VIA_PRB] & via1[VIA_DDRB]) ;
    }
	    }
#else

    {
	/* FIXME: not 100% sure about this... */
        BYTE val = ~via1[VIA_DDRB];
	BYTE msk = oldpa;
	int m, i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
	        val &= ~keyarr[i];

	/* Bit 7 is mapped to the right direction of the joystick (bit
	   3 in `joy[]'). */
	if ((joy[1] | joy[2]) & 0x8)
	    val &= 0x7f;

	byte = val | (via1[VIA_PRB] & via1[VIA_DDRB]) ;
    }
#endif
            byte = (byte & ~via1[VIA_DDRB]) | (via1[VIA_PRB] & via1[VIA_DDRB]);
            if (via1[VIA_ACR] & 0x80) {
                update_via1tal(rclk);
                byte = (byte & 0x7f) | (((via1pb7 ^ via1pb7x) | via1pb7o) ? 0x80 : 0);
            }
            return byte;
        }

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        return via1ta() & 0xff;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        return via1tb() & 0xff;

      default:
        break;
    }				/* switch */

    return read_via1(addr);
}


/* ------------------------------------------------------------------------- */

int int_via1t1(long offset)
{
/*    CLOCK rclk = clk - offset; */
#ifdef VIA1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("via1 timer A interrupt\n");
#endif

    if (!(via1[VIA_ACR] & 0x40)) {	/* one-shot mode */
#if 0				/* defined (VIA1_TIMER_DEBUG) */
	printf("VIA1 Timer A interrupt -- one-shot mode: next int won't happen\n");
#endif
	maincpu_unset_alarm(A_VIA1T1);	/*int_clk[I_VIA1T1] = 0; */
	via1tai = 0;
    } else {			/* continuous mode */
	/* load counter with latch value */
	via1tai += via1tal + 2;
	maincpu_set_alarm_clk(A_VIA1T1, via1tai);
    }
    via1ifr |= VIA_IM_T1;
    update_via1irq();

    /* TODO: toggle PB7? */
    return 0;			/*(viaier & VIA_IM_T1) ? 1:0; */
}

/*
 * Timer B is always in one-shot mode
 */

int int_via1t2(long offset)
{
#ifdef VIA1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("VIA1 timer B interrupt\n");
#endif
    maincpu_unset_alarm(A_VIA1T2);	/*int_clk[I_VIA1T2] = 0; */
    via1tbi = 0;

    via1ifr |= VIA_IM_T2;
    update_via1irq();

    return 0;
}

void via1_prevent_clk_overflow(CLOCK sub)
{
    unsigned int t;
    t = (via1tau - (clk + sub)) & 0xffff;
    via1tau = clk + t;
    t = (via1tbu - (clk + sub)) & 0xffff;
    via1tbu = clk + t;
    if (via1tai)
	via1tai -= sub;
}

/*------------------------------------------------------------------------*/

/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 0/0
 */

static char snap_module_name[] = "VIA1";
#define VIA_DUMP_VER_MAJOR      0
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

int via1_write_snapshot_module(snapshot_t * p)
{
    snapshot_module_t *m;

    if (via1tai && (via1tai <= clk))
        int_via1t1(clk - via1tai);
    if (via1tbi && (via1tbi <= clk))
        int_via1t2(clk - via1tbi);

    m = snapshot_module_create(p, snap_module_name,
                               VIA_DUMP_VER_MAJOR, VIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;
/*
printf("via1: write: clk=%d, tai=%d, tau=%d\n"
       "     : tbi=%d, tbu=%d\n",
		clk, via1tai, via1tau, via1tbi, via1tbu);
printf("     : ta=%d, tb=%d\n",via1ta() & 0xffff, via1tb() & 0xffff);
*/
    snapshot_module_write_byte(m, via1[VIA_PRA]);
    snapshot_module_write_byte(m, via1[VIA_DDRA]);
    snapshot_module_write_byte(m, via1[VIA_PRB]);
    snapshot_module_write_byte(m, via1[VIA_DDRB]);

    snapshot_module_write_word(m, via1tal);
    snapshot_module_write_word(m, via1ta());
    snapshot_module_write_byte(m, via1tbl);
    snapshot_module_write_word(m, via1tb());

    snapshot_module_write_byte(m, (via1tai ? 0x80 : 0)
					| (via1tbi ? 0x40 : 0) );

    snapshot_module_write_byte(m, via1[VIA_SR]);
    snapshot_module_write_byte(m, via1[VIA_ACR]);
    snapshot_module_write_byte(m, via1[VIA_PCR]);

    snapshot_module_write_byte(m, via1ifr);
    snapshot_module_write_byte(m, via1ier);

						/* FIXME! */
    snapshot_module_write_byte(m, (((via1pb7 ^ via1pb7x) | via1pb7o) ? 0x80 : 0));
    snapshot_module_write_byte(m, 0);		/* SRHBITS */

    snapshot_module_write_byte(m, (ca2_state ? 0x80 : 0) 
				| (cb2_state ? 0x40 : 0));

    snapshot_module_write_byte(m, via1_ila);
    snapshot_module_write_byte(m, via1_ilb);

    snapshot_module_close(m);

    return 0;
}

int via1_read_snapshot_module(snapshot_t * p)
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

    maincpu_unset_alarm(A_VIA1T1);
    maincpu_unset_alarm(A_VIA1T2);
    via1tai = 0;
    via1tbi = 0;

    snapshot_module_read_byte(m, &via1[VIA_PRA]);
    snapshot_module_read_byte(m, &via1[VIA_DDRA]);
    snapshot_module_read_byte(m, &via1[VIA_PRB]);
    snapshot_module_read_byte(m, &via1[VIA_DDRB]);
    {
        addr = VIA_DDRA;
	byte = via1[VIA_PRA] | ~via1[VIA_DDRA];
	
	oldpa = byte;

	addr = VIA_DDRB;
	byte = via1[VIA_PRB] | ~via1[VIA_DDRB];
	
	oldpb = byte;
    }

    snapshot_module_read_word(m, &word);
    via1tal = word;
    snapshot_module_read_word(m, &word);
    via1tau = rclk + word + 2 /* 3 */ + TAUOFFSET;
    via1tai = rclk + word + 1;

    snapshot_module_read_byte(m, &byte);
    via1tbl = byte;
    snapshot_module_read_word(m, &word);
    via1tbu = rclk + word + 2 /* 3 */;
    via1tbi = rclk + word + 1;

    snapshot_module_read_byte(m, &byte);
    if (byte & 0x80) {
    	maincpu_set_alarm_clk(A_VIA1T1, via1tai);
    } else {
	via1tai = 0;
    }
    if (byte & 0x40) {
    	maincpu_set_alarm_clk(A_VIA1T2, via1tbi);
    } else {
	via1tbi = 0;
    }

    snapshot_module_read_byte(m, &via1[VIA_SR]);
    {
	addr = via1[VIA_SR];
	byte = via1[addr];
	
    }
    snapshot_module_read_byte(m, &via1[VIA_ACR]);
    snapshot_module_read_byte(m, &via1[VIA_PCR]);
    {
	addr = via1[VIA_PCR];
	byte = via1[addr];
	
    }

    snapshot_module_read_byte(m, &byte);
    via1ifr = byte;
    snapshot_module_read_byte(m, &byte);
    via1ier = byte;

    via_restore_int(via1ifr & via1ier & 0x7f);

    /* FIXME! */
    snapshot_module_read_byte(m, &byte);
    via1pb7 = byte ? 1 : 0;
    via1pb7x = 0;
    via1pb7o = 0;
    snapshot_module_read_byte(m, &byte);	/* SRHBITS */

    snapshot_module_read_byte(m, &byte);	/* CABSTATE */
    ca2_state = byte & 0x80;
    cb2_state = byte & 0x40;

    snapshot_module_read_byte(m, &via1_ila);
    snapshot_module_read_byte(m, &via1_ilb);

/*
printf("via1: read: clk=%d, tai=%d, tau=%d\n"
       "     : tbi=%d, tbu=%d\n",
		clk, via1tai, via1tau, via1tbi, via1tbu);
printf("     : ta=%d, tb=%d\n",via1ta() & 0xffff, via1tb() & 0xffff);
*/
    return snapshot_module_close(m);
}



int show_keyarr(void)
{
    int     i, j;
    for (j = 0; j < KBD_ROWS; j++) {
        printf("%d:", j);
        for (i = 0x80; i; i >>= 1)
            printf(" %d", keyarr[j] & i);
        printf("\n");
    }
    return (0);
}

