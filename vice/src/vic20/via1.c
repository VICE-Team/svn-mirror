
/*
 * ./vic20/via1.c
 * This file is generated from ./via-tmpl.c and ./vic20/via1.def,
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

#define update_via1irq() \
        maincpu_set_irq(I_VIA1FL, (via1ifr & via1ier & 0x7f) ? IK_IRQ : 0)

#include "vice.h"

#include <stdio.h>
#include <time.h>

#include "vmachine.h"
#include "via.h"
#include "resources.h"


#include "true1541.h"
#include "kbd.h"

#include "interrupt.h"

/*#define VIA1_TIMER_DEBUG */

/* global */

BYTE    via1[16];
#if 0
int     via1ta_stop = 0; /* maybe 1? */
int     via1tb_stop = 0; /* maybe 1? */
int     via1ta_interrupt = 0;
int     via1tb_interrupt = 0;
#endif

extern int keyarr[KBD_ROWS];

/* local functions */

static void update_via1ta ( int );
static void update_via1tb ( int );

/*
 * Local variables
 */

static int   via1ifr;	/* Interrupt Flag register for via1 */
static int   via1ier;	/* Interrupt Enable register for via1 */

static int   via1ta;	/* value of via1 timer A at last update */
static int   via1tb;	/* value of via1 timer B at last update */

static CLOCK via1tau;	/* time when via1 timer A is updated */
static CLOCK via1tbu;	/* time when via1 timer B is updated */


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

    via1ier = 0;
    via1ifr = 0;

    /* disable vice interrupts */
#ifdef OLDIRQ
    maincpu_set_irq(I_VIA1T1, 0); maincpu_unset_alarm(A_VIA1T1);
    maincpu_set_irq(I_VIA1T2, 0); maincpu_unset_alarm(A_VIA1T2);
    maincpu_set_irq(I_VIA1SR, 0);
    maincpu_set_irq(I_VIA1FL, 0);
#else
    maincpu_unset_alarm(A_VIA1T1);
    maincpu_unset_alarm(A_VIA1T2);
    update_via1irq();
#endif


	serial_bus_pcr_write(0x22);

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
    addr &= 0xf;
#ifdef VIA1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("store via1[%x] %x\n", (int) addr, (int) byte);
#endif

    switch (addr) {

	/* these are done with saving the value */
      case VIA_PRA: /* port A */
        via1ifr &= ~VIA_IM_CA1;
        if( (via1[VIA_PCR] & 0x0a) != 0x2) {
          via1ifr &= ~VIA_IM_CA2;
        }
#ifndef OLDIRQ
        update_via1irq();
#endif
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
#ifndef OLDIRQ
        update_via1irq();
#endif
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
	break;

      case VIA_T1CH /*TIMER_AH*/: /* Write timer A high */
#ifdef VIA1_TIMER_DEBUG
	if(app_resources.debugFlag) printf("Write timer A high: %02x\n",byte);
#endif
	via1[VIA_T1LH] = byte;
        /* load counter with latch value */
        via1[VIA_T1CL] = via1[VIA_T1LL];
        via1[VIA_T1CH] = via1[VIA_T1LH];
        /* Clear T1 interrupt */
        via1ifr &= ~VIA_IM_T1;
#ifdef OLDIRQ
        maincpu_set_irq(I_VIA1T1, 0);
#else
        update_via1irq();
#endif
        update_via1ta(1);
        break;

      case VIA_T1LH: /* Write timer A high order latch */
        via1[addr] = byte;
        /* Clear T1 interrupt */
        via1ifr &= ~VIA_IM_T1;
#ifdef OLDIRQ
        maincpu_set_irq(I_VIA1T1, 0);
#else
	update_via1irq();
#endif
        break;

      case VIA_T2LL:	/* Write timer 2 low latch */
	via1[VIA_T2LL] = byte;
	
	break;

      case VIA_T2CH: /* Write timer 2 high */
        via1[VIA_T2CH] = byte;
        via1[VIA_T2CL] = via1[VIA_T2LL]; /* bogus, both are identical */
        update_via1tb(1);
        /* Clear T2 interrupt */
        via1ifr &= ~VIA_IM_T2;
#ifdef OLDIRQ
        maincpu_set_irq(I_VIA1T2, 0);
#else
	update_via1irq();
#endif
        break;

	/* Interrupts */

      case VIA_IFR: /* 6522 Interrupt Flag Register */
        via1ifr &= ~byte;
#ifdef OLDIRQ
        if(!via1ifr & VIA_IM_T1) maincpu_set_irq(I_VIA1T1, 0);
        if(!via1ifr & VIA_IM_T2) maincpu_set_irq(I_VIA1T2, 0);
#else
	update_via1irq();
#endif
        break;

      case VIA_IER: /* Interrupt Enable Register */
#if defined (VIA1_TIMER_DEBUG)
        printf ("Via#1 set VIA_IER: 0x%x\n", byte);
#endif
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
#ifdef OLDIRQ
            if ((byte & VIA_IM_T1) && (via1ifr & VIA_IM_T1)) {
                maincpu_set_irq(I_VIA1T1, IK_IRQ);
            }
            if ((byte & VIA_IM_T2) && (via1ifr & VIA_IM_T2)) {
                maincpu_set_irq(I_VIA1T2, IK_IRQ);
            }
#endif
            via1ier |= byte & 0x7f;
        }
        else {
            /* clear interrupts */
#ifdef OLDIRQ
            if( byte & VIA_IM_T1 ) maincpu_set_irq(I_VIA1T1, 0);
            if( byte & VIA_IM_T2 ) maincpu_set_irq(I_VIA1T2, 0);
#endif
            via1ier &= ~byte;
        }
