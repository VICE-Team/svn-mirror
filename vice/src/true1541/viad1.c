
/*
 * ../../../src/true1541/viad1.c
 * This file is generated from ../../../src/via-tmpl.c and ../../../src/true1541/viad1.def,
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

#include "vice.h"

#include <stdio.h>
#include <time.h>

#include "vmachine.h"
#include "via.h"
#include "resources.h"


    #include "true1541.h"
    #include "iecdrive.h"
    #include "viad.h"

#include "interrupt.h"

/*#define VIAD1_TIMER_DEBUG */
				/*#define VIAD1_NEED_PB7 *//* when PB7 is really used, set this
				   to enable pulse output from the timer.
				   Otherwise PB7 state is computed only
				   when port B is read -
				   not yet implemented */

/* global */

BYTE viaD1[16];



/*
 * Local variables
 */

static int viaD1ifr;		/* Interrupt Flag register for viaD1 */
static int viaD1ier;		/* Interrupt Enable register for viaD1 */

static unsigned int viaD1tal;	/* current timer A latch value */
static unsigned int viaD1tbl;	/* current timer B latch value */

static CLOCK viaD1tau;		/* time when viaD1 timer A is updated */
static CLOCK viaD1tbu;		/* time when viaD1 timer B is updated */
static CLOCK viaD1tai;		/* time when next timer A alarm is */
static CLOCK viaD1tbi;		/* time when next timer A alarm is */

static int viaD1pb7;		/* state of PB7 for pulse output... */
static int viaD1pb7x;		/* to be xored herewith  */
static int viaD1pb7o;		/* to be ored herewith  */
static int viaD1pb7xx;
static int viaD1pb7sx;

/*
 * local functions
 */

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
 viaD1t*u* clk ------------------------------------------+   +--------
                                                     |
                                                     |
                                                  call of
			                        int_viaD1*
                                                   here

   real viaD1tau value = viaD1tau* + TAUOFFSET
   viaD1tbu = viaD1tbu* + 0

 *
 * IRQ and PB7 are set/toggled at the low-high transition of Phi2,
 * but int_* is called a half-cycle before that. Does that matter?
 *
 * PB7 output is still to be implemented
 */

/* timer values do not depend on a certain value here, but PB7 does... */
#define	TAUOFFSET	(-1)

inline static void update_viaD1irq(void)
{
    true1541_set_irq(I_VIAD1FL, (viaD1ifr & viaD1ier & 0x7f) ? IK_IRQ : 0);
}

/* the next two are used in read_viaD1() */

inline static unsigned int viaD1ta(void)
{
    if (true1541_clk < viaD1tau - TAUOFFSET)
        return viaD1tau - TAUOFFSET - true1541_clk - 2;
    else
	return (viaD1tal - (true1541_clk - viaD1tau + TAUOFFSET) % (viaD1tal + 2));
}

inline static unsigned int viaD1tb(void)
{
    return viaD1tbu - true1541_clk - 2;
}

inline static void update_viaD1tal(CLOCK rclk)
{
    viaD1pb7x = 0;
    viaD1pb7xx = 0;

    if (rclk > viaD1tau) {
	int nuf = (viaD1tal + 1 + rclk - viaD1tau) / (viaD1tal + 2);

	if (!(viaD1[VIA_ACR] & 0x40)) {
	    if (((nuf - viaD1pb7sx) > 1) || (!viaD1pb7)) {
		viaD1pb7o = 1;
		viaD1pb7sx = 0;
	    }
	}
	viaD1pb7 ^= (nuf & 1);

	viaD1tau = TAUOFFSET + viaD1tal + 2 + (rclk - (rclk - viaD1tau + TAUOFFSET) % (viaD1tal + 2));
	if (rclk == viaD1tau - viaD1tal - 1) {
	    viaD1pb7xx = 1;
	}
    }

    if (viaD1tau == rclk)
	viaD1pb7x = 1;

    viaD1tal = viaD1[VIA_T1LL] + (viaD1[VIA_T1LH] << 8);
}

