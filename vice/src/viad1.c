
/*
 * ./viad1.c
 * This file is generated from ./via-tmpl.c and ./viad1.def,
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
 * into one interrupt flag, I_VIAD1FL.
 * if an interrupt condition changes, the function (i.e. cpp macro)
 * update_viaD1irq() id called, that checks the IRQ line state.
 * This is now possible, as ettore has decoupled A_* alarm events
 * from I_* interrupts for performance reasons.
 *
 * A new function for signaling rising/falling edges on the
 * control lines is introduced:
 *      viaD1_signal(VIA_SIG_[CA1|CA2|CB1|CB2], VIA_SIG_[RISE|FALL])
 * which signals the corresponding edge to the VIA. The constants
 * are defined in via.h.
 *
 * Except for shift register and input latching everything should be ok now.
 */

#define update_viaD1irq() \
        true1541_set_irq(I_VIAD1FL, (viaD1ifr & viaD1ier & 0x7f) ? IK_IRQ : 0)

#include "vice.h"

#include <stdio.h>
#include <time.h>

#include "vmachine.h"
#include "via.h"
#include "resources.h"


    #include "true1541.h"
    #include "viad.h"

#include "interrupt.h"

/*#define VIAD1_TIMER_DEBUG */

/* global */

BYTE    viaD1[16];
#if 0
int     viaD1ta_stop = 0; /* maybe 1? */
int     viaD1tb_stop = 0; /* maybe 1? */
int     viaD1ta_interrupt = 0;
int     viaD1tb_interrupt = 0;
#endif



/* local functions */

static void update_viaD1ta ( int );
static void update_viaD1tb ( int );

/*
 * Local variables
 */

static int   viaD1ifr;	/* Interrupt Flag register for viaD1 */
static int   viaD1ier;	/* Interrupt Enable register for viaD1 */

static int   viaD1ta;	/* value of viaD1 timer A at last update */
static int   viaD1tb;	/* value of viaD1 timer B at last update */

static CLOCK viaD1tau;	/* time when viaD1 timer A is updated */
static CLOCK viaD1tbu;	/* time when viaD1 timer B is updated */


/* ------------------------------------------------------------------------- */
/* VIAD1 */



void set_atn(BYTE state)
{
#ifdef OLDIRQ
   if (state) {
      viaD1ifr |= 2;
   } else {
      viaD1ifr &= 0xfe;
   }
#else
   viaD1_signal(VIA_SIG_CA1, state ? VIA_SIG_RISE : 0);
#endif
}


/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void    reset_viaD1(void)
{
    int i;
#ifdef VIAD1_TIMER_DEBUG
    if(app_resources.debugFlag) printf("VIAD1: reset\n");
#endif
    /* clear registers */
    for(i=0;i<4;i++) viaD1[i]=0;
    for(i=11;i<16;i++) viaD1[i]=0;

    viaD1ier = 0;
    viaD1ifr = 0;

    /* disable vice interrupts */
#ifdef OLDIRQ
    true1541_set_irq(I_VIAD1T1, 0); true1541_unset_alarm(A_VIAD1T1);
    true1541_set_irq(I_VIAD1T2, 0); true1541_unset_alarm(A_VIAD1T2);
    true1541_set_irq(I_VIAD1SR, 0);
    true1541_set_irq(I_VIAD1FL, 0);
#else
    true1541_unset_alarm(A_VIAD1T1);
    true1541_unset_alarm(A_VIAD1T2);
    update_viaD1irq();
#endif

    

}

void viaD1_signal(int line, int edge) {
        switch(line) {
        case VIA_SIG_CA1:
                viaD1ifr |= ((edge ^ viaD1[VIA_PCR]) & 0x01) ?
                                                        0 : VIA_IM_CA1;
                update_viaD1irq();
                break;
        case VIA_SIG_CA2:
                if( !(viaD1[VIA_PCR] & 0x08)) {
                  viaD1ifr |= (((edge<<2) ^ viaD1[VIA_PCR]) & 0x04) ?
                                                        0 : VIA_IM_CA2;
                  update_viaD1irq();
                }
                break;
        case VIA_SIG_CB1:
                viaD1ifr |= (((edge<<4) ^ viaD1[VIA_PCR]) & 0x10) ?
                                                        0 : VIA_IM_CB1;
                update_viaD1irq();
                break;
        case VIA_SIG_CB2:
                if( !(viaD1[VIA_PCR] & 0x80)) {
                  viaD1ifr |= (((edge<<6) ^ viaD1[VIA_PCR]) & 0x40) ?
                                                        0 : VIA_IM_CB2;
                  update_viaD1irq();
                }
                break;
        }
}

