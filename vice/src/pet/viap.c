
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
                           | N |N-1|N-2|N-3|     | 0 | N |N-1|N-2|N-3|
                           |                           |
                           |<---- N + 1.5 CYCLES ----->|<--- N + 1 cycles --->

                            \ /                      |
                             |                       |
                         viat*u                  call of
			  cycle                   int_via*
                                                   here
 *
 * I _assume_ that there is a bug in the diagrams in the 6522-VIA.txt,
 * as it assumes that in free running mode the further cycles (after the 
 * first underflow) is N+2 cycles, although the one-shot diagram suggests
 * N+1, as I have now emulated.
 *
 * Loading the counter from the latch during a timer underflow adds a 
 * cycle. 
 *
 * It is not known which counter value then is during the load cycle
 * (would be -1 above in the one-shot diagram). I assumed -1 as well,
 * but could also be N. decrementing starts one cycle later
 * (i.e. above -2 -> N, -3 -> N-1, ...)
 *
 * IRQ and PB7 are set/toggled at the low-high transition of Phi2,
 * but int_* is called a half-cycle before that. Does that matter?
 *
 * PB7 output is still to be implemented
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



/* 
 * local functions 
 */

/*
 * Local variables
 */

static int   		viaifr;   /* Interrupt Flag register for via */
static int   		viaier;   /* Interrupt Enable register for via */

static unsigned int   	viata;    /* value of via timer A at last update */
static unsigned int   	viatb;    /* value of via timer B at last update */

static CLOCK 		viatau;   /* time when via timer A is updated */
static CLOCK 		viatbu;   /* time when via timer B is updated */
static CLOCK 		viatai;   /* time when next timer A alarm is */


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

    /* This is probably against the specs, but without a reasonable 
       value for viat[ab]u as static initialization, we can do it here... */
    viata = 0;
    viatb = 0;
    viatau = clk;
    viatbu = clk;

    viaier = 0;
    viaifr = 0;

    /* disable vice interrupts */
    viatai = 0;
    maincpu_unset_alarm(A_VIAT1);
    maincpu_unset_alarm(A_VIAT2);
    update_viairq();


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
        update_viairq();

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
        update_viairq();

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
	viata = via[VIA_T1CL] + (via[VIA_T1CH] << 8);
	viatau = clk+1;	/* from clk + 1 (next cylce) -> count */
	viatai = clk + viata + 2;
        maincpu_set_alarm_clk(A_VIAT1, viatai);

        /* Clear T1 interrupt */
        viaifr &= ~VIA_IM_T1;
        update_viairq();
        break;

      case VIA_T1LH: /* Write timer A high order latch */
        via[addr] = byte;
        /* Clear T1 interrupt */
        viaifr &= ~VIA_IM_T1;
	update_viairq();
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
        via[VIA_T2CL] = via[VIA_T2LL];
	viatbu = clk+1;	/* from clk + 1 (next cylce) -> count */
	viatb = via[VIA_T2CL] + (via[VIA_T2CH] << 8);
        maincpu_set_alarm(A_VIAT2, viatb + 2);

        /* Clear T2 interrupt */
        viaifr &= ~VIA_IM_T2;
	update_viairq();
        break;

	/* Interrupts */

      case VIA_IFR: /* 6522 Interrupt Flag Register */
        viaifr &= ~byte;
	update_viairq();
        break;

      case VIA_IER: /* Interrupt Enable Register */
#if defined (VIA_TIMER_DEBUG)
        printf ("Via#1 set VIA_IER: 0x%x\n", byte);
#endif
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
            viaier |= byte & 0x7f;
        }
        else {
            /* clear interrupts */
            viaier &= ~byte;
        }
	update_viairq();
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

    if(viatai && (viatai <= clk)) int_viat1(clk - viatai);

    switch (addr) {

      case VIA_PRA: /* port A */
        viaifr &= ~VIA_IM_CA1;
        if( (via[VIA_PCR] & 0x0a) != 0x02) {
          viaifr &= ~VIA_IM_CA2;
        }
        update_viairq();

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
        update_viairq();

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
	update_viairq();
	return (viata - ((clk - viatau)%(viata + 1))) & 0xff;

      case VIA_T1CH /*TIMER_AH*/: /* timer A high */
	return ((viata - ((clk - viatau)%(viata + 1))) >> 8) & 0xff;

      case VIA_T2CL /*TIMER_BL*/: /* timer B low */
        viaifr &= ~VIA_IM_T2;
	update_viairq();
	return (viatb - ((clk - viatbu)%(viatb + 1))) & 0xff;

      case VIA_T2CH /*TIMER_BH*/: /* timer B high */
	return ((viatb - ((clk - viatbu)%(viatb + 1))) >> 8) & 0xff;

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
    CLOCK rclk = clk - offset;
#ifdef VIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("via timer A interrupt\n");
#endif

    if (!(via[VIA_ACR] & 0x40)) { /* one-shot mode */
#if defined (VIA_TIMER_DEBUG)
        printf ("VIA Timer A interrupt -- one-shot mode: next int won't happen\n");
#endif
	maincpu_unset_alarm(A_VIAT1);		/*int_clk[I_VIAT1] = 0;*/
	viatai = 0;
    }
    else {		/* continuous mode */
        /* load counter with latch value */
	viata = via[VIA_T1LL] + (via[VIA_T1LH] << 8); 
	viatau = rclk;
	viatai = rclk + viata + 1;
        maincpu_set_alarm_clk(A_VIAT1, viatai);
    }
    viaifr |= VIA_IM_T1;
    update_viairq();

    /* TODO: toggle PB7? */
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
    maincpu_unset_alarm(A_VIAT2);	/*int_clk[I_VIAT2] = 0;*/

    viaifr |= VIA_IM_T2;
    update_viairq();

    return 0;
}

void via_prevent_clk_overflow(void)
{
     unsigned int t;
     t = (viatau - (clk + PREVENT_CLK_OVERFLOW_SUB)) & 0xffff;
     viatau = clk + t;
     t = (viatbu - (clk + PREVENT_CLK_OVERFLOW_SUB)) & 0xffff;
     viatbu = clk + t;
     if(viatai) viatai -= PREVENT_CLK_OVERFLOW_SUB;
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

