/*
 * piacore.c -- PIA chip emulation.
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

static BYTE pia_last_read = 0;

void mypia_init(void)
{
   if (mypia_log == LOG_ERR)
       mypia_log = log_open(MYPIA_NAME);
}

void reset_mypia(void)
{
   /* clear _all_ internal registers */

   mypia.ctrl_a = 0;	/* PIA 1 Port A Control */
   mypia.ctrl_b = 0;	/* PIA 1 Port B Control */
   mypia.ddr_a = 0;	/* PIA 1 Port A DDR */
   mypia.ddr_b = 0;	/* PIA 1 Port B DDR */
   mypia.port_a = 255;	/* PIA 1 Port A input; nothing to read from keyboard */
   mypia.port_b = 255;	/* PIA 1 Port B input; nothing to read from keyboard */

   pia_reset();

   pia_set_ca2(1);
   mypia.ca_state = 1;
   pia_set_cb2(1);
   mypia.cb_state = 1;

   is_peek_access = 0;

   my_set_int(0);
}

static void mypia_update_irq(void) {
    if( 0 
	    || ((mypia.ctrl_a & 0x81) == 0x81)
	    || ((mypia.ctrl_a & 0x68) == 0x48)
	    || ((mypia.ctrl_b & 0x81) == 0x81)
	    || ((mypia.ctrl_b & 0x68) == 0x48)
	) {
        my_set_int(1);
    } else {
        my_set_int(0);
    }
}


/* control line flag support. Used for PET IRQ input.
 * this currently relies on each edge being called only once,
 * otherwise multiple IRQs could occur. */

void signal_mypia(int line, int edge) {
    switch(line) {
    case PIA_SIG_CA1:
	if ( ((mypia.ctrl_a & 0x02) ? PIA_SIG_RISE : PIA_SIG_FALL) == edge) {
	    mypia.ctrl_a |= 0x80;
	    mypia_update_irq();
	    if (IS_CA2_TOGGLE_MODE()) {
		pia_set_ca2(1);
		mypia.ca_state = 1;
	    }
	}
    case PIA_SIG_CB1:
	if ( ((mypia.ctrl_b & 0x02) ? PIA_SIG_RISE : PIA_SIG_FALL) == edge) {
	    mypia.ctrl_b |= 0x80;
	    mypia_update_irq();
	    if (IS_CB2_TOGGLE_MODE()) {
		pia_set_cb2(1);
		mypia.cb_state = 1;
	    }
	}
	break;
    }
}


/* ------------------------------------------------------------------------- */
/* PIA */

void REGPARM2 store_mypia(ADDRESS addr, BYTE byte)
{

    if (mycpu_rmw_flag) {
        myclk --;
        mycpu_rmw_flag = 0;
        store_mypia(addr, pia_last_read);
        myclk ++;
    }

    addr &= 3;

    switch (addr) {

      case P_PORT_A: /* port A */
	if (mypia.ctrl_a & 4) {
	    mypia.port_a = byte;
	} else {
	    mypia.ddr_a = byte;
	}
        byte = mypia.port_a | ~mypia.ddr_a;
	store_pa(byte);
	break;

      case P_PORT_B: /* port B */
	if (mypia.ctrl_b & 4) {
	    mypia.port_b = byte;
	} else {
	    mypia.ddr_b = byte;
	}
        byte = mypia.port_b | ~mypia.ddr_b;
	store_pb(byte);
	if (IS_CB2_HANDSHAKE()) {
	    pia_set_cb2(0);
	    mypia.cb_state = 0;
	    if (IS_CB2_PULSE_MODE()) {
	        pia_set_cb2(1);
	        mypia.cb_state = 1;
	    }
	}
	break;

	/* Control */

      case P_CTRL_A: /* Control A */
	if ((byte & 0x38) == 0x30 ) {	/* set output low */
	    pia_set_ca2(0);
	    mypia.ca_state = 0;
	} else 
	if ((byte & 0x38) == 0x38) {	/* set output high */
	    pia_set_ca2(1);
	    mypia.ca_state = 1;
	} else 				/* change to toggle/pulse */
	if ((mypia.ctrl_a & 0x30) == 0x30) {
	    pia_set_ca2(1);
	    mypia.ca_state = 1;
	} 

	mypia.ctrl_a = (mypia.ctrl_a & 0xc0) | (byte & 0x3f);

	if(mypia.ctrl_a & 0x20) mypia.ctrl_a &= 0xbf;

	mypia_update_irq();

	break;

      case P_CTRL_B: /* Control B */
	if ((byte & 0x38) == 0x30 ) {	/* set output low */
	    pia_set_cb2(0);
	    mypia.cb_state = 0;
	} else 
	if ((byte & 0x38) == 0x38) {	/* set output high */
	    pia_set_cb2(1);
	    mypia.cb_state = 1;
	} else 				/* change to toggle/pulse */
	if ((mypia.ctrl_b & 0x30) == 0x30) {
	    pia_set_cb2(1);
	    mypia.cb_state = 1;
	} 

	mypia.ctrl_b = (mypia.ctrl_b & 0xc0) | (byte & 0x3f);

	if(mypia.ctrl_b & 0x20) mypia.ctrl_b &= 0xbf;

	mypia_update_irq();

	break;
    }  /* switch */
}