void REGPARM2 store_viaD1(ADDRESS addr, BYTE byte)
{
    addr &= 0xf;
#ifdef VIAD1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("store viaD1[%x] %x\n", (int) addr, (int) byte);
#endif

    switch (addr) {

	/* these are done with saving the value */
      case VIA_PRA: /* port A */
        viaD1ifr &= ~VIA_IM_CA1;
        if( (viaD1[VIA_PCR] & 0x0a) != 0x2) {
          viaD1ifr &= ~VIA_IM_CA2;
        }
#ifndef OLDIRQ
        update_viaD1irq();
#endif
      case VIA_PRA_NHS: /* port A, no handshake */
	viaD1[VIA_PRA_NHS] = byte;
	addr = VIA_PRA;
      case VIA_DDRA:

    viaD1[addr] = byte;
    if(app_resources.true1541ParallelCable)
	parallel_cable_drive_write(viaD1[VIA_PRA] | ~viaD1[VIA_DDRA],
	((addr == VIA_PRA) && ((viaD1[VIA_PCR] & 0xe) == 0xa)) ? 1 : 0);
	break;

      case VIA_PRB: /* port B */
        viaD1ifr &= ~VIA_IM_CB1;
        if( (viaD1[VIA_PCR] & 0xa0) != 0x20) {
          viaD1ifr &= ~VIA_IM_CB2;
        }
#ifndef OLDIRQ
        update_viaD1irq();
#endif
      case VIA_DDRB:

    if (byte != viaD1[addr]) {
        viaD1[addr] = byte;
#if 0				/* !FAST_BUS */
	serial_bus_drive_write(~viaD1[VIA_DDRB] | viaD1[VIA_PRB]);
#else
 	serial_bus_drive_write(viaD1[VIA_DDRB] & ~viaD1[VIA_PRB]);
#endif
    }
	break;

      case VIA_SR: /* Serial Port output buffer */
	viaD1[addr] = byte;
	
	break;

	/* Timers */

      case VIA_T1CL:
      case VIA_T1LL:
	viaD1[VIA_T1LL] = byte;
	break;

      case VIA_T1CH /*TIMER_AH*/: /* Write timer A high */
#ifdef VIAD1_TIMER_DEBUG
	if(app_resources.debugFlag) printf("Write timer A high: %02x\n",byte);
#endif
	viaD1[VIA_T1LH] = byte;
        /* load counter with latch value */
        viaD1[VIA_T1CL] = viaD1[VIA_T1LL];
        viaD1[VIA_T1CH] = viaD1[VIA_T1LH];
        /* Clear T1 interrupt */
        viaD1ifr &= ~VIA_IM_T1;
#ifdef OLDIRQ
        true1541_set_irq(I_VIAD1T1, 0);
#else
        update_viaD1irq();
#endif
        update_viaD1ta(1);
        break;

      case VIA_T1LH: /* Write timer A high order latch */
        viaD1[addr] = byte;
        /* Clear T1 interrupt */
        viaD1ifr &= ~VIA_IM_T1;
#ifdef OLDIRQ
        true1541_set_irq(I_VIAD1T1, 0);
#else
	update_viaD1irq();
#endif
        break;

      case VIA_T2LL:	/* Write timer 2 low latch */
	viaD1[VIA_T2LL] = byte;
	
	break;

      case VIA_T2CH: /* Write timer 2 high */
        viaD1[VIA_T2CH] = byte;
        viaD1[VIA_T2CL] = viaD1[VIA_T2LL]; /* bogus, both are identical */
        update_viaD1tb(1);
        /* Clear T2 interrupt */
        viaD1ifr &= ~VIA_IM_T2;
#ifdef OLDIRQ
        true1541_set_irq(I_VIAD1T2, 0);
#else
	update_viaD1irq();
#endif
        break;

	/* Interrupts */

      case VIA_IFR: /* 6522 Interrupt Flag Register */
        viaD1ifr &= ~byte;
#ifdef OLDIRQ
        if(!viaD1ifr & VIA_IM_T1) true1541_set_irq(I_VIAD1T1, 0);
        if(!viaD1ifr & VIA_IM_T2) true1541_set_irq(I_VIAD1T2, 0);
#else
	update_viaD1irq();
#endif
        break;

      case VIA_IER: /* Interrupt Enable Register */
#if defined (VIAD1_TIMER_DEBUG)
        printf ("Via#1 set VIA_IER: 0x%x\n", byte);
#endif
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
#ifdef OLDIRQ
            if ((byte & VIA_IM_T1) && (viaD1ifr & VIA_IM_T1)) {
                true1541_set_irq(I_VIAD1T1, IK_IRQ);
            }
            if ((byte & VIA_IM_T2) && (viaD1ifr & VIA_IM_T2)) {
                true1541_set_irq(I_VIAD1T2, IK_IRQ);
            }
#endif
            viaD1ier |= byte & 0x7f;
        }
        else {
            /* clear interrupts */
#ifdef OLDIRQ
            if( byte & VIA_IM_T1 ) true1541_set_irq(I_VIAD1T1, 0);
            if( byte & VIA_IM_T2 ) true1541_set_irq(I_VIAD1T2, 0);
#endif
            viaD1ier &= ~byte;
        }
#ifndef OLDIRQ
	update_viaD1irq();
#endif
        break;

	/* Control */

      case VIA_ACR:
	viaD1[addr] = byte;

	

	/* bit 7 timer 1 output to PB7 */
	/* bit 6 timer 1 run mode -- default seems to be continuous */

	/* bit 5 timer 2 count mode */
	if (byte & 32) {
/* TODO */
/*	    update_viaD1tb(0);*/	/* stop timer if mode == 1 */
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

	
	viaD1[addr] = byte;
	break;

      default:
	viaD1[addr] = byte;

    }  /* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_viaD1(ADDRESS addr)
{
    addr &= 0xf;
#ifdef VIAD1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("read viaD1[%d]\n", addr);
#endif
    switch (addr) {

      case VIA_PRA: /* port A */
        viaD1ifr &= ~VIA_IM_CA1;
        if( (viaD1[VIA_PCR] & 0x0a) != 0x02) {
          viaD1ifr &= ~VIA_IM_CA2;
        }
#ifndef OLDIRQ
        update_viaD1irq();
#endif
      case VIA_PRA_NHS: /* port A, no handshake */

    return app_resources.true1541ParallelCable ?
	parallel_cable_drive_read((((addr == VIA_PRA) &&
	(viaD1[VIA_PCR] & 0xe) == 0xa)) ? 1 : 0) :
	((viaD1[VIA_PRA] & viaD1[VIA_DDRA]) | (0xff & ~viaD1[VIA_DDRA]));


      case VIA_PRB: /* port B */
        viaD1ifr &= ~VIA_IM_CB1;
        if( (viaD1[VIA_PCR] & 0xa0) != 0x20) {
          viaD1ifr &= ~VIA_IM_CB2;
        }
#ifndef OLDIRQ
        update_viaD1irq();
#endif

#if 0				/* !FAST_BUS */
        return ((viaD1[VIA_PRB] & viaD1[VIA_DDRB]) | ((serial_bus_drive_read()) & ~viaD1[VIA_DDRB]));
#else
	return ((viaD1[VIA_PRB] & 0x1a)
		| serial_bus_drive_read()) ^ 0x85;
#endif

	/* Timers */

      case VIA_T1CL /*TIMER_AL*/: /* timer A low */
        viaD1ifr &= ~VIA_IM_T1;
#ifdef OLDIRQ
        true1541_set_irq(I_VIAD1T1, 0);
#else
	update_viaD1irq();
#endif
        return ((viaD1ta - true1541_clk + viaD1tau) & 0xff);


      case VIA_T1CH /*TIMER_AH*/: /* timer A high */
        return (((viaD1ta - true1541_clk + viaD1tau) >> 8) & 0xff);

      case VIA_T2CL /*TIMER_BL*/: /* timer B low */
        viaD1ifr &= ~VIA_IM_T2;
#ifdef OLDIRQ
        true1541_set_irq(I_VIAD1T2, 0);
#else
	update_viaD1irq();
#endif
        return ((viaD1tb - true1541_clk + viaD1tbu) & 0xff);

      case VIA_T2CH /*TIMER_BH*/: /* timer B high */
        return (((viaD1tb - true1541_clk + viaD1tbu) >> 8) & 0xff);

      case VIA_SR: /* Serial Port Shift Register */
	return (viaD1[addr]);

	/* Interrupts */

      case VIA_IFR: /* Interrupt Flag Register */
	{
	    BYTE    t = viaD1ifr;
	    if (viaD1ifr & viaD1ier /*[VIA_IER]*/)
		t |= 0x80;
	    return (t);
	}

      case VIA_IER: /* 6522 Interrupt Control Register */
	    return (viaD1ier /*[VIA_IER]*/ | 0x80);

    }  /* switch */

    return (viaD1[addr]);
}


