/*
 * viacore.c - Core functions for VIA emulation.
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
 * into one interrupt flag, I_MYVIAFL.
 * if an interrupt condition changes, the function (i.e. cpp macro)
 * update_myviairq() id called, that checks the IRQ line state.
 * This is now possible, as ettore has decoupled A_* alarm events
 * from I_* interrupts for performance reasons.
 *
 * A new function for signaling rising/falling edges on the
 * control lines is introduced:
 *      myvia_signal(VIA_SIG_[CA1|CA2|CB1|CB2], VIA_SIG_[RISE|FALL])
 * which signals the corresponding edge to the VIA. The constants
 * are defined in via.h.
 *
 * Except for shift register and input latching everything should be ok now.
 */

				/* Timer debugging */
/*#define MYVIA_TIMER_DEBUG */
				/* when PB7 is really used, set this
				   to enable pulse output from the timer.
				   Otherwise PB7 state is computed only
				   when port B is read -
				   not yet implemented */
/*#define MYVIA_NEED_PB7 */
				/* When you really need latching, define this.
				   It implies additional READ_PR* when
				   writing the snapshot. When latching is 
				   enabled: it reads the port when enabling,
				   and when an active C*1 transition occurs. 
				   It does not read the port when reading the
				   port register. Side-effects beware! */
/*#define MYVIA_NEED_LATCHING */

/*
 * local prototypes
 */

static int int_myviat1(long offset);
static int int_myviat2(long offset);

/*
 * local variables
 */

static CLOCK via_read_clk = 0;
static int via_read_offset = 0;
static BYTE via_last_read = 0;  /* the byte read the last time (for RMW) */

/*
 * local functions
 */

#define IS_CA2_OUTPUT()          ((myvia[VIA_PCR] & 0x0c) == 0x0c)
#define IS_CA2_INDINPUT()        ((myvia[VIA_PCR] & 0x0a) == 0x02)
#define IS_CA2_HANDSHAKE()       ((myvia[VIA_PCR] & 0x0c) == 0x08)
#define IS_CA2_PULSE_MODE()      ((myvia[VIA_PCR] & 0x0e) == 0x09)
#define IS_CA2_TOGGLE_MODE()     ((myvia[VIA_PCR] & 0x0e) == 0x08)

#define IS_CB2_OUTPUT()          ((myvia[VIA_PCR] & 0xc0) == 0xc0)
#define IS_CB2_INDINPUT()        ((myvia[VIA_PCR] & 0xa0) == 0x20)
#define IS_CB2_HANDSHAKE()       ((myvia[VIA_PCR] & 0xc0) == 0x80)
#define IS_CB2_PULSE_MODE()      ((myvia[VIA_PCR] & 0xe0) == 0x90)
#define IS_CB2_TOGGLE_MODE()     ((myvia[VIA_PCR] & 0xe0) == 0x80)

#define	IS_PA_INPUT_LATCH()	 (myvia[VIA_ACR] & 0x01)
#define	IS_PB_INPUT_LATCH()	 (myvia[VIA_ACR] & 0x02)

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
 myviat*u* clk ------------------------------------------+   +--------
                                                     |
                                                     |
                                                  call of
			                        int_myvia*
                                                   here

   real myviatau value = myviatau* + TAUOFFSET
   myviatbu = myviatbu* + 0

 *
 * IRQ and PB7 are set/toggled at the low-high transition of Phi2,
 * but int_* is called a half-cycle before that. Does that matter?
 *
 * PB7 output is still to be implemented
 */

/* timer values do not depend on a certain value here, but PB7 does... */
#define	TAUOFFSET	(-1)


#ifndef via_restore_int	/* if VIA reports to other chip (TPI) for IRQ */
#define	via_restore_int(a)  set_int_noclk(&mycpu_int_status, I_MYVIAFL, \
		(a) ? MYVIA_INT : 0)
#endif

