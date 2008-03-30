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

#include "vice.h"

#include <stdio.h>

#include "types.h"
#include "parallel.h"
#include "serial.h"

/* globals */

int pardebug = 0;

/* state of the bus lines - if(par_eoi) { eoi is detected } */
char par_eoi = 0;
char par_ndac = 0;
char par_nrfd = 0;
char par_dav = 0;
char par_atn = 0;

BYTE par_bus = 0;	/* data lines */

int par_status = 0;		/* lower 8 bits = PET par_status, upper bits own */

/* local values */
/* what has been put on the bus from the CPU - to detect changes */
static char par_eoi_out = 0;
static char par_ndac_out = 0;
static char par_nrfd_out = 0;
static char par_dav_out = 0;
static char par_atn_out = 0;
static BYTE par_bus_out = 0;	/* data lines */

/* what has been put on the bus from the device */
static char par_eoi_dev = 0;
static char par_ndac_dev = 0;
static char par_nrfd_dev = 0;
static char par_dav_dev = 0;
static char par_atn_dev = 0;
static BYTE par_bus_dev = 0;	/* data lines */

/* set the real bus line values by 'wire-oring' the signals */
#define	set_atn_out(a)	par_atn_out=(a);par_atn=par_atn_dev|par_atn_out
#define	set_ndac_out(a)	par_ndac_out=(a);par_ndac=par_ndac_dev|par_ndac_out
#define	set_nrfd_out(a)	par_nrfd_out=(a);par_nrfd=par_nrfd_dev|par_nrfd_out
#define	set_dav_out(a)	par_dav_out=(a);par_dav=par_dav_dev|par_dav_out
#define	set_eoi_out(a)	par_eoi_out=(a);par_eoi=par_eoi_dev|par_eoi_out
#define	set_bus_out(a)	par_bus_out=(a);par_bus=par_bus_dev|par_bus_out

/* set the real bus line values by 'wire-oring' the signals */
#define	set_atn_dev(a)	par_atn_dev=(a);par_atn=par_atn_dev|par_atn_out
#define	set_ndac_dev(a)	par_ndac_dev=(a);par_ndac=par_ndac_dev|par_ndac_out
#define	set_nrfd_dev(a)	par_nrfd_dev=(a);par_nrfd=par_nrfd_dev|par_nrfd_out
#define	set_dav_dev(a)	par_dav_dev=(a);par_dav=par_dav_dev|par_dav_out
#define	set_eoi_dev(a)	par_eoi_dev=(a);par_eoi=par_eoi_dev|par_eoi_out

#if 1
#define	set_bus_dev(a)	par_bus_dev=(a);par_bus=par_bus_dev|par_bus_out
#else
void set_bus_dev(int a)
{
    par_bus_dev=(a);
    par_bus=par_bus_dev|par_bus_out;
printf("set_bus_dev: bus_dev = %02x, bus_out = %02x -> bus = %02x\n",
	par_bus_dev, par_bus_out, par_bus);
}
#endif


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
	char	*name;
	void	(*m[NTRANS])(int);
} State_t;

static char *Trans[NTRANS] = {
	"ATN low", "ATN high", "DAV low", "DAV high",
	"NDAC low", "NDAC high", "NRFD low", "NRFD high"
};

static State_t State[NSTATE];
static int state = WaitATN;

#define	Go(a)		state=(a);return
#define	isListening()	((par_status&0xf000)==0x2000)
#define	isTalking()	((par_status&0xf000)==0x4000)

/*#define	DoTrans(a)	State[state].m[(a)](a)}*/
static void DoTrans(int tr) {
/*
    	if(pardebug) {
	    printf("DoTrans(%s).%s\n",State[state].name, Trans[tr]);
	    fflush(stdout);
	}
*/
	State[state].m[tr](tr);
/*
    	if(pardebug) {
	    printf(" -> %s\n",State[state].name);
	    fflush(stdout);
	}
*/
}

static void ResetBus(void) {
	set_atn_dev(0);
	set_dav_dev(0);
	set_eoi_dev(0);
	set_nrfd_dev(0);
	set_ndac_dev(0);
	set_bus_dev(0);
	par_status = 0;
}

static void ignore(int i) {}

static void unexpected(int trans) {
	if(pardebug)
	printf("IEEE488: unexpected line transition in state %s: %s\n",
		State[state].name, Trans[trans]);
}

