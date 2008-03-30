
/*
 * ./pet/viap.c
 * This file is generated from ./via-tmpl.c and ./pet/viap.def,
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

#define update_viairq() \
        maincpu_set_irq(I_VIAFL, (viaifr & viaier & 0x7f) ? IK_IRQ : 0)

#include "vice.h"

#include <stdio.h>
#include <time.h>

#include "vmachine.h"
#include "via.h"
#include "resources.h"


#include "parallel.h"
#include "sid.h"
#include "kbd.h"
#include "crtc.h"

#include "interrupt.h"

/*#define VIA_TIMER_DEBUG */

/* global */

BYTE    via[16];
#if 0
int     viata_stop = 0; /* maybe 1? */
int     viatb_stop = 0; /* maybe 1? */
int     viata_interrupt = 0;
int     viatb_interrupt = 0;
#endif



/* local functions */

static void update_viata ( int );
static void update_viatb ( int );

/*
 * Local variables
 */

static int   viaifr;	/* Interrupt Flag register for via */
static int   viaier;	/* Interrupt Enable register for via */

static int   viata;	/* value of via timer A at last update */
static int   viatb;	/* value of via timer B at last update */

static CLOCK viatau;	/* time when via timer A is updated */
static CLOCK viatbu;	/* time when via timer B is updated */


/* ------------------------------------------------------------------------- */
/* VIA */



/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void    reset_via(void)
{
    int i;
#ifdef VIA_TIMER_DEBUG
    if(app_resources.debugFlag) printf("VIA: reset\n");
#endif
    /* clear registers */
    for(i=0;i<4;i++) via[i]=0;
    for(i=11;i<16;i++) via[i]=0;

    viaier = 0;
    viaifr = 0;

    /* disable vice interrupts */
#ifdef OLDIRQ
    maincpu_set_irq(I_VIAT1, 0); maincpu_unset_alarm(A_VIAT1);
    maincpu_set_irq(I_VIAT2, 0); maincpu_unset_alarm(A_VIAT2);
    maincpu_set_irq(I_VIASR, 0);
    maincpu_set_irq(I_VIAFL, 0);
#else
    maincpu_unset_alarm(A_VIAT1);
    maincpu_unset_alarm(A_VIAT2);
    update_viairq();
#endif


    /* set IEC output lines */
    par_set_atn(0);
    par_set_nrfd(0);

}

void via_signal(int line, int edge) {
        switch(line) {
        case VIA_SIG_CA1:
                viaifr |= ((edge ^ via[VIA_PCR]) & 0x01) ?
                                                        0 : VIA_IM_CA1;
                update_viairq();
                break;
        case VIA_SIG_CA2:
                if( !(via[VIA_PCR] & 0x08)) {
                  viaifr |= (((edge<<2) ^ via[VIA_PCR]) & 0x04) ?
                                                        0 : VIA_IM_CA2;
                  update_viairq();
                }
                break;
        case VIA_SIG_CB1:
                viaifr |= (((edge<<4) ^ via[VIA_PCR]) & 0x10) ?
                                                        0 : VIA_IM_CB1;
                update_viairq();
                break;
        case VIA_SIG_CB2:
                if( !(via[VIA_PCR] & 0x80)) {
                  viaifr |= (((edge<<6) ^ via[VIA_PCR]) & 0x40) ?
                                                        0 : VIA_IM_CB2;
                  update_viairq();
                }
                break;
        }
}

