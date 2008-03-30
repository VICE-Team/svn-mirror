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
 * There now is a new function, update_mycia(). It computes all differences
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
 * may be set a few cycles too late due to late call of int_myciat*() due to
 * opcode execution time. This can be fixed by checking in the beginning of
 * read_* and write_* if an int_* is scheduled and executing it before.  Then
 * the setting of the ICR could also be moved from update to int_*().  But
 * the bug only affects the contents of the ICR. The interrupt is generated
 * at the right time (hopefully).
 *
 * There is one HACK to make a game work: in update_mycia() a fix is done for
 * Arkanoid. This game counts shift register bits (i.e. TA underflows) by
 * setting TA to one-shot.  The ICR is read just before the int_myciata()
 * function is called, and the int bit is missed, so there is a check in
 * update_mycia() (this is probably a fix and not a hack... :-)
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
#include "mycpu.h"
#include "resources.h"

INCLUDES

#include "interrupt.h"

#undef MYCIA_TIMER_DEBUG
#undef MYCIA_IO_DEBUG

#define	STORE_OFFSET 0
#define	READ_OFFSET 0

#define	CIAT_STOPPED	0
#define	CIAT_RUNNING	1
#define	CIAT_COUNTTA	2

#ifdef MYCIA_TIMER_DEBUG
#define	my_set_int(int_num, value, rclk)				\
    do {								\
        if (app_resources.debugFlag)					\
	    printf("set_int(rclk=%d, int=%d, d=%d pc=%04x)\n",		\
		   rclk,(int_num),(value),*reg_pcp);			\
	mycia_set_int_clk((int_num), (value), (rclk));			\
	if ((value))							\
	    myciaint |= 0x80;						\
    } while(0)
#else
#define	my_set_int(int_num, value, rclk)				 \
    do {								 \
        mycia_set_int_clk((int_num), (value), (rclk));			 \
	if ((value))							 \
	    myciaint |= 0x80;						 \
    } while(0)
#endif

/* global */

BYTE mycia[16];
extern unsigned int *reg_pcp;

/* local functions */

static int update_mycia(CLOCK rclk);
static void check_myciatodalarm(CLOCK rclk);

void mycia_dump(FILE * fp);

/*
 * Local variables
 */

#define	myciaier	mycia[CIA_ICR]
static int myciaint;		/* Interrupt Flag register for cia 1 */

static CLOCK myciardi;		/* real clock = clk-offset */

static CLOCK mycia_tau;		/* when is the next underflow? */
static unsigned int mycia_tal;	/* latch value */
static unsigned int mycia_tac;	/* counter value */
static unsigned int mycia_tat;	/* timer A toggle bit */
static unsigned int mycia_tap;	/* timer A port bit */
static int mycia_tas;		/* timer state (CIAT_*) */

static CLOCK mycia_tbu;		/* when is the next underflow? */
static unsigned int mycia_tbl;	/* latch value */
static unsigned int mycia_tbc;	/* counter value */
static unsigned int mycia_tbt;	/* timer B toggle bit */
static unsigned int mycia_tbp;	/* timer B port bit */
static int mycia_tbs;		/* timer state (CIAT_*) */

static int myciasr_bits;	/* number of bits still to send * 2 */

static BYTE myciatodalarm[4];
static BYTE myciatodlatch[4];
static char myciatodstopped;
static char myciatodlatched;
static int myciatodticks = 100000;	/* approx. a 1/10 sec. */

static BYTE myciaflag = 0;

/* ------------------------------------------------------------------------- */
/* MYCIA */

PRE_CIA_FUNCS

inline static void check_myciatodalarm(CLOCK rclk)
{
    if (!memcmp(myciatodalarm, mycia + CIA_TOD_TEN, sizeof(myciatodalarm))) {
	myciaint |= CIA_IM_TOD;
	if (mycia[CIA_ICR] & CIA_IM_TOD) {
	    my_set_int(I_MYCIATOD, MYCIA_INT, rclk);
	}
    }
}

static int update_mycia(CLOCK rclk)
{
    int tmp = 0;
    unsigned int ista = 0;
    BYTE sif = (myciaint & myciaier & 0x7f);
    /* Tick when we virtually added an interrupt flag first. */
    CLOCK added_int_clk = (myciaint & 0x80) ? rclk - 3 : CLOCK_MAX;

#ifdef MYCIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("MYCIA: update: rclk=%d, tas=%d, tau=%d, tal=%u, ",
	       rclk, mycia_tas, mycia_tau, mycia_tal);
