
/*
 * ../../../src/drive/via2drive1.c
 * This file is generated from ../../../src/via-tmpl.c and ../../../src/drive/via2drive1.def,
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
 * into one interrupt flag, I_VIA2D1FL.
 * if an interrupt condition changes, the function (i.e. cpp macro)
 * update_via2d1irq() id called, that checks the IRQ line state.
 * This is now possible, as ettore has decoupled A_* alarm events
 * from I_* interrupts for performance reasons.
 *
 * A new function for signaling rising/falling edges on the
 * control lines is introduced:
 *      via2d1_signal(VIA_SIG_[CA1|CA2|CB1|CB2], VIA_SIG_[RISE|FALL])
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

#include "log.h"
#include "resources.h"
#include "snapshot.h"
#include "via.h"
#include "vmachine.h"


#include "drive.h"
#include "viad.h"

#define VIA_SET_CA2(a)
#define VIA_SET_CB2(a)

#include "interrupt.h"
				/* Timer debugging */
/*#define VIA2D1_TIMER_DEBUG */
				/* when PB7 is really used, set this
				   to enable pulse output from the timer.
				   Otherwise PB7 state is computed only
				   when port B is read -
				   not yet implemented */
/*#define VIA2D1_NEED_PB7 */
				/* When you really need latching, define this.
				   It implies additional READ_PR* when
				   writing the snapshot. When latching is 
				   enabled: it reads the port when enabling,
				   and when an active C*1 transition occurs. 
				   It does not read the port when reading the
				   port register. Side-effects beware! */
/*#define VIA2D1_NEED_LATCHING */

/* global */

BYTE via2d1[16];



/*
 * Local variables
 */

static int via2d1ifr;		/* Interrupt Flag register for via2d1 */
static int via2d1ier;		/* Interrupt Enable register for via2d1 */

static unsigned int via2d1tal;	/* current timer A latch value */
static unsigned int via2d1tbl;	/* current timer B latch value */

static CLOCK via2d1tau;		/* time when via2d1 timer A is updated */
static CLOCK via2d1tbu;		/* time when via2d1 timer B is updated */
static CLOCK via2d1tai;		/* time when next timer A alarm is */
static CLOCK via2d1tbi;		/* time when next timer A alarm is */

static int via2d1pb7;		/* state of PB7 for pulse output... */
static int via2d1pb7x;		/* to be xored herewith  */
static int via2d1pb7o;		/* to be ored herewith  */
static int via2d1pb7xx;
static int via2d1pb7sx;

static BYTE oldpa;		/* the actual output on PA (input = high) */
static BYTE oldpb;		/* the actual output on PB (input = high) */

static BYTE via2d1_ila;		/* input latch A */
static BYTE via2d1_ilb;		/* input latch B */

static int ca2_state;
static int cb2_state;

static log_t via2d1_log = LOG_ERR;

/*
 * local functions
 */

#define IS_CA2_OUTPUT()          ((via2d1[VIA_PCR] & 0x0c) == 0x0c)
#define IS_CA2_INDINPUT()        ((via2d1[VIA_PCR] & 0x0a) == 0x02)
#define IS_CA2_HANDSHAKE()       ((via2d1[VIA_PCR] & 0x0c) == 0x08)
#define IS_CA2_PULSE_MODE()      ((via2d1[VIA_PCR] & 0x0e) == 0x09)
#define IS_CA2_TOGGLE_MODE()     ((via2d1[VIA_PCR] & 0x0e) == 0x08)

#define IS_CB2_OUTPUT()          ((via2d1[VIA_PCR] & 0xc0) == 0xc0)
#define IS_CB2_INDINPUT()        ((via2d1[VIA_PCR] & 0xa0) == 0x20)
#define IS_CB2_HANDSHAKE()       ((via2d1[VIA_PCR] & 0xc0) == 0x80)
#define IS_CB2_PULSE_MODE()      ((via2d1[VIA_PCR] & 0xe0) == 0x90)
#define IS_CB2_TOGGLE_MODE()     ((via2d1[VIA_PCR] & 0xe0) == 0x80)

#define	IS_PA_INPUT_LATCH()	 (via2d1[VIA_ACR] & 0x01)
#define	IS_PB_INPUT_LATCH()	 (via2d1[VIA_ACR] & 0x02)

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
 via2d1t*u* clk ------------------------------------------+   +--------
                                                     |
                                                     |
                                                  call of
			                        int_via2d1*
                                                   here

   real via2d1tau value = via2d1tau* + TAUOFFSET
   via2d1tbu = via2d1tbu* + 0

 *
 * IRQ and PB7 are set/toggled at the low-high transition of Phi2,
 * but int_* is called a half-cycle before that. Does that matter?
 *
 * PB7 output is still to be implemented
 */