#ifndef OLDIRQ
	update_via1irq();
#endif
        break;

	/* Control */

      case VIA_ACR:
	via1[addr] = byte;

	

	/* bit 7 timer 1 output to PB7 */
	/* bit 6 timer 1 run mode -- default seems to be continuous */

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
	  serial_bus_pcr_write(tmp);
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
    addr &= 0xf;
#ifdef VIA1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("read via1[%d]\n", addr);
#endif
    switch (addr) {

      case VIA_PRA: /* port A */
        via1ifr &= ~VIA_IM_CA1;
        if( (via1[VIA_PCR] & 0x0a) != 0x02) {
          via1ifr &= ~VIA_IM_CA2;
        }
#ifndef OLDIRQ
        update_via1irq();
#endif
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
#ifndef OLDIRQ
        update_via1irq();
#endif

    {
	/* FIXME: not 100% sure about this... */
        BYTE val = via1[VIA_PRB] | ~via1[VIA_DDRB];
	BYTE msk = via1[VIA_PRA] | ~via1[VIA_DDRA];
	int m, i;

	for (m = 0x1, i = 0; i < KBD_COLS; m <<= 1, i++)
	    if (!(msk & m))
	        val &= ~keyarr[i];

	/* Bit 7 is mapped to the right direction of the joystick (bit
	   3 in `joy[]'). */
	if ((joy[1] | joy[2]) & 0x8)
	    val &= 0x7f;

	return val;
    }

	/* Timers */

      case VIA_T1CL /*TIMER_AL*/: /* timer A low */
        via1ifr &= ~VIA_IM_T1;
#ifdef OLDIRQ
        maincpu_set_irq(I_VIA1T1, 0);
#else
	update_via1irq();
#endif
        return ((via1ta - clk + via1tau) & 0xff);


      case VIA_T1CH /*TIMER_AH*/: /* timer A high */
        return (((via1ta - clk + via1tau) >> 8) & 0xff);

      case VIA_T2CL /*TIMER_BL*/: /* timer B low */
        via1ifr &= ~VIA_IM_T2;
#ifdef OLDIRQ
        maincpu_set_irq(I_VIA1T2, 0);
#else
	update_via1irq();
#endif
        return ((via1tb - clk + via1tbu) & 0xff);

      case VIA_T2CH /*TIMER_BH*/: /* timer B high */
        return (((via1tb - clk + via1tbu) >> 8) & 0xff);

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


/* ------------------------------------------------------------------------- */

int    int_via1t1(long offset)
{
#ifdef VIA1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("via1 timer A interrupt\n");
#endif

    if (!(via1[VIA_ACR] & 0x40)) { /* one-shot mode */
#if defined (VIA1_TIMER_DEBUG)
        printf ("VIA1 Timer A interrupt -- one-shot mode: next int won't happen\n");
#endif
	via1ta = 0;
	via1tau = clk;
	maincpu_unset_alarm(A_VIA1T1);		/*int_clk[I_VIA1T1] = 0;*/
    }
    else {		/* continuous mode */
        /* load counter with latch value */
        via1[VIA_T1CL] = via1[VIA_T1LL];
        via1[VIA_T1CH] = via1[VIA_T1LH];
	update_via1ta(1);
/*	int_clk[I_VIA1T1] = via1tau + via1ta;*/
    }
    via1ifr |= VIA_IM_T1;
#ifdef OLDIRQ
    if(via1ier /*[VIA_IER]*/ & VIA_IM_T1 )
	maincpu_set_irq(I_VIA1T1, IK_IRQ);
#else
    update_via1irq();
#endif
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
    via1tb = 0;
    via1tbu = clk;
    maincpu_unset_alarm(A_VIA1T2);	/*int_clk[I_VIA1T2] = 0;*/

    via1ifr |= VIA_IM_T2;
#ifdef OLDIRQ
    if( via1ier & VIA_IM_T2 ) maincpu_set_irq(I_VIA1T2, IK_IRQ);
#else
    update_via1irq();
#endif

    return 0;
}


/* ------------------------------------------------------------------------- */

static void update_via1ta(int force)
{
    if(force) {
#ifdef VIA1_TIMER_DEBUG
       if(app_resources.debugFlag)
          printf("update via timer A : latch=%d, counter =%d, clk = %d\n",
                via1[VIA_T1CL] + (via1[VIA_T1CH] << 8),
                via1[VIA_T1LL] + (via1[VIA_T1LH] << 8),
                clk);
#endif
      via1ta = via1[VIA_T1CL] + (via1[VIA_T1CH] << 8);
      via1tau = clk;
      maincpu_set_alarm(A_VIA1T1, via1ta + 1);
    }
}

static void update_via1tb(int force)
{
    if(force) {
      via1tb = via1[VIA_T2CL] + (via1[VIA_T2CH] << 8);
      via1tbu = clk;
      maincpu_set_alarm(A_VIA1T2, via1tb + 1);
    }
}

void via1_prevent_clk_overflow(void)
{
    via1tau -= PREVENT_CLK_OVERFLOW_SUB;
    via1tbu -= PREVENT_CLK_OVERFLOW_SUB;
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