static void clk_overflow_callback(CLOCK sub, void *data);


inline static void update_myviairq(void)
{
    via_set_int(I_MYVIAFL, (myviaifr & myviaier & 0x7f) ? MYVIA_INT : 0);
}

/* the next two are used in read_myvia() */

inline static unsigned int myviata(void)
{
    if (myclk < myviatau - TAUOFFSET)
        return myviatau - TAUOFFSET - myclk - 2;
    else
	return (myviatal - (myclk - myviatau + TAUOFFSET) % (myviatal + 2));
}

inline static unsigned int myviatb(void)
{
    return myviatbu - myclk - 2;
}

inline static void update_myviatal(CLOCK rclk)
{
    myviapb7x = 0;
    myviapb7xx = 0;

    if (rclk > myviatau) {
	int nuf = (myviatal + 1 + rclk - myviatau) / (myviatal + 2);

	if (!(myvia[VIA_ACR] & 0x40)) {
	    if (((nuf - myviapb7sx) > 1) || (!myviapb7)) {
		myviapb7o = 1;
		myviapb7sx = 0;
	    }
	}
	myviapb7 ^= (nuf & 1);

	myviatau = TAUOFFSET + myviatal + 2 + (rclk - (rclk - myviatau + TAUOFFSET) % (myviatal + 2));
	if (rclk == myviatau - myviatal - 1) {
	    myviapb7xx = 1;
	}
    }

    if (myviatau == rclk)
	myviapb7x = 1;

    myviatal = myvia[VIA_T1LL] + (myvia[VIA_T1LH] << 8);
}

inline static void update_myviatbl(void)
{
    myviatbl = myvia[VIA_T2CL] + (myvia[VIA_T2CH] << 8);
}


/* ------------------------------------------------------------------------- */
/* MYVIA */

void myvia_init(void)
{
    if (myvia_log == LOG_ERR)
        myvia_log = log_open(snap_module_name);

    alarm_init(&myvia_t1_alarm, &mycpu_alarm_context,
               MYVIA_NAME "T1", int_myviat1);
    alarm_init(&myvia_t2_alarm, &mycpu_alarm_context,
               MYVIA_NAME "T2", int_myviat2);
    clk_guard_add_callback(&mycpu_clk_guard, clk_overflow_callback, NULL);
}

/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void reset_myvia(void)
{
    int i;

    /* clear registers */
    for (i = 0; i < 4; i++)
	myvia[i] = 0;
    for (i = 4; i < 10; i++)
        myvia[i] = 0xff;        /* AB 98.08.23 */
    for (i = 11; i < 16; i++)
	myvia[i] = 0;

    myviatal = 0;
    myviatbl = 0;
    myviatau = myclk;
    myviatbu = myclk;

    via_read_clk = 0;

    myviaier = 0;
    myviaifr = 0;

    /* disable vice interrupts */
    myviatai = 0;
    myviatbi = 0;
    alarm_unset(&myvia_t1_alarm);
    alarm_unset(&myvia_t2_alarm);
    update_myviairq();

    oldpa = 0xff;
    oldpb = 0xff;

    ca2_state = 1;
    cb2_state = 1;
    VIA_SET_CA2( ca2_state )	/* input = high */
    VIA_SET_CB2( cb2_state )	/* input = high */

    res_via();
}

