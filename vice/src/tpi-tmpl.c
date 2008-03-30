/*
 * tpi-tmpl.c - TPI 6525 template
 *
 * Written by
 *   André Fachat (a.fachat@physik.tu-chemnitz.de)
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

#ifdef STDC_HEADERS
#include <stdio.h>
#endif

#include "types.h"
#include "tpi.h"
#include "interrupt.h"
#include "snapshot.h"

INCLUDES

int mytpi_debug = 0;

static BYTE pow2[] = { 1, 2, 4, 8, 16 };

static BYTE irq_previous;
static BYTE irq_stack;

#define	irq_active	tpi[TPI_AIR]
#define	irq_latches	tpi[TPI_PC]
#define	irq_mask	tpi[TPI_DDPC]
#define	irq_mode	(tpi[TPI_CREG] & 1)
#define	irq_priority	(tpi[TPI_CREG] & 2)

#define	IS_CA_MODE()		((tpi[TPI_CREG] & 0x20) == 0x00)
#define	IS_CA_PULSE_MODE()	((tpi[TPI_CREG] & 0x30) == 0x10)
#define	IS_CA_TOGGLE_MODE()	((tpi[TPI_CREG] & 0x30) == 0x00)
#define	IS_CB_MODE()		((tpi[TPI_CREG] & 0x80) == 0x00)
#define	IS_CB_PULSE_MODE()	((tpi[TPI_CREG] & 0xc0) == 0x40)
#define	IS_CB_TOGGLE_MODE()	((tpi[TPI_CREG] & 0xc0) == 0x00)

static BYTE tpi[8];

static BYTE oldpa;
static BYTE oldpb;
static BYTE oldpc;

static BYTE ca_state;
static BYTE cb_state;

/*------------------------------------------------------------------------*/
/* Handle irq stack etc */

static void set_latch_bit(int bit) {
    if(mytpi_debug && !(bit & irq_latches)) {
	printf("mytpi: set_latch_bit(%02x, mask=%02x)\n",bit, irq_mask);
    }

    irq_latches |= bit;

    if(!(irq_mask & bit)) return;

    /* if one IRQ is already active put on stack, if not, trigger CPU int */
    if(irq_priority) {
	if(bit > (irq_latches & ~bit)) {
	    irq_active = bit;
       	    mycpu_set_int(I_TPI1, MYIRQ);
	}
    } else {
        if(!irq_active) {
	    irq_active = bit;
            mycpu_set_int(I_TPI1, MYIRQ);
	}
    }
    irq_stack |= bit;
}

static void pop_irq_state(void) {
    if(mytpi_debug) {
        printf("mytpi: pop_irq_state(latches=%02x, stack=%02x, active=%02x)\n",
                (int)irq_latches, (int)irq_stack, (int)irq_active);
    }
    if(irq_priority) {
   	if(irq_stack) {
	    int i;
	    for(i=4;i>=0;i--) {
		if(irq_stack & pow2[i]) {
		    irq_active = pow2[i];
		    break;
		}
	    }
	}
    }
    mycpu_set_int(I_MYTPI, irq_active ? MYIRQ : 0);
}

static BYTE push_irq_state(void) {
    int old_active;

    old_active = irq_active;

    if(mytpi_debug) {
        printf("mytpi: push_irq_state(latches=%02x, act=%02x, stack=%02x, "
		"mask=%02x)\n", (int)irq_latches, (int)irq_active,
		(int)irq_stack, (int)irq_mask);
    }

    irq_latches &= ~irq_active;
    irq_stack &= ~irq_active;
    irq_active = 0;

    if(!irq_priority) {
	irq_active = irq_stack;
	irq_stack = 0;
    }
    mycpu_set_int(I_TPI1, irq_active ? MYIRQ : 0);
    return old_active;
}

/*------------------------------------------------------------------------*/

void reset_mytpi ( void ) {
	int i;
	for(i=0;i<8;i++) {
	  tpi[0] = 0;
	}

	irq_mask = 0;
	irq_latches = 0;
	irq_previous = 0xff;
	irq_stack = 0;
	irq_active = 0;
        mycpu_set_int(I_MYTPI, 0);

	oldpa = 0xff;
	oldpb = 0xff;
	oldpc = 0xff;

	TPI_SET_CA(0);
	TPI_SET_CB(0);
	ca_state = 0;
	cb_state = 0;

	RESET
}

