
/*
 * ./c64/cia1.c
 * This file is generated from ./cia-tmpl.c and ./c64/cia1.def,
 * Do not edit!
 */
/*
 * cia-tmpl.c - Template file for MOS6526 (CIA) emulation.
 *
 * Written by
 *  André Fachat (fachat@physik.tu-chemnitz.de)
 *
 * Patches and improvements by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Andreas Boose (boose@rzgw.rz.fh-hannover.de)
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
 * 03jun1997 a.fachat
 * complete timer rewrite
 *
 * There now is a new function, update_cia1(). It computes all differences
 * from one given state to a new state at rclk.  Well, not everything. For
 * the cascade mode (timer B counting timer A) as well as the shift register
 * the interrupt function is being used.
 *
 * The update routine computes, completely independent from the interrupt
 * stuff, the timer register values. The update routines are only called when
 * a timer register is touched.  int_*() is now called only _once_ per
 * interrupt, and it is much smaller than the last version. It is disabled
 * when the interrupt flag is not set (at least timer B. For timer A there
 * must not be a shift register operation and not timer B counting timer A)
 *
 * The timer B when counting timer A and the Shift register interrupt flags
 * may be set a few cycles too late due to late call of int_cia1t*() due to
 * opcode execution time. This can be fixed by checking in the beginning of
 * read_* and write_* if an int_* is scheduled and executing it before.  Then
 * the setting of the ICR could also be moved from update to int_*().  But
 * the bug only affects the contents of the ICR. The interrupt is generated
 * at the right time (hopefully).
 *
 * There is one HACK to make a game work: in update_cia1() a fix is done for
 * Arkanoid. This game counts shift register bits (i.e. TA underflows) by
 * setting TA to one-shot.  The ICR is read just before the int_cia1ta()
 * function is called, and the int bit is missed, so there is a check in
 * update_cia1() (this is probably a fix and not a hack... :-)
 *
 * Still some work has to be put into the dump functions, to really show the
 * state of the chip and not the state of the emulation. I.e. we need to
 * compute the bits described in the emulator test suite.
 *
 */

#include "vice.h"

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "vmachine.h"
#include "cia.h"
#include "vicii.h"
#include "maincpu.h"
#include "resources.h"


#include "kbd.h"

#include "interrupt.h"

#undef CIA1_TIMER_DEBUG
#undef CIA1_IO_DEBUG

#define	STORE_OFFSET 0
#define	READ_OFFSET 0

#define	CIAT_STOPPED	0
#define	CIAT_RUNNING	1
#define	CIAT_COUNTTA	2

#ifdef CIA1_TIMER_DEBUG
#define	my_set_int(int_num, value, rclk)				\
    do {								\
        if (app_resources.debugFlag)					\
	    printf("set_int(rclk=%d, int=%d, d=%d pc=%04x)\n",		\
		   rclk,(int_num),(value),*reg_pcp);			\
	maincpu_set_irq_clk((int_num), (value), (rclk));			\
	if ((value))							\
	    cia1int |= 0x80;						\
    } while(0)
#else
#define	my_set_int(int_num, value, rclk)				 \
    do {								 \
        maincpu_set_irq_clk((int_num), (value), (rclk));			 \
	if ((value))							 \
	    cia1int |= 0x80;						 \
    } while(0)
#endif

/* global */

BYTE cia1[16];
extern unsigned int *reg_pcp;

/* local functions */

static int update_cia1(CLOCK rclk);
static void check_cia1todalarm(CLOCK rclk);

void cia1_dump(FILE * fp);

/*
 * Local variables
 */

#define	cia1ier	cia1[CIA_ICR]
static int cia1int;		/* Interrupt Flag register for cia 1 */

static CLOCK cia1rdi;		/* real clock = clk-offset */

static CLOCK cia1_tau;		/* when is the next underflow? */
static unsigned int cia1_tal;	/* latch value */
static unsigned int cia1_tac;	/* counter value */
static unsigned int cia1_tat;	/* timer A toggle bit */
static unsigned int cia1_tap;	/* timer A port bit */
static int cia1_tas;		/* timer state (CIAT_*) */

static CLOCK cia1_tbu;		/* when is the next underflow? */
static unsigned int cia1_tbl;	/* latch value */
static unsigned int cia1_tbc;	/* counter value */
static unsigned int cia1_tbt;	/* timer B toggle bit */
static unsigned int cia1_tbp;	/* timer B port bit */
static int cia1_tbs;		/* timer state (CIAT_*) */

static int cia1sr_bits;	/* number of bits still to send * 2 */

static BYTE cia1todalarm[4];
static BYTE cia1todlatch[4];
static char cia1todstopped;
static char cia1todlatched;
static int cia1todticks = 100000;	/* approx. a 1/10 sec. */

static BYTE cia1flag = 0;

/* ------------------------------------------------------------------------- */
/* CIA1 */



inline static void check_cia1todalarm(CLOCK rclk)
{
    if (!memcmp(cia1todalarm, cia1 + CIA_TOD_TEN, sizeof(cia1todalarm))) {
	cia1int |= CIA_IM_TOD;
	if (cia1[CIA_ICR] & CIA_IM_TOD) {
	    my_set_int(I_CIA1TOD, IK_IRQ, rclk);
	}
    }
}