inline static void update_viaD1tbl(void)
{
    viaD1tbl = viaD1[VIA_T2CL] + (viaD1[VIA_T2CH] << 8);
}


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

static iec_info_t *iec_info;


/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void reset_viaD1(void)
{
    int i;
#ifdef VIAD1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("VIAD1: reset\n");
#endif
    /* clear registers */
    for (i = 0; i < 4; i++)
	viaD1[i] = 0;
    for (i = 4; i < 10; i++)
        viaD1[i] = 0xff;        /* AB 98.08.23 */
    for (i = 11; i < 16; i++)
	viaD1[i] = 0;

    viaD1tal = 0;
    viaD1tbl = 0;
    viaD1tau = true1541_clk;
    viaD1tbu = true1541_clk;

    viaD1ier = 0;
    viaD1ifr = 0;

    /* disable vice interrupts */
    viaD1tai = 0;
    viaD1tbi = 0;
    true1541_unset_alarm(A_VIAD1T1);
    true1541_unset_alarm(A_VIAD1T2);
    update_viaD1irq();


    iec_info = iec_get_drive_port();

}

void viaD1_signal(int line, int edge)
{
    switch (line) {
      case VIA_SIG_CA1:
        viaD1ifr |= ((edge ^ viaD1[VIA_PCR]) & 0x01) ?
            0 : VIA_IM_CA1;
        update_viaD1irq();
        break;
      case VIA_SIG_CA2:
        if (!(viaD1[VIA_PCR] & 0x08)) {
            viaD1ifr |= (((edge << 2) ^ viaD1[VIA_PCR]) & 0x04) ?
                0 : VIA_IM_CA2;
            update_viaD1irq();
        }
        break;
      case VIA_SIG_CB1:
        viaD1ifr |= (((edge << 4) ^ viaD1[VIA_PCR]) & 0x10) ?
            0 : VIA_IM_CB1;
        update_viaD1irq();
        break;
      case VIA_SIG_CB2:
        if (!(viaD1[VIA_PCR] & 0x80)) {
            viaD1ifr |= (((edge << 6) ^ viaD1[VIA_PCR]) & 0x40) ?
                0 : VIA_IM_CB2;
            update_viaD1irq();
        }
        break;
    }
}

