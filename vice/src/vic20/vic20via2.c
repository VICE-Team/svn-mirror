
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
/*
 * 01apr98 a.fachat
 * New timer code. Should be cycle-exact.
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
#define	TAUOFFSET	-1

#define update_via2irq() \
        maincpu_set_nmi(I_VIA2FL, (via2ifr & via2ier & 0x7f) ? IK_NMI : 0)

/* the next two are used in read_via2() */
#define	via2ta() \
	((clk<via2tau-TAUOFFSET)?(via2tau-TAUOFFSET-clk-2): \
	(via2tal-(clk-via2tau+TAUOFFSET)%(via2tal+2)))

#define	via2tb() \
	(via2tbu-clk-2)

#define	update_via2tal() \
	via2pb7x = 0; \
	via2pb7xx= 0; \
	if(rclk>via2tau) { \
	  int nuf = (via2tal + 1 + rclk-via2tau)/(via2tal+2); \
/*  printf("rclk=%d, tau=%d, nuf=%d, tal=%d, pb7=%d ->",rclk,via2tau,nuf,via2tal ,via2pb7); */\
	  if(!(via2[VIA_ACR] & 0x40)) { \
	    if(((nuf-via2pb7sx)>1) || (!via2pb7)) { \
	      via2pb7o = 1; \
	      via2pb7sx= 0; \
	    } \
	  } \
	  via2pb7 ^= (nuf & 1); \
	\
	  via2tau=TAUOFFSET+via2tal+2+(rclk-(rclk-via2tau+TAUOFFSET)%(via2tal+2)); \
	  if(rclk == via2tau - via2tal - 1) { \
	    via2pb7xx = 1; \
	  }\
	}\
	if(via2tau == rclk) { \
	  via2pb7x = 1;\
	} \
/* printf(" pb7=%d, pb7x=%d, pb7o=%d, tau=%d\n",via2pb7, via2pb7x,via2pb7o, via2tau);*/\
	via2tal = via2[VIA_T1LL] + (via2[VIA_T1LH] << 8)

#define	update_via2tbl() \
	via2tbl = via2[VIA_T2CL] + (via2[VIA_T2CH] << 8)

#include "vice.h"

#include <stdio.h>
#include <time.h>

#include "vmachine.h"
#include "via.h"
#include "resources.h"


    #include "true1541.h"
    #include "kbd.h"
    #include "vic20iec.h"
    #include "vic20via.h"
    #include "pruser.h"

#include "interrupt.h"

/*#define VIA2_TIMER_DEBUG */
/*#define VIA2_NEED_PB7 */	/* when PB7 is really used, set this
				   to enable pulse output from the timer.
				   Otherwise PB7 state is computed only
				   when port B is read -
				not yet implemented */

/* global */

BYTE    via2[16];



/*
 * local functions
 */

/*
 * Local variables
 */

static int   		via2ifr;   /* Interrupt Flag register for via2 */
static int   		via2ier;   /* Interrupt Enable register for via2 */

static unsigned int   	via2tal;   /* current timer A latch value */
static unsigned int   	via2tbl;   /* current timer B latch value */

static CLOCK 		via2tau;   /* time when via2 timer A is updated */
static CLOCK 		via2tbu;   /* time when via2 timer B is updated */
static CLOCK 		via2tai;   /* time when next timer A alarm is */
static CLOCK 		via2tbi;   /* time when next timer A alarm is */

static int 		via2pb7;   /* state of PB7 for pulse output... */
static int 		via2pb7x;  /* to be xored herewith  */
static int 		via2pb7o;  /* to be ored herewith  */
static int 		via2pb7xx;
static int 		via2pb7sx;

/* ------------------------------------------------------------------------- */
/* VIA2 */



/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void    reset_via2(void)
{
    int i;
#ifdef VIA2_TIMER_DEBUG
    if(app_resources.debugFlag) printf("VIA2: reset\n");
#endif
    /* clear registers */
    for(i=0;i<4;i++) via2[i]=0;
    for(i=11;i<16;i++) via2[i]=0;

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


     iec_pa_write(0xff);

     userport_printer_write_data(0xff);
     userport_printer_write_strobe(1);

}