static int update_cia1(CLOCK rclk)
{
    int tmp = 0;
    unsigned int ista = 0;
    BYTE sif = (cia1int & cia1ier & 0x7f);
    /* Tick when we virtually added an interrupt flag first. */
    CLOCK added_int_clk = (cia1int & 0x80) ? rclk - 3 : CLOCK_MAX;

#ifdef CIA1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("CIA1: update: rclk=%d, tas=%d, tau=%d, tal=%u, ",
	       rclk, cia1_tas, cia1_tau, cia1_tal);
#endif

    if (cia1_tas == CIAT_RUNNING) {
	if (rclk < cia1_tau + 1) {
	    cia1_tac = cia1_tau - rclk;
	    tmp = 0;
	} else {
	    if (cia1[CIA_CRA] & 0x08) {
		tmp = 1;
		if ((cia1ier & CIA_IM_TA)
		    && (cia1_tau < added_int_clk))
		    added_int_clk = cia1_tau;
		cia1_tau = 0;
		maincpu_unset_alarm(A_CIA1TA);
		cia1_tac = cia1_tal;
		cia1_tas = CIAT_STOPPED;
		cia1[CIA_CRA] &= 0xfe;

		/* this is a HACK for arkanoid... */
		if (cia1sr_bits) {
		    cia1sr_bits--;
		    if (!cia1sr_bits) {
			cia1int |= CIA_IM_SDR;
			if ((cia1ier & CIA_IM_SDR)
			    && (cia1_tau < added_int_clk))
			    added_int_clk = cia1_tau;
		    }
		}
	    } else {
		tmp = (rclk - cia1_tau - 1) / (cia1_tal + 1);
		cia1_tau += tmp * (cia1_tal + 1);
		if ((cia1ier & CIA_IM_TA)
		    && (cia1_tau < added_int_clk))
		    added_int_clk = cia1_tau;
		cia1_tau += 1 * (cia1_tal + 1);
		cia1_tac = cia1_tau - rclk;
	    }

	    if (cia1_tac == cia1_tal)
		ista = 1;

            cia1int |= CIA_IM_TA;
	}
    }
#ifdef CIA1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("aic=%d, tac-> %u, tau-> %d\n              tmp=%u, ", added_int_clk, cia1_tac, cia1_tau, tmp);
#endif

    if (cia1[CIA_CRA] & 0x04) {
	cia1_tap = cia1_tat;
    } else {
	cia1_tap = cia1_tac ? 0 : 1;
    }

    cia1_tbp = 0;
    if (cia1_tbs == CIAT_RUNNING) {
	if (rclk < cia1_tbu + 1) {
	    cia1_tbc = cia1_tbu - rclk;
	} else {
	    if (cia1[CIA_CRB] & 0x08) {
		tmp = 1;
		if ((cia1ier & CIA_IM_TB) && (cia1_tbu < added_int_clk))
		    added_int_clk = cia1_tbu;
		cia1_tbu = 0;
		maincpu_unset_alarm(A_CIA1TB);
		cia1_tbc = cia1_tbl;
		cia1_tbs = CIAT_STOPPED;
		cia1[CIA_CRB] &= 0xfe;
	    } else {
		tmp = (rclk - cia1_tbu - 1) / (cia1_tbl + 1);
		cia1_tbu += tmp * (cia1_tbl + 1);
		if ((cia1ier & CIA_IM_TB) && (cia1_tbu < added_int_clk))
		    added_int_clk = cia1_tbu;
		cia1_tbu += 1 * (cia1_tbl + 1);
		cia1_tbc = cia1_tbu - rclk;
	    }
	    if (!cia1_tbc)
		cia1_tbc = cia1_tbl;
            cia1int |= CIA_IM_TB;
	}
    } else if (cia1_tbs == CIAT_COUNTTA) {
	/* missing: set added_int */
	if ((!cia1_tbc) && ista) {
	    cia1_tbp = 1;
	    cia1_tbc = cia1_tbl;
	    cia1int |= CIA_IM_TB;
	}
    }
    if (cia1[CIA_CRB] & 0x04) {
	cia1_tbp ^= cia1_tbt;
    } else {
	cia1_tbp = cia1_tbc ? 0 : 1;
    }

#ifdef CIA1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("tbc-> %u, tbu-> %d, int %02x ->",
	       cia1_tbc, cia1_tbu, cia1int);
#endif

    /* have we changed the interrupt flags? */
    if (sif != (cia1ier & cia1int & 0x7f)) {
	/* if we do not read ICR, do standard operation */
	if (rclk != cia1rdi) {
	    if (cia1ier & cia1int & 0x7f) {
		/* sets bit 7 */
		my_set_int(I_CIA1FL, IK_IRQ, rclk);
	    }
	} else {
	    if (added_int_clk == cia1rdi) {
		cia1int &= 0x7f;
#ifdef CIA1_TIMER_DEBUG
		if (app_resources.debugFlag)
		    printf("CIA1: TA Reading ICR at rclk=%d prevented IRQ\n",
			   rclk);
#endif
	    } else {
		if (cia1ier & cia1int & 0x7f) {
		    /* sets bit 7 */
		    my_set_int(I_CIA1FL, IK_IRQ, rclk);
		}
	    }
	}
    }
#ifdef CIA1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("%02x\n", cia1int);
#endif

    /* return true sif interrupt line is set at this clock time */
    return (!sif) && (cia1int & cia1ier & 0x7f);
}

/* ------------------------------------------------------------------------- */

