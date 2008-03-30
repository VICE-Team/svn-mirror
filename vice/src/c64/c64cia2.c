
/*
 * ../../../src/c64/c64cia2.c
 * This file is generated from ../../../src/cia-tmpl.c and ../../../src/c64/c64cia2.def,
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
 * There now is a new function, update_cia2(). It computes all differences
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
 * may be set a few cycles too late due to late call of int_cia2t*() due to
 * opcode execution time. This can be fixed by checking in the beginning of
 * read_* and write_* if an int_* is scheduled and executing it before.  Then
 * the setting of the ICR could also be moved from update to int_*().  But
 * the bug only affects the contents of the ICR. The interrupt is generated
 * at the right time (hopefully).
 *
 * There is one HACK to make a game work: in update_cia2() a fix is done for
 * Arkanoid. This game counts shift register bits (i.e. TA underflows) by
 * setting TA to one-shot.  The ICR is read just before the int_cia2ta()
 * function is called, and the int bit is missed, so there is a check in
 * update_cia2() (this is probably a fix and not a hack... :-)
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

#include "vmachine.h"
#include "cia.h"
#include "vicii.h"
#include "maincpu.h"
#include "resources.h"


    #include "true1541.h"
    #include "c64mem.h"
    #include "c64iec.h"
    #include "c64cia.h"
    #include "pruser.h"
#ifdef HAVE_RS232
    #include "rsuser.h"
#endif

#include "interrupt.h"

#undef CIA2_TIMER_DEBUG
#undef CIA2_IO_DEBUG

#define	STORE_OFFSET 0
#define	READ_OFFSET 0

#define	CIAT_STOPPED	0
#define	CIAT_RUNNING	1
#define	CIAT_COUNTTA	2

#ifdef CIA2_TIMER_DEBUG
#define	my_set_int(int_num, value, rclk)				\
    do {								\
        if (cia2_debugFlag)					\
	    printf("set_int(rclk=%d, int=%d, d=%d pc=)\n",		\
		   rclk,(int_num),(value));			\
	maincpu_set_nmi_clk((int_num), (value), (rclk));			\
	if ((value))							\
	    cia2int |= 0x80;						\
    } while(0)
#else
#define	my_set_int(int_num, value, rclk)				 \
    do {								 \
        maincpu_set_nmi_clk((int_num), (value), (rclk));			 \
	if ((value))							 \
	    cia2int |= 0x80;						 \
    } while(0)
#endif

/* 
 * scheduling int_cia2t[ab] calls - 
 * warning: int_cia2ta uses maincpu_* stuff! 
 */

#define	my_set_tai_clk(clk) 						\
    do {								\
	cia2_tai = clk;						\
	maincpu_set_alarm_clk(A_CIA2TA, clk);				\
    } while(0)

#define	my_unset_tai() 							\
    do {								\
	cia2_tai = -1;							\
	maincpu_unset_alarm(A_CIA2TA);					\
    } while(0)

#define	my_set_tbi_clk(clk) 						\
    do {								\
	cia2_tbi = clk;						\
	maincpu_set_alarm_clk(A_CIA2TB, clk);				\
    } while(0)

#define	my_unset_tbi() 							\
    do {								\
	cia2_tbi = -1;							\
	maincpu_unset_alarm(A_CIA2TB);					\
    } while(0)

/*
 * Those routines setup the cia2t[ab]i clocks to a value above
 * rclk and schedule the next int_cia2t[ab] alarm
 */
#define	update_tai(rclk)							\
    do {								\
	if(cia2_tai < rclk) {						\
	    int t = cia2int;						\
	    cia2int = 0;						\
	    int_cia2ta(rclk - cia2_tai);				\
	    cia2int |= t;						\
	}								\
    } while(0)

#define	update_tbi(rclk)							\
    do {								\
	if(cia2_tbi < rclk) {						\
	    int t = cia2int;						\
	    cia2int = 0;						\
	    int_cia2tb(rclk - cia2_tbi);				\
	    cia2int |= t;						\
	}								\
    } while(0)

/* global */

static BYTE cia2[16];

#if defined(CIA2_TIMER_DEBUG) || defined(CIA2_IO_DEBUG)
int cia2_debugFlag = 0;

#endif

/* local functions */

static int update_cia2(CLOCK rclk);
static void check_cia2todalarm(CLOCK rclk);

void cia2_dump(FILE * fp);

/*
 * Local variables
 */

#define	cia2ier	cia2[CIA_ICR]
static int cia2int;		/* Interrupt Flag register for cia 1 */

static CLOCK cia2rdi;		/* real clock = clk-offset */

static CLOCK cia2_tau;		/* when is the next underflow? */
static CLOCK cia2_tai;		/* when is the next int_* scheduled? */
static unsigned int cia2_tal;	/* latch value */
static unsigned int cia2_tac;	/* counter value */
static unsigned int cia2_tat;	/* timer A toggle bit */
static unsigned int cia2_tap;	/* timer A port bit */
static int cia2_tas;		/* timer state (CIAT_*) */

static CLOCK cia2_tbu;		/* when is the next underflow? */
static CLOCK cia2_tbi;		/* when is the next int_* scheduled? */
static unsigned int cia2_tbl;	/* latch value */
static unsigned int cia2_tbc;	/* counter value */
static unsigned int cia2_tbt;	/* timer B toggle bit */
static unsigned int cia2_tbp;	/* timer B port bit */
static int cia2_tbs;		/* timer state (CIAT_*) */

static int cia2sr_bits;	/* number of bits still to send * 2 */

static BYTE cia2todalarm[4];
static BYTE cia2todlatch[4];
static char cia2todstopped;
static char cia2todlatched;
static int cia2todticks = 100000;	/* approx. a 1/10 sec. */

static BYTE cia2flag = 0;


/* ------------------------------------------------------------------------- */
/* CIA2 */


    static iec_info_t *iec_info;

