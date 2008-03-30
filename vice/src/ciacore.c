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
 * 29jun1998 a.fachat
 *
 * Implementing the peek function assumes that the READ_PA etc macros
 * do not have side-effects, i.e. they can be called more than once
 * at one clock cycle.
 *
 */

/*
 * new, generalized timer code
 */
#include "clkguard.h"
#include "ciatimer.h"

static ciat_t ciata;
static ciat_t ciatb;

static CLOCK cia_read_clk = 0;
static int cia_read_offset = 0;
static BYTE cia_last_read = 0;	/* the byte read the last time (for RMW) */

/* Make the TOD count 50/60Hz even if we do not run at 1MHz ... */
#ifndef CYCLES_PER_SEC
#define	CYCLES_PER_SEC 	1000000
#endif

#if defined(CIA_TIMER_DEBUG) || defined(CIA_IO_DEBUG)
int mycia_debugFlag = 0;

#endif

/* The following is an attempt in rewriting the interrupt defines into 
   static inline functions. This should not hurt, but I still kept the
   define below, to be able to compare speeds. 
   The semantics of the call has changed, the interrupt number is
   not needed anymore (because it's known to my_set_int(). Actually
   one could also remove MYCIA_INT as it is also known... */

/* new semantics and as inline function, value can be replaced by 0/1 */
static inline void my_set_int(int value, CLOCK rclk)
{
#ifdef CIA_TIMER_DEBUG
    if(mycia_debugFlag) {
        log_message(cia_log, "set_int(rclk=%d, d=%d pc=).",
           rclk,(value));
    }
#endif
    if ((value)) {
        /* ciaint |= 0x80; */
        cia_set_int_clk((MYCIA_INT), (rclk));
    } else {
        cia_set_int_clk(0, (rclk));
    }
}


/* ------------------------------------------------------------------------- */
/* cia */


inline static void check_ciatodalarm(CLOCK rclk)
{
    if (!memcmp(ciatodalarm, cia + CIA_TOD_TEN, sizeof(ciatodalarm))) {
	ciaint |= CIA_IM_TOD;
	if (ciaier & CIA_IM_TOD) {
	    ciaint |= 0x80;
            my_set_int(MYCIA_INT, myclk);
	}
    }
}

/* ------------------------------------------------------------------------- */
/*
 * ciat_update return the number of underflows
 * FIXME: SDR count, etc 
 */

#ifdef INLINE_CIA_FUNCS
#  define _CIA_FUNC inline static
#else
#  define _CIA_FUNC static
#endif

_CIA_FUNC void cia_do_update_ta(CLOCK rclk)
{
    int n;

    if((n=ciat_update(&ciata, rclk))) {
        ciaint |= CIA_IM_TA;
	cia_tat = (cia_tat + n) & 1;
    }
}

_CIA_FUNC void cia_do_update_tb(CLOCK rclk)
{
    int n;

    if((n=ciat_update(&ciatb, rclk))) {
        ciaint |= CIA_IM_TB;
	cia_tbt = (cia_tbt + n) & 1;
    }
}

_CIA_FUNC void cia_do_step_tb(CLOCK rclk)
{
    int n;
    
    if((n=ciat_single_step(&ciatb, rclk))) {
        ciaint |= CIA_IM_TB;
	cia_tbt = (cia_tbt + n) & 1;
    }
}

/*
 * Those functions are called everywhere but in the alarm functions.
 */


_CIA_FUNC void cia_update_ta(CLOCK rclk)
{
    CLOCK tmp, last_tmp;

    last_tmp = 0;
    tmp = ciat_alarm_clk(&ciata);
    while (tmp <= rclk) {
        int_ciata(myclk - tmp);
	last_tmp = tmp;
        tmp = ciat_alarm_clk(&ciata);
    }

    if (last_tmp != rclk) {
	cia_do_update_ta(rclk);
    }
}

_CIA_FUNC void cia_update_tb(CLOCK rclk)
{
    CLOCK tmp, last_tmp;

    if( (cia[CIA_CRB] & 0x41) == 0x41 ) {
	cia_update_ta(rclk);
    }

    last_tmp = 0;
    tmp = ciat_alarm_clk(&ciatb);
    while (tmp <= rclk) {
        int_ciatb(myclk - tmp);
	last_tmp = tmp;
        tmp = ciat_alarm_clk(&ciatb);
    }

    if (last_tmp != rclk) {
	cia_do_update_tb(rclk);
    }
}

