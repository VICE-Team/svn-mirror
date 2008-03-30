
/*
 * ../../src/drive/via1drive0.c
 * This file is generated from ../../src/via-tmpl.c and ../../src/drive/via1drive0.def,
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
 * into one interrupt flag, I_VIA1D0FL.
 * if an interrupt condition changes, the function (i.e. cpp macro)
 * update_via1d0irq() id called, that checks the IRQ line state.
 * This is now possible, as ettore has decoupled A_* alarm events
 * from I_* interrupts for performance reasons.
 *
 * A new function for signaling rising/falling edges on the
 * control lines is introduced:
 *      via1d0_signal(VIA_SIG_[CA1|CA2|CB1|CB2], VIA_SIG_[RISE|FALL])
 * which signals the corresponding edge to the VIA. The constants
 * are defined in via.h.
 *
 * Except for shift register and input latching everything should be ok now.
 */

#include "vice.h"

#ifdef STDC_HEADERS
#include <stdio.h>
#include <time.h>
#endif

#include "log.h"
#include "resources.h"
#include "snapshot.h"
#include "via.h"
#include "vmachine.h"


#include "drive.h"
#include "drivecpu.h"
#include "iecdrive.h"
#include "viad.h"
#include "parallel.h"

#define	VIA_SET_CA2(a)
#define	VIA_SET_CB2(a)

#include "interrupt.h"
				/* Timer debugging */
/*#define VIA1D0_TIMER_DEBUG */
				/* when PB7 is really used, set this
				   to enable pulse output from the timer.
				   Otherwise PB7 state is computed only
				   when port B is read -
				   not yet implemented */
/*#define VIA1D0_NEED_PB7 */
				/* When you really need latching, define this.
				   It implies additional READ_PR* when
				   writing the snapshot. When latching is 
				   enabled: it reads the port when enabling,
				   and when an active C*1 transition occurs. 
				   It does not read the port when reading the
				   port register. Side-effects beware! */
/*#define VIA1D0_NEED_LATCHING */

/* global */

BYTE via1d0[16];



/*
 * Local variables
 */

static int via1d0ifr;		/* Interrupt Flag register for via1d0 */
static int via1d0ier;		/* Interrupt Enable register for via1d0 */

static unsigned int via1d0tal;	/* current timer A latch value */
static unsigned int via1d0tbl;	/* current timer B latch value */

static CLOCK via1d0tau;		/* time when via1d0 timer A is updated */
static CLOCK via1d0tbu;		/* time when via1d0 timer B is updated */
static CLOCK via1d0tai;		/* time when next timer A alarm is */
static CLOCK via1d0tbi;		/* time when next timer A alarm is */

static int via1d0pb7;		/* state of PB7 for pulse output... */
static int via1d0pb7x;		/* to be xored herewith  */
static int via1d0pb7o;		/* to be ored herewith  */
static int via1d0pb7xx;
static int via1d0pb7sx;

static BYTE oldpa;		/* the actual output on PA (input = high) */
static BYTE oldpb;		/* the actual output on PB (input = high) */

static BYTE via1d0_ila;		/* input latch A */
static BYTE via1d0_ilb;		/* input latch B */

static int ca2_state;
static int cb2_state;

static log_t via1d0_log = LOG_ERR;

/*
 * local functions
 */

#define IS_CA2_OUTPUT()          ((via1d0[VIA_PCR] & 0x0c) == 0x0c)
#define IS_CA2_INDINPUT()        ((via1d0[VIA_PCR] & 0x0a) == 0x02)
#define IS_CA2_HANDSHAKE()       ((via1d0[VIA_PCR] & 0x0c) == 0x08)
#define IS_CA2_PULSE_MODE()      ((via1d0[VIA_PCR] & 0x0e) == 0x09)
#define IS_CA2_TOGGLE_MODE()     ((via1d0[VIA_PCR] & 0x0e) == 0x08)

#define IS_CB2_OUTPUT()          ((via1d0[VIA_PCR] & 0xc0) == 0xc0)
#define IS_CB2_INDINPUT()        ((via1d0[VIA_PCR] & 0xa0) == 0x20)
#define IS_CB2_HANDSHAKE()       ((via1d0[VIA_PCR] & 0xc0) == 0x80)
#define IS_CB2_PULSE_MODE()      ((via1d0[VIA_PCR] & 0xe0) == 0x90)
#define IS_CB2_TOGGLE_MODE()     ((via1d0[VIA_PCR] & 0xe0) == 0x80)

#define	IS_PA_INPUT_LATCH()	 (via1d0[VIA_ACR] & 0x01)
#define	IS_PB_INPUT_LATCH()	 (via1d0[VIA_ACR] & 0x02)

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
 via1d0t*u* clk ------------------------------------------+   +--------
                                                     |
                                                     |
                                                  call of
			                        int_via1d0*
                                                   here

   real via1d0tau value = via1d0tau* + TAUOFFSET
   via1d0tbu = via1d0tbu* + 0

 *
 * IRQ and PB7 are set/toggled at the low-high transition of Phi2,
 * but int_* is called a half-cycle before that. Does that matter?
 *
 * PB7 output is still to be implemented
 */

/* timer values do not depend on a certain value here, but PB7 does... */
#define	TAUOFFSET	(-1)


#ifndef via_restore_int	/* if VIA reports to other chip (TPI) for IRQ */
#define	via_restore_int(a)  set_int_noclk(&drive0_int_status, I_VIA1D0FL, \
		(a) ? IK_IRQ : 0)
#endif


inline static void update_via1d0irq(void)
{
    drive0_set_irq(I_VIA1D0FL, (via1d0ifr & via1d0ier & 0x7f) ? IK_IRQ : 0);
}

/* the next two are used in read_via1d0() */

inline static unsigned int via1d0ta(void)
{
    if (drive_clk[0] < via1d0tau - TAUOFFSET)
        return via1d0tau - TAUOFFSET - drive_clk[0] - 2;
    else
	return (via1d0tal - (drive_clk[0] - via1d0tau + TAUOFFSET) % (via1d0tal + 2));
}

inline static unsigned int via1d0tb(void)
{
    return via1d0tbu - drive_clk[0] - 2;
}