inline static void check_cia2todalarm(CLOCK rclk)
{
    if (!memcmp(cia2todalarm, cia2 + CIA_TOD_TEN, sizeof(cia2todalarm))) {
	cia2int |= CIA_IM_TOD;
	if (cia2[CIA_ICR] & CIA_IM_TOD) {
	    my_set_int(I_CIA2TOD, IK_NMI, rclk);
	}
    }
}

static int update_cia2(CLOCK rclk)
{
    int tmp = 0;
    unsigned int ista = 0;
    BYTE sif = (cia2int & cia2ier & 0x7f);
    /* Tick when we virtually added an interrupt flag first. */
    CLOCK added_int_clk = (cia2int & 0x80) ? rclk - 3 : CLOCK_MAX;

#ifdef CIA2_TIMER_DEBUG
    if (cia2_debugFlag)
	printf("CIA2: update: rclk=%d, tas=%d, tau=%d, tal=%u, ",
	       rclk, cia2_tas, cia2_tau, cia2_tal);
#endif

    if (cia2_tas == CIAT_RUNNING) {
	if (rclk < cia2_tau + 1) {
	    cia2_tac = cia2_tau - rclk;
	    tmp = 0;
	} else {
	    if (cia2[CIA_CRA] & 0x08) {
		tmp = 1;
		if ((cia2ier & CIA_IM_TA)
		    && (cia2_tau < added_int_clk))
		    added_int_clk = cia2_tau;
		cia2_tau = 0;
		my_unset_tai();
		cia2_tac = cia2_tal;
		cia2_tas = CIAT_STOPPED;
		cia2[CIA_CRA] &= 0xfe;

		/* this is a HACK for arkanoid... */
		if (cia2sr_bits) {
		    cia2sr_bits--;
		    if(cia2sr_bits==16) {
			
		    }
		    if (!cia2sr_bits) {
			cia2int |= CIA_IM_SDR;
			if ((cia2ier & CIA_IM_SDR)
			    && (cia2_tau < added_int_clk))
			    added_int_clk = cia2_tau;
		    }
		}
	    } else {
		tmp = (rclk - cia2_tau - 1) / (cia2_tal + 1);
		cia2_tau += tmp * (cia2_tal + 1);
		if ((cia2ier & CIA_IM_TA)
		    && (cia2_tau < added_int_clk))
		    added_int_clk = cia2_tau;
		cia2_tau += 1 * (cia2_tal + 1);
		cia2_tac = cia2_tau - rclk;
	    }

	    if (cia2_tac == cia2_tal)
		ista = 1;

            cia2int |= CIA_IM_TA;
	}
    }
#ifdef CIA2_TIMER_DEBUG
    if (cia2_debugFlag)
	printf("aic=%d, tac-> %u, tau-> %d\n              tmp=%u, ", added_int_clk, cia2_tac, cia2_tau, tmp);
#endif

    if (cia2[CIA_CRA] & 0x04) {
	cia2_tap = cia2_tat;
    } else {
	cia2_tap = cia2_tac ? 0 : 1;
    }

    cia2_tbp = 0;
    if (cia2_tbs == CIAT_RUNNING) {
	if (rclk < cia2_tbu + 1) {
	    cia2_tbc = cia2_tbu - rclk;
	} else {
	    if (cia2[CIA_CRB] & 0x08) {
		tmp = 1;
		if ((cia2ier & CIA_IM_TB) && (cia2_tbu < added_int_clk))
		    added_int_clk = cia2_tbu;
		cia2_tbu = 0;
		my_unset_tbi();
		cia2_tbc = cia2_tbl;
		cia2_tbs = CIAT_STOPPED;
		cia2[CIA_CRB] &= 0xfe;
	    } else {
		tmp = (rclk - cia2_tbu - 1) / (cia2_tbl + 1);
		cia2_tbu += tmp * (cia2_tbl + 1);
		if ((cia2ier & CIA_IM_TB) && (cia2_tbu < added_int_clk))
		    added_int_clk = cia2_tbu;
		cia2_tbu += 1 * (cia2_tbl + 1);
		cia2_tbc = cia2_tbu - rclk;
	    }
	    if (!cia2_tbc)
		cia2_tbc = cia2_tbl;
            cia2int |= CIA_IM_TB;
	}
    } else if (cia2_tbs == CIAT_COUNTTA) {
	/* missing: set added_int */
	if ((!cia2_tbc) && ista) {
	    cia2_tbp = 1;
	    cia2_tbc = cia2_tbl;
	    cia2int |= CIA_IM_TB;
	}
    }
    if (cia2[CIA_CRB] & 0x04) {
	cia2_tbp ^= cia2_tbt;
    } else {
	cia2_tbp = cia2_tbc ? 0 : 1;
    }

#ifdef CIA2_TIMER_DEBUG
    if (cia2_debugFlag)
	printf("tbc-> %u, tbu-> %d, int %02x ->",
	       cia2_tbc, cia2_tbu, cia2int);
#endif

    /* have we changed the interrupt flags? */
    if (sif != (cia2ier & cia2int & 0x7f)) {
	/* if we do not read ICR, do standard operation */
	if (rclk != cia2rdi) {
	    if (cia2ier & cia2int & 0x7f) {
		/* sets bit 7 */
		my_set_int(I_CIA2FL, IK_NMI, rclk);
	    }
	} else {
	    if (added_int_clk == cia2rdi) {
		cia2int &= 0x7f;
#ifdef CIA2_TIMER_DEBUG
		if (cia2_debugFlag)
		    printf("CIA2: TA Reading ICR at rclk=%d prevented IRQ\n",
			   rclk);
#endif
	    } else {
		if (cia2ier & cia2int & 0x7f) {
		    /* sets bit 7 */
		    my_set_int(I_CIA2FL, IK_NMI, rclk);
		}
	    }
	}
    }
#ifdef CIA2_TIMER_DEBUG
    if (cia2_debugFlag)
	printf("%02x\n", cia2int);
#endif

    /* return true sif interrupt line is set at this clock time */
    return (!sif) && (cia2int & cia2ier & 0x7f);
}