/*
 * set interrupt line 
 */
_CIA_FUNC void cia_do_set_int(CLOCK rclk)
{
    if ((ciardi != rclk - 1) || (MYCIA_INT == IK_NMI)) {
        if (ciaint & ciaier & 0x7f) {
            my_set_int(MYCIA_INT, rclk + 1);

	    if (ciardi != rclk) {
		ciaint |= 0x80;
	    }
        }
    }
}

/* ------------------------------------------------------------------------- */

static void clk_overflow_callback(CLOCK sub, void *data)
{
    /* we assume that sub has already been substracted from myclk */
    cia_update_ta(myclk + sub);
    cia_update_tb(myclk + sub);

    ciat_prevent_clock_overflow(&ciata, sub);
    ciat_prevent_clock_overflow(&ciatb, sub);

    if (ciardi > sub)
	ciardi -= sub;
    else
	ciardi = 0;

    if (cia_read_clk > sub)
	cia_read_clk -= sub;
    else
	cia_read_clk = 0;

    if (cia_todclk)
	cia_todclk -= sub;
}

/* -------------------------------------------------------------------------- */

void mycia_init(void)
{
    if (cia_log == LOG_ERR)
        cia_log = log_open(MYCIA_NAME);

    alarm_init(&cia_ta_alarm, &mycpu_alarm_context, MYCIA_NAME "_TA",
               int_ciata);
    alarm_init(&cia_tb_alarm, &mycpu_alarm_context, MYCIA_NAME "_TB",
               int_ciatb);
    alarm_init(&cia_tod_alarm, &mycpu_alarm_context, MYCIA_NAME "_TOD",
               int_ciatod);

    clk_guard_add_callback(&mycpu_clk_guard, clk_overflow_callback, NULL);

    ciat_init(&ciata, MYCIA_NAME "_TA", myclk, &cia_ta_alarm);
    ciat_init(&ciatb, MYCIA_NAME "_TB", myclk, &cia_tb_alarm);
}

void mycia_reset(void)
{
    int i;

    ciatodticks = CYCLES_PER_SEC / 10;  /* cycles per tenth of a second */

    for (i = 0; i < 16; i++)
	cia[i] = 0;

    ciardi = 0;
    ciasr_bits = 0;
    cia_read_clk = 0;

    ciat_reset(&ciata, myclk);
    ciat_reset(&ciatb, myclk);

    memset(ciatodalarm, 0, sizeof(ciatodalarm));
    ciatodlatched = 0;
    ciatodstopped = 0;
    cia_todclk = myclk + ciatodticks;
    alarm_set(&cia_tod_alarm, cia_todclk);

    ciaint = 0;
    my_set_int(0, myclk);

    oldpa = 0xff;
    oldpb = 0xff;

    do_reset_cia();
}


