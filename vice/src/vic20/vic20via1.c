
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

#define update_via1irq() \
        maincpu_set_irq(I_VIA1FL, (via1ifr & via1ier & 0x7f) ? IK_IRQ : 0)

/* the next two are used in read_via1() */
#define	via1ta() \
	((clk<via1tau-TAUOFFSET)?(via1tau-TAUOFFSET-clk-2): \
	(via1tal-(clk-via1tau+TAUOFFSET)%(via1tal+2)))

#define	via1tb() \
	(via1tbu-clk-2)

#define	update_via1tal() \
	via1pb7x = 0; \
	via1pb7xx= 0; \
	if(rclk>via1tau) { \
	  int nuf = (via1tal + 1 + rclk-via1tau)/(via1tal+2); \
/*  printf("rclk=%d, tau=%d, nuf=%d, tal=%d, pb7=%d ->",rclk,via1tau,nuf,via1tal ,via1pb7); */\
	  if(!(via1[VIA_ACR] & 0x40)) { \
	    if(((nuf-via1pb7sx)>1) || (!via1pb7)) { \
	      via1pb7o = 1; \
	      via1pb7sx= 0; \
	    } \
	  } \
	  via1pb7 ^= (nuf & 1); \
	\
	  via1tau=TAUOFFSET+via1tal+2+(rclk-(rclk-via1tau+TAUOFFSET)%(via1tal+2)); \
	  if(rclk == via1tau - via1tal - 1) { \
	    via1pb7xx = 1; \
	  }\
	}\
	if(via1tau == rclk) { \
	  via1pb7x = 1;\
	} \
/* printf(" pb7=%d, pb7x=%d, pb7o=%d, tau=%d\n",via1pb7, via1pb7x,via1pb7o, via1tau);*/\
	via1tal = via1[VIA_T1LL] + (via1[VIA_T1LH] << 8)

#define	update_via1tbl() \
	via1tbl = via1[VIA_T2CL] + (via1[VIA_T2CH] << 8)

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

#include "interrupt.h"

/*#define VIA1_TIMER_DEBUG */
/*#define VIA1_NEED_PB7 */	/* when PB7 is really used, set this
				   to enable pulse output from the timer.
				   Otherwise PB7 state is computed only
				   when port B is read -
				not yet implemented */

/* global */

BYTE    via1[16];

extern int keyarr[KBD_ROWS];

/*
 * local functions
 */

/*
 * Local variables
 */

static int   		via1ifr;   /* Interrupt Flag register for via1 */
static int   		via1ier;   /* Interrupt Enable register for via1 */

static unsigned int   	via1tal;   /* current timer A latch value */
static unsigned int   	via1tbl;   /* current timer B latch value */

static CLOCK 		via1tau;   /* time when via1 timer A is updated */
static CLOCK 		via1tbu;   /* time when via1 timer B is updated */
static CLOCK 		via1tai;   /* time when next timer A alarm is */
static CLOCK 		via1tbi;   /* time when next timer A alarm is */

static int 		via1pb7;   /* state of PB7 for pulse output... */
static int 		via1pb7x;  /* to be xored herewith  */
static int 		via1pb7o;  /* to be ored herewith  */
static int 		via1pb7xx;
static int 		via1pb7sx;

/* ------------------------------------------------------------------------- */
/* VIA1 */



/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void    reset_via1(void)
{
    int i;
#ifdef VIA1_TIMER_DEBUG
    if(app_resources.debugFlag) printf("VIA1: reset\n");
#endif
    /* clear registers */
    for(i=0;i<4;i++) via1[i]=0;
    for(i=11;i<16;i++) via1[i]=0;

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


	iec_pcr_write(0x22);

}