void store_mytpi ( ADDRESS addr, BYTE byte ) {

	addr &= 0x07;


	switch ( addr ) {
	case TPI_PA:
	case TPI_DDPA:
	    tpi[addr] = byte;
	    byte = tpi[TPI_PA] | ~tpi[TPI_DDPA];
	    STORE_PA
	    oldpa = byte;
	    return;
	case TPI_PB:
	case TPI_DDPB:
	    tpi[addr] = byte;
	    byte = tpi[TPI_PB] | ~tpi[TPI_DDPB];
	    STORE_PB
	    oldpb = byte;
	    if(IS_CB_MODE()) {
		cb_state = 0;
		TPI_SET_CB(0);
		if(IS_CB_PULSE_MODE()) {
		    cb_state = 1;
		    TPI_SET_CB(1);
		}
	    }
	    return;
	case TPI_PC:
	case TPI_DDPC:
	    tpi[addr] = byte;
	    if (irq_mode) {
	        if(addr == TPI_PC) {
	            irq_latches &= byte;
		} else {
		    int i;
		    for(i=4;i>=0;i--) {
                        if(irq_mask & irq_latches & pow2[i]) {
                            set_latch_bit(pow2[i]);
			}
		    }
	        }
	    } else {
	        byte = tpi[TPI_PC] | ~tpi[TPI_DDPC];
	        STORE_PC
		oldpc = byte;
	    }
	    return;
	case TPI_CREG:
	    tpi[addr] = byte;
	    if(mytpi_debug) {
		printf("write %02x to CREG\n",byte);
	    }
	    if(tpi[TPI_CREG] & 0x20) {
		ca_state = (tpi[TPI_CREG] & 0x10);
		TPI_SET_CA( ca_state );
	    } else
	    if(tpi[TPI_CREG] & 0x10) {
		ca_state = 1;
		TPI_SET_CA( 1 );
	    }
	    if(tpi[TPI_CREG] & 0x80) {
		cb_state = (tpi[TPI_CREG] & 0x40);
		TPI_SET_CB( cb_state );
	    } else
	    if(tpi[TPI_CREG] & 0x40) {
		cb_state = 1;
		TPI_SET_CB( 1 );
	    }
	    return;
	case TPI_AIR:
	    pop_irq_state();
	    return;
	}
	tpi[addr] = byte;
}

BYTE read_mytpi ( ADDRESS addr ) {
	BYTE byte = 0xff;
    	switch ( addr ) {
	case TPI_PA:
	    READ_PA
	    if(IS_CA_MODE()) {
		ca_state = 0;
		TPI_SET_CA(0);
		if(IS_CA_PULSE_MODE()) {
		    ca_state = 1;
		    TPI_SET_CA(1);
		}
	    }
	    return byte;
	case TPI_PB:
	    READ_PB
	    return byte;
	case TPI_PC:
	    if(irq_mode) {
		return (irq_latches & 0x1f) | (irq_active ? 0x20 : 0) | 0xc0;
	    } else {
	        READ_PC
		return byte;
	    }
	case TPI_AIR:
	    return push_irq_state();
	default:
	    return tpi[addr];
	}
}

BYTE peek_mytpi (ADDRESS addr) {
	BYTE b = read_mytpi(addr);
	return b;
}


/* Port C can be setup as interrupt input - this collects connected IRQ states
 * and sets IRQ if necessary 
 * Beware: An IRQ line is active low, but for active irqs we here get
 * a state parameter != 0 */
void mytpi_set_int(int bit, int state)
{
    if(bit>=5) return;

    bit = pow2[bit];

    state = !state;

    /* check low-high transition */
    if(state && !(irq_previous & bit)) {
        /* on those two lines the transition can be selected. */
	if((bit & 0x18) && ((bit>>1) & tpi[TPI_CREG])) {
	    set_latch_bit(bit);
	    if((bit & 0x08) && IS_CA_TOGGLE_MODE()) {
		ca_state = 1;
		TPI_SET_CA(1);
	    }
	    if((bit & 0x10) && IS_CB_TOGGLE_MODE()) {
		cb_state = 1;
		TPI_SET_CB(1);
	    }
	}
	irq_previous |= bit;
    } else
    /* check high-low transition */
    if((!state) && (irq_previous & bit)) {
        /* on those two lines the transition can be selected. */
	if((bit & 0x18) && !((bit>>1) & tpi[TPI_CREG])) {
	    set_latch_bit(bit);
	    if((bit & 0x08) && IS_CA_TOGGLE_MODE()) {
		ca_state = 1;
		TPI_SET_CA(1);
	    }
	    if((bit & 0x10) && IS_CB_TOGGLE_MODE()) {
		cb_state = 1;
		TPI_SET_CB(1);
	    }
	}
        /* those three always trigger at high-low */
	if(bit & 0x07) {
	    set_latch_bit(bit);
	}
	irq_previous &= ~bit;
    }
}