void REGPARM2 store_mycia(ADDRESS addr, BYTE byte)
{
    CLOCK rclk;

    if (mycpu_rmw_flag) {
	myclk --;
	mycpu_rmw_flag = 0;
	store_mycia(addr, cia_last_read);
	myclk ++;
    }

    addr &= 0xf;

    PRE_STORE_CIA

    rclk = myclk - STORE_OFFSET;

#ifdef CIA_TIMER_DEBUG
    if (mycia_debugFlag)
	log_message(cia_log, "store cia[%02x] %02x @ clk=%d",
                    (int) addr, (int) byte, rclk);
#endif

    switch (addr) {

      case CIA_PRA:		/* port A */
      case CIA_DDRA:
	cia[addr] = byte;
	byte = cia[CIA_PRA] | ~cia[CIA_DDRA];
	if(byte != oldpa) {
	    store_ciapa(rclk, byte);
	    oldpa = byte;
	}
	break;

      case CIA_PRB:		/* port B */
      case CIA_DDRB:
	cia[addr] = byte;
	byte = cia[CIA_PRB] | ~cia[CIA_DDRB];
	if ((cia[CIA_CRA] | cia[CIA_CRB]) & 0x02) {
	    if (cia[CIA_CRA] & 0x02) {
	        cia_update_ta(rclk);
		byte &= 0xbf;
		if ( ((cia[CIA_CRA] & 0x04) ? cia_tat 
				: ciat_is_underflow_clk(&ciata, rclk)) )
		    byte |= 0x40;
	    }
	    if (cia[CIA_CRB] & 0x02) {
	        cia_update_tb(rclk);
		byte &= 0x7f;
		if ( ((cia[CIA_CRB] & 0x04) ? cia_tbt 
			: ciat_is_underflow_clk(&ciatb, rclk) ) )
		    byte |= 0x80;
	    }
	}
	if(byte != oldpb) {
	    store_ciapb(rclk, byte);
	    oldpb = byte;
	}
	if(addr == CIA_PRB) {
	    pulse_ciapc(rclk);
 	}
	break;

      case CIA_TAL:
	cia_update_ta(rclk);
	ciat_set_latchlo(&ciata, rclk, byte);
	break;
      case CIA_TBL:
	cia_update_tb(rclk);
	ciat_set_latchlo(&ciatb, rclk, byte);
	break;
      case CIA_TAH:
	cia_update_ta(rclk);
	ciat_set_latchhi(&ciata, rclk, byte);
	break;
      case CIA_TBH:
	cia_update_tb(rclk);
	ciat_set_latchhi(&ciatb, rclk, byte);
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
	/* Flip AM/PM on hour 12 
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
/* FIXME
	            update_cia(rclk);
	            if (cia_tau) {
		        my_set_tai_clk(cia_tau + 1);
	            }
*/
		}

	        ciasr_bits += 16;

#if defined (CIA_TIMER_DEBUG)
	        if (mycia_debugFlag)
	    	    log_message(cia_log, "start SDR rclk=%d.", rclk);
#endif
  	    }
	}
	break;

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Control Register */

        CIAT_LOGIN(("store_icr: rclk=%d, byte=%02x", rclk, byte));

	cia_update_ta(rclk);
	cia_update_tb(rclk);

#if defined (CIA_TIMER_DEBUG)
	if (mycia_debugFlag)
	    log_message(cia_log, "cia set CIA_ICR: 0x%x.", byte);
#endif

	if (byte & CIA_IM_SET) {
	    ciaier |= (byte & 0x7f);
	} else {
	    ciaier &= ~(byte & 0x7f);
	}

#if defined(CIA_TIMER_DEBUG)
	if (mycia_debugFlag)
	    log_message(cia_log, "    set icr: ifr & ier & 0x7f -> %02x, int=%02x.",
                        ciaier & ciaint & 0x7f, ciaint);
#endif
	if (ciaier & ciaint & 0x7f) {
	   cia_do_set_int(rclk + 1);
	}

	if (ciaier & CIA_IM_TA) {
	    ciat_set_alarm(&ciata, rclk);
	}
	if (ciaier & CIA_IM_TB) {
	    ciat_set_alarm(&ciatb, rclk);
	}

	CIAT_LOGOUT((""));
	break;

      case CIA_CRA:		/* control register A */
	if( (byte & 1) && !(cia[CIA_CRA] & 1)) cia_tat = 1;

	cia_update_ta(rclk);

	ciat_set_ctrl(&ciata, rclk, byte);

#if defined (CIA_TIMER_DEBUG)
	if (mycia_debugFlag)
	    log_message(cia_log, "cia set CIA_CRA: 0x%x (clk=%d, pc=, tal=%u, tac=%u).",
		   byte, rclk, /*program_counter,*/ cia_tal, cia_tac);
#endif

	/* bit 7 tod frequency */
	/* bit 6 serial port mode */

	/* bit 3 timer run mode */
	/* bit 2 & 1 timer output to PB6 */

	/* bit 0 start/stop timer */
	/* bit 5 timer count mode */

#if defined (CIA_TIMER_DEBUG)
	if (mycia_debugFlag)
	    log_message(cia_log, "    -> tas=%d, tau=%d.", cia_tas, cia_tau);
