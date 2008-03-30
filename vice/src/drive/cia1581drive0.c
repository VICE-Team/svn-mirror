
/*
 * ../../src/drive/cia1581drive0.c
 * This file is generated from ../../src/cia-tmpl.c and ../../src/drive/cia1581drive0.def,
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
 * There now is a new function, update_cia1581d0(). It computes all differences
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
 * may be set a few cycles too late due to late call of int_cia1581d0t*() due to
 * opcode execution time. This can be fixed by checking in the beginning of
 * read_* and write_* if an int_* is scheduled and executing it before.  Then
 * the setting of the ICR could also be moved from update to int_*().  But
 * the bug only affects the contents of the ICR. The interrupt is generated
 * at the right time (hopefully).
 *
 * There is one HACK to make a game work: in update_cia1581d0() a fix is done for
 * Arkanoid. This game counts shift register bits (i.e. TA underflows) by
 * setting TA to one-shot.  The ICR is read just before the int_cia1581d0ta()
 * function is called, and the int bit is missed, so there is a check in
 * update_cia1581d0() (this is probably a fix and not a hack... :-)
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


#include "drive.h"
#include "iecdrive.h"
#include "ciad.h"

#include "interrupt.h"

#undef CIA1581D0_TIMER_DEBUG
#undef CIA1581D0_IO_DEBUG

#define	STORE_OFFSET 0
#define	READ_OFFSET 0

#define	CIAT_STOPPED	0
#define	CIAT_RUNNING	1
#define	CIAT_COUNTTA	2

#ifdef CIA1581D0_TIMER_DEBUG
#define	my_set_int(int_num, value, rclk)				\
    do {								\
        if (cia1581d0_debugFlag)					\
	    printf("set_int(rclk=%d, int=%d, d=%d pc=)\n",		\
		   rclk,(int_num),(value));			\
	drive0_set_irq_clk((int_num), (value), (rclk));			\
	if ((value))							\
	    cia1581d0int |= 0x80;						\
    } while(0)
#else
#define	my_set_int(int_num, value, rclk)				 \
    do {								 \
        drive0_set_irq_clk((int_num), (value), (rclk));			 \
	if ((value))							 \
	    cia1581d0int |= 0x80;						 \
    } while(0)
#endif

/* 
 * scheduling int_cia1581d0t[ab] calls - 
 * warning: int_cia1581d0ta uses drive0_* stuff! 
 */

#define	my_set_tai_clk(clk) 						\
    do {								\
	cia1581d0_tai = clk;						\
	drive0_set_alarm_clk(A_CIA1581D0TA, clk);				\
    } while(0)

#define	my_unset_tai() 							\
    do {								\
	cia1581d0_tai = -1;							\
	drive0_unset_alarm(A_CIA1581D0TA);					\
    } while(0)

#define	my_set_tbi_clk(clk) 						\
    do {								\
	cia1581d0_tbi = clk;						\
	drive0_set_alarm_clk(A_CIA1581D0TB, clk);				\
    } while(0)

#define	my_unset_tbi() 							\
    do {								\
	cia1581d0_tbi = -1;							\
	drive0_unset_alarm(A_CIA1581D0TB);					\
    } while(0)

/*
 * Those routines setup the cia1581d0t[ab]i clocks to a value above
 * rclk and schedule the next int_cia1581d0t[ab] alarm
 */
#define	update_tai(rclk)							\
    do {								\
	if(cia1581d0_tai < rclk) {						\
	    int t = cia1581d0int;						\
	    cia1581d0int = 0;						\
	    int_cia1581d0ta(rclk - cia1581d0_tai);				\
	    cia1581d0int |= t;						\
	}								\
    } while(0)

#define	update_tbi(rclk)							\
    do {								\
	if(cia1581d0_tbi < rclk) {						\
	    int t = cia1581d0int;						\
	    cia1581d0int = 0;						\
	    int_cia1581d0tb(rclk - cia1581d0_tbi);				\
	    cia1581d0int |= t;						\
	}								\
    } while(0)

/* global */

static BYTE cia1581d0[16];

#if defined(CIA1581D0_TIMER_DEBUG) || defined(CIA1581D0_IO_DEBUG)
int cia1581d0_debugFlag = 0;

#endif

/* local functions */

static int update_cia1581d0(CLOCK rclk);
static void check_cia1581d0todalarm(CLOCK rclk);

void cia1581d0_dump(FILE * fp);

/*
 * Local variables
 */

#define	cia1581d0ier	cia1581d0[CIA_ICR]
static int cia1581d0int;		/* Interrupt Flag register for cia 1 */

static CLOCK cia1581d0rdi;		/* real clock = clk-offset */

static CLOCK cia1581d0_tau;		/* when is the next underflow? */
static CLOCK cia1581d0_tai;		/* when is the next int_* scheduled? */
static unsigned int cia1581d0_tal;	/* latch value */
static unsigned int cia1581d0_tac;	/* counter value */
static unsigned int cia1581d0_tat;	/* timer A toggle bit */
static unsigned int cia1581d0_tap;	/* timer A port bit */
static int cia1581d0_tas;		/* timer state (CIAT_*) */

static CLOCK cia1581d0_tbu;		/* when is the next underflow? */
static CLOCK cia1581d0_tbi;		/* when is the next int_* scheduled? */
static unsigned int cia1581d0_tbl;	/* latch value */
static unsigned int cia1581d0_tbc;	/* counter value */
static unsigned int cia1581d0_tbt;	/* timer B toggle bit */
static unsigned int cia1581d0_tbp;	/* timer B port bit */
static int cia1581d0_tbs;		/* timer state (CIAT_*) */

static int cia1581d0sr_bits;	/* number of bits still to send * 2 */

static BYTE cia1581d0todalarm[4];
static BYTE cia1581d0todlatch[4];
static char cia1581d0todstopped;
static char cia1581d0todlatched;
static int cia1581d0todticks = 100000;	/* approx. a 1/10 sec. */

static BYTE cia1581d0flag = 0;


/* ------------------------------------------------------------------------- */
/* CIA1581D0 */


    static iec_info_t *iec_info;

inline static void check_cia1581d0todalarm(CLOCK rclk)
{
    if (!memcmp(cia1581d0todalarm, cia1581d0 + CIA_TOD_TEN, sizeof(cia1581d0todalarm))) {
	cia1581d0int |= CIA_IM_TOD;
	if (cia1581d0[CIA_ICR] & CIA_IM_TOD) {
	    my_set_int(I_CIA1581D0TOD, IK_IRQ, rclk);
	}
    }
}