void mytpi_restore_int(int bit, int state)
{
    if(bit>=5) return;

    bit = pow2[bit];

    if (state) {
	irq_previous |= bit;
    } else {
	irq_previous &= ~bit;
    }
}


/* -------------------------------------------------------------------------- */

/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 0/0
 */

#define TPI_DUMP_VER_MAJOR      1
#define TPI_DUMP_VER_MINOR      0

/*
 * The dump data:
 *
 * UBYTE	PRA	port A output register
 * UBYTE	PRB
 * UBYTE	PRC
 * UBYTE	DDRA	data register A
 * UBYTE	DDRB
 * UBYTE	DDRC
 * UBYTE	CR
 * UBYTE	AIR
 *
 * UBYTE	STACK	irq sources saved on stack
 * UBYTE	CABSTATE state of CA and CB pins
 */

static const char module_name[] = "MYTPI";

/* FIXME!!!  Error check.  */
int mytpi_write_snapshot_module(snapshot_t *p)
{
    snapshot_module_t *m;
    int byte;

    m = snapshot_module_create(p, module_name,
                               TPI_DUMP_VER_MAJOR, TPI_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    snapshot_module_write_byte(m, tpi[TPI_PA]);
    snapshot_module_write_byte(m, tpi[TPI_PB]);
    snapshot_module_write_byte(m, tpi[TPI_PC]);
    snapshot_module_write_byte(m, tpi[TPI_DDPA]);
    snapshot_module_write_byte(m, tpi[TPI_DDPB]);
    snapshot_module_write_byte(m, tpi[TPI_DDPC]);
    snapshot_module_write_byte(m, tpi[TPI_CREG]);
    snapshot_module_write_byte(m, tpi[TPI_AIR]);

    snapshot_module_write_byte(m, irq_stack);

    snapshot_module_write_byte(m, 
			(ca_state ? 0x80 : 0) | (cb_state ? 0x40 : 0) );

    snapshot_module_close(m);

    return 0;
}

int mytpi_read_snapshot_module(snapshot_t *p)
{
    BYTE vmajor, vminor;
    BYTE byte;
    snapshot_module_t *m;

    mycpu_restore_int(I_MYTPI, 0);  	/* just in case */

    m = snapshot_module_open(p, module_name, &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (vmajor != TPI_DUMP_VER_MAJOR) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_read_byte(m, &tpi[TPI_PA]);
    snapshot_module_read_byte(m, &tpi[TPI_PB]);
    snapshot_module_read_byte(m, &tpi[TPI_PC]);
    snapshot_module_read_byte(m, &tpi[TPI_DDPA]);
    snapshot_module_read_byte(m, &tpi[TPI_DDPB]);
    snapshot_module_read_byte(m, &tpi[TPI_DDPC]);
    snapshot_module_read_byte(m, &tpi[TPI_CREG]);
    snapshot_module_read_byte(m, &tpi[TPI_AIR]);

    snapshot_module_read_byte(m, &irq_stack);

    snapshot_module_read_byte(m, &byte);
    ca_state = byte & 0x80;
    cb_state = byte & 0x40;

    {
        byte = tpi[TPI_PA] | ~tpi[TPI_DDPA];
        UNDUMP_PA
        oldpa = byte;

        byte = tpi[TPI_PB] | ~tpi[TPI_DDPB];
        UNDUMP_PB
        oldpb = byte;

	if (!irq_mode) {
            byte = tpi[TPI_PC] | ~tpi[TPI_DDPC];
            UNDUMP_PC
            oldpc = byte;
	}
    }

    TPI_SET_CA( ca_state );
    TPI_SET_CB( cb_state );

    mycpu_restore_int(I_MYTPI, irq_active ? MYIRQ : 0); 

    if (snapshot_module_close(m) < 0)
        return -1;

    return 0;
}


