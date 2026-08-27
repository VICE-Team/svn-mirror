/*
 * petspeed-dongle.c - tape port dongle that helps PetSpeed work.
 *
 * Written by
 *  Olaf 'Rhialto' Seibert <rhialto@falu.nl>
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
#include <stdlib.h>
#include <string.h>

#include "cmdline.h"
#include "log.h"
#include "maincpu.h"
#include "machine.h"
#include "mem.h"
#include "resources.h"
#include "tapeport.h"

#include "petspeed-dongle.h"

/*
 * A PetSpeed dongle.
 * It uses CA1 (cassette #1 read line) as input for bits to a shifter (MSB
 * first).
 * Cassette sense #1 is set to output to supply a clock.
 * At the start, cassette write #1 is toggled 1 -> 0 -> 1.
 *
 * Usage notes for PetSpeed:
 * - Place source program in drive 1:
 * - Does not like DOS 2.7 with its additional ",1" in "00, ok,00,00,1"
 *   so either use DOS 2.5 (901482-03.bin + 901482-04.bin)
 *   or use a 4040 disk drive.
 */
static int petspeed_dongle_enabled[TAPEPORT_MAX_PORTS] = { 0 };

static log_t wclog = LOG_DEFAULT;

/* ------------------------------------------------------------------------- */

/* Some prototypes are needed */
static void petspeed_dongle_powerup(int port);
static int petspeed_dongle_enable(int port, int val);
static void petspeed_dongle_set_motor_line(int port, int value);
static void petspeed_dongle_toggle_write_line(int port, int value);
static void petspeed_dongle_set_sense_line(int port, int value);
static void petspeed_dongle_set_read_line(int port, int value);

static tapeport_device_t petspeed_dongle_device = {
    "PetSpeed dongle",                  /* device name */
    TAPEPORT_DEVICE_TYPE_DONGLE,        /* device is a 'dongle' type device */
    VICE_MACHINE_PET,                   /* device works on PETs only */
    TAPEPORT_PORT_1_MASK,               /* device works on port 1 only */
    petspeed_dongle_enable,             /* device enable function */
    petspeed_dongle_powerup,            /* device specific hard reset function */
    NULL,                               /* NO device shutdown function */
    petspeed_dongle_set_motor_line,     /* set motor line function */
    petspeed_dongle_toggle_write_line,  /* set write line function */
    petspeed_dongle_set_sense_line,     /* set sense line function */
    petspeed_dongle_set_read_line,      /* set read line function */
    NULL,                               /* NO device snapshot write function */
    NULL                                /* NO device snapshot read function */
};

/* ------------------------------------------------------------------------- */

static
struct dongle_data {
    bool write_line;
    bool clock_in_line;
    bool clock_enable_line;
    int shift_count;
    unsigned int dongle_value;
} dongle_data;

static unsigned int reverse16(unsigned int bits)
{
    unsigned int v = bits;

    /* swap odd and even bits */
    v = ((v >> 1) & 0x5555) | ((v & 0x5555) << 1);
    /* swap consecutive pairs */
    v = ((v >> 2) & 0x3333) | ((v & 0x3333) << 2);
    /* swap nibbles ... */
    v = ((v >> 4) & 0x0F0F) | ((v & 0x0F0F) << 4);
    /* swap bytes */
    v = ((v >> 8) & 0x00FF) | ((v & 0x00FF) << 8);

    return v;
}

static int petspeed_dongle_enable(int port, int value)
{
    int val = value ? 1 : 0;

    if (petspeed_dongle_enabled[port] == val) {
        return 0;
    }

    petspeed_dongle_enabled[port] = val;

    dongle_data.dongle_value = reverse16(0xAA00);

    return 0;
}

int petspeed_dongle_resources_init(int amount)
{
    wclog = log_open("PetSpeed");

    return tapeport_device_register(TAPEPORT_DEVICE_PETSPEED_DONGLE,
                                    &petspeed_dongle_device);
}

/* ---------------------------------------------------------------------*/

static void petspeed_dongle_powerup(int port)
{
    dongle_data.shift_count = 0;
    /* Code at $147C tests the cassette sense line */
    tapeport_set_tape_sense(0, port);
}

static void petspeed_dongle_set_motor_line(int port, int value)
{
    if (value != dongle_data.clock_enable_line) {
        log_verbose(wclog, "petspeed_dongle_set_motor_line: %d %d", port, value);
    }
    dongle_data.clock_enable_line = value;        /* but apparently not used */
}

/*
 * The tape input on the PET is strictly edge-triggered. To compensate for that,
 * after every "active transition", the dongle checking code toggles the active
 * direction. This way it can detect both 1 and 0 inputs.
 * Because of this, we do not need to worry if we send duplicate "transitions"
 * in the same direction into the PIA code. Those are implemented inaccurately.
 */
static void petspeed_dongle_set_sense_line(int port, int value)
{
    log_debug(wclog, "petspeed_dongle_set_sense_line: %d %d", port, value);

    if (!dongle_data.clock_in_line && value) {
        /*
         * Shift a bit on the RISING edge of the clock. The dongle checking
         * code does both transitions right after each other, before reading
         * input.  So it doesn't really matter which edge is used.  LSB first.
         */
        bool bit = (dongle_data.dongle_value >> dongle_data.shift_count) & 1;
        log_verbose(wclog, "petspeed_set_sense_line: send bit #%d %d",
                dongle_data.shift_count, bit);
        dongle_data.shift_count++;

        tapeport_set_read_in(!bit, port);

	uint8_t *mem = mem_ram;
	log_debug(wclog, "petspeed_dongle_set_sense_line: collected $%02x $%02x\n", mem[0x87D0], mem[0x87D1]);
    }
    dongle_data.clock_in_line = value;
}

static void petspeed_dongle_toggle_write_line(int port, int value)
{
    if (dongle_data.write_line && !value) {
        log_verbose(wclog, "petspeed_dongle_write_line: %d %d rise: restart shift sequence", port, value);
        /* Restart the shifting sequence */
	dongle_data.shift_count = 0;
    }
    dongle_data.write_line = value;
}

static void petspeed_dongle_set_read_line(int port, int value)
{
    log_debug(wclog, "petspeed_dongle_set_read_line: %d %d", port, value);
}