/* ------------------------------------------------------------------------- */

void reset_cia2(void)
{
    int i;

    for (i = 0; i < 16; i++)
	cia2[i] = 0;

    cia2rdi = 0;
    cia2sr_bits = 0;

    cia2_tac = cia2_tbc = 0xffff;
    cia2_tal = cia2_tbl = 0xffff;

    cia2_tas = CIAT_STOPPED;
    cia2_tbs = CIAT_STOPPED;
    cia2_tat = 0;
    cia2_tbt = 0;

    my_unset_tbi();
    my_unset_tai();

    memset(cia2todalarm, 0, sizeof(cia2todalarm));
    cia2todlatched = 0;
    cia2todstopped = 0;
    maincpu_set_alarm(A_CIA2TOD, cia2todticks);

    cia2int = 0;


#ifdef HAVE_RS232
    userport_printer_write_strobe(1);
    userport_printer_write_data(0xff);
    userport_serial_write_ctrl(0xff);
#endif
    iec_info = iec_get_drive_port();
}


void REGPARM2 store_cia2(ADDRESS addr, BYTE byte)
{
    CLOCK rclk;

    addr &= 0xf;

    vic_ii_handle_pending_alarms(maincpu_num_write_cycles());

    rclk = clk - STORE_OFFSET;

#ifdef CIA2_TIMER_DEBUG
    if (cia2_debugFlag)
	printf("store cia2[%02x] %02x @ clk=%d, pc=\n",
	       (int) addr, (int) byte, rclk);
#endif

    switch (addr) {

      case CIA_PRA:		/* port A */
      case CIA_DDRA:

    if (cia2[addr] != byte) {
	 BYTE tmp;

	 cia2[addr] = byte;
	 tmp = ~cia2[CIA_PRA] & cia2[CIA_DDRA];
	 mem_set_vbank(tmp & 3);
	 iec_cpu_write(tmp);
	 userport_printer_write_strobe(tmp & 0x04);
    }
	break;

      case CIA_PRB:		/* port B */
      case CIA_DDRB:
	if ((cia2[CIA_CRA] | cia2[CIA_CRB]) & 0x02) {
	    update_cia2(rclk);
	    if (cia2[CIA_CRA] & 0x02) {
		byte &= 0xbf;
		if (cia2_tap)
		    byte |= 0x40;
	    }
	    if (cia2[CIA_CRB] & 0x02) {
		byte &= 0x7f;
		if (cia2_tbp)
		    byte |= 0x80;
	    }
	}

    cia2[addr] = byte;
    byte = cia2[CIA_PRB] | ~cia2[CIA_DDRB];
    if (true1541_parallel_cable_enabled)
	parallel_cable_cpu_write(byte, ((addr == CIA_PRB) ? 1 : 0));
#ifdef HAVE_RS232
    userport_printer_write_data(byte);
    userport_serial_write_ctrl(byte);
#endif
	break;

	/* This handles the timer latches.  The kludgy stuff is an attempt
           emulate the correct behavior when the latch is written to during
           an underflow.  */
      case CIA_TAL:
	update_tai(rclk); /* schedule alarm in case latch value is changed */
	update_cia2(rclk - 1);
	if (cia2_tac == cia2_tal && cia2_tas == CIAT_RUNNING) {
	    cia2_tac = cia2_tal = (cia2_tal & 0xff00) | byte;
	    cia2_tau = rclk + cia2_tac;
	    update_cia2(rclk);
	} else {
	    cia2_tal = (cia2_tal & 0xff00) | byte;
	}
	break;
      case CIA_TBL:
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
	update_cia2(rclk - 1);
	if (cia2_tbc == cia2_tbl && cia2_tbs == CIAT_RUNNING) {
	    cia2_tbc = cia2_tbl = (cia2_tbl & 0xff00) | byte;
	    cia2_tbu = rclk + cia2_tbc + 1;
	    update_cia2(rclk);
	} else {
	    cia2_tbl = (cia2_tbl & 0xff00) | byte;
	}
	break;
      case CIA_TAH:
	update_tai(rclk); /* schedule alarm in case latch value is changed */
	update_cia2(rclk - 1);
	if (cia2_tac == cia2_tal && cia2_tas == CIAT_RUNNING) {
	    cia2_tac = cia2_tal = (cia2_tal & 0x00ff) | (byte << 8);
	    cia2_tau = rclk + cia2_tac;
	    update_cia2(rclk);
	} else {
	    cia2_tal = (cia2_tal & 0x00ff) | (byte << 8);
	}
	if (cia2_tas == CIAT_STOPPED)
	    cia2_tac = cia2_tal;
	break;
      case CIA_TBH:
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
	update_cia2(rclk - 1);
	if (cia2_tbc == cia2_tbl && cia2_tbs == CIAT_RUNNING) {
	    cia2_tbc = cia2_tbl = (cia2_tbl & 0x00ff) | (byte << 8);
	    cia2_tbu = rclk + cia2_tbc + 1;
	    update_cia2(rclk);
	} else {
	    cia2_tbl = (cia2_tbl & 0x00ff) | (byte << 8);
	}
	if (cia2_tbs == CIAT_STOPPED)
	    cia2_tbc = cia2_tbl;
	break;

	/*
	 * TOD clock is stopped by writing Hours, and restarted
	 * upon writing Tenths of Seconds.
	 *
	 * REAL:  TOD register + (wallclock - cia2todrel)
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
	if (cia2[CIA_CRB] & 0x80)
	    cia2todalarm[addr - CIA_TOD_TEN] = byte;
	else {
	    if (addr == CIA_TOD_TEN)
		cia2todstopped = 0;
	    if (addr == CIA_TOD_HR)
		cia2todstopped = 1;
	    cia2[addr] = byte;
	}
	check_cia2todalarm(rclk);
	break;

      case CIA_SDR:		/* Serial Port output buffer */
	cia2[addr] = byte;
	if ((cia2[CIA_CRA] & 0x40) == 0x40) {
	    if (cia2sr_bits <= 16) {
		if(!cia2sr_bits) {
    	            
		}
		if(cia2sr_bits < 16) {
	            /* switch timer A alarm on again, if necessary */
	            update_cia2(rclk);
	            if (cia2_tau) {
		        my_set_tai_clk(cia2_tau + 1);
	            }
		}

	        cia2sr_bits += 16;

#if defined (CIA2_TIMER_DEBUG)
	        if (cia2_debugFlag)
	    	    printf("CIA2: start SDR rclk=%d\n", rclk);
#endif
  	    }
	}
	break;

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Control Register */
	update_cia2(rclk);

#if defined (CIA2_TIMER_DEBUG)
	if (cia2_debugFlag)
	    printf("CIA2 set CIA_ICR: 0x%x\n", byte);
#endif

	if (byte & CIA_IM_SET) {
	    cia2ier |= (byte & 0x7f);
	} else {
	    cia2ier &= ~(byte & 0x7f);
	}

	/* This must actually be delayed one cycle! */
#if defined(CIA2_TIMER_DEBUG)
	if (cia2_debugFlag)
	    printf("    set icr: ifr & ier & 0x7f -> %02x, int=%02x\n",
		   cia2ier & cia2int & 0x7f, cia2int);
#endif
	if (cia2ier & cia2int & 0x7f) {
	    my_set_int(I_CIA2FL, IK_NMI, rclk);
	}
	if (cia2ier & (CIA_IM_TA + CIA_IM_TB)) {
	    if ((cia2ier & CIA_IM_TA) && cia2_tau) {
		my_set_tai_clk(cia2_tau + 1);
	    }
	    if ((cia2ier & CIA_IM_TB) && cia2_tbu) {
		my_set_tbi_clk(cia2_tbu + 1);
	    }
	}
	/* Control */
	break;

      case CIA_CRA:		/* control register A */
	update_tai(rclk); /* schedule alarm in case latch value is changed */
	update_cia2(rclk);
#if defined (CIA2_TIMER_DEBUG)
	if (cia2_debugFlag)
	    printf("CIA2 set CIA_CRA: 0x%x (clk=%d, pc=, tal=%u, tac=%u)\n",
		   byte, rclk, /*program_counter,*/ cia2_tal, cia2_tac);
#endif

	/* bit 7 tod frequency */
	/* bit 6 serial port mode */

	/* bit 4 force load */
	if (byte & 0x10) {
	    cia2_tac = cia2_tal;
	    if (cia2_tas == CIAT_RUNNING) {
		cia2_tau = rclk + cia2_tac + 2;
		my_set_tai_clk(cia2_tau + 1);
	    }
	}
	/* bit 3 timer run mode */
	/* bit 2 & 1 timer output to PB6 */

	/* bit 0 start/stop timer */
	/* bit 5 timer count mode */
	if ((byte & 1) && !(cia2[CIA_CRA] & 1))
	    cia2_tat = 1;
	if ((byte ^ cia2[addr]) & 0x21) {
	    if ((byte & 0x21) == 0x01) {	/* timer just started */
		cia2_tas = CIAT_RUNNING;
		cia2_tau = rclk + (cia2_tac + 1) + ((byte & 0x10) >> 4);
		my_set_tai_clk(cia2_tau + 1);
	    } else {		/* timer just stopped */
		cia2_tas = CIAT_STOPPED;
		cia2_tau = 0;
		/* 1 cycle delay for counter stop. */
		if (!(byte & 0x10)) {
		    /* 1 cycle delay for counter stop.  This must only happen
                       if we are not forcing load at the same time (i.e. bit
                       4 in `byte' is zero). */
		    if (cia2_tac > 0)
			cia2_tac--;
		}
		my_unset_tai();
	    }
	}
#if defined (CIA2_TIMER_DEBUG)
	if (cia2_debugFlag)
	    printf("    -> tas=%d, tau=%d\n", cia2_tas, cia2_tau);
#endif
	cia2[addr] = byte & 0xef;	/* remove strobe */

	break;

      case CIA_CRB:		/* control register B */
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
	update_cia2(rclk);

#if defined (CIA2_TIMER_DEBUG)
	if (cia2_debugFlag)
	    printf("CIA2 set CIA_CRB: 0x%x (clk=%d, pc=, tbl=%u, tbc=%u)\n",
		   byte, rclk, cia2_tbl, cia2_tbc);
#endif


	/* bit 7 set alarm/tod clock */
	/* bit 4 force load */
	if (byte & 0x10) {
	    cia2_tbc = cia2_tbl;
	    if (cia2_tbs == CIAT_RUNNING) {
		cia2_tbu = rclk + cia2_tbc + 2;
#if defined(CIA2_TIMER_DEBUG)
		if (cia2_debugFlag)
		    printf("CIA2: rclk=%d force load: set tbu alarm to %d\n", rclk, cia2_tbu);
#endif
		my_set_tbi_clk(cia2_tbu + 1);
	    }
	}
	/* bit 3 timer run mode */
	/* bit 2 & 1 timer output to PB6 */

	/* bit 0 stbrt/stop timer */
	/* bit 5 & 6 timer count mode */
	if ((byte & 1) && !(cia2[CIA_CRB] & 1))
	    cia2_tbt = 1;
	if ((byte ^ cia2[addr]) & 0x61) {
	    if ((byte & 0x61) == 0x01) {	/* timer just started */
		cia2_tbu = rclk + (cia2_tbc + 1) + ((byte & 0x10) >> 4);
#if defined(CIA2_TIMER_DEBUG)
		if (cia2_debugFlag)
		    printf("CIA2: rclk=%d start timer: set tbu alarm to %d\n", rclk, cia2_tbu);
#endif
		my_set_tbi_clk(cia2_tbu + 1);
		cia2_tbs = CIAT_RUNNING;
	    } else {		/* timer just stopped */
#if defined(CIA2_TIMER_DEBUG)
		if (cia2_debugFlag)
		    printf("CIA2: rclk=%d stop timer: set tbu alarm\n", rclk);
#endif
		my_unset_tbi();
		cia2_tbu = 0;
		if (!(byte & 0x10)) {
		    /* 1 cycle delay for counter stop.  This must only happen
                       if we are not forcing load at the same time (i.e. bit
                       4 in `byte' is zero). */
		    if (cia2_tbc > 0)
			cia2_tbc--;
		}
		/* this should actually read (byte & 0x61), but as CNT is high
		   by default, bit 0x20 is a `don't care' bit */
		if ((byte & 0x41) == 0x41) {
		    cia2_tbs = CIAT_COUNTTA;
		    update_cia2(rclk);
		    /* switch timer A alarm on if necessary */
		    if (cia2_tau) {
			my_set_tai_clk(cia2_tau + 1);
		    }
		} else {
		    cia2_tbs = CIAT_STOPPED;
		}
	    }
	}
	cia2[addr] = byte & 0xef;	/* remove strobe */
	break;

      default:
	cia2[addr] = byte;
    }				/* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_cia2(ADDRESS addr)
{

#if defined( CIA2_TIMER_DEBUG )

    BYTE read_cia2_(ADDRESS addr);
    BYTE tmp = read_cia2_(addr);

    if (cia2_debugFlag)
	printf("read cia2[%x] returns %02x @ clk=%d, pc=\n",
	       addr, tmp, clk - READ_OFFSET);
    return tmp;
}

BYTE read_cia2_(ADDRESS addr)
{

#endif

    static BYTE byte;
    CLOCK rclk;

    addr &= 0xf;

    vic_ii_handle_pending_alarms(0);

    rclk = clk - READ_OFFSET;


    switch (addr) {

      case CIA_PRA:		/* port A */

    if (!true1541_enabled)
	return ((cia2[CIA_PRA] | ~cia2[CIA_DDRA]) & 0x3f) |
	    (iec_info->iec_fast_1541 & 0x30) << 2;
    true1541_cpu_execute();
    byte = ((cia2[CIA_PRA] | ~cia2[CIA_DDRA]) & 0x3f) | iec_info->cpu_port;
	return byte;
	break;

      case CIA_PRB:		/* port B */

#ifdef HAVE_RS232
    byte = (true1541_parallel_cable_enabled
            ? parallel_cable_cpu_read()
            : (rsuser_enabled
		? userport_serial_read_ctrl()
		: cia2[CIA_PRB] | ~cia2[CIA_DDRB]));
#else
    byte = (true1541_parallel_cable_enabled
            ? parallel_cable_cpu_read()
            : cia2[CIA_PRB] | ~cia2[CIA_DDRB]));
