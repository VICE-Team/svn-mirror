
/*
 * ./viad2.c
 * This file is generated from ./via-tmpl.c and ./viad2.def,
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
 * into one interrupt flag, I_VIAD2FL.
 * if an interrupt condition changes, the function (i.e. cpp macro)
 * update_viaD2irq() id called, that checks the IRQ line state.
 * This is now possible, as ettore has decoupled A_* alarm events
 * from I_* interrupts for performance reasons.
 *
 * A new function for signaling rising/falling edges on the
 * control lines is introduced:
 *      viaD2_signal(VIA_SIG_[CA1|CA2|CB1|CB2], VIA_SIG_[RISE|FALL])
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
 viaD2t*u* clk ------------------------------------------+   +--------
                                                     |
                                                     |
                                                  call of
			                        int_viaD2*
                                                   here

   real viaD2tau value = viaD2tau* + TAUOFFSET
   viaD2tbu = viaD2tbu* + 0

 *
 * IRQ and PB7 are set/toggled at the low-high transition of Phi2,
 * but int_* is called a half-cycle before that. Does that matter?
 *
 * PB7 output is still to be implemented
 */

/* timer values do not depend on a certain value here, but PB7 does... */
#define	TAUOFFSET	-1

#define update_viaD2irq() \
        true1541_set_irq(I_VIAD2FL, (viaD2ifr & viaD2ier & 0x7f) ? IK_IRQ : 0)

/* the next two are used in read_viaD2() */
#define	viaD2ta() \
	((true1541_clk<viaD2tau-TAUOFFSET)?(viaD2tau-TAUOFFSET-true1541_clk-2): \
	(viaD2tal-(true1541_clk-viaD2tau+TAUOFFSET)%(viaD2tal+2)))

#define	viaD2tb() \
	(viaD2tbu-true1541_clk-2)

#define	update_viaD2tal() \
	viaD2pb7x = 0; \
	viaD2pb7xx= 0; \
	if(rclk>viaD2tau) { \
	  int nuf = (viaD2tal + 1 + rclk-viaD2tau)/(viaD2tal+2); \
/*  printf("rclk=%d, tau=%d, nuf=%d, tal=%d, pb7=%d ->",rclk,viaD2tau,nuf,viaD2tal ,viaD2pb7); */\
	  if(!(viaD2[VIA_ACR] & 0x40)) { \
	    if(((nuf-viaD2pb7sx)>1) || (!viaD2pb7)) { \
	      viaD2pb7o = 1; \
	      viaD2pb7sx= 0; \
	    } \
	  } \
	  viaD2pb7 ^= (nuf & 1); \
	\
	  viaD2tau=TAUOFFSET+viaD2tal+2+(rclk-(rclk-viaD2tau+TAUOFFSET)%(viaD2tal+2)); \
	  if(rclk == viaD2tau - viaD2tal - 1) { \
	    viaD2pb7xx = 1; \
	  }\
	}\
	if(viaD2tau == rclk) { \
	  viaD2pb7x = 1;\
	} \
/* printf(" pb7=%d, pb7x=%d, pb7o=%d, tau=%d\n",viaD2pb7, viaD2pb7x,viaD2pb7o, viaD2tau);*/\
	viaD2tal = viaD2[VIA_T1LL] + (viaD2[VIA_T1LH] << 8)

#define	update_viaD2tbl() \
	viaD2tbl = viaD2[VIA_T2CL] + (viaD2[VIA_T2CH] << 8)

#include "vice.h"

#include <stdio.h>
#include <time.h>

#include "vmachine.h"
#include "via.h"
#include "resources.h"


	#include "true1541.h"
        #include "viad.h"

#include "interrupt.h"

/*#define VIAD2_TIMER_DEBUG */
/*#define VIAD2_NEED_PB7 */	/* when PB7 is really used, set this
				   to enable pulse output from the timer.
				   Otherwise PB7 state is computed only
				   when port B is read -
				not yet implemented */

/* global */

BYTE    viaD2[16];