#endif

    if (mycia_tas == CIAT_RUNNING) {
	if (rclk < mycia_tau + 1) {
	    mycia_tac = mycia_tau - rclk;
	    tmp = 0;
	} else {
	    if (mycia[CIA_CRA] & 0x08) {
		tmp = 1;
		if ((myciaier & CIA_IM_TA)
		    && (mycia_tau < added_int_clk))
		    added_int_clk = mycia_tau;
		mycia_tau = 0;
		mycpu_unset_alarm(A_MYCIATA);
		mycia_tac = mycia_tal;
		mycia_tas = CIAT_STOPPED;
		mycia[CIA_CRA] &= 0xfe;

		/* this is a HACK for arkanoid... */
		if (myciasr_bits) {
		    myciasr_bits--;
		    if (!myciasr_bits) {
			myciaint |= CIA_IM_SDR;
			if ((myciaier & CIA_IM_SDR)
			    && (mycia_tau < added_int_clk))
			    added_int_clk = mycia_tau;
		    }
		}
	    } else {
		tmp = (rclk - mycia_tau - 1) / (mycia_tal + 1);
		mycia_tau += tmp * (mycia_tal + 1);
		if ((myciaier & CIA_IM_TA)
		    && (mycia_tau < added_int_clk))
		    added_int_clk = mycia_tau;
		mycia_tau += 1 * (mycia_tal + 1);
		mycia_tac = mycia_tau - rclk;
	    }

	    if (mycia_tac == mycia_tal)
		ista = 1;

            myciaint |= CIA_IM_TA;
	}
    }
#ifdef MYCIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("aic=%d, tac-> %u, tau-> %d\n              tmp=%u, ", added_int_clk, mycia_tac, mycia_tau, tmp);
#endif

    if (mycia[CIA_CRA] & 0x04) {
	mycia_tap = mycia_tat;
    } else {
	mycia_tap = mycia_tac ? 0 : 1;
    }

    mycia_tbp = 0;
    if (mycia_tbs == CIAT_RUNNING) {
	if (rclk < mycia_tbu + 1) {
	    mycia_tbc = mycia_tbu - rclk;
	} else {
	    if (mycia[CIA_CRB] & 0x08) {
		tmp = 1;
		if ((myciaier & CIA_IM_TB) && (mycia_tbu < added_int_clk))
		    added_int_clk = mycia_tbu;
		mycia_tbu = 0;
		mycpu_unset_alarm(A_MYCIATB);
		mycia_tbc = mycia_tbl;
		mycia_tbs = CIAT_STOPPED;
		mycia[CIA_CRB] &= 0xfe;
	    } else {
		tmp = (rclk - mycia_tbu - 1) / (mycia_tbl + 1);
		mycia_tbu += tmp * (mycia_tbl + 1);
		if ((myciaier & CIA_IM_TB) && (mycia_tbu < added_int_clk))
		    added_int_clk = mycia_tbu;
		mycia_tbu += 1 * (mycia_tbl + 1);
		mycia_tbc = mycia_tbu - rclk;
	    }
	    if (!mycia_tbc)
		mycia_tbc = mycia_tbl;
            myciaint |= CIA_IM_TB;
	}
    } else if (mycia_tbs == CIAT_COUNTTA) {
	/* missing: set added_int */
	if ((!mycia_tbc) && ista) {
	    mycia_tbp = 1;
	    mycia_tbc = mycia_tbl;
	    myciaint |= CIA_IM_TB;
	}
    }
    if (mycia[CIA_CRB] & 0x04) {
	mycia_tbp ^= mycia_tbt;
    } else {
	mycia_tbp = mycia_tbc ? 0 : 1;
    }

#ifdef MYCIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("tbc-> %u, tbu-> %d, int %02x ->",
	       mycia_tbc, mycia_tbu, myciaint);
#endif

    /* have we changed the interrupt flags? */
    if (sif != (myciaier & myciaint & 0x7f)) {
	/* if we do not read ICR, do standard operation */
	if (rclk != myciardi) {
	    if (myciaier & myciaint & 0x7f) {
		/* sets bit 7 */
		my_set_int(I_MYCIAFL, MYCIA_INT, rclk);
	    }
	} else {
	    if (added_int_clk == myciardi) {
		myciaint &= 0x7f;
#ifdef MYCIA_TIMER_DEBUG
		if (app_resources.debugFlag)
		    printf("MYCIA: TA Reading ICR at rclk=%d prevented IRQ\n",
			   rclk);
#endif
	    } else {
		if (myciaier & myciaint & 0x7f) {
		    /* sets bit 7 */
		    my_set_int(I_MYCIAFL, MYCIA_INT, rclk);
		}
	    }
	}
    }
#ifdef MYCIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("%02x\n", myciaint);
#endif

    /* return true sif interrupt line is set at this clock time */
    return (!sif) && (myciaint & myciaier & 0x7f);
}

/* ------------------------------------------------------------------------- */

void reset_mycia(void)
{
    int i;

    for (i = 0; i < 16; i++)
	mycia[i] = 0;

    myciardi = 0;
    myciasr_bits = 0;

    mycia_tac = mycia_tbc = 0xffff;
    mycia_tal = mycia_tbl = 0xffff;

    mycia_tas = CIAT_STOPPED;
    mycia_tbs = CIAT_STOPPED;
    mycia_tat = 0;
    mycia_tbt = 0;

    mycpu_unset_alarm(A_MYCIATB);
    mycpu_unset_alarm(A_MYCIATA);

    memset(myciatodalarm, 0, sizeof(myciatodalarm));
    myciatodlatched = 0;
    myciatodstopped = 0;
    mycpu_set_alarm(A_MYCIATOD, myciatodticks);

    myciaint = 0;
}


