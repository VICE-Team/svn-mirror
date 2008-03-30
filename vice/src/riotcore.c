/*
 * riotcore.c - Core functions for RIOT emulation.
 *
 * Written by
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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
 * The RIOT is a chip with 128 byte RAM, two 8 bit I/O ports, where
 * PA7 has an interrupt input capability. Also it has a timer
 * that can count 255 periods of 1, 8, 64 or 1024 cycles.
 * The addressing is pretty, though.
 *
 * In our emulation the RAM is emulated in the memory part, not
 * in this file.
 *
 * The timer is not yet emulated... :-(
 */

/*
 * local prototypes
 */

static int int_riot(long offset);

/*
 * local variables
 */

static CLOCK riot_read_clk = 0;
static int riot_read_offset = 0;
static BYTE riot_last_read = 0;	/* the byte read the last time (for RMW) */

static BYTE edgectrl = 0;	/* bit 0: 0=neg., 1=pos. edge, bit 1: en IRQ */
static BYTE irqfl = 0;
static BYTE irqline = 0;

/* timer related */
static CLOCK ti_write_clk;	/* when has timer been written */
static int ti_N;		/* value written - 1 */
static int ti_divider;		/* pre divider */
static int ti_irqen;		/* set if timer IRQ enabled */

/*
 * code
 */

static int divider[] = {
	1, 8, 64, 1024
};

static void update_irq(BYTE new_irqfl)
{
    int new_irqline;

    new_irqline = (new_irqfl & 0x80)
		|| ((new_irqfl & 0x40) && (edgectrl & 2));

    if (new_irqline && !irqline) {
	my_set_irq(1, myclk);
    }
    if (irqline && !new_irqline) {
	my_set_irq(0, myclk);
    }

    irqline = new_irqline;
    irqfl = new_irqfl;
}

void myriot_signal(int sig, int type)
{
    BYTE newirq = irqfl & 0xbf;

    /* You better not call that twice with the same flag - the IRQ
     * will be set twice... */
/*
    printf(MYRIOT_NAME ": signal type=%d\n",type);
*/
    if ((type == RIOT_SIG_FALL) && !(edgectrl & 1)) {
	newirq |= 0x40;
    } else
    if ((type == RIOT_SIG_RISE) && (edgectrl & 1)) {
	newirq |= 0x40;
    }

    update_irq(newirq);
}

static void update_timer(void)
{
    int underfl = (myclk - ti_write_clk) / ti_divider;

    if (underfl > ti_N) {
	ti_write_clk += ti_N * ti_divider;
	ti_N = 255;
	ti_divider = 1;
    }
    ti_write_clk += (myclk - ti_write_clk) & 0xff00;
}

static void clk_overflow_callback(CLOCK sub, void *data)
{
    update_timer();

    ti_write_clk -= sub;

    if (riot_read_clk > sub)
        riot_read_clk -= sub;
    else
        riot_read_clk = 0;
}

void myriot_init(void)
{
    if (riot_log == LOG_ERR)
        riot_log = log_open(MYRIOT_NAME);

    alarm_init(&riot_alarm, &mycpu_alarm_context,
               MYRIOT_NAME "T1", int_riot);
    clk_guard_add_callback(&mycpu_clk_guard, clk_overflow_callback, NULL);
}

void myriot_reset(void)
{
    riot_read_clk = 0;

    alarm_unset(&riot_alarm);

    oldpa = 0xff;
    oldpb = 0xff;

    edgectrl = 0;
    irqfl = 0;
    irqline = 0;
    my_set_irq(0, myclk);

    ti_write_clk = myclk;
    ti_N = 255;
    ti_divider = 1;

    riot_reset();
}

