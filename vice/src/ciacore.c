/*
 * ciacore.c - Template file for MOS6526 (CIA) emulation.
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
 * There now is a new function, update_cia(). It computes all differences
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
 * may be set a few cycles too late due to late call of int_ciat*() due to
 * opcode execution time. This can be fixed by checking in the beginning of
 * read_* and write_* if an int_* is scheduled and executing it before.  Then
 * the setting of the ICR could also be moved from update to int_*().  But
 * the bug only affects the contents of the ICR. The interrupt is generated
 * at the right time (hopefully).
 *
 * There is one HACK to make a game work: in update_cia() a fix is done for
 * Arkanoid. This game counts shift register bits (i.e. TA underflows) by
 * setting TA to one-shot.  The ICR is read just before the int_ciata()
 * function is called, and the int bit is missed, so there is a check in
 * update_cia() (this is probably a fix and not a hack... :-)
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


/* Make the TOD count 50/60Hz even if we do not run at 1MHz ... */
#ifndef CYCLES_PER_SEC
#define	CYCLES_PER_SEC 	1000000
#endif

/* The next two defines are the standard use of the chips. However,
   they can be overriden by a define from the .def file. That's where
   these defs should actually go in the first place. This here is not
   the best solution, but at the moment it's better than editing 9+
   .def files only to find it's slower... However, putting them in the
   .def files would allow making those functions static inline, which
   should be better. But is it as fast? */

#if 0
/* Fallback for "normal" use of the chip. In real operation the interrupt
   number can be replaced with the known constant I_ciaFL (see 
   cia_restore_int() below. */
#ifndef cia_set_int_clk
#define	cia_set_int_clk(value,num,clk)					\
		set_int(&mycpu_int_status,(num),(value),(clk))
#endif

/* Fallback for "normal" use of the chip. */
#ifndef cia_restore_int
#define	cia_restore_int(value)					\
		set_int_noclk(&mycpu_int_status,(I_ciaFL),(value))
#endif
#endif

/* The following is an attempt in rewriting the interrupt defines into 
   static inline functions. This should not hurt, but I still kept the
   define below, to be able to compare speeds. 
   The semantics of the call has changed, the interrupt number is
   not needed anymore (because it's known to my_set_int(). Actually
   one could also remove MYCIA_INT as it is also know... */

/* new semantics and as inline function, value can be replaced by 0/1 */
static inline void my_set_int(int value, CLOCK rclk)
{
#ifdef cia_TIMER_DEBUG
    if(cia_debugFlag) {
        log_message(cia_log, "set_int(rclk=%d, int=%d, d=%d pc=).",
           rclk,(int_num),(value));
    }
#endif
    if ((value)) {
        ciaint |= 0x80;
        cia_set_int_clk((MYCIA_INT), (rclk));
    } else {
        cia_set_int_clk(0, (rclk));
    }
}

/*
 * scheduling int_ciat[ab] calls -
 * warning: int_ciata uses mycpu_* stuff!
 */

static inline void my_set_tai_clk(CLOCK tai_clk) 
{
    cia_tai = tai_clk;
    alarm_set(&cia_ta_alarm, tai_clk);
}

static inline void my_unset_tai(void) 
{
    cia_tai = -1;							\
    alarm_unset(&cia_ta_alarm);
}

static inline void my_set_tbi_clk(CLOCK tbi_clk) 
{
    cia_tbi = tbi_clk;
    alarm_set(&cia_tb_alarm, tbi_clk);
}

static inline void my_unset_tbi(void)
{
    cia_tbi = -1;
    alarm_unset(&cia_tb_alarm);
}

/*
 * Those routines setup the ciat[ab]i clocks to a value above
 * rclk and schedule the next int_ciat[ab] alarm
 */
static inline void update_tai(CLOCK rclk)
{
    if(cia_tai < rclk) {
        int t = ciaint;
        ciaint = 0;
        int_ciata(rclk - cia_tai);
        ciaint |= t;
    }
}

static inline void update_tbi(CLOCK rclk)
{
    if(cia_tbi < rclk) {
        int t = ciaint;
        ciaint = 0;
        int_ciatb(rclk - cia_tbi);
        ciaint |= t;
    }
}

#if defined(cia_TIMER_DEBUG) || defined(cia_IO_DEBUG)
static int cia_debugFlag = 0;

#endif


/* ------------------------------------------------------------------------- */
/* cia */


inline static void check_ciatodalarm(CLOCK rclk)
{
    if (!memcmp(ciatodalarm, cia + CIA_TOD_TEN, sizeof(ciatodalarm))) {
	ciaint |= CIA_IM_TOD;
	if (cia[CIA_ICR] & CIA_IM_TOD) {
            my_set_int(MYCIA_INT, myclk);
	}
    }
}

