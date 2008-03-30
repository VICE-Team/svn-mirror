
/*
 * ../../../src/c64/c64cia1.c
 * This file is generated from ../../../src/cia-tmpl.c and ../../../src/c64/c64cia1.def,
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


#include "vmachine.h"
#include "vicii.h"
#include "maincpu.h"
#include "kbd.h"
#include "c64cia.h"
#ifdef HAVE_RS232
#include "rsuser.h"
#endif

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
        if (cia1_debugFlag)					\
	    printf("set_int(rclk=%d, int=%d, d=%d pc=)\n",		\
		   rclk,(int_num),(value));			\
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

/*
 * scheduling int_cia1t[ab] calls -
 * warning: int_cia1ta uses maincpu_* stuff!
 */

#define	my_set_tai_clk(clk) 						\
    do {								\
	cia1_tai = clk;						\
	maincpu_set_alarm_clk(A_CIA1TA, clk);				\
    } while(0)

#define	my_unset_tai() 							\
    do {								\
	cia1_tai = -1;							\
	maincpu_unset_alarm(A_CIA1TA);					\
    } while(0)

#define	my_set_tbi_clk(clk) 						\
    do {								\
	cia1_tbi = clk;						\
	maincpu_set_alarm_clk(A_CIA1TB, clk);				\
    } while(0)

#define	my_unset_tbi() 							\
    do {								\
	cia1_tbi = -1;							\
	maincpu_unset_alarm(A_CIA1TB);					\
    } while(0)

/*
 * Those routines setup the cia1t[ab]i clocks to a value above
 * rclk and schedule the next int_cia1t[ab] alarm
 */
#define	update_tai(rclk)							\
    do {								\
	if(cia1_tai < rclk) {						\
	    int t = cia1int;						\
	    cia1int = 0;						\
	    int_cia1ta(rclk - cia1_tai);				\
	    cia1int |= t;						\
	}								\
    } while(0)

#define	update_tbi(rclk)							\
    do {								\
	if(cia1_tbi < rclk) {						\
	    int t = cia1int;						\
	    cia1int = 0;						\
	    int_cia1tb(rclk - cia1_tbi);				\
	    cia1int |= t;						\
	}								\
    } while(0)

/* global */

static BYTE cia1[16];

#if defined(CIA1_TIMER_DEBUG) || defined(CIA1_IO_DEBUG)
int cia1_debugFlag = 0;

#endif

/* local functions */

static int update_cia1(CLOCK rclk);
static void check_cia1todalarm(CLOCK rclk);

/*
 * Local variables
 */

#define	cia1ier	cia1[CIA_ICR]
static int cia1int;		/* Interrupt Flag register for cia 1 */

static CLOCK cia1rdi;		/* real clock = clk-offset */

static CLOCK cia1_tau;		/* when is the next underflow? */
static CLOCK cia1_tai;		/* when is the next int_* scheduled? */
static unsigned int cia1_tal;	/* latch value */
static unsigned int cia1_tac;	/* counter value */
static unsigned int cia1_tat;	/* timer A toggle bit */
static unsigned int cia1_tap;	/* timer A port bit */
static int cia1_tas;		/* timer state (CIAT_*) */

static CLOCK cia1_tbu;		/* when is the next underflow? */
static CLOCK cia1_tbi;		/* when is the next int_* scheduled? */
static unsigned int cia1_tbl;	/* latch value */
static unsigned int cia1_tbc;	/* counter value */
static unsigned int cia1_tbt;	/* timer B toggle bit */
static unsigned int cia1_tbp;	/* timer B port bit */
static int cia1_tbs;		/* timer state (CIAT_*) */

static int cia1sr_bits;	/* number of bits still to send * 2 */

static BYTE oldpa;              /* the actual output on PA (input = high) */
static BYTE oldpb;              /* the actual output on PB (input = high) */