static int update_cia1581d0(CLOCK rclk)
{
    int tmp = 0;
    unsigned int ista = 0;
    BYTE sif = (cia1581d0int & cia1581d0ier & 0x7f);
    /* Tick when we virtually added an interrupt flag first. */
    CLOCK added_int_clk = (cia1581d0int & 0x80) ? rclk - 3 : CLOCK_MAX;

#ifdef CIA1581D0_TIMER_DEBUG
    if (cia1581d0_debugFlag)
	printf("CIA1581D0: update: rclk=%d, tas=%d, tau=%d, tal=%u, ",
	       rclk, cia1581d0_tas, cia1581d0_tau, cia1581d0_tal);
#endif

    if (cia1581d0_tas == CIAT_RUNNING) {
	if (rclk < cia1581d0_tau + 1) {
	    cia1581d0_tac = cia1581d0_tau - rclk;
	    tmp = 0;
	} else {
	    if (cia1581d0[CIA_CRA] & 0x08) {
		tmp = 1;
		if ((cia1581d0ier & CIA_IM_TA)
		    && (cia1581d0_tau < added_int_clk))
		    added_int_clk = cia1581d0_tau;
		cia1581d0_tau = 0;
		my_unset_tai();
		cia1581d0_tac = cia1581d0_tal;
		cia1581d0_tas = CIAT_STOPPED;
		cia1581d0[CIA_CRA] &= 0xfe;

		/* this is a HACK for arkanoid... */
		if (cia1581d0sr_bits) {
		    cia1581d0sr_bits--;
		    if(cia1581d0sr_bits==16) {
			
		    }
		    if (!cia1581d0sr_bits) {
			cia1581d0int |= CIA_IM_SDR;
			if ((cia1581d0ier & CIA_IM_SDR)
			    && (cia1581d0_tau < added_int_clk))
			    added_int_clk = cia1581d0_tau;
		    }
		}
	    } else {
		tmp = (rclk - cia1581d0_tau - 1) / (cia1581d0_tal + 1);
		cia1581d0_tau += tmp * (cia1581d0_tal + 1);
		if ((cia1581d0ier & CIA_IM_TA)
		    && (cia1581d0_tau < added_int_clk))
		    added_int_clk = cia1581d0_tau;
		cia1581d0_tau += 1 * (cia1581d0_tal + 1);
		cia1581d0_tac = cia1581d0_tau - rclk;
	    }

	    if (cia1581d0_tac == cia1581d0_tal)
		ista = 1;

            cia1581d0int |= CIA_IM_TA;
	}
    }
#ifdef CIA1581D0_TIMER_DEBUG
    if (cia1581d0_debugFlag)
	printf("aic=%d, tac-> %u, tau-> %d\n              tmp=%u, ", added_int_clk, cia1581d0_tac, cia1581d0_tau, tmp);
#endif

    if (cia1581d0[CIA_CRA] & 0x04) {
	cia1581d0_tap = cia1581d0_tat;
    } else {
	cia1581d0_tap = cia1581d0_tac ? 0 : 1;
    }

    cia1581d0_tbp = 0;
    if (cia1581d0_tbs == CIAT_RUNNING) {
	if (rclk < cia1581d0_tbu + 1) {
	    cia1581d0_tbc = cia1581d0_tbu - rclk;
	} else {
	    if (cia1581d0[CIA_CRB] & 0x08) {
		tmp = 1;
		if ((cia1581d0ier & CIA_IM_TB) && (cia1581d0_tbu < added_int_clk))
		    added_int_clk = cia1581d0_tbu;
		cia1581d0_tbu = 0;
		my_unset_tbi();
		cia1581d0_tbc = cia1581d0_tbl;
		cia1581d0_tbs = CIAT_STOPPED;
		cia1581d0[CIA_CRB] &= 0xfe;
	    } else {
		tmp = (rclk - cia1581d0_tbu - 1) / (cia1581d0_tbl + 1);
		cia1581d0_tbu += tmp * (cia1581d0_tbl + 1);
		if ((cia1581d0ier & CIA_IM_TB) && (cia1581d0_tbu < added_int_clk))
		    added_int_clk = cia1581d0_tbu;
		cia1581d0_tbu += 1 * (cia1581d0_tbl + 1);
		cia1581d0_tbc = cia1581d0_tbu - rclk;
	    }
	    if (!cia1581d0_tbc)
		cia1581d0_tbc = cia1581d0_tbl;
            cia1581d0int |= CIA_IM_TB;
	}
    } else if (cia1581d0_tbs == CIAT_COUNTTA) {
	/* missing: set added_int */
	if ((!cia1581d0_tbc) && ista) {
	    cia1581d0_tbp = 1;
	    cia1581d0_tbc = cia1581d0_tbl;
	    cia1581d0int |= CIA_IM_TB;
	}
    }
    if (cia1581d0[CIA_CRB] & 0x04) {
	cia1581d0_tbp ^= cia1581d0_tbt;
    } else {
	cia1581d0_tbp = cia1581d0_tbc ? 0 : 1;
    }

#ifdef CIA1581D0_TIMER_DEBUG
    if (cia1581d0_debugFlag)
	printf("tbc-> %u, tbu-> %d, int %02x ->",
	       cia1581d0_tbc, cia1581d0_tbu, cia1581d0int);
#endif

    /* have we changed the interrupt flags? */
    if (sif != (cia1581d0ier & cia1581d0int & 0x7f)) {
	/* if we do not read ICR, do standard operation */
	if (rclk != cia1581d0rdi) {
	    if (cia1581d0ier & cia1581d0int & 0x7f) {
		/* sets bit 7 */
		my_set_int(I_CIA1581D0FL, IK_IRQ, rclk);
	    }
	} else {
	    if (added_int_clk == cia1581d0rdi) {
		cia1581d0int &= 0x7f;
#ifdef CIA1581D0_TIMER_DEBUG
		if (cia1581d0_debugFlag)
		    printf("CIA1581D0: TA Reading ICR at rclk=%d prevented IRQ\n",
			   rclk);
#endif
	    } else {
		if (cia1581d0ier & cia1581d0int & 0x7f) {
		    /* sets bit 7 */
		    my_set_int(I_CIA1581D0FL, IK_IRQ, rclk);
		}
	    }
	}
    }
#ifdef CIA1581D0_TIMER_DEBUG
    if (cia1581d0_debugFlag)
	printf("%02x\n", cia1581d0int);
#endif

    /* return true sif interrupt line is set at this clock time */
    return (!sif) && (cia1581d0int & cia1581d0ier & 0x7f);
}