static int update_cia(CLOCK rclk)
{
    int tmp = 0;
    unsigned int ista = 0;
    BYTE sif = (ciaint & ciaier & 0x7f);
    /* Tick when we virtually added an interrupt flag first. */
    CLOCK added_int_clk = (ciaint & 0x80) ? rclk - 3 : CLOCK_MAX;

#ifdef cia_TIMER_DEBUG
    if (cia_debugFlag)
	log_message(cia_log, "update: rclk=%d, tas=%d, tau=%d, tal=%u, ",
	       rclk, cia_tas, cia_tau, cia_tal);
#endif

    if (cia_tas == CIAT_RUNNING) {
	if (rclk < cia_tau + 1) {
	    cia_tac = cia_tau - rclk;
	    tmp = 0;
	} else {
	    if (cia[CIA_CRA] & 0x08) {
		tmp = 1;
		if ((ciaier & CIA_IM_TA)
		    && (cia_tau < added_int_clk))
		    added_int_clk = cia_tau;
		cia_tau = 0;
		my_unset_tai();
		cia_tac = cia_tal;
		cia_tas = CIAT_STOPPED;
		cia[CIA_CRA] &= 0xfe;

		/* this is a HACK for arkanoid... */
		if (ciasr_bits) {
		    ciasr_bits--;
		    if(ciasr_bits==16) {
			store_sdr(cia[CIA_SDR]);
		    }
		    if (!ciasr_bits) {
			ciaint |= CIA_IM_SDR;
			if ((ciaier & CIA_IM_SDR)
			    && (cia_tau < added_int_clk))
			    added_int_clk = cia_tau;
		    }
		}
	    } else {
		tmp = (rclk - cia_tau - 1) / (cia_tal + 1);
		cia_tau += tmp * (cia_tal + 1);
		if ((ciaier & CIA_IM_TA)
		    && (cia_tau < added_int_clk))
		    added_int_clk = cia_tau;
		cia_tau += 1 * (cia_tal + 1);
		cia_tac = cia_tau - rclk;
	    }

	    if (cia_tac == cia_tal)
		ista = 1;

            ciaint |= CIA_IM_TA;
	}
    }
#ifdef cia_TIMER_DEBUG
    if (cia_debugFlag)
	log_message(cia_log, "aic=%d, tac-> %u, tau-> %d tmp=%u",
                    added_int_clk, cia_tac, cia_tau, tmp);
#endif

    if (cia[CIA_CRA] & 0x04) {
	cia_tap = cia_tat;
    } else {
	cia_tap = cia_tac ? 0 : 1;
    }

    cia_tbp = 0;
    if (cia_tbs == CIAT_RUNNING) {
	if (rclk < cia_tbu + 1) {
	    cia_tbc = cia_tbu - rclk;
	} else {
	    if (cia[CIA_CRB] & 0x08) {
		tmp = 1;
		if ((ciaier & CIA_IM_TB) && (cia_tbu < added_int_clk))
		    added_int_clk = cia_tbu;
		cia_tbu = 0;
		my_unset_tbi();
		cia_tbc = cia_tbl;
		cia_tbs = CIAT_STOPPED;
		cia[CIA_CRB] &= 0xfe;
	    } else {
		tmp = (rclk - cia_tbu - 1) / (cia_tbl + 1);
		cia_tbu += tmp * (cia_tbl + 1);
		if ((ciaier & CIA_IM_TB) && (cia_tbu < added_int_clk))
		    added_int_clk = cia_tbu;
		cia_tbu += 1 * (cia_tbl + 1);
		cia_tbc = cia_tbu - rclk;
	    }
	    if (!cia_tbc)
		cia_tbc = cia_tbl;
            ciaint |= CIA_IM_TB;
	}
    } else if (cia_tbs == CIAT_COUNTTA) {
	/* missing: set added_int */
	if ((!cia_tbc) && ista) {
	    cia_tbp = 1;
	    cia_tbc = cia_tbl;
	    ciaint |= CIA_IM_TB;
	}
    }
    if (cia[CIA_CRB] & 0x04) {
	cia_tbp ^= cia_tbt;
    } else {
	cia_tbp = cia_tbc ? 0 : 1;
    }

#ifdef cia_TIMER_DEBUG
    if (cia_debugFlag)
	log_message(cia_log, "tbc-> %u, tbu-> %d, int %02x ->",
	       cia_tbc, cia_tbu, ciaint);
#endif

    /* have we changed the interrupt flags? */
    if (sif != (ciaier & ciaint & 0x7f)) {
	/* if we do not read ICR, do standard operation */
	if (rclk != ciardi) {
	    if (ciaier & ciaint & 0x7f) {
		/* sets bit 7 */
		my_set_int(MYCIA_INT, rclk);
	    }
	} else {
	    if (added_int_clk == ciardi) {
		ciaint &= 0x7f;
#ifdef cia_TIMER_DEBUG
		if (cia_debugFlag)
		    log_message(cia_log, "TA Reading ICR at rclk=%d prevented IRQ.",
			   rclk);
#endif
	    } else {
		if (ciaier & ciaint & 0x7f) {
		    /* sets bit 7 */
		    my_set_int(MYCIA_INT, rclk);
		}
	    }
	}
    }
#ifdef cia_TIMER_DEBUG
    if (cia_debugFlag)
	log_message(cia_log, "%02x.", ciaint);
#endif

    /* return true sif interrupt line is set at this clock time */
    return (!sif) && (ciaint & ciaier & 0x7f);
}

/* ------------------------------------------------------------------------- */

void mycia_init(void)
{
    alarm_init(&cia_ta_alarm, &mycpu_alarm_context, "ciaTimerA",
               int_ciata);
    alarm_init(&cia_tb_alarm, &mycpu_alarm_context, "ciaTimerB",
               int_ciatb);
    alarm_init(&cia_tod_alarm, &mycpu_alarm_context, "ciaTimeOfDay",
               int_ciatod);
}

void reset_mycia(void)
{
    int i;

    if (cia_log == LOG_ERR)
        cia_log = log_open("cia");

    ciatodticks = CYCLES_PER_SEC / 10;  /* cycles per tenth of a second */

    for (i = 0; i < 16; i++)
	cia[i] = 0;

    ciardi = 0;
    ciasr_bits = 0;

    cia_tac = cia_tbc = 0xffff;
    cia_tal = cia_tbl = 0xffff;

    cia_tas = CIAT_STOPPED;
    cia_tbs = CIAT_STOPPED;
    cia_tat = 0;
    cia_tbt = 0;

    my_unset_tbi();
    my_unset_tai();

    memset(ciatodalarm, 0, sizeof(ciatodalarm));
    ciatodlatched = 0;
    ciatodstopped = 0;
    alarm_set(&cia_tod_alarm, myclk + ciatodticks);

    ciaint = 0;
    my_set_int(0, myclk);

    oldpa = 0xff;
    oldpb = 0xff;

    do_reset_cia();
}