static BYTE cia1todalarm[4];
static BYTE cia1todlatch[4];
static char cia1todstopped;
static char cia1todlatched;
static int cia1todticks = 100000;	/* approx. a 1/10 sec. */

static BYTE cia1flag = 0;


/* ------------------------------------------------------------------------- */
/* CIA1 */


    /* Flag: Are the 3 C128 extended rows enabled?  */
    static int extended_keyboard_rows_enabled;

    /* Mask for the extended keyboard rows.  */
    static BYTE extended_keyboard_rows_mask;

    void cia1_enable_extended_keyboard_rows(int flag)
    {
	extended_keyboard_rows_enabled = flag;
    }

    void cia1_set_extended_keyboard_rows_mask(BYTE value)
    {
	extended_keyboard_rows_mask = value;
    }

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
    if (cia1_debugFlag)
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
		my_unset_tai();
		cia1_tac = cia1_tal;
		cia1_tas = CIAT_STOPPED;
		cia1[CIA_CRA] &= 0xfe;

		/* this is a HACK for arkanoid... */
		if (cia1sr_bits) {
		    cia1sr_bits--;
		    if(cia1sr_bits==16) {

#ifdef HAVE_RS232
    if (rsuser_enabled) {
	rsuser_tx_byte(cia1[CIA_SDR]);
    }
#endif
		    }
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
    if (cia1_debugFlag)
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
		my_unset_tbi();
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
    if (cia1_debugFlag)
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
		if (cia1_debugFlag)
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
    if (cia1_debugFlag)
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

    my_unset_tbi();
    my_unset_tai();

    memset(cia1todalarm, 0, sizeof(cia1todalarm));
    cia1todlatched = 0;
    cia1todstopped = 0;
    maincpu_set_alarm(A_CIA1TOD, cia1todticks);

    cia1int = 0;

    oldpa = 0xff;
    oldpb = 0xff;

    
}