#endif
	cia[addr] = byte & 0xef;	/* remove strobe */

	break;

      case CIA_CRB:		/* control register B */
	if( (byte & 1) && !(cia[CIA_CRB] & 1)) cia_tbt = 1;

	cia_update_ta(rclk);
	cia_update_tb(rclk);

	/* bit 5 is set when single-stepping is set */
	if (byte & 0x40) {
	    /* we count ta - so we enable that */
	    ciat_set_alarm(&ciata, rclk);
	    ciat_set_ctrl(&ciatb, rclk, byte | 0x20);
	} else {
	    ciat_set_ctrl(&ciatb, rclk, byte);
	}

#if 0 /* defined (CIA_TIMER_DEBUG)
	if (mycia_debugFlag) */
	    log_message(cia_log, "cia set CIA_CRB: 0x%x (clk=%d).",
		   byte, rclk);
#endif


	/* bit 7 set alarm/tod clock */
	/* bit 4 force load */

	/* bit 3 timer run mode */
	/* bit 2 & 1 timer output to PB6 */

	/* bit 0 stbrt/stop timer */
	/* bit 5 & 6 timer count mode */

	cia[addr] = byte & 0xef;	/* remove strobe */
	break;

      default:
	cia[addr] = byte;
    }				/* switch */
}


/* ------------------------------------------------------------------------- */


