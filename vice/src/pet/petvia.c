
/*
 * ../../../src/pet/petvia.c
 * This file is generated from ../../../src/via-tmpl.c and ../../../src/pet/petvia.def,
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
 * into one interrupt flag, I_VIAFL.
 * if an interrupt condition changes, the function (i.e. cpp macro)
 * update_viairq() id called, that checks the IRQ line state.
 * This is now possible, as ettore has decoupled A_* alarm events
 * from I_* interrupts for performance reasons.
 *
 * A new function for signaling rising/falling edges on the
 * control lines is introduced:
 *      via_signal(VIA_SIG_[CA1|CA2|CB1|CB2], VIA_SIG_[RISE|FALL])
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


#include "petvia.h"

#include "crtc.h"
#include "kbd.h"
#include "parallel.h"
#include "drive.h"
#include "petsound.h"

#ifdef HAVE_PRINTER
#include "pruser.h"
#endif

#define	VIA_SET_CA2(byte)		\
          crtc_set_char( byte ); /* switching PET charrom with CA2 */
				     /* switching userport strobe with CB2 */
#ifdef HAVE_PRINTER
#define	VIA_SET_CB2(byte)		\
          pruser_write_strobe( byte );
#else
#define	VIA_SET_CB2(byte)
#endif


#include "interrupt.h"
				/* Timer debugging */
/*#define VIA_TIMER_DEBUG */
				/* when PB7 is really used, set this
				   to enable pulse output from the timer.
				   Otherwise PB7 state is computed only
				   when port B is read -
				   not yet implemented */
/*#define VIA_NEED_PB7 */
				/* When you really need latching, define this.
				   It implies additional READ_PR* when
				   writing the snapshot. When latching is 
				   enabled: it reads the port when enabling,
				   and when an active C*1 transition occurs. 
				   It does not read the port when reading the
				   port register. Side-effects beware! */
/*#define VIA_NEED_LATCHING */

/* global */

BYTE via[16];



/*
 * Local variables
 */

static int viaifr;		/* Interrupt Flag register for via */
static int viaier;		/* Interrupt Enable register for via */

static unsigned int viatal;	/* current timer A latch value */
static unsigned int viatbl;	/* current timer B latch value */

static CLOCK viatau;		/* time when via timer A is updated */
static CLOCK viatbu;		/* time when via timer B is updated */
static CLOCK viatai;		/* time when next timer A alarm is */
static CLOCK viatbi;		/* time when next timer A alarm is */

static int viapb7;		/* state of PB7 for pulse output... */
static int viapb7x;		/* to be xored herewith  */
static int viapb7o;		/* to be ored herewith  */
static int viapb7xx;
static int viapb7sx;

static BYTE oldpa;		/* the actual output on PA (input = high) */
static BYTE oldpb;		/* the actual output on PB (input = high) */

static BYTE via_ila;		/* input latch A */
static BYTE via_ilb;		/* input latch B */

static int ca2_state;
static int cb2_state;

/*
 * local functions
 */

#define IS_CA2_OUTPUT()          ((via[VIA_PCR] & 0x0c) == 0x0c)
#define IS_CA2_INDINPUT()        ((via[VIA_PCR] & 0x0a) == 0x02)
#define IS_CA2_HANDSHAKE()       ((via[VIA_PCR] & 0x0c) == 0x08)
#define IS_CA2_PULSE_MODE()      ((via[VIA_PCR] & 0x0e) == 0x09)
#define IS_CA2_TOGGLE_MODE()     ((via[VIA_PCR] & 0x0e) == 0x08)

#define IS_CB2_OUTPUT()          ((via[VIA_PCR] & 0xc0) == 0xc0)
#define IS_CB2_INDINPUT()        ((via[VIA_PCR] & 0xa0) == 0x20)
#define IS_CB2_HANDSHAKE()       ((via[VIA_PCR] & 0xc0) == 0x80)
#define IS_CB2_PULSE_MODE()      ((via[VIA_PCR] & 0xe0) == 0x90)
#define IS_CB2_TOGGLE_MODE()     ((via[VIA_PCR] & 0xe0) == 0x80)

#define	IS_PA_INPUT_LATCH()	 (via[VIA_ACR] & 0x01)
#define	IS_PB_INPUT_LATCH()	 (via[VIA_ACR] & 0x02)

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
 viat*u* clk ------------------------------------------+   +--------
                                                     |
                                                     |
                                                  call of
			                        int_via*
                                                   here

   real viatau value = viatau* + TAUOFFSET
   viatbu = viatbu* + 0

 *
 * IRQ and PB7 are set/toggled at the low-high transition of Phi2,
 * but int_* is called a half-cycle before that. Does that matter?
 *
 * PB7 output is still to be implemented
 */

/* timer values do not depend on a certain value here, but PB7 does... */
#define	TAUOFFSET	(-1)