/* ------------------------------------------------------------------------- */

int    int_viaD1t1(long offset)
{
#ifdef VIAD1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("viaD1 timer A interrupt\n");
#endif

    if (!(viaD1[VIA_ACR] & 0x40)) { /* one-shot mode */
#if defined (VIAD1_TIMER_DEBUG)
        printf ("VIAD1 Timer A interrupt -- one-shot mode: next int won't happen\n");
#endif
	viaD1ta = 0;
	viaD1tau = true1541_clk;
	true1541_unset_alarm(A_VIAD1T1);		/*int_clk[I_VIAD1T1] = 0;*/
    }
    else {		/* continuous mode */
        /* load counter with latch value */
        viaD1[VIA_T1CL] = viaD1[VIA_T1LL];
        viaD1[VIA_T1CH] = viaD1[VIA_T1LH];
	update_viaD1ta(1);
/*	int_clk[I_VIAD1T1] = viaD1tau + viaD1ta;*/
    }
    viaD1ifr |= VIA_IM_T1;
#ifdef OLDIRQ
    if(viaD1ier /*[VIA_IER]*/ & VIA_IM_T1 )
	true1541_set_irq(I_VIAD1T1, IK_IRQ);
#else
    update_viaD1irq();
#endif
    return 0; /*(viaier & VIA_IM_T1) ? 1:0;*/
}