void via2_signal(int line, int edge) {
        switch(line) {
        case VIA_SIG_CA1:
                via2ifr |= ((edge ^ via2[VIA_PCR]) & 0x01) ?
                                                        0 : VIA_IM_CA1;
                update_via2irq();
                break;
        case VIA_SIG_CA2:
                if( !(via2[VIA_PCR] & 0x08)) {
                  via2ifr |= (((edge<<2) ^ via2[VIA_PCR]) & 0x04) ?
                                                        0 : VIA_IM_CA2;
                  update_via2irq();
                }
                break;
        case VIA_SIG_CB1:
                via2ifr |= (((edge<<4) ^ via2[VIA_PCR]) & 0x10) ?
                                                        0 : VIA_IM_CB1;
                update_via2irq();
                break;
        case VIA_SIG_CB2:
                if( !(via2[VIA_PCR] & 0x80)) {
                  via2ifr |= (((edge<<6) ^ via2[VIA_PCR]) & 0x40) ?
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
    if ((addr<10 && addr>3) || (addr==VIA_ACR) || app_resources.debugFlag)
	printf("store via2[%x] %x, rmwf=%d, clk=%d, rclk=%d\n",
		(int) addr, (int) byte, rmw_flag, clk, rclk);
#endif

    switch (addr) {

	/* these are done with saving the value */
      case VIA_PRA: /* port A */
        via2ifr &= ~VIA_IM_CA1;
        if( (via2[VIA_PCR] & 0x0a) != 0x2) {
          via2ifr &= ~VIA_IM_CA2;
        }
        update_via2irq();

      case VIA_PRA_NHS: /* port A, no handshake */
	via2[VIA_PRA_NHS] = byte;
	addr = VIA_PRA;
      case VIA_DDRA:

     via2[addr] = byte;
     iec_pa_write(via2[VIA_PRA] | (~via2[VIA_DDRA]));
	break;

      case VIA_PRB: /* port B */
        via2ifr &= ~VIA_IM_CB1;
        if( (via2[VIA_PCR] & 0xa0) != 0x20) {
          via2ifr &= ~VIA_IM_CB2;
        }
        update_via2irq();

      case VIA_DDRB:

     via2[addr] = byte;
     userport_printer_write_data(via2[VIA_PRB] | (~via2[VIA_DDRB]));
	break;

      case VIA_SR: /* Serial Port output buffer */
	via2[addr] = byte;
	
	break;

	/* Timers */

      case VIA_T1CL:
      case VIA_T1LL:
	via2[VIA_T1LL] = byte;
	update_via2tal();
	break;

      case VIA_T1CH /*TIMER_AH*/: /* Write timer A high */
#ifdef VIA2_TIMER_DEBUG
	if(app_resources.debugFlag) printf("Write timer A high: %02x\n",byte);
#endif
	via2[VIA_T1LH] = byte;
	update_via2tal();
        /* load counter with latch value */
	via2tau = rclk + via2tal + 3 + TAUOFFSET;
	via2tai = rclk + via2tal + 2;
        maincpu_set_alarm_clk(A_VIA2T1, via2tai);

	/* set pb7 state */
	via2pb7 = 0;
	via2pb7o= 0;

        /* Clear T1 interrupt */
        via2ifr &= ~VIA_IM_T1;
        update_via2irq();
        break;

      case VIA_T1LH: /* Write timer A high order latch */
        via2[addr] = byte;
	update_via2tal();

        /* Clear T1 interrupt */
        via2ifr &= ~VIA_IM_T1;
	update_via2irq();
        break;

      case VIA_T2LL:	/* Write timer 2 low latch */
	via2[VIA_T2LL] = byte;
	update_via2tbl();
	
	break;

      case VIA_T2CH: /* Write timer 2 high */
        via2[VIA_T2CH] = byte;
	update_via2tbl();
	via2tbu = rclk + via2tbl + 3;
	via2tbi = rclk + via2tbl + 2;
        maincpu_set_alarm(A_VIA2T2, via2tbi);

        /* Clear T2 interrupt */
        via2ifr &= ~VIA_IM_T2;
	update_via2irq();
        break;

	/* Interrupts */

      case VIA_IFR: /* 6522 Interrupt Flag Register */
        via2ifr &= ~byte;
	update_via2irq();
        break;

      case VIA_IER: /* Interrupt Enable Register */
#if defined (VIA2_TIMER_DEBUG)
        printf ("Via#1 set VIA_IER: 0x%x\n", byte);
#endif
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
            via2ier |= byte & 0x7f;
        }
        else {
            /* clear interrupts */
            via2ier &= ~byte;
        }
	update_via2irq();
        break;

	/* Control */

      case VIA_ACR:
	/* bit 7 timer 1 output to PB7 */
	update_via2tal();
	if((via2[VIA_ACR] ^ byte) & 0x80) {
	  if(byte & 0x80) {
	    via2pb7 = 1 ^ via2pb7x;
	  }
	}
	if((via2[VIA_ACR] ^ byte) & 0x40) {
/* printf("store_acr (%02x): pb7=%d, pb7x=%d, pb7o=%d, pb7xx=%d pb7sx=%d->",
byte, via2pb7, via2pb7x, via2pb7o, via2pb7xx, via2pb7sx);*/
	  via2pb7 ^= via2pb7sx;
          if((byte & 0x40)) {
            if(via2pb7x || via2pb7xx) {
	      if(via2tal) {
                via2pb7o = 1;
	      } else {
                via2pb7o = 0;
               if((via2[VIA_ACR]&0x80) && via2pb7x && (!via2pb7xx)) via2pb7 ^= 1;
	      }
            }
	  }
/*printf("pb7o=%d\n",via2pb7o);*/
	}
	via2pb7sx = via2pb7x;
	via2[addr] = byte;

	

	/* bit 5 timer 2 count mode */
	if (byte & 32) {
/* TODO */
/*	    update_via2tb(0);*/	/* stop timer if mode == 1 */
	}

	/* bit 4, 3, 2 shift register control */

	/* bit 1, 0  latch enable port B and A */
	break;

      case VIA_PCR:

/*        if(viadebug) printf("VIA1: write %02x to PCR\n",byte);*/

	/* bit 7, 6, 5  CB2 handshake/interrupt control */
	/* bit 4  CB1 interrupt control */

	/* bit 3, 2, 1  CA2 handshake/interrupt control */
	/* bit 0  CA1 interrupt control */


        if(byte != via2[VIA_PCR]) {
          register BYTE tmp = byte;
          /* first set bit 1 and 5 to the real output values */
          if((tmp & 0x0c) != 0x0c) tmp |= 0x02;
          if((tmp & 0xc0) != 0xc0) tmp |= 0x20;
                                     /* switching userport strobe with CB2 */
          userport_printer_write_strobe( byte & 0x20 );
        }
	via2[addr] = byte;
	break;

      default:
	via2[addr] = byte;

    }  /* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_via2(ADDRESS addr)
{
#ifdef VIA2_TIMER_DEBUG
    BYTE read_via2_(ADDRESS);
    BYTE retv = read_via2_(addr);
    addr &= 0x0f;
    if((addr>3 && addr<10) || app_resources.debugFlag)
	printf("read_via2(%x) -> %02x, clk=%d\n",addr,retv,clk);
    return retv;
}
BYTE REGPARM1 read_via2_(ADDRESS addr)
{
#endif
    CLOCK rclk = clk;

    addr &= 0xf;

    if(via2tai && (via2tai <= clk)) int_via2t1(clk - via2tai);
    if(via2tbi && (via2tbi <= clk)) int_via2t2(clk - via2tbi);

    switch (addr) {

      case VIA_PRA: /* port A */
        via2ifr &= ~VIA_IM_CA1;
        if( (via2[VIA_PCR] & 0x0a) != 0x02) {
          via2ifr &= ~VIA_IM_CA2;
        }
        update_via2irq();

      case VIA_PRA_NHS: /* port A, no handshake */

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
	     bit 6  IEC ATN

	  */

	  /* Setup joy bits (2 through 5).  Use the `or' of the values
             of both joysticks so that it works with every joystick
             setting.  This is a bit slow... we might think of a
             faster method.  */
	  joy_bits = ~(joy[1] | joy[2]);
	  joy_bits = ((joy_bits & 0x7) << 2) | ((joy_bits & 0x10) << 1);

	  /* We assume `iec_pa_read()' returns the non-IEC bits
             as zeroes. */
	  return ((via2[VIA_PRA] & via2[VIA_DDRA])
		  | ((iec_pa_read() | joy_bits) & ~via2[VIA_DDRA]));
     }

      case VIA_PRB: /* port B */
        via2ifr &= ~VIA_IM_CB1;
        if( (via2[VIA_PCR] & 0xa0) != 0x20) {
          via2ifr &= ~VIA_IM_CB2;
        }
        update_via2irq();
	{
	  BYTE byte;

     byte = (via2[VIA_PRB] & via2[VIA_DDRB]) | (0xff & ~via2[VIA_DDRB]);
	  if(via2[VIA_ACR] & 0x80) {
	    update_via2tal();
/*printf("read: rclk=%d, pb7=%d, pb7o=%d, pb7ox=%d, pb7x=%d, pb7xx=%d\n",
               rclk, via2pb7, via2pb7o, via2pb7ox, via2pb7x, via2pb7xx);*/
	    byte = (byte & 0x7f) | (((via2pb7 ^ via2pb7x) | via2pb7o) ? 0x80 : 0);
	  }
	  return byte;
	}

	/* Timers */

      case VIA_T1CL /*TIMER_AL*/: /* timer A low */
        via2ifr &= ~VIA_IM_T1;
	update_via2irq();
	return via2ta() & 0xff;

      case VIA_T1CH /*TIMER_AH*/: /* timer A high */
	return (via2ta() >> 8) & 0xff;

      case VIA_T2CL /*TIMER_BL*/: /* timer B low */
        via2ifr &= ~VIA_IM_T2;
	update_via2irq();
	return via2tb() & 0xff;

      case VIA_T2CH /*TIMER_BH*/: /* timer B high */
	return (via2tb() >> 8) & 0xff;

      case VIA_SR: /* Serial Port Shift Register */
	return (via2[addr]);

	/* Interrupts */

      case VIA_IFR: /* Interrupt Flag Register */
	{
	    BYTE    t = via2ifr;
	    if (via2ifr & via2ier /*[VIA_IER]*/)
		t |= 0x80;
	    return (t);
	}

      case VIA_IER: /* 6522 Interrupt Control Register */
	    return (via2ier /*[VIA_IER]*/ | 0x80);

    }  /* switch */

    return (via2[addr]);
}