inline static void update_via1d0tal(CLOCK rclk)
{
    via1d0pb7x = 0;
    via1d0pb7xx = 0;

    if (rclk > via1d0tau) {
	int nuf = (via1d0tal + 1 + rclk - via1d0tau) / (via1d0tal + 2);

	if (!(via1d0[VIA_ACR] & 0x40)) {
	    if (((nuf - via1d0pb7sx) > 1) || (!via1d0pb7)) {
		via1d0pb7o = 1;
		via1d0pb7sx = 0;
	    }
	}
	via1d0pb7 ^= (nuf & 1);

	via1d0tau = TAUOFFSET + via1d0tal + 2 + (rclk - (rclk - via1d0tau + TAUOFFSET) % (via1d0tal + 2));
	if (rclk == via1d0tau - via1d0tal - 1) {
	    via1d0pb7xx = 1;
	}
    }

    if (via1d0tau == rclk)
	via1d0pb7x = 1;

    via1d0tal = via1d0[VIA_T1LL] + (via1d0[VIA_T1LH] << 8);
}

inline static void update_via1d0tbl(void)
{
    via1d0tbl = via1d0[VIA_T2CL] + (via1d0[VIA_T2CH] << 8);
}


/* ------------------------------------------------------------------------- */
/* VIA1D0 */


    static int parieee_is_out = 1;    /* 0= listener, 1= talker */

    void drive0_parallel_set_atn(int state)
    {
        if (drive[0].type == DRIVE_TYPE_2031) {
           via1d0_signal(VIA_SIG_CA1, state ? VIA_SIG_RISE : 0);
           parallel_drv0_set_nrfd( ((!parieee_is_out) && (!(oldpb & 0x02)))
                               || (parallel_atn && (!(oldpb & 0x01)))
                               || ((!parallel_atn) && (oldpb & 0x01)));
           parallel_drv0_set_ndac( ((!parieee_is_out) && (!(oldpb & 0x04)))
                               || (parallel_atn && (!(oldpb & 0x01)))
                               || ((!parallel_atn) && (oldpb & 0x01)));
        }
    }

    static iec_info_t *iec_info;

/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void reset_via1d0(void)
{
    int i;

    if (via1d0_log == LOG_ERR)
        via1d0_log = log_open("VIA1D0");

    /* clear registers */
    for (i = 0; i < 4; i++)
	via1d0[i] = 0;
    for (i = 4; i < 10; i++)
        via1d0[i] = 0xff;        /* AB 98.08.23 */
    for (i = 11; i < 16; i++)
	via1d0[i] = 0;

    via1d0tal = 0;
    via1d0tbl = 0;
    via1d0tau = drive_clk[0];
    via1d0tbu = drive_clk[0];

    via1d0ier = 0;
    via1d0ifr = 0;

    /* disable vice interrupts */
    via1d0tai = 0;
    via1d0tbi = 0;
    drive0_unset_alarm(A_VIA1D0T1);
    drive0_unset_alarm(A_VIA1D0T2);
    update_via1d0irq();

    oldpa = 0xff;
    oldpb = 0xff;

    ca2_state = 1;
    cb2_state = 1;
    VIA_SET_CA2( ca2_state )	/* input = high */
    VIA_SET_CB2( cb2_state )	/* input = high */


    parallel_drv0_set_ndac(0);
    parallel_drv0_set_nrfd(0);
    parallel_drv0_set_dav(0);
    parallel_drv0_set_eoi(0);
    parallel_drv0_set_bus(0);

    parieee_is_out = 1;

    iec_info = iec_get_drive_port();
    if (iec_info && drive[0].type == DRIVE_TYPE_2031) {
        iec_info->drive_bus = 0xff;
        iec_info->drive_data = 0xff;
        iec_info = NULL;
    }
}

void via1d0_signal(int line, int edge)
{
    switch (line) {
      case VIA_SIG_CA1:
	if ( (edge ? 1 : 0) == (via1d0[VIA_PCR] & 0x01) ) {
	    if (IS_CA2_TOGGLE_MODE() && !ca2_state) {
		ca2_state = 1;
		VIA_SET_CA2( ca2_state )
	    }
            via1d0ifr |= VIA_IM_CA1;
            update_via1d0irq();
#ifdef VIA1D0_NEED_LATCHING
	    if (IS_PA_INPUT_LATCH()) {
		BYTE byte;

    if (drive[0].type == DRIVE_TYPE_1571) {
        BYTE tmp;
        if (drive[0].byte_ready_active == 0x6)
            drive_rotate_disk(&drive[0]);
        tmp = (drive[0].byte_ready ? 0 : 0x80)
            | (drive[0].current_half_track == 2 ? 0 : 1);
        return (tmp & ~via1d0[VIA_DDRA])
            | (via1d0[VIA_PRA] & via1d0[VIA_DDRA]);
    }
    if (drive[0].type == DRIVE_TYPE_2031) {
/*
	if (drive[1].enable)
	    drive1_cpu_execute();
*/
        if (parallel_debug) {
	    printf("read_pra(is_out=%d, parallel_bus=%02x, ddra=%02x\n",
		parieee_is_out, parallel_bus, via1d0[VIA_DDRA]);
	}
        byte = parieee_is_out ? 0xff : ~parallel_bus;
        return (byte & ~via1d0[VIA_DDRA]) | (via1d0[VIA_PRA] & via1d0[VIA_DDRA]);
    }
    byte = (drive[0].parallel_cable_enabled
            ? parallel_cable_drive_read((((addr == VIA_PRA) &&
                                          (via1d0[VIA_PCR] & 0xe) == 0xa))
                                        ? 1 : 0)
            : ((via1d0[VIA_PRA] & via1d0[VIA_DDRA])
               | (0xff & ~via1d0[VIA_DDRA])) );
		via1d0_ila = byte;
	    }
#endif
	}
        break;
      case VIA_SIG_CA2:
        if (!(via1d0[VIA_PCR] & 0x08)) {
            via1d0ifr |= (((edge << 2) ^ via1d0[VIA_PCR]) & 0x04) ?
                0 : VIA_IM_CA2;
            update_via1d0irq();
        }
        break;
      case VIA_SIG_CB1:
	if ( (edge ? 0x10 : 0) == (via1d0[VIA_PCR] & 0x10) ) {
	    if (IS_CB2_TOGGLE_MODE() && !cb2_state) {
		cb2_state = 1;
		VIA_SET_CB2( cb2_state )
	    }
            via1d0ifr |= VIA_IM_CB1;
            update_via1d0irq();
#ifdef VIA1D0_NEED_LATCHING
	    if (IS_PB_INPUT_LATCH()) {
		BYTE byte;

    if (iec_info != NULL) {
	byte = ((via1d0[VIA_PRB] & 0x1a) | iec_info->drive_port) ^ 0x85;
    } else {
        if (drive[0].type == DRIVE_TYPE_2031) {
/*
	   if (drive[1].enable)
		drive1_cpu_execute();
*/
           byte = 0xff;
           if (parieee_is_out) {
               /* talk enable */
               if (parallel_nrfd) byte &= 0xfd ;
               if (parallel_ndac) byte &= 0xfb ;
           } else {
               /* listener */
               if (parallel_eoi) byte &= 0xf7 ;
               if (parallel_dav) byte &= 0xbf ;
           }
           if (!parallel_atn) byte &= 0x7f;
           if (parallel_debug) {
 	       printf("read_prb(is_out=%d, byte=%02x, prb=%02x, ddrb=%02x\n",
		   parieee_is_out, byte, via1d0[VIA_PRB], via1d0[VIA_DDRB]);
	   }
           byte = (byte & ~via1d0[VIA_DDRB]) | (via1d0[VIA_PRB] & via1d0[VIA_DDRB]);
           if (!ca2_state) {
               byte &= 0xfe /* 0xff */;  /* byte & 3 + 8 -> device-no */
               byte &= 0xfd /* 0xff */;  /* device-no switche */
	       if (parallel_debug) {
		   printf("read with ca2_state = 0 -> byte=%02x\n", byte);
	       }
           }
	   if (parallel_debug) {
		printf("       -> byte=%02x\n", byte);
	   }
        } else {
           byte = ((via1d0[VIA_PRB] & 0x1a) | iec_drive0_read()) ^ 0x85;
        }
    }
		via1d0_ilb = byte;
	    }
#endif	
	}
        break;
      case VIA_SIG_CB2:
        if (!(via1d0[VIA_PCR] & 0x80)) {
            via1d0ifr |= (((edge << 6) ^ via1d0[VIA_PCR]) & 0x40) ?
                0 : VIA_IM_CB2;
            update_via1d0irq();
        }
        break;
    }
}