/*
 * Timer B is always in one-shot mode
 */

int    int_viaD1t2(long offset)
{
#ifdef VIAD1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("VIAD1 timer B interrupt\n");
#endif
    viaD1tb = 0;
    viaD1tbu = true1541_clk;
    true1541_unset_alarm(A_VIAD1T2);	/*int_clk[I_VIAD1T2] = 0;*/

    viaD1ifr |= VIA_IM_T2;
#ifdef OLDIRQ
    if( viaD1ier & VIA_IM_T2 ) true1541_set_irq(I_VIAD1T2, IK_IRQ);
#else
    update_viaD1irq();
#endif

    return 0;
}


/* ------------------------------------------------------------------------- */

static void update_viaD1ta(int force)
{
    if(force) {
#ifdef VIAD1_TIMER_DEBUG
       if(app_resources.debugFlag)
          printf("update via timer A : latch=%d, counter =%d, true1541_clk = %d\n",
                viaD1[VIA_T1CL] + (viaD1[VIA_T1CH] << 8),
                viaD1[VIA_T1LL] + (viaD1[VIA_T1LH] << 8),
                true1541_clk);
#endif
      viaD1ta = viaD1[VIA_T1CL] + (viaD1[VIA_T1CH] << 8);
      viaD1tau = true1541_clk;
      true1541_set_alarm(A_VIAD1T1, viaD1ta);
    }
}

static void update_viaD1tb(int force)
{
    if(force) {
      viaD1tb = viaD1[VIA_T2CL] + (viaD1[VIA_T2CH] << 8);
      viaD1tbu = true1541_clk;
      true1541_set_alarm(A_VIAD1T2, viaD1tb);
    }
}

void viaD1_prevent_clk_overflow(void)
{
    viaD1tau -= PREVENT_CLK_OVERFLOW_SUB;
    viaD1tbu -= PREVENT_CLK_OVERFLOW_SUB;
}