void reset_cia1(void)
{
    int i;
    for (i = 0; i < 16; i++)
	cia1[i] = 0;

    cia1rdi = 0;
    cia1sr_bits = 0;

    cia1_tac = cia1_tbc = 0xffff;
    cia1_tal = cia1_tbl = 0xffff;

    cia1_tas = CIAT_STOPPED;
    cia1_tbs = CIAT_STOPPED;
    cia1_tat = 0;
    cia1_tbt = 0;

    maincpu_unset_alarm(A_CIA1TB);
    maincpu_unset_alarm(A_CIA1TA);

    memset(cia1todalarm, 0, sizeof(cia1todalarm));
    cia1todlatched = 0;
    cia1todstopped = 0;
    maincpu_set_alarm(A_CIA1TOD, cia1todticks);

    cia1int = 0;
}


void REGPARM2 store_cia1(ADDRESS addr, BYTE byte)
{
    CLOCK rclk;

    addr &= 0xf;

    vic_ii_handle_pending_alarms(maincpu_num_write_cycles());

    rclk = clk - STORE_OFFSET;

#ifdef CIA1_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("store cia1[%02x] %02x @ clk=%d, pc=%04x\n",
	       (int) addr, (int) byte, rclk, *reg_pcp);
#endif

    switch (addr) {

      case CIA_PRA:		/* port A */
      case CIA_DDRA:
	cia1[addr] = byte;
	break;

      case CIA_PRB:		/* port B */
      case CIA_DDRB:
	if ((cia1[CIA_CRA] | cia1[CIA_CRB]) & 0x02) {
	    update_cia1(rclk);
	    if (cia1[CIA_CRA] & 0x02) {
		byte &= 0xbf;
		if (cia1_tap)
		    byte |= 0x40;
	    }
	    if (cia1[CIA_CRB] & 0x02) {
		byte &= 0x7f;
		if (cia1_tbp)
		    byte |= 0x80;
	    }
	}

    {
        static int old_lp = 0x10;
        int new_lp;
      
        cia1[addr] = byte;

	/* Handle software-triggered light pen. */
        new_lp = (cia1[CIA_PRB] | ~cia1[CIA_DDRB]) & 0x10;
        if (new_lp != old_lp) {
            vic_ii_trigger_light_pen(rclk);
	    old_lp = new_lp;
        }
    }
	break;

	/* This handles the timer latches.  The kludgy stuff is an attempt
           emulate the correct behavior when the latch is written to during
           an underflow.  */
      case CIA_TAL:
	update_cia1(rclk - 1);
	if (cia1_tac == cia1_tal && cia1_tas == CIAT_RUNNING) {
	    cia1_tac = cia1_tal = (cia1_tal & 0xff00) | byte;
	    cia1_tau = rclk + cia1_tac;
	    update_cia1(rclk);
	} else {
	    cia1_tal = (cia1_tal & 0xff00) | byte;
	}
	break;
      case CIA_TBL:
	update_cia1(rclk - 1);
	if (cia1_tbc == cia1_tbl && cia1_tbs == CIAT_RUNNING) {
	    cia1_tbc = cia1_tbl = (cia1_tbl & 0xff00) | byte;
	    cia1_tbu = rclk + cia1_tbc + 1;
	    update_cia1(rclk);
	} else {
	    cia1_tbl = (cia1_tbl & 0xff00) | byte;
	}
	break;
      case CIA_TAH:
	update_cia1(rclk - 1);
	if (cia1_tac == cia1_tal && cia1_tas == CIAT_RUNNING) {
	    cia1_tac = cia1_tal = (cia1_tal & 0x00ff) | (byte << 8);
	    cia1_tau = rclk + cia1_tac;
	    update_cia1(rclk);
	} else {
	    cia1_tal = (cia1_tal & 0x00ff) | (byte << 8);
	}
	if (cia1_tas == CIAT_STOPPED)
	    cia1_tac = cia1_tal;
	break;
      case CIA_TBH:
	update_cia1(rclk - 1);
	if (cia1_tbc == cia1_tbl && cia1_tbs == CIAT_RUNNING) {
	    cia1_tbc = cia1_tbl = (cia1_tbl & 0x00ff) | (byte << 8);
	    cia1_tbu = rclk + cia1_tbc + 1;
	    update_cia1(rclk);
	} else {
	    cia1_tbl = (cia1_tbl & 0x00ff) | (byte << 8);
	}
	if (cia1_tbs == CIAT_STOPPED)
	    cia1_tbc = cia1_tbl;
	break;

	/*
	 * TOD clock is stopped by writing Hours, and restarted
	 * upon writing Tenths of Seconds.
	 *
	 * REAL:  TOD register + (wallclock - cia1todrel)
	 * VIRT:  TOD register + (cycles - begin)/cycles_per_sec
	 */
      case CIA_TOD_TEN:	/* Time Of Day clock 1/10 s */
      case CIA_TOD_HR:		/* Time Of Day clock hour */
      case CIA_TOD_SEC:	/* Time Of Day clock sec */
      case CIA_TOD_MIN:	/* Time Of Day clock min */
	/* Mask out undefined bits and flip AM/PM on hour 12
	   (Andreas Boose <boose@rzgw.rz.fh-hannover.de> 1997/10/11). */
	if (addr == CIA_TOD_HR)
	    byte = ((byte & 0x1f) == 18) ? (byte & 0x9f) ^ 0x80 : byte & 0x9f;
	if (cia1[CIA_CRB] & 0x80)
	    cia1todalarm[addr - CIA_TOD_TEN] = byte;
	else {
	    if (addr == CIA_TOD_TEN)
		cia1todstopped = 0;
	    if (addr == CIA_TOD_HR)
		cia1todstopped = 1;
	    cia1[addr] = byte;
	}
	check_cia1todalarm(rclk);
	break;

      case CIA_SDR:		/* Serial Port output buffer */
	cia1[addr] = byte;
	if (((cia1[CIA_CRA] & 0x40) == 0x40) && (cia1sr_bits < 16)) {
	    cia1sr_bits += 16;

	    /* switch timer A alarm on again, if necessary */
	    update_cia1(rclk);
	    if (cia1_tau) {
		maincpu_set_alarm_clk(A_CIA1TA, cia1_tau + 1);
	    }

#if defined (CIA1_TIMER_DEBUG)
	    if (app_resources.debugFlag)
		printf("CIA1: start SDR rclk=%d\n", rclk);
#endif
	}
	break;

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Control Register */
	update_cia1(rclk);

#if defined (CIA1_TIMER_DEBUG)
	if (app_resources.debugFlag)
	    printf("CIA1 set CIA_ICR: 0x%x\n", byte);
#endif

	if (byte & CIA_IM_SET) {
	    cia1ier |= (byte & 0x7f);
	} else {
	    cia1ier &= ~(byte & 0x7f);
	}

	/* This must actually be delayed one cycle! */
#if defined(CIA1_TIMER_DEBUG)
	if (app_resources.debugFlag)
	    printf("    set icr: ifr & ier & 0x7f -> %02x, int=%02x\n",
		   cia1ier & cia1int & 0x7f, cia1int);
#endif
	if (cia1ier & cia1int & 0x7f) {
	    my_set_int(I_CIA1FL, IK_IRQ, rclk);
	}
	if (cia1ier & (CIA_IM_TA + CIA_IM_TB)) {
	    if ((cia1ier & CIA_IM_TA) && cia1_tau) {
		maincpu_set_alarm_clk(A_CIA1TA, cia1_tau + 1);
	    }
	    if ((cia1ier & CIA_IM_TB) && cia1_tbu) {
		maincpu_set_alarm_clk(A_CIA1TB, cia1_tbu + 1);
	    }
	}
	/* Control */
	break;

      case CIA_CRA:		/* control register A */
	update_cia1(rclk);
#if defined (CIA1_TIMER_DEBUG)
	if (app_resources.debugFlag)
	    printf("CIA1 set CIA_CRA: 0x%x (clk=%d, pc=%04x, tal=%u, tac=%u)\n",
		   byte, rclk, program_counter, cia1_tal, cia1_tac);
#endif

	/* bit 7 tod frequency */
	/* bit 6 serial port mode */

	/* bit 4 force load */
	if (byte & 0x10) {
	    cia1_tac = cia1_tal;
	    if (cia1_tas == CIAT_RUNNING) {
		cia1_tau = rclk + cia1_tac + 2;
		maincpu_set_alarm_clk(A_CIA1TA, cia1_tau + 1);
	    }
	}
	/* bit 3 timer run mode */
	/* bit 2 & 1 timer output to PB6 */

	/* bit 0 start/stop timer */
	/* bit 5 timer count mode */
	if ((byte & 1) && !(cia1[CIA_CRA] & 1))
	    cia1_tat = 1;
	if ((byte ^ cia1[addr]) & 0x21) {
	    if ((byte & 0x21) == 0x01) {	/* timer just started */
		cia1_tas = CIAT_RUNNING;
		cia1_tau = rclk + (cia1_tac + 1) + ((byte & 0x10) >> 4);
		maincpu_set_alarm_clk(A_CIA1TA, cia1_tau + 1);
	    } else {		/* timer just stopped */
		cia1_tas = CIAT_STOPPED;
		cia1_tau = 0;
		/* 1 cycle delay for counter stop. */
		if (!(byte & 0x10)) {
		    /* 1 cycle delay for counter stop.  This must only happen
                       if we are not forcing load at the same time (i.e. bit
                       4 in `byte' is zero). */
		    if (cia1_tac > 0)
			cia1_tac--;
		}
		maincpu_unset_alarm(A_CIA1TA);
	    }
	}
#if defined (CIA1_TIMER_DEBUG)
	if (app_resources.debugFlag)
	    printf("    -> tas=%d, tau=%d\n", cia1_tas, cia1_tau);
#endif
	cia1[addr] = byte & 0xef;	/* remove strobe */

	break;

      case CIA_CRB:		/* control register B */
	update_cia1(rclk);

#if defined (CIA1_TIMER_DEBUG)
	if (app_resources.debugFlag)
	    printf("CIA1 set CIA_CRB: 0x%x (clk=%d, pc=%04x, tbl=%u, tbc=%u)\n",
		   byte, rclk, *reg_pcp, cia1_tbl, cia1_tbc);
#endif


	/* bit 7 set alarm/tod clock */
	/* bit 4 force load */
	if (byte & 0x10) {
	    cia1_tbc = cia1_tbl;
	    if (cia1_tbs == CIAT_RUNNING) {
		cia1_tbu = rclk + cia1_tbc + 2;
#if defined(CIA1_TIMER_DEBUG)
		if (app_resources.debugFlag)
		    printf("CIA1: rclk=%d force load: set tbu alarm to %d\n", rclk, cia1_tbu);
#endif
		maincpu_set_alarm_clk(A_CIA1TB, cia1_tbu + 1);
	    }
	}
	/* bit 3 timer run mode */
	/* bit 2 & 1 timer output to PB6 */

	/* bit 0 stbrt/stop timer */
	/* bit 5 & 6 timer count mode */
	if ((byte & 1) && !(cia1[CIA_CRB] & 1))
	    cia1_tbt = 1;
	if ((byte ^ cia1[addr]) & 0x61) {
	    if ((byte & 0x61) == 0x01) {	/* timer just started */
		cia1_tbu = rclk + (cia1_tbc + 1) + ((byte & 0x10) >> 4);
#if defined(CIA1_TIMER_DEBUG)
		if (app_resources.debugFlag)
		    printf("CIA1: rclk=%d start timer: set tbu alarm to %d\n", rclk, cia1_tbu);
#endif
		maincpu_set_alarm_clk(A_CIA1TB, cia1_tbu + 1);
		cia1_tbs = CIAT_RUNNING;
	    } else {		/* timer just stopped */
#if defined(CIA1_TIMER_DEBUG)
		if (app_resources.debugFlag)
		    printf("CIA1: rclk=%d stop timer: set tbu alarm\n", rclk);
#endif
		maincpu_unset_alarm(A_CIA1TB);
		cia1_tbu = 0;
		if (!(byte & 0x10)) {
		    /* 1 cycle delay for counter stop.  This must only happen
                       if we are not forcing load at the same time (i.e. bit
                       4 in `byte' is zero). */
		    if (cia1_tbc > 0)
			cia1_tbc--;
		}
		/* this should actually read (byte & 0x61), but as CNT is high
		   by default, bit 0x20 is a `don't care' bit */
		if ((byte & 0x41) == 0x41) {
		    cia1_tbs = CIAT_COUNTTA;
		    update_cia1(rclk);
		    /* switch timer A alarm on if necessary */
		    if (cia1_tau) {
			maincpu_set_alarm_clk(A_CIA1TA, cia1_tau + 1);
		    }
		} else {
		    cia1_tbs = CIAT_STOPPED;
		}
	    }
	}
	cia1[addr] = byte & 0xef;	/* remove strobe */
	break;

      default:
	cia1[addr] = byte;
    }				/* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_cia1(ADDRESS addr)
{

#if defined( CIA1_TIMER_DEBUG )

    BYTE read_cia1_(ADDRESS addr);
    BYTE tmp = read_cia1_(addr);

    if (app_resources.debugFlag)
	printf("read cia1[%x] returns %02x @ clk=%d, pc=%04x\n",
	       addr, tmp, clk - READ_OFFSET, *reg_pcp);
    return tmp;
}

BYTE read_cia1_(ADDRESS addr)
{

#endif

    static BYTE byte;
    CLOCK rclk;

    addr &= 0xf;

    vic_ii_handle_pending_alarms(0);

    rclk = clk - READ_OFFSET;


    switch (addr) {

      case CIA_PRA:		/* port A */

	{
	    BYTE val = cia1[CIA_PRA] | ~cia1[CIA_DDRA];
	    BYTE msk = (cia1[CIA_PRB] | ~cia1[CIA_DDRB]);
	    BYTE joy1 = joy[1];
	    int m, i;

	    msk &= ~joy1;
	    for (m = 0x1, i = 0; i < KBD_COLS; m <<= 1, i++)
		if (!(msk & m))
		    val &= ~rev_keyarr[i];
	    byte= val & ~joy[2];
	}
	return byte;
	break;

      case CIA_PRB:		/* port B */

	{
	    BYTE val = ~cia1[CIA_DDRB];
	    BYTE msk = (cia1[CIA_PRA] | ~cia1[CIA_DDRA]) & ~joy[2];
	    BYTE joy1 = joy[1];
	    BYTE m;
	    int i;
	    
	    for (m = 0x1, i = 0; i < KBD_ROWS; m <<= 1, i++)
		if (!(msk & m))
		    val &= ~keyarr[i];
	    byte= (val | (cia1[CIA_PRB] & cia1[CIA_DDRB])) & ~joy1;
	}
        if ((cia1[CIA_CRA] | cia1[CIA_CRB]) & 0x02) {
	    update_cia1(rclk);
	    if (cia1[CIA_CRA] & 0x02) {
		byte &= 0xbf;
		if (cia1_tap)
		    byte |= 0x40;
	    }
	    if (cia1[CIA_CRB] & 0x02) {
		byte &= 0x7f;
		if (cia1_tbp)
		    byte |= 0x80;
	    }
	}

	return byte;
	break;

	/* Timers */
      case CIA_TAL:		/* timer A low */
	update_cia1(rclk);
	return ((cia1_tac ? cia1_tac : cia1_tal) & 0xff);

      case CIA_TAH:		/* timer A high */
	update_cia1(rclk);
	return ((cia1_tac ? cia1_tac : cia1_tal) >> 8) & 0xff;

      case CIA_TBL:		/* timer B low */
	update_cia1(rclk);
	return cia1_tbc & 0xff;

      case CIA_TBH:		/* timer B high */
	update_cia1(rclk);
	return (cia1_tbc >> 8) & 0xff;

	/*
	 * TOD clock is latched by reading Hours, and released
	 * upon reading Tenths of Seconds. The counter itself
	 * keeps ticking all the time.
	 * Also note that this latching is different from the input one.
	 */
      case CIA_TOD_TEN:	/* Time Of Day clock 1/10 s */
      case CIA_TOD_SEC:	/* Time Of Day clock sec */
      case CIA_TOD_MIN:	/* Time Of Day clock min */
      case CIA_TOD_HR:		/* Time Of Day clock hour */
	if (!cia1todlatched)
	    memcpy(cia1todlatch, cia1 + CIA_TOD_TEN, sizeof(cia1todlatch));
	if (addr == CIA_TOD_TEN)
	    cia1todlatched = 0;
	if (addr == CIA_TOD_HR)
	    cia1todlatched = 1;
	return cia1[addr];

      case CIA_SDR:		/* Serial Port Shift Register */