/*
 * local functions
 */

/*
 * Local variables
 */

static int   		viaD2ifr;   /* Interrupt Flag register for viaD2 */
static int   		viaD2ier;   /* Interrupt Enable register for viaD2 */

static unsigned int   	viaD2tal;   /* current timer A latch value */
static unsigned int   	viaD2tbl;   /* current timer B latch value */

static CLOCK 		viaD2tau;   /* time when viaD2 timer A is updated */
static CLOCK 		viaD2tbu;   /* time when viaD2 timer B is updated */
static CLOCK 		viaD2tai;   /* time when next timer A alarm is */
static CLOCK 		viaD2tbi;   /* time when next timer A alarm is */

static int 		viaD2pb7;   /* state of PB7 for pulse output... */
static int 		viaD2pb7x;  /* to be xored herewith  */
static int 		viaD2pb7o;  /* to be ored herewith  */
static int 		viaD2pb7xx;
static int 		viaD2pb7sx;

/* ------------------------------------------------------------------------- */
/* VIAD2 */



/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void    reset_viaD2(void)
{
    int i;
#ifdef VIAD2_TIMER_DEBUG
    if(app_resources.debugFlag) printf("VIAD2: reset\n");
#endif
    /* clear registers */
    for(i=0;i<4;i++) viaD2[i]=0;
    for(i=11;i<16;i++) viaD2[i]=0;

    viaD2tal = 0;
    viaD2tbl = 0;
    viaD2tau = true1541_clk;
    viaD2tbu = true1541_clk;

    viaD2ier = 0;
    viaD2ifr = 0;

    /* disable vice interrupts */
    viaD2tai = 0;
    viaD2tbi = 0;
    true1541_unset_alarm(A_VIAD2T1);
    true1541_unset_alarm(A_VIAD2T2);
    update_viaD2irq();

    

}

void viaD2_signal(int line, int edge) {
        switch(line) {
        case VIA_SIG_CA1:
                viaD2ifr |= ((edge ^ viaD2[VIA_PCR]) & 0x01) ?
                                                        0 : VIA_IM_CA1;
                update_viaD2irq();
                break;
        case VIA_SIG_CA2:
                if( !(viaD2[VIA_PCR] & 0x08)) {
                  viaD2ifr |= (((edge<<2) ^ viaD2[VIA_PCR]) & 0x04) ?
                                                        0 : VIA_IM_CA2;
                  update_viaD2irq();
                }
                break;
        case VIA_SIG_CB1:
                viaD2ifr |= (((edge<<4) ^ viaD2[VIA_PCR]) & 0x10) ?
                                                        0 : VIA_IM_CB1;
                update_viaD2irq();
                break;
        case VIA_SIG_CB2:
                if( !(viaD2[VIA_PCR] & 0x80)) {
                  viaD2ifr |= (((edge<<6) ^ viaD2[VIA_PCR]) & 0x40) ?
                                                        0 : VIA_IM_CB2;
                  update_viaD2irq();
                }
                break;
        }
}