/* timer values do not depend on a certain value here, but PB7 does... */
#define	TAUOFFSET	(-1)


#ifndef via_restore_int	/* if VIA reports to other chip (TPI) for IRQ */
#define	via_restore_int(a)  set_int_noclk(&drive1_int_status, I_VIA2D1FL, \
		(a) ? IK_IRQ : 0)
#endif


inline static void update_via2d1irq(void)
{
    drive1_set_irq(I_VIA2D1FL, (via2d1ifr & via2d1ier & 0x7f) ? IK_IRQ : 0);
}

/* the next two are used in read_via2d1() */

inline static unsigned int via2d1ta(void)
{
    if (drive_clk[1] < via2d1tau - TAUOFFSET)
        return via2d1tau - TAUOFFSET - drive_clk[1] - 2;
    else
	return (via2d1tal - (drive_clk[1] - via2d1tau + TAUOFFSET) % (via2d1tal + 2));
}

inline static unsigned int via2d1tb(void)
{
    return via2d1tbu - drive_clk[1] - 2;
}

inline static void update_via2d1tal(CLOCK rclk)
{
    via2d1pb7x = 0;
    via2d1pb7xx = 0;

    if (rclk > via2d1tau) {
	int nuf = (via2d1tal + 1 + rclk - via2d1tau) / (via2d1tal + 2);

	if (!(via2d1[VIA_ACR] & 0x40)) {
	    if (((nuf - via2d1pb7sx) > 1) || (!via2d1pb7)) {
		via2d1pb7o = 1;
		via2d1pb7sx = 0;
	    }
	}
	via2d1pb7 ^= (nuf & 1);

	via2d1tau = TAUOFFSET + via2d1tal + 2 + (rclk - (rclk - via2d1tau + TAUOFFSET) % (via2d1tal + 2));
	if (rclk == via2d1tau - via2d1tal - 1) {
	    via2d1pb7xx = 1;
	}
    }

    if (via2d1tau == rclk)
	via2d1pb7x = 1;

    via2d1tal = via2d1[VIA_T1LL] + (via2d1[VIA_T1LH] << 8);
}

inline static void update_via2d1tbl(void)
{
    via2d1tbl = via2d1[VIA_T2CL] + (via2d1[VIA_T2CH] << 8);
}


/* ------------------------------------------------------------------------- */
/* VIA2D1 */



/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void reset_via2d1(void)
{
    int i;

    if (via2d1_log == LOG_ERR)
        via2d1_log = log_open("VIA2D1");

    /* clear registers */
    for (i = 0; i < 4; i++)
	via2d1[i] = 0;
    for (i = 4; i < 10; i++)
        via2d1[i] = 0xff;        /* AB 98.08.23 */
    for (i = 11; i < 16; i++)
	via2d1[i] = 0;

    via2d1tal = 0;
    via2d1tbl = 0;
    via2d1tau = drive_clk[1];
    via2d1tbu = drive_clk[1];

    via2d1ier = 0;
    via2d1ifr = 0;

    /* disable vice interrupts */
    via2d1tai = 0;
    via2d1tbi = 0;
    drive1_unset_alarm(A_VIA2D1T1);
    drive1_unset_alarm(A_VIA2D1T2);
    update_via2d1irq();

    oldpa = 0xff;
    oldpb = 0xff;

    ca2_state = 1;
    cb2_state = 1;
    VIA_SET_CA2( ca2_state )	/* input = high */
    VIA_SET_CB2( cb2_state )	/* input = high */


    drive[1].led_status = 8;
    drive_update_ui_status();
}