void REGPARM2 store_viaD1(ADDRESS addr, BYTE byte)
{
    CLOCK rclk = true1541_clk - 1;	/* stores have a one-cylce offset */

    addr &= 0xf;
#ifdef VIAD1_TIMER_DEBUG
    if ((addr < 10 && addr > 3) || (addr == VIA_ACR) || app_resources.debugFlag)
	printf("store viaD1[%x] %x, rmwf=%d, clk=%d, rclk=%d\n",
	       (int) addr, (int) byte, true1541_rmw_flag, true1541_clk, rclk);
#endif

    switch (addr) {

      /* these are done with saving the value */
      case VIA_PRA:		/* port A */
        viaD1ifr &= ~VIA_IM_CA1;
        if ((viaD1[VIA_PCR] & 0x0a) != 0x2) {
            viaD1ifr &= ~VIA_IM_CA2;
        }
        update_viaD1irq();

      case VIA_PRA_NHS:	/* port A, no handshake */
        viaD1[VIA_PRA_NHS] = byte;
        addr = VIA_PRA;
      case VIA_DDRA:

    viaD1[addr] = byte;
    if (true1541_parallel_cable_enabled)
	parallel_cable_drive_write(viaD1[VIA_PRA] | ~viaD1[VIA_DDRA],
                                   (((addr == VIA_PRA)
                                    && ((viaD1[VIA_PCR] & 0xe) == 0xa))
                                    ? 1 : 0));
            break;

      case VIA_PRB:		/* port B */
        viaD1ifr &= ~VIA_IM_CB1;
        if ((viaD1[VIA_PCR] & 0xa0) != 0x20) {
            viaD1ifr &= ~VIA_IM_CB2;
        }
        update_viaD1irq();

      case VIA_DDRB:

    if (byte != viaD1[addr]) {
	viaD1[addr] = byte;
	if (iec_info != NULL) {
	    iec_info->drive_data = viaD1[VIA_DDRB] & ~viaD1[VIA_PRB];
	    iec_info->drive_bus = (((iec_info->drive_data << 3) & 0x40)
	        | ((iec_info->drive_data << 6)
	        & ((~iec_info->drive_data ^ iec_info->cpu_bus) << 3) & 0x80));
	    iec_info->cpu_port = iec_info->cpu_bus & iec_info->drive_bus;
	    iec_info->drive_port = (((iec_info->cpu_port >> 4) & 0x4)
	        | (iec_info->cpu_port >> 7)
	        | ((iec_info->cpu_bus << 3) & 0x80));
	} else {
	    iec_drive_write(viaD1[VIA_DDRB] & ~viaD1[VIA_PRB]);
	}
    }
            break;

      case VIA_SR:		/* Serial Port output buffer */
        viaD1[addr] = byte;
        
            break;

        /* Timers */

      case VIA_T1CL:
      case VIA_T1LL:
        viaD1[VIA_T1LL] = byte;
        update_viaD1tal(rclk);
        break;

      case VIA_T1CH /*TIMER_AH */ :	/* Write timer A high */
#ifdef VIAD1_TIMER_DEBUG
        if (app_resources.debugFlag)
            printf("Write timer A high: %02x\n", byte);
#endif
        viaD1[VIA_T1LH] = byte;
        update_viaD1tal(rclk);
        /* load counter with latch value */
        viaD1tau = rclk + viaD1tal + 3 + TAUOFFSET;
        viaD1tai = rclk + viaD1tal + 2;
        true1541_set_alarm_clk(A_VIAD1T1, viaD1tai);

        /* set pb7 state */
        viaD1pb7 = 0;
        viaD1pb7o = 0;

        /* Clear T1 interrupt */
        viaD1ifr &= ~VIA_IM_T1;
        update_viaD1irq();
        break;

      case VIA_T1LH:		/* Write timer A high order latch */
        viaD1[addr] = byte;
        update_viaD1tal(rclk);

        /* Clear T1 interrupt */
        viaD1ifr &= ~VIA_IM_T1;
        update_viaD1irq();
        break;

      case VIA_T2LL:		/* Write timer 2 low latch */
        viaD1[VIA_T2LL] = byte;
        update_viaD1tbl();
        
            break;

      case VIA_T2CH:		/* Write timer 2 high */
        viaD1[VIA_T2CH] = byte;
        update_viaD1tbl();
        viaD1tbu = rclk + viaD1tbl + 3;
        viaD1tbi = rclk + viaD1tbl + 2;
        true1541_set_alarm(A_VIAD1T2, viaD1tbi);

        /* Clear T2 interrupt */
        viaD1ifr &= ~VIA_IM_T2;
        update_viaD1irq();
        break;

        /* Interrupts */

      case VIA_IFR:		/* 6522 Interrupt Flag Register */
        viaD1ifr &= ~byte;
        update_viaD1irq();
        break;

      case VIA_IER:		/* Interrupt Enable Register */
#if defined (VIAD1_TIMER_DEBUG)
        printf("Via#1 set VIA_IER: 0x%x\n", byte);
#endif
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
            viaD1ier |= byte & 0x7f;
        } else {
            /* clear interrupts */
            viaD1ier &= ~byte;
        }
        update_viaD1irq();
        break;

        /* Control */

      case VIA_ACR:
        /* bit 7 timer 1 output to PB7 */
        update_viaD1tal(rclk);
        if ((viaD1[VIA_ACR] ^ byte) & 0x80) {
            if (byte & 0x80) {
                viaD1pb7 = 1 ^ viaD1pb7x;
            }
        }
        if ((viaD1[VIA_ACR] ^ byte) & 0x40) {
            viaD1pb7 ^= viaD1pb7sx;
            if ((byte & 0x40)) {
                if (viaD1pb7x || viaD1pb7xx) {
                    if (viaD1tal) {
                        viaD1pb7o = 1;
                    } else {
                        viaD1pb7o = 0;
                        if ((viaD1[VIA_ACR] & 0x80) && viaD1pb7x && (!viaD1pb7xx))
                            viaD1pb7 ^= 1;
                    }
                }
            }
        }
        viaD1pb7sx = viaD1pb7x;
        viaD1[addr] = byte;

        

        /* bit 5 timer 2 count mode */
        if (byte & 32) {
            /* TODO */
            /* update_viaD1tb(0); *//* stop timer if mode == 1 */
        }

        /* bit 4, 3, 2 shift register control */
        /* bit 1, 0  latch enable port B and A */
        break;

      case VIA_PCR:

        /* if(viadebug) printf("VIA1: write %02x to PCR\n",byte); */

        /* bit 7, 6, 5  CB2 handshake/interrupt control */
        /* bit 4  CB1 interrupt control */

        /* bit 3, 2, 1  CA2 handshake/interrupt control */
        /* bit 0  CA1 interrupt control */

        
        viaD1[addr] = byte;
        break;

      default:
        viaD1[addr] = byte;

    }				/* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_viaD1(ADDRESS addr)
{
#ifdef VIAD1_TIMER_DEBUG
    BYTE read_viaD1_(ADDRESS);
    BYTE retv = read_viaD1_(addr);
    addr &= 0x0f;
    if ((addr > 3 && addr < 10) || app_resources.debugFlag)
	printf("read_viaD1(%x) -> %02x, clk=%d\n", addr, retv, true1541_clk);
    return retv;
}
BYTE REGPARM1 read_viaD1_(ADDRESS addr)
{
#endif
    CLOCK rclk = true1541_clk;

    addr &= 0xf;

    if (viaD1tai && (viaD1tai <= true1541_clk))
	int_viaD1t1(true1541_clk - viaD1tai);
    if (viaD1tbi && (viaD1tbi <= true1541_clk))
	int_viaD1t2(true1541_clk - viaD1tbi);

    switch (addr) {

      case VIA_PRA:		/* port A */
        viaD1ifr &= ~VIA_IM_CA1;
        if ((viaD1[VIA_PCR] & 0x0a) != 0x02) {
            viaD1ifr &= ~VIA_IM_CA2;
        }
        update_viaD1irq();

      case VIA_PRA_NHS:	/* port A, no handshake */

    return (true1541_parallel_cable_enabled
            ? parallel_cable_drive_read((((addr == VIA_PRA) &&
                                          (viaD1[VIA_PCR] & 0xe) == 0xa))
                                        ? 1 : 0)
            : ((viaD1[VIA_PRA] & viaD1[VIA_DDRA])
               | (0xff & ~viaD1[VIA_DDRA])));


      case VIA_PRB:		/* port B */
        viaD1ifr &= ~VIA_IM_CB1;

        if ((viaD1[VIA_PCR] & 0xa0) != 0x20)
            viaD1ifr &= ~VIA_IM_CB2;

        update_viaD1irq();
        {
            BYTE byte;


    if (iec_info != NULL)
	byte = ((viaD1[VIA_PRB] & 0x1a) | iec_info->drive_port) ^ 0x85;
    else
	byte = ((viaD1[VIA_PRB] & 0x1a) | iec_drive_read()) ^ 0x85;

            if (viaD1[VIA_ACR] & 0x80) {
                update_viaD1tal(rclk);
                byte = (byte & 0x7f) | (((viaD1pb7 ^ viaD1pb7x) | viaD1pb7o) ? 0x80 : 0);
            }
            return byte;
        }

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        viaD1ifr &= ~VIA_IM_T1;
        update_viaD1irq();
        return viaD1ta() & 0xff;

      case VIA_T1CH /*TIMER_AH */ :	/* timer A high */
        return (viaD1ta() >> 8) & 0xff;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        viaD1ifr &= ~VIA_IM_T2;
        update_viaD1irq();
        return viaD1tb() & 0xff;

      case VIA_T2CH /*TIMER_BH */ :	/* timer B high */
        return (viaD1tb() >> 8) & 0xff;

      case VIA_SR:		/* Serial Port Shift Register */
        return (viaD1[addr]);

        /* Interrupts */

      case VIA_IFR:		/* Interrupt Flag Register */
        {
            BYTE t = viaD1ifr;
            if (viaD1ifr & viaD1ier /*[VIA_IER] */ )
                t |= 0x80;
            return (t);
        }

      case VIA_IER:		/* 6522 Interrupt Control Register */
        return (viaD1ier /*[VIA_IER] */  | 0x80);

    }				/* switch */

    return (viaD1[addr]);
}