void REGPARM2 store_myriot(ADDRESS addr, BYTE byte)
{
    CLOCK rclk;

    if (mycpu_rmw_flag) {
        myclk --;
        mycpu_rmw_flag = 0;
        store_myriot(addr, riot_last_read);
        myclk ++;
    }

    rclk = myclk - 1;   /* stores have a one-cylce offset */

    addr &= 0x1f;

    /* manage the weird addressing schemes */

    if ((addr & 0x04) == 0) {		/* I/O */
	addr &= 3;
	switch (addr) {
	case 0: 	/* ORA */
        case 1: 	/* DDRA */
            riotio[addr] = byte;
            byte = riotio[0] | ~riotio[1];
            store_pra(byte);
            oldpa = byte;
            break;
	case 2: 	/* ORB */
        case 3: 	/* DDRB */
            riotio[addr] = byte;
            byte = riotio[2] | ~riotio[3];
            store_prb(byte);
            oldpb = byte;
            break;
	}
    } else
    if ((addr & 0x14) == 0x14) {	/* set timer */
	int newirq = irqfl & 0x7f;
/*
	log_warning(riot_log, "write timer %02x@%d not yet implemented\n",
		byte, addr);
*/
	ti_divider = divider[addr & 3];
	ti_write_clk = rclk + 1;
	ti_N = byte;
	ti_irqen = (addr & 8); 

	if (byte) {
	    ti_N --;
	    if (ti_irqen) {
		alarm_set(&riot_alarm, ti_write_clk + ti_N * ti_divider);
	    }
	} else {
	    /* setup IRQ? */
	    ti_N = 255;
	    ti_divider = 1;
	    if (ti_irqen) {
		newirq |= 0x80;
	    }
	}

	update_irq(newirq);

	if (!ti_irqen) {
	    alarm_unset(&riot_alarm);
	}
    } else 
    if ((addr & 0x14) == 0x04) {	/* set edge detect control */
/*
	log_message(riot_log, "edge control %02x@%d\n", byte, addr);
*/
	edgectrl = addr & 3;

	update_irq(irqfl);
    }
}

BYTE REGPARM1 read_myriot(ADDRESS addr)
{
#ifdef MYRIOT_TIMER_DEBUG
    BYTE REGPARM1 read_myriot_(ADDRESS);
    BYTE retv = read_myriot_(addr);
    addr &= 0x1f;
    if ((addr > 3 && addr < 10) || app_resources.debugFlag)
        log_message(riot_log,
                    MYRIOT_NAME "(%x) -> %02x, clk=%d", addr, retv, myclk);
    return retv;
}
BYTE REGPARM1 read_myriot_(ADDRESS addr)
{
#endif
    CLOCK rclk;

    addr &= 0x1f;

    /* Hack for opcode fetch, where the clock does not change */
    if (myclk <= riot_read_clk) {
        rclk = riot_read_clk + (++riot_read_offset);
    } else {
        riot_read_clk = myclk;
        riot_read_offset = 0;
        rclk = myclk;
    }

    /* manage the weird addressing schemes */

    if ((addr & 0x04) == 0) {		/* I/O */
	switch (addr & 3) {
	case 0: 	/* ORA */
	    riot_last_read = read_pra();
            return riot_last_read;
            break;
        case 1: 	/* DDRA */
            riot_last_read = riotio[1];
            return riot_last_read;
            break;
	case 2: 	/* ORB */
	    riot_last_read = read_prb();
            return riot_last_read;
            break;
        case 3: 	/* DDRB */
            riot_last_read = riotio[3];
            return riot_last_read;
            break;
	}
    } else
    if ((addr & 0x05) == 0x04) {	/* read timer */
/*
	log_warning(riot_log, "read timer @%d not yet implemented\n",
		addr);
*/
	update_irq(irqfl & 0x7f);

	update_timer();

	ti_irqen = addr & 8;

	if (ti_irqen) {
	     alarm_set(&riot_alarm, ti_write_clk + ti_N * ti_divider);
	} else {
	     alarm_unset(&riot_alarm);
	}

	riot_last_read = ti_N - (rclk - ti_write_clk) / ti_divider;
	return riot_last_read;
    } else 
    if ((addr & 0x05) == 0x05) {	/* read irq flag */
/*
	log_message(riot_log, "read irq flag @%d\n", addr);
*/
	riot_last_read = irqfl;

        if (ti_irqen) {
	    update_timer();
	    alarm_set(&riot_alarm, ti_write_clk + ti_N * ti_divider);
        }

	update_irq(irqfl & 0xbf);
    }
    return 0xff;
}

