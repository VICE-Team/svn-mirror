
/*
 * ../../../src/pet/petpia1.c
 * This file is generated from ../../../src/pia-tmpl.c and ../../../src/pet/petpia1.def,
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


#include "crtc.h"
#include "kbd.h"
#include "parallel.h"
#include "drive.h"
#include "pets.h"
#include "petmem.h"
#include "petpia.h"

#define	my_set_int(a)							\
	maincpu_set_irq(I_PIA1, (a)? IK_IRQ : IK_NONE)

#define	my_restore_int(a)						\
	set_int_noclk(&maincpu_int_status, I_PIA1, (a) ? IK_IRQ : IK_NONE)


/* ------------------------------------------------------------------------- */



/* PIA resources.  */

/* Flag: is the diagnostic pin enabled?  */
static int diagnostic_pin_enabled;

static int set_diagnostic_pin_enabled(resource_value_t v)
{
    diagnostic_pin_enabled = (int) v;
    return 0;
}

static resource_t resources[] = {
    { "DiagPin", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &diagnostic_pin_enabled, set_diagnostic_pin_enabled },
    { NULL }
};

int pia1_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static cmdline_option_t cmdline_options[] = {
    { "-diagpin", SET_RESOURCE, 0, NULL, NULL, "DiagPin", (resource_value_t) 1,
      NULL, "Enable userport diagnostic pin" },
    { "+diagpin", SET_RESOURCE, 0, NULL, NULL, "DiagPin", (resource_value_t) 1,
      NULL, "Disable userport diagnostic pin" },
    { NULL }
};

int pia1_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

static int tape1_sense = 0;

void mem_set_tape_sense(int v)
{
    tape1_sense = v;
}

#define	PIA_SET_CA2(a)	do { 						\
			    parallel_cpu_set_eoi((a)?0:1); 		\
			    if(pet.pet2k) 				\
				crtc_screen_enable((a)?1:0);		\
			} while(0)

#define	PIA_SET_CB2(a)	do {} while(0)


/* ------------------------------------------------------------------------- */

#define	IS_CA2_HANDSHAKE()	((pia1.ctrl_a & 0x30) == 0x20)
#define	IS_CA2_PULSE_MODE()	((pia1.ctrl_a & 0x38) == 0x28)
#define	IS_CA2_TOGGLE_MODE()	((pia1.ctrl_a & 0x38) == 0x20)

#define	IS_CB2_HANDSHAKE()	((pia1.ctrl_b & 0x30) == 0x20)
#define	IS_CB2_PULSE_MODE()	((pia1.ctrl_b & 0x38) == 0x28)
#define	IS_CB2_TOGGLE_MODE()	((pia1.ctrl_b & 0x38) == 0x20)

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

static piareg  pia1;
static int is_peek_access = 0;

/* ------------------------------------------------------------------------- */

void    reset_pia1(void)
{
   /* clear _all_ internal registers */

   pia1.ctrl_a = 0;	/* PIA 1 Port A Control */
   pia1.ctrl_b = 0;	/* PIA 1 Port B Control */
   pia1.ddr_a = 0;	/* PIA 1 Port A DDR */
   pia1.ddr_b = 0;	/* PIA 1 Port B DDR */
   pia1.port_a = 255;	/* PIA 1 Port A input; nothing to read from keyboard */
   pia1.port_b = 255;	/* PIA 1 Port B input; nothing to read from keyboard */



   PIA_SET_CA2(1);
   pia1.ca_state = 1;
   PIA_SET_CB2(1);
   pia1.cb_state = 1;

   is_peek_access = 0;

   my_set_int(0);
}

static void pia1_update_irq(void) {
    if( 0 
	    || ((pia1.ctrl_a & 0x81) == 0x81)
	    || ((pia1.ctrl_a & 0x68) == 0x48)
	    || ((pia1.ctrl_b & 0x81) == 0x81)
	    || ((pia1.ctrl_b & 0x68) == 0x48)
	) {
        my_set_int(1);
    } else {
        my_set_int(0);
    }
}


/* control line flag support. Used for PET IRQ input.
 * this currently relies on each edge being called only once,
 * otherwise multiple IRQs could occur. */