/* ------------------------------------------------------------------------- */

BYTE REGPARM1 read_mypia(ADDRESS addr)
{
    static BYTE byte = 0xff;

    addr &= 3;

    switch (addr) {

      case P_PORT_A: /* port A */
	if (mypia.ctrl_a & 4) {

	    if(!is_peek_access) {
	        mypia.ctrl_a &= 0x3f;		/* Clear CA1,CA2 IRQ */
	        mypia_update_irq();
	    }
	    /* WARNING: this pin reads the voltage of the output pins, not 
	       the ORA value as the other port. Value read might be different
	       from what is expected due to excessive electrical load on the 
	       pin. */
	    byte = read_pa();
	    pia_last_read = byte;
	    return byte;
	}
	pia_last_read = (mypia.ddr_a);
	return pia_last_read;
	break;

      case P_PORT_B: /* port B */
	if (mypia.ctrl_b & 4) {

	    if(!is_peek_access) {
	        mypia.ctrl_b &= 0x3f;		/* Clear CB1,CB2 IRQ */
	        mypia_update_irq();
	    }

	    /* WARNING: this pin reads the ORA for output pins, not 
	       the voltage on the pins as the other port. */
	    byte = read_pb();
	    pia_last_read = (byte & ~mypia.ddr_b) 
			| (mypia.port_b & mypia.ddr_b);
	    return pia_last_read;
	}
	pia_last_read = (mypia.ddr_a);
	return pia_last_read;
	break;

	/* Control */

      case P_CTRL_A: /* Control A */
	pia_last_read = (mypia.ctrl_a);
	return pia_last_read;
	break;

      case P_CTRL_B: /* Control B */
	pia_last_read = (mypia.ctrl_b);
	return pia_last_read;
	break;

    }  /* switch */

    /* should never happen */
    return (0xFF);
}


BYTE REGPARM1 peek_mypia(ADDRESS addr)
{
    BYTE t;
    is_peek_access = 1;
    t = read_mypia(addr);
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

static char snap_module_name[] = MYPIA_NAME;

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

int mypia_write_snapshot_module(snapshot_t * p)
{
    snapshot_module_t *m;

    m = snapshot_module_create(p, snap_module_name,
                               PIA_DUMP_VER_MAJOR, PIA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    snapshot_module_write_byte(m, mypia.port_a);
    snapshot_module_write_byte(m, mypia.ddr_a);
    snapshot_module_write_byte(m, mypia.ctrl_a);

    snapshot_module_write_byte(m, mypia.port_b);
    snapshot_module_write_byte(m, mypia.ddr_b);
    snapshot_module_write_byte(m, mypia.ctrl_b);

    snapshot_module_write_byte(m, (mypia.ca_state ? 0x80 : 0)
					| (mypia.cb_state ? 0x40 : 0) );

    snapshot_module_close(m);

    return 0;
}

int mypia_read_snapshot_module(snapshot_t * p)
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

    snapshot_module_read_byte(m, &mypia.port_a);
    snapshot_module_read_byte(m, &mypia.ddr_a);
    snapshot_module_read_byte(m, &mypia.ctrl_a);

    snapshot_module_read_byte(m, &mypia.port_b);
    snapshot_module_read_byte(m, &mypia.ddr_b);
    snapshot_module_read_byte(m, &mypia.ctrl_b);

    snapshot_module_read_byte(m, &byte);
    mypia.ca_state = (byte & 0x80) ? 1 : 0;
    mypia.cb_state = (byte & 0x80) ? 1 : 0;

    pia_set_ca2(mypia.ca_state);
    pia_set_cb2(mypia.cb_state);

    byte = mypia.port_a | ~mypia.ddr_a;
    undump_pa(byte);

    byte = mypia.port_b | ~mypia.ddr_b;
    undump_pb(byte);

    if( 0
            || ((mypia.ctrl_a & 0x81) == 0x81)
            || ((mypia.ctrl_a & 0x68) == 0x48)
            || ((mypia.ctrl_b & 0x81) == 0x81)
            || ((mypia.ctrl_b & 0x68) == 0x48)
        ) {
        my_restore_int(1);
    }

    return snapshot_module_close(m);
}

