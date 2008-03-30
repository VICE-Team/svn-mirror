/*
 * parallel.c - IEEE488 emulation.
 *
 * Written by
 *  André Fachat (a.fachat@physik.tu-chemnitz.de)
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

/* This file contains the ieee488 emulator.
 * The ieee488 emulator calls (modifed) routines from serial.c
 * to use the standard floppy interface.
 * The current state of the bus and methods to set output lines
 * are exported.
 * This hardware emulation is necessary, as different PET kernels would
 * need different traps. But it's also much faster than the (hardware
 * simulated) serial bus, as it's parallel. So we don't need traps.
 */

/* FIXME: This should have its own log instead of using `LOG_DEFAULT'.  */

#include "vice.h"

#ifdef STDC_HEADERS
#include <stdio.h>
#endif

#include "types.h"
#include "parallel.h"
#include "serial.h"
#include "drive.h"

/* globals */

int parallel_debug = 0;
int parallel_emu = 1;

/* state of the bus lines -> "if(parallel_eoi) { eoi is active }" */
char parallel_eoi = 0;
char parallel_ndac = 0;
char parallel_nrfd = 0;
char parallel_dav = 0;
char parallel_atn = 0;

BYTE parallel_bus = 0xff;	/* data lines */

static int par_status = 0;	/* lower 8 bits = PET par_status, upper bits own */


/***************************************************************************
 * State engine for the parallel bus
 *
 * Names here are as seen from a device, not from the PET
 *
 * Possible States
 *	WaitATN		Wait for ATN, ignore everything else
 *
 *	In1		Wait for DAV low when reading a byte
 *	In2		Wait for DAV high when reading a byte
 *
 * 	OldPet		The PET 3032 doesn't set NRFD and NDAC low
 *			before releasing ATN after a TALK command,
 *			wait for a NDAC low first!
 *
 *	Out1		Wait for NRFD high when sending a byte
 *	Out1a		Wait for NRFD low when sending a byte
 *			Trying to save this didn't work
 *	Out2		Wait for NDAC high when sending a byte
 *
 *
 * Each state reacts on the different line transitions.
 *
 * 	atnlo, atnhi, ndaclo, ndachi, nrfdlo, nrfdhi, davlo, davhi
 *
 *
 * Some common functions are:
 *
 *	ResetBus	Set all lines high
 *	ignore		ignore any transition
 *	unexpected	this transition is unexpected
 *
 *	Go		change the state
 *
 * Globals:
 *
 *	Trans[]		name of the transitions
 *	State[]		jump table
 *	state		actual state
 */

#define	NTRANS		8	/* number of possible transitions */
#define	NSTATE		7

/* States */

#define	WaitATN		0
#define	In1		1
#define	In2		2
#define	OldPet		3
#define	Out1		4
#define	Out1a		5
#define	Out2		6

/* Transitions */

#define	ATNlo		0
#define	ATNhi		1
#define	DAVlo		2
#define	DAVhi		3
#define	NDAClo		4
#define	NDAChi		5
#define	NRFDlo		6
#define	NRFDhi		7

typedef struct State_t {
    const char *name;
    void (*m[NTRANS])(int);
} State_t;

static const char *Trans[NTRANS] = {
	"ATN low", "ATN high", "DAV low", "DAV high",
	"NDAC low", "NDAC high", "NRFD low", "NRFD high"
};

static State_t State[NSTATE];
int state = WaitATN;

#define	Go(a)		state=(a);return
#define	isListening()	((par_status&0xf000)==0x2000)
#define	isTalking()	((par_status&0xf000)==0x4000)

#ifdef PARALLEL_DEBUG_VERBOSE
static void DoTrans(int tr) {
    	if(parallel_debug)
	    log_debug("DoTrans(%s).%s\n",State[state].name, Trans[tr]);
	State[state].m[tr](tr);
    	if(parallel_debug)
	    log_debug(" -> %s\n",State[state].name);
}
#else
#define	DoTrans(a)	State[state].m[(a)]((a))
#endif

static void ResetBus(void) {
	parallel_emu_set_dav(0);
	parallel_emu_set_eoi(0);
	parallel_emu_set_nrfd(0);
	parallel_emu_set_ndac(0);
	parallel_emu_set_bus(0xff);
	par_status = 0;
}

/**************************************************************************
 * transition functions 
 */

static void ignore(int i) {}

static void unexpected(int trans) {
	if(parallel_debug)
            log_warning(LOG_DEFAULT,
                        "IEEE488: unexpected line transition in state %s: %s.",
                        State[state].name, Trans[trans]);
}

static void WATN_atnlo(int tr) {
	parallel_emu_set_ndac(1);
	parallel_emu_set_dav(0);
	parallel_emu_set_eoi(0);
	parallel_emu_set_bus(0xff);
	parallel_emu_set_nrfd(0);
	Go(In1); 
}

#define	In1_atnlo	WATN_atnlo

