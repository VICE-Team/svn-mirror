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

/*
 * 29jun1998 a.fachat
 *
 * Implementing the peek function assumes that the READ_PA etc macros
 * do not have side-effects, i.e. they can be called more than once
 * at one clock cycle.
 *
 */

#include "vice.h"

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "cia.h"
#include "resources.h"
#include "snapshot.h"

#include "interrupt.h"

INCLUDES


#undef MYCIA_TIMER_DEBUG
#undef MYCIA_IO_DEBUG
#undef MYCIA_DUMP_DEBUG

#define	STORE_OFFSET 0
#define	READ_OFFSET 0

#define	CIAT_STOPPED	0
#define	CIAT_RUNNING	1
#define	CIAT_COUNTTA	2

#ifdef MYCIA_TIMER_DEBUG
#define	my_set_int(int_num, value, rclk)				\
    do {								\
        if (mycia_debugFlag)					\
	    printf("set_int(rclk=%d, int=%d, d=%d pc=)\n",		\
		   rclk,(int_num),(value));			\
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

/*
 * scheduling int_myciat[ab] calls -
 * warning: int_myciata uses mycpu_* stuff!
 */

#define	my_set_tai_clk(clk) 						\
    do {								\
	mycia_tai = clk;						\
	mycpu_set_alarm_clk(A_MYCIATA, clk);				\
    } while(0)

#define	my_unset_tai() 							\
    do {								\
	mycia_tai = -1;							\
	mycpu_unset_alarm(A_MYCIATA);					\
    } while(0)

#define	my_set_tbi_clk(clk) 						\
    do {								\
	mycia_tbi = clk;						\
	mycpu_set_alarm_clk(A_MYCIATB, clk);				\
    } while(0)

#define	my_unset_tbi() 							\
    do {								\
	mycia_tbi = -1;							\
	mycpu_unset_alarm(A_MYCIATB);					\
    } while(0)

/*
 * Those routines setup the myciat[ab]i clocks to a value above
 * rclk and schedule the next int_myciat[ab] alarm
 */
#define	update_tai(rclk)						\
    do {								\
	if(mycia_tai < rclk) {						\
	    int t = myciaint;						\
	    myciaint = 0;						\
	    int_myciata(rclk - mycia_tai);				\
	    myciaint |= t;						\
	}								\
    } while(0)

#define	update_tbi(rclk)						\
    do {								\
	if(mycia_tbi < rclk) {						\
	    int t = myciaint;						\
	    myciaint = 0;						\
	    int_myciatb(rclk - mycia_tbi);				\
	    myciaint |= t;						\
	}								\
    } while(0)

/* global */

static BYTE mycia[16];

#if defined(MYCIA_TIMER_DEBUG) || defined(MYCIA_IO_DEBUG)
int mycia_debugFlag = 0;

#endif

/* local functions */

static int update_mycia(CLOCK rclk);
static void check_myciatodalarm(CLOCK rclk);

/*
 * Local variables
 */

#define	myciaier	mycia[CIA_ICR]
static int myciaint;		/* Interrupt Flag register for cia 1 */

static CLOCK myciardi;		/* real clock = clk-offset */

static CLOCK mycia_tau;		/* when is the next underflow? */
static CLOCK mycia_tai;		/* when is the next int_* scheduled? */
static unsigned int mycia_tal;	/* latch value */
static unsigned int mycia_tac;	/* counter value */
static unsigned int mycia_tat;	/* timer A toggle bit */
static unsigned int mycia_tap;	/* timer A port bit */
static int mycia_tas;		/* timer state (CIAT_*) */

static CLOCK mycia_tbu;		/* when is the next underflow? */
static CLOCK mycia_tbi;		/* when is the next int_* scheduled? */
static unsigned int mycia_tbl;	/* latch value */
static unsigned int mycia_tbc;	/* counter value */
static unsigned int mycia_tbt;	/* timer B toggle bit */
static unsigned int mycia_tbp;	/* timer B port bit */
static int mycia_tbs;		/* timer state (CIAT_*) */

static int myciasr_bits;	/* number of bits still to send * 2 */

static BYTE oldpa;              /* the actual output on PA (input = high) */
static BYTE oldpb;              /* the actual output on PB (input = high) */

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
            my_set_int(I_MYCIAFL, MYCIA_INT, myclk);
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
    if (mycia_debugFlag)
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
		my_unset_tai();
		mycia_tac = mycia_tal;
		mycia_tas = CIAT_STOPPED;
		mycia[CIA_CRA] &= 0xfe;

		/* this is a HACK for arkanoid... */
		if (myciasr_bits) {
		    myciasr_bits--;
		    if(myciasr_bits==16) {
			STORE_SDR
		    }
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
    if (mycia_debugFlag)
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
		my_unset_tbi();
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
    if (mycia_debugFlag)
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
		if (mycia_debugFlag)
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
    if (mycia_debugFlag)
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

    my_unset_tbi();
    my_unset_tai();

    memset(myciatodalarm, 0, sizeof(myciatodalarm));
    myciatodlatched = 0;
    myciatodstopped = 0;
    mycpu_set_alarm(A_MYCIATOD, myciatodticks);

    myciaint = 0;
    my_set_int(I_MYCIAFL, 0, myclk);

    oldpa = 0xff;
    oldpb = 0xff;

    RESET_CIA
}


void REGPARM2 store_mycia(ADDRESS addr, BYTE byte)
{
    CLOCK rclk;

    addr &= 0xf;

    PRE_STORE_CIA

    rclk = myclk - STORE_OFFSET;

#ifdef MYCIA_TIMER_DEBUG
    if (mycia_debugFlag)
	printf("store mycia[%02x] %02x @ clk=%d, pc=\n",
	       (int) addr, (int) byte, rclk);
#endif

    switch (addr) {

      case CIA_PRA:		/* port A */
      case CIA_DDRA:
	mycia[addr] = byte;
	byte = mycia[CIA_PRA] | ~mycia[CIA_DDRA];
	STORE_CIAPA
	oldpa = byte;
	break;

      case CIA_PRB:		/* port B */
      case CIA_DDRB:
	mycia[addr] = byte;
	byte = mycia[CIA_PRB] | ~mycia[CIA_DDRB];
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
	oldpb = byte;
	break;

	/* This handles the timer latches.  The kludgy stuff is an attempt
           emulate the correct behavior when the latch is written to during
           an underflow.  */
      case CIA_TAL:
	update_tai(rclk); /* schedule alarm in case latch value is changed */
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
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
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
	update_tai(rclk); /* schedule alarm in case latch value is changed */
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
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
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
	if ((mycia[CIA_CRA] & 0x40) == 0x40) {
	    if (myciasr_bits <= 16) {
		if(!myciasr_bits) {
    	            STORE_SDR
		}
		if(myciasr_bits < 16) {
	            /* switch timer A alarm on again, if necessary */
	            update_mycia(rclk);
	            if (mycia_tau) {
		        my_set_tai_clk(mycia_tau + 1);
	            }
		}

	        myciasr_bits += 16;

#if defined (MYCIA_TIMER_DEBUG)
	        if (mycia_debugFlag)
	    	    printf("MYCIA: start SDR rclk=%d\n", rclk);
#endif
  	    }
	}
	break;

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Control Register */
	update_mycia(rclk);

#if defined (MYCIA_TIMER_DEBUG)
	if (mycia_debugFlag)
	    printf("MYCIA set CIA_ICR: 0x%x\n", byte);
#endif

	if (byte & CIA_IM_SET) {
	    myciaier |= (byte & 0x7f);
	} else {
	    myciaier &= ~(byte & 0x7f);
	}

	/* This must actually be delayed one cycle! */
#if defined(MYCIA_TIMER_DEBUG)
	if (mycia_debugFlag)
	    printf("    set icr: ifr & ier & 0x7f -> %02x, int=%02x\n",
		   myciaier & myciaint & 0x7f, myciaint);
#endif
	if (myciaier & myciaint & 0x7f) {
	    my_set_int(I_MYCIAFL, MYCIA_INT, rclk);
	}
	if (myciaier & (CIA_IM_TA + CIA_IM_TB)) {
	    if ((myciaier & CIA_IM_TA) && mycia_tau) {
		my_set_tai_clk(mycia_tau + 1);
	    }
	    if ((myciaier & CIA_IM_TB) && mycia_tbu) {
		my_set_tbi_clk(mycia_tbu + 1);
	    }
	}
	/* Control */
	break;

      case CIA_CRA:		/* control register A */
	update_tai(rclk); /* schedule alarm in case latch value is changed */
	update_mycia(rclk);
#if defined (MYCIA_TIMER_DEBUG)
	if (mycia_debugFlag)
	    printf("MYCIA set CIA_CRA: 0x%x (clk=%d, pc=, tal=%u, tac=%u)\n",
		   byte, rclk, /*program_counter,*/ mycia_tal, mycia_tac);
#endif

	/* bit 7 tod frequency */
	/* bit 6 serial port mode */

	/* bit 4 force load */
	if (byte & 0x10) {
	    mycia_tac = mycia_tal;
	    if (mycia_tas == CIAT_RUNNING) {
		mycia_tau = rclk + mycia_tac + 2;
		my_set_tai_clk(mycia_tau + 1);
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
		my_set_tai_clk(mycia_tau + 1);
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
		my_unset_tai();
	    }
	}
#if defined (MYCIA_TIMER_DEBUG)
	if (mycia_debugFlag)
	    printf("    -> tas=%d, tau=%d\n", mycia_tas, mycia_tau);
#endif
	mycia[addr] = byte & 0xef;	/* remove strobe */

	break;

      case CIA_CRB:		/* control register B */
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
	update_mycia(rclk);

#if defined (MYCIA_TIMER_DEBUG)
	if (mycia_debugFlag)
	    printf("MYCIA set CIA_CRB: 0x%x (clk=%d, pc=, tbl=%u, tbc=%u)\n",
		   byte, rclk, mycia_tbl, mycia_tbc);
#endif


	/* bit 7 set alarm/tod clock */
	/* bit 4 force load */
	if (byte & 0x10) {
	    mycia_tbc = mycia_tbl;
	    if (mycia_tbs == CIAT_RUNNING) {
		mycia_tbu = rclk + mycia_tbc + 2;
#if defined(MYCIA_TIMER_DEBUG)
		if (mycia_debugFlag)
		    printf("MYCIA: rclk=%d force load: set tbu alarm to %d\n", rclk, mycia_tbu);
#endif
		my_set_tbi_clk(mycia_tbu + 1);
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
		if (mycia_debugFlag)
		    printf("MYCIA: rclk=%d start timer: set tbu alarm to %d\n", rclk, mycia_tbu);
#endif
		my_set_tbi_clk(mycia_tbu + 1);
		mycia_tbs = CIAT_RUNNING;
	    } else {		/* timer just stopped */
#if defined(MYCIA_TIMER_DEBUG)
		if (mycia_debugFlag)
		    printf("MYCIA: rclk=%d stop timer: set tbu alarm\n", rclk);
#endif
		my_unset_tbi();
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
			my_set_tai_clk(mycia_tau + 1);
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

    if (mycia_debugFlag)
	printf("read mycia[%x] returns %02x @ clk=%d, pc=\n",
	       addr, tmp, myclk - READ_OFFSET);
    return tmp;
}

BYTE read_mycia_(ADDRESS addr)
{

#endif

    BYTE byte = 0xff;
    CLOCK rclk;

    addr &= 0xf;

    PRE_READ_CIA

    rclk = myclk - READ_OFFSET;


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
	return myciatodlatch[addr - CIA_TOD_TEN];

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
#ifdef MYCIA_TIMER_DEBUG
	    if (mycia_debugFlag)
		printf("MYCIA read intfl: rclk=%d, alarm_ta=%d, alarm_tb=%d\n",
			rclk, mycpu_int_status.alarm_clk[A_MYCIATA],
			mycpu_int_status.alarm_clk[A_MYCIATB]);
#endif

	    myciardi = rclk;
            t = myciaint;	/* we clean myciaint anyway, so make int_* */
	    myciaint = 0;	/* believe it is already */

            if (rclk >= mycia_tai)
                int_myciata(rclk - mycia_tai);
            if (rclk >= mycia_tbi)
                int_myciatb(rclk - mycia_tbi);

	    myciaint |= t;	/* some bits can be set -> or with old value */

	    update_mycia(rclk);
	    t = myciaint | myciaflag;

#ifdef MYCIA_TIMER_DEBUG
	    if (mycia_debugFlag)
		printf("MYCIA read intfl gives myciaint=%02x -> %02x @"
		       " PC=, sr_bits=%d, clk=%d, ta=%d, tb=%d\n",
		       myciaint, t, myciasr_bits, clk,
			(mycia_tac ? mycia_tac : mycia_tal),
			mycia_tbc);
#endif

	    myciaflag = 0;
	    myciaint = 0;
	    my_set_int(I_MYCIAFL, 0, rclk);

	    return (t);
	}
      case CIA_CRA:		/* Control Register A */
      case CIA_CRB:		/* Control Register B */
	update_mycia(rclk);
	return mycia[addr];
    }				/* switch */

    return (mycia[addr]);
}

BYTE REGPARM1 peek_mycia(ADDRESS addr)
{
    /* This code assumes that update_mycia is a projector - called at
     * the same cycle again it doesn't change anything. This way
     * it does not matter if we call it from peek first in the monitor
     * and probably the same cycle again when the CPU runs on...
     */
    CLOCK rclk;

    addr &= 0xf;

    PRE_PEEK_CIA

    rclk = myclk - READ_OFFSET;

    switch (addr) {

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
	return mycia[addr];

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Flag Register */
	{
	    BYTE t = 0;

	    READ_CIAICR
#ifdef MYCIA_TIMER_DEBUG
	    if (mycia_debugFlag)
		printf("MYCIA read intfl: rclk=%d, alarm_ta=%d, alarm_tb=%d\n",
			rclk, mycpu_int_status.alarm_clk[A_MYCIATA],
			mycpu_int_status.alarm_clk[A_MYCIATB]);
#endif

	    /* myciardi = rclk; makes int_* and update_mycia fiddle with IRQ */
            t = myciaint;	/* we clean myciaint anyway, so make int_* */
	    myciaint = 0;	/* believe it is already */

            if (rclk >= mycia_tai)
                int_myciata(rclk - mycia_tai);
            if (rclk >= mycia_tbi)
                int_myciatb(rclk - mycia_tbi);

	    myciaint |= t;	/* some bits can be set -> or with old value */

	    update_mycia(rclk);
	    t = myciaint | myciaflag;

#ifdef MYCIA_TIMER_DEBUG
	    if (mycia_debugFlag)
		printf("MYCIA read intfl gives myciaint=%02x -> %02x @"
		       " PC=, sr_bits=%d, clk=%d, ta=%d, tb=%d\n",
		       myciaint, t, myciasr_bits, clk,
			(mycia_tac ? mycia_tac : mycia_tal),
			mycia_tbc);
#endif

/*
	    myciaflag = 0;
	    myciaint = 0;
	    my_set_int(I_MYCIAFL, 0, rclk);
*/
	    return (t);
	}
      default:
	break;
    }				/* switch */

    return read_mycia(addr);
}

/* ------------------------------------------------------------------------- */

int int_myciata(long offset)
{
    CLOCK rclk = myclk - offset;

#if defined(MYCIA_TIMER_DEBUG)
    if (mycia_debugFlag)
	printf("MYCIA: int_myciata(rclk = %u, tal = %u, cra=%02x\n",
	       rclk, mycia_tal, mycia[CIA_CRA]);
#endif

    mycia_tat = (mycia_tat + 1) & 1;

    if ((mycia_tas == CIAT_RUNNING) && !(mycia[CIA_CRA] & 8)) {
	/* if we do not need alarm, no PB6, no shift register, and not timer B
	   counting timer A, then we can savely skip alarms... */
	if ( ( (myciaier & CIA_IM_TA) &&
		(!(myciaint & 0x80)) )
	    || (mycia[CIA_CRA] & 0x42)
	    || (mycia_tbs == CIAT_COUNTTA)) {
	    if(offset > mycia_tal+1) {
	        my_set_tai_clk(
			myclk - (offset % (mycia_tal+1)) + mycia_tal + 1 );
	    } else {
	        my_set_tai_clk(rclk + mycia_tal + 1 );
	    }
	} else {
	    /* mycia_tai = rclk + mycia_tal +1; - now keeps tai */
	    /* printf("mycia unset alarm: clk=%d, rclk=%d, rdi=%d -> tai=%d\n",
			myclk, rclk, myciardi, mycia_tai); */
	    mycpu_unset_alarm(A_MYCIATA);	/* do _not_ clear mycia_tai */
	}
    } else {
#if 0
	mycia_tas = CIAT_STOPPED;
	mycia[CIA_CRA] &= 0xfe;	/* clear run flag. Correct? */
	mycia_tau = 0;
#endif
	my_unset_tai();
    }

    if (mycia[CIA_CRA] & 0x40) {
	if (myciasr_bits) {
#if defined(MYCIA_TIMER_DEBUG)
	    if (mycia_debugFlag)
		printf("MYCIA: rclk=%d SDR: timer A underflow, bits=%d\n",
		       rclk, myciasr_bits);
#endif
	    if (!(--myciasr_bits)) {
		myciaint |= CIA_IM_SDR;
	    }
	    if(myciasr_bits == 16) {
		STORE_SDR
	    }
	}
    }
    if (mycia_tbs == CIAT_COUNTTA) {
	if (!mycia_tbc) {
	    mycia_tbc = mycia_tbl;
	    mycia_tbu = rclk;
#if defined(MYCIA_TIMER_DEBUG)
	    if (mycia_debugFlag)
		printf("MYCIA: timer B underflow when counting timer A occured, rclk=%d!\n", rclk);
#endif
	    myciaint |= CIA_IM_TB;
	    my_set_tbi_clk(rclk);
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
    CLOCK rclk = myclk - offset;

#if defined(MYCIA_TIMER_DEBUG)
    if (mycia_debugFlag)
	printf("MYCIA: timer B int_myciatb(rclk=%d, tbs=%d)\n", rclk, mycia_tbs);
#endif

    mycia_tbt = (mycia_tbt + 1) & 1;

    /* running and continous, then next alarm */
    if (mycia_tbs == CIAT_RUNNING) {
	if (!(mycia[CIA_CRB] & 8)) {
#if defined(MYCIA_TIMER_DEBUG)
	    if (mycia_debugFlag)
		printf("MYCIA: rclk=%d myciatb: set tbu alarm to %d\n", rclk, rclk + mycia_tbl + 1);
#endif
	    /* if no interrupt flag we can safely skip alarms */
	    if (myciaier & CIA_IM_TB) {
		if(offset > mycia_tbl+1) {
		    my_set_tbi_clk(
			myclk - (offset % (mycia_tbl+1)) + mycia_tbl + 1);
		} else {
		    my_set_tbi_clk(rclk + mycia_tbl + 1);
		}
	    } else {
		/* mycia_tbi = rclk + mycia_tbl + 1; */
		mycpu_unset_alarm(A_MYCIATB);
	    }
	} else {
#if 0
	    mycia_tbs = CIAT_STOPPED;
	    mycia[CIA_CRB] &= 0xfe; /* clear start bit */
	    mycia_tbu = 0;
#endif /* 0 */
#if defined(MYCIA_TIMER_DEBUG)
	    if (mycia_debugFlag)
		printf("MYCIA: rclk=%d myciatb: unset tbu alarm\n", rclk);
#endif
	    my_unset_tbi();
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
	my_unset_tbi();
#if defined(MYCIA_TIMER_DEBUG)
	if (mycia_debugFlag)
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
    myciaint |= CIA_IM_FLG;
    if (mycia[CIA_ICR] & CIA_IM_FLG) {
        my_set_int(I_MYCIAFL, MYCIA_INT, myclk);
    }
}

void mycia_set_sdr(BYTE data)
{
    mycia[CIA_SDR] = data;
    myciaint |= CIA_IM_SDR;
    if (mycia[CIA_ICR] & CIA_IM_SDR) {
        my_set_int(I_MYCIAFL, MYCIA_INT, myclk);
    }
}

/* ------------------------------------------------------------------------- */

int int_myciatod(long offset)
{
    int t, pm;
    CLOCK rclk = myclk - offset;

#ifdef DEBUG
    if (mycia_debugFlag)
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
	if (mycia_debugFlag)
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

    update_tai(myclk);
    update_tbi(myclk);

    update_mycia(myclk);

    if(mycia_tai && (mycia_tai != -1))
        mycia_tai -= sub;
    if(mycia_tbi && (mycia_tbi != -1))
        mycia_tbi -= sub;

    if (mycia_tau)
	mycia_tau -= sub;
    if (mycia_tbu)
	mycia_tbu -= sub;
    if (myciardi > sub)
	myciardi -= sub;
    else
	myciardi = 0;
}

/* -------------------------------------------------------------------------- */

/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 0/0
 */

#define	CIA_DUMP_VER_MAJOR	0
#define	CIA_DUMP_VER_MINOR	0

/*
 * The dump data:
 *
 * UBYTE	ORA
 * UBYTE	ORB
 * UBYTE	DDRA
 * UBYTE	DDRB
 * UWORD	TA
 * UWORD 	TB
 * UBYTE	TOD_TEN		current value
 * UBYTE	TOD_SEC
 * UBYTE	TOD_MIN
 * UBYTE	TOD_HR
 * UBYTE	SDR
 * UBYTE	IER		enabled interrupts mask
 * UBYTE	CRA
 * UBYTE	CRB
 *
 * UWORD	TAL		latch value
 * UWORD	TBL		latch value
 * UBYTE	IFR		interrupts currently active
 * UBYTE	PBSTATE		bit6/7 reflect PB6/7 toggle state
 *				bit 2/3 reflect port bit state
 * UBYTE	SRHBITS		number of half-bits to still shift in/out SDR
 * UBYTE	ALARM_TEN
 * UBYTE	ALARM_SEC
 * UBYTE	ALARM_MIN
 * UBYTE	ALARM_HR
 *
 * UBYTE	READICR		clk - when ICR was read last + 128
 * UBYTE	TODLATCHED	0=running, 1=latched, 2=stopped (writing)
 * UBYTE	TODL_TEN		latch value
 * UBYTE	TODL_SEC
 * UBYTE	TODL_MIN
 * UBYTE	TODL_HR
 * DWORD	TOD_TICKS	clk ticks till next tenth of second
 *
 * UBYTE	IRQ		0=IRQ line inactive, 1=IRQ line active
 */

/* FIXME!!!  Error check.  */
int mycia_write_snapshot_module(snapshot_t *p)
{
    snapshot_module_t *m;
    int byte;

    m = snapshot_module_create(p, "MYCIA",
                               CIA_DUMP_VER_MAJOR, CIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    update_tai(myclk); /* schedule alarm in case latch value is changed */
    update_tbi(myclk); /* schedule alarm in case latch value is changed */
    update_mycia(myclk);

#ifdef MYCIA_DUMP_DEBUG
printf("MYCIA: clk=%d, cra=%02x, crb=%02x, tas=%d, tbs=%d\n",myclk, mycia[CIA_CRA], mycia[CIA_CRB],mycia_tas, mycia_tbs);
printf("tai=%d, tau=%d, tac=%04x, tal=%04x\n",mycia_tai, mycia_tau, mycia_tac, mycia_tal);
printf("tbi=%d, tbu=%d, tbc=%04x, tbl=%04x\n",mycia_tbi, mycia_tbu, mycia_tbc, mycia_tbl);
printf("MYCIA: write myciaint=%02x, myciaier=%02x\n", myciaint, myciaier);
#endif

    snapshot_module_write_byte(m, mycia[CIA_PRA]);
    snapshot_module_write_byte(m, mycia[CIA_PRB]);
    snapshot_module_write_byte(m, mycia[CIA_DDRA]);
    snapshot_module_write_byte(m, mycia[CIA_DDRB]);
    snapshot_module_write_word(m, mycia_tac);
    snapshot_module_write_word(m, mycia_tbc);
    snapshot_module_write_byte(m, mycia[CIA_TOD_TEN]);
    snapshot_module_write_byte(m, mycia[CIA_TOD_SEC]);
    snapshot_module_write_byte(m, mycia[CIA_TOD_MIN]);
    snapshot_module_write_byte(m, mycia[CIA_TOD_HR]);
    snapshot_module_write_byte(m, mycia[CIA_SDR]);
    snapshot_module_write_byte(m, mycia[CIA_ICR]);
    snapshot_module_write_byte(m, mycia[CIA_CRA]);
    snapshot_module_write_byte(m, mycia[CIA_CRB]);

    snapshot_module_write_word(m, mycia_tal);
    snapshot_module_write_word(m, mycia_tbl);
    snapshot_module_write_byte(m, peek_mycia(CIA_ICR));
    snapshot_module_write_byte(m, ((mycia_tat ? 0x40 : 0)
                                   | (mycia_tbt ? 0x80 : 0)));
    snapshot_module_write_byte(m, myciasr_bits);
    snapshot_module_write_byte(m, myciatodalarm[0]);
    snapshot_module_write_byte(m, myciatodalarm[1]);
    snapshot_module_write_byte(m, myciatodalarm[2]);
    snapshot_module_write_byte(m, myciatodalarm[3]);

    if(myciardi) {
	if((myclk - myciardi) > 120) {
	    byte = 0;
	} else {
	    byte = myclk + 128 - myciardi;
	}
    } else {
	byte = 0;
    }
    snapshot_module_write_byte(m, byte);

    snapshot_module_write_byte(m, ((myciatodlatched ? 1 : 0)
                                   | (myciatodstopped ? 2 : 0)));
    snapshot_module_write_byte(m, myciatodlatch[0]);
    snapshot_module_write_byte(m, myciatodlatch[1]);
    snapshot_module_write_byte(m, myciatodlatch[2]);
    snapshot_module_write_byte(m, myciatodlatch[3]);

    snapshot_module_write_dword(m, (mycpu_int_status.alarm_clk[A_MYCIATOD]
                                    - myclk));

    snapshot_module_close(m);

    return 0;
}

int mycia_read_snapshot_module(snapshot_t *p)
{
    BYTE vmajor, vminor;
    BYTE byte;
    WORD word;
    DWORD dword;
    ADDRESS addr;
    CLOCK rclk = myclk;
    snapshot_module_t *m;

    m = snapshot_module_open(p, "MYCIA", &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (vmajor != CIA_DUMP_VER_MAJOR) {
        snapshot_module_close(m);
        return -1;
    }

    /* stop timers, just in case */
    mycia_tas = CIAT_STOPPED;
    mycia_tau = 0;
    my_unset_tai();
    mycia_tbs = CIAT_STOPPED;
    mycia_tbu = 0;
    my_unset_tbi();
    mycpu_unset_alarm(A_MYCIATOD);

    {
        snapshot_module_read_byte(m, &mycia[CIA_PRA]);
        snapshot_module_read_byte(m, &mycia[CIA_PRB]);
        snapshot_module_read_byte(m, &mycia[CIA_DDRA]);
        snapshot_module_read_byte(m, &mycia[CIA_DDRB]);

        addr = CIA_DDRA;
	byte = mycia[CIA_PRA] | ~mycia[CIA_DDRA];
        oldpa = byte ^ 0xff;	/* all bits change? */
        UNDUMP_CIAPA
        oldpa = byte;

        addr = CIA_DDRB;
	byte = mycia[CIA_PRB] | ~mycia[CIA_DDRB];
        oldpb = byte ^ 0xff;	/* all bits change? */
        UNDUMP_CIAPB
        oldpb = byte;
    }

    snapshot_module_read_word(m, &word);
    mycia_tac = word;
    snapshot_module_read_word(m, &word);
    mycia_tbc = word;
    snapshot_module_read_byte(m, &mycia[CIA_TOD_TEN]);
    snapshot_module_read_byte(m, &mycia[CIA_TOD_SEC]);
    snapshot_module_read_byte(m, &mycia[CIA_TOD_MIN]);
    snapshot_module_read_byte(m, &mycia[CIA_TOD_HR]);
    snapshot_module_read_byte(m, &mycia[CIA_SDR]);
    {
	STORE_SDR
    }
    snapshot_module_read_byte(m, &mycia[CIA_ICR]);
    snapshot_module_read_byte(m, &mycia[CIA_CRA]);
    snapshot_module_read_byte(m, &mycia[CIA_CRB]);

    snapshot_module_read_word(m, &word);
    mycia_tal = word;
    snapshot_module_read_word(m, &word);
    mycia_tbl = word;

    snapshot_module_read_byte(m, &byte);
    myciaint = byte;

#ifdef MYCIA_DUMP_DEBUG
printf("MYCIA: read myciaint=%02x, myciaier=%02x\n", myciaint, myciaier);
#endif

    snapshot_module_read_byte(m, &byte);
    mycia_tat = (byte & 0x40) ? 1 : 0;
    mycia_tbt = (byte & 0x80) ? 1 : 0;
    mycia_tap = (byte & 0x04) ? 1 : 0;
    mycia_tbp = (byte & 0x08) ? 1 : 0;

    snapshot_module_read_byte(m, &byte);
    myciasr_bits = byte;

    snapshot_module_read_byte(m, &myciatodalarm[0]);
    snapshot_module_read_byte(m, &myciatodalarm[1]);
    snapshot_module_read_byte(m, &myciatodalarm[2]);
    snapshot_module_read_byte(m, &myciatodalarm[3]);

    snapshot_module_read_byte(m, &byte);
    if(byte) {
	myciardi = myclk + 128 - byte;
    } else {
	myciardi = 0;
    }
#ifdef MYCIA_DUMP_DEBUG
printf("MYCIA: snap read rdi=%02x\n", byte);
printf("MYCIA: snap setting rdi to %d (rclk=%d)\n", myciardi, myclk);
#endif

    snapshot_module_read_byte(m, &byte);
    myciatodlatched = byte & 1;
    myciatodstopped = byte & 2;
    snapshot_module_read_byte(m, &myciatodlatch[0]);
    snapshot_module_read_byte(m, &myciatodlatch[1]);
    snapshot_module_read_byte(m, &myciatodlatch[2]);
    snapshot_module_read_byte(m, &myciatodlatch[3]);

    snapshot_module_read_dword(m, &dword);
    mycpu_set_alarm(A_MYCIATOD, dword);

    /* timer switch-on code from store_mycia[CIA_CRA/CRB] */

#ifdef MYCIA_DUMP_DEBUG
printf("MYCIA: clk=%d, cra=%02x, crb=%02x, tas=%d, tbs=%d\n",myclk, mycia[CIA_CRA], mycia[CIA_CRB],mycia_tas, mycia_tbs);
printf("tai=%d, tau=%d, tac=%04x, tal=%04x\n",mycia_tai, mycia_tau, mycia_tac, mycia_tal);
printf("tbi=%d, tbu=%d, tbc=%04x, tbl=%04x\n",mycia_tbi, mycia_tbu, mycia_tbc, mycia_tbl);
#endif

    if ((mycia[CIA_CRA] & 0x21) == 0x01) {        /* timer just started */
        mycia_tas = CIAT_RUNNING;
        mycia_tau = rclk + (mycia_tac /*+ 1) + ((byte & 0x10) >> 4*/ );
        my_set_tai_clk(mycia_tau + 1);
    }

    if ((mycia[CIA_CRB] & 0x61) == 0x01) {        /* timer just started */
        mycia_tbu = rclk + (mycia_tbc /*+ 1) + ((byte & 0x10) >> 4*/ );
        my_set_tbi_clk(mycia_tbu + 1);
        mycia_tbs = CIAT_RUNNING;
    } else
    if ((mycia[CIA_CRB] & 0x41) == 0x41) {
        mycia_tbs = CIAT_COUNTTA;
        update_mycia(rclk);
        /* switch timer A alarm on if necessary */
        if (mycia_tau) {
            my_set_tai_clk(mycia_tau + 1);
        }
    }

#ifdef MYCIA_DUMP_DEBUG
printf("MYCIA: clk=%d, cra=%02x, crb=%02x, tas=%d, tbs=%d\n",myclk, mycia[CIA_CRA], mycia[CIA_CRB],mycia_tas, mycia_tbs);
printf("tai=%d, tau=%d, tac=%04x, tal=%04x\n",mycia_tai, mycia_tau, mycia_tac, mycia_tal);
printf("tbi=%d, tbu=%d, tbc=%04x, tbl=%04x\n",mycia_tbi, mycia_tbu, mycia_tbc, mycia_tbl);
#endif

    if (mycia[CIA_ICR] & 0x80) {
        set_int_noclk(&mycpu_int_status, I_MYCIAFL, MYCIA_INT);
    } else {
        set_int_noclk(&mycpu_int_status, I_MYCIAFL, 0);
    }

    if (snapshot_module_close(m) < 0)
        return -1;

    return 0;
}


POST_CIA_FUNCS

