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
 * into one interrupt flag, I_MYVIAFL.
 * if an interrupt condition changes, the function (i.e. cpp macro)
 * update_myviairq() id called, that checks the IRQ line state.
 * This is now possible, as ettore has decoupled A_* alarm events
 * from I_* interrupts for performance reasons.
 *
 * A new function for signaling rising/falling edges on the
 * control lines is introduced:
 *      myvia_signal(VIA_SIG_[CA1|CA2|CB1|CB2], VIA_SIG_[RISE|FALL])
 * which signals the corresponding edge to the VIA. The constants
 * are defined in via.h.
 *
 * Except for shift register and input latching everything should be ok now.
 */

#define update_myviairq() \
        via_set_int(I_MYVIAFL, (myviaifr & myviaier & 0x7f) ? MYVIA_INT : 0)

#include "vice.h"

#include <stdio.h>
#include <time.h>

#include "vmachine.h"
#include "via.h"
#include "resources.h"

INCLUDES

#include "interrupt.h"

/*#define MYVIA_TIMER_DEBUG */

/* global */

BYTE    myvia[16];
#if 0
int     myviata_stop = 0; /* maybe 1? */
int     myviatb_stop = 0; /* maybe 1? */
int     myviata_interrupt = 0;
int     myviatb_interrupt = 0;
#endif

GLOBALS

/* local functions */

static void update_myviata ( int );
static void update_myviatb ( int );

/*
 * Local variables
 */

static int   myviaifr;	/* Interrupt Flag register for myvia */
static int   myviaier;	/* Interrupt Enable register for myvia */

static int   myviata;	/* value of myvia timer A at last update */
static int   myviatb;	/* value of myvia timer B at last update */

static CLOCK myviatau;	/* time when myvia timer A is updated */
static CLOCK myviatbu;	/* time when myvia timer B is updated */


/* ------------------------------------------------------------------------- */
/* MYVIA */

PRE_VIA_FUNCS

/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void    reset_myvia(void)
{
    int i;
#ifdef MYVIA_TIMER_DEBUG
    if(app_resources.debugFlag) printf("MYVIA: reset\n");
#endif
    /* clear registers */
    for(i=0;i<4;i++) myvia[i]=0;
    for(i=11;i<16;i++) myvia[i]=0;

    myviaier = 0;
    myviaifr = 0;

    /* disable vice interrupts */
#ifdef OLDIRQ
    via_set_int(I_MYVIAT1, 0); mycpu_unset_alarm(A_MYVIAT1);
    via_set_int(I_MYVIAT2, 0); mycpu_unset_alarm(A_MYVIAT2);
    via_set_int(I_MYVIASR, 0);
    via_set_int(I_MYVIAFL, 0);
#else
    mycpu_unset_alarm(A_MYVIAT1);
    mycpu_unset_alarm(A_MYVIAT2);
    update_myviairq();
#endif

    RESET_VIA

}

void myvia_signal(int line, int edge) {
        switch(line) {
        case VIA_SIG_CA1:
                myviaifr |= ((edge ^ myvia[VIA_PCR]) & 0x01) ?
                                                        0 : VIA_IM_CA1;
                update_myviairq();
                break;
        case VIA_SIG_CA2:
                if( !(myvia[VIA_PCR] & 0x08)) {
                  myviaifr |= (((edge<<2) ^ myvia[VIA_PCR]) & 0x04) ?
                                                        0 : VIA_IM_CA2;
                  update_myviairq();
                }
                break;
        case VIA_SIG_CB1:
                myviaifr |= (((edge<<4) ^ myvia[VIA_PCR]) & 0x10) ?
                                                        0 : VIA_IM_CB1;
                update_myviairq();
                break;
        case VIA_SIG_CB2:
                if( !(myvia[VIA_PCR] & 0x80)) {
                  myviaifr |= (((edge<<6) ^ myvia[VIA_PCR]) & 0x40) ?
                                                        0 : VIA_IM_CB2;
                  update_myviairq();
                }
                break;
        }
}