void via2d1_signal(int line, int edge)
{
    switch (line) {
      case VIA_SIG_CA1:
	if ( (edge ? 1 : 0) == (via2d1[VIA_PCR] & 0x01) ) {
	    if (IS_CA2_TOGGLE_MODE() && !ca2_state) {
		ca2_state = 1;
		VIA_SET_CA2( ca2_state )
	    }
            via2d1ifr |= VIA_IM_CA1;
            update_via2d1irq();
#ifdef VIA2D1_NEED_LATCHING
	    if (IS_PA_INPUT_LATCH()) {
		BYTE byte;

    if (drive[1].attach_clk != (CLOCK)0) {
        if (drive_clk[1] - drive[1].attach_clk < DRIVE_ATTACH_DELAY)
            drive[1].GCR_read = 0;
        drive[1].attach_clk = (CLOCK)0;
    } else {
        if (drive[1].byte_ready_active == 0x06)
            drive_rotate_disk(&drive[1]);
    }
    byte = ((drive[1].GCR_read & ~via2d1[VIA_DDRA])
        | (via2d1[VIA_PRA] & via2d1[VIA_DDRA] ));
    if (drive[1].type == DRIVE_TYPE_1571)
        if (drive[1].byte_ready)
            drive[1].byte_ready = 0;
		via2d1_ila = byte;
	    }
#endif
	}
        break;
      case VIA_SIG_CA2:
        if (!(via2d1[VIA_PCR] & 0x08)) {
            via2d1ifr |= (((edge << 2) ^ via2d1[VIA_PCR]) & 0x04) ?
                0 : VIA_IM_CA2;
            update_via2d1irq();
        }
        break;
      case VIA_SIG_CB1:
	if ( (edge ? 0x10 : 0) == (via2d1[VIA_PCR] & 0x10) ) {
	    if (IS_CB2_TOGGLE_MODE() && !cb2_state) {
		cb2_state = 1;
		VIA_SET_CB2( cb2_state )
	    }
            via2d1ifr |= VIA_IM_CB1;
            update_via2d1irq();
#ifdef VIA2D1_NEED_LATCHING
	    if (IS_PB_INPUT_LATCH()) {
		BYTE byte;

	byte = (drive_read_viad2_prb(&drive[1]) & ~via2d1[VIA_DDRB])
			| (via2d1[VIA_PRB] & via2d1[VIA_DDRB] );
		via2d1_ilb = byte;
	    }
#endif	
	}
        break;
      case VIA_SIG_CB2:
        if (!(via2d1[VIA_PCR] & 0x80)) {
            via2d1ifr |= (((edge << 6) ^ via2d1[VIA_PCR]) & 0x40) ?
                0 : VIA_IM_CB2;
            update_via2d1irq();
        }
        break;
    }
}