BYTE REGPARM1 read_mycia(ADDRESS addr)
{
#if defined( CIA_TIMER_DEBUG )

    BYTE read_cia_(ADDRESS addr);
    BYTE tmp = read_cia_(addr);

    if (mycia_debugFlag)
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

    /* Hack for opcode fetch, where the clock does not change */
    if (myclk <= cia_read_clk) {
	rclk = cia_read_clk + (++cia_read_offset) - READ_OFFSET;
    } else {
        cia_read_clk = myclk;
	cia_read_offset = 0;
        rclk = myclk - READ_OFFSET;
    }

    PRE_READ_CIA

    switch (addr) {

      case CIA_PRA:		/* port A */
        /* WARNING: this pin reads the voltage of the output pins, not
           the ORA value. Value read might be different from what is 
	   expected due to excessive load. */
	cia_last_read = read_ciapa();
	return cia_last_read;
	break;

      case CIA_PRB:		/* port B */
        /* WARNING: this pin reads the voltage of the output pins, not
           the ORA value. Value read might be different from what is 
	   expected due to excessive load. */
	byte = read_ciapb();
	pulse_ciapc(rclk);
        if ((cia[CIA_CRA] | cia[CIA_CRB]) & 0x02) {
	    if (cia[CIA_CRA] & 0x02) {
	        cia_update_ta(rclk);
		byte &= 0xbf;
		if ( ((cia[CIA_CRA] & 0x04) ? cia_tat 
			: ciat_is_underflow_clk(&ciata, rclk) ) )
		    byte |= 0x40;
	    }
	    if (cia[CIA_CRB] & 0x02) {
	        cia_update_tb(rclk);
		byte &= 0x7f;
		if ( ((cia[CIA_CRB] & 0x04) ? cia_tbt 
			: ciat_is_underflow_clk(&ciatb, rclk) ) )
		    byte |= 0x80;
	    }
	}
	cia_last_read = byte;
	return byte;
	break;

	/* Timers */
      case CIA_TAL:		/* timer A low */
	cia_update_ta(rclk);
	cia_last_read = ciat_read_timer(&ciata, rclk) & 0xff;
	return cia_last_read;
	break;

      case CIA_TAH:		/* timer A high */
	cia_update_ta(rclk);
	cia_last_read = (ciat_read_timer(&ciata, rclk) >> 8) & 0xff;
	return cia_last_read;
	break;

      case CIA_TBL:		/* timer B low */
	cia_update_tb(rclk);
	cia_last_read = ciat_read_timer(&ciatb, rclk) & 0xff;
	return cia_last_read;
	break;

      case CIA_TBH:		/* timer B high */
	cia_update_tb(rclk);
	cia_last_read = (ciat_read_timer(&ciatb, rclk) >> 8) & 0xff;
	return cia_last_read;
	break;

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
	cia_last_read = ciatodlatch[addr - CIA_TOD_TEN];
	return cia_last_read;
	break;

      case CIA_SDR:		/* Serial Port Shift Register */
	cia_last_read = cia[CIA_SDR];
	return cia_last_read;
	break;

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Flag Register */
	{
	    BYTE t = 0;

	    CIAT_LOGIN(("read_icr: rclk=%d, rdi=%d", rclk, ciardi));

	    ciardi = rclk;

	    cia_update_ta(rclk);
	    cia_update_tb(rclk);

	    read_ciaicr();

#ifdef CIA_TIMER_DEBUG
	    if (mycia_debugFlag)
		log_message(cia_log, "cia read intfl: rclk=%d, alarm_ta=%d, alarm_tb=%d, ciaint=%02x",
			rclk, cia_tai, cia_tbi, (int)ciaint);
#endif

	    ciat_set_alarm(&ciata, rclk);
	    ciat_set_alarm(&ciatb, rclk);

            CIAT_LOG(("read_icr -> ta alarm at %d, tb at %d", 
		ciat_alarm_clk(&ciata), ciat_alarm_clk(&ciatb)));

	    t = ciaint;

	    CIAT_LOG(( "read intfl gives ciaint=%02x -> %02x "
                            "sr_bits=%d, clk=%d",
                            ciaint, t, ciasr_bits, clk));

	    ciaint = 0;
	    my_set_int(0, rclk + 1);

	    CIAT_LOGOUT((""));

	    cia_last_read = t;

	    return (t);
	}
	break;

      case CIA_CRA:		/* Control Register A */
	cia_update_ta(rclk);
	cia_last_read = (cia[CIA_CRA] & 0xfe) | ciat_is_running(&ciata, rclk);
	return cia_last_read;
	break;

      case CIA_CRB:		/* Control Register B */
	cia_update_tb(rclk);
	cia_last_read = (cia[CIA_CRB] & 0xfe) | ciat_is_running(&ciatb, rclk);
	return cia_last_read;
	break;
    }				/* switch */

    cia_last_read = cia[addr];
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
    BYTE byte;

    addr &= 0xf;

    PRE_PEEK_CIA

    rclk = myclk - READ_OFFSET;

    switch (addr) {

      case CIA_PRB:		/* port B */
        /* WARNING: this pin reads the voltage of the output pins, not
           the ORA value. Value read might be different from what is 
	   expected due to excessive load. */
	byte = read_ciapb();
	/* pulse_ciapc(rclk); */
        if ((cia[CIA_CRA] | cia[CIA_CRB]) & 0x02) {
	    if (cia[CIA_CRA] & 0x02) {
	        cia_update_ta(rclk);
		byte &= 0xbf;
		if ( ((cia[CIA_CRA] & 0x04) ? cia_tat 
			: ciat_is_underflow_clk(&ciata, rclk) ) )
		    byte |= 0x40;
	    }
	    if (cia[CIA_CRB] & 0x02) {
	        cia_update_tb(rclk);
		byte &= 0x7f;
		if ( ((cia[CIA_CRB] & 0x04) ? cia_tbt 
			: ciat_is_underflow_clk(&ciatb, rclk) ) )
		    byte |= 0x80;
	    }
	}
	return byte;
	break;

	/*
	 * TOD clock is latched by reading Hours, and released
	 * upon reading Tenths of Seconds. The counter itself
	 * keeps ticking all the time.
	 * Also note that this latching is different from the input one.
	 */
      case CIA_TOD_TEN:	/* Time Of Day clock 1/10 s */
      case CIA_TOD_SEC:	/* Time Of Day clock sec */
      case CIA_TOD_MIN:	/* Time Of Day clock min */
      case CIA_TOD_HR:	/* Time Of Day clock hour */
	if (!ciatodlatched)
	    memcpy(ciatodlatch, cia + CIA_TOD_TEN, sizeof(ciatodlatch));
	return cia[addr];

	/* Interrupts */

      case CIA_ICR:		/* Interrupt Flag Register */
	{
	    BYTE t = 0;

	    CIAT_LOGIN(("peek_icr: rclk=%d, rdi=%d", rclk, ciardi));

	    /* ciardi = rclk; */

	    cia_update_ta(rclk);
	    cia_update_tb(rclk);

	    /* read_ciaicr(); */

#ifdef CIA_TIMER_DEBUG
	    if (mycia_debugFlag)
		log_message(cia_log, "cia read intfl: rclk=%d, alarm_ta=%d, alarm_tb=%d, ciaint=%02x",
			rclk, cia_tai, cia_tbi, (int)ciaint);
#endif

	    ciat_set_alarm(&ciata, rclk);
	    ciat_set_alarm(&ciatb, rclk);

            CIAT_LOG(("peek_icr -> ta alarm at %d, tb at %d", 
		ciat_alarm_clk(&ciata), ciat_alarm_clk(&ciatb)));

	    t = ciaint;

	    CIAT_LOG(( "peek intfl gives ciaint=%02x -> %02x "
                            "sr_bits=%d, clk=%d",
                            ciaint, t, ciasr_bits, clk));
/*
	    ciaint = 0;
	    my_set_int(0, rclk + 1);
*/
	    CIAT_LOGOUT((""));

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
/*    int n; */

    CIAT_LOGIN(("ciaTimerA int_ciata: myclk=%d rclk=%d", myclk, rclk));

#if 0
    if( (n=ciat_update(&ciata, rclk)) && (ciardi != rclk-1)) {
	ciaint |= CIA_IM_TA;
	cia_tat = (cia_tat + n) & 1;
    }
#else
    cia_do_update_ta(rclk);
#endif

    ciat_ack_alarm(&ciata, rclk);

    CIAT_LOG((
          "int_ciata(rclk = %u, tal = %u, cra=%02x, int=%02x, ier=%02x.",
          rclk, ciat_read_latch(&ciata, rclk), cia[CIA_CRA], ciaint, ciaier));

    /* cia_tat = (cia_tat + 1) & 1; */

    if ( (cia[CIA_CRA] & 0x29) == 0x01 ) {
	/* if we do not need alarm, no PB6, no shift register, and not timer B
	   counting timer A, then we can savely skip alarms... */
	if ( ( (ciaier & CIA_IM_TA) &&
		(!(ciaint & 0x80)) )
	    || (cia[CIA_CRA] & 0x42)
	    || (cia[CIA_CRB] & 0x40) ) {
	    ciat_set_alarm(&ciata, rclk);
	}
    }

    if (cia[CIA_CRA] & 0x40) {
	if (ciasr_bits) {
	    CIAT_LOG(("rclk=%d SDR: timer A underflow, bits=%d",
		       rclk, ciasr_bits));

	    if (!(--ciasr_bits)) {
		ciaint |= CIA_IM_SDR;
	    }
	    if(ciasr_bits == 16) {
		store_sdr(cia[CIA_SDR]);
	    }
	}
    }
    if ((cia[CIA_CRB] & 0x41) == 0x41) {
        cia_update_tb(rclk);
	cia_do_step_tb(rclk);
    }

    cia_do_set_int(rclk);

    CIAT_LOGOUT((""));

/* if(ciaint == 0x80) ciaint = *((BYTE*)0); */

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
    int n;

    CIAT_LOGIN(("ciaTimerB int_myciatb: myclk=%d, rclk=%d", myclk,rclk));

#if 1
    if( (n=ciat_update(&ciatb, rclk)) && (ciardi != rclk-1)) {
	ciaint |= CIA_IM_TB;
	cia_tbt = (cia_tbt + n) & 1;
    }
#else
    cia_do_update_tb(rclk);
#endif

    ciat_ack_alarm(&ciatb, rclk);

    CIAT_LOG((
            "timer B int_ciatb(rclk=%d, crb=%d, int=%02x, ier=%02x).", 
		rclk, cia[CIA_CRB], ciaint, ciaier));

    /* cia_tbt = (cia_tbt + 1) & 1; */

    /* running and continous, then next alarm */
    if ( (cia[CIA_CRB] & 0x69) == 0x01 ) {
	/* if no interrupt flag we can safely skip alarms */
	if (ciaier & CIA_IM_TB) {
	    ciat_set_alarm(&ciatb, rclk);
	}
    }

    cia_do_set_int(rclk);

    CIAT_LOGOUT((""));

    return 0;
}

/* ------------------------------------------------------------------------- */

void mycia_set_flag(void)
{
    ciaint |= CIA_IM_FLG;
    if (ciaier & CIA_IM_FLG) {
        my_set_int(MYCIA_INT, myclk);
    }
}

void mycia_set_sdr(BYTE data)
{
    cia[CIA_SDR] = data;
    ciaint |= CIA_IM_SDR;
    if (ciaier & CIA_IM_SDR) {
        my_set_int(MYCIA_INT, myclk);
    }
}

/* ------------------------------------------------------------------------- */

static int int_ciatod(long offset)
{
    int t, pm;
    CLOCK rclk = myclk - offset;

#ifdef DEBUG
    if (mycia_debugFlag)
	log_message(cia_log,
                    "TOD timer event (1/10 sec tick), tod=%02x:%02x,%02x.%x.",
                    cia[CIA_TOD_HR], cia[CIA_TOD_MIN], cia[CIA_TOD_SEC],
                    cia[CIA_TOD_TEN]);
#endif

    /* set up new int */
    cia_todclk = myclk + ciatodticks;
    alarm_set(&cia_tod_alarm, cia_todclk);

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
		    t = cia[CIA_TOD_HR] & 0x1f;
		    if (t == 0x11) {
			pm ^= 0x80;	/* toggle am/pm on 0:59->1:00 hr */
		    }
		    if (t == 0x12) {
			t = 1;
		    } else {
			t ++;
		    }
		    t &= 0x1f;
		    cia[CIA_TOD_HR] = t | pm;
		}
	    }
	}