void REGPARM2 store_via1d0(ADDRESS addr, BYTE byte)
{
    CLOCK rclk = drive_clk[0] - 1;	/* stores have a one-cylce offset */

    addr &= 0xf;

    switch (addr) {

      /* these are done with saving the value */
      case VIA_PRA:		/* port A */
        via1d0ifr &= ~VIA_IM_CA1;
        if (!IS_CA2_INDINPUT()) {
            via1d0ifr &= ~VIA_IM_CA2;
        }
	if(IS_CA2_HANDSHAKE()) {
	    ca2_state = 0;
	    VIA_SET_CA2( ca2_state )
	    if(IS_CA2_PULSE_MODE()) {
	  	ca2_state = 1;
	    	VIA_SET_CA2( ca2_state )
	    }
	}
	if (via1d0ier & (VIA_IM_CA1 | VIA_IM_CA2))
            update_via1d0irq();

      case VIA_PRA_NHS:	/* port A, no handshake */
        via1d0[VIA_PRA_NHS] = byte;
        addr = VIA_PRA;
      case VIA_DDRA:
	via1d0[addr] = byte;
	byte = via1d0[VIA_PRA] | ~via1d0[VIA_DDRA];

    {
        if (drive[0].type == DRIVE_TYPE_1571) {
            if ((oldpa ^ byte) & 0x20)
                drive_set_1571_sync_factor(byte & 0x20, 0);
            if ((oldpa ^ byte) & 0x04)
                drive_set_1571_side((byte >> 2) & 1, 0);
        } else
        if (drive[0].type == DRIVE_TYPE_2031) {
            if(parallel_debug) 
		printf("store_pra(byte=%02x, ~byte=%02x)\n",byte, 0xff^byte);
	    parallel_drv0_set_bus(parieee_is_out ? byte : 0);
	} else
        if (drive[0].parallel_cable_enabled && drive[0].type == DRIVE_TYPE_1541)
            parallel_cable_drive0_write(byte,
                                        (((addr == VIA_PRA)
                                        && ((via1d0[VIA_PCR] & 0xe) == 0xa))
                                        ? 1 : 0));
    }
	oldpa = byte;
        break;

      case VIA_PRB:		/* port B */
        via1d0ifr &= ~VIA_IM_CB1;
        if ((via1d0[VIA_PCR] & 0xa0) != 0x20) {
            via1d0ifr &= ~VIA_IM_CB2;
        }
        if(IS_CB2_HANDSHAKE()) {
            cb2_state = 0;
            VIA_SET_CB2( cb2_state )
            if(IS_CB2_PULSE_MODE()) {
                cb2_state = 1;
                VIA_SET_CB2( cb2_state )
            }
        }
	if (via1d0ier & (VIA_IM_CB1 | VIA_IM_CB2))
            update_via1d0irq();

      case VIA_DDRB:
	via1d0[addr] = byte;
	byte = via1d0[VIA_PRB] | ~via1d0[VIA_DDRB];

    if (byte != oldpb) {
	if (iec_info != NULL) {
	    iec_info->drive_data = ~byte;
	    iec_info->drive_bus = (((iec_info->drive_data << 3) & 0x40)
	        | ((iec_info->drive_data << 6)
	        & ((~iec_info->drive_data ^ iec_info->cpu_bus) << 3) & 0x80));
	    iec_info->cpu_port = iec_info->cpu_bus & iec_info->drive_bus
	        & iec_info->drive2_bus;
	    iec_info->drive_port = iec_info->drive2_port = (((iec_info->cpu_port >> 4) & 0x4)
	        | (iec_info->cpu_port >> 7)
	        | ((iec_info->cpu_bus << 3) & 0x80));
	} else 
        if (drive[0].type == DRIVE_TYPE_2031) {
	    BYTE tmp = ~byte;
            if(parallel_debug) {
		printf("store_prb(byte=%02x, ~byte=%02x, prb=%02x, ddrb=%02x)\n",
			byte, tmp, via1d0[VIA_PRB],via1d0[VIA_DDRB]);
		printf("  -> is_out=%d, eoi=%d, dav=%d\n",byte & 0x10, 
			!(byte & 0x08), !(byte & 0x40));
	    }
            parieee_is_out = byte & 0x10;
            parallel_drv0_set_bus(parieee_is_out ? oldpa : 0);

	    if ( parieee_is_out ) {
                parallel_drv0_set_eoi( tmp & 0x08 );
                parallel_drv0_set_dav( tmp & 0x40 );
	    } else {
                parallel_drv0_set_eoi( 0 );
                parallel_drv0_set_dav( 0 );
	    }
            parallel_drv0_set_nrfd( ((!parieee_is_out) && (tmp & 0x02))
                                || (parallel_atn && (tmp & 0x01))
                                || ((!parallel_atn) && (byte & 0x01)));
            parallel_drv0_set_ndac( ((!parieee_is_out) && (tmp & 0x04))
                                || (parallel_atn && (tmp & 0x01))
                                || ((!parallel_atn) && (byte & 0x01)));
        } else {
	    iec_drive0_write(~byte);
	}
    }
	oldpb = byte;
        break;

      case VIA_SR:		/* Serial Port output buffer */
        via1d0[addr] = byte;
        
            break;

        /* Timers */

      case VIA_T1CL:
      case VIA_T1LL:
        via1d0[VIA_T1LL] = byte;
        update_via1d0tal(rclk);
        break;

      case VIA_T1CH:	/* Write timer A high */
        via1d0[VIA_T1LH] = byte;
        update_via1d0tal(rclk);
        /* load counter with latch value */
        via1d0tau = rclk + via1d0tal + 3 + TAUOFFSET;
        via1d0tai = rclk + via1d0tal + 2;
        drive0_set_alarm_clk(A_VIA1D0T1, via1d0tai);

        /* set pb7 state */
        via1d0pb7 = 0;
        via1d0pb7o = 0;

        /* Clear T1 interrupt */
        via1d0ifr &= ~VIA_IM_T1;
        update_via1d0irq();
        break;

      case VIA_T1LH:		/* Write timer A high order latch */
        via1d0[addr] = byte;
        update_via1d0tal(rclk);

        /* Clear T1 interrupt */
        via1d0ifr &= ~VIA_IM_T1;
        update_via1d0irq();
        break;

      case VIA_T2LL:		/* Write timer 2 low latch */
        via1d0[VIA_T2LL] = byte;
        update_via1d0tbl();
        
            break;

      case VIA_T2CH:		/* Write timer 2 high */
        via1d0[VIA_T2CH] = byte;
        update_via1d0tbl();
        via1d0tbu = rclk + via1d0tbl + 3;
        via1d0tbi = rclk + via1d0tbl + 2;
        drive0_set_alarm_clk(A_VIA1D0T2, via1d0tbi);

        /* Clear T2 interrupt */
        via1d0ifr &= ~VIA_IM_T2;
        update_via1d0irq();
        break;

        /* Interrupts */

      case VIA_IFR:		/* 6522 Interrupt Flag Register */
        via1d0ifr &= ~byte;
        update_via1d0irq();
        break;

      case VIA_IER:		/* Interrupt Enable Register */
        if (byte & VIA_IM_IRQ) {
            /* set interrupts */
            via1d0ier |= byte & 0x7f;
        } else {
            /* clear interrupts */
            via1d0ier &= ~byte;
        }
        update_via1d0irq();
        break;

        /* Control */

      case VIA_ACR:
        /* bit 7 timer 1 output to PB7 */
        update_via1d0tal(rclk);
        if ((via1d0[VIA_ACR] ^ byte) & 0x80) {
            if (byte & 0x80) {
                via1d0pb7 = 1 ^ via1d0pb7x;
            }
        }
        if ((via1d0[VIA_ACR] ^ byte) & 0x40) {
            via1d0pb7 ^= via1d0pb7sx;
            if ((byte & 0x40)) {
                if (via1d0pb7x || via1d0pb7xx) {
                    if (via1d0tal) {
                        via1d0pb7o = 1;
                    } else {
                        via1d0pb7o = 0;
                        if ((via1d0[VIA_ACR] & 0x80) && via1d0pb7x && (!via1d0pb7xx))
                            via1d0pb7 ^= 1;
                    }
                }
            }
        }
        via1d0pb7sx = via1d0pb7x;

        /* bit 1, 0  latch enable port B and A */
#ifdef VIA1D0_NEED_LATCHING
	/* switch on port A latching - FIXME: is this ok? */
	if ( (!(via1d0[addr] & 1)) && (byte & 1)) {

    if (drive[0].type == DRIVE_TYPE_1571) {
        BYTE tmp;
        if (drive[0].byte_ready_active == 0x6)
            drive_rotate_disk(&drive[0]);
        tmp = (drive[0].byte_ready ? 0 : 0x80)
            | (drive[0].current_half_track == 2 ? 0 : 1);
        return (tmp & ~via1d0[VIA_DDRA])
            | (via1d0[VIA_PRA] & via1d0[VIA_DDRA]);
    }
    if (drive[0].type == DRIVE_TYPE_2031) {
/*
	if (drive[1].enable)
	    drive1_cpu_execute();
*/
        if (parallel_debug) {
	    printf("read_pra(is_out=%d, parallel_bus=%02x, ddra=%02x\n",
		parieee_is_out, parallel_bus, via1d0[VIA_DDRA]);
	}
        byte = parieee_is_out ? 0xff : ~parallel_bus;
        return (byte & ~via1d0[VIA_DDRA]) | (via1d0[VIA_PRA] & via1d0[VIA_DDRA]);
    }
    byte = (drive[0].parallel_cable_enabled
            ? parallel_cable_drive_read((((addr == VIA_PRA) &&
                                          (via1d0[VIA_PCR] & 0xe) == 0xa))
                                        ? 1 : 0)
            : ((via1d0[VIA_PRA] & via1d0[VIA_DDRA])
               | (0xff & ~via1d0[VIA_DDRA])) );
	    via1d0_ila = byte;
	}
	/* switch on port B latching - FIXME: is this ok? */
	if ( (!(via1d0[addr] & 2)) && (byte & 2)) {

    if (iec_info != NULL) {
	byte = ((via1d0[VIA_PRB] & 0x1a) | iec_info->drive_port) ^ 0x85;
    } else {
        if (drive[0].type == DRIVE_TYPE_2031) {
/*
	   if (drive[1].enable)
		drive1_cpu_execute();
*/
           byte = 0xff;
           if (parieee_is_out) {
               /* talk enable */
               if (parallel_nrfd) byte &= 0xfd ;
               if (parallel_ndac) byte &= 0xfb ;
           } else {
               /* listener */
               if (parallel_eoi) byte &= 0xf7 ;
               if (parallel_dav) byte &= 0xbf ;
           }
           if (!parallel_atn) byte &= 0x7f;
           if (parallel_debug) {
 	       printf("read_prb(is_out=%d, byte=%02x, prb=%02x, ddrb=%02x\n",
		   parieee_is_out, byte, via1d0[VIA_PRB], via1d0[VIA_DDRB]);
	   }
           byte = (byte & ~via1d0[VIA_DDRB]) | (via1d0[VIA_PRB] & via1d0[VIA_DDRB]);
           if (!ca2_state) {
               byte &= 0xfe /* 0xff */;  /* byte & 3 + 8 -> device-no */
               byte &= 0xfd /* 0xff */;  /* device-no switche */
	       if (parallel_debug) {
		   printf("read with ca2_state = 0 -> byte=%02x\n", byte);
	       }
           }
	   if (parallel_debug) {
		printf("       -> byte=%02x\n", byte);
	   }
        } else {
           byte = ((via1d0[VIA_PRB] & 0x1a) | iec_drive0_read()) ^ 0x85;
        }
    }
	    via1d0_ilb = byte;
	}
#endif

        via1d0[addr] = byte;

        

        /* bit 5 timer 2 count mode */
        if (byte & 32) {
            /* TODO */
            /* update_via1d0tb(0); *//* stop timer if mode == 1 */
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

        

        via1d0[addr] = byte;

        break;

      default:
        via1d0[addr] = byte;

    }				/* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_via1d0(ADDRESS addr)
{
#ifdef VIA1D0_TIMER_DEBUG
    BYTE read_via1d0_(ADDRESS);
    BYTE retv = read_via1d0_(addr);
    addr &= 0x0f;
    if ((addr > 3 && addr < 10) || app_resources.debugFlag)
	log_message(via1d0_log,
                    "read_via1d0(%x) -> %02x, clk=%d", addr, retv, drive_clk[0]);
    return retv;
}
BYTE REGPARM1 read_via1d0_(ADDRESS addr)
{
#endif
    BYTE byte = 0xff;
    CLOCK rclk = drive_clk[0];

    addr &= 0xf;

    if (addr >= VIA_T1CL && addr <= VIA_IER) { 
        if (via1d0tai && (via1d0tai <= drive_clk[0]))
	    int_via1d0t1(drive_clk[0] - via1d0tai);
        if (via1d0tbi && (via1d0tbi <= drive_clk[0]))
	    int_via1d0t2(drive_clk[0] - via1d0tbi);
    }

    switch (addr) {

      case VIA_PRA:		/* port A */
        via1d0ifr &= ~VIA_IM_CA1;
        if ((via1d0[VIA_PCR] & 0x0a) != 0x02) {
            via1d0ifr &= ~VIA_IM_CA2;
        }
        if(IS_CA2_HANDSHAKE()) {
            ca2_state = 0;
            VIA_SET_CA2( ca2_state )
            if(IS_CA2_PULSE_MODE()) {
                ca2_state = 1;
                VIA_SET_CA2( ca2_state )
            }
        }
        if (via1d0ier & (VIA_IM_CA1 | VIA_IM_CA2)) 
	    update_via1d0irq();

      case VIA_PRA_NHS:	/* port A, no handshake */
        /* WARNING: this pin reads the voltage of the output pins, not
           the ORA value as the other port. Value read might be different
           from what is expected due to excessive load. */
#ifdef VIA1D0_NEED_LATCHING
	if (IS_PA_INPUT_LATCH()) {
	    byte = via1d0_ila;
	} else {

    if (drive[0].type == DRIVE_TYPE_1571) {
        BYTE tmp;
        if (drive[0].byte_ready_active == 0x6)
            drive_rotate_disk(&drive[0]);
        tmp = (drive[0].byte_ready ? 0 : 0x80)
            | (drive[0].current_half_track == 2 ? 0 : 1);
        return (tmp & ~via1d0[VIA_DDRA])
            | (via1d0[VIA_PRA] & via1d0[VIA_DDRA]);
    }
    if (drive[0].type == DRIVE_TYPE_2031) {
/*
	if (drive[1].enable)
	    drive1_cpu_execute();
*/
        if (parallel_debug) {
	    printf("read_pra(is_out=%d, parallel_bus=%02x, ddra=%02x\n",
		parieee_is_out, parallel_bus, via1d0[VIA_DDRA]);
	}
        byte = parieee_is_out ? 0xff : ~parallel_bus;
        return (byte & ~via1d0[VIA_DDRA]) | (via1d0[VIA_PRA] & via1d0[VIA_DDRA]);
    }
    byte = (drive[0].parallel_cable_enabled
            ? parallel_cable_drive_read((((addr == VIA_PRA) &&
                                          (via1d0[VIA_PCR] & 0xe) == 0xa))
                                        ? 1 : 0)
            : ((via1d0[VIA_PRA] & via1d0[VIA_DDRA])
               | (0xff & ~via1d0[VIA_DDRA])) );
	}
#else

    if (drive[0].type == DRIVE_TYPE_1571) {
        BYTE tmp;
        if (drive[0].byte_ready_active == 0x6)
            drive_rotate_disk(&drive[0]);
        tmp = (drive[0].byte_ready ? 0 : 0x80)
            | (drive[0].current_half_track == 2 ? 0 : 1);
        return (tmp & ~via1d0[VIA_DDRA])
            | (via1d0[VIA_PRA] & via1d0[VIA_DDRA]);
    }
    if (drive[0].type == DRIVE_TYPE_2031) {
/*
	if (drive[1].enable)
	    drive1_cpu_execute();
*/
        if (parallel_debug) {
	    printf("read_pra(is_out=%d, parallel_bus=%02x, ddra=%02x\n",
		parieee_is_out, parallel_bus, via1d0[VIA_DDRA]);
	}
        byte = parieee_is_out ? 0xff : ~parallel_bus;
        return (byte & ~via1d0[VIA_DDRA]) | (via1d0[VIA_PRA] & via1d0[VIA_DDRA]);
    }
    byte = (drive[0].parallel_cable_enabled
            ? parallel_cable_drive_read((((addr == VIA_PRA) &&
                                          (via1d0[VIA_PCR] & 0xe) == 0xa))
                                        ? 1 : 0)
            : ((via1d0[VIA_PRA] & via1d0[VIA_DDRA])
               | (0xff & ~via1d0[VIA_DDRA])) );
#endif
	via1d0_ila = byte;
	return byte;

      case VIA_PRB:		/* port B */
        via1d0ifr &= ~VIA_IM_CB1;
        if ((via1d0[VIA_PCR] & 0xa0) != 0x20)
            via1d0ifr &= ~VIA_IM_CB2;
        if (via1d0ier & (VIA_IM_CB1 | VIA_IM_CB2)) 
	    update_via1d0irq();

        /* WARNING: this pin reads the ORA for output pins, not
           the voltage on the pins as the other port. */
#ifdef VIA1D0_NEED_LATCHING
	if (IS_PB_INPUT_LATCH()) {
	    byte = via1d0_ilb;
	} else {

    if (iec_info != NULL) {
	byte = ((via1d0[VIA_PRB] & 0x1a) | iec_info->drive_port) ^ 0x85;
    } else {
        if (drive[0].type == DRIVE_TYPE_2031) {
/*
	   if (drive[1].enable)
		drive1_cpu_execute();
*/
           byte = 0xff;
           if (parieee_is_out) {
               /* talk enable */
               if (parallel_nrfd) byte &= 0xfd ;
               if (parallel_ndac) byte &= 0xfb ;
           } else {
               /* listener */
               if (parallel_eoi) byte &= 0xf7 ;
               if (parallel_dav) byte &= 0xbf ;
           }
           if (!parallel_atn) byte &= 0x7f;
           if (parallel_debug) {
 	       printf("read_prb(is_out=%d, byte=%02x, prb=%02x, ddrb=%02x\n",
		   parieee_is_out, byte, via1d0[VIA_PRB], via1d0[VIA_DDRB]);
	   }
           byte = (byte & ~via1d0[VIA_DDRB]) | (via1d0[VIA_PRB] & via1d0[VIA_DDRB]);
           if (!ca2_state) {
               byte &= 0xfe /* 0xff */;  /* byte & 3 + 8 -> device-no */
               byte &= 0xfd /* 0xff */;  /* device-no switche */
	       if (parallel_debug) {
		   printf("read with ca2_state = 0 -> byte=%02x\n", byte);
	       }
           }
	   if (parallel_debug) {
		printf("       -> byte=%02x\n", byte);
	   }
        } else {
           byte = ((via1d0[VIA_PRB] & 0x1a) | iec_drive0_read()) ^ 0x85;
        }
    }
	}
#else

    if (iec_info != NULL) {
	byte = ((via1d0[VIA_PRB] & 0x1a) | iec_info->drive_port) ^ 0x85;
    } else {
        if (drive[0].type == DRIVE_TYPE_2031) {
/*
	   if (drive[1].enable)
		drive1_cpu_execute();
*/
           byte = 0xff;
           if (parieee_is_out) {
               /* talk enable */
               if (parallel_nrfd) byte &= 0xfd ;
               if (parallel_ndac) byte &= 0xfb ;
           } else {
               /* listener */
               if (parallel_eoi) byte &= 0xf7 ;
               if (parallel_dav) byte &= 0xbf ;
           }
           if (!parallel_atn) byte &= 0x7f;
           if (parallel_debug) {
 	       printf("read_prb(is_out=%d, byte=%02x, prb=%02x, ddrb=%02x\n",
		   parieee_is_out, byte, via1d0[VIA_PRB], via1d0[VIA_DDRB]);
	   }
           byte = (byte & ~via1d0[VIA_DDRB]) | (via1d0[VIA_PRB] & via1d0[VIA_DDRB]);
           if (!ca2_state) {
               byte &= 0xfe /* 0xff */;  /* byte & 3 + 8 -> device-no */
               byte &= 0xfd /* 0xff */;  /* device-no switche */
	       if (parallel_debug) {
		   printf("read with ca2_state = 0 -> byte=%02x\n", byte);
	       }
           }
	   if (parallel_debug) {
		printf("       -> byte=%02x\n", byte);
	   }
        } else {
           byte = ((via1d0[VIA_PRB] & 0x1a) | iec_drive0_read()) ^ 0x85;
        }
    }
#endif
	via1d0_ilb = byte;
        byte = (byte & ~via1d0[VIA_DDRB]) | (via1d0[VIA_PRB] & via1d0[VIA_DDRB]);

        if (via1d0[VIA_ACR] & 0x80) {
            update_via1d0tal(rclk);
            byte = (byte & 0x7f) | (((via1d0pb7 ^ via1d0pb7x) | via1d0pb7o) ? 0x80 : 0);
        }
        return byte;

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        via1d0ifr &= ~VIA_IM_T1;
        update_via1d0irq();
        return via1d0ta() & 0xff;

      case VIA_T1CH /*TIMER_AH */ :	/* timer A high */
        return (via1d0ta() >> 8) & 0xff;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        via1d0ifr &= ~VIA_IM_T2;
        update_via1d0irq();
        return via1d0tb() & 0xff;

      case VIA_T2CH /*TIMER_BH */ :	/* timer B high */
        return (via1d0tb() >> 8) & 0xff;

      case VIA_SR:		/* Serial Port Shift Register */
        return (via1d0[addr]);

        /* Interrupts */

      case VIA_IFR:		/* Interrupt Flag Register */
        {
            BYTE t = via1d0ifr;
            if (via1d0ifr & via1d0ier /*[VIA_IER] */ )
                t |= 0x80;
            return (t);
        }

      case VIA_IER:		/* 6522 Interrupt Control Register */
        return (via1d0ier /*[VIA_IER] */  | 0x80);

    }				/* switch */

    return (via1d0[addr]);
}

