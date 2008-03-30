
/*
 * ../../../vice-0.15.0.11+/src/pet/petpia2.c
 * This file is generated from ../../../vice-0.15.0.11+/src/pia-tmpl.c and ../../../vice-0.15.0.11+/src/pet/petpia2.def,
 * Do not edit!
 */
/*
 * pia-tmpl.c -- PIA chip emulation.
 *
 * Written by
 *  Jouko Valta (jopi@stekt.oulu.fi)
 *  Andre' Fachat (fachat@physik.tu-chemnitz.de)
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

#include "vice.h"

#include <stdio.h>
#include <time.h>

#include "types.h"

#include "cmdline.h"
#include "mem.h"
#include "interrupt.h"
#include "parallel.h"
#include "resources.h"
#include "vmachine.h"


#include "parallel.h"
#include "petpia.h"

#define my_set_int(a)                                                   \
        maincpu_set_irq(I_PIA1, (a)? IK_IRQ : IK_NONE)

#define my_restore_int(a)                                               \
        set_int_noclk(&maincpu_int_status, I_PIA1, (a) ? IK_IRQ : IK_NONE)


/* ------------------------------------------------------------------------- */



#define PIA_SET_CA2(a)  do {                                            \
                            par_set_ndac((a)?0:1);                      \
                        } while(0)

#define PIA_SET_CB2(a)  do {						\
			    par_set_dav((a)?0:1);			\
			} while(0)


/* ------------------------------------------------------------------------- */

#define	IS_CA2_HANDSHAKE()	((pia2.ctrl_a & 0x30) == 0x20)
#define	IS_CA2_PULSE_MODE()	((pia2.ctrl_a & 0x38) == 0x28)
#define	IS_CA2_TOGGLE_MODE()	((pia2.ctrl_a & 0x38) == 0x20)

#define	IS_CB2_HANDSHAKE()	((pia2.ctrl_b & 0x30) == 0x20)
#define	IS_CB2_PULSE_MODE()	((pia2.ctrl_b & 0x38) == 0x28)
#define	IS_CB2_TOGGLE_MODE()	((pia2.ctrl_b & 0x38) == 0x20)

#define P_PORT_A	0
#define P_CTRL_A	1
#define P_PORT_B	2
#define P_CTRL_B	3

typedef struct {
    BYTE  port_a;	/* output register, i.e. what has been written by
			   the CPU. input is assembled at read time */
    BYTE  ddr_a;	/* PIA Port A DDR */
    BYTE  ctrl_a;

    BYTE  port_b;
    BYTE  ddr_b;	/* PIA Port B DDR */
    BYTE  ctrl_b;

    int ca_state;
    int cb_state;
} piareg;


/*
 * Local variables
 */

static piareg  pia2;
static int is_peek_access = 0;

/* ------------------------------------------------------------------------- */

void    reset_pia2(void)
{
   /* clear _all_ internal registers */

   pia2.ctrl_a = 0;	/* PIA 1 Port A Control */
   pia2.ctrl_b = 0;	/* PIA 1 Port B Control */
   pia2.ddr_a = 0;	/* PIA 1 Port A DDR */
   pia2.ddr_b = 0;	/* PIA 1 Port B DDR */
   pia2.port_a = 255;	/* PIA 1 Port A input; nothing to read from keyboard */
   pia2.port_b = 255;	/* PIA 1 Port B input; nothing to read from keyboard */


    par_set_bus(0xff);	/* all data lines high, because of input mode */

   PIA_SET_CA2(1);
   pia2.ca_state = 1;
   PIA_SET_CB2(1);
   pia2.cb_state = 1;

   is_peek_access = 0;

   my_set_int(0);
}

static void pia2_update_irq(void) {
    if( 0 
	    || ((pia2.ctrl_a & 0x81) == 0x81)
	    || ((pia2.ctrl_a & 0x68) == 0x48)
	    || ((pia2.ctrl_b & 0x81) == 0x81)
	    || ((pia2.ctrl_b & 0x68) == 0x48)
	) {
        my_set_int(1);
    } else {
        my_set_int(0);
    }
}


/* control line flag support. Used for PET IRQ input.
 * this currently relies on each edge being called only once,
 * otherwise multiple IRQs could occur. */