void myvia_signal(int line, int edge)
{
    switch (line) {
      case VIA_SIG_CA1:
	if ( (edge ? 1 : 0) == (myvia[VIA_PCR] & 0x01) ) {
	    if (IS_CA2_TOGGLE_MODE() && !ca2_state) {
		ca2_state = 1;
		VIA_SET_CA2( ca2_state )
	    }
            myviaifr |= VIA_IM_CA1;
            update_myviairq();
#ifdef MYVIA_NEED_LATCHING
	    if (IS_PA_INPUT_LATCH()) {
		myvia_ila = read_pra(addr);
	    }
#endif
	}
        break;
      case VIA_SIG_CA2:
        if (!(myvia[VIA_PCR] & 0x08)) {
            myviaifr |= (((edge << 2) ^ myvia[VIA_PCR]) & 0x04) ?
                0 : VIA_IM_CA2;
            update_myviairq();
        }
        break;
      case VIA_SIG_CB1:
	if ( (edge ? 0x10 : 0) == (myvia[VIA_PCR] & 0x10) ) {
	    if (IS_CB2_TOGGLE_MODE() && !cb2_state) {
		cb2_state = 1;
		VIA_SET_CB2( cb2_state )
	    }
            myviaifr |= VIA_IM_CB1;
            update_myviairq();
#ifdef MYVIA_NEED_LATCHING
	    if (IS_PB_INPUT_LATCH()) {
		myvia_ilb = read_prb();
	    }
#endif	
	}
        break;
      case VIA_SIG_CB2:
        if (!(myvia[VIA_PCR] & 0x80)) {
            myviaifr |= (((edge << 6) ^ myvia[VIA_PCR]) & 0x40) ?
                0 : VIA_IM_CB2;
            update_myviairq();
        }
        break;
    }
}