void REGPARM2 store_mycia(ADDRESS addr, BYTE byte)
{
    CLOCK rclk;

    addr &= 0xf;

    PRE_STORE_CIA

    rclk = myclk - STORE_OFFSET;

#ifdef cia_TIMER_DEBUG
    if (cia_debugFlag)
	log_message(cia_log, "store cia[%02x] %02x @ clk=%d",
                    (int) addr, (int) byte, rclk);
#endif

    switch (addr) {

      case CIA_PRA:		/* port A */
      case CIA_DDRA:
	cia[addr] = byte;
	byte = cia[CIA_PRA] | ~cia[CIA_DDRA];
	store_ciapa(rclk, byte);
	oldpa = byte;
	break;

      case CIA_PRB:		/* port B */
      case CIA_DDRB:
	cia[addr] = byte;
	byte = cia[CIA_PRB] | ~cia[CIA_DDRB];
	if ((cia[CIA_CRA] | cia[CIA_CRB]) & 0x02) {
	    update_cia(rclk);
	    if (cia[CIA_CRA] & 0x02) {
		byte &= 0xbf;
		if (cia_tap)
		    byte |= 0x40;
	    }
	    if (cia[CIA_CRB] & 0x02) {
		byte &= 0x7f;
		if (cia_tbp)
		    byte |= 0x80;
	    }
	}
	store_ciapb(rclk, byte);
	oldpb = byte;
	break;

	/* This handles the timer latches.  The kludgy stuff is an attempt
           emulate the correct behavior when the latch is written to during
           an underflow.  */
      case CIA_TAL:
	update_tai(rclk); /* schedule alarm in case latch value is changed */
	update_cia(rclk - 1);
	if (cia_tac == cia_tal && cia_tas == CIAT_RUNNING) {
	    cia_tac = cia_tal = (cia_tal & 0xff00) | byte;
	    cia_tau = rclk + cia_tac;
	    update_cia(rclk);
	} else {
	    cia_tal = (cia_tal & 0xff00) | byte;
	}
	break;
      case CIA_TBL:
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
	update_cia(rclk - 1);
	if (cia_tbc == cia_tbl && cia_tbs == CIAT_RUNNING) {
	    cia_tbc = cia_tbl = (cia_tbl & 0xff00) | byte;
	    cia_tbu = rclk + cia_tbc + 1;
	    update_cia(rclk);
	} else {
	    cia_tbl = (cia_tbl & 0xff00) | byte;
	}
	break;
      case CIA_TAH:
	update_tai(rclk); /* schedule alarm in case latch value is changed */
	update_cia(rclk - 1);
	if (cia_tac == cia_tal && cia_tas == CIAT_RUNNING) {
	    cia_tac = cia_tal = (cia_tal & 0x00ff) | (byte << 8);
	    cia_tau = rclk + cia_tac;
	    update_cia(rclk);
	} else {
	    cia_tal = (cia_tal & 0x00ff) | (byte << 8);
	}
	if (cia_tas == CIAT_STOPPED)
	    cia_tac = cia_tal;
	break;
      case CIA_TBH:
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
	update_cia(rclk - 1);
	if (cia_tbc == cia_tbl && cia_tbs == CIAT_RUNNING) {
	    cia_tbc = cia_tbl = (cia_tbl & 0x00ff) | (byte << 8);
	    cia_tbu = rclk + cia_tbc + 1;
	    update_cia(rclk);
	} else {
	    cia_tbl = (cia_tbl & 0x00ff) | (byte << 8);
	}
	if (cia_tbs == CIAT_STOPPED)
	    cia_tbc = cia_tbl;
	break;

	/*
	 * TOD clock is stopped by writing Hours, and restarted
	 * upon writing Tenths of Seconds.
	 *
	 * REAL:  TOD register + (wallclock - ciatodrel)
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
	if (cia[CIA_CRB] & 0x80)
	    ciatodalarm[addr - CIA_TOD_TEN] = byte;
	else {
	    if (addr == CIA_TOD_TEN)
		ciatodstopped = 0;
	    if (addr == CIA_TOD_HR)
		ciatodstopped = 1;
	    cia[addr] = byte;
	}
	check_ciatodalarm(rclk);
	break;

      case CIA_SDR:		/* Serial Port output buffer */
	cia[addr] = byte;
	if ((cia[CIA_CRA] & 0x40) == 0x40) {
	    if (ciasr_bits <= 16) {
		if(!ciasr_bits) {
    	            store_sdr(cia[CIA_SDR]);
		}
		if(ciasr_bits < 16) {
	            /* switch timer A alarm on again, if necessary */
	            update_cia(rclk);
	            if (cia_tau) {
		        my_set_tai_clk(cia_tau + 1);
	            }
		}

	        ciasr_bits += 16;

#if defined (cia_TIMER_DEBUG)
	        if (cia_debugFlag)
	    	    log_message(cia_log, "start SDR rclk=%d.", rclk);
#endif
  	    }
	}
	break;

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Control Register */
	update_cia(rclk);

#if defined (cia_TIMER_DEBUG)
	if (cia_debugFlag)
	    log_message(cia_log, "cia set CIA_ICR: 0x%x.", byte);
#endif

	if (byte & CIA_IM_SET) {
	    ciaier |= (byte & 0x7f);
	} else {
	    ciaier &= ~(byte & 0x7f);
	}

	/* This must actually be delayed one cycle! */
#if defined(cia_TIMER_DEBUG)
	if (cia_debugFlag)
	    log_message(cia_log, "    set icr: ifr & ier & 0x7f -> %02x, int=%02x.",
                        ciaier & ciaint & 0x7f, ciaint);
#endif
	if (ciaier & ciaint & 0x7f) {
	    my_set_int(MYCIA_INT, rclk);
	}
	if (ciaier & (CIA_IM_TA + CIA_IM_TB)) {
	    if ((ciaier & CIA_IM_TA) && cia_tau) {
		my_set_tai_clk(cia_tau + 1);
	    }
	    if ((ciaier & CIA_IM_TB) && cia_tbu) {
		my_set_tbi_clk(cia_tbu + 1);
	    }
	}
	/* Control */
	break;

      case CIA_CRA:		/* control register A */
	update_tai(rclk); /* schedule alarm in case latch value is changed */
	update_cia(rclk);
#if defined (cia_TIMER_DEBUG)
	if (cia_debugFlag)
	    log_message(cia_log, "cia set CIA_CRA: 0x%x (clk=%d, pc=, tal=%u, tac=%u).",
		   byte, rclk, /*program_counter,*/ cia_tal, cia_tac);
#endif

	/* bit 7 tod frequency */
	/* bit 6 serial port mode */

	/* bit 4 force load */
	if (byte & 0x10) {
	    cia_tac = cia_tal;
	    if (cia_tas == CIAT_RUNNING) {
		cia_tau = rclk + cia_tac + 2;
		my_set_tai_clk(cia_tau + 1);
	    }
	}
	/* bit 3 timer run mode */
	/* bit 2 & 1 timer output to PB6 */

	/* bit 0 start/stop timer */
	/* bit 5 timer count mode */
	if ((byte & 1) && !(cia[CIA_CRA] & 1))
	    cia_tat = 1;
	if ((byte ^ cia[addr]) & 0x21) {
	    if ((byte & 0x21) == 0x01) {	/* timer just started */
		cia_tas = CIAT_RUNNING;
		cia_tau = rclk + (cia_tac + 1) + ((byte & 0x10) >> 4);
		my_set_tai_clk(cia_tau + 1);
	    } else {		/* timer just stopped */
		cia_tas = CIAT_STOPPED;
		cia_tau = 0;
		/* 1 cycle delay for counter stop. */
		if (!(byte & 0x10)) {
		    /* 1 cycle delay for counter stop.  This must only happen
                       if we are not forcing load at the same time (i.e. bit
                       4 in `byte' is zero). */
		    if (cia_tac > 0)
			cia_tac--;
		}
		my_unset_tai();
	    }
	}
#if defined (cia_TIMER_DEBUG)
	if (cia_debugFlag)
	    log_message(cia_log, "    -> tas=%d, tau=%d.", cia_tas, cia_tau);
#endif
	cia[addr] = byte & 0xef;	/* remove strobe */

	break;

      case CIA_CRB:		/* control register B */
	update_tbi(rclk); /* schedule alarm in case latch value is changed */
	update_cia(rclk);

#if defined (cia_TIMER_DEBUG)
	if (cia_debugFlag)
	    log_message(cia_log, "cia set CIA_CRB: 0x%x (clk=%d, pc=, tbl=%u, tbc=%u).",
		   byte, rclk, cia_tbl, cia_tbc);
#endif


	/* bit 7 set alarm/tod clock */
	/* bit 4 force load */
	if (byte & 0x10) {
	    cia_tbc = cia_tbl;
	    if (cia_tbs == CIAT_RUNNING) {
		cia_tbu = rclk + cia_tbc + 2;
#if defined(cia_TIMER_DEBUG)
		if (cia_debugFlag)
		    log_message(cia_log, "rclk=%d force load: set tbu alarm to %d.", rclk, cia_tbu);
#endif
		my_set_tbi_clk(cia_tbu + 1);
	    }
	}
	/* bit 3 timer run mode */
	/* bit 2 & 1 timer output to PB6 */

	/* bit 0 stbrt/stop timer */
	/* bit 5 & 6 timer count mode */
	if ((byte & 1) && !(cia[CIA_CRB] & 1))
	    cia_tbt = 1;
	if ((byte ^ cia[addr]) & 0x61) {
	    if ((byte & 0x61) == 0x01) {	/* timer just started */
		cia_tbu = rclk + (cia_tbc + 1) + ((byte & 0x10) >> 4);
#if defined(cia_TIMER_DEBUG)
		if (cia_debugFlag)
		    log_message(cia_log, "rclk=%d start timer: set tbu alarm to %d.", rclk, cia_tbu);
#endif
		my_set_tbi_clk(cia_tbu + 1);
		cia_tbs = CIAT_RUNNING;
	    } else {		/* timer just stopped */
#if defined(cia_TIMER_DEBUG)
		if (cia_debugFlag)
		    log_message(cia_log, "rclk=%d stop timer: set tbu alarm.", rclk);
#endif
		my_unset_tbi();
		cia_tbu = 0;
		if (!(byte & 0x10)) {
		    /* 1 cycle delay for counter stop.  This must only happen
                       if we are not forcing load at the same time (i.e. bit
                       4 in `byte' is zero). */
		    if (cia_tbc > 0)
			cia_tbc--;
		}
		/* this should actually read (byte & 0x61), but as CNT is high
		   by default, bit 0x20 is a `don't care' bit */
		if ((byte & 0x41) == 0x41) {
		    cia_tbs = CIAT_COUNTTA;
		    update_cia(rclk);
		    /* switch timer A alarm on if necessary */
		    if (cia_tau) {
			my_set_tai_clk(cia_tau + 1);
		    }
		} else {
		    cia_tbs = CIAT_STOPPED;
		}
	    }
	}
	cia[addr] = byte & 0xef;	/* remove strobe */
	break;

      default:
	cia[addr] = byte;
    }				/* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_mycia(ADDRESS addr)
{

#if defined( cia_TIMER_DEBUG )

    BYTE read_cia_(ADDRESS addr);
    BYTE tmp = read_cia_(addr);

    if (cia_debugFlag)
	log_message(cia_log, "read cia[%x] returns %02x @ clk=%d.",
                    addr, tmp, myclk - READ_OFFSET);
    return tmp;
}

BYTE read_cia_(ADDRESS addr)
{

#endif

    BYTE byte = 0xff;
    CLOCK rclk;

    addr &= 0xf;

    PRE_READ_CIA

    rclk = myclk - READ_OFFSET;


    switch (addr) {

      case CIA_PRA:		/* port A */
        /* WARNING: this pin reads the voltage of the output pins, not
           the ORA value. Value read might be different from what is 
	   expected due to excessive load. */
	return read_ciapa();
	break;

      case CIA_PRB:		/* port B */
        /* WARNING: this pin reads the voltage of the output pins, not
           the ORA value. Value read might be different from what is 
	   expected due to excessive load. */
	byte = read_ciapb();
        if ((cia[CIA_CRA] | cia[CIA_CRB]) & 0x02) {
	    update_cia(rclk);
	    if (cia[CIA_CRA] & 0x02) {
		byte &= 0xbf;
		if (cia_tap)
		    byte |= 0x40;
	    }
	    if (cia[CIA_CRB] & 0x02) {
		byte &= 0x7f;
		if (cia_tbp)
		    byte |= 0x80;
	    }
	}
	return byte;
	break;

	/* Timers */
      case CIA_TAL:		/* timer A low */
	update_cia(rclk);
	return ((cia_tac ? cia_tac : cia_tal) & 0xff);

      case CIA_TAH:		/* timer A high */
	update_cia(rclk);
	return ((cia_tac ? cia_tac : cia_tal) >> 8) & 0xff;

      case CIA_TBL:		/* timer B low */
	update_cia(rclk);
	return cia_tbc & 0xff;

      case CIA_TBH:		/* timer B high */
	update_cia(rclk);
	return (cia_tbc >> 8) & 0xff;

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
	if (!ciatodlatched)
	    memcpy(ciatodlatch, cia + CIA_TOD_TEN, sizeof(ciatodlatch));
	if (addr == CIA_TOD_TEN)
	    ciatodlatched = 0;
	if (addr == CIA_TOD_HR)
	    ciatodlatched = 1;
	return ciatodlatch[addr - CIA_TOD_TEN];

      case CIA_SDR:		/* Serial Port Shift Register */
	return (cia[addr]);

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Flag Register */
	{
	    BYTE t = 0;

	    READ_CIAICR

#ifdef cia_TIMER_DEBUG
	    if (cia_debugFlag)
		log_message(cia_log, "cia read intfl: rclk=%d, alarm_ta=%d, alarm_tb=%d",
			rclk, mycpu_int_status.alarm_clk[A_ciaTA],
			mycpu_int_status.alarm_clk[A_ciaTB]);
#endif

	    ciardi = rclk;
            t = ciaint;	/* we clean ciaint anyway, so make int_* */
	    ciaint = 0;	/* believe it is already */

            if (rclk >= cia_tai)
                int_ciata(rclk - cia_tai);
            if (rclk >= cia_tbi)
                int_ciatb(rclk - cia_tbi);

	    ciaint |= t;	/* some bits can be set -> or with old value */

	    update_cia(rclk);
	    t = ciaint | ciaflag;

#ifdef cia_TIMER_DEBUG
	    if (cia_debugFlag)
		log_message(cia_log,
                            "read intfl gives ciaint=%02x -> %02x "
                            "sr_bits=%d, clk=%d, ta=%d, tb=%d.",
                            ciaint, t, ciasr_bits, clk,
                            (cia_tac ? cia_tac : cia_tal),
                            cia_tbc);
#endif

	    ciaflag = 0;
	    ciaint = 0;
	    my_set_int(0, rclk);

	    return (t);
	}
      case CIA_CRA:		/* Control Register A */
      case CIA_CRB:		/* Control Register B */
	update_cia(rclk);
	return cia[addr];
    }				/* switch */

    return (cia[addr]);
}