#ifndef via_restore_int	/* if VIA reports to other chip (TPI) for IRQ */
#define	via_restore_int(a)  set_int_noclk(&maincpu_int_status, I_VIAFL, \
		(a) ? IK_IRQ : 0)
#endif


inline static void update_viairq(void)
{
#if 0	/* DEBUG */
    static int irq = 0;
    if(irq && !(viaifr & viaier & 0x7f)) {
       printf("via: clk=%d, IRQ off\n", clk);
    }
    if(!irq && (viaifr & viaier & 0x7f)) {
       printf("via: clk=%d, IRQ on\n", clk);
    }
    irq = (viaifr & viaier & 0x7f);
#endif
    maincpu_set_irq(I_VIAFL, (viaifr & viaier & 0x7f) ? IK_IRQ : 0);
}

/* the next two are used in read_via() */

inline static unsigned int viata(void)
{
    if (clk < viatau - TAUOFFSET)
        return viatau - TAUOFFSET - clk - 2;
    else
	return (viatal - (clk - viatau + TAUOFFSET) % (viatal + 2));
}

inline static unsigned int viatb(void)
{
    return viatbu - clk - 2;
}

inline static void update_viatal(CLOCK rclk)
{
    viapb7x = 0;
    viapb7xx = 0;

    if (rclk > viatau) {
	int nuf = (viatal + 1 + rclk - viatau) / (viatal + 2);

	if (!(via[VIA_ACR] & 0x40)) {
	    if (((nuf - viapb7sx) > 1) || (!viapb7)) {
		viapb7o = 1;
		viapb7sx = 0;
	    }
	}
	viapb7 ^= (nuf & 1);

	viatau = TAUOFFSET + viatal + 2 + (rclk - (rclk - viatau + TAUOFFSET) % (viatal + 2));
	if (rclk == viatau - viatal - 1) {
	    viapb7xx = 1;
	}
    }

    if (viatau == rclk)
	viapb7x = 1;

    viatal = via[VIA_T1LL] + (via[VIA_T1LH] << 8);
}

inline static void update_viatbl(void)
{
    viatbl = via[VIA_T2CL] + (via[VIA_T2CH] << 8);
}


/* ------------------------------------------------------------------------- */
/* VIA */



/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void reset_via(void)
{
    int i;
#ifdef VIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("VIA: reset\n");
#endif
    /* clear registers */
    for (i = 0; i < 4; i++)
	via[i] = 0;
    for (i = 4; i < 10; i++)
        via[i] = 0xff;        /* AB 98.08.23 */
    for (i = 11; i < 16; i++)
	via[i] = 0;

    viatal = 0;
    viatbl = 0;
    viatau = clk;
    viatbu = clk;

    viaier = 0;
    viaifr = 0;

    /* disable vice interrupts */
    viatai = 0;
    viatbi = 0;
    maincpu_unset_alarm(A_VIAT1);
    maincpu_unset_alarm(A_VIAT2);
    update_viairq();

    oldpa = 0xff;
    oldpb = 0xff;

    ca2_state = 1;
    cb2_state = 1;
    VIA_SET_CA2( ca2_state )	/* input = high */
    VIA_SET_CB2( cb2_state )	/* input = high */


    /* set IEC output lines */
    parallel_cpu_set_atn(0);
    parallel_cpu_set_nrfd(0);

#ifdef HAVE_PRINTER
    pruser_write_data(0xff);
    pruser_write_strobe(1);
#endif
}