void REGPARM2 store_myvia(ADDRESS addr, BYTE byte)
{
    CLOCK rclk;

    if (mycpu_rmw_flag) {
        myclk --;
        mycpu_rmw_flag = 0;
        store_myvia(addr, via_last_read);
        myclk ++;
    }

    rclk = myclk - 1;	/* stores have a one-cylce offset */

    addr &= 0xf;

    switch (addr) {

      /* these are done with saving the value */
      case VIA_PRA:		/* port A */
        myviaifr &= ~VIA_IM_CA1;
        if (!IS_CA2_INDINPUT()) {
            myviaifr &= ~VIA_IM_CA2;
        }
	if(IS_CA2_HANDSHAKE()) {
	    ca2_state = 0;
	    VIA_SET_CA2( ca2_state )
	    if(IS_CA2_PULSE_MODE()) {
	  	ca2_state = 1;
	    	VIA_SET_CA2( ca2_state )
	    }
	}
	if (myviaier & (VIA_IM_CA1 | VIA_IM_CA2))
            update_myviairq();

      case VIA_PRA_NHS:	/* port A, no handshake */
        myvia[VIA_PRA_NHS] = byte;
        addr = VIA_PRA;
      case VIA_DDRA:
	myvia[addr] = byte;
	byte = myvia[VIA_PRA] | ~myvia[VIA_DDRA];
        store_pra(byte, oldpa, addr);
	oldpa = byte;
        break;

      case VIA_PRB:		/* port B */
        myviaifr &= ~VIA_IM_CB1;
        if ((myvia[VIA_PCR] & 0xa0) != 0x20) {
            myviaifr &= ~VIA_IM_CB2;
        }
        if(IS_CB2_HANDSHAKE()) {
            cb2_state = 0;
            VIA_SET_CB2( cb2_state )
            if(IS_CB2_PULSE_MODE()) {
                cb2_state = 1;
                VIA_SET_CB2( cb2_state )
            }
        }
	if (myviaier & (VIA_IM_CB1 | VIA_IM_CB2))
            update_myviairq();

      case VIA_DDRB:
	myvia[addr] = byte;
	byte = myvia[VIA_PRB] | ~myvia[VIA_DDRB];
        store_prb(byte, oldpb, addr);
	oldpb = byte;
        break;

      case VIA_SR:		/* Serial Port output buffer */
        myvia[addr] = byte;
        store_sr(byte);
        break;

        /* Timers */

      case VIA_T1CL:
      case VIA_T1LL:
        myvia[VIA_T1LL] = byte;
        update_myviatal(rclk);
        break;

      case VIA_T1CH:	/* Write timer A high */
        myvia[VIA_T1LH] = byte;
        update_myviatal(rclk);
        /* load counter with latch value */
        myviatau = rclk + myviatal + 3 + TAUOFFSET;
        myviatai = rclk + myviatal + 2;
        alarm_set(&myvia_t1_alarm, myviatai);

        /* set pb7 state */
        myviapb7 = 0;
        myviapb7o = 0;

        /* Clear T1 interrupt */
        myviaifr &= ~VIA_IM_T1;
        update_myviairq();
        break;

      case VIA_T1LH:		/* Write timer A high order latch */
        myvia[addr] = byte;
        update_myviatal(rclk);

        /* Clear T1 interrupt */
        myviaifr &= ~VIA_IM_T1;
        update_myviairq();
        break;

      case VIA_T2LL:		/* Write timer 2 low latch */
        myvia[VIA_T2LL] = byte;
        update_myviatbl();
        store_t2l(byte);
        break;

      case VIA_T2CH:		/* Write timer 2 high */
        myvia[VIA_T2CH] = byte;
        update_myviatbl();
        myviatbu = rclk + myviatbl + 3;
        myviatbi = rclk + myviatbl + 2;
        alarm_set(&myvia_t2_alarm, myviatbi);

        /* Clear T2 interrupt */
        myviaifr &= ~VIA_IM_T2;
        update_myviairq();
        break;

        /* Interrupts */

      case VIA_IFR:		/* 6522 Interrupt Flag Register */
        myviaifr &= ~byte;
        update_myviairq();
        break;

      case VIA_IER:		/* Interrupt Enable Register */
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
            myviaier |= byte & 0x7f;
        } else {
            /* clear interrupts */
            myviaier &= ~byte;
        }
        update_myviairq();
        break;

        /* Control */

      case VIA_ACR:
        /* bit 7 timer 1 output to PB7 */
        update_myviatal(rclk);
        if ((myvia[VIA_ACR] ^ byte) & 0x80) {
            if (byte & 0x80) {
                myviapb7 = 1 ^ myviapb7x;
            }
        }
        if ((myvia[VIA_ACR] ^ byte) & 0x40) {
            myviapb7 ^= myviapb7sx;
            if ((byte & 0x40)) {
                if (myviapb7x || myviapb7xx) {
                    if (myviatal) {
                        myviapb7o = 1;
                    } else {
                        myviapb7o = 0;
                        if ((myvia[VIA_ACR] & 0x80) && myviapb7x && (!myviapb7xx))
                            myviapb7 ^= 1;
                    }
                }
            }
        }
        myviapb7sx = myviapb7x;

        /* bit 1, 0  latch enable port B and A */
#ifdef MYVIA_NEED_LATCHING
	/* switch on port A latching - FIXME: is this ok? */
	if ( (!(myvia[addr] & 1)) && (byte & 1)) {
	    myvia_ila = read_pra(addr);
	}
	/* switch on port B latching - FIXME: is this ok? */
	if ( (!(myvia[addr] & 2)) && (byte & 2)) {
	    myvia_ilb = read_prb;
	}
#endif

        myvia[addr] = byte;
        store_acr(byte);

        /* bit 5 timer 2 count mode */
        if (byte & 32) {
            /* TODO */
            /* update_myviatb(0); *//* stop timer if mode == 1 */
        }

        /* bit 4, 3, 2 shift register control */

        break;

      case VIA_PCR:

        /* bit 7, 6, 5  CB2 handshake/interrupt control */
        /* bit 4  CB1 interrupt control */

        /* bit 3, 2, 1  CA2 handshake/interrupt control */
        /* bit 0  CA1 interrupt control */

	if ( (byte & 0x0e) == 0x0c ) {	/* set output low */
	    ca2_state = 0;
	} else 
	if ( (byte & 0x0e) == 0x0e ) {	/* set output high */
	    ca2_state = 1;
	} else {			/* set to toggle/pulse/input */
	    /* FIXME: is this correct if handshake is already active? */
	    ca2_state = 1;
	}
	VIA_SET_CA2( ca2_state )

	if ( (byte & 0xe0) == 0xc0 ) {	/* set output low */
	    cb2_state = 0;
	} else 
	if ( (byte & 0xe0) == 0xe0 ) {	/* set output high */
	    cb2_state = 1;
	} else {			/* set to toggle/pulse/input */
	    /* FIXME: is this correct if handshake is already active? */
	    cb2_state = 1;
	}
	VIA_SET_CB2( cb2_state )

        byte = store_pcr(byte, addr);

        myvia[addr] = byte;

        break;

      default:
        myvia[addr] = byte;

    }				/* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_myvia(ADDRESS addr)
{
#ifdef MYVIA_TIMER_DEBUG
    BYTE read_myvia_(ADDRESS);
    BYTE retv = read_myvia_(addr);
    addr &= 0x0f;
    if ((addr > 3 && addr < 10) || app_resources.debugFlag)
	log_message(myvia_log,
                    "read_myvia(%x) -> %02x, clk=%d", addr, retv, myclk);
    return retv;
}
BYTE REGPARM1 read_myvia_(ADDRESS addr)
{
#endif
    BYTE byte = 0xff;
    CLOCK rclk;

    addr &= 0xf;

    /* Hack for opcode fetch, where the clock does not change */
    if (myclk <= via_read_clk) {
        rclk = via_read_clk + (++via_read_offset);
    } else {
        via_read_clk = myclk;
        via_read_offset = 0;
        rclk = myclk;
    }

    if (addr >= VIA_T1CL && addr <= VIA_IER) { 
        if (myviatai && (myviatai <= myclk))
	    int_myviat1(myclk - myviatai);
        if (myviatbi && (myviatbi <= myclk))
	    int_myviat2(myclk - myviatbi);
    }

    switch (addr) {

      case VIA_PRA:		/* port A */
        myviaifr &= ~VIA_IM_CA1;
        if ((myvia[VIA_PCR] & 0x0a) != 0x02) {
            myviaifr &= ~VIA_IM_CA2;
        }
        if(IS_CA2_HANDSHAKE()) {
            ca2_state = 0;
            VIA_SET_CA2( ca2_state )
            if(IS_CA2_PULSE_MODE()) {
                ca2_state = 1;
                VIA_SET_CA2( ca2_state )
            }
        }
        if (myviaier & (VIA_IM_CA1 | VIA_IM_CA2)) 
	    update_myviairq();

      case VIA_PRA_NHS:	/* port A, no handshake */
        /* WARNING: this pin reads the voltage of the output pins, not
           the ORA value as the other port. Value read might be different
           from what is expected due to excessive load. */
#ifdef MYVIA_NEED_LATCHING
	if (IS_PA_INPUT_LATCH()) {
	    byte = myvia_ila;
	} else {
	    byte = read_pra(addr);
	}
#else
        byte = read_pra(addr);
#endif
	myvia_ila = byte;
	via_last_read = byte;
	return byte;

      case VIA_PRB:		/* port B */
        myviaifr &= ~VIA_IM_CB1;
        if ((myvia[VIA_PCR] & 0xa0) != 0x20)
            myviaifr &= ~VIA_IM_CB2;
        if (myviaier & (VIA_IM_CB1 | VIA_IM_CB2)) 
	    update_myviairq();

        /* WARNING: this pin reads the ORA for output pins, not
           the voltage on the pins as the other port. */
#ifdef MYVIA_NEED_LATCHING
	if (IS_PB_INPUT_LATCH()) {
	    byte = myvia_ilb;
	} else {
	    byte = read_prb();
	}
#else
        byte = read_prb();
#endif
	myvia_ilb = byte;
        byte = (byte & ~myvia[VIA_DDRB]) | (myvia[VIA_PRB] & myvia[VIA_DDRB]);

        if (myvia[VIA_ACR] & 0x80) {
            update_myviatal(rclk);
            byte = (byte & 0x7f) | (((myviapb7 ^ myviapb7x) | myviapb7o) ? 0x80 : 0);
        }
	via_last_read = byte;
        return byte;

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        myviaifr &= ~VIA_IM_T1;
        update_myviairq();
        via_last_read = myviata() & 0xff;
	return via_last_read;

      case VIA_T1CH /*TIMER_AH */ :	/* timer A high */
        via_last_read = (myviata() >> 8) & 0xff;
	return via_last_read;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        myviaifr &= ~VIA_IM_T2;
        update_myviairq();
        via_last_read = myviatb() & 0xff;
	return via_last_read;

      case VIA_T2CH /*TIMER_BH */ :	/* timer B high */
        via_last_read = (myviatb() >> 8) & 0xff;
	return via_last_read;

      case VIA_SR:		/* Serial Port Shift Register */
        via_last_read = (myvia[addr]);
	return via_last_read;

        /* Interrupts */

      case VIA_IFR:		/* Interrupt Flag Register */
        {
            BYTE t = myviaifr;
            if (myviaifr & myviaier /*[VIA_IER] */ )
                t |= 0x80;
	    via_last_read = t;
            return (t);
        }

      case VIA_IER:		/* 6522 Interrupt Control Register */
        via_last_read = (myviaier /*[VIA_IER] */  | 0x80);
	return via_last_read;

    }				/* switch */

    via_last_read = myvia[addr];
    return (myvia[addr]);
}

