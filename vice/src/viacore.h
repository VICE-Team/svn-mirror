/*
 * viacore.h - Core functions for VIA emulation.
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

#include "alarm.h"
#include "clkguard.h"
#include "interrupt.h"
#include "log.h"
#include "resources.h"
#include "snapshot.h"
#include "via.h"
#include "vmachine.h"

/* global */

BYTE myvia[16];

/*
 * Local variables
 */

static int myviaifr;		/* Interrupt Flag register for myvia */
static int myviaier;		/* Interrupt Enable register for myvia */

static unsigned int myviatal;	/* current timer A latch value */
static unsigned int myviatbl;	/* current timer B latch value */

static CLOCK myviatau;		/* time when myvia timer A is updated */
static CLOCK myviatbu;		/* time when myvia timer B is updated */
static CLOCK myviatai;		/* time when next timer A alarm is */
static CLOCK myviatbi;		/* time when next timer A alarm is */

static int myviapb7;		/* state of PB7 for pulse output... */
static int myviapb7x;		/* to be xored herewith  */
static int myviapb7o;		/* to be ored herewith  */
static int myviapb7xx;
static int myviapb7sx;

static BYTE oldpa;		/* the actual output on PA (input = high) */
static BYTE oldpb;		/* the actual output on PB (input = high) */

static BYTE myvia_ila;		/* input latch A */
static BYTE myvia_ilb;		/* input latch B */

static int ca2_state;
static int cb2_state;

static log_t myvia_log = LOG_ERR;

/* VIA alarms.  */
static alarm_t myvia_t1_alarm;
static alarm_t myvia_t2_alarm;