void REGPARM2 store_via2d1(ADDRESS addr, BYTE byte)
{
    CLOCK rclk = drive_clk[1] - 1;	/* stores have a one-cylce offset */

    addr &= 0xf;

    switch (addr) {

      /* these are done with saving the value */
      case VIA_PRA:		/* port A */
        via2d1ifr &= ~VIA_IM_CA1;
        if (!IS_CA2_INDINPUT()) {
            via2d1ifr &= ~VIA_IM_CA2;
        }
	if(IS_CA2_HANDSHAKE()) {
	    ca2_state = 0;
	    VIA_SET_CA2( ca2_state )
	    if(IS_CA2_PULSE_MODE()) {
	  	ca2_state = 1;
	    	VIA_SET_CA2( ca2_state )
	    }
	}
	if (via2d1ier & (VIA_IM_CA1 | VIA_IM_CA2))
            update_via2d1irq();

      case VIA_PRA_NHS:	/* port A, no handshake */
        via2d1[VIA_PRA_NHS] = byte;
        addr = VIA_PRA;
      case VIA_DDRA:
	via2d1[addr] = byte;
	byte = via2d1[VIA_PRA] | ~via2d1[VIA_DDRA];

    if (drive[1].byte_ready_active == 0x06)
        drive_rotate_disk(&drive[1]);
    drive[1].GCR_write_value = byte;
	oldpa = byte;
        break;

      case VIA_PRB:		/* port B */
        via2d1ifr &= ~VIA_IM_CB1;
        if ((via2d1[VIA_PCR] & 0xa0) != 0x20) {
            via2d1ifr &= ~VIA_IM_CB2;
        }
        if(IS_CB2_HANDSHAKE()) {
            cb2_state = 0;
            VIA_SET_CB2( cb2_state )
            if(IS_CB2_PULSE_MODE()) {
                cb2_state = 1;
                VIA_SET_CB2( cb2_state )
            }
        }
	if (via2d1ier & (VIA_IM_CB1 | VIA_IM_CB2))
            update_via2d1irq();

      case VIA_DDRB:
	via2d1[addr] = byte;
	byte = via2d1[VIA_PRB] | ~via2d1[VIA_DDRB];

    drive[1].led_status = byte & 8;
    if (((oldpb ^ byte) & 0x3) && (byte & 0x4)) {
        /* Stepper motor */
        if ((oldpb & 0x3) == ((byte + 1) & 0x3))
            drive_move_head(-1, 1);
        else if ((oldpb & 0x3) == ((byte - 1) & 0x3))
            drive_move_head(+1, 1);
    }
    if ((oldpb ^ byte) & 0x60)     /* Zone bits */
        drive[1].rotation_table_ptr = drive[1].rotation_table[(byte >> 5) & 0x3];
    if ((oldpb ^ byte) & 0x04)     /* Motor on/off */
        drive[1].byte_ready_active = (drive[1].byte_ready_active & ~0x04)
                                     | (byte & 0x04);
	oldpb = byte;
        break;

      case VIA_SR:		/* Serial Port output buffer */
        via2d1[addr] = byte;
        
            break;

        /* Timers */

      case VIA_T1CL:
      case VIA_T1LL:
        via2d1[VIA_T1LL] = byte;
        update_via2d1tal(rclk);
        break;

      case VIA_T1CH:	/* Write timer A high */
        via2d1[VIA_T1LH] = byte;
        update_via2d1tal(rclk);
        /* load counter with latch value */
        via2d1tau = rclk + via2d1tal + 3 + TAUOFFSET;
        via2d1tai = rclk + via2d1tal + 2;
        drive1_set_alarm_clk(A_VIA2D1T1, via2d1tai);

        /* set pb7 state */
        via2d1pb7 = 0;
        via2d1pb7o = 0;

        /* Clear T1 interrupt */
        via2d1ifr &= ~VIA_IM_T1;
        update_via2d1irq();
        break;

      case VIA_T1LH:		/* Write timer A high order latch */
        via2d1[addr] = byte;
        update_via2d1tal(rclk);

        /* Clear T1 interrupt */
        via2d1ifr &= ~VIA_IM_T1;
        update_via2d1irq();
        break;

      case VIA_T2LL:		/* Write timer 2 low latch */
        via2d1[VIA_T2LL] = byte;
        update_via2d1tbl();
        
            break;

      case VIA_T2CH:		/* Write timer 2 high */
        via2d1[VIA_T2CH] = byte;
        update_via2d1tbl();
        via2d1tbu = rclk + via2d1tbl + 3;
        via2d1tbi = rclk + via2d1tbl + 2;
        drive1_set_alarm_clk(A_VIA2D1T2, via2d1tbi);

        /* Clear T2 interrupt */
        via2d1ifr &= ~VIA_IM_T2;
        update_via2d1irq();
        break;

        /* Interrupts */

      case VIA_IFR:		/* 6522 Interrupt Flag Register */
        via2d1ifr &= ~byte;
        update_via2d1irq();
        break;

      case VIA_IER:		/* Interrupt Enable Register */
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
            via2d1ier |= byte & 0x7f;
        } else {
            /* clear interrupts */
            via2d1ier &= ~byte;
        }
        update_via2d1irq();
        break;

        /* Control */

      case VIA_ACR:
        /* bit 7 timer 1 output to PB7 */
        update_via2d1tal(rclk);
        if ((via2d1[VIA_ACR] ^ byte) & 0x80) {
            if (byte & 0x80) {
                via2d1pb7 = 1 ^ via2d1pb7x;
            }
        }
        if ((via2d1[VIA_ACR] ^ byte) & 0x40) {
            via2d1pb7 ^= via2d1pb7sx;
            if ((byte & 0x40)) {
                if (via2d1pb7x || via2d1pb7xx) {
                    if (via2d1tal) {
                        via2d1pb7o = 1;
                    } else {
                        via2d1pb7o = 0;
                        if ((via2d1[VIA_ACR] & 0x80) && via2d1pb7x && (!via2d1pb7xx))
                            via2d1pb7 ^= 1;
                    }
                }
            }
        }
        via2d1pb7sx = via2d1pb7x;

        /* bit 1, 0  latch enable port B and A */
#ifdef VIA2D1_NEED_LATCHING
	/* switch on port A latching - FIXME: is this ok? */
	if ( (!(via2d1[addr] & 1)) && (byte & 1)) {

    if (drive[1].attach_clk != (CLOCK)0) {
        if (drive_clk[1] - drive[1].attach_clk < DRIVE_ATTACH_DELAY)
            drive[1].GCR_read = 0;
        drive[1].attach_clk = (CLOCK)0;
    } else {
        if (drive[1].byte_ready_active == 0x06)
            drive_rotate_disk(&drive[1]);
    }
    byte = ((drive[1].GCR_read & ~via2d1[VIA_DDRA])
        | (via2d1[VIA_PRA] & via2d1[VIA_DDRA] ));
    if (drive[1].type == DRIVE_TYPE_1571)
        if (drive[1].byte_ready)
            drive[1].byte_ready = 0;
	    via2d1_ila = byte;
	}
	/* switch on port B latching - FIXME: is this ok? */
	if ( (!(via2d1[addr] & 2)) && (byte & 2)) {

	byte = (drive_read_viad2_prb(&drive[1]) & ~via2d1[VIA_DDRB])
			| (via2d1[VIA_PRB] & via2d1[VIA_DDRB] );
	    via2d1_ilb = byte;
	}
#endif

        via2d1[addr] = byte;

        

        /* bit 5 timer 2 count mode */
        if (byte & 32) {
            /* TODO */
            /* update_via2d1tb(0); *//* stop timer if mode == 1 */
        }

        /* bit 4, 3, 2 shift register control */

        break;

      case VIA_PCR:

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
    if(byte != via2d1[VIA_PCR]) {
        register BYTE tmp = byte;
        /* first set bit 1 and 5 to the real output values */
        if((tmp & 0x0c) != 0x0c)
            tmp |= 0x02;
        if((tmp & 0xc0) != 0xc0)
            tmp |= 0x20;
        /* insert_your_favourite_drive_function_here(tmp);
        bit 5 is the write output to the analog circuitry:
        0 = writing, 0x20 = reading */
        drive_update_viad2_pcr(tmp, &drive[1]);
        if ((byte & 0x20) != (via2d1[addr] & 0x20)) {
            if (drive[1].byte_ready_active == 0x06)
                drive_rotate_disk(&drive[1]);
            drive[1].finish_byte = 1;
        }
        byte = tmp;
    }

        via2d1[addr] = byte;

        break;

      default:
        via2d1[addr] = byte;

    }				/* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_via2d1(ADDRESS addr)
{
#ifdef VIA2D1_TIMER_DEBUG
    BYTE read_via2d1_(ADDRESS);
    BYTE retv = read_via2d1_(addr);
    addr &= 0x0f;
    if ((addr > 3 && addr < 10) || app_resources.debugFlag)
	log_message(via2d1_log,
                    "read_via2d1(%x) -> %02x, clk=%d", addr, retv, drive_clk[1]);
    return retv;
}
BYTE REGPARM1 read_via2d1_(ADDRESS addr)
{
#endif
    BYTE byte = 0xff;
    CLOCK rclk = drive_clk[1];

    addr &= 0xf;

    if (addr >= VIA_T1CL && addr <= VIA_IER) { 
        if (via2d1tai && (via2d1tai <= drive_clk[1]))
	    int_via2d1t1(drive_clk[1] - via2d1tai);
        if (via2d1tbi && (via2d1tbi <= drive_clk[1]))
	    int_via2d1t2(drive_clk[1] - via2d1tbi);
    }

    switch (addr) {

      case VIA_PRA:		/* port A */
        via2d1ifr &= ~VIA_IM_CA1;
        if ((via2d1[VIA_PCR] & 0x0a) != 0x02) {
            via2d1ifr &= ~VIA_IM_CA2;
        }
        if(IS_CA2_HANDSHAKE()) {
            ca2_state = 0;
            VIA_SET_CA2( ca2_state )
            if(IS_CA2_PULSE_MODE()) {
                ca2_state = 1;
                VIA_SET_CA2( ca2_state )
            }
        }
        if (via2d1ier & (VIA_IM_CA1 | VIA_IM_CA2)) 
	    update_via2d1irq();

      case VIA_PRA_NHS:	/* port A, no handshake */
        /* WARNING: this pin reads the voltage of the output pins, not
           the ORA value as the other port. Value read might be different
           from what is expected due to excessive load. */