BYTE REGPARM1 peek_myvia(ADDRESS addr)
{
    CLOCK rclk = myclk;

    addr &= 0xf;

    if (myviatai && (myviatai <= myclk))
	int_myviat1(myclk - myviatai);
    if (myviatbi && (myviatbi <= myclk))
	int_myviat2(myclk - myviatbi);

    switch (addr) {
      case VIA_PRA:
        return read_myvia(VIA_PRA_NHS);

      case VIA_PRB:		/* port B */
        {
            BYTE byte;
#ifdef MYVIA_NEED_LATCHING
	    if (IS_PB_INPUT_LATCH()) {
	        byte = myvia_ilb;
	    } else {
	        byte = read_prb();
	    }
#else
            byte = read_prb();
#endif
            byte = (byte & ~myvia[VIA_DDRB]) | (myvia[VIA_PRB] & myvia[VIA_DDRB]);
            if (myvia[VIA_ACR] & 0x80) {
                update_myviatal(rclk);
                byte = (byte & 0x7f) | (((myviapb7 ^ myviapb7x) | myviapb7o) ? 0x80 : 0);
            }
            return byte;
        }

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        return myviata() & 0xff;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        return myviatb() & 0xff;

      default:
        break;
    }				/* switch */

    return read_myvia(addr);
}


