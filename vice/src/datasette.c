/*
 * datasette.c - CBM cassette implementation.
 *
 * Written by
 *  Andreas Boose (boose@linux.rz.fh-hannover.de)
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

#include "alarm.h"
#include "c64/c64cia.h"
#include "datasette.h"
#include "maincpu.h"
#include "mem.h"

/* Attached TAP tape image.  */
tap_t *current_image = NULL;

int datasette_play = 0;

int datasette_motor = 0;

static alarm_t datasette_alarm;

int datasette_read_bit(long offset)
{
    if (current_image != NULL && datasette_motor)
    {
        BYTE comp_gap;
        CLOCK gap;

        datasette_trigger_flux_change();

        if (fread(&comp_gap, 1, 1, current_image->fd) < 1)
            return 0;

        gap = (CLOCK)comp_gap * 8 - offset;

        if (gap > 0) {
            alarm_set(&datasette_alarm, clk + gap);
            return 0;
        }
    }
    alarm_unset(&datasette_alarm);
    return 0;
}

void datasette_init(void)
{
    alarm_init(&datasette_alarm, &maincpu_alarm_context,
               "Datasette", datasette_read_bit);
}

static void datasette_wait_read(void)
{
    if (current_image != NULL)
    {
        alarm_set(&datasette_alarm, clk + 1000);
    }
}

void datasette_set_tape_image(tap_t *image)
{
    current_image = image;
}

void datasette_control(int command)
{
    switch(command) {
      case DATASETTE_CONTROL_STOP:
        datasette_play = 0;
        mem_set_tape_sense(0);
        break;
      case DATASETTE_CONTROL_START:
        datasette_play = 1;
        mem_set_tape_sense(1);
        datasette_wait_read();
        break;
      case DATASETTE_CONTROL_FORWARD:
        datasette_play = 0;
        mem_set_tape_sense(0);
        break;
      case DATASETTE_CONTROL_REWIND:
        datasette_play = 0;
        mem_set_tape_sense(0);
        break;
      case DATASETTE_CONTROL_RECORD:
        break;
      case DATASETTE_CONTROL_RESET:
        break;
    }
}

void datasette_set_motor(int flag)
{
    /*printf("Motor %x\n", flag); */
    datasette_motor = flag;
    if (current_image != NULL)
    {
        alarm_set(&datasette_alarm, clk + 1000);
    }
}