void signal_pia2(int line, int edge) {
    switch(line) {
    case PIA_SIG_CA1:
	if ( ((pia2.ctrl_a & 0x02) ? PIA_SIG_RISE : PIA_SIG_FALL) == edge) {
	    pia2.ctrl_a |= 0x80;
	    pia2_update_irq();
	    if (IS_CA2_TOGGLE_MODE()) {
		PIA_SET_CA2(1);
		pia2.ca_state = 1;
	    }
	}
    case PIA_SIG_CB1:
        /*printf("signal_pia2(line=%d, edge=%d, ctrl=%02x)\n",
						line,edge,pia2.ctrl_b);*/
	if ( ((pia2.ctrl_b & 0x02) ? PIA_SIG_RISE : PIA_SIG_FALL) == edge) {
	    pia2.ctrl_b |= 0x80;
	    pia2_update_irq();
	    if (IS_CB2_TOGGLE_MODE()) {
		PIA_SET_CB2(1);
		pia2.cb_state = 1;
	    }
	}
	break;
    }
}


/* ------------------------------------------------------------------------- */
/* PIA */

void REGPARM2 store_pia2(ADDRESS addr, BYTE byte)
{

    addr &= 3;

#if 0
    printf("store pia2 [%x] %x\n", (int) addr, (int) byte);
#endif

    switch (addr) {

      case P_PORT_A: /* port A */
	if (pia2.ctrl_a & 4) {
	    pia2.port_a = byte;
	} else {
	    pia2.ddr_a = byte;
	}
        byte = pia2.port_a | ~pia2.ddr_a;
	
	break;

      case P_PORT_B: /* port B */
	if (pia2.ctrl_b & 4) {
	    pia2.port_b = byte;
	} else {
	    pia2.ddr_b = byte;
	}
        byte = pia2.port_b | ~pia2.ddr_b;

    par_set_bus(byte^255);
	if (IS_CB2_HANDSHAKE()) {
	    PIA_SET_CB2(0);
	    pia2.cb_state = 0;
	    if (IS_CB2_PULSE_MODE()) {
	        PIA_SET_CB2(1);
	        pia2.cb_state = 1;
	    }
	}
	break;

	/* Control */

      case P_CTRL_A: /* Control A */
	if ((byte & 0x38) == 0x30 ) {	/* set output low */
	    PIA_SET_CA2(0);
	    pia2.ca_state = 0;
	} else 
	if ((byte & 0x38) == 0x38) {	/* set output high */
	    PIA_SET_CA2(1);
	    pia2.ca_state = 1;
	} else 				/* change to toggle/pulse */
	if ((pia2.ctrl_a & 0x30) == 0x30) {
	    PIA_SET_CA2(1);
	    pia2.ca_state = 1;
	} 

	pia2.ctrl_a = (pia2.ctrl_a & 0xc0) | (byte & 0x3f);

	if(pia2.ctrl_a & 0x20) pia2.ctrl_a &= 0xbf;

	pia2_update_irq();

	break;

      case P_CTRL_B: /* Control B */
	if ((byte & 0x38) == 0x30 ) {	/* set output low */
	    PIA_SET_CB2(0);
	    pia2.cb_state = 0;
	} else 
	if ((byte & 0x38) == 0x38) {	/* set output high */
	    PIA_SET_CB2(1);
	    pia2.cb_state = 1;
	} else 				/* change to toggle/pulse */
	if ((pia2.ctrl_b & 0x30) == 0x30) {
	    PIA_SET_CB2(1);
	    pia2.cb_state = 1;
	} 

	pia2.ctrl_b = (pia2.ctrl_b & 0xc0) | (byte & 0x3f);

	if(pia2.ctrl_b & 0x20) pia2.ctrl_b &= 0xbf;

	pia2_update_irq();

	break;
    }  /* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_pia2(ADDRESS addr)
{
    static BYTE byte = 0xff;

    addr &= 3;

#if 0
    printf("read pia2 [%d]  [%02x %02x] [%02x] [%02x %02x] [%02x]\n",
           addr,
           pia2.port_a, pia2.ddr_a, pia2.ctrl_a,
           pia2.port_b, pia2.ddr_b, pia2.ctrl_b);
#endif

    switch (addr) {

      case P_PORT_A: /* port A */
	if (pia2.ctrl_a & 4) {

	    if(!is_peek_access) {
	        pia2.ctrl_a &= 0x3f;		/* Clear CA1,CA2 IRQ */
	        pia2_update_irq();
	    }



	    if (pardebug)
		printf("read pia2 port A %x, par_bus=%x, gives %x\n",
			pia2.port_a, par_bus,
			(par_bus & ~pia2.ddr_a) |(pia2.port_a & pia2.ddr_a));

	    byte = (par_bus & ~pia2.ddr_a) | (pia2.port_a & pia2.ddr_a);
	    return byte;
	}
	return (pia2.ddr_a);

      case P_PORT_B: /* port B */
	if (pia2.ctrl_b & 4) {

	    if(!is_peek_access) {
	        pia2.ctrl_b &= 0x3f;		/* Clear CB1,CB2 IRQ */
	        pia2_update_irq();
	    }



	    if (pardebug)
		printf("read pia2 port B %x, par_bus=%x, gives %x\n",
			pia2.port_b, par_bus,
			(~(par_bus|pia2.ddr_b)) |(pia2.port_b & pia2.ddr_b));

	    byte = (~pia2.ddr_b) |(pia2.port_b & pia2.ddr_b);
	    return (byte);
	}
	return (pia2.ddr_a);

	/* Control */

      case P_CTRL_A: /* Control A */
	return (pia2.ctrl_a);

      case P_CTRL_B: /* Control B */
	return (pia2.ctrl_b);
	break;

    }  /* switch */

    return (0xFF);
}