void REGPARM2 store_via(ADDRESS addr, BYTE byte)
{
    addr &= 0xf;
#ifdef VIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("store via[%x] %x\n", (int) addr, (int) byte);
#endif

    switch (addr) {

	/* these are done with saving the value */
      case VIA_PRA: /* port A */
        viaifr &= ~VIA_IM_CA1;
        if( (via[VIA_PCR] & 0x0a) != 0x2) {
          viaifr &= ~VIA_IM_CA2;
        }
#ifndef OLDIRQ
        update_viairq();
#endif
      case VIA_PRA_NHS: /* port A, no handshake */
	via[VIA_PRA_NHS] = byte;
	addr = VIA_PRA;
      case VIA_DDRA:
	via[addr] = byte;
	break;

      case VIA_PRB: /* port B */
        viaifr &= ~VIA_IM_CB1;
        if( (via[VIA_PCR] & 0xa0) != 0x20) {
          viaifr &= ~VIA_IM_CB2;
        }
#ifndef OLDIRQ
        update_viairq();
#endif
      case VIA_DDRB:

	via[addr] = byte;
	byte = via[VIA_PRB] | ~via[VIA_DDRB];
        par_set_nrfd(!(byte & 0x02));
        par_set_atn(!(byte & 0x04));
	break;

      case VIA_SR: /* Serial Port output buffer */
	via[addr] = byte;

#ifdef SOUND
	store_petsnd_sample(byte);
#endif
	break;

	/* Timers */

      case VIA_T1CL:
      case VIA_T1LL:
	via[VIA_T1LL] = byte;
	break;

      case VIA_T1CH /*TIMER_AH*/: /* Write timer A high */
#ifdef VIA_TIMER_DEBUG
	if(app_resources.debugFlag) printf("Write timer A high: %02x\n",byte);
#endif
	via[VIA_T1LH] = byte;
        /* load counter with latch value */
        via[VIA_T1CL] = via[VIA_T1LL];
        via[VIA_T1CH] = via[VIA_T1LH];
        /* Clear T1 interrupt */
        viaifr &= ~VIA_IM_T1;
#ifdef OLDIRQ
        maincpu_set_irq(I_VIAT1, 0);
#else
        update_viairq();
#endif
        update_viata(1);
        break;

      case VIA_T1LH: /* Write timer A high order latch */
        via[addr] = byte;
        /* Clear T1 interrupt */
        viaifr &= ~VIA_IM_T1;
#ifdef OLDIRQ
        maincpu_set_irq(I_VIAT1, 0);
#else
	update_viairq();
#endif
        break;

      case VIA_T2LL:	/* Write timer 2 low latch */
	via[VIA_T2LL] = byte;

#ifdef SOUND
        store_petsnd_rate(2*byte+4);
	if(!byte) {
	  store_petsnd_onoff(0);
	} else {
	  store_petsnd_onoff(((via[VIA_ACR] & 0x1c)==0x10)?1:0);
	}
#endif
	break;

      case VIA_T2CH: /* Write timer 2 high */
        via[VIA_T2CH] = byte;
        via[VIA_T2CL] = via[VIA_T2LL]; /* bogus, both are identical */
        update_viatb(1);
        /* Clear T2 interrupt */
        viaifr &= ~VIA_IM_T2;
#ifdef OLDIRQ
        maincpu_set_irq(I_VIAT2, 0);
#else
	update_viairq();
#endif
        break;

	/* Interrupts */

      case VIA_IFR: /* 6522 Interrupt Flag Register */
        viaifr &= ~byte;
#ifdef OLDIRQ
        if(!viaifr & VIA_IM_T1) maincpu_set_irq(I_VIAT1, 0);
        if(!viaifr & VIA_IM_T2) maincpu_set_irq(I_VIAT2, 0);
#else
	update_viairq();
#endif
        break;

      case VIA_IER: /* Interrupt Enable Register */
#if defined (VIA_TIMER_DEBUG)
        printf ("Via#1 set VIA_IER: 0x%x\n", byte);
#endif
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
#ifdef OLDIRQ
            if ((byte & VIA_IM_T1) && (viaifr & VIA_IM_T1)) {
                maincpu_set_irq(I_VIAT1, IK_IRQ);
            }
            if ((byte & VIA_IM_T2) && (viaifr & VIA_IM_T2)) {
                maincpu_set_irq(I_VIAT2, IK_IRQ);
            }
#endif
            viaier |= byte & 0x7f;
        }
        else {
            /* clear interrupts */
#ifdef OLDIRQ
            if( byte & VIA_IM_T1 ) maincpu_set_irq(I_VIAT1, 0);
            if( byte & VIA_IM_T2 ) maincpu_set_irq(I_VIAT2, 0);
#endif
            viaier &= ~byte;
        }
#ifndef OLDIRQ
	update_viairq();
#endif
        break;

	/* Control */

      case VIA_ACR:
	via[addr] = byte;


#ifdef SOUND
	store_petsnd_onoff(via[VIA_T2LL] ? (((byte & 0x1c)==0x10)?1:0) : 0);
#endif

	/* bit 7 timer 1 output to PB7 */
	/* bit 6 timer 1 run mode -- default seems to be continuous */

	/* bit 5 timer 2 count mode */
	if (byte & 32) {
/* TODO */
/*	    update_viatb(0);*/	/* stop timer if mode == 1 */
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


        if(byte != via[VIA_PCR]) {
          register BYTE tmp = byte;
          /* first set bit 1 and 5 to the real output values */
          if((tmp & 0x0c) != 0x0c) tmp |= 0x02;
          if((tmp & 0xc0) != 0xc0) tmp |= 0x20;
          crtc_set_char( byte & 2 ); /* switching PET charrom */
	}
	via[addr] = byte;
	break;

      default:
	via[addr] = byte;

    }  /* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_via(ADDRESS addr)
{
    addr &= 0xf;
#ifdef VIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("read via[%d]\n", addr);
#endif
    switch (addr) {

      case VIA_PRA: /* port A */
        viaifr &= ~VIA_IM_CA1;
        if( (via[VIA_PCR] & 0x0a) != 0x02) {
          viaifr &= ~VIA_IM_CA2;
        }
#ifndef OLDIRQ
        update_viairq();
#endif
      case VIA_PRA_NHS: /* port A, no handshake */

        {
            BYTE    j = 255;
            /* VIA PA is connected to the userport pins C-L */
	    j &= (joy[1] & 1) ? ~0x80 : 0xff;
	    j &= (joy[1] & 2) ? ~0x40 : 0xff;
	    j &= (joy[1] & 4) ? ~0x20 : 0xff;
	    j &= (joy[1] & 8) ? ~0x10 : 0xff;
	    j &= (joy[1] & 16)? ~0xc0 : 0xff;
	    j &= (joy[2] & 1) ? ~0x08 : 0xff;
	    j &= (joy[2] & 2) ? ~0x04 : 0xff;
	    j &= (joy[2] & 4) ? ~0x02 : 0xff;
	    j &= (joy[2] & 8) ? ~0x01 : 0xff;
	    j &= (joy[2] & 16)? ~0x0c : 0xff;
            if (app_resources.debugFlag) {
                printf("read port A %d\n", j);
                printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                       (int) j, (int) via[VIA_PRB],
                       (int) via[VIA_DDRA], (int) via[VIA_DDRB], joy[2]);
            }
            return ((j & ~via[VIA_DDRA]) | (via[VIA_PRA] & via[VIA_DDRA]));
        }

      case VIA_PRB: /* port B */
        viaifr &= ~VIA_IM_CB1;
        if( (via[VIA_PCR] & 0xa0) != 0x20) {
          viaifr &= ~VIA_IM_CB2;
        }
