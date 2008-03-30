/*
 * tpicore.c - TPI 6525 template
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


/*------------------------------------------------------------------------*/

static int mytpi_debug = 0;

static BYTE tpi_last_read = 0;	/* the byte read the last time (for RMW) */

/*------------------------------------------------------------------------*/
/* Handle irq stack etc */

static void set_latch_bit(int bit) {
    
    if(mytpi_debug && !(bit & irq_latches)) {
	log_message(mytpi_log, "set_latch_bit(%02x, mask=%02x)",
                    bit, irq_mask);
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
        log_message(mytpi_log, "pop_irq_state(latches=%02x, stack=%02x, active=%02x)",
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
        log_message(mytpi_log,
                    "push_irq_state(latches=%02x, act=%02x, stack=%02x mask=%02x).",
                    (int)irq_latches, (int)irq_active,
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

void mytpi_init (void) 
{
    if (mytpi_log == LOG_ERR)
        mytpi_log = log_open(MYTPI_NAME);
}

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

	tpi_set_ca(0);
	tpi_set_cb(0);
	ca_state = 0;
	cb_state = 0;

	tpi_reset();
}

void store_mytpi ( ADDRESS addr, BYTE byte ) {

        if (mycpu_rmw_flag) {
            myclk --;
            mycpu_rmw_flag = 0;
            store_mytpi(addr, tpi_last_read);
            myclk ++;
        }

	addr &= 0x07;


	switch ( addr ) {
	case TPI_PA:
	case TPI_DDPA:
	    tpi[addr] = byte;
	    byte = tpi[TPI_PA] | ~tpi[TPI_DDPA];
	    store_pa(byte);
	    oldpa = byte;
	    return;
	case TPI_PB:
	case TPI_DDPB:
	    tpi[addr] = byte;
	    byte = tpi[TPI_PB] | ~tpi[TPI_DDPB];
	    store_pb(byte);
	    oldpb = byte;
	    if(IS_CB_MODE()) {
		cb_state = 0;
		tpi_set_cb(0);
		if(IS_CB_PULSE_MODE()) {
		    cb_state = 1;
		    tpi_set_cb(1);
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
	        store_pc(byte);
		oldpc = byte;
	    }
	    return;
	case TPI_CREG:
	    tpi[addr] = byte;
	    if(mytpi_debug) {
		log_message(mytpi_log, "write %02x to CREG",byte);
	    }
	    if(tpi[TPI_CREG] & 0x20) {
		ca_state = (tpi[TPI_CREG] & 0x10);
		tpi_set_ca( ca_state );
	    } else
	    if(tpi[TPI_CREG] & 0x10) {
		ca_state = 1;
		tpi_set_ca( 1 );
	    }
	    if(tpi[TPI_CREG] & 0x80) {
		cb_state = (tpi[TPI_CREG] & 0x40);
		tpi_set_cb( cb_state );
	    } else
	    if(tpi[TPI_CREG] & 0x40) {
		cb_state = 1;
		tpi_set_cb( 1 );
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
	    byte = read_pa();
	    if(IS_CA_MODE()) {
		ca_state = 0;
		tpi_set_ca(0);
		if(IS_CA_PULSE_MODE()) {
		    ca_state = 1;
		    tpi_set_ca(1);
		}
	    }
	    tpi_last_read = byte;
	    return byte;
	case TPI_PB:
	    byte = read_pb();
	    tpi_last_read = byte;
	    return byte;
	case TPI_PC:
	    if(irq_mode) {
		byte = (irq_latches & 0x1f) | (irq_active ? 0x20 : 0) | 0xc0;
	    } else {
	        byte = read_pc();
	    }
	    tpi_last_read = byte;
	    return byte;
	case TPI_AIR:
	    tpi_last_read = push_irq_state();
	    return tpi_last_read;
	default:
	    tpi_last_read = tpi[addr];
	    return tpi_last_read;
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
		tpi_set_ca(1);
	    }
	    if((bit & 0x10) && IS_CB_TOGGLE_MODE()) {
		cb_state = 1;
		tpi_set_cb(1);
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
		tpi_set_ca(1);
	    }
	    if((bit & 0x10) && IS_CB_TOGGLE_MODE()) {
		cb_state = 1;
		tpi_set_cb(1);
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

static const char module_name[] = MYTPI_NAME;

/* FIXME!!!  Error check.  */
int mytpi_write_snapshot_module(snapshot_t *p)
{
    snapshot_module_t *m;

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
        undump_pa(byte);
        oldpa = byte;

        byte = tpi[TPI_PB] | ~tpi[TPI_DDPB];
        undump_pb(byte);
        oldpb = byte;

	if (!irq_mode) {
            byte = tpi[TPI_PC] | ~tpi[TPI_DDPC];
            undump_pc(byte);
            oldpc = byte;
	}
    }

    tpi_set_ca( ca_state );
    tpi_set_cb( cb_state );

    mycpu_restore_int(I_MYTPI, irq_active ? MYIRQ : 0); 

    if (snapshot_module_close(m) < 0)
        return -1;

    return 0;
}