/* ------------------------------------------------------------------------- */

void reset_cia1581d0(void)
{
    int i;

    for (i = 0; i < 16; i++)
	cia1581d0[i] = 0;

    cia1581d0rdi = 0;
    cia1581d0sr_bits = 0;

    cia1581d0_tac = cia1581d0_tbc = 0xffff;
    cia1581d0_tal = cia1581d0_tbl = 0xffff;

    cia1581d0_tas = CIAT_STOPPED;
    cia1581d0_tbs = CIAT_STOPPED;
    cia1581d0_tat = 0;
    cia1581d0_tbt = 0;

    my_unset_tbi();
    my_unset_tai();

    memset(cia1581d0todalarm, 0, sizeof(cia1581d0todalarm));
    cia1581d0todlatched = 0;
    cia1581d0todstopped = 0;
    drive0_set_alarm(A_CIA1581D0TOD, cia1581d0todticks);

    cia1581d0int = 0;


    iec_info = iec_get_drive_port();
}


void REGPARM2 store_cia1581d0(ADDRESS addr, BYTE byte)
{
    CLOCK rclk;

    addr &= 0xf;

    

    rclk = clk - STORE_OFFSET;

#ifdef CIA1581D0_TIMER_DEBUG
    if (cia1581d0_debugFlag)
	printf("store cia1581d0[%02x] %02x @ clk=%d, pc=\n",
	       (int) addr, (int) byte, rclk);
#endif

    switch (addr) {

      case CIA_PRA:		/* port A */
      case CIA_DDRA:

    {
        BYTE oldval = cia1581d0[CIA_PRA] | ~cia1581d0[CIA_DDRA];

        cia1581d0[addr] = byte;
        byte = cia1581d0[CIA_PRA] | ~cia1581d0[CIA_DDRA];   /* newval */

        drive[0].led_status = byte & 0x40;
    }
	break;

      case CIA_PRB:		/* port B */
      case CIA_DDRB:
	if ((cia1581d0[CIA_CRA] | cia1581d0[CIA_CRB]) & 0x02) {
	    update_cia1581d0(rclk);
	    if (cia1581d0[CIA_CRA] & 0x02) {
		byte &= 0xbf;
		if (cia1581d0_tap)
		    byte |= 0x40;
	    }
	    if (cia1581d0[CIA_CRB] & 0x02) {
		byte &= 0x7f;
		if (cia1581d0_tbp)
		    byte |= 0x80;
	    }
	}

    if (byte != cia1581d0[addr]) {
        cia1581d0[addr] = byte;
        if (iec_info != NULL) {
            iec_info->drive_data = cia1581d0[CIA_DDRB] & ~cia1581d0[CIA_PRB];
            iec_info->drive_bus = (((iec_info->drive_data << 3) & 0x40)
                | ((iec_info->drive_data << 6)
                & ((~iec_info->drive_data ^ iec_info->cpu_bus) << 3) & 0x80));
            iec_info->cpu_port = iec_info->cpu_bus & iec_info->drive_bus
                & iec_info->drive2_bus;
            iec_info->drive_port = iec_info->drive2_port = (((iec_info->cpu_port >> 4) & 0x4)
                | (iec_info->cpu_port >> 7)
                | ((iec_info->cpu_bus << 3) & 0x80));
        } else {
            iec_drive_write(cia1581d0[CIA_DDRB] & ~cia1581d0[CIA_PRB]);
        }
    }
	break;

	/* This handles the timer latches.  The kludgy stuff is an attempt
           emulate the correct behavior when the latch is written to during
           an underflow.  */
      case CIA_TAL:
	update_tai(rclk); /* schedule alarm in case latch value is changed */
	update_cia1581d0(rclk - 1);
	if (cia1581d0_tac == cia1581d0_tal && cia1581d0_tas == CIAT_RUNNING) {
	    cia1581d0_tac = cia1581d0_tal = (cia1581d0_tal & 0xff00) | byte;
	    cia1581d0_tau = rclk + cia1581d0_tac;
	    update_cia1581d0(rclk);
	} else {
	    cia1581d0_tal = (cia1581d0_tal & 0xff00) | byte;
	}
	break;
      case CIA_TBL:
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
	update_cia1581d0(rclk - 1);
	if (cia1581d0_tbc == cia1581d0_tbl && cia1581d0_tbs == CIAT_RUNNING) {
	    cia1581d0_tbc = cia1581d0_tbl = (cia1581d0_tbl & 0xff00) | byte;
	    cia1581d0_tbu = rclk + cia1581d0_tbc + 1;
	    update_cia1581d0(rclk);
	} else {
	    cia1581d0_tbl = (cia1581d0_tbl & 0xff00) | byte;
	}
	break;
      case CIA_TAH:
	update_tai(rclk); /* schedule alarm in case latch value is changed */
	update_cia1581d0(rclk - 1);
	if (cia1581d0_tac == cia1581d0_tal && cia1581d0_tas == CIAT_RUNNING) {
	    cia1581d0_tac = cia1581d0_tal = (cia1581d0_tal & 0x00ff) | (byte << 8);
	    cia1581d0_tau = rclk + cia1581d0_tac;
	    update_cia1581d0(rclk);
	} else {
	    cia1581d0_tal = (cia1581d0_tal & 0x00ff) | (byte << 8);
	}
	if (cia1581d0_tas == CIAT_STOPPED)
	    cia1581d0_tac = cia1581d0_tal;
	break;
      case CIA_TBH:
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
	update_cia1581d0(rclk - 1);
	if (cia1581d0_tbc == cia1581d0_tbl && cia1581d0_tbs == CIAT_RUNNING) {
	    cia1581d0_tbc = cia1581d0_tbl = (cia1581d0_tbl & 0x00ff) | (byte << 8);
	    cia1581d0_tbu = rclk + cia1581d0_tbc + 1;
	    update_cia1581d0(rclk);
	} else {
	    cia1581d0_tbl = (cia1581d0_tbl & 0x00ff) | (byte << 8);
	}
	if (cia1581d0_tbs == CIAT_STOPPED)
	    cia1581d0_tbc = cia1581d0_tbl;
	break;

	/*
	 * TOD clock is stopped by writing Hours, and restarted
	 * upon writing Tenths of Seconds.
	 *
	 * REAL:  TOD register + (wallclock - cia1581d0todrel)
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
	if (cia1581d0[CIA_CRB] & 0x80)
	    cia1581d0todalarm[addr - CIA_TOD_TEN] = byte;
	else {
	    if (addr == CIA_TOD_TEN)
		cia1581d0todstopped = 0;
	    if (addr == CIA_TOD_HR)
		cia1581d0todstopped = 1;
	    cia1581d0[addr] = byte;
	}
	check_cia1581d0todalarm(rclk);
	break;

      case CIA_SDR:		/* Serial Port output buffer */
	cia1581d0[addr] = byte;
	if ((cia1581d0[CIA_CRA] & 0x40) == 0x40) {
	    if (cia1581d0sr_bits <= 16) {
		if(!cia1581d0sr_bits) {
    	            
		}
		if(cia1581d0sr_bits < 16) {
	            /* switch timer A alarm on again, if necessary */
	            update_cia1581d0(rclk);
	            if (cia1581d0_tau) {
		        my_set_tai_clk(cia1581d0_tau + 1);
	            }
		}

	        cia1581d0sr_bits += 16;

#if defined (CIA1581D0_TIMER_DEBUG)
	        if (cia1581d0_debugFlag)
	    	    printf("CIA1581D0: start SDR rclk=%d\n", rclk);
#endif
  	    }
	}
	break;

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Control Register */
	update_cia1581d0(rclk);

#if defined (CIA1581D0_TIMER_DEBUG)
	if (cia1581d0_debugFlag)
	    printf("CIA1581D0 set CIA_ICR: 0x%x\n", byte);
#endif

	if (byte & CIA_IM_SET) {
	    cia1581d0ier |= (byte & 0x7f);
	} else {
	    cia1581d0ier &= ~(byte & 0x7f);
	}

	/* This must actually be delayed one cycle! */
#if defined(CIA1581D0_TIMER_DEBUG)
	if (cia1581d0_debugFlag)
	    printf("    set icr: ifr & ier & 0x7f -> %02x, int=%02x\n",
		   cia1581d0ier & cia1581d0int & 0x7f, cia1581d0int);
#endif
	if (cia1581d0ier & cia1581d0int & 0x7f) {
	    my_set_int(I_CIA1581D0FL, IK_IRQ, rclk);
	}
	if (cia1581d0ier & (CIA_IM_TA + CIA_IM_TB)) {
	    if ((cia1581d0ier & CIA_IM_TA) && cia1581d0_tau) {
		my_set_tai_clk(cia1581d0_tau + 1);
	    }
	    if ((cia1581d0ier & CIA_IM_TB) && cia1581d0_tbu) {
		my_set_tbi_clk(cia1581d0_tbu + 1);
	    }
	}
	/* Control */
	break;

      case CIA_CRA:		/* control register A */
	update_tai(rclk); /* schedule alarm in case latch value is changed */
	update_cia1581d0(rclk);
#if defined (CIA1581D0_TIMER_DEBUG)
	if (cia1581d0_debugFlag)
	    printf("CIA1581D0 set CIA_CRA: 0x%x (clk=%d, pc=, tal=%u, tac=%u)\n",
		   byte, rclk, /*program_counter,*/ cia1581d0_tal, cia1581d0_tac);
#endif

	/* bit 7 tod frequency */
	/* bit 6 serial port mode */

	/* bit 4 force load */
	if (byte & 0x10) {
	    cia1581d0_tac = cia1581d0_tal;
	    if (cia1581d0_tas == CIAT_RUNNING) {
		cia1581d0_tau = rclk + cia1581d0_tac + 2;
		my_set_tai_clk(cia1581d0_tau + 1);
	    }
	}
	/* bit 3 timer run mode */
	/* bit 2 & 1 timer output to PB6 */

	/* bit 0 start/stop timer */
	/* bit 5 timer count mode */
	if ((byte & 1) && !(cia1581d0[CIA_CRA] & 1))
	    cia1581d0_tat = 1;
	if ((byte ^ cia1581d0[addr]) & 0x21) {
	    if ((byte & 0x21) == 0x01) {	/* timer just started */
		cia1581d0_tas = CIAT_RUNNING;
		cia1581d0_tau = rclk + (cia1581d0_tac + 1) + ((byte & 0x10) >> 4);
		my_set_tai_clk(cia1581d0_tau + 1);
	    } else {		/* timer just stopped */
		cia1581d0_tas = CIAT_STOPPED;
		cia1581d0_tau = 0;
		/* 1 cycle delay for counter stop. */
		if (!(byte & 0x10)) {
		    /* 1 cycle delay for counter stop.  This must only happen
                       if we are not forcing load at the same time (i.e. bit
                       4 in `byte' is zero). */
		    if (cia1581d0_tac > 0)
			cia1581d0_tac--;
		}
		my_unset_tai();
	    }
	}
#if defined (CIA1581D0_TIMER_DEBUG)
	if (cia1581d0_debugFlag)
	    printf("    -> tas=%d, tau=%d\n", cia1581d0_tas, cia1581d0_tau);
#endif
	cia1581d0[addr] = byte & 0xef;	/* remove strobe */

	break;

      case CIA_CRB:		/* control register B */
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
	update_cia1581d0(rclk);

#if defined (CIA1581D0_TIMER_DEBUG)
	if (cia1581d0_debugFlag)
	    printf("CIA1581D0 set CIA_CRB: 0x%x (clk=%d, pc=, tbl=%u, tbc=%u)\n",
		   byte, rclk, cia1581d0_tbl, cia1581d0_tbc);
#endif


	/* bit 7 set alarm/tod clock */
	/* bit 4 force load */
	if (byte & 0x10) {
	    cia1581d0_tbc = cia1581d0_tbl;
	    if (cia1581d0_tbs == CIAT_RUNNING) {
		cia1581d0_tbu = rclk + cia1581d0_tbc + 2;
#if defined(CIA1581D0_TIMER_DEBUG)
		if (cia1581d0_debugFlag)
		    printf("CIA1581D0: rclk=%d force load: set tbu alarm to %d\n", rclk, cia1581d0_tbu);
#endif
		my_set_tbi_clk(cia1581d0_tbu + 1);
	    }
	}
	/* bit 3 timer run mode */
	/* bit 2 & 1 timer output to PB6 */

	/* bit 0 stbrt/stop timer */
	/* bit 5 & 6 timer count mode */
	if ((byte & 1) && !(cia1581d0[CIA_CRB] & 1))
	    cia1581d0_tbt = 1;
	if ((byte ^ cia1581d0[addr]) & 0x61) {
	    if ((byte & 0x61) == 0x01) {	/* timer just started */
		cia1581d0_tbu = rclk + (cia1581d0_tbc + 1) + ((byte & 0x10) >> 4);
#if defined(CIA1581D0_TIMER_DEBUG)
		if (cia1581d0_debugFlag)
		    printf("CIA1581D0: rclk=%d start timer: set tbu alarm to %d\n", rclk, cia1581d0_tbu);
#endif
		my_set_tbi_clk(cia1581d0_tbu + 1);
		cia1581d0_tbs = CIAT_RUNNING;
	    } else {		/* timer just stopped */
#if defined(CIA1581D0_TIMER_DEBUG)
		if (cia1581d0_debugFlag)
		    printf("CIA1581D0: rclk=%d stop timer: set tbu alarm\n", rclk);
#endif
		my_unset_tbi();
		cia1581d0_tbu = 0;
		if (!(byte & 0x10)) {
		    /* 1 cycle delay for counter stop.  This must only happen
                       if we are not forcing load at the same time (i.e. bit
                       4 in `byte' is zero). */
		    if (cia1581d0_tbc > 0)
			cia1581d0_tbc--;
		}
		/* this should actually read (byte & 0x61), but as CNT is high
		   by default, bit 0x20 is a `don't care' bit */
		if ((byte & 0x41) == 0x41) {
		    cia1581d0_tbs = CIAT_COUNTTA;
		    update_cia1581d0(rclk);
		    /* switch timer A alarm on if necessary */
		    if (cia1581d0_tau) {
			my_set_tai_clk(cia1581d0_tau + 1);
		    }
		} else {
		    cia1581d0_tbs = CIAT_STOPPED;
		}
	    }
	}
	cia1581d0[addr] = byte & 0xef;	/* remove strobe */
	break;

      default:
	cia1581d0[addr] = byte;
    }				/* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_cia1581d0(ADDRESS addr)
{

#if defined( CIA1581D0_TIMER_DEBUG )

    BYTE read_cia1581d0_(ADDRESS addr);
    BYTE tmp = read_cia1581d0_(addr);

    if (cia1581d0_debugFlag)
	printf("read cia1581d0[%x] returns %02x @ clk=%d, pc=\n",
	       addr, tmp, clk - READ_OFFSET);
    return tmp;
}

BYTE read_cia1581d0_(ADDRESS addr)
{

#endif

    static BYTE byte;
    CLOCK rclk;

    addr &= 0xf;

    

    rclk = clk - READ_OFFSET;


    switch (addr) {

      case CIA_PRA:		/* port A */


	return byte;
	break;

      case CIA_PRB:		/* port B */

    if (iec_info != NULL)
        byte = ((cia1581d0[CIA_PRB] & 0x1a) | iec_info->drive_port) ^ 0x85;
    else
        byte = ((cia1581d0[CIA_PRB] & 0x1a) | iec_drive_read()) ^ 0x85;
        if ((cia1581d0[CIA_CRA] | cia1581d0[CIA_CRB]) & 0x02) {
	    update_cia1581d0(rclk);
	    if (cia1581d0[CIA_CRA] & 0x02) {
		byte &= 0xbf;
		if (cia1581d0_tap)
		    byte |= 0x40;
	    }
	    if (cia1581d0[CIA_CRB] & 0x02) {
		byte &= 0x7f;
		if (cia1581d0_tbp)
		    byte |= 0x80;
	    }
	}

	return byte;
	break;

	/* Timers */
      case CIA_TAL:		/* timer A low */
	update_cia1581d0(rclk);
	return ((cia1581d0_tac ? cia1581d0_tac : cia1581d0_tal) & 0xff);

      case CIA_TAH:		/* timer A high */
	update_cia1581d0(rclk);
	return ((cia1581d0_tac ? cia1581d0_tac : cia1581d0_tal) >> 8) & 0xff;

      case CIA_TBL:		/* timer B low */
	update_cia1581d0(rclk);
	return cia1581d0_tbc & 0xff;

      case CIA_TBH:		/* timer B high */
	update_cia1581d0(rclk);
	return (cia1581d0_tbc >> 8) & 0xff;

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
	if (!cia1581d0todlatched)
	    memcpy(cia1581d0todlatch, cia1581d0 + CIA_TOD_TEN, sizeof(cia1581d0todlatch));
	if (addr == CIA_TOD_TEN)
	    cia1581d0todlatched = 0;
	if (addr == CIA_TOD_HR)
	    cia1581d0todlatched = 1;
	return cia1581d0[addr];

      case CIA_SDR:		/* Serial Port Shift Register */
#if 0				/*def DEBUG */
	cia1581d0_dump(stdout);
	/* little hack .... */
	{
	    int i;
	    printf("\ndrive0_ints:");
	    for (i = 0; i < NUMOFINT; i++) {
		printf(" %d", drive0_int_status.pending_int[i]);
	    }
	    printf("\n");
	}
#endif
	return (cia1581d0[addr]);

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Flag Register */
	{
	    BYTE t = 0;



#ifdef CIA1581D0_TIMER_DEBUG
	    if (cia1581d0_debugFlag)
		printf("CIA1581D0 read intfl: rclk=%d, alarm_ta=%d, alarm_tb=%d\n",
			rclk, drive0_int_status.alarm_clk[A_CIA1581D0TA],
			drive0_int_status.alarm_clk[A_CIA1581D0TB]);
#endif
	
	    cia1581d0rdi = rclk;
            t = cia1581d0int;	/* we clean cia1581d0int anyway, so make int_* */
	    cia1581d0int = 0;	/* believe it is already */

            if (rclk >= cia1581d0_tai)
                int_cia1581d0ta(rclk - cia1581d0_tai);
            if (rclk >= cia1581d0_tbi)
                int_cia1581d0tb(rclk - cia1581d0_tbi);

	    cia1581d0int |= t;	/* some bits can be set -> or with old value */

	    update_cia1581d0(rclk);
	    t = cia1581d0int | cia1581d0flag;

#ifdef CIA1581D0_TIMER_DEBUG
	    if (cia1581d0_debugFlag)
		printf("CIA1581D0 read intfl gives cia1581d0int=%02x -> %02x @"
		       " PC=, sr_bits=%d, clk=%d, ta=%d, tb=%d\n",
		       cia1581d0int, t, cia1581d0sr_bits, clk, 
			(cia1581d0_tac ? cia1581d0_tac : cia1581d0_tal),
			cia1581d0_tbc);
#endif

	    cia1581d0flag = 0;
	    cia1581d0int = 0;
	    my_set_int(I_CIA1581D0FL, 0, rclk);

	    return (t);
	}
    }				/* switch */

    return (cia1581d0[addr]);
}