void REGPARM2 store_cia1(ADDRESS addr, BYTE byte)
{
    CLOCK rclk;

    addr &= 0xf;


    vic_ii_handle_pending_alarms(maincpu_num_write_cycles());

    rclk = clk - STORE_OFFSET;

#ifdef CIA1_TIMER_DEBUG
    if (cia1_debugFlag)
	printf("store cia1[%02x] %02x @ clk=%d, pc=\n",
	       (int) addr, (int) byte, rclk);
#endif

    switch (addr) {

      case CIA_PRA:		/* port A */
      case CIA_DDRA:
	cia1[addr] = byte;
	byte = cia1[CIA_PRA] | ~cia1[CIA_DDRA];
	
	oldpa = byte;
	break;

      case CIA_PRB:		/* port B */
      case CIA_DDRB:
	cia1[addr] = byte;
	byte = cia1[CIA_PRB] | ~cia1[CIA_DDRB];
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
	/* Handle software-triggered light pen.  */
	if ( (byte ^ oldpb) & 0x10) {
	    vic_ii_trigger_light_pen(rclk);
	}
    }
	oldpb = byte;
	break;

	/* This handles the timer latches.  The kludgy stuff is an attempt
           emulate the correct behavior when the latch is written to during
           an underflow.  */
      case CIA_TAL:
	update_tai(rclk); /* schedule alarm in case latch value is changed */
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
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
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
	update_tai(rclk); /* schedule alarm in case latch value is changed */
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
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
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
	if ((cia1[CIA_CRA] & 0x40) == 0x40) {
	    if (cia1sr_bits <= 16) {
		if(!cia1sr_bits) {

#ifdef HAVE_RS232
    if (rsuser_enabled) {
	rsuser_tx_byte(cia1[CIA_SDR]);
    }
#endif
		}
		if(cia1sr_bits < 16) {
	            /* switch timer A alarm on again, if necessary */
	            update_cia1(rclk);
	            if (cia1_tau) {
		        my_set_tai_clk(cia1_tau + 1);
	            }
		}

	        cia1sr_bits += 16;

#if defined (CIA1_TIMER_DEBUG)
	        if (cia1_debugFlag)
	    	    printf("CIA1: start SDR rclk=%d\n", rclk);
#endif
  	    }
	}
	break;

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Control Register */
	update_cia1(rclk);

#if defined (CIA1_TIMER_DEBUG)
	if (cia1_debugFlag)
	    printf("CIA1 set CIA_ICR: 0x%x\n", byte);
#endif

	if (byte & CIA_IM_SET) {
	    cia1ier |= (byte & 0x7f);
	} else {
	    cia1ier &= ~(byte & 0x7f);
	}

	/* This must actually be delayed one cycle! */
#if defined(CIA1_TIMER_DEBUG)
	if (cia1_debugFlag)
	    printf("    set icr: ifr & ier & 0x7f -> %02x, int=%02x\n",
		   cia1ier & cia1int & 0x7f, cia1int);
#endif
	if (cia1ier & cia1int & 0x7f) {
	    my_set_int(I_CIA1FL, IK_IRQ, rclk);
	}
	if (cia1ier & (CIA_IM_TA + CIA_IM_TB)) {
	    if ((cia1ier & CIA_IM_TA) && cia1_tau) {
		my_set_tai_clk(cia1_tau + 1);
	    }
	    if ((cia1ier & CIA_IM_TB) && cia1_tbu) {
		my_set_tbi_clk(cia1_tbu + 1);
	    }
	}
	/* Control */
	break;

      case CIA_CRA:		/* control register A */
	update_tai(rclk); /* schedule alarm in case latch value is changed */
	update_cia1(rclk);
#if defined (CIA1_TIMER_DEBUG)
	if (cia1_debugFlag)
	    printf("CIA1 set CIA_CRA: 0x%x (clk=%d, pc=, tal=%u, tac=%u)\n",
		   byte, rclk, /*program_counter,*/ cia1_tal, cia1_tac);
#endif

	/* bit 7 tod frequency */
	/* bit 6 serial port mode */

	/* bit 4 force load */
	if (byte & 0x10) {
	    cia1_tac = cia1_tal;
	    if (cia1_tas == CIAT_RUNNING) {
		cia1_tau = rclk + cia1_tac + 2;
		my_set_tai_clk(cia1_tau + 1);
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
		my_set_tai_clk(cia1_tau + 1);
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
		my_unset_tai();
	    }
	}
#if defined (CIA1_TIMER_DEBUG)
	if (cia1_debugFlag)
	    printf("    -> tas=%d, tau=%d\n", cia1_tas, cia1_tau);
#endif
	cia1[addr] = byte & 0xef;	/* remove strobe */

	break;

      case CIA_CRB:		/* control register B */
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
	update_cia1(rclk);

#if defined (CIA1_TIMER_DEBUG)
	if (cia1_debugFlag)
	    printf("CIA1 set CIA_CRB: 0x%x (clk=%d, pc=, tbl=%u, tbc=%u)\n",
		   byte, rclk, cia1_tbl, cia1_tbc);
#endif


	/* bit 7 set alarm/tod clock */
	/* bit 4 force load */
	if (byte & 0x10) {
	    cia1_tbc = cia1_tbl;
	    if (cia1_tbs == CIAT_RUNNING) {
		cia1_tbu = rclk + cia1_tbc + 2;
#if defined(CIA1_TIMER_DEBUG)
		if (cia1_debugFlag)
		    printf("CIA1: rclk=%d force load: set tbu alarm to %d\n", rclk, cia1_tbu);
#endif
		my_set_tbi_clk(cia1_tbu + 1);
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
		if (cia1_debugFlag)
		    printf("CIA1: rclk=%d start timer: set tbu alarm to %d\n", rclk, cia1_tbu);
#endif
		my_set_tbi_clk(cia1_tbu + 1);
		cia1_tbs = CIAT_RUNNING;
	    } else {		/* timer just stopped */
#if defined(CIA1_TIMER_DEBUG)
		if (cia1_debugFlag)
		    printf("CIA1: rclk=%d stop timer: set tbu alarm\n", rclk);
#endif
		my_unset_tbi();
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
			my_set_tai_clk(cia1_tau + 1);
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

    if (cia1_debugFlag)
	printf("read cia1[%x] returns %02x @ clk=%d, pc=\n",
	       addr, tmp, clk - READ_OFFSET);
    return tmp;
}

BYTE read_cia1_(ADDRESS addr)
{

#endif

    BYTE byte;
    CLOCK rclk;

    addr &= 0xf;


    vic_ii_handle_pending_alarms(0);

    rclk = clk - READ_OFFSET;


    switch (addr) {

      case CIA_PRA:		/* port A */

    {
	BYTE val = oldpa;
	BYTE msk = (oldpb) & ~joy[2];
	BYTE m;
	int i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
		val &= ~rev_keyarr[i];
	byte = val & ~joy[2];
    }
	byte = (byte & ~cia1[CIA_DDRA]) | (cia1[CIA_PRA] & cia1[CIA_DDRA]);
	return byte;
	break;

      case CIA_PRB:		/* port B */

    {
	BYTE val = ~cia1[CIA_DDRB];
	BYTE msk = (oldpa) & ~joy[1];
	BYTE m;
	int i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
		val &= ~keyarr[i];

	if (extended_keyboard_rows_enabled)
	    for (m = 0x1, i = 8; i < 11; m <<= 1, i++)
		if (!(extended_keyboard_rows_mask & m))
		    val &= ~keyarr[i];

	byte = (val /* | (cia1[CIA_PRB] & cia1[CIA_DDRB])*/ ) & ~joy[1];
    }
	byte = (byte & ~cia1[CIA_DDRB]) | (cia1[CIA_PRB] & cia1[CIA_DDRB]);
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
	return cia1todlatch[addr - CIA_TOD_TEN];

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

	    
#ifdef CIA1_TIMER_DEBUG
	    if (cia1_debugFlag)
		printf("CIA1 read intfl: rclk=%d, alarm_ta=%d, alarm_tb=%d\n",
			rclk, maincpu_int_status.alarm_clk[A_CIA1TA],
			maincpu_int_status.alarm_clk[A_CIA1TB]);
#endif

	    cia1rdi = rclk;
            t = cia1int;	/* we clean cia1int anyway, so make int_* */
	    cia1int = 0;	/* believe it is already */

            if (rclk >= cia1_tai)
                int_cia1ta(rclk - cia1_tai);
            if (rclk >= cia1_tbi)
                int_cia1tb(rclk - cia1_tbi);

	    cia1int |= t;	/* some bits can be set -> or with old value */

	    update_cia1(rclk);
	    t = cia1int | cia1flag;

#ifdef CIA1_TIMER_DEBUG
	    if (cia1_debugFlag)
		printf("CIA1 read intfl gives cia1int=%02x -> %02x @"
		       " PC=, sr_bits=%d, clk=%d, ta=%d, tb=%d\n",
		       cia1int, t, cia1sr_bits, clk,
			(cia1_tac ? cia1_tac : cia1_tal),
			cia1_tbc);
#endif

	    cia1flag = 0;
	    cia1int = 0;
	    my_set_int(I_CIA1FL, 0, rclk);

	    return (t);
	}
    }				/* switch */

    return (cia1[addr]);
}

BYTE REGPARM1 peek_cia1(ADDRESS addr)
{
    /* This code assumes that update_cia1 is a projector - called at
     * the same cycle again it doesn't change anything. This way
     * it does not matter if we call it from peek first in the monitor
     * and probably the same cycle again when the CPU runs on...
     */
    CLOCK rclk;

    addr &= 0xf;


    vic_ii_handle_pending_alarms(0);

    rclk = clk - READ_OFFSET;

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
	if (!cia1todlatched)
	    memcpy(cia1todlatch, cia1 + CIA_TOD_TEN, sizeof(cia1todlatch));
	return cia1[addr];

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Flag Register */
	{
	    BYTE t = 0;

	    
#ifdef CIA1_TIMER_DEBUG
	    if (cia1_debugFlag)
		printf("CIA1 read intfl: rclk=%d, alarm_ta=%d, alarm_tb=%d\n",
			rclk, maincpu_int_status.alarm_clk[A_CIA1TA],
			maincpu_int_status.alarm_clk[A_CIA1TB]);
#endif

	    /* cia1rdi = rclk; makes int_* and update_cia1 fiddle with IRQ */
            t = cia1int;	/* we clean cia1int anyway, so make int_* */
	    cia1int = 0;	/* believe it is already */

            if (rclk >= cia1_tai)
                int_cia1ta(rclk - cia1_tai);
            if (rclk >= cia1_tbi)
                int_cia1tb(rclk - cia1_tbi);

	    cia1int |= t;	/* some bits can be set -> or with old value */

	    update_cia1(rclk);
	    t = cia1int | cia1flag;

#ifdef CIA1_TIMER_DEBUG
	    if (cia1_debugFlag)
		printf("CIA1 read intfl gives cia1int=%02x -> %02x @"
		       " PC=, sr_bits=%d, clk=%d, ta=%d, tb=%d\n",
		       cia1int, t, cia1sr_bits, clk,
			(cia1_tac ? cia1_tac : cia1_tal),
			cia1_tbc);
#endif

/*
	    cia1flag = 0;
	    cia1int = 0;
	    my_set_int(I_CIA1FL, 0, rclk);
*/
	    return (t);
	}
      default:
	break;
    }				/* switch */

    return read_cia1(addr);
}