BYTE REGPARM1 peek_via1d0(ADDRESS addr)
{
    CLOCK rclk = drive_clk[0];

    addr &= 0xf;

    if (via1d0tai && (via1d0tai <= drive_clk[0]))
	int_via1d0t1(drive_clk[0] - via1d0tai);
    if (via1d0tbi && (via1d0tbi <= drive_clk[0]))
	int_via1d0t2(drive_clk[0] - via1d0tbi);

    switch (addr) {
      case VIA_PRA:
        return read_via1d0(VIA_PRA_NHS);

      case VIA_PRB:		/* port B */
        {
            BYTE byte;
#ifdef VIA1D0_NEED_LATCHING
	    if (IS_PB_INPUT_LATCH()) {
	        byte = via1d0_ilb;
	    } else {

    if (iec_info != NULL) {
	byte = ((via1d0[VIA_PRB] & 0x1a) | iec_info->drive_port) ^ 0x85;
    } else {
        if (drive[0].type == DRIVE_TYPE_2031) {
/*
	   if (drive[1].enable)
		drive1_cpu_execute();
*/
           byte = 0xff;
           if (parieee_is_out) {
               /* talk enable */
               if (parallel_nrfd) byte &= 0xfd ;
               if (parallel_ndac) byte &= 0xfb ;
           } else {
               /* listener */
               if (parallel_eoi) byte &= 0xf7 ;
               if (parallel_dav) byte &= 0xbf ;
           }
           if (!parallel_atn) byte &= 0x7f;
           if (parallel_debug) {
 	       printf("read_prb(is_out=%d, byte=%02x, prb=%02x, ddrb=%02x\n",
		   parieee_is_out, byte, via1d0[VIA_PRB], via1d0[VIA_DDRB]);
	   }
           byte = (byte & ~via1d0[VIA_DDRB]) | (via1d0[VIA_PRB] & via1d0[VIA_DDRB]);
           if (!ca2_state) {
               byte &= 0xfe /* 0xff */;  /* byte & 3 + 8 -> device-no */
               byte &= 0xfd /* 0xff */;  /* device-no switche */
	       if (parallel_debug) {
		   printf("read with ca2_state = 0 -> byte=%02x\n", byte);
	       }
           }
	   if (parallel_debug) {
		printf("       -> byte=%02x\n", byte);
	   }
        } else {
           byte = ((via1d0[VIA_PRB] & 0x1a) | iec_drive0_read()) ^ 0x85;
        }
    }
	    }
#else

    if (iec_info != NULL) {
	byte = ((via1d0[VIA_PRB] & 0x1a) | iec_info->drive_port) ^ 0x85;
    } else {
        if (drive[0].type == DRIVE_TYPE_2031) {
/*
	   if (drive[1].enable)
		drive1_cpu_execute();
*/
           byte = 0xff;
           if (parieee_is_out) {
               /* talk enable */
               if (parallel_nrfd) byte &= 0xfd ;
               if (parallel_ndac) byte &= 0xfb ;
           } else {
               /* listener */
               if (parallel_eoi) byte &= 0xf7 ;
               if (parallel_dav) byte &= 0xbf ;
           }
           if (!parallel_atn) byte &= 0x7f;
           if (parallel_debug) {
 	       printf("read_prb(is_out=%d, byte=%02x, prb=%02x, ddrb=%02x\n",
		   parieee_is_out, byte, via1d0[VIA_PRB], via1d0[VIA_DDRB]);
	   }
           byte = (byte & ~via1d0[VIA_DDRB]) | (via1d0[VIA_PRB] & via1d0[VIA_DDRB]);
           if (!ca2_state) {
               byte &= 0xfe /* 0xff */;  /* byte & 3 + 8 -> device-no */
               byte &= 0xfd /* 0xff */;  /* device-no switche */
	       if (parallel_debug) {
		   printf("read with ca2_state = 0 -> byte=%02x\n", byte);
	       }
           }
	   if (parallel_debug) {
		printf("       -> byte=%02x\n", byte);
	   }
        } else {
           byte = ((via1d0[VIA_PRB] & 0x1a) | iec_drive0_read()) ^ 0x85;
        }
    }
#endif
            byte = (byte & ~via1d0[VIA_DDRB]) | (via1d0[VIA_PRB] & via1d0[VIA_DDRB]);
            if (via1d0[VIA_ACR] & 0x80) {
                update_via1d0tal(rclk);
                byte = (byte & 0x7f) | (((via1d0pb7 ^ via1d0pb7x) | via1d0pb7o) ? 0x80 : 0);
            }
            return byte;
        }

        /* Timers */

      case VIA_T1CL /*TIMER_AL */ :	/* timer A low */
        return via1d0ta() & 0xff;

      case VIA_T2CL /*TIMER_BL */ :	/* timer B low */
        return via1d0tb() & 0xff;

      default:
        break;
    }				/* switch */

    return read_via1d0(addr);
}