BYTE REGPARM1 peek_viaD1(ADDRESS addr)
{
    CLOCK rclk = true1541_clk;

    addr &= 0xf;

    if (viaD1tai && (viaD1tai <= true1541_clk))
	int_viaD1t1(true1541_clk - viaD1tai);
    if (viaD1tbi && (viaD1tbi <= true1541_clk))
	int_viaD1t2(true1541_clk - viaD1tbi);

    switch (addr) {
      case VIA_PRA:
        return read_viaD1(VIA_PRA_NHS);

      case VIA_PRB:		/* port B */
        {
            BYTE byte;


    if (iec_info != NULL)
	byte = ((viaD1[VIA_PRB] & 0x1a) | iec_info->drive_port) ^ 0x85;
    else
	byte = ((viaD1[VIA_PRB] & 0x1a) | iec_drive_read()) ^ 0x85;
            if (viaD1[VIA_ACR] & 0x80) {
                update_viaD1tal(rclk);
                byte = (byte & 0x7f) | (((viaD1pb7 ^ viaD1pb7x) | viaD1pb7o) ? 0x80 : 0);
            }
            return byte;
        }

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        return viaD1ta() & 0xff;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        return viaD1tb() & 0xff;

      default:
        break;
    }				/* switch */

    return read_viaD1(addr);
}