#ifndef OLDIRQ
        update_viairq();
#endif

        {
            BYTE    j;
            /* read parallel IEC interface line states */
            j = 255 - (par_nrfd ? 64:0) - (par_ndac ? 1:0) - (par_dav ? 128:0);
            /* vertical retrace */
            j -= crtc_offscreen() ? 32:0;
            if (0 /*pardebug*/) {
                printf("read port B %d\n", j);
                printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                       (int) via[VIA_PRA], (int) j,
                       (int) via[VIA_DDRA], (int) via[VIA_DDRB], joy[1]);
            }
            return ((j & ~via[VIA_DDRB]) | (via[VIA_PRB] & via[VIA_DDRB]));
        }

	/* Timers */

      case VIA_T1CL /*TIMER_AL*/: /* timer A low */
        viaifr &= ~VIA_IM_T1;
#ifdef OLDIRQ
        maincpu_set_irq(I_VIAT1, 0);
#else
	update_viairq();
#endif
        return ((viata - clk + viatau) & 0xff);


      case VIA_T1CH /*TIMER_AH*/: /* timer A high */
        return (((viata - clk + viatau) >> 8) & 0xff);

      case VIA_T2CL /*TIMER_BL*/: /* timer B low */
        viaifr &= ~VIA_IM_T2;