static void In1_atnhi(int tr) {
	if(par_status & 0xff) {
	  ResetBus();
	  Go(WaitATN);
	} else
	if(isListening()) {
	  Go(In1);
	} else
	if(isTalking()) {
	  ResetBus();
	  if(!parallel_ndac) {	/* old pet... */
	    Go(OldPet);
	  } else {
	    State[OldPet].m[NDAClo](tr);
	    return;
	  }
	} else {
	  if(parallel_debug)
	  log_warning(LOG_DEFAULT, "IEEE488: Ouch, something weird happened: %s got %s",
		State[In1].name, Trans[tr]);
	  ResetBus();
	  Go(WaitATN);
	}
}

static void In1_davlo(int tr) {
	static BYTE b;

	parallel_emu_set_nrfd(1);
	b = parallel_bus;
	parallel_emu_set_ndac(0);

	if(parallel_atn) {
	  par_status = parallelattention(b ^ 0xff);
	} else {
	  par_status = parallelsendbyte(b ^ 0xff);
	}
	if(parallel_debug) log_warning(LOG_DEFAULT, "IEEE488: sendbyte returns %04x",par_status);

	Go(In2);
}

static void In1_ndaclo(int tr) {
	if(!parallel_atn) unexpected(tr);
}

static void In1_nrfdlo(int tr) {
	if(!parallel_atn) unexpected(tr);
}


static void In1_nrfdhi(int tr) {
	unexpected(tr);
}

#define	In2_atnlo	WATN_atnlo

static void In2_atnhi(int a) {	/* atn data transfer interrupted */
	ResetBus();
	Go(WaitATN);		/* ??? */
}

static void In2_davhi(int tr) {
	parallel_emu_set_ndac(1);
	parallel_emu_set_nrfd(0);

 	Go(In1);
}

/* OldPET fixed PET2*** and PET3*** IEEE, as well as CBM610 */

#define	OPet_atnlo	WATN_atnlo

static void OPet_ndaclo(int tr) {
	if(!parallel_nrfd) {
	  State[Out1].m[NRFDhi](tr);
	  return;
	} else {
	  Go(Out1);
	}
}

/* this is for CBM 610 only */

static void OPet_nrfdlo(int tr) {
	if(parallel_debug) log_warning(LOG_DEFAULT, "OPet_nrfdlo()");
	State[Out1].m[NRFDhi](tr);
}

#define	Out1_atnlo	WATN_atnlo

static void Out1_nrfdhi(int tr) {
	static BYTE b;

	par_status = parallelreceivebyte(&b, 1);

	if(par_status & 0x40) {
	  parallel_emu_set_eoi(1);
	} else {
	  parallel_emu_set_eoi(0);
	}

	parallel_emu_set_bus(b ^ 0xff);

	parallel_emu_set_dav(1);

	Go(Out1a);
}

#define	Out1a_atnlo	WATN_atnlo

static void Out1a_nrfdlo(int tr) {
	Go(Out2);
}

static void Out1a_ndachi(int tr) {
	ResetBus();
	Go(WaitATN);
}

#define	Out2_atnlo	WATN_atnlo

static void Out2_ndachi(int tr) {
	static BYTE b;

	parallel_emu_set_dav(0);
	parallel_emu_set_eoi(0);
	parallel_emu_set_bus(0xff);

	par_status = parallelreceivebyte(&b, 0);
	if(par_status & 0xff) {
	  ResetBus();
	  Go(WaitATN);
	} else {
	  Go(Out1);
	}
}

/**************************************************************************
 * State table
 *
 */

static State_t State[NSTATE] = {
	{ "WaitATN", 	{ WATN_atnlo,  ignore, 	   ignore,     ignore,     ignore,      ignore,       ignore,       ignore } },
	{ "In1", 	{ In1_atnlo,   In1_atnhi,  In1_davlo,  unexpected, In1_ndaclo,  unexpected,   In1_nrfdlo,   In1_nrfdhi } },
	{ "In2", 	{ In2_atnlo,   In2_atnhi,  unexpected, In2_davhi,  unexpected,  unexpected,   unexpected,   unexpected } },
	{ "OldPet", 	{ OPet_atnlo,  unexpected, unexpected, unexpected, OPet_ndaclo, unexpected,   OPet_nrfdlo,   unexpected } },
	{ "Out1", 	{ Out1_atnlo,  unexpected, unexpected, unexpected, ignore,      unexpected,   unexpected,   Out1_nrfdhi } },
	{ "Out1a", 	{ Out1a_atnlo, unexpected, unexpected, unexpected, unexpected,  Out1a_ndachi, Out1a_nrfdlo, unexpected } },
	{ "Out2", 	{ Out2_atnlo,  unexpected, unexpected, unexpected, unexpected,  Out2_ndachi,  unexpected,   unexpected } }
};

/**************************************************************************
 * methods to set handshake lines for the devices
 *
 */

void parallel_set_eoi( char mask ) 
{
    parallel_eoi |= mask;
}

void parallel_clr_eoi( char mask ) 
{
    parallel_eoi &= mask;
}