void via_signal(int line, int edge)
{
    switch (line) {
      case VIA_SIG_CA1:
	if ( (edge ? 1 : 0) == (via[VIA_PCR] & 0x01) ) {
	    if (IS_CA2_TOGGLE_MODE() && !ca2_state) {
		ca2_state = 1;
		VIA_SET_CA2( ca2_state )
	    }
            viaifr |= VIA_IM_CA1;
            update_viairq();
#ifdef VIA_NEED_LATCHING
	    if (IS_PA_INPUT_LATCH()) {
		BYTE byte;

        {
            byte = 255;
            /* VIA PA is connected to the userport pins C-L */
	    byte &= (joy[1] & 1) ? ~0x80 : 0xff;
	    byte &= (joy[1] & 2) ? ~0x40 : 0xff;
	    byte &= (joy[1] & 4) ? ~0x20 : 0xff;
	    byte &= (joy[1] & 8) ? ~0x10 : 0xff;
	    byte &= (joy[1] & 16)? ~0xc0 : 0xff;
	    byte &= (joy[2] & 1) ? ~0x08 : 0xff;
	    byte &= (joy[2] & 2) ? ~0x04 : 0xff;
	    byte &= (joy[2] & 4) ? ~0x02 : 0xff;
	    byte &= (joy[2] & 8) ? ~0x01 : 0xff;
	    byte &= (joy[2] & 16)? ~0x0c : 0xff;

#if 0
            printf("read port A %d\n", byte);
            printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                   (int) byte, (int) via[VIA_PRB],
                   (int) via[VIA_DDRA], (int) via[VIA_DDRB], joy[2]);
#endif
	    /* joystick always pulls low, even if high output, so no
	       masking with DDRA */
            /*return ((j & ~via[VIA_DDRA]) | (via[VIA_PRA] & via[VIA_DDRA]));*/
        }
		via_ila = byte;
	    }
#endif
	}
        break;
      case VIA_SIG_CA2:
        if (!(via[VIA_PCR] & 0x08)) {
            viaifr |= (((edge << 2) ^ via[VIA_PCR]) & 0x04) ?
                0 : VIA_IM_CA2;
            update_viairq();
        }
        break;
      case VIA_SIG_CB1:
	if ( (edge ? 0x10 : 0) == (via[VIA_PCR] & 0x10) ) {
	    if (IS_CB2_TOGGLE_MODE() && !cb2_state) {
		cb2_state = 1;
		VIA_SET_CB2( cb2_state )
	    }
            viaifr |= VIA_IM_CB1;
            update_viairq();
#ifdef VIA_NEED_LATCHING
	    if (IS_PB_INPUT_LATCH()) {
		BYTE byte;

        {
	    if (drive[0].enable)
	        drive0_cpu_execute();
	    if (drive[1].enable)
	        drive1_cpu_execute();

            /* read parallel IEC interface line states */
            byte = 255 
		- (parallel_nrfd ? 64:0) 
		- (parallel_ndac ? 1:0) 
		- (parallel_dav ? 128:0);
            /* vertical retrace */
            byte -= crtc_offscreen() ? 32:0;
#if 0
                printf("read port B %d\n", byte);
                printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                       (int) via[VIA_PRA], (int) byte,
                       (int) via[VIA_DDRA], (int) via[VIA_DDRB], joy[1]);
#endif
	    /* none of the load changes output register value -> std. masking */
            byte = ((byte & ~via[VIA_DDRB]) | (via[VIA_PRB] & via[VIA_DDRB]));
        }
		via_ilb = byte;
	    }
#endif	
	}
        break;
      case VIA_SIG_CB2:
        if (!(via[VIA_PCR] & 0x80)) {
            viaifr |= (((edge << 6) ^ via[VIA_PCR]) & 0x40) ?
                0 : VIA_IM_CB2;
            update_viairq();
        }
        break;
    }
}