/* ------------------------------------------------------------------------- */

static int int_myviat1(long offset)
{
#ifdef MYVIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	log_message(myvia_log, "myvia timer A interrupt");
#endif

    if (!(myvia[VIA_ACR] & 0x40)) {	/* one-shot mode */
#ifdef MYVIA_TIMER_DEBUG
	log_message(myvia_log, "MYVIA Timer A interrupt -- one-shot mode: next int won't happen");
#endif
        alarm_unset(&myvia_t1_alarm);
	myviatai = 0;
    } else {			/* continuous mode */
	/* load counter with latch value */
	myviatai += myviatal + 2;
        alarm_set(&myvia_t1_alarm, myviatai);
    }
    myviaifr |= VIA_IM_T1;
    update_myviairq();

    /* TODO: toggle PB7? */
    return 0;			/*(viaier & VIA_IM_T1) ? 1:0; */
}

/*
 * Timer B is always in one-shot mode
 */

static int int_myviat2(long offset)
{
#ifdef MYVIA_TIMER_DEBUG
    if (app_resources.debugFlag)
	log_message(myvia_log, "MYVIA timer B interrupt.");
#endif

    alarm_unset(&myvia_t2_alarm);	/*int_clk[I_MYVIAT2] = 0; */
    myviatbi = 0;

    myviaifr |= VIA_IM_T2;
    update_myviairq();

    return 0;
}