#endif
        if ((cia2[CIA_CRA] | cia2[CIA_CRB]) & 0x02) {
	    update_cia2(rclk);
	    if (cia2[CIA_CRA] & 0x02) {
		byte &= 0xbf;
		if (cia2_tap)
		    byte |= 0x40;
	    }
	    if (cia2[CIA_CRB] & 0x02) {
		byte &= 0x7f;
		if (cia2_tbp)
		    byte |= 0x80;
	    }
	}

	return byte;
	break;

	/* Timers */
      case CIA_TAL:		/* timer A low */
	update_cia2(rclk);
	return ((cia2_tac ? cia2_tac : cia2_tal) & 0xff);

      case CIA_TAH:		/* timer A high */
	update_cia2(rclk);
	return ((cia2_tac ? cia2_tac : cia2_tal) >> 8) & 0xff;

      case CIA_TBL:		/* timer B low */
	update_cia2(rclk);
	return cia2_tbc & 0xff;

      case CIA_TBH:		/* timer B high */
	update_cia2(rclk);
	return (cia2_tbc >> 8) & 0xff;

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
	if (!cia2todlatched)
	    memcpy(cia2todlatch, cia2 + CIA_TOD_TEN, sizeof(cia2todlatch));
	if (addr == CIA_TOD_TEN)
	    cia2todlatched = 0;
	if (addr == CIA_TOD_HR)
	    cia2todlatched = 1;
	return cia2[addr];

      case CIA_SDR:		/* Serial Port Shift Register */