void REGPARM2 store_via(ADDRESS addr, BYTE byte)
{
    CLOCK rclk = clk - 1;	/* stores have a one-cylce offset */

    addr &= 0xf;
#ifdef VIA_TIMER_DEBUG
    if ((addr < 10 && addr > 3) || (addr == VIA_ACR))
	printf("store via[%x] %x, rmwf=%d, clk=%d, rclk=%d\n",
	       (int) addr, (int) byte, rmw_flag, clk, rclk);
#endif

    switch (addr) {

      /* these are done with saving the value */
      case VIA_PRA:		/* port A */
        viaifr &= ~VIA_IM_CA1;
        if (!IS_CA2_INDINPUT()) {
            viaifr &= ~VIA_IM_CA2;
        }
	if(IS_CA2_HANDSHAKE()) {
	    ca2_state = 0;
	    VIA_SET_CA2( ca2_state )
	    if(IS_CA2_PULSE_MODE()) {
	  	ca2_state = 1;
	    	VIA_SET_CA2( ca2_state )
	    }
	}
        update_viairq();

      case VIA_PRA_NHS:	/* port A, no handshake */
        via[VIA_PRA_NHS] = byte;
        addr = VIA_PRA;
      case VIA_DDRA:
	via[addr] = byte;
	byte = via[VIA_PRA] | ~via[VIA_DDRA];

#ifdef HAVE_PRINTER
	pruser_write_data(byte);
#endif
	oldpa = byte;
        break;

      case VIA_PRB:		/* port B */
        viaifr &= ~VIA_IM_CB1;
        if ((via[VIA_PCR] & 0xa0) != 0x20) {
            viaifr &= ~VIA_IM_CB2;
        }
        if(IS_CB2_HANDSHAKE()) {
            cb2_state = 0;
            VIA_SET_CB2( cb2_state )
            if(IS_CB2_PULSE_MODE()) {
                cb2_state = 1;
                VIA_SET_CB2( cb2_state )
            }
        }
        update_viairq();

      case VIA_DDRB:
	via[addr] = byte;
	byte = via[VIA_PRB] | ~via[VIA_DDRB];

	if((addr==VIA_DDRB) && (via[addr] & 0x20)) {
	    fprintf(stderr,"PET: Killer POKE! might kill a real PET!\n");
	}
        parallel_cpu_set_nrfd(!(byte & 0x02));
        parallel_cpu_set_atn(!(byte & 0x04));
	oldpb = byte;
        break;

      case VIA_SR:		/* Serial Port output buffer */
        via[addr] = byte;

	store_petsnd_sample(byte);
            break;

        /* Timers */

      case VIA_T1CL:
      case VIA_T1LL:
        via[VIA_T1LL] = byte;
        update_viatal(rclk);
        break;

      case VIA_T1CH /*TIMER_AH */ :	/* Write timer A high */
#ifdef VIA_TIMER_DEBUG
        if (app_resources.debugFlag)
            printf("Write timer A high: %02x\n", byte);
#endif
        via[VIA_T1LH] = byte;
        update_viatal(rclk);
        /* load counter with latch value */
        viatau = rclk + viatal + 3 + TAUOFFSET;
        viatai = rclk + viatal + 2;
        maincpu_set_alarm_clk(A_VIAT1, viatai);

        /* set pb7 state */
        viapb7 = 0;
        viapb7o = 0;

        /* Clear T1 interrupt */
        viaifr &= ~VIA_IM_T1;
        update_viairq();
        break;

      case VIA_T1LH:		/* Write timer A high order latch */
        via[addr] = byte;
        update_viatal(rclk);

        /* Clear T1 interrupt */
        viaifr &= ~VIA_IM_T1;
        update_viairq();
        break;

      case VIA_T2LL:		/* Write timer 2 low latch */
        via[VIA_T2LL] = byte;
        update_viatbl();

        store_petsnd_rate(2*byte+4);
	if(!byte) {
	  store_petsnd_onoff(0);
	} else {
	  store_petsnd_onoff(((via[VIA_ACR] & 0x1c)==0x10)?1:0);
	}
            break;

      case VIA_T2CH:		/* Write timer 2 high */
        via[VIA_T2CH] = byte;
        update_viatbl();
        viatbu = rclk + viatbl + 3;
        viatbi = rclk + viatbl + 2;
        maincpu_set_alarm_clk(A_VIAT2, viatbi);

        /* Clear T2 interrupt */
        viaifr &= ~VIA_IM_T2;
        update_viairq();
        break;

        /* Interrupts */

      case VIA_IFR:		/* 6522 Interrupt Flag Register */
        viaifr &= ~byte;
        update_viairq();
        break;

      case VIA_IER:		/* Interrupt Enable Register */
#if defined (VIA_TIMER_DEBUG)
        printf("Via#1 set VIA_IER: 0x%x\n", byte);
#endif
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
            viaier |= byte & 0x7f;
        } else {
            /* clear interrupts */
            viaier &= ~byte;
        }
        update_viairq();
        break;

        /* Control */

      case VIA_ACR:
        /* bit 7 timer 1 output to PB7 */
        update_viatal(rclk);
        if ((via[VIA_ACR] ^ byte) & 0x80) {
            if (byte & 0x80) {
                viapb7 = 1 ^ viapb7x;
            }
        }
        if ((via[VIA_ACR] ^ byte) & 0x40) {
            viapb7 ^= viapb7sx;
            if ((byte & 0x40)) {
                if (viapb7x || viapb7xx) {
                    if (viatal) {
                        viapb7o = 1;
                    } else {
                        viapb7o = 0;
                        if ((via[VIA_ACR] & 0x80) && viapb7x && (!viapb7xx))
                            viapb7 ^= 1;
                    }
                }
            }
        }
        viapb7sx = viapb7x;

        /* bit 1, 0  latch enable port B and A */
#ifdef VIA_NEED_LATCHING
	/* switch on port A latching - FIXME: is this ok? */
	if ( (!(via[addr] & 1)) && (byte & 1)) {

        {
            byte = 255;
            /* VIA PA is connected to the userport pins C-L */
	    byte &= (joy[1] & 1) ? ~0x80 : 0xff;
	    byte &= (joy[1] & 2) ? ~0x40 : 0xff;
	    byte &= (joy[1] & 4) ? ~0x20 : 0xff;
	    byte &= (joy[1] & 8) ? ~0x10 : 0xff;
	    byte &= (joy[1] & 16)? ~0xc0 : 0xff;
	    byte &= (joy[2] & 1) ? ~0x08 : 0xff;
	    byte &= (joy[2] & 2) ? ~0x04 : 0xff;
	    byte &= (joy[2] & 4) ? ~0x02 : 0xff;
	    byte &= (joy[2] & 8) ? ~0x01 : 0xff;
	    byte &= (joy[2] & 16)? ~0x0c : 0xff;

#if 0
            printf("read port A %d\n", byte);
            printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                   (int) byte, (int) via[VIA_PRB],
                   (int) via[VIA_DDRA], (int) via[VIA_DDRB], joy[2]);
#endif
	    /* joystick always pulls low, even if high output, so no
	       masking with DDRA */
            /*return ((j & ~via[VIA_DDRA]) | (via[VIA_PRA] & via[VIA_DDRA]));*/
        }
	    via_ila = byte;
	}
	/* switch on port B latching - FIXME: is this ok? */
	if ( (!(via[addr] & 2)) && (byte & 2)) {

        {
	    if (drive[0].enable)
	        drive0_cpu_execute();
	    if (drive[1].enable)
	        drive1_cpu_execute();

            /* read parallel IEC interface line states */
            byte = 255 
		- (parallel_nrfd ? 64:0) 
		- (parallel_ndac ? 1:0) 
		- (parallel_dav ? 128:0);
            /* vertical retrace */
            byte -= crtc_offscreen() ? 32:0;
#if 0
                printf("read port B %d\n", byte);
                printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                       (int) via[VIA_PRA], (int) byte,
                       (int) via[VIA_DDRA], (int) via[VIA_DDRB], joy[1]);
#endif
	    /* none of the load changes output register value -> std. masking */
            byte = ((byte & ~via[VIA_DDRB]) | (via[VIA_PRB] & via[VIA_DDRB]));
        }
	    via_ilb = byte;
	}
