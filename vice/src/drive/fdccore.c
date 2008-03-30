
/*
 * fdccore.c - 1001/8x50 FDC emulation
 *
 * Written by
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


static BYTE *buffer;
static log_t fdc_log = LOG_ERR;
static alarm_t fdc_alarm;
static int fdc_state;

void myfdc_reset(int enabled)
{
    log_message(fdc_log, MYFDC_NAME "_reset: enabled=%d\n",enabled);

    if (enabled) {
	fdc_state = FDC_RESET0;
	alarm_set(&fdc_alarm, myclk + 20);
    } else {
	alarm_unset(&fdc_alarm);
	fdc_state = FDC_UNUSED;
    }
}

static int int_fdc(long offset)
{
    CLOCK rclk = myclk - offset;

    if (fdc_state < FDC_RUN) {
	static int old_state = -1;
	if (fdc_state != old_state)
	    log_message(fdc_log, "%d: int_" MYFDC_NAME ": state=%d\n",
							rclk, fdc_state);
	old_state = fdc_state;
    }

    switch(fdc_state) {
    case FDC_RESET0:
	buffer[0] = 2;
	fdc_state++;
	alarm_set(&fdc_alarm, rclk + 2000);
	break;
    case FDC_RESET1:
	if (buffer[0] == 0) {
	    buffer[0] = 1;
	    fdc_state++;
	}
	alarm_set(&fdc_alarm, rclk + 2000);
	break;
    case FDC_RESET2:
	if (buffer[0] == 0) {
	    /* emulate routine written to buffer RAM */
	    buffer[1] = 0x0e;
	    buffer[2] = 0x2d;
	    buffer[0xac] = 2;	/* number of sides on disk drive */
	    buffer[0xea] = 1;	/* 0 = 4040 (2A), 1 = 8x80 (2C) drive type */
	    buffer[0xee] = 5;	/* 3 for 4040, 5 for 8x50 */
	    buffer[0] = 3;	/* 5 for 4040, 3 for 8x50 */
	    fdc_state = FDC_RUN;
	    alarm_set(&fdc_alarm, rclk + 10000);
	} else {
	    alarm_set(&fdc_alarm, rclk + 2000);
	}
	break;
    case FDC_RUN:
	alarm_set(&fdc_alarm, rclk + 20000);
	/* job loop */
	break;
    }

    return 0;
}

static void clk_overflow_callback(CLOCK sub, void *data)
{
}

void myfdc_init(BYTE *buffermem)
{
    buffer = buffermem;

    if (fdc_log == LOG_ERR)
        fdc_log = log_open(MYFDC_NAME);

    alarm_init(&fdc_alarm, &mycpu_alarm_context,
               MYFDC_NAME, int_fdc);

    clk_guard_add_callback(&mycpu_clk_guard, clk_overflow_callback, NULL);
}