void via1_signal(int line, int edge) {
        switch(line) {
        case VIA_SIG_CA1:
                via1ifr |= ((edge ^ via1[VIA_PCR]) & 0x01) ?
                                                        0 : VIA_IM_CA1;
                update_via1irq();
                break;
        case VIA_SIG_CA2:
                if( !(via1[VIA_PCR] & 0x08)) {
                  via1ifr |= (((edge<<2) ^ via1[VIA_PCR]) & 0x04) ?
                                                        0 : VIA_IM_CA2;
                  update_via1irq();
                }
                break;
        case VIA_SIG_CB1:
                via1ifr |= (((edge<<4) ^ via1[VIA_PCR]) & 0x10) ?
                                                        0 : VIA_IM_CB1;
                update_via1irq();
                break;
        case VIA_SIG_CB2:
                if( !(via1[VIA_PCR] & 0x80)) {
                  via1ifr |= (((edge<<6) ^ via1[VIA_PCR]) & 0x40) ?
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
    if ((addr<10 && addr>3) || (addr==VIA_ACR) || app_resources.debugFlag)
	printf("store via1[%x] %x, rmwf=%d, clk=%d, rclk=%d\n",
		(int) addr, (int) byte, rmw_flag, clk, rclk);
#endif

    switch (addr) {

	/* these are done with saving the value */
      case VIA_PRA: /* port A */
        via1ifr &= ~VIA_IM_CA1;
        if( (via1[VIA_PCR] & 0x0a) != 0x2) {
          via1ifr &= ~VIA_IM_CA2;
        }
        update_via1irq();

      case VIA_PRA_NHS: /* port A, no handshake */
	via1[VIA_PRA_NHS] = byte;
	addr = VIA_PRA;
      case VIA_DDRA:
	via1[addr] = byte;
	break;

      case VIA_PRB: /* port B */
        via1ifr &= ~VIA_IM_CB1;
        if( (via1[VIA_PCR] & 0xa0) != 0x20) {
          via1ifr &= ~VIA_IM_CB2;
        }
        update_via1irq();

      case VIA_DDRB:
	via1[addr] = byte;
	break;

      case VIA_SR: /* Serial Port output buffer */
	via1[addr] = byte;
	
	break;

	/* Timers */

      case VIA_T1CL:
      case VIA_T1LL:
	via1[VIA_T1LL] = byte;
	update_via1tal();
	break;

      case VIA_T1CH /*TIMER_AH*/: /* Write timer A high */
#ifdef VIA1_TIMER_DEBUG
	if(app_resources.debugFlag) printf("Write timer A high: %02x\n",byte);
#endif
	via1[VIA_T1LH] = byte;
	update_via1tal();
        /* load counter with latch value */
	via1tau = rclk + via1tal + 3 + TAUOFFSET;
	via1tai = rclk + via1tal + 2;
        maincpu_set_alarm_clk(A_VIA1T1, via1tai);

	/* set pb7 state */
	via1pb7 = 0;
	via1pb7o= 0;

        /* Clear T1 interrupt */
        via1ifr &= ~VIA_IM_T1;
        update_via1irq();
        break;

      case VIA_T1LH: /* Write timer A high order latch */
        via1[addr] = byte;
	update_via1tal();

        /* Clear T1 interrupt */
        via1ifr &= ~VIA_IM_T1;
	update_via1irq();
        break;

      case VIA_T2LL:	/* Write timer 2 low latch */
	via1[VIA_T2LL] = byte;
	update_via1tbl();
	
	break;

      case VIA_T2CH: /* Write timer 2 high */
        via1[VIA_T2CH] = byte;
	update_via1tbl();
	via1tbu = rclk + via1tbl + 3;
	via1tbi = rclk + via1tbl + 2;
        maincpu_set_alarm(A_VIA1T2, via1tbi);

        /* Clear T2 interrupt */
        via1ifr &= ~VIA_IM_T2;
	update_via1irq();
        break;

	/* Interrupts */

      case VIA_IFR: /* 6522 Interrupt Flag Register */
        via1ifr &= ~byte;
	update_via1irq();
        break;

      case VIA_IER: /* Interrupt Enable Register */
#if defined (VIA1_TIMER_DEBUG)
        printf ("Via#1 set VIA_IER: 0x%x\n", byte);
#endif
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
            via1ier |= byte & 0x7f;
        }
        else {
            /* clear interrupts */
            via1ier &= ~byte;
        }
	update_via1irq();
        break;

	/* Control */

      case VIA_ACR:
	/* bit 7 timer 1 output to PB7 */
	update_via1tal();
	if((via1[VIA_ACR] ^ byte) & 0x80) {
	  if(byte & 0x80) {
	    via1pb7 = 1 ^ via1pb7x;
	  }
	}
	if((via1[VIA_ACR] ^ byte) & 0x40) {
/* printf("store_acr (%02x): pb7=%d, pb7x=%d, pb7o=%d, pb7xx=%d pb7sx=%d->",
byte, via1pb7, via1pb7x, via1pb7o, via1pb7xx, via1pb7sx);*/
	  via1pb7 ^= via1pb7sx;
          if((byte & 0x40)) {
            if(via1pb7x || via1pb7xx) {
	      if(via1tal) {
                via1pb7o = 1;
	      } else {
                via1pb7o = 0;
               if((via1[VIA_ACR]&0x80) && via1pb7x && (!via1pb7xx)) via1pb7 ^= 1;
	      }
            }
	  }
/*printf("pb7o=%d\n",via1pb7o);*/
	}
	via1pb7sx = via1pb7x;
	via1[addr] = byte;

	

	/* bit 5 timer 2 count mode */
	if (byte & 32) {
/* TODO */
/*	    update_via1tb(0);*/	/* stop timer if mode == 1 */
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

    }  /* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_via1(ADDRESS addr)
{
#ifdef VIA1_TIMER_DEBUG
    BYTE read_via1_(ADDRESS);
    BYTE retv = read_via1_(addr);
    addr &= 0x0f;
    if((addr>3 && addr<10) || app_resources.debugFlag)
	printf("read_via1(%x) -> %02x, clk=%d\n",addr,retv,clk);
    return retv;
}
BYTE REGPARM1 read_via1_(ADDRESS addr)
{
#endif
    CLOCK rclk = clk;

    addr &= 0xf;

    if(via1tai && (via1tai <= clk)) int_via1t1(clk - via1tai);
    if(via1tbi && (via1tbi <= clk)) int_via1t2(clk - via1tbi);

    switch (addr) {

      case VIA_PRA: /* port A */
        via1ifr &= ~VIA_IM_CA1;
        if( (via1[VIA_PCR] & 0x0a) != 0x02) {
          via1ifr &= ~VIA_IM_CA2;
        }
        update_via1irq();

      case VIA_PRA_NHS: /* port A, no handshake */

    {
	/* FIXME: not 100% sure about this... */
	BYTE val = ~via1[VIA_DDRA];
	BYTE msk = via1[VIA_PRB] | ~via1[VIA_DDRB];
	BYTE m;
	int i;

	for (m = 0x1, i = 0; i < KBD_ROWS; m <<= 1, i++)
	    if (!(msk & m))
		val &= ~rev_keyarr[i];

	return val | (via1[VIA_PRA] & via1[VIA_DDRA]);
    }

      case VIA_PRB: /* port B */
        via1ifr &= ~VIA_IM_CB1;
        if( (via1[VIA_PCR] & 0xa0) != 0x20) {
          via1ifr &= ~VIA_IM_CB2;
        }
        update_via1irq();
	{
	  BYTE byte;

    {
	/* FIXME: not 100% sure about this... */
        BYTE val = ~via1[VIA_DDRB];
	BYTE msk = via1[VIA_PRA] | ~via1[VIA_DDRA];
	int m, i;

	for (m = 0x1, i = 0; i < KBD_COLS; m <<= 1, i++)
	    if (!(msk & m))
	        val &= ~keyarr[i];

	/* Bit 7 is mapped to the right direction of the joystick (bit
	   3 in `joy[]'). */
	if ((joy[1] | joy[2]) & 0x8)
	    val &= 0x7f;

	byte = val | (via1[VIA_PRB] & via1[VIA_DDRB]);
    }
	  if(via1[VIA_ACR] & 0x80) {
	    update_via1tal();
/*printf("read: rclk=%d, pb7=%d, pb7o=%d, pb7ox=%d, pb7x=%d, pb7xx=%d\n",
               rclk, via1pb7, via1pb7o, via1pb7ox, via1pb7x, via1pb7xx);*/
	    byte = (byte & 0x7f) | (((via1pb7 ^ via1pb7x) | via1pb7o) ? 0x80 : 0);
	  }
	  return byte;
	}

	/* Timers */

      case VIA_T1CL /*TIMER_AL*/: /* timer A low */
        via1ifr &= ~VIA_IM_T1;
	update_via1irq();
	return via1ta() & 0xff;

      case VIA_T1CH /*TIMER_AH*/: /* timer A high */
	return (via1ta() >> 8) & 0xff;

      case VIA_T2CL /*TIMER_BL*/: /* timer B low */
        via1ifr &= ~VIA_IM_T2;
	update_via1irq();
	return via1tb() & 0xff;

      case VIA_T2CH /*TIMER_BH*/: /* timer B high */
	return (via1tb() >> 8) & 0xff;

      case VIA_SR: /* Serial Port Shift Register */
	return (via1[addr]);

	/* Interrupts */

      case VIA_IFR: /* Interrupt Flag Register */
	{
	    BYTE    t = via1ifr;
	    if (via1ifr & via1ier /*[VIA_IER]*/)
		t |= 0x80;
	    return (t);
	}

      case VIA_IER: /* 6522 Interrupt Control Register */
	    return (via1ier /*[VIA_IER]*/ | 0x80);

    }  /* switch */

    return (via1[addr]);
}

BYTE REGPARM1 peek_via1(ADDRESS addr)
{
    CLOCK rclk = clk;

    addr &= 0xf;

    if(via1tai && (via1tai <= clk)) int_via1t1(clk - via1tai);
    if(via1tbi && (via1tbi <= clk)) int_via1t2(clk - via1tbi);

    switch (addr) {
      case VIA_PRA:
	return read_via1(VIA_PRA_NHS);

      case VIA_PRB: /* port B */
	{
	  BYTE byte;

    {
	/* FIXME: not 100% sure about this... */
        BYTE val = ~via1[VIA_DDRB];
	BYTE msk = via1[VIA_PRA] | ~via1[VIA_DDRA];
	int m, i;

	for (m = 0x1, i = 0; i < KBD_COLS; m <<= 1, i++)
	    if (!(msk & m))
	        val &= ~keyarr[i];

	/* Bit 7 is mapped to the right direction of the joystick (bit
	   3 in `joy[]'). */
	if ((joy[1] | joy[2]) & 0x8)
	    val &= 0x7f;

	byte = val | (via1[VIA_PRB] & via1[VIA_DDRB]);
    }
	  if(via1[VIA_ACR] & 0x80) {
	    update_via1tal();
/*printf("read: rclk=%d, pb7=%d, pb7o=%d, pb7ox=%d, pb7x=%d, pb7xx=%d\n",
               rclk, via1pb7, via1pb7o, via1pb7ox, via1pb7x, via1pb7xx);*/
	    byte = (byte & 0x7f) | (((via1pb7 ^ via1pb7x) | via1pb7o) ? 0x80 : 0);
	  }
	  return byte;
	}

	/* Timers */

      case VIA_T1CL /*TIMER_AL*/: /* timer A low */
	return via1ta() & 0xff;

      case VIA_T2CL /*TIMER_BL*/: /* timer B low */
	return via1tb() & 0xff;

      default:
	break;
    }  /* switch */

    return read_via1(addr);
}


/* ------------------------------------------------------------------------- */

int    int_via1t1(long offset)
{
/*    CLOCK rclk = clk - offset; */
#ifdef VIA1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("via1 timer A interrupt\n");
#endif

    if (!(via1[VIA_ACR] & 0x40)) { /* one-shot mode */
#if 0 /* defined (VIA1_TIMER_DEBUG) */
        printf ("VIA1 Timer A interrupt -- one-shot mode: next int won't happen\n");
#endif
	maincpu_unset_alarm(A_VIA1T1);		/*int_clk[I_VIA1T1] = 0;*/
	via1tai = 0;
    }
    else {		/* continuous mode */
        /* load counter with latch value */
	via1tai += via1tal + 2;
        maincpu_set_alarm_clk(A_VIA1T1, via1tai);
    }
    via1ifr |= VIA_IM_T1;
    update_via1irq();

    /* TODO: toggle PB7? */
    return 0; /*(viaier & VIA_IM_T1) ? 1:0;*/
}

/*
 * Timer B is always in one-shot mode
 */

int    int_via1t2(long offset)
{
#ifdef VIA1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("VIA1 timer B interrupt\n");
#endif
    maincpu_unset_alarm(A_VIA1T2);	/*int_clk[I_VIA1T2] = 0;*/

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
     if(via1tai) via1tai -= sub;
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