void REGPARM2 store_viaD2(ADDRESS addr, BYTE byte)
{
    CLOCK rclk = true1541_clk - 1;	/* stores have a one-cylce offset */

    addr &= 0xf;
#ifdef VIAD2_TIMER_DEBUG
    if ((addr<10 && addr>3) || (addr==VIA_ACR) || app_resources.debugFlag)
	printf("store viaD2[%x] %x, rmwf=%d, clk=%d, rclk=%d\n",
		(int) addr, (int) byte, true1541_rmw_flag, true1541_clk, rclk);
#endif

    switch (addr) {

	/* these are done with saving the value */
      case VIA_PRA: /* port A */
        viaD2ifr &= ~VIA_IM_CA1;
        if( (viaD2[VIA_PCR] & 0x0a) != 0x2) {
          viaD2ifr &= ~VIA_IM_CA2;
        }
        update_viaD2irq();

      case VIA_PRA_NHS: /* port A, no handshake */
	viaD2[VIA_PRA_NHS] = byte;
	addr = VIA_PRA;
      case VIA_DDRA:

	viaD2[addr] = byte;
        true1541_rotate_disk(0);
	true1541_write_gcr(viaD2[VIA_PRA] | ~viaD2[VIA_DDRA]);
	break;

      case VIA_PRB: /* port B */
        viaD2ifr &= ~VIA_IM_CB1;
        if( (viaD2[VIA_PCR] & 0xa0) != 0x20) {
          viaD2ifr &= ~VIA_IM_CB2;
        }
        update_viaD2irq();

      case VIA_DDRB:

	{
	  BYTE oldval = viaD2[VIA_PRB] | ~viaD2[VIA_DDRB];

	  viaD2[addr] = byte;
	  byte = viaD2[VIA_PRB] | ~viaD2[VIA_DDRB];	/* newval */

	  true1541_led_status = byte & 8;

	  if (((oldval ^ byte) & 0x3) && (byte & 0x4))  /* Stepper motor */
	  {
	    if ((oldval & 0x3) == ((byte + 1) & 0x3))
              true1541_move_head(-1);
            else if ((oldval & 0x3) == ((byte - 1) & 0x3))
              true1541_move_head(+1);
	  }
	  if ((oldval ^ byte) & 0x60)     /* Zone bits */
	      true1541_update_zone_bits((byte >> 5) & 0x3);

	  if ((oldval ^ byte) & 0x04)     /* Motor on/off */
	      true1541_motor_control(byte & 0x04);
	}
	break;

      case VIA_SR: /* Serial Port output buffer */
	viaD2[addr] = byte;
	
	break;

	/* Timers */

      case VIA_T1CL:
      case VIA_T1LL:
	viaD2[VIA_T1LL] = byte;
	update_viaD2tal();
	break;

      case VIA_T1CH /*TIMER_AH*/: /* Write timer A high */
#ifdef VIAD2_TIMER_DEBUG
	if(app_resources.debugFlag) printf("Write timer A high: %02x\n",byte);
#endif
	viaD2[VIA_T1LH] = byte;
	update_viaD2tal();
        /* load counter with latch value */
	viaD2tau = rclk + viaD2tal + 3 + TAUOFFSET;
	viaD2tai = rclk + viaD2tal + 2;
        true1541_set_alarm_clk(A_VIAD2T1, viaD2tai);

	/* set pb7 state */
	viaD2pb7 = 0;
	viaD2pb7o= 0;

        /* Clear T1 interrupt */
        viaD2ifr &= ~VIA_IM_T1;
        update_viaD2irq();
        break;

      case VIA_T1LH: /* Write timer A high order latch */
        viaD2[addr] = byte;
	update_viaD2tal();

        /* Clear T1 interrupt */
        viaD2ifr &= ~VIA_IM_T1;
	update_viaD2irq();
        break;

      case VIA_T2LL:	/* Write timer 2 low latch */
	viaD2[VIA_T2LL] = byte;
	update_viaD2tbl();
	
	break;

      case VIA_T2CH: /* Write timer 2 high */
        viaD2[VIA_T2CH] = byte;
	update_viaD2tbl();
	viaD2tbu = rclk + viaD2tbl + 3;
	viaD2tbi = rclk + viaD2tbl + 2;
        true1541_set_alarm(A_VIAD2T2, viaD2tbi);

        /* Clear T2 interrupt */
        viaD2ifr &= ~VIA_IM_T2;
	update_viaD2irq();
        break;

	/* Interrupts */

      case VIA_IFR: /* 6522 Interrupt Flag Register */
        viaD2ifr &= ~byte;
	update_viaD2irq();
        break;

      case VIA_IER: /* Interrupt Enable Register */
#if defined (VIAD2_TIMER_DEBUG)
        printf ("Via#1 set VIA_IER: 0x%x\n", byte);
#endif
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
            viaD2ier |= byte & 0x7f;
        }
        else {
            /* clear interrupts */
            viaD2ier &= ~byte;
        }
	update_viaD2irq();
        break;

	/* Control */

      case VIA_ACR:
	/* bit 7 timer 1 output to PB7 */
	update_viaD2tal();
	if((viaD2[VIA_ACR] ^ byte) & 0x80) {
	  if(byte & 0x80) {
	    viaD2pb7 = 1 ^ viaD2pb7x;
	  }
	}
	if((viaD2[VIA_ACR] ^ byte) & 0x40) {
/* printf("store_acr (%02x): pb7=%d, pb7x=%d, pb7o=%d, pb7xx=%d pb7sx=%d->",
byte, viaD2pb7, viaD2pb7x, viaD2pb7o, viaD2pb7xx, viaD2pb7sx);*/
	  viaD2pb7 ^= viaD2pb7sx;
          if((byte & 0x40)) {
            if(viaD2pb7x || viaD2pb7xx) {
	      if(viaD2tal) {
                viaD2pb7o = 1;
	      } else {
                viaD2pb7o = 0;
               if((viaD2[VIA_ACR]&0x80) && viaD2pb7x && (!viaD2pb7xx)) viaD2pb7 ^= 1;
	      }
            }
	  }
/*printf("pb7o=%d\n",viaD2pb7o);*/
	}
	viaD2pb7sx = viaD2pb7x;
	viaD2[addr] = byte;

	

	/* bit 5 timer 2 count mode */
	if (byte & 32) {
/* TODO */
/*	    update_viaD2tb(0);*/	/* stop timer if mode == 1 */
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


        if(byte != viaD2[VIA_PCR]) {
          register BYTE tmp = byte;
          /* first set bit 1 and 5 to the real output values */
          if((tmp & 0x0c) != 0x0c) tmp |= 0x02;
          if((tmp & 0xc0) != 0xc0) tmp |= 0x20;
          /* insert_your_favourite_true1541_function_here(tmp);
	     bit 5 is the write output to the analog circuitry:
	     0 = writing, 0x20 = reading */
	     true1541_update_viad2_pcr(tmp);
          if ((byte&0x20) != (viaD2[addr]&0x20)) {
             true1541_rotate_disk(0);
             true1541_rotate_disk(1);
          }
          byte = tmp;
        }
	viaD2[addr] = byte;
	break;

      default:
	viaD2[addr] = byte;

    }  /* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_viaD2(ADDRESS addr)
{
#ifdef VIAD2_TIMER_DEBUG
    BYTE read_viaD2_(ADDRESS);
    BYTE retv = read_viaD2_(addr);
    addr &= 0x0f;
    if((addr>3 && addr<10) || app_resources.debugFlag)
	printf("read_viaD2(%x) -> %02x, clk=%d\n",addr,retv,true1541_clk);
    return retv;
}
BYTE REGPARM1 read_viaD2_(ADDRESS addr)
{
#endif
    CLOCK rclk = true1541_clk;

    addr &= 0xf;

    if(viaD2tai && (viaD2tai <= true1541_clk)) int_viaD2t1(true1541_clk - viaD2tai);
    if(viaD2tbi && (viaD2tbi <= true1541_clk)) int_viaD2t2(true1541_clk - viaD2tbi);

    switch (addr) {

      case VIA_PRA: /* port A */
        viaD2ifr &= ~VIA_IM_CA1;
        if( (viaD2[VIA_PCR] & 0x0a) != 0x02) {
          viaD2ifr &= ~VIA_IM_CA2;
        }
        update_viaD2irq();

      case VIA_PRA_NHS: /* port A, no handshake */

       return ((true1541_read_disk_byte() & ~viaD2[VIA_DDRA])
	       | (viaD2[VIA_PRA] & viaD2[VIA_DDRA]));

      case VIA_PRB: /* port B */
        viaD2ifr &= ~VIA_IM_CB1;
        if( (viaD2[VIA_PCR] & 0xa0) != 0x20) {
          viaD2ifr &= ~VIA_IM_CB2;
        }
        update_viaD2irq();
	{
	  BYTE byte;

	true1541_rotate_disk(0);

	/* I hope I got the polarities right */
	byte = (true1541_sync_found() ? 0 : 0x80)
		| (true1541_write_protect_sense() ? 0 : 0x10);
	byte = (byte & ~viaD2[VIA_DDRB])
			| (viaD2[VIA_PRB] & viaD2[VIA_DDRB]);
	  if(viaD2[VIA_ACR] & 0x80) {
	    update_viaD2tal();
/*printf("read: rclk=%d, pb7=%d, pb7o=%d, pb7ox=%d, pb7x=%d, pb7xx=%d\n",
               rclk, viaD2pb7, viaD2pb7o, viaD2pb7ox, viaD2pb7x, viaD2pb7xx);*/
	    byte = (byte & 0x7f) | (((viaD2pb7 ^ viaD2pb7x) | viaD2pb7o) ? 0x80 : 0);
	  }
	  return byte;
	}

	/* Timers */

      case VIA_T1CL /*TIMER_AL*/: /* timer A low */
        viaD2ifr &= ~VIA_IM_T1;
	update_viaD2irq();
	return viaD2ta() & 0xff;

      case VIA_T1CH /*TIMER_AH*/: /* timer A high */
	return (viaD2ta() >> 8) & 0xff;

      case VIA_T2CL /*TIMER_BL*/: /* timer B low */
        viaD2ifr &= ~VIA_IM_T2;
	update_viaD2irq();
	return viaD2tb() & 0xff;

      case VIA_T2CH /*TIMER_BH*/: /* timer B high */
	return (viaD2tb() >> 8) & 0xff;

      case VIA_SR: /* Serial Port Shift Register */
	return (viaD2[addr]);

	/* Interrupts */

      case VIA_IFR: /* Interrupt Flag Register */
	{
	    BYTE    t = viaD2ifr;
	    if (viaD2ifr & viaD2ier /*[VIA_IER]*/)
		t |= 0x80;
	    return (t);
	}

      case VIA_IER: /* 6522 Interrupt Control Register */
	    return (viaD2ier /*[VIA_IER]*/ | 0x80);

    }  /* switch */

    return (viaD2[addr]);
}