#ifdef DEBUG
	if (mycia_debugFlag)
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

/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 1.1
 */

#define	CIA_DUMP_VER_MAJOR	1
#define	CIA_DUMP_VER_MINOR	1

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
 *
 *				These bits have been added in V1.1
 *
 * WORD		TA		Timer A state bits (see ciatimer.h)
 * WORD		TB		Timer B state bits (see ciatimer.h)
 */

/* FIXME!!!  Error check.  */
int mycia_write_snapshot_module(snapshot_t *p)
{
    snapshot_module_t *m;
    int byte;

    cia_update_ta(myclk);
    cia_update_tb(myclk);

    m = snapshot_module_create(p, MYCIA_NAME,
                               CIA_DUMP_VER_MAJOR, CIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;


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
    snapshot_module_write_word(m, ciat_read_timer(&ciata, myclk));
    snapshot_module_write_word(m, ciat_read_timer(&ciatb, myclk));
    snapshot_module_write_byte(m, cia[CIA_TOD_TEN]);
    snapshot_module_write_byte(m, cia[CIA_TOD_SEC]);
    snapshot_module_write_byte(m, cia[CIA_TOD_MIN]);
    snapshot_module_write_byte(m, cia[CIA_TOD_HR]);
    snapshot_module_write_byte(m, cia[CIA_SDR]);
    snapshot_module_write_byte(m, ciaier);
    snapshot_module_write_byte(m, cia[CIA_CRA]);
    snapshot_module_write_byte(m, cia[CIA_CRB]);

    snapshot_module_write_word(m, ciat_read_latch(&ciata, myclk));
    snapshot_module_write_word(m, ciat_read_latch(&ciatb, myclk));
    snapshot_module_write_byte(m, peek_mycia(CIA_ICR));

    /* Bits 2 & 3 are compatibility to snapshot format v1.0 */
    snapshot_module_write_byte(m, ((cia_tat ? 0x40 : 0)
                           | (cia_tbt ? 0x80 : 0)
			   | (ciat_is_underflow_clk(&ciata, myclk) ? 0x04 : 0)
			   | (ciat_is_underflow_clk(&ciatb, myclk) ? 0x08 : 0)
			));
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

    snapshot_module_write_dword(m, (cia_todclk - myclk));

    ciat_save_snapshot(&ciata, myclk, m, 
			(CIA_DUMP_VER_MAJOR << 8) | CIA_DUMP_VER_MINOR);
    ciat_save_snapshot(&ciatb, myclk, m, 
			(CIA_DUMP_VER_MAJOR << 8) | CIA_DUMP_VER_MINOR);

    snapshot_module_close(m);

    return 0;
}

int mycia_read_snapshot_module(snapshot_t *p)
{
    BYTE vmajor, vminor;
    BYTE byte;
    DWORD dword;
    ADDRESS addr;
    CLOCK rclk = myclk;
    snapshot_module_t *m;
    WORD cia_tal, cia_tbl, cia_tac, cia_tbc;

    m = snapshot_module_open(p, MYCIA_NAME, &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (vmajor != CIA_DUMP_VER_MAJOR) {
        log_error(cia_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  vmajor, vminor, CIA_DUMP_VER_MAJOR, CIA_DUMP_VER_MINOR);
        snapshot_module_close(m);
        return -1;
    }

    /* stop timers, just in case */
    ciat_set_ctrl(&ciata, myclk, 0);
    ciat_set_ctrl(&ciatb, myclk, 0);
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

    snapshot_module_read_word(m, &cia_tac);
    snapshot_module_read_word(m, &cia_tbc);
    snapshot_module_read_byte(m, &cia[CIA_TOD_TEN]);
    snapshot_module_read_byte(m, &cia[CIA_TOD_SEC]);
    snapshot_module_read_byte(m, &cia[CIA_TOD_MIN]);
    snapshot_module_read_byte(m, &cia[CIA_TOD_HR]);
    snapshot_module_read_byte(m, &cia[CIA_SDR]);
    {
	store_sdr(cia[CIA_SDR]);
    }
    snapshot_module_read_byte(m, &ciaier);
    snapshot_module_read_byte(m, &cia[CIA_CRA]);
    snapshot_module_read_byte(m, &cia[CIA_CRB]);

    snapshot_module_read_word(m, &cia_tal);
    snapshot_module_read_word(m, &cia_tbl);

    snapshot_module_read_byte(m, &byte);
    ciaint = byte;

#ifdef cia_DUMP_DEBUG
log_message(cia_log, "read ciaint=%02x, ciaier=%02x.", ciaint, ciaier);
#endif

    snapshot_module_read_byte(m, &byte);
    cia_tat = (byte & 0x40) ? 1 : 0;
    cia_tbt = (byte & 0x80) ? 1 : 0;

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
    cia_todclk = myclk + dword;
    alarm_set(&cia_tod_alarm, cia_todclk);

    /* timer switch-on code from store_cia[CIA_CRA/CRB] */

#ifdef cia_DUMP_DEBUG
log_message(cia_log, "clk=%d, cra=%02x, crb=%02x, tas=%d, tbs=%d",myclk, cia[CIA_CRA], cia[CIA_CRB],cia_tas, cia_tbs);
log_message(cia_log, "tai=%d, tau=%d, tac=%04x, tal=%04x",cia_tai, cia_tau, cia_tac, cia_tal);
log_message(cia_log, "tbi=%d, tbu=%d, tbc=%04x, tbl=%04x",cia_tbi, cia_tbu, cia_tbc, cia_tbl);
#endif

    ciat_load_snapshot(&ciata, rclk, cia_tac, cia_tal, cia[CIA_CRA], m,
						(vmajor << 8) | vminor);
    ciat_load_snapshot(&ciatb, rclk, cia_tbc, cia_tbl, cia[CIA_CRB], m,
						(vmajor << 8) | vminor);

#ifdef cia_DUMP_DEBUG
log_message(cia_log, "clk=%d, cra=%02x, crb=%02x, tas=%d, tbs=%d",myclk, cia[CIA_CRA], cia[CIA_CRB],cia_tas, cia_tbs);
log_message(cia_log, "tai=%d, tau=%d, tac=%04x, tal=%04x",cia_tai, cia_tau, cia_tac, cia_tal);
log_message(cia_log, "tbi=%d, tbu=%d, tbc=%04x, tbl=%04x",cia_tbi, cia_tbu, cia_tbc, cia_tbl);
#endif

    if (ciaier & 0x80) {
        cia_restore_int(MYCIA_INT);
    } else {
        cia_restore_int(0);
    }

    if (snapshot_module_close(m) < 0)
        return -1;

    return 0;
}