#if 0				/*def DEBUG */
	cia2_dump(stdout);
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
	return (cia2[addr]);

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Flag Register */
	{
	    BYTE t = 0;


    if (true1541_parallel_cable_enabled)
	true1541_cpu_execute();
#ifdef CIA2_TIMER_DEBUG
	    if (cia2_debugFlag)
		printf("CIA2 read intfl: rclk=%d, alarm_ta=%d, alarm_tb=%d\n",
			rclk, maincpu_int_status.alarm_clk[A_CIA2TA],
			maincpu_int_status.alarm_clk[A_CIA2TB]);
#endif
	
	    cia2rdi = rclk;
            t = cia2int;	/* we clean cia2int anyway, so make int_* */
	    cia2int = 0;	/* believe it is already */

            if (rclk >= cia2_tai)
                int_cia2ta(rclk - cia2_tai);
            if (rclk >= cia2_tbi)
                int_cia2tb(rclk - cia2_tbi);

	    cia2int |= t;	/* some bits can be set -> or with old value */

	    update_cia2(rclk);
	    t = cia2int | cia2flag;

#ifdef CIA2_TIMER_DEBUG
	    if (cia2_debugFlag)
		printf("CIA2 read intfl gives cia2int=%02x -> %02x @"
		       " PC=, sr_bits=%d, clk=%d, ta=%d, tb=%d\n",
		       cia2int, t, cia2sr_bits, clk, 
			(cia2_tac ? cia2_tac : cia2_tal),
			cia2_tbc);
#endif

	    cia2flag = 0;
	    cia2int = 0;
	    my_set_int(I_CIA2FL, 0, rclk);

	    return (t);
	}
    }				/* switch */

    return (cia2[addr]);
}