#if 0				/*def DEBUG */
	cia1_dump(stdout);
	/* little hack .... */
	{
	    int i;
	    printf("\nmaincpu_ints:");
	    for (i = 0; i < NUMOFINT; i++) {
		printf(" %d", maincpu_int_status.pending_int[i]);
	    }
	    printf("\n");
	}
#endif
	return (cia1[addr]);

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Flag Register */
	{
	    BYTE t = 0;

	    

	    cia1rdi = rclk;

            if (rclk >= maincpu_int_status.alarm_clk[A_CIA2TA])
                int_cia1ta(rclk - maincpu_int_status.alarm_clk[A_CIA1TA]);
            if (rclk >= maincpu_int_status.alarm_clk[A_CIA2TB])
                int_cia1tb(rclk - maincpu_int_status.alarm_clk[A_CIA1TB]);

	    update_cia1(rclk);
	    t = cia1int | cia1flag;

#ifdef DEBUG
	    if (app_resources.debugFlag)
		printf("CIA1 read intfl gives cia1int=%02x -> %02x @"
		       " PC = %04x, sr_bits=%d, clk=%d, ta=%d, tb=%d\n",
		       cia1int, t, PC, cia1sr_bits, clk, readta(), readtb());
#endif

	    cia1flag = 0;
	    cia1int = 0;
	    my_set_int(I_CIA1FL, 0, rclk);

	    return (t);
	}
    }				/* switch */

    return (cia1[addr]);
}