#ifdef OLDIRQ
        maincpu_set_irq(I_VIAT2, 0);
#else
	update_viairq();
#endif
        return ((viatb - clk + viatbu) & 0xff);

      case VIA_T2CH /*TIMER_BH*/: /* timer B high */
        return (((viatb - clk + viatbu) >> 8) & 0xff);

      case VIA_SR: /* Serial Port Shift Register */
	return (via[addr]);

	/* Interrupts */

      case VIA_IFR: /* Interrupt Flag Register */
	{
	    BYTE    t = viaifr;
	    if (viaifr & viaier /*[VIA_IER]*/)
		t |= 0x80;
	    return (t);
	}

      case VIA_IER: /* 6522 Interrupt Control Register */
	    return (viaier /*[VIA_IER]*/ | 0x80);

    }  /* switch */

    return (via[addr]);
}


/* ------------------------------------------------------------------------- */

int    int_viat1(long offset)
{
#ifdef VIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("via timer A interrupt\n");
#endif

    if (!(via[VIA_ACR] & 0x40)) { /* one-shot mode */
#if defined (VIA_TIMER_DEBUG)
        printf ("VIA Timer A interrupt -- one-shot mode: next int won't happen\n");
#endif
	viata = 0;
	viatau = clk;
	maincpu_unset_alarm(A_VIAT1);		/*int_clk[I_VIAT1] = 0;*/
    }
    else {		/* continuous mode */
        /* load counter with latch value */
        via[VIA_T1CL] = via[VIA_T1LL];
        via[VIA_T1CH] = via[VIA_T1LH];
	update_viata(1);
/*	int_clk[I_VIAT1] = viatau + viata;*/
    }
    viaifr |= VIA_IM_T1;
#ifdef OLDIRQ
    if(viaier /*[VIA_IER]*/ & VIA_IM_T1 )
	maincpu_set_irq(I_VIAT1, IK_IRQ);
#else
    update_viairq();
#endif
    return 0; /*(viaier & VIA_IM_T1) ? 1:0;*/
}

/*
 * Timer B is always in one-shot mode
 */

int    int_viat2(long offset)
{
#ifdef VIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("VIA timer B interrupt\n");
#endif
    viatb = 0;
    viatbu = clk;
    maincpu_unset_alarm(A_VIAT2);	/*int_clk[I_VIAT2] = 0;*/

    viaifr |= VIA_IM_T2;
#ifdef OLDIRQ
    if( viaier & VIA_IM_T2 ) maincpu_set_irq(I_VIAT2, IK_IRQ);
#else
    update_viairq();
#endif

    return 0;
}


/* ------------------------------------------------------------------------- */

static void update_viata(int force)
{
    if(force) {
#ifdef VIA_TIMER_DEBUG
       if(app_resources.debugFlag)
          printf("update via timer A : latch=%d, counter =%d, clk = %d\n",
                via[VIA_T1CL] + (via[VIA_T1CH] << 8),
                via[VIA_T1LL] + (via[VIA_T1LH] << 8),
                clk);
#endif
      viata = via[VIA_T1CL] + (via[VIA_T1CH] << 8);
      viatau = clk;
      maincpu_set_alarm(A_VIAT1, viata + 1);
    }
}

static void update_viatb(int force)
{
    if(force) {
      viatb = via[VIA_T2CL] + (via[VIA_T2CH] << 8);
      viatbu = clk;
      maincpu_set_alarm(A_VIAT2, viatb + 1);
    }
}

void via_prevent_clk_overflow(void)
{
    viatau -= PREVENT_CLK_OVERFLOW_SUB;
    viatbu -= PREVENT_CLK_OVERFLOW_SUB;
}


int     show_keyarr(void)
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