void signal_pia1(int line, int edge) {
    switch(line) {
    case PIA_SIG_CA1:
	if ( ((pia1.ctrl_a & 0x02) ? PIA_SIG_RISE : PIA_SIG_FALL) == edge) {
	    pia1.ctrl_a |= 0x80;
	    pia1_update_irq();
	    if (IS_CA2_TOGGLE_MODE()) {
		PIA_SET_CA2(1);
		pia1.ca_state = 1;
	    }
	}
    case PIA_SIG_CB1:
        /*printf("signal_pia1(line=%d, edge=%d, ctrl=%02x)\n",
						line,edge,pia1.ctrl_b);*/
	if ( ((pia1.ctrl_b & 0x02) ? PIA_SIG_RISE : PIA_SIG_FALL) == edge) {
	    pia1.ctrl_b |= 0x80;
	    pia1_update_irq();
	    if (IS_CB2_TOGGLE_MODE()) {
		PIA_SET_CB2(1);
		pia1.cb_state = 1;
	    }
	}
	break;
    }
}


/* ------------------------------------------------------------------------- */
/* PIA */

void REGPARM2 store_pia1(ADDRESS addr, BYTE byte)
{

    addr &= 3;

#if 0
    printf("store pia1 [%x] %x\n", (int) addr, (int) byte);
#endif

    switch (addr) {

      case P_PORT_A: /* port A */
	if (pia1.ctrl_a & 4) {
	    pia1.port_a = byte;
	} else {
	    pia1.ddr_a = byte;
	}
        byte = pia1.port_a | ~pia1.ddr_a;
	
	break;

      case P_PORT_B: /* port B */
	if (pia1.ctrl_b & 4) {
	    pia1.port_b = byte;
	} else {
	    pia1.ddr_b = byte;
	}
        byte = pia1.port_b | ~pia1.ddr_b;
	
	if (IS_CB2_HANDSHAKE()) {
	    PIA_SET_CB2(0);
	    pia1.cb_state = 0;
	    if (IS_CB2_PULSE_MODE()) {
	        PIA_SET_CB2(1);
	        pia1.cb_state = 1;
	    }
	}
	break;

	/* Control */

      case P_CTRL_A: /* Control A */
	if ((byte & 0x38) == 0x30 ) {	/* set output low */
	    PIA_SET_CA2(0);
	    pia1.ca_state = 0;
	} else 
	if ((byte & 0x38) == 0x38) {	/* set output high */
	    PIA_SET_CA2(1);
	    pia1.ca_state = 1;
	} else 				/* change to toggle/pulse */
	if ((pia1.ctrl_a & 0x30) == 0x30) {
	    PIA_SET_CA2(1);
	    pia1.ca_state = 1;
	} 

	pia1.ctrl_a = (pia1.ctrl_a & 0xc0) | (byte & 0x3f);

	if(pia1.ctrl_a & 0x20) pia1.ctrl_a &= 0xbf;

	pia1_update_irq();

	break;

      case P_CTRL_B: /* Control B */
	if ((byte & 0x38) == 0x30 ) {	/* set output low */
	    PIA_SET_CB2(0);
	    pia1.cb_state = 0;
	} else 
	if ((byte & 0x38) == 0x38) {	/* set output high */
	    PIA_SET_CB2(1);
	    pia1.cb_state = 1;
	} else 				/* change to toggle/pulse */
	if ((pia1.ctrl_b & 0x30) == 0x30) {
	    PIA_SET_CB2(1);
	    pia1.cb_state = 1;
	} 

	pia1.ctrl_b = (pia1.ctrl_b & 0xc0) | (byte & 0x3f);

	if(pia1.ctrl_b & 0x20) pia1.ctrl_b &= 0xbf;

	pia1_update_irq();

	break;
    }  /* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_pia1(ADDRESS addr)
{
    static BYTE byte = 0xff;

    addr &= 3;

#if 0
    printf("read pia1 [%d]  [%02x %02x] [%02x] [%02x %02x] [%02x]\n",
           addr,
           pia1.port_a, pia1.ddr_a, pia1.ctrl_a,
           pia1.port_b, pia1.ddr_b, pia1.ctrl_b);
#endif

    switch (addr) {

      case P_PORT_A: /* port A */
	if (pia1.ctrl_a & 4) {

	    if(!is_peek_access) {
	        pia1.ctrl_a &= 0x3f;		/* Clear CA1,CA2 IRQ */
	        pia1_update_irq();
	    }
	    /* WARNING: this pin reads the voltage of the output pins, not 
	       the ORA value as the other port. Value read might be different
	       from what is expected due to excessive load. */

    if (drive[0].enable)
        drive0_cpu_execute();
    if (drive[1].enable)
        drive1_cpu_execute();

    byte = 0xff
	- (tape1_sense ? 16 : 0)
	- (parallel_eoi ? 64 : 0)
	- ((diagnostic_pin_enabled || superpet_diag()) ? 128 : 0);
    byte = ((byte & ~pia1.ddr_a) | (pia1.port_a & pia1.ddr_a));
	    return byte;
	}
	return (pia1.ddr_a);

      case P_PORT_B: /* port B */
	if (pia1.ctrl_b & 4) {

	    if(!is_peek_access) {
	        pia1.ctrl_b &= 0x3f;		/* Clear CB1,CB2 IRQ */
	        pia1_update_irq();
	    }

	    /* WARNING: this pin reads the ORA for output pins, not 
	       the voltage on the pins as the other port. */

	{
	    int     row;
	    BYTE    j = 0xFF;

	    row = pia1.port_a & 15;

	    if (row < KBD_ROWS)
		j = ~keyarr[row];

#if 0
            printf("read pia1 port B %d\n", j);
            printf("a: %x b:%x  ca: %x cb: %x joy: %x\n",
                   (int) pia1.port_a, (int) j,
                   (int) pia1.ddr_a, (int) pia1.ddr_b, joy[2]);
#endif
#if (defined(DEBUG_PIA) || defined(KBDBUG))
	    if (j < 255)
                printf("%02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X - row %d  %02x\n",
                       keyarr[0], keyarr[1], keyarr[2], keyarr[3], keyarr[4],
                       keyarr[5], keyarr[6], keyarr[7], keyarr[8], keyarr[9],
                       row, j);
#endif

            byte = j;

	}
	    return (byte & ~pia1.ddr_b) | (pia1.port_b & pia1.ddr_b);
	}
	return (pia1.ddr_a);

	/* Control */

      case P_CTRL_A: /* Control A */
	return (pia1.ctrl_a);

      case P_CTRL_B: /* Control B */
	return (pia1.ctrl_b);
	break;

    }  /* switch */

    return (0xFF);
}