BYTE REGPARM1 peek_mycia(ADDRESS addr)
{
    /* This code assumes that update_cia is a projector - called at
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
	if (!ciatodlatched)
	    memcpy(ciatodlatch, cia + CIA_TOD_TEN, sizeof(ciatodlatch));
	return cia[addr];

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Flag Register */
	{
	    BYTE t = 0;

	    READ_CIAICR
#ifdef cia_TIMER_DEBUG
	    if (cia_debugFlag)
		log_message(cia_log,
                            "cia read intfl: rclk=%d, alarm_ta=%d, alarm_tb=%d.",
                            rclk, mycpu_int_status.alarm_clk[A_ciaTA],
                            mycpu_int_status.alarm_clk[A_ciaTB]);
#endif

	    /* ciardi = rclk; makes int_* and update_cia fiddle with IRQ */
            t = ciaint;	/* we clean ciaint anyway, so make int_* */
	    ciaint = 0;	/* believe it is already */

            if (rclk >= cia_tai)
                int_ciata(rclk - cia_tai);
            if (rclk >= cia_tbi)
                int_ciatb(rclk - cia_tbi);

	    ciaint |= t;	/* some bits can be set -> or with old value */

	    update_cia(rclk);
	    t = ciaint | ciaflag;

#ifdef cia_TIMER_DEBUG
	    if (cia_debugFlag)
		log_message(cia_log,
                            "read intfl gives ciaint=%02x -> %02x "
                            "sr_bits=%d, clk=%d, ta=%d, tb=%d.",
                            ciaint, t, ciasr_bits, clk,
                            (cia_tac ? cia_tac : cia_tal),
                            cia_tbc);
#endif

/*
	    ciaflag = 0;
	    ciaint = 0;
	    my_set_int(0, rclk);
*/
	    return (t);
	}
      default:
	break;
    }				/* switch */

    return read_mycia(addr);
}

