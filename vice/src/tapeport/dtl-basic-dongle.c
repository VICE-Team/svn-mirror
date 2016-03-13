/*
 * dtlbasic-dongle.c - dtl basic tape port dongle emulation.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include "resources.h"
#include "tapeport.h"
#include "translate.h"

/* DTL Basic Dongle description:

   This emulation currently does not work for the software using it,
   help/more information is needed to fix this.

   Documentation/information used for making the emulation:

   http://sid.fi/~tnt/dtl/

   The current emulation does the following:

   1- wait for the sense line to go high.
   2- wait for the sense line to go low.
   3- wait for the write line to go high.
   4- wait for the write line to go low.
   5- set the first bit of the bit sequence 0010 0100 0000 0010 (0x2402) on the read line.

   After setting the first bit of the sequence the following needs to happen before setting the next bit:

   1- wait for the write line to go high.
   2- wait for the write line to go low.

   Be aware that the current emulation keeps the 'old' bit set between steps 1 and 2.

   The emulation 'shows' the correct bit pattern when using the 'dongle2.prg' file, but does not work (good enough) for the actual software.
*/

#define DTLBASIC_DONGLE_IDLE       0
#define DTLBASIC_DONGLE_SENSE_HIGH 1
#define DTLBASIC_DONGLE_SENSE_LOW  2
#define DTLBASIC_DONGLE_WRITE_HIGH 3
#define DTLBASIC_DONGLE_ACTIVE     4

static int dtlbasic_dongle_enabled = 0;

static int dtlbasic_counter = -1;

static BYTE dtlbasic_key[20] = { 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 };

static int write_status = -1;
static int sense_status = -1;

static int dtlbasic_state = DTLBASIC_DONGLE_IDLE;

/* ------------------------------------------------------------------------- */

/* Some prototypes are needed */
static void dtlbasic_dongle_reset(void);
static void dtlbasic_write(int write_bit);
static void dtlbasic_sense_out(int sense);

static tapeport_device_t dtlbasic_dongle_device = {
    "Sense dongle",
    IDGS_SENSE_DONGLE,
    0,
    dtlbasic_dongle_reset,
    NULL, /* no set motor */
    dtlbasic_write,
    dtlbasic_sense_out,
    NULL, /* no passthrough */
    NULL  /* no passthrough */
};

static tapeport_device_list_t *dtlbasic_dongle_list_item = NULL;

/* ------------------------------------------------------------------------- */

static int set_dtlbasic_dongle_enabled(int value, void *param)
{
    int val = value ? 1 : 0;

    if (dtlbasic_dongle_enabled == val) {
        return 0;
    }

    if (val) {
        dtlbasic_dongle_list_item = tapeport_device_register(&dtlbasic_dongle_device);
        if (dtlbasic_dongle_list_item == NULL) {
            return -1;
        }
        dtlbasic_counter = -1;
        dtlbasic_state = DTLBASIC_DONGLE_IDLE;
    } else {
        tapeport_device_unregister(dtlbasic_dongle_list_item);
        dtlbasic_dongle_list_item = NULL;
    }

    dtlbasic_dongle_enabled = val;
    return 0;
}

static const resource_int_t resources_int[] = {
    { "DTLBasicDongle", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &dtlbasic_dongle_enabled, set_dtlbasic_dongle_enabled, NULL },
    { NULL }
};

int dtlbasic_dongle_resources_init(void)
{
    return resources_register_int(resources_int);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-dtlbasicdongle", SET_RESOURCE, 0,
      NULL, NULL, "DTLBasicDongle", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_DTL_BASIC_DONGLE,
      NULL, NULL },
    { "+dtlbasicdongle", SET_RESOURCE, 0,
      NULL, NULL, "DTLBasicDongle", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_DTL_BASIC_DONGLE,
      NULL, NULL },
    { NULL }
};

int dtlbasic_dongle_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ---------------------------------------------------------------------*/

static void dtlbasic_dongle_reset(void)
{
    dtlbasic_state = DTLBASIC_DONGLE_IDLE;
    dtlbasic_counter = -1;
    write_status = -1;
    sense_status = -1;
}

static void dtlbasic_write(int write_bit)
{
    if (write_bit == write_status) {
        return;
    }

    write_status = write_bit;

    if (dtlbasic_state == DTLBASIC_DONGLE_SENSE_LOW && write_bit) {
        dtlbasic_state = DTLBASIC_DONGLE_WRITE_HIGH;
        return;
    }

    if (dtlbasic_state == DTLBASIC_DONGLE_WRITE_HIGH && !write_bit) {
        dtlbasic_state = DTLBASIC_DONGLE_ACTIVE;
        dtlbasic_counter = 1;
        return;
    }

    if (!write_bit) {
        if (dtlbasic_counter != -1) {
            if (dtlbasic_key[dtlbasic_counter]) {
                tapeport_trigger_flux_change(1, dtlbasic_dongle_device.id);
            }
            ++dtlbasic_counter;
            if (dtlbasic_counter == 20) {
                dtlbasic_counter = -1;
            }
        }
    }
}

static void dtlbasic_sense_out(int sense)
{
    if (sense == sense_status) {
        return;
    }

    sense_status = sense;

    if (dtlbasic_state == DTLBASIC_DONGLE_IDLE && sense) {
        dtlbasic_state = DTLBASIC_DONGLE_SENSE_HIGH;
        return;
    }

    if (dtlbasic_state == DTLBASIC_DONGLE_SENSE_HIGH && !sense) {
        dtlbasic_state = DTLBASIC_DONGLE_SENSE_LOW;
        return;
    }
}