#endif

        via[addr] = byte;


	store_petsnd_onoff(via[VIA_T2LL] ? (((byte & 0x1c)==0x10)?1:0) : 0);

        /* bit 5 timer 2 count mode */
        if (byte & 32) {
            /* TODO */
            /* update_viatb(0); *//* stop timer if mode == 1 */
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


#if 0
        if(byte != via[VIA_PCR]) {
          register BYTE tmp = byte;
          /* first set bit 1 and 5 to the real output values */
          if((tmp & 0x0c) != 0x0c) tmp |= 0x02;
          if((tmp & 0xc0) != 0xc0) tmp |= 0x20;
          crtc_set_char( byte & 2 ); /* switching PET charrom with CA2 */
				     /* switching userport strobe with CB2 */
#ifdef HAVE_PRINTER
          pruser_write_strobe( byte & 0x20 );
#endif
	}
#endif

        via[addr] = byte;

        break;

      default:
        via[addr] = byte;

    }				/* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_via(ADDRESS addr)
{
#ifdef VIA_TIMER_DEBUG
    BYTE read_via_(ADDRESS);
    BYTE retv = read_via_(addr);
    addr &= 0x0f;
    if ((addr > 3 && addr < 10) || app_resources.debugFlag)
	printf("read_via(%x) -> %02x, clk=%d\n", addr, retv, clk);
    return retv;
}
BYTE REGPARM1 read_via_(ADDRESS addr)
{
#endif
    BYTE byte = 0xff;
    CLOCK rclk = clk;

    addr &= 0xf;

    if (viatai && (viatai <= clk))
	int_viat1(clk - viatai);
    if (viatbi && (viatbi <= clk))
	int_viat2(clk - viatbi);

    switch (addr) {

      case VIA_PRA:		/* port A */
        viaifr &= ~VIA_IM_CA1;
        if ((via[VIA_PCR] & 0x0a) != 0x02) {
            viaifr &= ~VIA_IM_CA2;
        }
        if(IS_CA2_HANDSHAKE()) {
            ca2_state = 0;
            VIA_SET_CA2( ca2_state )
            if(IS_CA2_PULSE_MODE()) {
                ca2_state = 1;
                VIA_SET_CA2( ca2_state )
            }
        }
        update_viairq();

      case VIA_PRA_NHS:	/* port A, no handshake */
        /* WARNING: this pin reads the voltage of the output pins, not
           the ORA value as the other port. Value read might be different
           from what is expected due to excessive load. */
#ifdef VIA_NEED_LATCHING
	if (IS_PA_INPUT_LATCH()) {
	    byte = via_ila;
	} else {

        {
            byte = 255;
            /* VIA PA is connected to the userport pins C-L */
	    byte &= (joy[1] & 1) ? ~0x80 : 0xff;
	    byte &= (joy[1] & 2) ? ~0x40 : 0xff;
	    byte &= (joy[1] & 4) ? ~0x20 : 0xff;
	    byte &= (joy[1] & 8) ? ~0x10 : 0xff;
	    byte &= (joy[1] & 16)? ~0xc0 : 0xff;
	    byte &= (joy[2] & 1) ? ~0x08 : 0xff;
	    byte &= (joy[2] & 2) ? ~0x04 : 0xff;
	    byte &= (joy[2] & 4) ? ~0x02 : 0xff;
	    byte &= (joy[2] & 8) ? ~0x01 : 0xff;
	    byte &= (joy[2] & 16)? ~0x0c : 0xff;

#if 0
            printf("read port A %d\n", byte);
            printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                   (int) byte, (int) via[VIA_PRB],
                   (int) via[VIA_DDRA], (int) via[VIA_DDRB], joy[2]);
#endif
	    /* joystick always pulls low, even if high output, so no
	       masking with DDRA */
            /*return ((j & ~via[VIA_DDRA]) | (via[VIA_PRA] & via[VIA_DDRA]));*/
        }
	}