void REGPARM2 store_mycia(ADDRESS addr, BYTE byte)
{
    CLOCK rclk;

    addr &= 0xf;

    vic_ii_handle_pending_alarms(mycpu_num_write_cycles());

    rclk = clk - STORE_OFFSET;

#ifdef MYCIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	printf("store mycia[%02x] %02x @ clk=%d, pc=%04x\n",
	       (int) addr, (int) byte, rclk, *reg_pcp);
#endif

    switch (addr) {

      case CIA_PRA:		/* port A */
      case CIA_DDRA:
	STORE_CIAPA
	break;

      case CIA_PRB:		/* port B */
      case CIA_DDRB:
	if ((mycia[CIA_CRA] | mycia[CIA_CRB]) & 0x02) {
	    update_mycia(rclk);
	    if (mycia[CIA_CRA] & 0x02) {
		byte &= 0xbf;
		if (mycia_tap)
		    byte |= 0x40;
	    }
	    if (mycia[CIA_CRB] & 0x02) {
		byte &= 0x7f;
		if (mycia_tbp)
		    byte |= 0x80;
	    }
	}
	STORE_CIAPB
	break;

	/* This handles the timer latches.  The kludgy stuff is an attempt
           emulate the correct behavior when the latch is written to during
           an underflow.  */
      case CIA_TAL:
	update_mycia(rclk - 1);
	if (mycia_tac == mycia_tal && mycia_tas == CIAT_RUNNING) {
	    mycia_tac = mycia_tal = (mycia_tal & 0xff00) | byte;
	    mycia_tau = rclk + mycia_tac;
	    update_mycia(rclk);
	} else {
	    mycia_tal = (mycia_tal & 0xff00) | byte;
	}
	break;
      case CIA_TBL:
	update_mycia(rclk - 1);
	if (mycia_tbc == mycia_tbl && mycia_tbs == CIAT_RUNNING) {
	    mycia_tbc = mycia_tbl = (mycia_tbl & 0xff00) | byte;
	    mycia_tbu = rclk + mycia_tbc + 1;
	    update_mycia(rclk);
	} else {
	    mycia_tbl = (mycia_tbl & 0xff00) | byte;
	}
	break;
      case CIA_TAH:
	update_mycia(rclk - 1);
	if (mycia_tac == mycia_tal && mycia_tas == CIAT_RUNNING) {
	    mycia_tac = mycia_tal = (mycia_tal & 0x00ff) | (byte << 8);
	    mycia_tau = rclk + mycia_tac;
	    update_mycia(rclk);
	} else {
	    mycia_tal = (mycia_tal & 0x00ff) | (byte << 8);
	}
	if (mycia_tas == CIAT_STOPPED)
	    mycia_tac = mycia_tal;
	break;
      case CIA_TBH:
	update_mycia(rclk - 1);
	if (mycia_tbc == mycia_tbl && mycia_tbs == CIAT_RUNNING) {
	    mycia_tbc = mycia_tbl = (mycia_tbl & 0x00ff) | (byte << 8);
	    mycia_tbu = rclk + mycia_tbc + 1;
	    update_mycia(rclk);
	} else {
	    mycia_tbl = (mycia_tbl & 0x00ff) | (byte << 8);
	}
	if (mycia_tbs == CIAT_STOPPED)
	    mycia_tbc = mycia_tbl;
	break;

	/*
	 * TOD clock is stopped by writing Hours, and restarted
	 * upon writing Tenths of Seconds.
	 *
	 * REAL:  TOD register + (wallclock - myciatodrel)
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
	if (mycia[CIA_CRB] & 0x80)
	    myciatodalarm[addr - CIA_TOD_TEN] = byte;
	else {
	    if (addr == CIA_TOD_TEN)
		myciatodstopped = 0;
	    if (addr == CIA_TOD_HR)
		myciatodstopped = 1;
	    mycia[addr] = byte;
	}
	check_myciatodalarm(rclk);
	break;

      case CIA_SDR:		/* Serial Port output buffer */
	mycia[addr] = byte;
	if (((mycia[CIA_CRA] & 0x40) == 0x40) && (myciasr_bits < 16)) {
	    myciasr_bits += 16;

	    /* switch timer A alarm on again, if necessary */
	    update_mycia(rclk);
	    if (mycia_tau) {
		mycpu_set_alarm_clk(A_MYCIATA, mycia_tau + 1);
	    }

#if defined (MYCIA_TIMER_DEBUG)
	    if (app_resources.debugFlag)
		printf("MYCIA: start SDR rclk=%d\n", rclk);
#endif
	}
	break;

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Control Register */
	update_mycia(rclk);

#if defined (MYCIA_TIMER_DEBUG)
	if (app_resources.debugFlag)
	    printf("MYCIA set CIA_ICR: 0x%x\n", byte);
#endif

	if (byte & CIA_IM_SET) {
	    myciaier |= (byte & 0x7f);
	} else {
	    myciaier &= ~(byte & 0x7f);
	}

	/* This must actually be delayed one cycle! */
#if defined(MYCIA_TIMER_DEBUG)
	if (app_resources.debugFlag)
	    printf("    set icr: ifr & ier & 0x7f -> %02x, int=%02x\n",
		   myciaier & myciaint & 0x7f, myciaint);
#endif
	if (myciaier & myciaint & 0x7f) {
	    my_set_int(I_MYCIAFL, MYCIA_INT, rclk);
	}
	if (myciaier & (CIA_IM_TA + CIA_IM_TB)) {
	    if ((myciaier & CIA_IM_TA) && mycia_tau) {
		mycpu_set_alarm_clk(A_MYCIATA, mycia_tau + 1);
	    }
	    if ((myciaier & CIA_IM_TB) && mycia_tbu) {
		mycpu_set_alarm_clk(A_MYCIATB, mycia_tbu + 1);
	    }
	}
	/* Control */
	break;

      case CIA_CRA:		/* control register A */
	update_mycia(rclk);
#if defined (MYCIA_TIMER_DEBUG)
	if (app_resources.debugFlag)
	    printf("MYCIA set CIA_CRA: 0x%x (clk=%d, pc=%04x, tal=%u, tac=%u)\n",
		   byte, rclk, program_counter, mycia_tal, mycia_tac);
#endif

	/* bit 7 tod frequency */
	/* bit 6 serial port mode */

	/* bit 4 force load */
	if (byte & 0x10) {
	    mycia_tac = mycia_tal;
	    if (mycia_tas == CIAT_RUNNING) {
		mycia_tau = rclk + mycia_tac + 2;
		mycpu_set_alarm_clk(A_MYCIATA, mycia_tau + 1);
	    }
	}
	/* bit 3 timer run mode */
	/* bit 2 & 1 timer output to PB6 */

	/* bit 0 start/stop timer */
	/* bit 5 timer count mode */
	if ((byte & 1) && !(mycia[CIA_CRA] & 1))
	    mycia_tat = 1;
	if ((byte ^ mycia[addr]) & 0x21) {
	    if ((byte & 0x21) == 0x01) {	/* timer just started */
		mycia_tas = CIAT_RUNNING;
		mycia_tau = rclk + (mycia_tac + 1) + ((byte & 0x10) >> 4);
		mycpu_set_alarm_clk(A_MYCIATA, mycia_tau + 1);
	    } else {		/* timer just stopped */
		mycia_tas = CIAT_STOPPED;
		mycia_tau = 0;
		/* 1 cycle delay for counter stop. */
		if (!(byte & 0x10)) {
		    /* 1 cycle delay for counter stop.  This must only happen
                       if we are not forcing load at the same time (i.e. bit
                       4 in `byte' is zero). */
		    if (mycia_tac > 0)
			mycia_tac--;
		}
		mycpu_unset_alarm(A_MYCIATA);
	    }
	}
#if defined (MYCIA_TIMER_DEBUG)
	if (app_resources.debugFlag)
	    printf("    -> tas=%d, tau=%d\n", mycia_tas, mycia_tau);
#endif
	mycia[addr] = byte & 0xef;	/* remove strobe */

	break;

      case CIA_CRB:		/* control register B */
	update_mycia(rclk);

#if defined (MYCIA_TIMER_DEBUG)
	if (app_resources.debugFlag)
	    printf("MYCIA set CIA_CRB: 0x%x (clk=%d, pc=%04x, tbl=%u, tbc=%u)\n",
		   byte, rclk, *reg_pcp, mycia_tbl, mycia_tbc);
#endif


	/* bit 7 set alarm/tod clock */
	/* bit 4 force load */
	if (byte & 0x10) {
	    mycia_tbc = mycia_tbl;
	    if (mycia_tbs == CIAT_RUNNING) {
		mycia_tbu = rclk + mycia_tbc + 2;
#if defined(MYCIA_TIMER_DEBUG)
		if (app_resources.debugFlag)
		    printf("MYCIA: rclk=%d force load: set tbu alarm to %d\n", rclk, mycia_tbu);
#endif
		mycpu_set_alarm_clk(A_MYCIATB, mycia_tbu + 1);
	    }
	}
	/* bit 3 timer run mode */
	/* bit 2 & 1 timer output to PB6 */

	/* bit 0 stbrt/stop timer */
	/* bit 5 & 6 timer count mode */
	if ((byte & 1) && !(mycia[CIA_CRB] & 1))
	    mycia_tbt = 1;
	if ((byte ^ mycia[addr]) & 0x61) {
	    if ((byte & 0x61) == 0x01) {	/* timer just started */
		mycia_tbu = rclk + (mycia_tbc + 1) + ((byte & 0x10) >> 4);
#if defined(MYCIA_TIMER_DEBUG)
		if (app_resources.debugFlag)
		    printf("MYCIA: rclk=%d start timer: set tbu alarm to %d\n", rclk, mycia_tbu);
#endif
		mycpu_set_alarm_clk(A_MYCIATB, mycia_tbu + 1);
		mycia_tbs = CIAT_RUNNING;
	    } else {		/* timer just stopped */
#if defined(MYCIA_TIMER_DEBUG)
		if (app_resources.debugFlag)
		    printf("MYCIA: rclk=%d stop timer: set tbu alarm\n", rclk);
#endif
		mycpu_unset_alarm(A_MYCIATB);
		mycia_tbu = 0;
		if (!(byte & 0x10)) {
		    /* 1 cycle delay for counter stop.  This must only happen
                       if we are not forcing load at the same time (i.e. bit
                       4 in `byte' is zero). */
		    if (mycia_tbc > 0)
			mycia_tbc--;
		}
		/* this should actually read (byte & 0x61), but as CNT is high
		   by default, bit 0x20 is a `don't care' bit */
		if ((byte & 0x41) == 0x41) {
		    mycia_tbs = CIAT_COUNTTA;
		    update_mycia(rclk);
		    /* switch timer A alarm on if necessary */
		    if (mycia_tau) {
			mycpu_set_alarm_clk(A_MYCIATA, mycia_tau + 1);
		    }
		} else {
		    mycia_tbs = CIAT_STOPPED;
		}
	    }
	}
	mycia[addr] = byte & 0xef;	/* remove strobe */
	break;

      default:
	mycia[addr] = byte;
    }				/* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_mycia(ADDRESS addr)
{

#if defined( MYCIA_TIMER_DEBUG )

    BYTE read_mycia_(ADDRESS addr);
    BYTE tmp = read_mycia_(addr);

    if (app_resources.debugFlag)
	printf("read mycia[%x] returns %02x @ clk=%d, pc=%04x\n",
	       addr, tmp, clk - READ_OFFSET, *reg_pcp);
    return tmp;
}

BYTE read_mycia_(ADDRESS addr)
{

#endif

    static BYTE byte;
    CLOCK rclk;

    addr &= 0xf;

    vic_ii_handle_pending_alarms(0);

    rclk = clk - READ_OFFSET;


    switch (addr) {

      case CIA_PRA:		/* port A */
	READ_CIAPA
	return byte;
	break;

      case CIA_PRB:		/* port B */
	READ_CIAPB
        if ((mycia[CIA_CRA] | mycia[CIA_CRB]) & 0x02) {
	    update_mycia(rclk);
	    if (mycia[CIA_CRA] & 0x02) {
		byte &= 0xbf;
		if (mycia_tap)
		    byte |= 0x40;
	    }
	    if (mycia[CIA_CRB] & 0x02) {
		byte &= 0x7f;
		if (mycia_tbp)
		    byte |= 0x80;
	    }
	}

	return byte;
	break;

	/* Timers */
      case CIA_TAL:		/* timer A low */
	update_mycia(rclk);
	return ((mycia_tac ? mycia_tac : mycia_tal) & 0xff);

      case CIA_TAH:		/* timer A high */
	update_mycia(rclk);
	return ((mycia_tac ? mycia_tac : mycia_tal) >> 8) & 0xff;

      case CIA_TBL:		/* timer B low */
	update_mycia(rclk);
	return mycia_tbc & 0xff;

      case CIA_TBH:		/* timer B high */
	update_mycia(rclk);
	return (mycia_tbc >> 8) & 0xff;

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
	if (!myciatodlatched)
	    memcpy(myciatodlatch, mycia + CIA_TOD_TEN, sizeof(myciatodlatch));
	if (addr == CIA_TOD_TEN)
	    myciatodlatched = 0;
	if (addr == CIA_TOD_HR)
	    myciatodlatched = 1;
	return mycia[addr];

      case CIA_SDR:		/* Serial Port Shift Register */