static void clk_overflow_callback(CLOCK sub, void *data)
{
    unsigned int t;
    t = (myviatau - (myclk + sub)) & 0xffff;
    myviatau = myclk + t;
    t = (myviatbu - (myclk + sub)) & 0xffff;
    myviatbu = myclk + t;
    if (myviatai)
	myviatai -= sub;
    if (via_read_clk > sub)
        via_read_clk -= sub;
    else
        via_read_clk = 0;
}

/*------------------------------------------------------------------------*/

/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 0/0
 */

/* The name of the modul must be defined before including this file.  */
#define VIA_DUMP_VER_MAJOR      1
#define VIA_DUMP_VER_MINOR      0

/*
 * The dump data:
 *
 * UBYTE        ORA
 * UBYTE        DDRA
 * UBYTE        ORB
 * UBYTE        DDRB
 * UWORD	T1L
 * UWORD	T1C
 * UBYTE	T2L
 * UWORD	T2C
 * UBYTE	SR
 * UBYTE	ACR
 * UBYTE	PCR
 * UBYTE	IFR		 active interrupts
 * UBYTE	IER		 interrupt masks
 * UBYTE	PB7		 bit 7 = pb7 state
 * UBYTE	SRHBITS		 number of half bits to shift out on SR
 * UBYTE	CABSTATE	 bit 7 = ca2 state, bi 6 = cb2 state
 * UBYTE	ILA		 input latch port A
 * UBYTE	ILB		 input latch port B
 */

/* FIXME!!!  Error check.  */