#else

        {
            byte = 255;
            /* VIA PA is connected to the userport pins C-L */
	    byte &= (joy[1] & 1) ? ~0x80 : 0xff;
	    byte &= (joy[1] & 2) ? ~0x40 : 0xff;
	    byte &= (joy[1] & 4) ? ~0x20 : 0xff;
	    byte &= (joy[1] & 8) ? ~0x10 : 0xff;
	    byte &= (joy[1] & 16)? ~0xc0 : 0xff;
	    byte &= (joy[2] & 1) ? ~0x08 : 0xff;
	    byte &= (joy[2] & 2) ? ~0x04 : 0xff;
	    byte &= (joy[2] & 4) ? ~0x02 : 0xff;
	    byte &= (joy[2] & 8) ? ~0x01 : 0xff;
	    byte &= (joy[2] & 16)? ~0x0c : 0xff;

#if 0
            printf("read port A %d\n", byte);
            printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                   (int) byte, (int) via[VIA_PRB],
                   (int) via[VIA_DDRA], (int) via[VIA_DDRB], joy[2]);
#endif
	    /* joystick always pulls low, even if high output, so no
	       masking with DDRA */
            /*return ((j & ~via[VIA_DDRA]) | (via[VIA_PRA] & via[VIA_DDRA]));*/
        }
#endif
	via_ila = byte;
	return byte;

      case VIA_PRB:		/* port B */
        viaifr &= ~VIA_IM_CB1;
        if ((via[VIA_PCR] & 0xa0) != 0x20)
            viaifr &= ~VIA_IM_CB2;
        update_viairq();

        /* WARNING: this pin reads the ORA for output pins, not
           the voltage on the pins as the other port. */
#ifdef VIA_NEED_LATCHING
	if (IS_PB_INPUT_LATCH()) {
	    byte = via_ilb;
	} else {

        {
	    if (drive[0].enable)
	        drive0_cpu_execute();
	    if (drive[1].enable)
	        drive1_cpu_execute();

            /* read parallel IEC interface line states */
            byte = 255 
		- (parallel_nrfd ? 64:0) 
		- (parallel_ndac ? 1:0) 
		- (parallel_dav ? 128:0);
            /* vertical retrace */
            byte -= crtc_offscreen() ? 32:0;
#if 0
                printf("read port B %d\n", byte);
                printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                       (int) via[VIA_PRA], (int) byte,
                       (int) via[VIA_DDRA], (int) via[VIA_DDRB], joy[1]);
#endif
	    /* none of the load changes output register value -> std. masking */
            byte = ((byte & ~via[VIA_DDRB]) | (via[VIA_PRB] & via[VIA_DDRB]));
        }
	}
#else

        {
	    if (drive[0].enable)
	        drive0_cpu_execute();
	    if (drive[1].enable)
	        drive1_cpu_execute();

            /* read parallel IEC interface line states */
            byte = 255 
		- (parallel_nrfd ? 64:0) 
		- (parallel_ndac ? 1:0) 
		- (parallel_dav ? 128:0);
            /* vertical retrace */
            byte -= crtc_offscreen() ? 32:0;
#if 0
                printf("read port B %d\n", byte);
                printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                       (int) via[VIA_PRA], (int) byte,
                       (int) via[VIA_DDRA], (int) via[VIA_DDRB], joy[1]);
#endif
	    /* none of the load changes output register value -> std. masking */
            byte = ((byte & ~via[VIA_DDRB]) | (via[VIA_PRB] & via[VIA_DDRB]));
        }
#endif
	via_ilb = byte;
        byte = (byte & ~via[VIA_DDRB]) | (via[VIA_PRB] & via[VIA_DDRB]);

        if (via[VIA_ACR] & 0x80) {
            update_viatal(rclk);
            byte = (byte & 0x7f) | (((viapb7 ^ viapb7x) | viapb7o) ? 0x80 : 0);
        }
        return byte;

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        viaifr &= ~VIA_IM_T1;
        update_viairq();
        return viata() & 0xff;

      case VIA_T1CH /*TIMER_AH */ :	/* timer A high */
        return (viata() >> 8) & 0xff;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        viaifr &= ~VIA_IM_T2;
        update_viairq();
        return viatb() & 0xff;

      case VIA_T2CH /*TIMER_BH */ :	/* timer B high */
        return (viatb() >> 8) & 0xff;

      case VIA_SR:		/* Serial Port Shift Register */
        return (via[addr]);

        /* Interrupts */

      case VIA_IFR:		/* Interrupt Flag Register */
        {
            BYTE t = viaifr;
            if (viaifr & viaier /*[VIA_IER] */ )
                t |= 0x80;
            return (t);
        }

      case VIA_IER:		/* 6522 Interrupt Control Register */
        return (viaier /*[VIA_IER] */  | 0x80);

    }				/* switch */

    return (via[addr]);
}