#ifdef VIA2D1_NEED_LATCHING
	if (IS_PA_INPUT_LATCH()) {
	    byte = via2d1_ila;
	} else {

    if (drive[1].attach_clk != (CLOCK)0) {
        if (drive_clk[1] - drive[1].attach_clk < DRIVE_ATTACH_DELAY)
            drive[1].GCR_read = 0;
        drive[1].attach_clk = (CLOCK)0;
    } else {
        if (drive[1].byte_ready_active == 0x06)
            drive_rotate_disk(&drive[1]);
    }
    byte = ((drive[1].GCR_read & ~via2d1[VIA_DDRA])
        | (via2d1[VIA_PRA] & via2d1[VIA_DDRA] ));
    if (drive[1].type == DRIVE_TYPE_1571)
        if (drive[1].byte_ready)
            drive[1].byte_ready = 0;
	}
#else

    if (drive[1].attach_clk != (CLOCK)0) {
        if (drive_clk[1] - drive[1].attach_clk < DRIVE_ATTACH_DELAY)
            drive[1].GCR_read = 0;
        drive[1].attach_clk = (CLOCK)0;
    } else {
        if (drive[1].byte_ready_active == 0x06)
            drive_rotate_disk(&drive[1]);
    }
    byte = ((drive[1].GCR_read & ~via2d1[VIA_DDRA])
        | (via2d1[VIA_PRA] & via2d1[VIA_DDRA] ));
    if (drive[1].type == DRIVE_TYPE_1571)
        if (drive[1].byte_ready)
            drive[1].byte_ready = 0;