#if 0				/*def DEBUG */
	mycia_dump(stdout);
	/* little hack .... */
	{
	    int i;
	    printf("\nmycpu_ints:");
	    for (i = 0; i < NUMOFINT; i++) {
		printf(" %d", mycpu_int_status.pending_int[i]);
	    }
	    printf("\n");
	}
#endif
	return (mycia[addr]);

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Flag Register */
	{
	    BYTE t = 0;

	    READ_CIAICR

	    myciardi = rclk;

            if (rclk >= mycpu_int_status.alarm_clk[A_MYCIATA])
                int_myciata(rclk - mycpu_int_status.alarm_clk[A_MYCIATA]);
            if (rclk >= mycpu_int_status.alarm_clk[A_MYCIATB])
                int_myciatb(rclk - mycpu_int_status.alarm_clk[A_MYCIATB]);

	    update_mycia(rclk);
	    t = myciaint | myciaflag;

#ifdef DEBUG
	    if (app_resources.debugFlag)
		printf("MYCIA read intfl gives myciaint=%02x -> %02x @"
		       " PC = %04x, sr_bits=%d, clk=%d, ta=%d, tb=%d\n",
		       myciaint, t, PC, myciasr_bits, clk, readta(), readtb());
#endif

	    myciaflag = 0;
	    myciaint = 0;
	    my_set_int(I_MYCIAFL, 0, rclk);

	    return (t);
	}
    }				/* switch */

    return (mycia[addr]);
}