/* ------------------------------------------------------------------------- */

static int int_ciata(long offset)
{
    CLOCK rclk = myclk - offset;

#if defined(cia_TIMER_DEBUG)
    if (cia_debugFlag)
	log_message(cia_log,
                    "int_ciata(rclk = %u, tal = %u, cra=%02x.",
                    rclk, cia_tal, cia[CIA_CRA]);
#endif

    cia_tat = (cia_tat + 1) & 1;

    if ((cia_tas == CIAT_RUNNING) && !(cia[CIA_CRA] & 8)) {
	/* if we do not need alarm, no PB6, no shift register, and not timer B
	   counting timer A, then we can savely skip alarms... */
	if ( ( (ciaier & CIA_IM_TA) &&
		(!(ciaint & 0x80)) )
	    || (cia[CIA_CRA] & 0x42)
	    || (cia_tbs == CIAT_COUNTTA)) {
	    if(offset > cia_tal+1) {
	        my_set_tai_clk(
			myclk - (offset % (cia_tal+1)) + cia_tal + 1 );
	    } else {
	        my_set_tai_clk(rclk + cia_tal + 1 );
	    }
	} else {
            alarm_unset(&cia_ta_alarm); /* do _not_ clear cia_tai */
	}
    } else {
	my_unset_tai();
    }

    if (cia[CIA_CRA] & 0x40) {
	if (ciasr_bits) {
#if defined(cia_TIMER_DEBUG)
	    if (cia_debugFlag)
		log_message(cia_log, "rclk=%d SDR: timer A underflow, bits=%d",
		       rclk, ciasr_bits);
#endif
	    if (!(--ciasr_bits)) {
		ciaint |= CIA_IM_SDR;
	    }
	    if(ciasr_bits == 16) {
		store_sdr(cia[CIA_SDR]);
	    }
	}
    }
    if (cia_tbs == CIAT_COUNTTA) {
	if (!cia_tbc) {
	    cia_tbc = cia_tbl;
	    cia_tbu = rclk;
#if defined(cia_TIMER_DEBUG)
	    if (cia_debugFlag)
		log_message(cia_log,
                            "timer B underflow when counting timer A occured, rclk=%d!", rclk);
#endif
	    ciaint |= CIA_IM_TB;
	    my_set_tbi_clk(rclk);
	} else {
	    cia_tbc--;
	}
    }

    /* CIA_IM_TA is not set here, as it can be set in update(), reset
       by reading the ICR and then set again here because of delayed
       calling of int() */
    if ((MYCIA_INT == IK_NMI && ciardi != rclk - 1)
        || (MYCIA_INT == IK_IRQ && ciardi < rclk - 1)) {
        if ((ciaint | CIA_IM_TA) & ciaier & 0x7f) {
            my_set_int(MYCIA_INT, rclk);
        }
    }

    return 0;
}