#endif
	via2d1_ila = byte;
	return byte;

      case VIA_PRB:		/* port B */
        via2d1ifr &= ~VIA_IM_CB1;
        if ((via2d1[VIA_PCR] & 0xa0) != 0x20)
            via2d1ifr &= ~VIA_IM_CB2;
        if (via2d1ier & (VIA_IM_CB1 | VIA_IM_CB2)) 
	    update_via2d1irq();

        /* WARNING: this pin reads the ORA for output pins, not
           the voltage on the pins as the other port. */
#ifdef VIA2D1_NEED_LATCHING
	if (IS_PB_INPUT_LATCH()) {
	    byte = via2d1_ilb;
	} else {

	byte = (drive_read_viad2_prb(&drive[1]) & ~via2d1[VIA_DDRB])
			| (via2d1[VIA_PRB] & via2d1[VIA_DDRB] );
	}
#else

	byte = (drive_read_viad2_prb(&drive[1]) & ~via2d1[VIA_DDRB])
			| (via2d1[VIA_PRB] & via2d1[VIA_DDRB] );
#endif
	via2d1_ilb = byte;
        byte = (byte & ~via2d1[VIA_DDRB]) | (via2d1[VIA_PRB] & via2d1[VIA_DDRB]);

        if (via2d1[VIA_ACR] & 0x80) {
            update_via2d1tal(rclk);
            byte = (byte & 0x7f) | (((via2d1pb7 ^ via2d1pb7x) | via2d1pb7o) ? 0x80 : 0);
        }
        return byte;

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        via2d1ifr &= ~VIA_IM_T1;
        update_via2d1irq();
        return via2d1ta() & 0xff;

      case VIA_T1CH /*TIMER_AH */ :	/* timer A high */
        return (via2d1ta() >> 8) & 0xff;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        via2d1ifr &= ~VIA_IM_T2;
        update_via2d1irq();
        return via2d1tb() & 0xff;

      case VIA_T2CH /*TIMER_BH */ :	/* timer B high */
        return (via2d1tb() >> 8) & 0xff;

      case VIA_SR:		/* Serial Port Shift Register */
        return (via2d1[addr]);

        /* Interrupts */

      case VIA_IFR:		/* Interrupt Flag Register */
        {
            BYTE t = via2d1ifr;
            if (via2d1ifr & via2d1ier /*[VIA_IER] */ )
                t |= 0x80;
            return (t);
        }

      case VIA_IER:		/* 6522 Interrupt Control Register */
        return (via2d1ier /*[VIA_IER] */  | 0x80);

    }				/* switch */

    return (via2d1[addr]);
}

BYTE REGPARM1 peek_via2d1(ADDRESS addr)
{
    CLOCK rclk = drive_clk[1];

    addr &= 0xf;

    if (via2d1tai && (via2d1tai <= drive_clk[1]))
	int_via2d1t1(drive_clk[1] - via2d1tai);
    if (via2d1tbi && (via2d1tbi <= drive_clk[1]))
	int_via2d1t2(drive_clk[1] - via2d1tbi);

    switch (addr) {
      case VIA_PRA:
        return read_via2d1(VIA_PRA_NHS);

      case VIA_PRB:		/* port B */
        {
            BYTE byte;
#ifdef VIA2D1_NEED_LATCHING
	    if (IS_PB_INPUT_LATCH()) {
	        byte = via2d1_ilb;
	    } else {

	byte = (drive_read_viad2_prb(&drive[1]) & ~via2d1[VIA_DDRB])
			| (via2d1[VIA_PRB] & via2d1[VIA_DDRB] );
	    }
#else

	byte = (drive_read_viad2_prb(&drive[1]) & ~via2d1[VIA_DDRB])
			| (via2d1[VIA_PRB] & via2d1[VIA_DDRB] );
#endif
            byte = (byte & ~via2d1[VIA_DDRB]) | (via2d1[VIA_PRB] & via2d1[VIA_DDRB]);
            if (via2d1[VIA_ACR] & 0x80) {
                update_via2d1tal(rclk);
                byte = (byte & 0x7f) | (((via2d1pb7 ^ via2d1pb7x) | via2d1pb7o) ? 0x80 : 0);
            }
            return byte;
        }

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        return via2d1ta() & 0xff;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        return via2d1tb() & 0xff;

      default:
        break;
    }				/* switch */

    return read_via2d1(addr);
}