/* ------------------------------------------------------------------------- */

int int_viaD1t1(long offset)
{
/*    CLOCK rclk = true1541_clk - offset; */
#ifdef VIAD1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("viaD1 timer A interrupt\n");
#endif

    if (!(viaD1[VIA_ACR] & 0x40)) {	/* one-shot mode */
#if 0				/* defined (VIAD1_TIMER_DEBUG) */
	printf("VIAD1 Timer A interrupt -- one-shot mode: next int won't happen\n");
#endif
	true1541_unset_alarm(A_VIAD1T1);	/*int_clk[I_VIAD1T1] = 0; */
	viaD1tai = 0;
    } else {			/* continuous mode */
	/* load counter with latch value */
	viaD1tai += viaD1tal + 2;
	true1541_set_alarm_clk(A_VIAD1T1, viaD1tai);
    }
    viaD1ifr |= VIA_IM_T1;
    update_viaD1irq();

    /* TODO: toggle PB7? */
    return 0;			/*(viaier & VIA_IM_T1) ? 1:0; */
}

/*
 * Timer B is always in one-shot mode
 */

int int_viaD1t2(long offset)
{
#ifdef VIAD1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("VIAD1 timer B interrupt\n");
#endif
    true1541_unset_alarm(A_VIAD1T2);	/*int_clk[I_VIAD1T2] = 0; */

    viaD1ifr |= VIA_IM_T2;
    update_viaD1irq();

    return 0;
}

void viaD1_prevent_clk_overflow(CLOCK sub)
{
    unsigned int t;
    t = (viaD1tau - (true1541_clk + sub)) & 0xffff;
    viaD1tau = true1541_clk + t;
    t = (viaD1tbu - (true1541_clk + sub)) & 0xffff;
    viaD1tbu = true1541_clk + t;
    if (viaD1tai)
	viaD1tai -= sub;
}