BYTE REGPARM1 peek_cia1581d0(ADDRESS addr)
{
    /* This code assumes that update_cia1581d0 is a projector - called at
     * the same cycle again it doesn't change anything. This way
     * it does not matter if we call it from peek first in the monitor
     * and probably the same cycle again when the CPU runs on...
     */
    CLOCK rclk;

    addr &= 0xf;

    

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
	if (!cia1581d0todlatched)
	    memcpy(cia1581d0todlatch, cia1581d0 + CIA_TOD_TEN, sizeof(cia1581d0todlatch));
	return cia1581d0[addr];

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Flag Register */
	{
	    BYTE t = 0;



#ifdef CIA1581D0_TIMER_DEBUG
	    if (cia1581d0_debugFlag)
		printf("CIA1581D0 read intfl: rclk=%d, alarm_ta=%d, alarm_tb=%d\n",
			rclk, drive0_int_status.alarm_clk[A_CIA1581D0TA],
			drive0_int_status.alarm_clk[A_CIA1581D0TB]);
#endif
	
	    cia1581d0rdi = rclk;
            t = cia1581d0int;	/* we clean cia1581d0int anyway, so make int_* */
	    cia1581d0int = 0;	/* believe it is already */

            if (rclk >= cia1581d0_tai)
                int_cia1581d0ta(rclk - cia1581d0_tai);
            if (rclk >= cia1581d0_tbi)
                int_cia1581d0tb(rclk - cia1581d0_tbi);

	    cia1581d0int |= t;	/* some bits can be set -> or with old value */

	    update_cia1581d0(rclk);
	    t = cia1581d0int | cia1581d0flag;

#ifdef CIA1581D0_TIMER_DEBUG
	    if (cia1581d0_debugFlag)
		printf("CIA1581D0 read intfl gives cia1581d0int=%02x -> %02x @"
		       " PC=, sr_bits=%d, clk=%d, ta=%d, tb=%d\n",
		       cia1581d0int, t, cia1581d0sr_bits, clk, 
			(cia1581d0_tac ? cia1581d0_tac : cia1581d0_tal),
			cia1581d0_tbc);
#endif

/*
	    cia1581d0flag = 0;
	    cia1581d0int = 0;
	    my_set_int(I_CIA1581D0FL, 0, rclk);
*/
	    return (t);
	}
      default:
	break;
    }				/* switch */

    return read_cia1581d0(addr);
}

