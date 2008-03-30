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
static BYTE riot_last_read = 0;  /* the byte read the last time (for RMW) */

/*
 * code
 */

void myriot_signal(int sig, int type)
{
    printf(MYRIOT_NAME ": signal type=%d\n",type);
}

static void clk_overflow_callback(CLOCK sub, void *data)
{
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
	switch (addr & 3) {
	case 0: 	/* ORA */
            riotio[0] = byte;
        case 1: 	/* DDRA */
            riotio[1] = byte;
            byte = riotio[0] | ~riotio[1];
            store_pra(byte);
            oldpa = byte;
            break;
	case 2: 	/* ORB */
            riotio[2] = byte;
        case 3: 	/* DDRB */
            riotio[3] = byte;
            byte = riotio[2] | ~riotio[3];
            store_prb(byte);
            oldpa = byte;
            break;
	}
    } else
    if ((addr & 0x14) == 0x14) {	/* set timer */
	log_warning(riot_log, "write timer %02x@%d not yet implemented\n",
		byte, addr);
    } else 
    if ((addr & 0x14) == 0x04) {	/* set edge detect control */
	log_warning(riot_log, "edge control %02x@%d not yet implemented\n",
		byte, addr);
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
    BYTE byte = 0xff;
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
	log_warning(riot_log, "read timer @%d not yet implemented\n",
		addr);
    } else 
    if ((addr & 0x05) == 0x05) {	/* read irq flag */
	log_warning(riot_log, "read irq flag @%d not yet implemented\n",
		addr);
    }
}

static int int_riot(long offset)
{
    CLOCK rclk = myclk - offset;
}

/*-------------------------------------------------------------------*/

int myriot_write_snapshot_module(snapshot_t * p)
{
}

int myriot_read_snapshot_module(snapshot_t * p)
{
}


