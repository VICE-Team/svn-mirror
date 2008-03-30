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
static tap_t *current_image = NULL;

static int datasette_motor = 0;

static alarm_t datasette_alarm;

int datasette_read_bit(long offset)
{
    alarm_unset(&datasette_alarm);
    alarm_context_update_next_pending(datasette_alarm.context);

    if (current_image == NULL)
        return 0;
    /*printf("seek: %i\n",current_image->current_file_seek_position);*/
    switch (current_image->mode) {
      case DATASETTE_CONTROL_START:
        if (datasette_motor)
        {
            BYTE comp_gap;
            CLOCK gap;

            datasette_trigger_flux_change();

            if (fread(&comp_gap, 1, 1, current_image->fd) < 1) {
                current_image->mode = DATASETTE_CONTROL_STOP;
                return 0;
            }

            current_image->current_file_seek_position++;
  
            if (current_image->version >= 1 && !comp_gap) {
                BYTE long_gap[3];
                int i;
                for (i = 0; i < 3; i++) {
                    if (fread(&long_gap[i], 1, 1, current_image->fd) < 1) {
                        current_image->mode = DATASETTE_CONTROL_STOP;
                        return 0;
                    }
                }
                gap = long_gap[0] + (long_gap[1] << 8) + (long_gap[2] << 16);
            } else {
                gap = (comp_gap ? (CLOCK)comp_gap : (CLOCK)512) * 8 - offset;
            }

            if (gap > 0) {
                alarm_set(&datasette_alarm, clk + gap);
                return 0;
            }
        }
        break;
      case DATASETTE_CONTROL_FORWARD:
        if (current_image->current_file_seek_position
            >= (current_image->size - 20)) {
            current_image->current_file_seek_position = current_image->size;
            current_image->mode = DATASETTE_CONTROL_STOP;
        } else {
            current_image->current_file_seek_position += 20;
            alarm_set(&datasette_alarm, clk + 10000);
        }
        break;
      case DATASETTE_CONTROL_REWIND:
        if (current_image->current_file_seek_position <= 20) {
            current_image->current_file_seek_position = 0;
            current_image->mode = DATASETTE_CONTROL_STOP;
        } else {
            current_image->current_file_seek_position -= 20;
            alarm_set(&datasette_alarm, clk + 10000);
        }
        break;
    }
    return 0;
}

void datasette_init(void)
{
    alarm_init(&datasette_alarm, &maincpu_alarm_context,
               "Datasette", datasette_read_bit);
}

void datasette_set_tape_image(tap_t *image)
{
    current_image = image;
}

void datasette_forward(void)
{
    if (current_image->mode == DATASETTE_CONTROL_START
       || current_image->mode == DATASETTE_CONTROL_REWIND)
    {
        alarm_unset(&datasette_alarm);
        alarm_context_update_next_pending(datasette_alarm.context);
    }
    alarm_set(&datasette_alarm, clk + 1000);
}

void datasette_rewind(void)
{
    if (current_image->mode == DATASETTE_CONTROL_START 
       || current_image->mode == DATASETTE_CONTROL_FORWARD)
    {
        alarm_unset(&datasette_alarm);
        alarm_context_update_next_pending(datasette_alarm.context);
    }
    alarm_set(&datasette_alarm, clk + 1000);
}

void datasette_control(int command)
{
    if (current_image != NULL) {
        switch(command) {
          case DATASETTE_CONTROL_STOP:
            current_image->mode = DATASETTE_CONTROL_STOP;
            mem_set_tape_sense(0);
            break;
          case DATASETTE_CONTROL_START:
            current_image->mode = DATASETTE_CONTROL_START;
            mem_set_tape_sense(1);
            break;
          case DATASETTE_CONTROL_FORWARD:
            current_image->mode = DATASETTE_CONTROL_FORWARD;
            datasette_forward();
            mem_set_tape_sense(0);
            break;
          case DATASETTE_CONTROL_REWIND:
            current_image->mode = DATASETTE_CONTROL_REWIND;
            datasette_rewind();
            mem_set_tape_sense(0);
            break;
          case DATASETTE_CONTROL_RECORD:
            break;
          case DATASETTE_CONTROL_RESET:
            break;
        }
    }
}

void datasette_set_motor(int flag)
{
    if (current_image != NULL
        && current_image->mode != DATASETTE_CONTROL_REWIND
        && current_image->mode != DATASETTE_CONTROL_FORWARD) {
        if (flag && !datasette_motor)
        {
            fseek(current_image->fd, current_image->current_file_seek_position,
                  SEEK_SET);
            alarm_set(&datasette_alarm, clk + 1000);
        }
        if (!flag && datasette_motor)
        {
            alarm_unset(&datasette_alarm);
            alarm_context_update_next_pending(datasette_alarm.context);
        }
    }
    datasette_motor = flag;
}

