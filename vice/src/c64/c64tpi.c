
/*
 * ../../../src/c64/c64tpi.c
 * This file is generated from ../../../src/tpi-tmpl.c and ../../../src/c64/c64tpi.def,
 * Do not edit!
 */
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

#include <stdio.h>

#include "vice.h"
#include "types.h"
#include "tpi.h"
#include "interrupt.h"


#include "parallel.h"
#include "tpi.h"

#define	mycpu_set_int(a,b)

#define	TPI_SET_CA(a)
#define	TPI_SET_CB(a)

int tpi_debug = 0;

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

/*------------------------------------------------------------------------*/
/* Handle irq stack etc */

static void set_latch_bit(int bit) {
    if(tpi_debug && !(bit & irq_latches)) {
	printf("tpi: set_latch_bit(%02x, mask=%02x)\n",bit, irq_mask);
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
    if(tpi_debug) {
        printf("tpi: pop_irq_state(latches=%02x, stack=%02x, active=%02x)\n",
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

    if(tpi_debug) {
        printf("tpi: push_irq_state(latches=%02x, act=%02x, stack=%02x, "
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

void reset_tpi ( void ) {
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


	/* assuming input after reset */
	par_set_atn(0);
	par_set_ndac(0);
	par_set_nrfd(0);
	par_set_dav(0);
	par_set_eoi(0);
	par_set_bus(0);
}

void store_tpi ( ADDRESS addr, BYTE byte ) {

	addr &= 0x07;


	switch ( addr ) {
	case TPI_PA:
	case TPI_DDPA:
	    tpi[addr] = byte;
	    byte = tpi[TPI_PA] | ~tpi[TPI_DDPA];

	{ BYTE tmp = ~byte;
	    par_set_atn( tmp & 0x08 );
	    par_set_dav( tmp & 0x10 );
	    par_set_eoi( tmp & 0x20 );
	    par_set_ndac( tmp & 0x40 );
	    par_set_nrfd( tmp & 0x80 );
	}
	    oldpa = byte;
	    return;
	case TPI_PB:
	case TPI_DDPB:
	    tpi[addr] = byte;
	    byte = tpi[TPI_PB] | ~tpi[TPI_DDPB];

	par_set_bus( ~byte );
	    oldpb = byte;
	    if(IS_CB_MODE()) {
		TPI_SET_CB(0);
		if(IS_CB_PULSE_MODE()) {
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
	        byte = tpi[TPI_PC] | tpi[TPI_DDPC];
	        
		oldpc = byte;
	    }
	    return;
	case TPI_CREG:
	    tpi[addr] = byte;
	    if(tpi_debug) {
		printf("write %02x to CREG\n",byte);
	    }
	    if(tpi[TPI_CREG] & 0x20) {
		TPI_SET_CA( (tpi[TPI_CREG] & 0x10) );
	    } else
	    if(tpi[TPI_CREG] & 0x10) {
		TPI_SET_CA( 1 );
	    }
	    if(tpi[TPI_CREG] & 0x80) {
		TPI_SET_CB( (tpi[TPI_CREG] & 0x40) );
	    } else
	    if(tpi[TPI_CREG] & 0x40) {
		TPI_SET_CB( 1 );
	    }
	    return;
	case TPI_AIR:
	    pop_irq_state();
	    return;
	}
	tpi[addr] = byte;
}

BYTE read_tpi ( ADDRESS addr ) {
	BYTE byte = 0xff;
    	switch ( addr ) {
	case TPI_PA:

	byte = par_atn ? 0 : 8;
	byte += par_dav ? 0 : 16;
	byte += par_eoi ? 0 : 32;
	byte += par_ndac ? 0 : 64;
	byte += par_nrfd ? 0 : 128;
	    if(IS_CA_MODE()) {
		TPI_SET_CA(0);
		if(IS_CA_PULSE_MODE()) {
		    TPI_SET_CA(1);
		}
	    }
	    return ((byte & (tpi[TPI_DDPA]^255))
				     | (tpi[TPI_PA] & tpi[TPI_DDPA]));
	case TPI_PB:

	byte = par_bus;
	    return ((byte & (tpi[TPI_DDPB]^255))
				     | (tpi[TPI_PB] & tpi[TPI_DDPB]));
	case TPI_PC:
	    if(irq_mode) {
		return (irq_latches & 0x1f) | (irq_active ? 0x20 : 0) | 0xc0;
	    } else {
	        
	        return  ((byte & (tpi[TPI_DDPC]^255))
				     | (tpi[TPI_PC] & tpi[TPI_DDPC]));
	    }
	case TPI_AIR:
	    return push_irq_state();
	default:
	    return tpi[addr];
	}
}

BYTE peek_tpi (ADDRESS addr) {
	BYTE b = read_tpi(addr);
	return b;
}


/* Port C can be setup as interrupt input - this collects connected IRQ states
 * and sets IRQ if necessary */
void tpi_set_int(int bit, int state)
{
    if(bit>=5) return;

    bit = pow2[bit];

    /* check low-high transition */
    if(state && !(irq_previous & bit)) {
	if((bit & 0x18) && ((bit>>1) & tpi[TPI_CREG])) {
	    set_latch_bit(bit);
	    if((bit & 0x08) && IS_CA_TOGGLE_MODE()) {
		TPI_SET_CA(1);
	    }
	    if((bit & 0x10) && IS_CB_TOGGLE_MODE()) {
		TPI_SET_CB(1);
	    }
	}
	irq_previous |= bit;
    } else
    /* check high-low transition */
    if((!state) && (irq_previous & bit)) {
	if((bit & 0x18) && !((bit>>1) & tpi[TPI_CREG])) {
	    set_latch_bit(bit);
	    if((bit & 0x08) && IS_CA_TOGGLE_MODE()) {
		TPI_SET_CA(1);
	    }
	    if((bit & 0x10) && IS_CB_TOGGLE_MODE()) {
		TPI_SET_CB(1);
	    }
	}
	if(bit & 0x07) {
	    set_latch_bit(bit);
	}
	irq_previous &= ~bit;
    }
}