/* ------------------------------------------------------------------------- */

int int_cia1ta(long offset)
{
    CLOCK rclk = clk - offset;

#if defined(CIA1_TIMER_DEBUG)
    if (cia1_debugFlag)
	printf("CIA1: int_cia1ta(rclk = %u, tal = %u, cra=%02x\n",
	       rclk, cia1_tal, cia1[CIA_CRA]);
#endif

    cia1_tat = (cia1_tat + 1) & 1;

    if ((cia1_tas == CIAT_RUNNING) && !(cia1[CIA_CRA] & 8)) {
	/* if we do not need alarm, no PB6, no shift register, and not timer B
	   counting timer A, then we can savely skip alarms... */
	if ( ( (cia1ier & CIA_IM_TA) &&
		(!(cia1int & 0x80)) )
	    || (cia1[CIA_CRA] & 0x42)
	    || (cia1_tbs == CIAT_COUNTTA)) {
	    if(offset > cia1_tal+1) {
	        my_set_tai_clk(
			clk - (offset % (cia1_tal+1)) + cia1_tal + 1 );
	    } else {
	        my_set_tai_clk(rclk + cia1_tal + 1 );
	    }
	} else {
	    /* cia1_tai = rclk + cia1_tal +1; - now keeps tai */
	    /* printf("cia1 unset alarm: clk=%d, rclk=%d, rdi=%d -> tai=%d\n",
			clk, rclk, cia1rdi, cia1_tai); */
	    maincpu_unset_alarm(A_CIA1TA);	/* do _not_ clear cia1_tai */
	}
    } else {
#if 0
	cia1_tas = CIAT_STOPPED;
	cia1[CIA_CRA] &= 0xfe;	/* clear run flag. Correct? */
	cia1_tau = 0;
#endif
	my_unset_tai();
    }

    if (cia1[CIA_CRA] & 0x40) {
	if (cia1sr_bits) {
#if defined(CIA1_TIMER_DEBUG)
	    if (cia1_debugFlag)
		printf("CIA1: rclk=%d SDR: timer A underflow, bits=%d\n",
		       rclk, cia1sr_bits);
#endif
	    if (!(--cia1sr_bits)) {
		cia1int |= CIA_IM_SDR;
	    }
	    if(cia1sr_bits == 16) {

#ifdef HAVE_RS232
    if (rsuser_enabled) {
	rsuser_tx_byte(cia1[CIA_SDR]);
    }
#endif
	    }
	}
    }
    if (cia1_tbs == CIAT_COUNTTA) {
	if (!cia1_tbc) {
	    cia1_tbc = cia1_tbl;
	    cia1_tbu = rclk;
#if defined(CIA1_TIMER_DEBUG)
	    if (cia1_debugFlag)
		printf("CIA1: timer B underflow when counting timer A occured, rclk=%d!\n", rclk);
#endif
	    cia1int |= CIA_IM_TB;
	    my_set_tbi_clk(rclk);
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
    if (cia1_debugFlag)
	printf("CIA1: timer B int_cia1tb(rclk=%d, tbs=%d)\n", rclk, cia1_tbs);
#endif

    cia1_tbt = (cia1_tbt + 1) & 1;

    /* running and continous, then next alarm */
    if (cia1_tbs == CIAT_RUNNING) {
	if (!(cia1[CIA_CRB] & 8)) {
#if defined(CIA1_TIMER_DEBUG)
	    if (cia1_debugFlag)
		printf("CIA1: rclk=%d cia1tb: set tbu alarm to %d\n", rclk, rclk + cia1_tbl + 1);
#endif
	    /* if no interrupt flag we can safely skip alarms */
	    if (cia1ier & CIA_IM_TB) {
		if(offset > cia1_tbl+1) {
		    my_set_tbi_clk(
			clk - (offset % (cia1_tbl+1)) + cia1_tbl + 1);
		} else {
		    my_set_tbi_clk(rclk + cia1_tbl + 1);
		}
	    } else {
		/* cia1_tbi = rclk + cia1_tbl + 1; */
		maincpu_unset_alarm(A_CIA1TB);
	    }
	} else {
#if 0
	    cia1_tbs = CIAT_STOPPED;
	    cia1[CIA_CRB] &= 0xfe; /* clear start bit */
	    cia1_tbu = 0;
#endif /* 0 */
#if defined(CIA1_TIMER_DEBUG)
	    if (cia1_debugFlag)
		printf("CIA1: rclk=%d cia1tb: unset tbu alarm\n", rclk);
#endif
	    my_unset_tbi();
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
	my_unset_tbi();
#if defined(CIA1_TIMER_DEBUG)
	if (cia1_debugFlag)
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
    cia1int |= CIA_IM_FLG;
    if (cia1[CIA_ICR] & CIA_IM_FLG) {
        my_set_int(I_CIA1FL, IK_IRQ, clk);
    }
}

void cia1_set_sdr(BYTE data)
{
    cia1[CIA_SDR] = data;
    cia1int |= CIA_IM_SDR;
    if (cia1[CIA_ICR] & CIA_IM_SDR) {
        my_set_int(I_CIA1FL, IK_IRQ, clk);
    }
}

/* ------------------------------------------------------------------------- */

int int_cia1tod(long offset)
{
    int t, pm;
    CLOCK rclk = clk - offset;

#ifdef DEBUG
    if (cia1_debugFlag)
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
	if (cia1_debugFlag)
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


void cia1_prevent_clk_overflow(CLOCK sub)
{

    update_tai(clk);
    update_tbi(clk);

    update_cia1(clk);

    if(cia1_tai && (cia1_tai != -1))
        cia1_tai -= sub;
    if(cia1_tbi && (cia1_tbi != -1))
        cia1_tbi -= sub;

    if (cia1_tau)
	cia1_tau -= sub;
    if (cia1_tbu)
	cia1_tbu -= sub;
    if (cia1rdi > sub)
	cia1rdi -= sub;
    else
	cia1rdi = 0;
}

/* -------------------------------------------------------------------------- */

#if 1

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
 * UBYTE	READICR		offset when ICR was read last
 * UBYTE	TODLATCHED	0=running, 1=latched, 2=stopped (writing)
 * UBYTE	TODL_TEN		latch value
 * UBYTE	TODL_SEC
 * UBYTE	TODL_MIN
 * UBYTE	TODL_HR
 * DWORD	TOD_TICKS	clk ticks till next tenth of second
 *
 */

/* FIXME!!!  Error check.  */
int cia1_write_snapshot_module(FILE * p)
{
    snapshot_module_t *m;
    int byte;

    m = snapshot_module_create(p, "CIA1",
                               CIA_DUMP_VER_MAJOR, CIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    update_cia1(clk);

    snapshot_module_write_byte(m, cia1[CIA_PRA]);
    snapshot_module_write_byte(m, cia1[CIA_PRB]);
    snapshot_module_write_byte(m, cia1[CIA_DDRA]);
    snapshot_module_write_byte(m, cia1[CIA_DDRB]);
    snapshot_module_write_word(m, cia1_tac);
    snapshot_module_write_word(m, cia1_tbc);
    snapshot_module_write_byte(m, cia1[CIA_TOD_TEN]);
    snapshot_module_write_byte(m, cia1[CIA_TOD_SEC]);
    snapshot_module_write_byte(m, cia1[CIA_TOD_MIN]);
    snapshot_module_write_byte(m, cia1[CIA_TOD_HR]);
    snapshot_module_write_byte(m, cia1[CIA_SDR]);
    snapshot_module_write_byte(m, cia1[CIA_ICR]);
    snapshot_module_write_byte(m, cia1[CIA_CRA]);
    snapshot_module_write_byte(m, cia1[CIA_CRB]);

    snapshot_module_write_word(m, cia1_tal);
    snapshot_module_write_word(m, cia1_tbl);
    snapshot_module_write_byte(m, peek_cia1(CIA_ICR));
    snapshot_module_write_byte(m, ((cia1_tat ? 0x40 : 0)
                                   | (cia1_tbt ? 0x80 : 0)));
    snapshot_module_write_byte(m, cia1sr_bits);
    snapshot_module_write_byte(m, cia1todalarm[0]);
    snapshot_module_write_byte(m, cia1todalarm[1]);
    snapshot_module_write_byte(m, cia1todalarm[2]);
    snapshot_module_write_byte(m, cia1todalarm[3]);

    byte = cia1rdi ? clk - cia1rdi : 0;
    if (byte > 128 || byte < -16)
        byte = 0;
    snapshot_module_write_byte(m, byte);

    snapshot_module_write_byte(m, ((cia1todlatched ? 1 : 0)
                                   | (cia1todstopped ? 2 : 0)));
    snapshot_module_write_byte(m, cia1todlatch[0]);
    snapshot_module_write_byte(m, cia1todlatch[1]);
    snapshot_module_write_byte(m, cia1todlatch[2]);
    snapshot_module_write_byte(m, cia1todlatch[3]);

    snapshot_module_write_dword(m, (maincpu_int_status.alarm_clk[A_CIA1TOD]
                                    - clk));

    snapshot_module_close(m);

    return 0;
}

int cia1_read_snapshot_module(FILE * p)
{
    char name[SNAPSHOT_MODULE_NAME_LEN];
    BYTE vmajor, vminor;
    BYTE byte;
    WORD word;
    DWORD dword;
    ADDRESS addr;
    CLOCK rclk = clk;
    snapshot_module_t *m;

    m = snapshot_module_open(p, name, &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (strcmp(name, "CIA1") || vmajor != CIA_DUMP_VER_MAJOR) {
        snapshot_module_close(m);
        return -1;
    }

    /* Argh.  This is ugly.  */
    {
        snapshot_module_read_byte(m, &byte);
        addr = CIA_PRA;
        oldpa = byte ^ 0xff;
        
        oldpa = byte;

        snapshot_module_read_byte(m, &byte);
        addr = CIA_PRB;
        oldpa = byte ^ 0xff;

    {
	/* Handle software-triggered light pen.  */
	if ( (byte ^ oldpb) & 0x10) {
	    vic_ii_trigger_light_pen(rclk);
	}
    }
        oldpa = byte;

        snapshot_module_read_byte(m, &byte);
        addr = CIA_DDRA;
        oldpa = byte ^ 0xff;
        
        oldpa = byte;

        snapshot_module_read_byte(m, &byte);
        addr = CIA_DDRB;
        oldpa = byte ^ 0xff;

    {
	/* Handle software-triggered light pen.  */
	if ( (byte ^ oldpb) & 0x10) {
	    vic_ii_trigger_light_pen(rclk);
	}
    }
        oldpa = byte;
    }

    snapshot_module_read_word(m, &word);
    cia1_tac = word;
    snapshot_module_read_word(m, &word);
    cia1_tbc = word;
    snapshot_module_read_byte(m, &cia1[CIA_TOD_TEN]);
    snapshot_module_read_byte(m, &cia1[CIA_TOD_SEC]);
    snapshot_module_read_byte(m, &cia1[CIA_TOD_MIN]);
    snapshot_module_read_byte(m, &cia1[CIA_TOD_HR]);
    snapshot_module_read_byte(m, &cia1[CIA_SDR]);
    {

#ifdef HAVE_RS232
    if (rsuser_enabled) {
	rsuser_tx_byte(cia1[CIA_SDR]);
    }
#endif
    }
    snapshot_module_read_byte(m, &cia1[CIA_ICR]);
    snapshot_module_read_byte(m, &cia1[CIA_CRA]);
    snapshot_module_read_byte(m, &cia1[CIA_CRB]);

    snapshot_module_read_word(m, &word);
    cia1_tal = word;
    snapshot_module_read_word(m, &word);
    cia1_tbl = word;

    snapshot_module_read_byte(m, &byte);
    cia1int = byte;

    /* my_set_int(I_CIA1FL, IK_IRQ, clk); */

    snapshot_module_read_byte(m, &byte);
    cia1_tat = (byte & 0x40) ? 1 : 0;
    cia1_tbt = (byte & 0x80) ? 1 : 0;
    cia1_tap = (byte & 0x04) ? 1 : 0;
    cia1_tbp = (byte & 0x08) ? 1 : 0;

    snapshot_module_read_byte(m, &byte);
    cia1sr_bits = byte;

    snapshot_module_read_byte(m, &cia1todalarm[0]);
    snapshot_module_read_byte(m, &cia1todalarm[1]);
    snapshot_module_read_byte(m, &cia1todalarm[2]);
    snapshot_module_read_byte(m, &cia1todalarm[3]);

    snapshot_module_read_byte(m, &byte);
    if(byte) {
	cia1rdi = clk + byte;
    }

    snapshot_module_read_byte(m, &byte);
    cia1todlatched = byte & 1;
    cia1todstopped = byte & 2;
    snapshot_module_read_byte(m, &cia1todlatch[0]);
    snapshot_module_read_byte(m, &cia1todlatch[1]);
    snapshot_module_read_byte(m, &cia1todlatch[2]);
    snapshot_module_read_byte(m, &cia1todlatch[3]);

    snapshot_module_read_dword(m, &dword);
    maincpu_set_alarm(A_CIA1TOD, dword);

    cia1_tau = clk + cia1_tac;
    my_set_tai_clk(cia1_tau + 1);
    cia1_tbu = clk + cia1_tbc;
    my_set_tbi_clk(cia1_tbu + 1);

    cia1_tas = (cia1[CIA_CRA] & 1) ? CIAT_RUNNING : CIAT_STOPPED;
    cia1_tbs = (cia1[CIA_CRB] & 1) ? CIAT_RUNNING : CIAT_STOPPED;
    if ((cia1[CIA_CRB] & 0x41) == 0x41)
        cia1_tbs = CIAT_COUNTTA;

    if (snapshot_module_close(m) < 0)
        return -1;

    return 0;
}

#endif