void parallel_set_atn( char mask ) 
{
    char old = parallel_atn;
    parallel_atn |= mask;

    if(parallel_debug && !old) 
	log_warning(LOG_DEFAULT, "set_atn(%02x) -> ATNlo", mask);

    /* if ATN went active, signal to attached devices */
    if (!old) {
        if (parallel_emu) {
	    DoTrans(ATNlo);
        }
        if (drive[0].enable) {
            drive0_parallel_set_atn(1);
        }
        if (drive[1].enable) {
            drive1_parallel_set_atn(1);
        }
    }
}

void parallel_clr_atn( char mask ) 
{
    char old = parallel_atn;
    parallel_atn &= mask;

    if(parallel_debug && old && !parallel_atn) 
	log_warning(LOG_DEFAULT, "clr_atn(%02x) -> ATNhi", ~mask);

    /* if ATN went inactive, signal to attached devices */
    if (old && !parallel_atn) {
        if (parallel_emu) {
	    DoTrans(ATNhi);
        }
	if (drive[0].enable) {
	    drive0_parallel_set_atn(0);
	}
	if (drive[1].enable) {
	    drive1_parallel_set_atn(0);
	}
    }
}

void parallel_restore_set_atn( char mask ) 
{
    char old = parallel_atn;
    parallel_atn |= mask;

    if(parallel_debug && !old) 
	log_warning(LOG_DEFAULT, "set_atn(%02x) -> ATNlo", mask);

    /* we do not send IRQ signals to chips on restore */
}

void parallel_restore_clr_atn( char mask ) 
{
    char old = parallel_atn;
    parallel_atn &= mask;

    if(parallel_debug && old && !parallel_atn) 
	log_warning(LOG_DEFAULT, "clr_atn(%02x) -> ATNhi", ~mask);

    /* we do not send IRQ signals to chips on restore */
}

void parallel_set_dav( char mask ) 
{
    char old = parallel_dav;
    parallel_dav |= mask;

    if(parallel_debug && !old) 
	log_warning(LOG_DEFAULT, "set_dav(%02x) -> DAVlo", mask);

    if (parallel_emu && !old) {
	DoTrans(DAVlo);
    }
}

void parallel_clr_dav( char mask ) 
{
    char old = parallel_dav;
    parallel_dav &= mask;

    if(parallel_debug && old && !parallel_dav) 
	log_warning(LOG_DEFAULT, "clr_dav(%02x) -> DAVhi", ~mask);

    if (parallel_emu && old && !parallel_dav) {
	DoTrans(DAVhi);
    }
}

void parallel_set_nrfd( char mask ) 
{
    char old = parallel_nrfd;
    parallel_nrfd |= mask;

    if(parallel_debug && !old) 
	log_warning(LOG_DEFAULT, "set_nrfd(%02x) -> NRFDlo", mask);

    if (parallel_emu && !old) {
	DoTrans(NRFDlo);
    }
}

void parallel_clr_nrfd( char mask ) 
{
    char old = parallel_nrfd;
    parallel_nrfd &= mask;

    if(parallel_debug && old && !parallel_nrfd) 
	log_warning(LOG_DEFAULT, "clr_nrfd(%02x) -> NRFDhi", ~mask);

    if (parallel_emu && old && !parallel_nrfd) {
	DoTrans(NRFDhi);
    }
}

void parallel_set_ndac( char mask ) 
{
    char old = parallel_ndac;
    parallel_ndac |= mask;

    if(parallel_debug && !old) 
	log_warning(LOG_DEFAULT, "set_ndac(%02x) -> NDAClo", mask);

    if (parallel_emu && !old) {
	DoTrans(NDAClo);
    }
}

void parallel_clr_ndac( char mask ) 
{
    char old = parallel_ndac;
    parallel_ndac &= mask;

    if(parallel_debug && old && !parallel_ndac) 
	log_warning(LOG_DEFAULT, "clr_ndac(%02x) -> NDAChi", ~mask);

    if (parallel_emu && old && !parallel_ndac) {
	DoTrans(NDAChi);
    }
}

/**************************************************************************
 * methods to set data lines
 */

static BYTE par_emu_bus = 0xff;
static BYTE par_cpu_bus = 0xff;
static BYTE par_drv0_bus = 0xff;
static BYTE par_drv1_bus = 0xff;

void parallel_emu_set_bus( BYTE b ) 
{
    par_emu_bus = b;
    parallel_bus = par_emu_bus & par_cpu_bus & par_drv0_bus & par_drv1_bus;
}

void parallel_cpu_set_bus( BYTE b ) 
{
    par_cpu_bus = b;
    parallel_bus = par_emu_bus & par_cpu_bus & par_drv0_bus & par_drv1_bus;
}

void parallel_drv0_set_bus( BYTE b ) 
{
    par_drv0_bus = b;
    parallel_bus = par_emu_bus & par_cpu_bus & par_drv0_bus & par_drv1_bus;
}

void parallel_drv1_set_bus( BYTE b ) 
{
    par_drv1_bus = b;
    parallel_bus = par_emu_bus & par_cpu_bus & par_drv0_bus & par_drv1_bus;
}