/* ------------------------------------------------------------------------- */

int int_cia1ta(long offset)
{
    CLOCK rclk = clk - offset;

#if defined(CIA1_TIMER_DEBUG)
    if (app_resources.debugFlag)
	printf("CIA1: int_cia1ta(rclk = %u, tal = %u, cra=%02x\n",
	       rclk, cia1_tal, cia1[CIA_CRA]);
#endif

    cia1_tat = (cia1_tat + 1) & 1;

    if ((cia1_tas == CIAT_RUNNING) && !(cia1[CIA_CRA] & 8)) {
	/* if we do not need alarm, no PB6, no shift register, and not timer B
	   counting timer A, then we can savely skip alarms... */
	if ((cia1ier & CIA_IM_TA)
	    || (cia1[CIA_CRA] & 0x42)
	    || (cia1_tbs == CIAT_COUNTTA)) {
	    maincpu_set_alarm_clk(A_CIA1TA, rclk + cia1_tal + 1 /*+ 1*/);
	} else {
	    maincpu_unset_alarm(A_CIA1TA);
	}
    } else {
#if 0
	cia1_tas = CIAT_STOPPED;
	cia1[CIA_CRA] &= 0xfe;	/* clear run flag. Correct? */
	cia1_tau = 0;
#endif
	maincpu_unset_alarm(A_CIA1TA);
    }

    if (cia1[CIA_CRA] & 0x40) {
	if (cia1sr_bits) {
#if defined(CIA1_TIMER_DEBUG)
	    if (app_resources.debugFlag)
		printf("CIA1: rclk=%d SDR: timer A underflow, bits=%d\n",
		       rclk, cia1sr_bits);
#endif
	    if (!(--cia1sr_bits)) {
		cia1int |= CIA_IM_SDR;
	    }
	}
    }
    if (cia1_tbs == CIAT_COUNTTA) {
	if (!cia1_tbc) {
	    cia1_tbc = cia1_tbl;
	    cia1_tbu = rclk;
#if defined(CIA1_TIMER_DEBUG)
	    if (app_resources.debugFlag)
		printf("CIA1: timer B underflow when counting timer A occured, rclk=%d!\n", rclk);
#endif
	    cia1int |= CIA_IM_TB;
	    maincpu_set_alarm_clk(A_CIA1TB, rclk);
	} else {
	    cia1_tbc--;
	}
    }

    /* CIA_IM_TA is not set here, as it can be set in update(), reset
       by reading the ICR and then set again here because of delayed
       calling of int() */
    if ((IK_IRQ == IK_NMI && cia1rdi != rclk - 1)
        || (IK_IRQ == IK_IRQ && cia1rdi < rclk - 1)) {
        if ((cia1int | CIA_IM_TA) & cia1ier & 0x7f) {
            my_set_int(I_CIA1FL, IK_IRQ, rclk);
        }
    }

    return 0;
}