/* ------------------------------------------------------------------------- */

int int_via1d0t1(long offset)
{
#ifdef VIA1D0_TIMER_DEBUG
    if (app_resources.debugFlag)
	log_message(via1d0_log, "via1d0 timer A interrupt");
#endif

    if (!(via1d0[VIA_ACR] & 0x40)) {	/* one-shot mode */
#ifdef VIA1D0_TIMER_DEBUG
	log_message(via1d0_log, "VIA1D0 Timer A interrupt -- one-shot mode: next int won't happen");
#endif
	drive0_unset_alarm(A_VIA1D0T1);	/*int_clk[I_VIA1D0T1] = 0; */
	via1d0tai = 0;
    } else {			/* continuous mode */
	/* load counter with latch value */
	via1d0tai += via1d0tal + 2;
	drive0_set_alarm_clk(A_VIA1D0T1, via1d0tai);
    }
    via1d0ifr |= VIA_IM_T1;
    update_via1d0irq();

    /* TODO: toggle PB7? */
    return 0;			/*(viaier & VIA_IM_T1) ? 1:0; */
}

/*
 * Timer B is always in one-shot mode
 */

int int_via1d0t2(long offset)
{
#ifdef VIA1D0_TIMER_DEBUG
    if (app_resources.debugFlag)
	log_message(via1d0_log, "VIA1D0 timer B interrupt.");
#endif
    drive0_unset_alarm(A_VIA1D0T2);	/*int_clk[I_VIA1D0T2] = 0; */
    via1d0tbi = 0;

    via1d0ifr |= VIA_IM_T2;
    update_via1d0irq();

    return 0;
}

