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
#include "clkguard.h"
#include "datasette.h"
#include "maincpu.h"

/* Attached TAP tape image.  */
static tap_t *current_image = NULL;

/* State of the datasette motor.  */
static int datasette_motor = 0;

/* Last time we have recorded a flux change.  */
static CLOCK last_write_clk = (CLOCK)0;

static alarm_t datasette_alarm;

static int datasette_alarm_pending = 0;

static int datasette_read_bit(long offset)
{
    alarm_unset(&datasette_alarm);
    datasette_alarm_pending = 0;

    if (current_image == NULL)
        return 0;

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
                    current_image->current_file_seek_position++;
                }
                gap = long_gap[0] + (long_gap[1] << 8) + (long_gap[2] << 16);
            } else {
                gap = (comp_gap ? (CLOCK)comp_gap : (CLOCK)2500) * 8 - offset;
            }

            if (gap > 0) {
                alarm_set(&datasette_alarm, clk + gap);
                datasette_alarm_pending = 1;
            } else {
                /* If the offset is geater than the gap to the next flux
                   change, the change happend during DMA.  Schedule it now.  */
                alarm_set(&datasette_alarm, clk);
                datasette_alarm_pending = 1;
            }
            return 0;
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
            datasette_alarm_pending = 1;
        }
        break;
      case DATASETTE_CONTROL_REWIND:
        if (current_image->current_file_seek_position <= 20) {
            current_image->current_file_seek_position = 0;
            current_image->mode = DATASETTE_CONTROL_STOP;
        } else {
            current_image->current_file_seek_position -= 20;
            alarm_set(&datasette_alarm, clk + 10000);
            datasette_alarm_pending = 1;
        }
        break;
    }
    return 0;
}

static void clk_overflow_callback(CLOCK sub, void *data)
{
    if (last_write_clk > (CLOCK)0)
        last_write_clk -= sub;
}

void datasette_init(void)
{
    alarm_init(&datasette_alarm, &maincpu_alarm_context,
               "Datasette", datasette_read_bit);

    clk_guard_add_callback(&maincpu_clk_guard, clk_overflow_callback, NULL);
}

void datasette_set_tape_image(tap_t *image)
{
    current_image = image;
}

static void datasette_forward(void)
{
    if (current_image->mode == DATASETTE_CONTROL_START
       || current_image->mode == DATASETTE_CONTROL_REWIND)
    {
        alarm_unset(&datasette_alarm);
        datasette_alarm_pending = 0;
    }
    alarm_set(&datasette_alarm, clk + 1000);
    datasette_alarm_pending = 1;
}

static void datasette_rewind(void)
{
    if (current_image->mode == DATASETTE_CONTROL_START 
       || current_image->mode == DATASETTE_CONTROL_FORWARD)
    {
        alarm_unset(&datasette_alarm);
        datasette_alarm_pending = 0;
    }
    alarm_set(&datasette_alarm, clk + 1000);
    datasette_alarm_pending = 1;
}

void datasette_reset(void)
{
    if (current_image != NULL) {
        if (current_image->mode == DATASETTE_CONTROL_START
            || current_image->mode == DATASETTE_CONTROL_FORWARD
            || current_image->mode == DATASETTE_CONTROL_REWIND) {
            alarm_unset(&datasette_alarm);
            datasette_alarm_pending = 0;
        }
        datasette_control(DATASETTE_CONTROL_STOP);
        current_image->current_file_seek_position = 0;
        fseek(current_image->fd, current_image->offset, SEEK_SET);
    }
}

void datasette_control(int command)
{
    if (current_image != NULL) {
        switch(command) {
          case DATASETTE_CONTROL_STOP:
            current_image->mode = DATASETTE_CONTROL_STOP;
            datasette_set_tape_sense(0);
            last_write_clk = (CLOCK)0;
            break;
          case DATASETTE_CONTROL_START:
            current_image->mode = DATASETTE_CONTROL_START;
            datasette_set_tape_sense(1);
            last_write_clk = (CLOCK)0;
            break;
          case DATASETTE_CONTROL_FORWARD:
            current_image->mode = DATASETTE_CONTROL_FORWARD;
            datasette_forward();
            datasette_set_tape_sense(0);
            last_write_clk = (CLOCK)0;
            break;
          case DATASETTE_CONTROL_REWIND:
            current_image->mode = DATASETTE_CONTROL_REWIND;
            datasette_rewind();
            datasette_set_tape_sense(0);
            last_write_clk = (CLOCK)0;
            break;
          case DATASETTE_CONTROL_RECORD:
            if (current_image->read_only == 0) {
                current_image->mode = DATASETTE_CONTROL_RECORD;
                datasette_set_tape_sense(1);
                last_write_clk = (CLOCK)0;
            }
            break;
          case DATASETTE_CONTROL_RESET:
            break;
        }
    }
}

void datasette_set_motor(int flag)
{
    /*printf("MT %i\n",flag);*/
    if (current_image != NULL
        && current_image->mode != DATASETTE_CONTROL_REWIND
        && current_image->mode != DATASETTE_CONTROL_FORWARD) {
        if (flag && !datasette_motor)
        {
            fseek(current_image->fd, current_image->current_file_seek_position
                  + current_image->offset, SEEK_SET);
            if (!datasette_alarm_pending) {
                alarm_set(&datasette_alarm, clk + 10000);
                datasette_alarm_pending = 1;
            }
        }
        if (!flag && datasette_motor)
        {
            alarm_unset(&datasette_alarm);
            datasette_alarm_pending = 0;
        }
    }
    datasette_motor = flag;
}

void datasette_toggle_write_bit(int write_bit)
{
    if (current_image != NULL && datasette_motor && write_bit
        && current_image->mode == DATASETTE_CONTROL_RECORD) {
        if (last_write_clk == (CLOCK)0) {
            last_write_clk = clk;
        } else {
            CLOCK write_time;
            BYTE write_gap;
            write_time = clk - last_write_clk;
            last_write_clk = clk;
            if (write_time < (CLOCK)7)
                return;
            if (write_time < (CLOCK)(255 * 8 + 7)) {
                write_gap = (write_time / (CLOCK)8);
                if (fwrite(&write_gap, 1, 1, current_image->fd) < 1) {
                    datasette_control(DATASETTE_CONTROL_STOP);
                    return;
                }
                current_image->current_file_seek_position++;
            } else {
                write_gap = 0;
                fwrite(&write_gap, 1, 1, current_image->fd);
                current_image->current_file_seek_position++;
                if (current_image->version >= 1) {
                    BYTE long_gap[3];
                    int bytes_written;
                    long_gap[0] = write_time & 0xff;
                    long_gap[1] = (write_time >> 8) & 0xff;
                    long_gap[2] = (write_time >> 16) & 0xff;
                    bytes_written = fwrite(long_gap, 3, 1, current_image->fd);
                    current_image->current_file_seek_position += bytes_written;
                    if (bytes_written < 3) {
                        datasette_control(DATASETTE_CONTROL_STOP);
                        return;
                    }
                }
            }
        }
    }
}