BYTE REGPARM1 peek_pia1(ADDRESS addr)
{
    BYTE t;
    is_peek_access = 1;
    t = read_pia1(addr);
    is_peek_access = 0;
    return t;
}


/*------------------------------------------------------------------------*/

/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 0/0
 */

#define PIA_DUMP_VER_MAJOR      1
#define PIA_DUMP_VER_MINOR      0

static char snap_module_name[] = "PIA1";

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

int pia1_write_snapshot_module(snapshot_t * p)
{
    snapshot_module_t *m;

    m = snapshot_module_create(p, snap_module_name,
                               PIA_DUMP_VER_MAJOR, PIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    snapshot_module_write_byte(m, pia1.port_a);
    snapshot_module_write_byte(m, pia1.ddr_a);
    snapshot_module_write_byte(m, pia1.ctrl_a);

    snapshot_module_write_byte(m, pia1.port_b);
    snapshot_module_write_byte(m, pia1.ddr_b);
    snapshot_module_write_byte(m, pia1.ctrl_b);

    snapshot_module_write_byte(m, (pia1.ca_state ? 0x80 : 0)
					| (pia1.cb_state ? 0x40 : 0) );

    snapshot_module_close(m);

    return 0;
}

int pia1_read_snapshot_module(snapshot_t * p)
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

    snapshot_module_read_byte(m, &pia1.port_a);
    snapshot_module_read_byte(m, &pia1.ddr_a);
    snapshot_module_read_byte(m, &pia1.ctrl_a);

    snapshot_module_read_byte(m, &pia1.port_b);
    snapshot_module_read_byte(m, &pia1.ddr_b);
    snapshot_module_read_byte(m, &pia1.ctrl_b);

    snapshot_module_read_byte(m, &byte);
    pia1.ca_state = (byte & 0x80) ? 1 : 0;
    pia1.cb_state = (byte & 0x80) ? 1 : 0;

    PIA_SET_CA2(pia1.ca_state);
    PIA_SET_CB2(pia1.cb_state);

    byte = pia1.port_a | ~pia1.ddr_a;
    

    byte = pia1.port_b | ~pia1.ddr_b;
    

    if( 0
            || ((pia1.ctrl_a & 0x81) == 0x81)
            || ((pia1.ctrl_a & 0x68) == 0x48)
            || ((pia1.ctrl_b & 0x81) == 0x81)
            || ((pia1.ctrl_b & 0x68) == 0x48)
        ) {
        my_restore_int(1);
    }

    return snapshot_module_close(m);
}