/* ------------------------------------------------------------------------- */

int int_via2d1t1(long offset)
{
#ifdef VIA2D1_TIMER_DEBUG
    if (app_resources.debugFlag)
	log_message(via2d1_log, "via2d1 timer A interrupt");
#endif

    if (!(via2d1[VIA_ACR] & 0x40)) {	/* one-shot mode */
#ifdef VIA2D1_TIMER_DEBUG
	log_message(via2d1_log, "VIA2D1 Timer A interrupt -- one-shot mode: next int won't happen");
#endif
	drive1_unset_alarm(A_VIA2D1T1);	/*int_clk[I_VIA2D1T1] = 0; */
	via2d1tai = 0;
    } else {			/* continuous mode */
	/* load counter with latch value */
	via2d1tai += via2d1tal + 2;
	drive1_set_alarm_clk(A_VIA2D1T1, via2d1tai);
    }
    via2d1ifr |= VIA_IM_T1;
    update_via2d1irq();

    /* TODO: toggle PB7? */
    return 0;			/*(viaier & VIA_IM_T1) ? 1:0; */
}

/*
 * Timer B is always in one-shot mode
 */

int int_via2d1t2(long offset)
{
#ifdef VIA2D1_TIMER_DEBUG
    if (app_resources.debugFlag)
	log_message(via2d1_log, "VIA2D1 timer B interrupt.");
#endif
    drive1_unset_alarm(A_VIA2D1T2);	/*int_clk[I_VIA2D1T2] = 0; */
    via2d1tbi = 0;

    via2d1ifr |= VIA_IM_T2;
    update_via2d1irq();

    return 0;
}

void via2d1_prevent_clk_overflow(CLOCK sub)
{
    unsigned int t;
    t = (via2d1tau - (drive_clk[1] + sub)) & 0xffff;
    via2d1tau = drive_clk[1] + t;
    t = (via2d1tbu - (drive_clk[1] + sub)) & 0xffff;
    via2d1tbu = drive_clk[1] + t;
    if (via2d1tai)
	via2d1tai -= sub;
}

/*------------------------------------------------------------------------*/

/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 0/0
 */

static char snap_module_name[] = "VIA2D1";
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