void REGPARM2 store_myvia(ADDRESS addr, BYTE byte)
{
    addr &= 0xf;
#ifdef MYVIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("store myvia[%x] %x\n", (int) addr, (int) byte);
#endif

    switch (addr) {

	/* these are done with saving the value */
      case VIA_PRA: /* port A */
        myviaifr &= ~VIA_IM_CA1;
        if( (myvia[VIA_PCR] & 0x0a) != 0x2) {
          myviaifr &= ~VIA_IM_CA2;
        }
#ifndef OLDIRQ
        update_myviairq();
#endif
      case VIA_PRA_NHS: /* port A, no handshake */
	myvia[VIA_PRA_NHS] = byte;
	addr = VIA_PRA;
      case VIA_DDRA:
	STORE_PRA
	break;

      case VIA_PRB: /* port B */
        myviaifr &= ~VIA_IM_CB1;
        if( (myvia[VIA_PCR] & 0xa0) != 0x20) {
          myviaifr &= ~VIA_IM_CB2;
        }
#ifndef OLDIRQ
        update_myviairq();
#endif
      case VIA_DDRB:
	STORE_PRB
	break;

      case VIA_SR: /* Serial Port output buffer */
	myvia[addr] = byte;
	STORE_SR
	break;

	/* Timers */

      case VIA_T1CL:
      case VIA_T1LL:
	myvia[VIA_T1LL] = byte;
	break;

      case VIA_T1CH /*TIMER_AH*/: /* Write timer A high */
#ifdef MYVIA_TIMER_DEBUG
	if(app_resources.debugFlag) printf("Write timer A high: %02x\n",byte);
#endif
	myvia[VIA_T1LH] = byte;
        /* load counter with latch value */
        myvia[VIA_T1CL] = myvia[VIA_T1LL];
        myvia[VIA_T1CH] = myvia[VIA_T1LH];
        /* Clear T1 interrupt */
        myviaifr &= ~VIA_IM_T1;
#ifdef OLDIRQ
        via_set_int(I_MYVIAT1, 0);
#else
        update_myviairq();
#endif
        update_myviata(1);
        break;

      case VIA_T1LH: /* Write timer A high order latch */
        myvia[addr] = byte;
        /* Clear T1 interrupt */
        myviaifr &= ~VIA_IM_T1;
#ifdef OLDIRQ
        via_set_int(I_MYVIAT1, 0);
#else
	update_myviairq();
#endif
        break;

      case VIA_T2LL:	/* Write timer 2 low latch */
	myvia[VIA_T2LL] = byte;
	STORE_T2L
	break;

      case VIA_T2CH: /* Write timer 2 high */
        myvia[VIA_T2CH] = byte;
        myvia[VIA_T2CL] = myvia[VIA_T2LL]; /* bogus, both are identical */
        update_myviatb(1);
        /* Clear T2 interrupt */
        myviaifr &= ~VIA_IM_T2;
#ifdef OLDIRQ
        via_set_int(I_MYVIAT2, 0);
#else
	update_myviairq();
#endif
        break;

	/* Interrupts */

      case VIA_IFR: /* 6522 Interrupt Flag Register */
        myviaifr &= ~byte;
#ifdef OLDIRQ
        if(!myviaifr & VIA_IM_T1) via_set_int(I_MYVIAT1, 0);
        if(!myviaifr & VIA_IM_T2) via_set_int(I_MYVIAT2, 0);
#else
	update_myviairq();
#endif
        break;

      case VIA_IER: /* Interrupt Enable Register */
#if defined (MYVIA_TIMER_DEBUG)
        printf ("Via#1 set VIA_IER: 0x%x\n", byte);
#endif
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
#ifdef OLDIRQ
            if ((byte & VIA_IM_T1) && (myviaifr & VIA_IM_T1)) {
                via_set_int(I_MYVIAT1, MYVIA_INT);
            }
            if ((byte & VIA_IM_T2) && (myviaifr & VIA_IM_T2)) {
                via_set_int(I_MYVIAT2, MYVIA_INT);
            }
#endif
            myviaier |= byte & 0x7f;
        }
        else {
            /* clear interrupts */
#ifdef OLDIRQ
            if( byte & VIA_IM_T1 ) via_set_int(I_MYVIAT1, 0);
            if( byte & VIA_IM_T2 ) via_set_int(I_MYVIAT2, 0);
#endif
            myviaier &= ~byte;
        }
#ifndef OLDIRQ
	update_myviairq();
#endif
        break;

	/* Control */

      case VIA_ACR:
	myvia[addr] = byte;

	STORE_ACR

	/* bit 7 timer 1 output to PB7 */
	/* bit 6 timer 1 run mode -- default seems to be continuous */

	/* bit 5 timer 2 count mode */
	if (byte & 32) {
/* TODO */
/*	    update_myviatb(0);*/	/* stop timer if mode == 1 */
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

	STORE_PCR
	myvia[addr] = byte;
	break;

      default:
	myvia[addr] = byte;

    }  /* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_myvia(ADDRESS addr)
{
    addr &= 0xf;
#ifdef MYVIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("read myvia[%d]\n", addr);
#endif
    switch (addr) {

      case VIA_PRA: /* port A */
        myviaifr &= ~VIA_IM_CA1;
        if( (myvia[VIA_PCR] & 0x0a) != 0x02) {
          myviaifr &= ~VIA_IM_CA2;
        }
#ifndef OLDIRQ
        update_myviairq();
#endif
      case VIA_PRA_NHS: /* port A, no handshake */
	READ_PRA

      case VIA_PRB: /* port B */
        myviaifr &= ~VIA_IM_CB1;
        if( (myvia[VIA_PCR] & 0xa0) != 0x20) {
          myviaifr &= ~VIA_IM_CB2;
        }
#ifndef OLDIRQ
        update_myviairq();
#endif
	READ_PRB

	/* Timers */

      case VIA_T1CL /*TIMER_AL*/: /* timer A low */
        myviaifr &= ~VIA_IM_T1;
#ifdef OLDIRQ
        via_set_int(I_MYVIAT1, 0);
#else
	update_myviairq();
#endif
        return ((myviata - myclk + myviatau) & 0xff);


      case VIA_T1CH /*TIMER_AH*/: /* timer A high */
        return (((myviata - myclk + myviatau) >> 8) & 0xff);

      case VIA_T2CL /*TIMER_BL*/: /* timer B low */
        myviaifr &= ~VIA_IM_T2;
#ifdef OLDIRQ
        via_set_int(I_MYVIAT2, 0);
#else
	update_myviairq();
#endif
        return ((myviatb - myclk + myviatbu) & 0xff);

      case VIA_T2CH /*TIMER_BH*/: /* timer B high */
        return (((myviatb - myclk + myviatbu) >> 8) & 0xff);

      case VIA_SR: /* Serial Port Shift Register */
	return (myvia[addr]);

	/* Interrupts */

      case VIA_IFR: /* Interrupt Flag Register */
	{
	    BYTE    t = myviaifr;
	    if (myviaifr & myviaier /*[VIA_IER]*/)
		t |= 0x80;
	    return (t);
	}

      case VIA_IER: /* 6522 Interrupt Control Register */
	    return (myviaier /*[VIA_IER]*/ | 0x80);

    }  /* switch */

    return (myvia[addr]);
}