/* ------------------------------------------------------------------------- */

int int_myciata(long offset)
{
    CLOCK rclk = clk - offset;

#if defined(MYCIA_TIMER_DEBUG)
    if (app_resources.debugFlag)
	printf("MYCIA: int_myciata(rclk = %u, tal = %u, cra=%02x\n",
	       rclk, mycia_tal, mycia[CIA_CRA]);
#endif

    mycia_tat = (mycia_tat + 1) & 1;

    if ((mycia_tas == CIAT_RUNNING) && !(mycia[CIA_CRA] & 8)) {
	/* if we do not need alarm, no PB6, no shift register, and not timer B
	   counting timer A, then we can savely skip alarms... */
	if ( ((myciaier & CIA_IM_TA) && 
		(!mycpu_int_status.pending_int[I_MYCIATA]))
	    || (mycia[CIA_CRA] & 0x42)
	    || (mycia_tbs == CIAT_COUNTTA)) {
	    mycpu_set_alarm_clk(A_MYCIATA, rclk + mycia_tal + 1 /*+ 1*/);
	} else {
	    mycpu_unset_alarm(A_MYCIATA);
	}
    } else {
#if 0
	mycia_tas = CIAT_STOPPED;
	mycia[CIA_CRA] &= 0xfe;	/* clear run flag. Correct? */
	mycia_tau = 0;
#endif
	mycpu_unset_alarm(A_MYCIATA);
    }

    if (mycia[CIA_CRA] & 0x40) {
	if (myciasr_bits) {
#if defined(MYCIA_TIMER_DEBUG)
	    if (app_resources.debugFlag)
		printf("MYCIA: rclk=%d SDR: timer A underflow, bits=%d\n",
		       rclk, myciasr_bits);
#endif
	    if (!(--myciasr_bits)) {
		myciaint |= CIA_IM_SDR;
	    }
	}
    }
    if (mycia_tbs == CIAT_COUNTTA) {
	if (!mycia_tbc) {
	    mycia_tbc = mycia_tbl;
	    mycia_tbu = rclk;
#if defined(MYCIA_TIMER_DEBUG)
	    if (app_resources.debugFlag)
		printf("MYCIA: timer B underflow when counting timer A occured, rclk=%d!\n", rclk);
#endif
	    myciaint |= CIA_IM_TB;
	    mycpu_set_alarm_clk(A_MYCIATB, rclk);
	} else {
	    mycia_tbc--;
	}
    }

    /* CIA_IM_TA is not set here, as it can be set in update(), reset
       by reading the ICR and then set again here because of delayed
       calling of int() */
    if ((MYCIA_INT == IK_NMI && myciardi != rclk - 1)
        || (MYCIA_INT == IK_IRQ && myciardi < rclk - 1)) {
        if ((myciaint | CIA_IM_TA) & myciaier & 0x7f) {
            my_set_int(I_MYCIAFL, MYCIA_INT, rclk);
        }
    }

    return 0;
}