static void WATN_atnlo(int tr) {
	set_ndac_dev(1);
	set_dav_dev(0);
	set_eoi_dev(0);
	set_bus_dev(0);
	set_nrfd_dev(0);
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
	  if(!par_ndac) {	/* old pet... */
	    Go(OldPet);
	  } else {
	    State[OldPet].m[NDAClo](tr);
	    return;
	  }
	} else {
	  if(pardebug)
	  printf("IEEE488: Ouch, something weird happened: %s got %s\n",
		State[In1].name, Trans[tr]);
	  ResetBus();
	  Go(WaitATN);
	}
}

static void In1_davlo(int tr) {
	static BYTE b;

	set_nrfd_dev(1);
	b = par_bus;
	set_ndac_dev(0);

	if(par_atn) {
	  par_status = parallelattention(b);
	} else {
	  par_status = parallelsendbyte(b);
	}
	if(pardebug) printf("IEEE488: sendbyte returns %04x\n",par_status);

	Go(In2);
}

static void In1_ndaclo(int tr) {
	if(!par_atn) unexpected(tr);
}

static void In1_nrfdlo(int tr) {
	if(!par_atn) unexpected(tr);
}


static void In1_nrfdhi(int tr) {
	unexpected(tr);
}

#define	In2_atnlo	WATN_atnlo

static void In2_atnhi(a) {	/* atn data transfer interrupted */
	ResetBus();
	Go(WaitATN);		/* ??? */
}

static void In2_davhi(int tr) {
	set_ndac_dev(1);
	set_nrfd_dev(0);

 	Go(In1);
}

/* OldPET fixed PET2*** and PET3*** IEEE, as well as CBM610 */

#define	OPet_atnlo	WATN_atnlo

static void OPet_ndaclo(int tr) {
	if(!par_nrfd) {
	  State[Out1].m[NRFDhi](tr);
	  return;
	} else {
	  Go(Out1);
	}
}

/* this is for CBM 610 only */

static void OPet_nrfdlo(int tr) {
	if(pardebug) printf("OPet_nrfdlo()\n");
	State[Out1].m[NRFDhi](tr);
}

#define	Out1_atnlo	WATN_atnlo

static void Out1_nrfdhi(int tr) {
	static BYTE b;

	par_status = parallelreceivebyte(&b, 1);
/*if(par_status & 0xff)
	  printf("IEEE488: Out1_nrfdhi: par_status=%x\n",par_status);*/

	if(par_status & 0x40) {
	  set_eoi_dev(1);
	} else {
	  set_eoi_dev(0);
	}

	set_bus_dev(b^255);

	set_dav_dev(1);

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

	set_dav_dev(0);
	set_eoi_dev(0);
	set_bus_dev(0);

	par_status = parallelreceivebyte(&b, 0);
/*if(par_status & 0xff)
	  printf("IEEE488: Out2_ndachi: par_status=%x\n",par_status);*/
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
 * methods to set output lines for the CPU
 *
 */

void par_set_atn( char b ) {
	if(b==par_atn_out) return; set_atn_out(b);
	if(pardebug) printf("par_set_atn(%d)\n",b);
	if(b) DoTrans(ATNlo); else DoTrans(ATNhi); return;
}

void par_set_ndac( char b ) {
	if(b==par_ndac_out) return; set_ndac_out(b);
	if(pardebug) printf("par_set_ndac(%d)\n",b);
	if(b) DoTrans(NDAClo); else DoTrans(NDAChi); return;
}

void par_set_nrfd( char b ) {
	if(b==par_nrfd_out) return; set_nrfd_out(b);
	if(pardebug) printf("par_set_nrfd(%d)\n",b);
	if(b) DoTrans(NRFDlo); else DoTrans(NRFDhi); return;
}

void par_set_dav( char b ) {
	if(b==par_dav_out) return; set_dav_out(b);
	if(b) DoTrans(DAVlo); else DoTrans(DAVhi); return;
}

void par_set_bus( BYTE b ) {
	if(b==par_bus_out) return; set_bus_out(b);
	if(pardebug) printf("par_set_bus(%d)\n",b);
}

void par_set_eoi( char b ) {
	if(b==par_eoi_out) return; set_eoi_out(b);
	if(pardebug) printf("par_set_eoi(%d)\n",b);
}