int myvia_write_snapshot_module(snapshot_t * p)
{
    snapshot_module_t *m;

    if (myviatai && (myviatai <= myclk))
        int_myviat1(myclk - myviatai);
    if (myviatbi && (myviatbi <= myclk))
        int_myviat2(myclk - myviatbi);

    m = snapshot_module_create(p, snap_module_name,
                               VIA_DUMP_VER_MAJOR, VIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    snapshot_module_write_byte(m, myvia[VIA_PRA]);
    snapshot_module_write_byte(m, myvia[VIA_DDRA]);
    snapshot_module_write_byte(m, myvia[VIA_PRB]);
    snapshot_module_write_byte(m, myvia[VIA_DDRB]);

    snapshot_module_write_word(m, myviatal);
    snapshot_module_write_word(m, myviata());
    snapshot_module_write_byte(m, myvia[VIA_T2LL]);
    snapshot_module_write_word(m, myviatb());

    snapshot_module_write_byte(m, (myviatai ? 0x80 : 0)
					| (myviatbi ? 0x40 : 0) );

    snapshot_module_write_byte(m, myvia[VIA_SR]);
    snapshot_module_write_byte(m, myvia[VIA_ACR]);
    snapshot_module_write_byte(m, myvia[VIA_PCR]);

    snapshot_module_write_byte(m, myviaifr);
    snapshot_module_write_byte(m, myviaier);

						/* FIXME! */
    snapshot_module_write_byte(m, (((myviapb7 ^ myviapb7x) | myviapb7o) ? 0x80 : 0));
    snapshot_module_write_byte(m, 0);		/* SRHBITS */

    snapshot_module_write_byte(m, (ca2_state ? 0x80 : 0) 
				| (cb2_state ? 0x40 : 0));

    snapshot_module_write_byte(m, myvia_ila);
    snapshot_module_write_byte(m, myvia_ilb);

    snapshot_module_close(m);

    return 0;
}

int myvia_read_snapshot_module(snapshot_t * p)
{
    BYTE vmajor, vminor;
    BYTE byte;
    WORD word;
    ADDRESS addr;
    CLOCK rclk = myclk;
    snapshot_module_t *m;

    m = snapshot_module_open(p, snap_module_name, &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (vmajor != VIA_DUMP_VER_MAJOR) {
        log_error(myvia_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  vmajor, vminor, VIA_DUMP_VER_MAJOR, VIA_DUMP_VER_MINOR);
        snapshot_module_close(m);
        return -1;
    }

    alarm_unset(&myvia_t1_alarm);
    alarm_unset(&myvia_t2_alarm);

    myviatai = 0;
    myviatbi = 0;

    snapshot_module_read_byte(m, &myvia[VIA_PRA]);
    snapshot_module_read_byte(m, &myvia[VIA_DDRA]);
    snapshot_module_read_byte(m, &myvia[VIA_PRB]);
    snapshot_module_read_byte(m, &myvia[VIA_DDRB]);
    {
        addr = VIA_DDRA;
	byte = myvia[VIA_PRA] | ~myvia[VIA_DDRA];
	undump_pra(byte);
	oldpa = byte;

	addr = VIA_DDRB;
	byte = myvia[VIA_PRB] | ~myvia[VIA_DDRB];
	undump_prb(byte);
	oldpb = byte;
    }

    snapshot_module_read_word(m, &word);
    myviatal = word;
    myvia[VIA_T1LL] = myviatal & 0xff;
    myvia[VIA_T1LH] = (myviatal >> 8) & 0xff;
    snapshot_module_read_word(m, &word);
    myviatau = rclk + word + 2 /* 3 */ + TAUOFFSET;
    myviatai = rclk + word + 1;

    snapshot_module_read_byte(m, &myvia[VIA_T2LL]);
    snapshot_module_read_word(m, &word);
    myviatbu = rclk + word + 2 /* 3 */;
    myviatbi = rclk + word + 1;

    snapshot_module_read_byte(m, &byte);
    if (byte & 0x80) {
        alarm_set(&myvia_t1_alarm, myviatai);
    } else {
	myviatai = 0;
    }
    if (byte & 0x40) {
        alarm_set(&myvia_t2_alarm, myviatbi);
    } else {
	myviatbi = 0;
    }

    snapshot_module_read_byte(m, &myvia[VIA_SR]);
    snapshot_module_read_byte(m, &myvia[VIA_ACR]);
    snapshot_module_read_byte(m, &myvia[VIA_PCR]);

    snapshot_module_read_byte(m, &byte);
    myviaifr = byte;
    snapshot_module_read_byte(m, &byte);
    myviaier = byte;

    via_restore_int(myviaifr & myviaier & 0x7f);

    /* FIXME! */
    snapshot_module_read_byte(m, &byte);
    myviapb7 = byte ? 1 : 0;
    myviapb7x = 0;
    myviapb7o = 0;
    snapshot_module_read_byte(m, &byte);	/* SRHBITS */

    snapshot_module_read_byte(m, &byte);	/* CABSTATE */
    ca2_state = byte & 0x80;
    cb2_state = byte & 0x40;

    /* undump_pcr also restores the ca2_state/cb2_state effects if necessary;
       i.e. calls VIA_SET_C*2( c*2_state ) if necessary */
    {
	addr = VIA_PCR;
	byte = myvia[addr];
	undump_pcr(byte);
    }
    {
	addr = VIA_SR;
	byte = myvia[addr];
	store_sr(byte);
    }
    {
	addr = VIA_ACR;
	byte = myvia[addr];
	undump_acr(byte);
    }

    snapshot_module_read_byte(m, &myvia_ila);
    snapshot_module_read_byte(m, &myvia_ilb);

    return snapshot_module_close(m);
}