BYTE REGPARM1 peek_via(ADDRESS addr)
{
    CLOCK rclk = clk;

    addr &= 0xf;

    if (viatai && (viatai <= clk))
	int_viat1(clk - viatai);
    if (viatbi && (viatbi <= clk))
	int_viat2(clk - viatbi);

    switch (addr) {
      case VIA_PRA:
        return read_via(VIA_PRA_NHS);

      case VIA_PRB:		/* port B */
        {
            BYTE byte;
#ifdef VIA_NEED_LATCHING
	    if (IS_PB_INPUT_LATCH()) {
	        byte = via_ilb;
	    } else {

        {
	    if (drive[0].enable)
	        drive0_cpu_execute();
	    if (drive[1].enable)
	        drive1_cpu_execute();

            /* read parallel IEC interface line states */
            byte = 255 
		- (parallel_nrfd ? 64:0) 
		- (parallel_ndac ? 1:0) 
		- (parallel_dav ? 128:0);
            /* vertical retrace */
            byte -= crtc_offscreen() ? 32:0;
#if 0
                printf("read port B %d\n", byte);
                printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                       (int) via[VIA_PRA], (int) byte,
                       (int) via[VIA_DDRA], (int) via[VIA_DDRB], joy[1]);
#endif
	    /* none of the load changes output register value -> std. masking */
            byte = ((byte & ~via[VIA_DDRB]) | (via[VIA_PRB] & via[VIA_DDRB]));
        }
	    }
#else

        {
	    if (drive[0].enable)
	        drive0_cpu_execute();
	    if (drive[1].enable)
	        drive1_cpu_execute();

            /* read parallel IEC interface line states */
            byte = 255 
		- (parallel_nrfd ? 64:0) 
		- (parallel_ndac ? 1:0) 
		- (parallel_dav ? 128:0);
            /* vertical retrace */
            byte -= crtc_offscreen() ? 32:0;
#if 0
                printf("read port B %d\n", byte);
                printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                       (int) via[VIA_PRA], (int) byte,
                       (int) via[VIA_DDRA], (int) via[VIA_DDRB], joy[1]);
#endif
	    /* none of the load changes output register value -> std. masking */
            byte = ((byte & ~via[VIA_DDRB]) | (via[VIA_PRB] & via[VIA_DDRB]));
        }
#endif
            byte = (byte & ~via[VIA_DDRB]) | (via[VIA_PRB] & via[VIA_DDRB]);
            if (via[VIA_ACR] & 0x80) {
                update_viatal(rclk);
                byte = (byte & 0x7f) | (((viapb7 ^ viapb7x) | viapb7o) ? 0x80 : 0);
            }
            return byte;
        }

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        return viata() & 0xff;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        return viatb() & 0xff;

      default:
        break;
    }				/* switch */

    return read_via(addr);
}


/* ------------------------------------------------------------------------- */

int int_viat1(long offset)
{
/*    CLOCK rclk = clk - offset; */
#ifdef VIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("via timer A interrupt\n");
#endif

    if (!(via[VIA_ACR] & 0x40)) {	/* one-shot mode */
#if 0				/* defined (VIA_TIMER_DEBUG) */
	printf("VIA Timer A interrupt -- one-shot mode: next int won't happen\n");
#endif
	maincpu_unset_alarm(A_VIAT1);	/*int_clk[I_VIAT1] = 0; */
	viatai = 0;
    } else {			/* continuous mode */
	/* load counter with latch value */
	viatai += viatal + 2;
	maincpu_set_alarm_clk(A_VIAT1, viatai);
    }
    viaifr |= VIA_IM_T1;
    update_viairq();

    /* TODO: toggle PB7? */
    return 0;			/*(viaier & VIA_IM_T1) ? 1:0; */
}

/*
 * Timer B is always in one-shot mode
 */

int int_viat2(long offset)
{
#ifdef VIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("VIA timer B interrupt\n");
#endif
    maincpu_unset_alarm(A_VIAT2);	/*int_clk[I_VIAT2] = 0; */
    viatbi = 0;

    viaifr |= VIA_IM_T2;
    update_viairq();

    return 0;
}

void via_prevent_clk_overflow(CLOCK sub)
{
    unsigned int t;
    t = (viatau - (clk + sub)) & 0xffff;
    viatau = clk + t;
    t = (viatbu - (clk + sub)) & 0xffff;
    viatbu = clk + t;
    if (viatai)
	viatai -= sub;
}

/*------------------------------------------------------------------------*/

/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 0/0
 */

static char snap_module_name[] = "VIA";
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