static int int_riot(long offset)
{
/*    CLOCK rclk = myclk - offset; */

    alarm_unset(&riot_alarm);

    update_irq(irqfl | 0x80);

    return 0;
}

/*-------------------------------------------------------------------*/

/* The dump format has a module header and the data generated by the
 * chip...
 *
 * The version of this dump description is 0.0
 *
 */

#define RIOT_DUMP_VER_MAJOR      0
#define RIOT_DUMP_VER_MINOR      0

/*
 * The dump data:
 *
 * UBYTE        ORA
 * UBYTE 	DDRA
 * UBYTE 	ORB
 * UBYTE 	DDRB
 * UBYTE	EDGECTRL	edge control value 
 * UBYTE 	IRQFL		IRQ fl:
 *				Bit 6/7: see RIOT IRQ flag
 *				Bit 0: state of IRQ line (1=active)
 * UBYTE	N		current timer value
 * UWORD	divider		1, 8, 64, 104
 * UWORD	rest		cycles that the divider has done since last 
 *				counter tick
 * UBYTE	irqen		0= timer IRQ disabled, 1= enabled
 *
 */
                                                
int myriot_write_snapshot_module(snapshot_t * p)
{
    snapshot_module_t *m;

    m = snapshot_module_create(p, MYRIOT_NAME,
                               RIOT_DUMP_VER_MAJOR, RIOT_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    update_timer();

    snapshot_module_write_byte(m, riotio[0]);
    snapshot_module_write_byte(m, riotio[1]);
    snapshot_module_write_byte(m, riotio[2]);
    snapshot_module_write_byte(m, riotio[3]);

    snapshot_module_write_byte(m, edgectrl);
    snapshot_module_write_byte(m, irqfl | (irqline ? 1 : 0));

    snapshot_module_write_byte(m, ti_N - (myclk - ti_write_clk) / ti_divider);
    snapshot_module_write_word(m, ti_divider);
    snapshot_module_write_word(m, (myclk - ti_write_clk) % ti_divider);
    snapshot_module_write_byte(m, ti_irqen ? 1 : 0);

    snapshot_module_close(m);

    return 0;
}

int myriot_read_snapshot_module(snapshot_t * p)
{
    BYTE vmajor, vminor;
    BYTE byte;
    WORD word;
    snapshot_module_t *m;

    m = snapshot_module_open(p, MYRIOT_NAME, &vmajor, &vminor);
    if (m == NULL) {
	log_message(riot_log, "Could not find snapshot module " MYRIOT_NAME );
        return -1;
    }

    if (vmajor != RIOT_DUMP_VER_MAJOR) {
        log_error(riot_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  vmajor, vminor, RIOT_DUMP_VER_MAJOR, RIOT_DUMP_VER_MINOR);
        snapshot_module_close(m);
        return -1;
    }

    /* just to be safe */
    alarm_unset(&riot_alarm);

    snapshot_module_read_byte(m, &riotio[0]);
    snapshot_module_read_byte(m, &riotio[1]);
    oldpa = riotio[0] | ~riotio[1];
    undump_pra(oldpa);

    snapshot_module_read_byte(m, &riotio[2]);
    snapshot_module_read_byte(m, &riotio[3]);
    oldpb = riotio[2] | ~riotio[3];
    undump_prb(oldpb);

    snapshot_module_read_byte(m, &edgectrl);
    snapshot_module_read_byte(m, &irqfl);
    if (irqfl & 1) {
	irqline = 1;
	my_restore_irq(1);
    }
    irqfl &= 0xc0;

    snapshot_module_read_byte(m, &byte);
    ti_N = byte;
    snapshot_module_read_word(m, &word);
    ti_divider = word;
    snapshot_module_read_word(m, &word);
    ti_write_clk = myclk - word;
    snapshot_module_read_byte(m, &byte);
    ti_irqen = byte;
    if (ti_irqen) {
	alarm_set(&riot_alarm, ti_write_clk + ti_N * ti_divider);
    } 

    snapshot_module_close(m);

    riot_read_clk = 0;

    return 0;
}