/*
 * Timer B can run in 2 (4) modes
 * cia[f] & 0x60 == 0x00   count system 02 pulses
 * cia[f] & 0x60 == 0x40   count timer A underflows
 * cia[f] & 0x60 == 0x20 | 0x60 count CNT pulses => counter stops
 */


static int int_ciatb(long offset)
{
    CLOCK rclk = myclk - offset;

#if defined(cia_TIMER_DEBUG)
    if (cia_debugFlag)
	log_message(cia_log,
                    "timer B int_ciatb(rclk=%d, tbs=%d).", rclk, cia_tbs);
#endif

    cia_tbt = (cia_tbt + 1) & 1;

    /* running and continous, then next alarm */
    if (cia_tbs == CIAT_RUNNING) {
	if (!(cia[CIA_CRB] & 8)) {
#if defined(cia_TIMER_DEBUG)
	    if (cia_debugFlag)
		log_message(cia_log,
                            "rclk=%d ciatb: set tbu alarm to %d.",
                            rclk, rclk + cia_tbl + 1);
#endif
	    /* if no interrupt flag we can safely skip alarms */
	    if (ciaier & CIA_IM_TB) {
		if(offset > cia_tbl+1) {
		    my_set_tbi_clk(
			myclk - (offset % (cia_tbl+1)) + cia_tbl + 1);
		} else {
		    my_set_tbi_clk(rclk + cia_tbl + 1);
		}
	    } else {
		/* cia_tbi = rclk + cia_tbl + 1; */
		alarm_unset(&cia_tb_alarm);
	    }
	} else {
#if 0
	    cia_tbs = CIAT_STOPPED;
	    cia[CIA_CRB] &= 0xfe; /* clear start bit */
	    cia_tbu = 0;
#endif /* 0 */
#if defined(cia_TIMER_DEBUG)
	    if (cia_debugFlag)
		log_message(cia_log,
                            "rclk=%d ciatb: unset tbu alarm.", rclk);
#endif
	    my_unset_tbi();
	}
    } else {
	if (cia_tbs == CIAT_COUNTTA) {
	    if ((cia[CIA_CRB] & 8)) {
		cia_tbs = CIAT_STOPPED;
		cia[CIA_CRB] &= 0xfe;		/* clear start bit */
		cia_tbu = 0;
	    }
	}
	cia_tbu = 0;
	my_unset_tbi();
#if defined(cia_TIMER_DEBUG)
	if (cia_debugFlag)
	    log_message(cia_log,
                        "rclk=%d ciatb: unset tbu alarm.", rclk);
#endif
    }

    if ((MYCIA_INT == IK_NMI && ciardi != rclk - 1)
        || (MYCIA_INT == IK_IRQ && ciardi < rclk - 1)) {
        if ((ciaint | CIA_IM_TB) & ciaier & 0x7f) {
            my_set_int(MYCIA_INT, rclk);
        }
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

void mycia_set_flag(void)
{
    ciaint |= CIA_IM_FLG;
    if (cia[CIA_ICR] & CIA_IM_FLG) {
        my_set_int(MYCIA_INT, myclk);
    }
}

void mycia_set_sdr(BYTE data)
{
    cia[CIA_SDR] = data;
    ciaint |= CIA_IM_SDR;
    if (cia[CIA_ICR] & CIA_IM_SDR) {
        my_set_int(MYCIA_INT, myclk);
    }
}

/* ------------------------------------------------------------------------- */

static int int_ciatod(long offset)
{
    int t, pm;
    CLOCK rclk = myclk - offset;

#ifdef DEBUG
    if (cia_debugFlag)
	log_message(cia_log,
                    "TOD timer event (1/10 sec tick), tod=%02x:%02x,%02x.%x.",
                    cia[CIA_TOD_HR], cia[CIA_TOD_MIN], cia[CIA_TOD_SEC],
                    cia[CIA_TOD_TEN]);
#endif

    /* set up new int */
    alarm_set(&cia_tod_alarm, myclk + ciatodticks);

    if (!ciatodstopped) {
	/* inc timer */
	t = bcd2byte(cia[CIA_TOD_TEN]);
	t++;
	cia[CIA_TOD_TEN] = byte2bcd(t % 10);
	if (t >= 10) {
	    t = bcd2byte(cia[CIA_TOD_SEC]);
	    t++;
	    cia[CIA_TOD_SEC] = byte2bcd(t % 60);
	    if (t >= 60) {
		t = bcd2byte(cia[CIA_TOD_MIN]);
		t++;
		cia[CIA_TOD_MIN] = byte2bcd(t % 60);
		if (t >= 60) {
		    pm = cia[CIA_TOD_HR] & 0x80;
		    t = bcd2byte(cia[CIA_TOD_HR] & 0x1f);
		    if (!t)
			pm ^= 0x80;	/* toggle am/pm on 0:59->1:00 hr */
		    t++;
		    t = t % 12 | pm;
		    cia[CIA_TOD_HR] = byte2bcd(t);
		}
	    }
	}
#ifdef DEBUG
	if (cia_debugFlag)
	    log_message(cia_log,
                        "TOD after event :tod=%02x:%02x,%02x.%x.",
                        cia[CIA_TOD_HR], cia[CIA_TOD_MIN], cia[CIA_TOD_SEC],
                        cia[CIA_TOD_TEN]);
#endif
	/* check alarm */
	check_ciatodalarm(rclk);
    }
    return 0;
}

/* -------------------------------------------------------------------------- */


void mycia_prevent_clk_overflow(CLOCK sub)
{

    update_tai(myclk);
    update_tbi(myclk);

    update_cia(myclk);

    if(cia_tai && (cia_tai != -1))
        cia_tai -= sub;
    if(cia_tbi && (cia_tbi != -1))
        cia_tbi -= sub;

    if (cia_tau)
	cia_tau -= sub;
    if (cia_tbu)
	cia_tbu -= sub;
    if (ciardi > sub)
	ciardi -= sub;
    else
	ciardi = 0;
}

/* -------------------------------------------------------------------------- */

/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 0/0
 */

#define	CIA_DUMP_VER_MAJOR	1
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

    m = snapshot_module_create(p, "cia",
                               CIA_DUMP_VER_MAJOR, CIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    update_tai(myclk); /* schedule alarm in case latch value is changed */
    update_tbi(myclk); /* schedule alarm in case latch value is changed */
    update_cia(myclk);

#ifdef cia_DUMP_DEBUG
    log_message(cia_log, "clk=%d, cra=%02x, crb=%02x, tas=%d, tbs=%d",myclk, cia[CIA_CRA], cia[CIA_CRB],cia_tas, cia_tbs);
    log_message(cia_log, "tai=%d, tau=%d, tac=%04x, tal=%04x",cia_tai, cia_tau, cia_tac, cia_tal);
    log_message(cia_log, "tbi=%d, tbu=%d, tbc=%04x, tbl=%04x",cia_tbi, cia_tbu, cia_tbc, cia_tbl);
    log_message(cia_log, "write ciaint=%02x, ciaier=%02x", ciaint, ciaier);
#endif

    snapshot_module_write_byte(m, cia[CIA_PRA]);
    snapshot_module_write_byte(m, cia[CIA_PRB]);
    snapshot_module_write_byte(m, cia[CIA_DDRA]);
    snapshot_module_write_byte(m, cia[CIA_DDRB]);
    snapshot_module_write_word(m, cia_tac);
    snapshot_module_write_word(m, cia_tbc);
    snapshot_module_write_byte(m, cia[CIA_TOD_TEN]);
    snapshot_module_write_byte(m, cia[CIA_TOD_SEC]);
    snapshot_module_write_byte(m, cia[CIA_TOD_MIN]);
    snapshot_module_write_byte(m, cia[CIA_TOD_HR]);
    snapshot_module_write_byte(m, cia[CIA_SDR]);
    snapshot_module_write_byte(m, cia[CIA_ICR]);
    snapshot_module_write_byte(m, cia[CIA_CRA]);
    snapshot_module_write_byte(m, cia[CIA_CRB]);

    snapshot_module_write_word(m, cia_tal);
    snapshot_module_write_word(m, cia_tbl);
    snapshot_module_write_byte(m, peek_mycia(CIA_ICR));
    snapshot_module_write_byte(m, ((cia_tat ? 0x40 : 0)
                                   | (cia_tbt ? 0x80 : 0)));
    snapshot_module_write_byte(m, ciasr_bits);
    snapshot_module_write_byte(m, ciatodalarm[0]);
    snapshot_module_write_byte(m, ciatodalarm[1]);
    snapshot_module_write_byte(m, ciatodalarm[2]);
    snapshot_module_write_byte(m, ciatodalarm[3]);

    if(ciardi) {
	if((myclk - ciardi) > 120) {
	    byte = 0;
	} else {
	    byte = myclk + 128 - ciardi;
	}
    } else {
	byte = 0;
    }
    snapshot_module_write_byte(m, byte);

    snapshot_module_write_byte(m, ((ciatodlatched ? 1 : 0)
                                   | (ciatodstopped ? 2 : 0)));
    snapshot_module_write_byte(m, ciatodlatch[0]);
    snapshot_module_write_byte(m, ciatodlatch[1]);
    snapshot_module_write_byte(m, ciatodlatch[2]);
    snapshot_module_write_byte(m, ciatodlatch[3]);

    /* FIXME */
#if 0
    snapshot_module_write_dword(m, (mycpu_int_status.alarm_clk[A_ciaTOD]
                                    - myclk));
#endif

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

    m = snapshot_module_open(p, "cia", &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (vmajor != CIA_DUMP_VER_MAJOR) {
        snapshot_module_close(m);
        return -1;
    }

    /* stop timers, just in case */
    cia_tas = CIAT_STOPPED;
    cia_tau = 0;
    my_unset_tai();
    cia_tbs = CIAT_STOPPED;
    cia_tbu = 0;
    my_unset_tbi();
    alarm_unset(&cia_tod_alarm);

    {
        snapshot_module_read_byte(m, &cia[CIA_PRA]);
        snapshot_module_read_byte(m, &cia[CIA_PRB]);
        snapshot_module_read_byte(m, &cia[CIA_DDRA]);
        snapshot_module_read_byte(m, &cia[CIA_DDRB]);

        addr = CIA_DDRA;
	byte = cia[CIA_PRA] | ~cia[CIA_DDRA];
        oldpa = byte ^ 0xff;	/* all bits change? */
        undump_ciapa(rclk, byte);
        oldpa = byte;

        addr = CIA_DDRB;
	byte = cia[CIA_PRB] | ~cia[CIA_DDRB];
        oldpb = byte ^ 0xff;	/* all bits change? */
        undump_ciapb(rclk, byte);
        oldpb = byte;
    }

    snapshot_module_read_word(m, &word);
    cia_tac = word;
    snapshot_module_read_word(m, &word);
    cia_tbc = word;
    snapshot_module_read_byte(m, &cia[CIA_TOD_TEN]);
    snapshot_module_read_byte(m, &cia[CIA_TOD_SEC]);
    snapshot_module_read_byte(m, &cia[CIA_TOD_MIN]);
    snapshot_module_read_byte(m, &cia[CIA_TOD_HR]);
    snapshot_module_read_byte(m, &cia[CIA_SDR]);
    {
	store_sdr(cia[CIA_SDR]);
    }
    snapshot_module_read_byte(m, &cia[CIA_ICR]);
    snapshot_module_read_byte(m, &cia[CIA_CRA]);
    snapshot_module_read_byte(m, &cia[CIA_CRB]);

    snapshot_module_read_word(m, &word);
    cia_tal = word;
    snapshot_module_read_word(m, &word);
    cia_tbl = word;

    snapshot_module_read_byte(m, &byte);
    ciaint = byte;

#ifdef cia_DUMP_DEBUG
log_message(cia_log, "read ciaint=%02x, ciaier=%02x.", ciaint, ciaier);
#endif

    snapshot_module_read_byte(m, &byte);
    cia_tat = (byte & 0x40) ? 1 : 0;
    cia_tbt = (byte & 0x80) ? 1 : 0;
    cia_tap = (byte & 0x04) ? 1 : 0;
    cia_tbp = (byte & 0x08) ? 1 : 0;

    snapshot_module_read_byte(m, &byte);
    ciasr_bits = byte;

    snapshot_module_read_byte(m, &ciatodalarm[0]);
    snapshot_module_read_byte(m, &ciatodalarm[1]);
    snapshot_module_read_byte(m, &ciatodalarm[2]);
    snapshot_module_read_byte(m, &ciatodalarm[3]);

    snapshot_module_read_byte(m, &byte);
    if(byte) {
	ciardi = myclk + 128 - byte;
    } else {
	ciardi = 0;
    }
#ifdef cia_DUMP_DEBUG
log_message(cia_log, "snap read rdi=%02x", byte);
log_message(cia_log, "snap setting rdi to %d (rclk=%d)", ciardi, myclk);
#endif

    snapshot_module_read_byte(m, &byte);
    ciatodlatched = byte & 1;
    ciatodstopped = byte & 2;
    snapshot_module_read_byte(m, &ciatodlatch[0]);
    snapshot_module_read_byte(m, &ciatodlatch[1]);
    snapshot_module_read_byte(m, &ciatodlatch[2]);
    snapshot_module_read_byte(m, &ciatodlatch[3]);

    snapshot_module_read_dword(m, &dword);
    alarm_set(&cia_tod_alarm, myclk + dword);

    /* timer switch-on code from store_cia[CIA_CRA/CRB] */

#ifdef cia_DUMP_DEBUG
log_message(cia_log, "clk=%d, cra=%02x, crb=%02x, tas=%d, tbs=%d",myclk, cia[CIA_CRA], cia[CIA_CRB],cia_tas, cia_tbs);
log_message(cia_log, "tai=%d, tau=%d, tac=%04x, tal=%04x",cia_tai, cia_tau, cia_tac, cia_tal);
log_message(cia_log, "tbi=%d, tbu=%d, tbc=%04x, tbl=%04x",cia_tbi, cia_tbu, cia_tbc, cia_tbl);
#endif

    if ((cia[CIA_CRA] & 0x21) == 0x01) {        /* timer just started */
        cia_tas = CIAT_RUNNING;
        cia_tau = rclk + (cia_tac /*+ 1) + ((byte & 0x10) >> 4*/ );
        my_set_tai_clk(cia_tau + 1);
    }

    if ((cia[CIA_CRB] & 0x61) == 0x01) {        /* timer just started */
        cia_tbu = rclk + (cia_tbc /*+ 1) + ((byte & 0x10) >> 4*/ );
        my_set_tbi_clk(cia_tbu + 1);
        cia_tbs = CIAT_RUNNING;
    } else
    if ((cia[CIA_CRB] & 0x41) == 0x41) {
        cia_tbs = CIAT_COUNTTA;
        update_cia(rclk);
        /* switch timer A alarm on if necessary */
        if (cia_tau) {
            my_set_tai_clk(cia_tau + 1);
        }
    }

#ifdef cia_DUMP_DEBUG
log_message(cia_log, "clk=%d, cra=%02x, crb=%02x, tas=%d, tbs=%d",myclk, cia[CIA_CRA], cia[CIA_CRB],cia_tas, cia_tbs);
log_message(cia_log, "tai=%d, tau=%d, tac=%04x, tal=%04x",cia_tai, cia_tau, cia_tac, cia_tal);
log_message(cia_log, "tbi=%d, tbu=%d, tbc=%04x, tbl=%04x",cia_tbi, cia_tbu, cia_tbc, cia_tbl);
#endif

    if (cia[CIA_ICR] & 0x80) {
        cia_restore_int(MYCIA_INT);
    } else {
        cia_restore_int(0);
    }

    if (snapshot_module_close(m) < 0)
        return -1;

    return 0;
}