/* ------------------------------------------------------------------------- */

int int_cia1581d0ta(long offset)
{
    CLOCK rclk = clk - offset;

#if defined(CIA1581D0_TIMER_DEBUG)
    if (cia1581d0_debugFlag)
	printf("CIA1581D0: int_cia1581d0ta(rclk = %u, tal = %u, cra=%02x\n",
	       rclk, cia1581d0_tal, cia1581d0[CIA_CRA]);
#endif

    cia1581d0_tat = (cia1581d0_tat + 1) & 1;

    if ((cia1581d0_tas == CIAT_RUNNING) && !(cia1581d0[CIA_CRA] & 8)) {
	/* if we do not need alarm, no PB6, no shift register, and not timer B
	   counting timer A, then we can savely skip alarms... */
	if ( ( (cia1581d0ier & CIA_IM_TA) && 
		(!(cia1581d0int & 0x80)) )
	    || (cia1581d0[CIA_CRA] & 0x42)
	    || (cia1581d0_tbs == CIAT_COUNTTA)) {
	    if(offset > cia1581d0_tal+1) {
	        my_set_tai_clk(
			clk - (offset % (cia1581d0_tal+1)) + cia1581d0_tal + 1 );
	    } else {
	        my_set_tai_clk(rclk + cia1581d0_tal + 1 );
	    }
	} else {
	    /* cia1581d0_tai = rclk + cia1581d0_tal +1; - now keeps tai */
	    /* printf("cia1581d0 unset alarm: clk=%d, rclk=%d, rdi=%d -> tai=%d\n",
			clk, rclk, cia1581d0rdi, cia1581d0_tai); */
	    drive0_unset_alarm(A_CIA1581D0TA);	/* do _not_ clear cia1581d0_tai */
	}
    } else {
#if 0
	cia1581d0_tas = CIAT_STOPPED;
	cia1581d0[CIA_CRA] &= 0xfe;	/* clear run flag. Correct? */
	cia1581d0_tau = 0;
#endif
	my_unset_tai();
    }

    if (cia1581d0[CIA_CRA] & 0x40) {
	if (cia1581d0sr_bits) {
#if defined(CIA1581D0_TIMER_DEBUG)
	    if (cia1581d0_debugFlag)
		printf("CIA1581D0: rclk=%d SDR: timer A underflow, bits=%d\n",
		       rclk, cia1581d0sr_bits);
#endif
	    if (!(--cia1581d0sr_bits)) {
		cia1581d0int |= CIA_IM_SDR;
	    }
	    if(cia1581d0sr_bits == 16) {
		
	    }
	}
    }
    if (cia1581d0_tbs == CIAT_COUNTTA) {
	if (!cia1581d0_tbc) {
	    cia1581d0_tbc = cia1581d0_tbl;
	    cia1581d0_tbu = rclk;
#if defined(CIA1581D0_TIMER_DEBUG)
	    if (cia1581d0_debugFlag)
		printf("CIA1581D0: timer B underflow when counting timer A occured, rclk=%d!\n", rclk);
#endif
	    cia1581d0int |= CIA_IM_TB;
	    my_set_tbi_clk(rclk);
	} else {
	    cia1581d0_tbc--;
	}
    }

    /* CIA_IM_TA is not set here, as it can be set in update(), reset
       by reading the ICR and then set again here because of delayed
       calling of int() */
    if ((IK_IRQ == IK_NMI && cia1581d0rdi != rclk - 1)
        || (IK_IRQ == IK_IRQ && cia1581d0rdi < rclk - 1)) {
        if ((cia1581d0int | CIA_IM_TA) & cia1581d0ier & 0x7f) {
            my_set_int(I_CIA1581D0FL, IK_IRQ, rclk);
        }
    }

    return 0;
}