/* ------------------------------------------------------------------------- */

int    int_via2t1(long offset)
{
/*    CLOCK rclk = clk - offset; */
#ifdef VIA2_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("via2 timer A interrupt\n");
#endif

    if (!(via2[VIA_ACR] & 0x40)) { /* one-shot mode */
#if 0 /* defined (VIA2_TIMER_DEBUG) */
        printf ("VIA2 Timer A interrupt -- one-shot mode: next int won't happen\n");
#endif
	maincpu_unset_alarm(A_VIA2T1);		/*int_clk[I_VIA2T1] = 0;*/
	via2tai = 0;
    }
    else {		/* continuous mode */
        /* load counter with latch value */
	via2tai += via2tal + 2;
        maincpu_set_alarm_clk(A_VIA2T1, via2tai);
    }
    via2ifr |= VIA_IM_T1;
    update_via2irq();

    /* TODO: toggle PB7? */
    return 0; /*(viaier & VIA_IM_T1) ? 1:0;*/
}

/*
 * Timer B is always in one-shot mode
 */

int    int_via2t2(long offset)
{
#ifdef VIA2_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("VIA2 timer B interrupt\n");
#endif
    maincpu_unset_alarm(A_VIA2T2);	/*int_clk[I_VIA2T2] = 0;*/

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
     if(via2tai) via2tai -= sub;
}



void userport_printer_set_busy(int b) {
    via2_signal(VIA_SIG_CB1, b ? VIA_SIG_RISE : VIA_SIG_FALL);
}