BYTE REGPARM1 peek_cia2(ADDRESS addr)
{
    /* This code assumes that update_cia2 is a projector - called at
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
	if (!cia2todlatched)
	    memcpy(cia2todlatch, cia2 + CIA_TOD_TEN, sizeof(cia2todlatch));
	return cia2[addr];

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Flag Register */
	{
	    BYTE t = 0;


    if (true1541_parallel_cable_enabled)
	true1541_cpu_execute();
#ifdef CIA2_TIMER_DEBUG
	    if (cia2_debugFlag)
		printf("CIA2 read intfl: rclk=%d, alarm_ta=%d, alarm_tb=%d\n",
			rclk, maincpu_int_status.alarm_clk[A_CIA2TA],
			maincpu_int_status.alarm_clk[A_CIA2TB]);
#endif
	
	    cia2rdi = rclk;
            t = cia2int;	/* we clean cia2int anyway, so make int_* */
	    cia2int = 0;	/* believe it is already */

            if (rclk >= cia2_tai)
                int_cia2ta(rclk - cia2_tai);
            if (rclk >= cia2_tbi)
                int_cia2tb(rclk - cia2_tbi);

	    cia2int |= t;	/* some bits can be set -> or with old value */

	    update_cia2(rclk);
	    t = cia2int | cia2flag;

#ifdef CIA2_TIMER_DEBUG
	    if (cia2_debugFlag)
		printf("CIA2 read intfl gives cia2int=%02x -> %02x @"
		       " PC=, sr_bits=%d, clk=%d, ta=%d, tb=%d\n",
		       cia2int, t, cia2sr_bits, clk, 
			(cia2_tac ? cia2_tac : cia2_tal),
			cia2_tbc);
#endif

/*
	    cia2flag = 0;
	    cia2int = 0;
	    my_set_int(I_CIA2FL, 0, rclk);
*/
	    return (t);
	}
      default:
	break;
    }				/* switch */

    return read_cia2(addr);
}

/* ------------------------------------------------------------------------- */

int int_cia2ta(long offset)
{
    CLOCK rclk = clk - offset;

#if defined(CIA2_TIMER_DEBUG)
    if (cia2_debugFlag)
	printf("CIA2: int_cia2ta(rclk = %u, tal = %u, cra=%02x\n",
	       rclk, cia2_tal, cia2[CIA_CRA]);
#endif

    cia2_tat = (cia2_tat + 1) & 1;

    if ((cia2_tas == CIAT_RUNNING) && !(cia2[CIA_CRA] & 8)) {
	/* if we do not need alarm, no PB6, no shift register, and not timer B
	   counting timer A, then we can savely skip alarms... */
	if ( ( (cia2ier & CIA_IM_TA) && 
		(!(cia2int & 0x80)) )
	    || (cia2[CIA_CRA] & 0x42)
	    || (cia2_tbs == CIAT_COUNTTA)) {
	    if(offset > cia2_tal+1) {
	        my_set_tai_clk(
			clk - (offset % (cia2_tal+1)) + cia2_tal + 1 );
	    } else {
	        my_set_tai_clk(rclk + cia2_tal + 1 );
	    }
	} else {
	    /* cia2_tai = rclk + cia2_tal +1; - now keeps tai */
	    /* printf("cia2 unset alarm: clk=%d, rclk=%d, rdi=%d -> tai=%d\n",
			clk, rclk, cia2rdi, cia2_tai); */
	    maincpu_unset_alarm(A_CIA2TA);	/* do _not_ clear cia2_tai */
	}
    } else {
#if 0
	cia2_tas = CIAT_STOPPED;
	cia2[CIA_CRA] &= 0xfe;	/* clear run flag. Correct? */
	cia2_tau = 0;
#endif
	my_unset_tai();
    }

    if (cia2[CIA_CRA] & 0x40) {
	if (cia2sr_bits) {
#if defined(CIA2_TIMER_DEBUG)
	    if (cia2_debugFlag)
		printf("CIA2: rclk=%d SDR: timer A underflow, bits=%d\n",
		       rclk, cia2sr_bits);
#endif
	    if (!(--cia2sr_bits)) {
		cia2int |= CIA_IM_SDR;
	    }
	    if(cia2sr_bits == 16) {
		
	    }
	}
    }
    if (cia2_tbs == CIAT_COUNTTA) {
	if (!cia2_tbc) {
	    cia2_tbc = cia2_tbl;
	    cia2_tbu = rclk;
#if defined(CIA2_TIMER_DEBUG)
	    if (cia2_debugFlag)
		printf("CIA2: timer B underflow when counting timer A occured, rclk=%d!\n", rclk);
#endif
	    cia2int |= CIA_IM_TB;
	    my_set_tbi_clk(rclk);
	} else {
	    cia2_tbc--;
	}
    }

    /* CIA_IM_TA is not set here, as it can be set in update(), reset
       by reading the ICR and then set again here because of delayed
       calling of int() */
    if ((IK_NMI == IK_NMI && cia2rdi != rclk - 1)
        || (IK_NMI == IK_IRQ && cia2rdi < rclk - 1)) {
        if ((cia2int | CIA_IM_TA) & cia2ier & 0x7f) {
            my_set_int(I_CIA2FL, IK_NMI, rclk);
        }
    }

    return 0;
}


/*
 * Timer B can run in 2 (4) modes
 * cia2[f] & 0x60 == 0x00   count system 02 pulses
 * cia2[f] & 0x60 == 0x40   count timer A underflows
 * cia2[f] & 0x60 == 0x20 | 0x60 count CNT pulses => counter stops
 */