/* ------------------------------------------------------------------------- */

int    int_viaD2t1(long offset)
{
/*    CLOCK rclk = true1541_clk - offset; */
#ifdef VIAD2_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("viaD2 timer A interrupt\n");
#endif

    if (!(viaD2[VIA_ACR] & 0x40)) { /* one-shot mode */
#if 0 /* defined (VIAD2_TIMER_DEBUG) */
        printf ("VIAD2 Timer A interrupt -- one-shot mode: next int won't happen\n");
#endif
	true1541_unset_alarm(A_VIAD2T1);		/*int_clk[I_VIAD2T1] = 0;*/
	viaD2tai = 0;
    }
    else {		/* continuous mode */
        /* load counter with latch value */
	viaD2tai += viaD2tal + 2;
        true1541_set_alarm_clk(A_VIAD2T1, viaD2tai);
    }
    viaD2ifr |= VIA_IM_T1;
    update_viaD2irq();

    /* TODO: toggle PB7? */
    return 0; /*(viaier & VIA_IM_T1) ? 1:0;*/
}

/*
 * Timer B is always in one-shot mode
 */

int    int_viaD2t2(long offset)
{
#ifdef VIAD2_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("VIAD2 timer B interrupt\n");
#endif
    true1541_unset_alarm(A_VIAD2T2);	/*int_clk[I_VIAD2T2] = 0;*/

    viaD2ifr |= VIA_IM_T2;
    update_viaD2irq();

    return 0;
}

void viaD2_prevent_clk_overflow(CLOCK sub)
{
     unsigned int t;
     t = (viaD2tau - (true1541_clk + sub)) & 0xffff;
     viaD2tau = true1541_clk + t;
     t = (viaD2tbu - (true1541_clk + sub)) & 0xffff;
     viaD2tbu = true1541_clk + t;
     if(viaD2tai) viaD2tai -= sub;
}