/*
 * Timer B can run in 2 (4) modes
 * cia1581d0[f] & 0x60 == 0x00   count system 02 pulses
 * cia1581d0[f] & 0x60 == 0x40   count timer A underflows
 * cia1581d0[f] & 0x60 == 0x20 | 0x60 count CNT pulses => counter stops
 */


int int_cia1581d0tb(long offset)
{
    CLOCK rclk = clk - offset;

#if defined(CIA1581D0_TIMER_DEBUG)
    if (cia1581d0_debugFlag)
	printf("CIA1581D0: timer B int_cia1581d0tb(rclk=%d, tbs=%d)\n", rclk, cia1581d0_tbs);
#endif

    cia1581d0_tbt = (cia1581d0_tbt + 1) & 1;

    /* running and continous, then next alarm */
    if (cia1581d0_tbs == CIAT_RUNNING) {
	if (!(cia1581d0[CIA_CRB] & 8)) {
#if defined(CIA1581D0_TIMER_DEBUG)
	    if (cia1581d0_debugFlag)
		printf("CIA1581D0: rclk=%d cia1581d0tb: set tbu alarm to %d\n", rclk, rclk + cia1581d0_tbl + 1);
#endif
	    /* if no interrupt flag we can safely skip alarms */
	    if (cia1581d0ier & CIA_IM_TB) {
		if(offset > cia1581d0_tbl+1) {
		    my_set_tbi_clk( 
			clk - (offset % (cia1581d0_tbl+1)) + cia1581d0_tbl + 1);
		} else {
		    my_set_tbi_clk(rclk + cia1581d0_tbl + 1);
		}
	    } else {
		/* cia1581d0_tbi = rclk + cia1581d0_tbl + 1; */
		drive0_unset_alarm(A_CIA1581D0TB);
	    }
	} else {
#if 0
	    cia1581d0_tbs = CIAT_STOPPED;
	    cia1581d0[CIA_CRB] &= 0xfe; /* clear start bit */
	    cia1581d0_tbu = 0;
#endif /* 0 */
#if defined(CIA1581D0_TIMER_DEBUG)
	    if (cia1581d0_debugFlag)
		printf("CIA1581D0: rclk=%d cia1581d0tb: unset tbu alarm\n", rclk);
#endif
	    my_unset_tbi();
	}
    } else {
	if (cia1581d0_tbs == CIAT_COUNTTA) {
	    if ((cia1581d0[CIA_CRB] & 8)) {
		cia1581d0_tbs = CIAT_STOPPED;
		cia1581d0[CIA_CRB] &= 0xfe;		/* clear start bit */
		cia1581d0_tbu = 0;
	    }
	}
	cia1581d0_tbu = 0;
	my_unset_tbi();
#if defined(CIA1581D0_TIMER_DEBUG)
	if (cia1581d0_debugFlag)
	    printf("CIA1581D0: rclk=%d cia1581d0tb: unset tbu alarm\n", rclk);
#endif
    }

    if ((IK_IRQ == IK_NMI && cia1581d0rdi != rclk - 1)
        || (IK_IRQ == IK_IRQ && cia1581d0rdi < rclk - 1)) {
        if ((cia1581d0int | CIA_IM_TB) & cia1581d0ier & 0x7f) {
            my_set_int(I_CIA1581D0FL, IK_IRQ, rclk);
        }
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

void cia1581d0_set_flag(void)
{
    cia1581d0int |= CIA_IM_FLG;
    if (cia1581d0[CIA_ICR] & CIA_IM_FLG) {
        my_set_int(I_CIA1581D0FL, IK_IRQ, clk);
    }
}

void cia1581d0_set_sdr(BYTE data)
{
    cia1581d0[CIA_SDR] = data;
    cia1581d0int |= CIA_IM_SDR;
    if (cia1581d0[CIA_ICR] & CIA_IM_SDR) {
        my_set_int(I_CIA1581D0FL, IK_IRQ, clk);
    }
}

/* ------------------------------------------------------------------------- */

int int_cia1581d0tod(long offset)
{
    int t, pm;
    CLOCK rclk = clk - offset;

#ifdef DEBUG
    if (cia1581d0_debugFlag)
	printf("CIA1581D0: TOD timer event (1/10 sec tick), tod=%02x:%02x,%02x.%x\n",
	       cia1581d0[CIA_TOD_HR], cia1581d0[CIA_TOD_MIN], cia1581d0[CIA_TOD_SEC],
	       cia1581d0[CIA_TOD_TEN]);
#endif

    /* set up new int */
    drive0_set_alarm(A_CIA1581D0TOD, cia1581d0todticks);

    if (!cia1581d0todstopped) {
	/* inc timer */
	t = bcd2byte(cia1581d0[CIA_TOD_TEN]);
	t++;
	cia1581d0[CIA_TOD_TEN] = byte2bcd(t % 10);
	if (t >= 10) {
	    t = bcd2byte(cia1581d0[CIA_TOD_SEC]);
	    t++;
	    cia1581d0[CIA_TOD_SEC] = byte2bcd(t % 60);
	    if (t >= 60) {
		t = bcd2byte(cia1581d0[CIA_TOD_MIN]);
		t++;
		cia1581d0[CIA_TOD_MIN] = byte2bcd(t % 60);
		if (t >= 60) {
		    pm = cia1581d0[CIA_TOD_HR] & 0x80;
		    t = bcd2byte(cia1581d0[CIA_TOD_HR] & 0x1f);
		    if (!t)
			pm ^= 0x80;	/* toggle am/pm on 0:59->1:00 hr */
		    t++;
		    t = t % 12 | pm;
		    cia1581d0[CIA_TOD_HR] = byte2bcd(t);
		}
	    }
	}
#ifdef DEBUG
	if (cia1581d0_debugFlag)
	    printf("CIA1581D0: TOD after event :tod=%02x:%02x,%02x.%x\n",
	       cia1581d0[CIA_TOD_HR], cia1581d0[CIA_TOD_MIN], cia1581d0[CIA_TOD_SEC],
		   cia1581d0[CIA_TOD_TEN]);
#endif
	/* check alarm */
	check_cia1581d0todalarm(rclk);
    }
    return 0;
}

/* -------------------------------------------------------------------------- */


void cia1581d0_prevent_clk_overflow(CLOCK sub)
{

    update_tai(clk);
    update_tbi(clk);

    update_cia1581d0(clk);

    if(cia1581d0_tai && (cia1581d0_tai != -1))
        cia1581d0_tai -= sub;
    if(cia1581d0_tbi && (cia1581d0_tbi != -1))
        cia1581d0_tbi -= sub;

    if (cia1581d0_tau)
	cia1581d0_tau -= sub;
    if (cia1581d0_tbu)
	cia1581d0_tbu -= sub;
    if (cia1581d0rdi > sub)
	cia1581d0rdi -= sub;
    else
	cia1581d0rdi = 0;
}

#if 0

void cia1581d0_dump(FILE * fp)
{

    update_cia1581d0(clk);
    fprintf(fp, "[CIA1581D0]\n");
    fprintf(fp, "PA %d %d\n", cia1581d0[CIA_PRA], cia1581d0[CIA_DDRA]);
    fprintf(fp, "PB %d %d\n", cia1581d0[CIA_PRB], cia1581d0[CIA_DDRB]);
    fprintf(fp, "TA %u %u %d [$%02x = ", cia1581d0_tac, cia1581d0_tal,
	    cia1581d0[CIA_CRA], cia1581d0[CIA_CRA]);
    fprintf(fp, "%s%s%s%s%s]\n",
	    (cia1581d0[CIA_CRA] & 1) ? "running " : "stopped ",
	    (cia1581d0[CIA_CRA] & 8) ? "one-shot " : "continous ",
	    (cia1581d0[CIA_CRA] & 16) ? "force-load " : "",
	    (cia1581d0[CIA_CRA] & 32) ? "cnt " : "phi2 ",
	    (cia1581d0[CIA_CRA] & 64) ? "sr_out " : "sr_in ");

    fprintf(fp, "TB %u %u %d [$%02x = ", cia1581d0_tbc, cia1581d0_tbl,
	    cia1581d0[CIA_CRB], cia1581d0[CIA_CRB]);
    fprintf(fp, "%s%s%s%s]\n",
	    (cia1581d0[CIA_CRB] & 1) ? "running " : "stopped ",
	    (cia1581d0[CIA_CRB] & 8) ? "one-shot " : "continous ",
	    (cia1581d0[CIA_CRB] & 16) ? "force-load " : "",
	    (cia1581d0[CIA_CRB] & 32)
	    ? ((cia1581d0[CIA_CRB] & 64) ? "timerA+cnt" : "cnt ")
	    : ((cia1581d0[CIA_CRB] & 64) ? "timerA" : "phi2 ")
	);

    fprintf(fp, "ICR %u %u %u", cia1581d0int, cia1581d0ier,
	    drive0_int_status.pending_int[I_CIA1581D0FL]);
    fprintf(fp, " [fl= %s%s%s%s%s] ",
	    (cia1581d0int & 1) ? "TA " : "",
	    (cia1581d0int & 2) ? "TB " : "",
	    (cia1581d0int & 4) ? "Alarm " : "",
	    (cia1581d0int & 8) ? "SDR " : "",
	    (cia1581d0int & 16) ? "Flag " : "");
    fprintf(fp, "[mask= %s%s%s%s%s]\n",
	    (cia1581d0ier & 1) ? "TA " : "",
	    (cia1581d0ier & 2) ? "TB " : "",
	    (cia1581d0ier & 4) ? "Alarm " : "",
	    (cia1581d0ier & 8) ? "SDR " : "",
	    (cia1581d0ier & 16) ? "Flag " : "");

    fprintf(fp, "SR %d %d\n", cia1581d0[CIA_SDR], cia1581d0sr_bits);
    fprintf(fp, "TOD %d %d %d %d\n", cia1581d0[CIA_TOD_HR],
	    cia1581d0[CIA_TOD_MIN], cia1581d0[CIA_TOD_SEC], cia1581d0[CIA_TOD_SEC]);
}


void cia1581d0_undump_line(char *s)
{
    unsigned int d1, d2, d3, d4;

    if (s == strstr(s, "PA")) {
	sscanf(s + 2, "%u %u", &d1, &d2);
	cia1581d0[CIA_PRA] = d1;
	cia1581d0[CIA_DDRA] = d2;
	store_cia1581d0(CIA_PRA, cia1581d0[CIA_PRA]);
    } else if (s == strstr(s, "PB")) {
	sscanf(s + 2, "%u %u", &d1, &d2);
	cia1581d0[CIA_PRB] = d1;
	cia1581d0[CIA_DDRB] = d2;
	store_cia1581d0(CIA_PRB, cia1581d0[CIA_PRB]);
    } else if (s == strstr(s, "TA")) {
	sscanf(s + 2, "%u %u %u", &cia1581d0_tac, &cia1581d0_tal, &d1);
	cia1581d0[CIA_CRA] = d1;
	if ((cia1581d0[CIA_CRA] & 0x21) == 0x01) {
	    cia1581d0_tau = clk + cia1581d0_tac;
	    cia1581d0_tas = CIAT_RUNNING;
	    my_set_tai_clk(cia1581d0_tau + 1);
	} else {
	    cia1581d0_tau = 0;
	    cia1581d0_tas = CIAT_STOPPED;
	}
    } else if (s == strstr(s, "TB")) {
	sscanf(s + 2, "%u %u %u", &cia1581d0_tbc, &cia1581d0_tbl, &d1);
	cia1581d0[CIA_CRB] = d1;
	if ((cia1581d0[CIA_CRB] & 0x61) == 0x01) {
	    cia1581d0_tbu = clk + cia1581d0_tbc;
	    cia1581d0_tbs = CIAT_RUNNING;
	    my_set_tbi_clk(cia1581d0_tbu + 1);
	} else {
	    cia1581d0_tbu = 0;
	    if ((cia1581d0[CIA_CRB] & 0x61) == 0x41) {
		cia1581d0_tbs = CIAT_COUNTTA;
	    } else {
		cia1581d0_tbs = CIAT_STOPPED;
	    }
	}
    } else if (s == strstr(s, "ICR")) {
	sscanf(s + 3, "%d %d", &d1, &d2);
	cia1581d0int = d1;
	cia1581d0ier = d2;
	if (cia1581d0int & cia1581d0ier & 0x7f) {
	    my_set_int(I_CIA1581D0FL, IK_IRQ, rclk);
	} else {
	    my_set_int(I_CIA1581D0FL, 0, rclk);
	}
    } else if (s == strstr(s, "SR")) {
	sscanf(s + 2, "%d %d", &d1, &cia1581d0sr_bits);
	cia1581d0[CIA_SDR] = d1;
    } else if (s == strstr(s, "TOD")) {
	sscanf(s + 3, "%u %u %u %u", &d1, &d2, &d3, &d4);
	cia1581d0[CIA_TOD_HR] = d1;
	cia1581d0[CIA_TOD_MIN] = d2;
	cia1581d0[CIA_TOD_SEC] = d3;
	cia1581d0[CIA_TOD_TEN] = d4;
    } else {
	printf("unknown dump format line for CIA1581D0: \n%s\n", s);
    }
}

#endif