BYTE REGPARM1 peek_pia2(ADDRESS addr)
{
    BYTE t;
    is_peek_access = 1;
    t = read_pia2(addr);
    is_peek_access = 0;
    return t;
}


/*------------------------------------------------------------------------*/

/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 0/0
 */

#define PIA_DUMP_VER_MAJOR      0
#define PIA_DUMP_VER_MINOR      0

static char snap_module_name[] = "PIA2";

/*
 * The dump data:
 *
 * UBYTE        ORA
 * UBYTE        DDRA
 * UBYTE	CTRLA
 * UBYTE        ORB
 * UBYTE	DDRB
 * UBYTE	CTRLB
 * UBYTE	CABSTATE	Bit 7 = state of CA2, Bit 6 = state of CB2
 *
 */

/* FIXME!!!  Error check.  */

int pia2_write_snapshot_module(snapshot_t * p)
{
    snapshot_module_t *m;

    m = snapshot_module_create(p, snap_module_name,
                               PIA_DUMP_VER_MAJOR, PIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    snapshot_module_write_byte(m, pia2.port_a);
    snapshot_module_write_byte(m, pia2.ddr_a);
    snapshot_module_write_byte(m, pia2.ctrl_a);

    snapshot_module_write_byte(m, pia2.port_b);
    snapshot_module_write_byte(m, pia2.ddr_b);
    snapshot_module_write_byte(m, pia2.ctrl_b);

    snapshot_module_write_byte(m, (pia2.ca_state ? 0x80 : 0)
					| (pia2.cb_state ? 0x40 : 0) );

    snapshot_module_close(m);

    return 0;
}

int pia2_read_snapshot_module(snapshot_t * p)
{
    BYTE vmajor, vminor;
    BYTE byte;
    snapshot_module_t *m;

    my_restore_int(0);		/* just in case */

    m = snapshot_module_open(p, snap_module_name, &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (vmajor != PIA_DUMP_VER_MAJOR) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_read_byte(m, &pia2.port_a);
    snapshot_module_read_byte(m, &pia2.ddr_a);
    snapshot_module_read_byte(m, &pia2.ctrl_a);

    snapshot_module_read_byte(m, &pia2.port_b);
    snapshot_module_read_byte(m, &pia2.ddr_b);
    snapshot_module_read_byte(m, &pia2.ctrl_b);

    snapshot_module_read_byte(m, &byte);
    pia2.ca_state = (byte & 0x80) ? 1 : 0;
    pia2.cb_state = (byte & 0x80) ? 1 : 0;

    PIA_SET_CA2(pia2.ca_state);
    PIA_SET_CB2(pia2.cb_state);

    byte = pia2.port_a | ~pia2.ddr_a;
    

    byte = pia2.port_b | ~pia2.ddr_b;

    par_set_bus(byte^255);

    if( 0
            || ((pia2.ctrl_a & 0x81) == 0x81)
            || ((pia2.ctrl_a & 0x68) == 0x48)
            || ((pia2.ctrl_b & 0x81) == 0x81)
            || ((pia2.ctrl_b & 0x68) == 0x48)
        ) {
        my_restore_int(1);
    }

    return snapshot_module_close(m);
}