int via_write_snapshot_module(snapshot_t * p)
{
    snapshot_module_t *m;

    if (viatai && (viatai <= clk))
        int_viat1(clk - viatai);
    if (viatbi && (viatbi <= clk))
        int_viat2(clk - viatbi);

    m = snapshot_module_create(p, snap_module_name,
                               VIA_DUMP_VER_MAJOR, VIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;
/*
printf("via: write: clk=%d, tai=%d, tau=%d\n"
       "     : tbi=%d, tbu=%d\n",
		clk, viatai, viatau, viatbi, viatbu);
printf("     : ta=%d, tb=%d\n",viata() & 0xffff, viatb() & 0xffff);
*/
    snapshot_module_write_byte(m, via[VIA_PRA]);
    snapshot_module_write_byte(m, via[VIA_DDRA]);
    snapshot_module_write_byte(m, via[VIA_PRB]);
    snapshot_module_write_byte(m, via[VIA_DDRB]);

    snapshot_module_write_word(m, viatal);
    snapshot_module_write_word(m, viata());
    snapshot_module_write_byte(m, viatbl);
    snapshot_module_write_word(m, viatb());

    snapshot_module_write_byte(m, (viatai ? 0x80 : 0)
					| (viatbi ? 0x40 : 0) );

    snapshot_module_write_byte(m, via[VIA_SR]);
    snapshot_module_write_byte(m, via[VIA_ACR]);
    snapshot_module_write_byte(m, via[VIA_PCR]);

    snapshot_module_write_byte(m, viaifr);
    snapshot_module_write_byte(m, viaier);

						/* FIXME! */
    snapshot_module_write_byte(m, (((viapb7 ^ viapb7x) | viapb7o) ? 0x80 : 0));
    snapshot_module_write_byte(m, 0);		/* SRHBITS */

    snapshot_module_write_byte(m, (ca2_state ? 0x80 : 0) 
				| (cb2_state ? 0x40 : 0));

    snapshot_module_write_byte(m, via_ila);
    snapshot_module_write_byte(m, via_ilb);

    snapshot_module_close(m);

    return 0;
}

int via_read_snapshot_module(snapshot_t * p)
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

    maincpu_unset_alarm(A_VIAT1);
    maincpu_unset_alarm(A_VIAT2);
    viatai = 0;
    viatbi = 0;

    snapshot_module_read_byte(m, &via[VIA_PRA]);
    snapshot_module_read_byte(m, &via[VIA_DDRA]);
    snapshot_module_read_byte(m, &via[VIA_PRB]);
    snapshot_module_read_byte(m, &via[VIA_DDRB]);
    {
        addr = VIA_DDRA;
	byte = via[VIA_PRA] | ~via[VIA_DDRA];

#ifdef HAVE_PRINTER
	pruser_write_data(byte);
#endif
	oldpa = byte;

	addr = VIA_DDRB;
	byte = via[VIA_PRB] | ~via[VIA_DDRB];

        parallel_cpu_set_nrfd(!(byte & 0x02));
        parallel_cpu_set_atn(!(byte & 0x04));
	oldpb = byte;
    }

    snapshot_module_read_word(m, &word);
    viatal = word;
    snapshot_module_read_word(m, &word);
    viatau = rclk + word + 2 /* 3 */ + TAUOFFSET;
    viatai = rclk + word + 1;

    snapshot_module_read_byte(m, &byte);
    viatbl = byte;
    snapshot_module_read_word(m, &word);
    viatbu = rclk + word + 2 /* 3 */;
    viatbi = rclk + word + 1;

    snapshot_module_read_byte(m, &byte);
    if (byte & 0x80) {
    	maincpu_set_alarm_clk(A_VIAT1, viatai);
    } else {
	viatai = 0;
    }
    if (byte & 0x40) {
    	maincpu_set_alarm_clk(A_VIAT2, viatbi);
    } else {
	viatbi = 0;
    }

    snapshot_module_read_byte(m, &via[VIA_SR]);
    {
	addr = via[VIA_SR];
	byte = via[addr];

	store_petsnd_sample(byte);
    }
    snapshot_module_read_byte(m, &via[VIA_ACR]);
    snapshot_module_read_byte(m, &via[VIA_PCR]);
    {
	addr = via[VIA_PCR];
	byte = via[addr];
	
    }

    snapshot_module_read_byte(m, &byte);
    viaifr = byte;
    snapshot_module_read_byte(m, &byte);
    viaier = byte;

    via_restore_int(viaifr & viaier & 0x7f);

    /* FIXME! */
    snapshot_module_read_byte(m, &byte);
    viapb7 = byte ? 1 : 0;
    viapb7x = 0;
    viapb7o = 0;
    snapshot_module_read_byte(m, &byte);	/* SRHBITS */

    snapshot_module_read_byte(m, &byte);	/* CABSTATE */
    ca2_state = byte & 0x80;
    cb2_state = byte & 0x40;

    snapshot_module_read_byte(m, &via_ila);
    snapshot_module_read_byte(m, &via_ilb);

/*
printf("via: read: clk=%d, tai=%d, tau=%d\n"
       "     : tbi=%d, tbu=%d\n",
		clk, viatai, viatau, viatbi, viatbu);
printf("     : ta=%d, tb=%d\n",viata() & 0xffff, viatb() & 0xffff);
*/
    return snapshot_module_close(m);
}


#ifdef HAVE_PRINTER
void pruser_set_busy(int b)
{
    via_signal(VIA_SIG_CA1, b ? VIA_SIG_RISE : VIA_SIG_FALL);
}
#endif