int via2d1_write_snapshot_module(snapshot_t * p)
{
    snapshot_module_t *m;

    if (via2d1tai && (via2d1tai <= drive_clk[1]))
        int_via2d1t1(drive_clk[1] - via2d1tai);
    if (via2d1tbi && (via2d1tbi <= drive_clk[1]))
        int_via2d1t2(drive_clk[1] - via2d1tbi);

    m = snapshot_module_create(p, snap_module_name,
                               VIA_DUMP_VER_MAJOR, VIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    snapshot_module_write_byte(m, via2d1[VIA_PRA]);
    snapshot_module_write_byte(m, via2d1[VIA_DDRA]);
    snapshot_module_write_byte(m, via2d1[VIA_PRB]);
    snapshot_module_write_byte(m, via2d1[VIA_DDRB]);

    snapshot_module_write_word(m, via2d1tal);
    snapshot_module_write_word(m, via2d1ta());
    snapshot_module_write_byte(m, via2d1[VIA_T2LL]);
    snapshot_module_write_word(m, via2d1tb());

    snapshot_module_write_byte(m, (via2d1tai ? 0x80 : 0)
					| (via2d1tbi ? 0x40 : 0) );

    snapshot_module_write_byte(m, via2d1[VIA_SR]);
    snapshot_module_write_byte(m, via2d1[VIA_ACR]);
    snapshot_module_write_byte(m, via2d1[VIA_PCR]);

    snapshot_module_write_byte(m, via2d1ifr);
    snapshot_module_write_byte(m, via2d1ier);

						/* FIXME! */
    snapshot_module_write_byte(m, (((via2d1pb7 ^ via2d1pb7x) | via2d1pb7o) ? 0x80 : 0));
    snapshot_module_write_byte(m, 0);		/* SRHBITS */

    snapshot_module_write_byte(m, (ca2_state ? 0x80 : 0) 
				| (cb2_state ? 0x40 : 0));

    snapshot_module_write_byte(m, via2d1_ila);
    snapshot_module_write_byte(m, via2d1_ilb);

    snapshot_module_close(m);

    return 0;
}

int via2d1_read_snapshot_module(snapshot_t * p)
{
    BYTE vmajor, vminor;
    BYTE byte;
    WORD word;
    ADDRESS addr;
    CLOCK rclk = drive_clk[1];
    snapshot_module_t *m;

    m = snapshot_module_open(p, snap_module_name, &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (vmajor != VIA_DUMP_VER_MAJOR) {
        log_error(via2d1_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  vmajor, vminor, VIA_DUMP_VER_MAJOR, VIA_DUMP_VER_MINOR);
        snapshot_module_close(m);
        return -1;
    }

    drive1_unset_alarm(A_VIA2D1T1);
    drive1_unset_alarm(A_VIA2D1T2);
    via2d1tai = 0;
    via2d1tbi = 0;

    snapshot_module_read_byte(m, &via2d1[VIA_PRA]);
    snapshot_module_read_byte(m, &via2d1[VIA_DDRA]);
    snapshot_module_read_byte(m, &via2d1[VIA_PRB]);
    snapshot_module_read_byte(m, &via2d1[VIA_DDRB]);
    {
        addr = VIA_DDRA;
	byte = via2d1[VIA_PRA] | ~via2d1[VIA_DDRA];


	oldpa = byte;

	addr = VIA_DDRB;
	byte = via2d1[VIA_PRB] | ~via2d1[VIA_DDRB];

    drive[1].led_status = byte & 8;
    drive[1].rotation_table_ptr = drive[1].rotation_table[(byte >> 5) & 0x3];
    drive[1].byte_ready_active = (drive[1].byte_ready_active & ~0x04)
                                 | (byte & 0x04);
	oldpb = byte;
    }

    snapshot_module_read_word(m, &word);
    via2d1tal = word;
    via2d1[VIA_T1LL] = via2d1tal & 0xff;
    via2d1[VIA_T1LH] = (via2d1tal >> 8) & 0xff;
    snapshot_module_read_word(m, &word);
    via2d1tau = rclk + word + 2 /* 3 */ + TAUOFFSET;
    via2d1tai = rclk + word + 1;

    snapshot_module_read_byte(m, &via2d1[VIA_T2LL]);
    snapshot_module_read_word(m, &word);
    via2d1tbu = rclk + word + 2 /* 3 */;
    via2d1tbi = rclk + word + 1;

    snapshot_module_read_byte(m, &byte);
    if (byte & 0x80) {
    	drive1_set_alarm_clk(A_VIA2D1T1, via2d1tai);
    } else {
	via2d1tai = 0;
    }
    if (byte & 0x40) {
    	drive1_set_alarm_clk(A_VIA2D1T2, via2d1tbi);
    } else {
	via2d1tbi = 0;
    }

    snapshot_module_read_byte(m, &via2d1[VIA_SR]);
    snapshot_module_read_byte(m, &via2d1[VIA_ACR]);
    snapshot_module_read_byte(m, &via2d1[VIA_PCR]);

    snapshot_module_read_byte(m, &byte);
    via2d1ifr = byte;
    snapshot_module_read_byte(m, &byte);
    via2d1ier = byte;

    via_restore_int(via2d1ifr & via2d1ier & 0x7f);

    /* FIXME! */
    snapshot_module_read_byte(m, &byte);
    via2d1pb7 = byte ? 1 : 0;
    via2d1pb7x = 0;
    via2d1pb7o = 0;
    snapshot_module_read_byte(m, &byte);	/* SRHBITS */

    snapshot_module_read_byte(m, &byte);	/* CABSTATE */
    ca2_state = byte & 0x80;
    cb2_state = byte & 0x40;

    /* UNDUMP_PCR also restores the ca2_state/cb2_state effects if necessary;
       i.e. calls VIA_SET_C*2( c*2_state ) if necessary */
    {
	addr = VIA_PCR;
	byte = via2d1[addr];

    drive_update_viad2_pcr(byte, &drive[1]);
    }
    {
	addr = VIA_SR;
	byte = via2d1[addr];
	
    }
    {
	addr = VIA_ACR;
	byte = via2d1[addr];
	
    }

    snapshot_module_read_byte(m, &via2d1_ila);
    snapshot_module_read_byte(m, &via2d1_ilb);

    return snapshot_module_close(m);
}