void via1d0_prevent_clk_overflow(CLOCK sub)
{
    unsigned int t;
    t = (via1d0tau - (drive_clk[0] + sub)) & 0xffff;
    via1d0tau = drive_clk[0] + t;
    t = (via1d0tbu - (drive_clk[0] + sub)) & 0xffff;
    via1d0tbu = drive_clk[0] + t;
    if (via1d0tai)
	via1d0tai -= sub;
}

/*------------------------------------------------------------------------*/

/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 0/0
 */

static char snap_module_name[] = "VIA1D0";
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

int via1d0_write_snapshot_module(snapshot_t * p)
{
    snapshot_module_t *m;

    if (via1d0tai && (via1d0tai <= drive_clk[0]))
        int_via1d0t1(drive_clk[0] - via1d0tai);
    if (via1d0tbi && (via1d0tbi <= drive_clk[0]))
        int_via1d0t2(drive_clk[0] - via1d0tbi);

    m = snapshot_module_create(p, snap_module_name,
                               VIA_DUMP_VER_MAJOR, VIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    snapshot_module_write_byte(m, via1d0[VIA_PRA]);
    snapshot_module_write_byte(m, via1d0[VIA_DDRA]);
    snapshot_module_write_byte(m, via1d0[VIA_PRB]);
    snapshot_module_write_byte(m, via1d0[VIA_DDRB]);

    snapshot_module_write_word(m, via1d0tal);
    snapshot_module_write_word(m, via1d0ta());
    snapshot_module_write_byte(m, via1d0[VIA_T2LL]);
    snapshot_module_write_word(m, via1d0tb());

    snapshot_module_write_byte(m, (via1d0tai ? 0x80 : 0)
					| (via1d0tbi ? 0x40 : 0) );

    snapshot_module_write_byte(m, via1d0[VIA_SR]);
    snapshot_module_write_byte(m, via1d0[VIA_ACR]);
    snapshot_module_write_byte(m, via1d0[VIA_PCR]);

    snapshot_module_write_byte(m, via1d0ifr);
    snapshot_module_write_byte(m, via1d0ier);

						/* FIXME! */
    snapshot_module_write_byte(m, (((via1d0pb7 ^ via1d0pb7x) | via1d0pb7o) ? 0x80 : 0));
    snapshot_module_write_byte(m, 0);		/* SRHBITS */

    snapshot_module_write_byte(m, (ca2_state ? 0x80 : 0) 
				| (cb2_state ? 0x40 : 0));

    snapshot_module_write_byte(m, via1d0_ila);
    snapshot_module_write_byte(m, via1d0_ilb);

    snapshot_module_close(m);

    return 0;
}

int via1d0_read_snapshot_module(snapshot_t * p)
{
    BYTE vmajor, vminor;
    BYTE byte;
    WORD word;
    ADDRESS addr;
    CLOCK rclk = drive_clk[0];
    snapshot_module_t *m;

    m = snapshot_module_open(p, snap_module_name, &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (vmajor != VIA_DUMP_VER_MAJOR) {
        log_error(via1d0_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  vmajor, vminor, VIA_DUMP_VER_MAJOR, VIA_DUMP_VER_MINOR);
        snapshot_module_close(m);
        return -1;
    }

    drive0_unset_alarm(A_VIA1D0T1);
    drive0_unset_alarm(A_VIA1D0T2);
    via1d0tai = 0;
    via1d0tbi = 0;

    snapshot_module_read_byte(m, &via1d0[VIA_PRA]);
    snapshot_module_read_byte(m, &via1d0[VIA_DDRA]);
    snapshot_module_read_byte(m, &via1d0[VIA_PRB]);
    snapshot_module_read_byte(m, &via1d0[VIA_DDRB]);
    {
        addr = VIA_DDRA;
	byte = via1d0[VIA_PRA] | ~via1d0[VIA_DDRA];

    iec_info = iec_get_drive_port();
    if (drive[0].type == DRIVE_TYPE_1571) {
        drive_set_1571_sync_factor(byte & 0x20, 0);        
        drive_set_1571_side((byte >> 2) & 1, 0);
    } else
    if (drive[0].type == DRIVE_TYPE_2031) {
       parallel_drv0_set_bus(parieee_is_out ? byte : 0);
    }
    if (drive[0].parallel_cable_enabled && drive[0].type == DRIVE_TYPE_1541)
        parallel_cable_drive0_write(byte, 0);
	oldpa = byte;

	addr = VIA_DDRB;
	byte = via1d0[VIA_PRB] | ~via1d0[VIA_DDRB];

    if (iec_info != NULL) {
        iec_info->drive_data = ~byte;
        iec_info->drive_bus = (((iec_info->drive_data << 3) & 0x40)
            | ((iec_info->drive_data << 6)
            & ((~iec_info->drive_data ^ iec_info->cpu_bus) << 3) & 0x80));
        iec_info->cpu_port = iec_info->cpu_bus & iec_info->drive_bus
            & iec_info->drive2_bus;
        iec_info->drive_port = iec_info->drive2_port = (((iec_info->cpu_port >> 4) & 0x4)
            | (iec_info->cpu_port >> 7)
            | ((iec_info->cpu_bus << 3) & 0x80));
    } else {
        if (drive[0].type == DRIVE_TYPE_2031) {
            parieee_is_out = byte & 0x10;
            parallel_drv0_set_bus(parieee_is_out ? oldpa : 0);

            parallel_drv0_set_eoi( parieee_is_out && !(byte & 0x08) );
            parallel_drv0_set_dav( parieee_is_out && !(byte & 0x40) );
            parallel_drv0_set_ndac( ((!parieee_is_out) && (!(byte & 0x04)))
                               || (parallel_atn && (!(byte & 0x01)))
                               || ((!parallel_atn) && (byte & 0x01)));
            parallel_drv0_set_nrfd( ((!parieee_is_out) && (!(byte & 0x02)))
                               || (parallel_atn && (!(byte & 0x01)))
                               || ((!parallel_atn) && (byte & 0x01)));
        } else {
            iec_drive0_write(~byte);
	}
    }
	oldpb = byte;
    }

    snapshot_module_read_word(m, &word);
    via1d0tal = word;
    via1d0[VIA_T1LL] = via1d0tal & 0xff;
    via1d0[VIA_T1LH] = (via1d0tal >> 8) & 0xff;
    snapshot_module_read_word(m, &word);
    via1d0tau = rclk + word + 2 /* 3 */ + TAUOFFSET;
    via1d0tai = rclk + word + 1;

    snapshot_module_read_byte(m, &via1d0[VIA_T2LL]);
    snapshot_module_read_word(m, &word);
    via1d0tbu = rclk + word + 2 /* 3 */;
    via1d0tbi = rclk + word + 1;

    snapshot_module_read_byte(m, &byte);
    if (byte & 0x80) {
    	drive0_set_alarm_clk(A_VIA1D0T1, via1d0tai);
    } else {
	via1d0tai = 0;
    }
    if (byte & 0x40) {
    	drive0_set_alarm_clk(A_VIA1D0T2, via1d0tbi);
    } else {
	via1d0tbi = 0;
    }

    snapshot_module_read_byte(m, &via1d0[VIA_SR]);
    snapshot_module_read_byte(m, &via1d0[VIA_ACR]);
    snapshot_module_read_byte(m, &via1d0[VIA_PCR]);

    snapshot_module_read_byte(m, &byte);
    via1d0ifr = byte;
    snapshot_module_read_byte(m, &byte);
    via1d0ier = byte;

    via_restore_int(via1d0ifr & via1d0ier & 0x7f);

    /* FIXME! */
    snapshot_module_read_byte(m, &byte);
    via1d0pb7 = byte ? 1 : 0;
    via1d0pb7x = 0;
    via1d0pb7o = 0;
    snapshot_module_read_byte(m, &byte);	/* SRHBITS */

    snapshot_module_read_byte(m, &byte);	/* CABSTATE */
    ca2_state = byte & 0x80;
    cb2_state = byte & 0x40;

    /*  also restores the ca2_state/cb2_state effects if necessary;
       i.e. calls VIA_SET_C*2( c*2_state ) if necessary */
    {
	addr = VIA_PCR;
	byte = via1d0[addr];
	
    }
    {
	addr = VIA_SR;
	byte = via1d0[addr];
	
    }
    {
	addr = VIA_ACR;
	byte = via1d0[addr];
	
    }

    snapshot_module_read_byte(m, &via1d0_ila);
    snapshot_module_read_byte(m, &via1d0_ilb);

    return snapshot_module_close(m);
}