int int_cia2tb(long offset)
{
    CLOCK rclk = clk - offset;

#if defined(CIA2_TIMER_DEBUG)
    if (cia2_debugFlag)
	printf("CIA2: timer B int_cia2tb(rclk=%d, tbs=%d)\n", rclk, cia2_tbs);
#endif

    cia2_tbt = (cia2_tbt + 1) & 1;

    /* running and continous, then next alarm */
    if (cia2_tbs == CIAT_RUNNING) {
	if (!(cia2[CIA_CRB] & 8)) {
#if defined(CIA2_TIMER_DEBUG)
	    if (cia2_debugFlag)
		printf("CIA2: rclk=%d cia2tb: set tbu alarm to %d\n", rclk, rclk + cia2_tbl + 1);
#endif
	    /* if no interrupt flag we can safely skip alarms */
	    if (cia2ier & CIA_IM_TB) {
		if(offset > cia2_tbl+1) {
		    my_set_tbi_clk( 
			clk - (offset % (cia2_tbl+1)) + cia2_tbl + 1);
		} else {
		    my_set_tbi_clk(rclk + cia2_tbl + 1);
		}
	    } else {
		/* cia2_tbi = rclk + cia2_tbl + 1; */
		maincpu_unset_alarm(A_CIA2TB);
	    }
	} else {
#if 0
	    cia2_tbs = CIAT_STOPPED;
	    cia2[CIA_CRB] &= 0xfe; /* clear start bit */
	    cia2_tbu = 0;
#endif /* 0 */
#if defined(CIA2_TIMER_DEBUG)
	    if (cia2_debugFlag)
		printf("CIA2: rclk=%d cia2tb: unset tbu alarm\n", rclk);
#endif
	    my_unset_tbi();
	}
    } else {
	if (cia2_tbs == CIAT_COUNTTA) {
	    if ((cia2[CIA_CRB] & 8)) {
		cia2_tbs = CIAT_STOPPED;
		cia2[CIA_CRB] &= 0xfe;		/* clear start bit */
		cia2_tbu = 0;
	    }
	}
	cia2_tbu = 0;
	my_unset_tbi();
#if defined(CIA2_TIMER_DEBUG)
	if (cia2_debugFlag)
	    printf("CIA2: rclk=%d cia2tb: unset tbu alarm\n", rclk);
#endif
    }

    if ((IK_NMI == IK_NMI && cia2rdi != rclk - 1)
        || (IK_NMI == IK_IRQ && cia2rdi < rclk - 1)) {
        if ((cia2int | CIA_IM_TB) & cia2ier & 0x7f) {
            my_set_int(I_CIA2FL, IK_NMI, rclk);
        }
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

void cia2_set_flag(void)
{
    cia2int |= CIA_IM_FLG;
    if (cia2[CIA_ICR] & CIA_IM_FLG) {
        my_set_int(I_CIA2FL, IK_NMI, clk);
    }
}

void cia2_set_sdr(BYTE data)
{
    cia2[CIA_SDR] = data;
    cia2int |= CIA_IM_SDR;
    if (cia2[CIA_ICR] & CIA_IM_SDR) {
        my_set_int(I_CIA2FL, IK_NMI, clk);
    }
}

/* ------------------------------------------------------------------------- */

int int_cia2tod(long offset)
{
    int t, pm;
    CLOCK rclk = clk - offset;

#ifdef DEBUG
    if (cia2_debugFlag)
	printf("CIA2: TOD timer event (1/10 sec tick), tod=%02x:%02x,%02x.%x\n",
	       cia2[CIA_TOD_HR], cia2[CIA_TOD_MIN], cia2[CIA_TOD_SEC],
	       cia2[CIA_TOD_TEN]);
#endif

    /* set up new int */
    maincpu_set_alarm(A_CIA2TOD, cia2todticks);

    if (!cia2todstopped) {
	/* inc timer */
	t = bcd2byte(cia2[CIA_TOD_TEN]);
	t++;
	cia2[CIA_TOD_TEN] = byte2bcd(t % 10);
	if (t >= 10) {
	    t = bcd2byte(cia2[CIA_TOD_SEC]);
	    t++;
	    cia2[CIA_TOD_SEC] = byte2bcd(t % 60);
	    if (t >= 60) {
		t = bcd2byte(cia2[CIA_TOD_MIN]);
		t++;
		cia2[CIA_TOD_MIN] = byte2bcd(t % 60);
		if (t >= 60) {
		    pm = cia2[CIA_TOD_HR] & 0x80;
		    t = bcd2byte(cia2[CIA_TOD_HR] & 0x1f);
		    if (!t)
			pm ^= 0x80;	/* toggle am/pm on 0:59->1:00 hr */
		    t++;
		    t = t % 12 | pm;
		    cia2[CIA_TOD_HR] = byte2bcd(t);
		}
	    }
	}
#ifdef DEBUG
	if (cia2_debugFlag)
	    printf("CIA2: TOD after event :tod=%02x:%02x,%02x.%x\n",
	       cia2[CIA_TOD_HR], cia2[CIA_TOD_MIN], cia2[CIA_TOD_SEC],
		   cia2[CIA_TOD_TEN]);
#endif
	/* check alarm */
	check_cia2todalarm(rclk);
    }
    return 0;
}

/* -------------------------------------------------------------------------- */


void cia2_prevent_clk_overflow(CLOCK sub)
{

    update_tai(clk);
    update_tbi(clk);

    update_cia2(clk);

    if(cia2_tai && (cia2_tai != -1))
        cia2_tai -= sub;
    if(cia2_tbi && (cia2_tbi != -1))
        cia2_tbi -= sub;

    if (cia2_tau)
	cia2_tau -= sub;
    if (cia2_tbu)
	cia2_tbu -= sub;
    if (cia2rdi > sub)
	cia2rdi -= sub;
    else
	cia2rdi = 0;
}

#if 0

void cia2_dump(FILE * fp)
{

    update_cia2(clk);
    fprintf(fp, "[CIA2]\n");
    fprintf(fp, "PA %d %d\n", cia2[CIA_PRA], cia2[CIA_DDRA]);
    fprintf(fp, "PB %d %d\n", cia2[CIA_PRB], cia2[CIA_DDRB]);
    fprintf(fp, "TA %u %u %d [$%02x = ", cia2_tac, cia2_tal,
	    cia2[CIA_CRA], cia2[CIA_CRA]);
    fprintf(fp, "%s%s%s%s%s]\n",
	    (cia2[CIA_CRA] & 1) ? "running " : "stopped ",
	    (cia2[CIA_CRA] & 8) ? "one-shot " : "continous ",
	    (cia2[CIA_CRA] & 16) ? "force-load " : "",
	    (cia2[CIA_CRA] & 32) ? "cnt " : "phi2 ",
	    (cia2[CIA_CRA] & 64) ? "sr_out " : "sr_in ");

    fprintf(fp, "TB %u %u %d [$%02x = ", cia2_tbc, cia2_tbl,
	    cia2[CIA_CRB], cia2[CIA_CRB]);
    fprintf(fp, "%s%s%s%s]\n",
	    (cia2[CIA_CRB] & 1) ? "running " : "stopped ",
	    (cia2[CIA_CRB] & 8) ? "one-shot " : "continous ",
	    (cia2[CIA_CRB] & 16) ? "force-load " : "",
	    (cia2[CIA_CRB] & 32)
	    ? ((cia2[CIA_CRB] & 64) ? "timerA+cnt" : "cnt ")
	    : ((cia2[CIA_CRB] & 64) ? "timerA" : "phi2 ")
	);

    fprintf(fp, "ICR %u %u %u", cia2int, cia2ier,
	    maincpu_int_status.pending_int[I_CIA2FL]);
    fprintf(fp, " [fl= %s%s%s%s%s] ",
	    (cia2int & 1) ? "TA " : "",
	    (cia2int & 2) ? "TB " : "",
	    (cia2int & 4) ? "Alarm " : "",
	    (cia2int & 8) ? "SDR " : "",
	    (cia2int & 16) ? "Flag " : "");
    fprintf(fp, "[mask= %s%s%s%s%s]\n",
	    (cia2ier & 1) ? "TA " : "",
	    (cia2ier & 2) ? "TB " : "",
	    (cia2ier & 4) ? "Alarm " : "",
	    (cia2ier & 8) ? "SDR " : "",
	    (cia2ier & 16) ? "Flag " : "");

    fprintf(fp, "SR %d %d\n", cia2[CIA_SDR], cia2sr_bits);
    fprintf(fp, "TOD %d %d %d %d\n", cia2[CIA_TOD_HR],
	    cia2[CIA_TOD_MIN], cia2[CIA_TOD_SEC], cia2[CIA_TOD_SEC]);
}


void cia2_undump_line(char *s)
{
    unsigned int d1, d2, d3, d4;

    if (s == strstr(s, "PA")) {
	sscanf(s + 2, "%u %u", &d1, &d2);
	cia2[CIA_PRA] = d1;
	cia2[CIA_DDRA] = d2;
	store_cia2(CIA_PRA, cia2[CIA_PRA]);
    } else if (s == strstr(s, "PB")) {
	sscanf(s + 2, "%u %u", &d1, &d2);
	cia2[CIA_PRB] = d1;
	cia2[CIA_DDRB] = d2;
	store_cia2(CIA_PRB, cia2[CIA_PRB]);
    } else if (s == strstr(s, "TA")) {
	sscanf(s + 2, "%u %u %u", &cia2_tac, &cia2_tal, &d1);
	cia2[CIA_CRA] = d1;
	if ((cia2[CIA_CRA] & 0x21) == 0x01) {
	    cia2_tau = clk + cia2_tac;
	    cia2_tas = CIAT_RUNNING;
	    my_set_tai_clk(cia2_tau + 1);
	} else {
	    cia2_tau = 0;
	    cia2_tas = CIAT_STOPPED;
	}
    } else if (s == strstr(s, "TB")) {
	sscanf(s + 2, "%u %u %u", &cia2_tbc, &cia2_tbl, &d1);
	cia2[CIA_CRB] = d1;
	if ((cia2[CIA_CRB] & 0x61) == 0x01) {
	    cia2_tbu = clk + cia2_tbc;
	    cia2_tbs = CIAT_RUNNING;
	    my_set_tbi_clk(cia2_tbu + 1);
	} else {
	    cia2_tbu = 0;
	    if ((cia2[CIA_CRB] & 0x61) == 0x41) {
		cia2_tbs = CIAT_COUNTTA;
	    } else {
		cia2_tbs = CIAT_STOPPED;
	    }
	}
    } else if (s == strstr(s, "ICR")) {
	sscanf(s + 3, "%d %d", &d1, &d2);
	cia2int = d1;
	cia2ier = d2;
	if (cia2int & cia2ier & 0x7f) {
	    my_set_int(I_CIA2FL, IK_NMI, rclk);
	} else {
	    my_set_int(I_CIA2FL, 0, rclk);
	}
    } else if (s == strstr(s, "SR")) {
	sscanf(s + 2, "%d %d", &d1, &cia2sr_bits);
	cia2[CIA_SDR] = d1;
    } else if (s == strstr(s, "TOD")) {
	sscanf(s + 3, "%u %u %u %u", &d1, &d2, &d3, &d4);
	cia2[CIA_TOD_HR] = d1;
	cia2[CIA_TOD_MIN] = d2;
	cia2[CIA_TOD_SEC] = d3;
	cia2[CIA_TOD_TEN] = d4;
    } else {
	printf("unknown dump format line for CIA2: \n%s\n", s);
    }
}

#endif



void userport_printer_set_busy(int flank)
{
    if(!flank) {
	cia2_set_flag();
    }
}