/*
 * Timer B can run in 2 (4) modes
 * cia1[f] & 0x60 == 0x00   count system 02 pulses
 * cia1[f] & 0x60 == 0x40   count timer A underflows
 * cia1[f] & 0x60 == 0x20 | 0x60 count CNT pulses => counter stops
 */


int int_cia1tb(long offset)
{
    CLOCK rclk = clk - offset;

#if defined(CIA1_TIMER_DEBUG)
    if (app_resources.debugFlag)
	printf("CIA1: timer B int_cia1tb(rclk=%d, tbs=%d)\n", rclk, cia1_tbs);
#endif

    cia1_tbt = (cia1_tbt + 1) & 1;

    /* running and continous, then next alarm */
    if (cia1_tbs == CIAT_RUNNING) {
	if (!(cia1[CIA_CRB] & 8)) {
#if defined(CIA1_TIMER_DEBUG)
	    if (app_resources.debugFlag)
		printf("CIA1: rclk=%d cia1tb: set tbu alarm to %d\n", rclk, rclk + cia1_tbl + 1);
#endif
	    /* if no interrupt flag we can safely skip alarms */
	    if (cia1ier & CIA_IM_TB) {
		maincpu_set_alarm_clk(A_CIA1TB, rclk + cia1_tbl + 1 /*+ 1*/);
	    } else {
		maincpu_unset_alarm(A_CIA1TB);
	    }
	} else {
#if 0
	    cia1_tbs = CIAT_STOPPED;
	    cia1[CIA_CRB] &= 0xfe; /* clear start bit */
	    cia1_tbu = 0;
#endif /* 0 */
#if defined(CIA1_TIMER_DEBUG)
	    if (app_resources.debugFlag)
		printf("CIA1: rclk=%d cia1tb: unset tbu alarm\n", rclk);
#endif
	    maincpu_unset_alarm(A_CIA1TB);
	}
    } else {
	if (cia1_tbs == CIAT_COUNTTA) {
	    if ((cia1[CIA_CRB] & 8)) {
		cia1_tbs = CIAT_STOPPED;
		cia1[CIA_CRB] &= 0xfe;		/* clear start bit */
		cia1_tbu = 0;
	    }
	}
	cia1_tbu = 0;
	maincpu_unset_alarm(A_CIA1TB);
#if defined(CIA1_TIMER_DEBUG)
	if (app_resources.debugFlag)
	    printf("CIA1: rclk=%d cia1tb: unset tbu alarm\n", rclk);
#endif
    }

    if ((IK_IRQ == IK_NMI && cia1rdi != rclk - 1)
        || (IK_IRQ == IK_IRQ && cia1rdi < rclk - 1)) {
        if ((cia1int | CIA_IM_TB) & cia1ier & 0x7f) {
            my_set_int(I_CIA1FL, IK_IRQ, rclk);
        }
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

void cia1_set_flag(void)
{
    cia1flag = CIA_IM_FLG;
}

/* ------------------------------------------------------------------------- */

int int_cia1tod(long offset)
{
    int t, pm;
    CLOCK rclk = clk - offset;

#ifdef DEBUG
    if (app_resources.debugFlag)
	printf("CIA1: TOD timer event (1/10 sec tick), tod=%02x:%02x,%02x.%x\n",
	       cia1[CIA_TOD_HR], cia1[CIA_TOD_MIN], cia1[CIA_TOD_SEC],
	       cia1[CIA_TOD_TEN]);
#endif

    /* set up new int */
    maincpu_set_alarm(A_CIA1TOD, cia1todticks);

    if (!cia1todstopped) {
	/* inc timer */
	t = bcd2byte(cia1[CIA_TOD_TEN]);
	t++;
	cia1[CIA_TOD_TEN] = byte2bcd(t % 10);
	if (t >= 10) {
	    t = bcd2byte(cia1[CIA_TOD_SEC]);
	    t++;
	    cia1[CIA_TOD_SEC] = byte2bcd(t % 60);
	    if (t >= 60) {
		t = bcd2byte(cia1[CIA_TOD_MIN]);
		t++;
		cia1[CIA_TOD_MIN] = byte2bcd(t % 60);
		if (t >= 60) {
		    pm = cia1[CIA_TOD_HR] & 0x80;
		    t = bcd2byte(cia1[CIA_TOD_HR] & 0x1f);
		    if (!t)
			pm ^= 0x80;	/* toggle am/pm on 0:59->1:00 hr */
		    t++;
		    t = t % 12 | pm;
		    cia1[CIA_TOD_HR] = byte2bcd(t);
		}
	    }
	}
#ifdef DEBUG
	if (app_resources.debugFlag)
	    printf("CIA1: TOD after event :tod=%02x:%02x,%02x.%x\n",
	       cia1[CIA_TOD_HR], cia1[CIA_TOD_MIN], cia1[CIA_TOD_SEC],
		   cia1[CIA_TOD_TEN]);
#endif
	/* check alarm */
	check_cia1todalarm(rclk);
    }
    return 0;
}

/* -------------------------------------------------------------------------- */


void cia1_prevent_clk_overflow()
{
    update_cia1(clk);

    if (cia1_tau)
	cia1_tau -= PREVENT_CLK_OVERFLOW_SUB;
    if (cia1_tbu)
	cia1_tbu -= PREVENT_CLK_OVERFLOW_SUB;
    if (cia1rdi > PREVENT_CLK_OVERFLOW_SUB)
	cia1rdi -= PREVENT_CLK_OVERFLOW_SUB;
    else
	cia1rdi = 0;
}

#if 0

void cia1_dump(FILE * fp)
{

    update_cia1(clk);
    fprintf(fp, "[CIA1]\n");
    fprintf(fp, "PA %d %d\n", cia1[CIA_PRA], cia1[CIA_DDRA]);
    fprintf(fp, "PB %d %d\n", cia1[CIA_PRB], cia1[CIA_DDRB]);
    fprintf(fp, "TA %u %u %d [$%02x = ", cia1_tac, cia1_tal,
	    cia1[CIA_CRA], cia1[CIA_CRA]);
    fprintf(fp, "%s%s%s%s%s]\n",
	    (cia1[CIA_CRA] & 1) ? "running " : "stopped ",
	    (cia1[CIA_CRA] & 8) ? "one-shot " : "continous ",
	    (cia1[CIA_CRA] & 16) ? "force-load " : "",
	    (cia1[CIA_CRA] & 32) ? "cnt " : "phi2 ",
	    (cia1[CIA_CRA] & 64) ? "sr_out " : "sr_in ");

    fprintf(fp, "TB %u %u %d [$%02x = ", cia1_tbc, cia1_tbl,
	    cia1[CIA_CRB], cia1[CIA_CRB]);
    fprintf(fp, "%s%s%s%s]\n",
	    (cia1[CIA_CRB] & 1) ? "running " : "stopped ",
	    (cia1[CIA_CRB] & 8) ? "one-shot " : "continous ",
	    (cia1[CIA_CRB] & 16) ? "force-load " : "",
	    (cia1[CIA_CRB] & 32)
	    ? ((cia1[CIA_CRB] & 64) ? "timerA+cnt" : "cnt ")
	    : ((cia1[CIA_CRB] & 64) ? "timerA" : "phi2 ")
	);

    fprintf(fp, "ICR %u %u %u", cia1int, cia1ier,
	    maincpu_int_status.pending_int[I_CIA1FL]);
    fprintf(fp, " [fl= %s%s%s%s%s] ",
	    (cia1int & 1) ? "TA " : "",
	    (cia1int & 2) ? "TB " : "",
	    (cia1int & 4) ? "Alarm " : "",
	    (cia1int & 8) ? "SDR " : "",
	    (cia1int & 16) ? "Flag " : "");
    fprintf(fp, "[mask= %s%s%s%s%s]\n",
	    (cia1ier & 1) ? "TA " : "",
	    (cia1ier & 2) ? "TB " : "",
	    (cia1ier & 4) ? "Alarm " : "",
	    (cia1ier & 8) ? "SDR " : "",
	    (cia1ier & 16) ? "Flag " : "");

    fprintf(fp, "SR %d %d\n", cia1[CIA_SDR], cia1sr_bits);
    fprintf(fp, "TOD %d %d %d %d\n", cia1[CIA_TOD_HR],
	    cia1[CIA_TOD_MIN], cia1[CIA_TOD_SEC], cia1[CIA_TOD_SEC]);
}


void cia1_undump_line(char *s)
{
    unsigned int d1, d2, d3, d4;

    if (s == strstr(s, "PA")) {
	sscanf(s + 2, "%u %u", &d1, &d2);
	cia1[CIA_PRA] = d1;
	cia1[CIA_DDRA] = d2;
	store_cia1(CIA_PRA, cia1[CIA_PRA]);
    } else if (s == strstr(s, "PB")) {
	sscanf(s + 2, "%u %u", &d1, &d2);
	cia1[CIA_PRB] = d1;
	cia1[CIA_DDRB] = d2;
	store_cia1(CIA_PRB, cia1[CIA_PRB]);
    } else if (s == strstr(s, "TA")) {
	sscanf(s + 2, "%u %u %u", &cia1_tac, &cia1_tal, &d1);
	cia1[CIA_CRA] = d1;
	if ((cia1[CIA_CRA] & 0x21) == 0x01) {
	    cia1_tau = clk + cia1_tac;
	    cia1_tas = CIAT_RUNNING;
	    maincpu_set_alarm_clk(A_CIA1TA, cia1_tau + 1);
	} else {
	    cia1_tau = 0;
	    cia1_tas = CIAT_STOPPED;
	}
    } else if (s == strstr(s, "TB")) {
	sscanf(s + 2, "%u %u %u", &cia1_tbc, &cia1_tbl, &d1);
	cia1[CIA_CRB] = d1;
	if ((cia1[CIA_CRB] & 0x61) == 0x01) {
	    cia1_tbu = clk + cia1_tbc;
	    cia1_tbs = CIAT_RUNNING;
	    maincpu_set_alarm_clk(A_CIA1TB, cia1_tbu + 1);
	} else {
	    cia1_tbu = 0;
	    if ((cia1[CIA_CRB] & 0x61) == 0x41) {
		cia1_tbs = CIAT_COUNTTA;
	    } else {
		cia1_tbs = CIAT_STOPPED;
	    }
	}
    } else if (s == strstr(s, "ICR")) {
	sscanf(s + 3, "%d %d", &d1, &d2);
	cia1int = d1;
	cia1ier = d2;
	if (cia1int & cia1ier & 0x7f) {
	    my_set_int(I_CIA1FL, IK_IRQ, rclk);
	} else {
	    my_set_int(I_CIA1FL, 0, rclk);
	}
    } else if (s == strstr(s, "SR")) {
	sscanf(s + 2, "%d %d", &d1, &cia1sr_bits);
	cia1[CIA_SDR] = d1;
    } else if (s == strstr(s, "TOD")) {
	sscanf(s + 3, "%u %u %u %u", &d1, &d2, &d3, &d4);
	cia1[CIA_TOD_HR] = d1;
	cia1[CIA_TOD_MIN] = d2;
	cia1[CIA_TOD_SEC] = d3;
	cia1[CIA_TOD_TEN] = d4;
    } else {
	printf("unknown dump format line for CIA1: \n%s\n", s);
    }
}

#endif



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