/*
 * Timer B can run in 2 (4) modes
 * mycia[f] & 0x60 == 0x00   count system 02 pulses
 * mycia[f] & 0x60 == 0x40   count timer A underflows
 * mycia[f] & 0x60 == 0x20 | 0x60 count CNT pulses => counter stops
 */


int int_myciatb(long offset)
{
    CLOCK rclk = clk - offset;

#if defined(MYCIA_TIMER_DEBUG)
    if (app_resources.debugFlag)
	printf("MYCIA: timer B int_myciatb(rclk=%d, tbs=%d)\n", rclk, mycia_tbs);
#endif

    mycia_tbt = (mycia_tbt + 1) & 1;

    /* running and continous, then next alarm */
    if (mycia_tbs == CIAT_RUNNING) {
	if (!(mycia[CIA_CRB] & 8)) {
#if defined(MYCIA_TIMER_DEBUG)
	    if (app_resources.debugFlag)
		printf("MYCIA: rclk=%d myciatb: set tbu alarm to %d\n", rclk, rclk + mycia_tbl + 1);
#endif
	    /* if no interrupt flag we can safely skip alarms */
	    if (myciaier & CIA_IM_TB) {
		mycpu_set_alarm_clk(A_MYCIATB, rclk + mycia_tbl + 1 /*+ 1*/);
	    } else {
		mycpu_unset_alarm(A_MYCIATB);
	    }
	} else {
#if 0
	    mycia_tbs = CIAT_STOPPED;
	    mycia[CIA_CRB] &= 0xfe; /* clear start bit */
	    mycia_tbu = 0;
#endif /* 0 */
#if defined(MYCIA_TIMER_DEBUG)
	    if (app_resources.debugFlag)
		printf("MYCIA: rclk=%d myciatb: unset tbu alarm\n", rclk);
#endif
	    mycpu_unset_alarm(A_MYCIATB);
	}
    } else {
	if (mycia_tbs == CIAT_COUNTTA) {
	    if ((mycia[CIA_CRB] & 8)) {
		mycia_tbs = CIAT_STOPPED;
		mycia[CIA_CRB] &= 0xfe;		/* clear start bit */
		mycia_tbu = 0;
	    }
	}
	mycia_tbu = 0;
	mycpu_unset_alarm(A_MYCIATB);
#if defined(MYCIA_TIMER_DEBUG)
	if (app_resources.debugFlag)
	    printf("MYCIA: rclk=%d myciatb: unset tbu alarm\n", rclk);
#endif
    }

    if ((MYCIA_INT == IK_NMI && myciardi != rclk - 1)
        || (MYCIA_INT == IK_IRQ && myciardi < rclk - 1)) {
        if ((myciaint | CIA_IM_TB) & myciaier & 0x7f) {
            my_set_int(I_MYCIAFL, MYCIA_INT, rclk);
        }
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

void mycia_set_flag(void)
{
    myciaflag = CIA_IM_FLG;
}

/* ------------------------------------------------------------------------- */

int int_myciatod(long offset)
{
    int t, pm;
    CLOCK rclk = clk - offset;

#ifdef DEBUG
    if (app_resources.debugFlag)
	printf("MYCIA: TOD timer event (1/10 sec tick), tod=%02x:%02x,%02x.%x\n",
	       mycia[CIA_TOD_HR], mycia[CIA_TOD_MIN], mycia[CIA_TOD_SEC],
	       mycia[CIA_TOD_TEN]);
#endif

    /* set up new int */
    mycpu_set_alarm(A_MYCIATOD, myciatodticks);

    if (!myciatodstopped) {
	/* inc timer */
	t = bcd2byte(mycia[CIA_TOD_TEN]);
	t++;
	mycia[CIA_TOD_TEN] = byte2bcd(t % 10);
	if (t >= 10) {
	    t = bcd2byte(mycia[CIA_TOD_SEC]);
	    t++;
	    mycia[CIA_TOD_SEC] = byte2bcd(t % 60);
	    if (t >= 60) {
		t = bcd2byte(mycia[CIA_TOD_MIN]);
		t++;
		mycia[CIA_TOD_MIN] = byte2bcd(t % 60);
		if (t >= 60) {
		    pm = mycia[CIA_TOD_HR] & 0x80;
		    t = bcd2byte(mycia[CIA_TOD_HR] & 0x1f);
		    if (!t)
			pm ^= 0x80;	/* toggle am/pm on 0:59->1:00 hr */
		    t++;
		    t = t % 12 | pm;
		    mycia[CIA_TOD_HR] = byte2bcd(t);
		}
	    }
	}
#ifdef DEBUG
	if (app_resources.debugFlag)
	    printf("MYCIA: TOD after event :tod=%02x:%02x,%02x.%x\n",
	       mycia[CIA_TOD_HR], mycia[CIA_TOD_MIN], mycia[CIA_TOD_SEC],
		   mycia[CIA_TOD_TEN]);
#endif
	/* check alarm */
	check_myciatodalarm(rclk);
    }
    return 0;
}

/* -------------------------------------------------------------------------- */


void mycia_prevent_clk_overflow(CLOCK sub)
{
    update_mycia(clk);

    if (mycia_tau)
	mycia_tau -= sub;
    if (mycia_tbu)
	mycia_tbu -= sub;
    if (myciardi > sub)
	myciardi -= sub;
    else
	myciardi = 0;
}

#if 0

void mycia_dump(FILE * fp)
{

    update_mycia(clk);
    fprintf(fp, "[MYCIA]\n");
    fprintf(fp, "PA %d %d\n", mycia[CIA_PRA], mycia[CIA_DDRA]);
    fprintf(fp, "PB %d %d\n", mycia[CIA_PRB], mycia[CIA_DDRB]);
    fprintf(fp, "TA %u %u %d [$%02x = ", mycia_tac, mycia_tal,
	    mycia[CIA_CRA], mycia[CIA_CRA]);
    fprintf(fp, "%s%s%s%s%s]\n",
	    (mycia[CIA_CRA] & 1) ? "running " : "stopped ",
	    (mycia[CIA_CRA] & 8) ? "one-shot " : "continous ",
	    (mycia[CIA_CRA] & 16) ? "force-load " : "",
	    (mycia[CIA_CRA] & 32) ? "cnt " : "phi2 ",
	    (mycia[CIA_CRA] & 64) ? "sr_out " : "sr_in ");

    fprintf(fp, "TB %u %u %d [$%02x = ", mycia_tbc, mycia_tbl,
	    mycia[CIA_CRB], mycia[CIA_CRB]);
    fprintf(fp, "%s%s%s%s]\n",
	    (mycia[CIA_CRB] & 1) ? "running " : "stopped ",
	    (mycia[CIA_CRB] & 8) ? "one-shot " : "continous ",
	    (mycia[CIA_CRB] & 16) ? "force-load " : "",
	    (mycia[CIA_CRB] & 32)
	    ? ((mycia[CIA_CRB] & 64) ? "timerA+cnt" : "cnt ")
	    : ((mycia[CIA_CRB] & 64) ? "timerA" : "phi2 ")
	);

    fprintf(fp, "ICR %u %u %u", myciaint, myciaier,
	    mycpu_int_status.pending_int[I_MYCIAFL]);
    fprintf(fp, " [fl= %s%s%s%s%s] ",
	    (myciaint & 1) ? "TA " : "",
	    (myciaint & 2) ? "TB " : "",
	    (myciaint & 4) ? "Alarm " : "",
	    (myciaint & 8) ? "SDR " : "",
	    (myciaint & 16) ? "Flag " : "");
    fprintf(fp, "[mask= %s%s%s%s%s]\n",
	    (myciaier & 1) ? "TA " : "",
	    (myciaier & 2) ? "TB " : "",
	    (myciaier & 4) ? "Alarm " : "",
	    (myciaier & 8) ? "SDR " : "",
	    (myciaier & 16) ? "Flag " : "");

    fprintf(fp, "SR %d %d\n", mycia[CIA_SDR], myciasr_bits);
    fprintf(fp, "TOD %d %d %d %d\n", mycia[CIA_TOD_HR],
	    mycia[CIA_TOD_MIN], mycia[CIA_TOD_SEC], mycia[CIA_TOD_SEC]);
}


void mycia_undump_line(char *s)
{
    unsigned int d1, d2, d3, d4;

    if (s == strstr(s, "PA")) {
	sscanf(s + 2, "%u %u", &d1, &d2);
	mycia[CIA_PRA] = d1;
	mycia[CIA_DDRA] = d2;
	store_mycia(CIA_PRA, mycia[CIA_PRA]);
    } else if (s == strstr(s, "PB")) {
	sscanf(s + 2, "%u %u", &d1, &d2);
	mycia[CIA_PRB] = d1;
	mycia[CIA_DDRB] = d2;
	store_mycia(CIA_PRB, mycia[CIA_PRB]);
    } else if (s == strstr(s, "TA")) {
	sscanf(s + 2, "%u %u %u", &mycia_tac, &mycia_tal, &d1);
	mycia[CIA_CRA] = d1;
	if ((mycia[CIA_CRA] & 0x21) == 0x01) {
	    mycia_tau = clk + mycia_tac;
	    mycia_tas = CIAT_RUNNING;
	    mycpu_set_alarm_clk(A_MYCIATA, mycia_tau + 1);
	} else {
	    mycia_tau = 0;
	    mycia_tas = CIAT_STOPPED;
	}
    } else if (s == strstr(s, "TB")) {
	sscanf(s + 2, "%u %u %u", &mycia_tbc, &mycia_tbl, &d1);
	mycia[CIA_CRB] = d1;
	if ((mycia[CIA_CRB] & 0x61) == 0x01) {
	    mycia_tbu = clk + mycia_tbc;
	    mycia_tbs = CIAT_RUNNING;
	    mycpu_set_alarm_clk(A_MYCIATB, mycia_tbu + 1);
	} else {
	    mycia_tbu = 0;
	    if ((mycia[CIA_CRB] & 0x61) == 0x41) {
		mycia_tbs = CIAT_COUNTTA;
	    } else {
		mycia_tbs = CIAT_STOPPED;
	    }
	}
    } else if (s == strstr(s, "ICR")) {
	sscanf(s + 3, "%d %d", &d1, &d2);
	myciaint = d1;
	myciaier = d2;
	if (myciaint & myciaier & 0x7f) {
	    my_set_int(I_MYCIAFL, MYCIA_INT, rclk);
	} else {
	    my_set_int(I_MYCIAFL, 0, rclk);
	}
    } else if (s == strstr(s, "SR")) {
	sscanf(s + 2, "%d %d", &d1, &myciasr_bits);
	mycia[CIA_SDR] = d1;
    } else if (s == strstr(s, "TOD")) {
	sscanf(s + 3, "%u %u %u %u", &d1, &d2, &d3, &d4);
	mycia[CIA_TOD_HR] = d1;
	mycia[CIA_TOD_MIN] = d2;
	mycia[CIA_TOD_SEC] = d3;
	mycia[CIA_TOD_TEN] = d4;
    } else {
	printf("unknown dump format line for MYCIA: \n%s\n", s);
    }
}

#endif

POST_CIA_FUNCS