/* ------------------------------------------------------------------------- */

int    int_myviat1(long offset)
{
#ifdef MYVIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("myvia timer A interrupt\n");
#endif

    if (!(myvia[VIA_ACR] & 0x40)) { /* one-shot mode */
#if defined (MYVIA_TIMER_DEBUG)
        printf ("MYVIA Timer A interrupt -- one-shot mode: next int won't happen\n");
#endif
	myviata = 0;
	myviatau = myclk;
	mycpu_unset_alarm(A_MYVIAT1);		/*int_clk[I_MYVIAT1] = 0;*/
    }
    else {		/* continuous mode */
        /* load counter with latch value */
        myvia[VIA_T1CL] = myvia[VIA_T1LL];
        myvia[VIA_T1CH] = myvia[VIA_T1LH];
	update_myviata(1);
/*	int_clk[I_MYVIAT1] = myviatau + myviata;*/
    }
    myviaifr |= VIA_IM_T1;
#ifdef OLDIRQ
    if(myviaier /*[VIA_IER]*/ & VIA_IM_T1 )
	via_set_int(I_MYVIAT1, MYVIA_INT);
#else
    update_myviairq();
#endif
    return 0; /*(viaier & VIA_IM_T1) ? 1:0;*/
}

/*
 * Timer B is always in one-shot mode
 */

int    int_myviat2(long offset)
{
#ifdef MYVIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("MYVIA timer B interrupt\n");
#endif
    myviatb = 0;
    myviatbu = myclk;
    mycpu_unset_alarm(A_MYVIAT2);	/*int_clk[I_MYVIAT2] = 0;*/

    myviaifr |= VIA_IM_T2;
#ifdef OLDIRQ
    if( myviaier & VIA_IM_T2 ) via_set_int(I_MYVIAT2, MYVIA_INT);
#else
    update_myviairq();
#endif

    return 0;
}


/* ------------------------------------------------------------------------- */

static void update_myviata(int force)
{
    if(force) {
#ifdef MYVIA_TIMER_DEBUG
       if(app_resources.debugFlag)
          printf("update via timer A : latch=%d, counter =%d, myclk = %d\n",
                myvia[VIA_T1CL] + (myvia[VIA_T1CH] << 8),
                myvia[VIA_T1LL] + (myvia[VIA_T1LH] << 8),
                myclk);
#endif
      myviata = myvia[VIA_T1CL] + (myvia[VIA_T1CH] << 8);
      myviatau = myclk;
      mycpu_set_alarm(A_MYVIAT1, myviata);
    }
}

static void update_myviatb(int force)
{
    if(force) {
      myviatb = myvia[VIA_T2CL] + (myvia[VIA_T2CH] << 8);
      myviatbu = myclk;
      mycpu_set_alarm(A_MYVIAT2, myviatb);
    }
}

void myvia_prevent_clk_overflow(void)
{
    myviatau -= PREVENT_CLK_OVERFLOW_SUB;
    myviatbu -= PREVENT_CLK_OVERFLOW_SUB;
}

POST_VIA_FUNCS

