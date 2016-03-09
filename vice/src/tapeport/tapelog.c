/*
 * tapelog.c - Generic tapeport diagnostic tool / logger emulation.
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

/* tapelog:

This device is attached to the tape port and has a passthrough port
to which any other device can be attached. The device logs any
data that goes through.

TAPEPORT | TAPELOG
--------------------------------
  MOTOR  | MOTOR IN -> MOTOR OUT
  SENSE  | SENSE OUT <- SENSE IN
  WRITE  | WRITE IN -> WRITE OUT
  READ   | READ OUT <- READ IN
*/

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmdline.h"
#include "log.h"
#include "maincpu.h"
#include "resources.h"
#include "tapeport.h"
#include "translate.h"
#include "util.h"

/* Device enabled */
static int tapelog_enabled = 0;

/* log to specified file (1) or generic logfile (0) */
static int tapelog_destination = 0;

/* log filename */
static char *tapelog_filename = NULL;

/* keep track of lines */
static BYTE tapelog_motor = 2;
static BYTE tapelog_sense = 2;
static BYTE tapelog_write = 2;
static unsigned int tapelog_read  = 0;

/* ------------------------------------------------------------------------- */

/* Some prototypes are needed */
static void tapelog_set_motor(int flag);
static void tapelog_toggle_write_bit(int write_bit);
static void tapelog_trigger_flux_change_passthrough(unsigned int on);
static void tapelog_set_tape_sense_passthrough(int sense);

static tapeport_device_t tapelog_device = {
    "Tape Log",
    IDGS_TAPE_LOG,
    0,
    NULL,
    tapelog_set_motor,
    tapelog_toggle_write_bit,
    tapelog_trigger_flux_change_passthrough,
    tapelog_set_tape_sense_passthrough
};

static tapeport_device_list_t *tapelog_list_item = NULL;

static log_t log_tapelog;

static FILE *tapelog_out = NULL;

/* ------------------------------------------------------------------------- */

static int enable_tapelog(void)
{
    if (tapelog_destination) {
        tapelog_out = fopen(tapelog_filename, "w+");
        if (tapelog_out == NULL) {
            return -1;
        }
        fprintf(tapelog_out, "\n-------------------------------------------------------------------------\n\n");
    } else {
        log_tapelog = log_open("Tape Log");
    }
    return 0;
}

static void disable_tapelog(void)
{
    if (tapelog_destination) {
        fclose(tapelog_out);
        tapelog_out = NULL;
    } else {
        log_close(log_tapelog);
    }
}

#if 0
/* Maybe to be used in the future */
static void tapelog_message(char *msg)
{
    if (tapelog_destination) {
        fprintf(tapelog_out, "%s\n", msg);
    } else {
        log_message(log_tapelog, msg);
    }
}
#endif

static void tapelog_initial_set(char *line, BYTE val)
{
    if (tapelog_destination) {
        fprintf(tapelog_out, "Initial set of %s to %d at %X\n", line, val, (unsigned int)maincpu_clk);
    } else {
        log_message(log_tapelog, "Initial set of %s to %d at %X", line, val, (unsigned int)maincpu_clk);
    }
}

static void tapelog_transition(char *line, BYTE val)
{
    if (tapelog_destination) {
        fprintf(tapelog_out, "%s: %d -> %d at %X\n", line, !val, val, (unsigned int)maincpu_clk);
    } else {
        log_message(log_tapelog, "%s: %d -> %d at %X", line, !val, val, (unsigned int)maincpu_clk);
    }
}

/* ------------------------------------------------------------------------- */

static int set_tapelog_enabled(int value, void *param)
{
    int val = value ? 1 : 0;

    if (tapelog_enabled == val) {
        return 0;
    }

    if (val) {
        if (enable_tapelog() < 0) {
            return -1;
        }
        tapelog_list_item = tapeport_device_register(&tapelog_device);
        if (tapelog_list_item == NULL) {
            disable_tapelog();
            return -1;
        }
    } else {
        disable_tapelog();
        tapeport_device_unregister(tapelog_list_item);
        tapelog_list_item = NULL;
    }

    tapelog_enabled = val;
    return 0;
}

static int set_tapelog_destination(int value, void *param)
{
    int val = value ? 1 : 0;

    if (tapelog_destination == val) {
        return 0;
    }

    if (tapelog_enabled) {
        disable_tapelog();
    }

    tapelog_destination = val;

    if (tapelog_enabled) {
        if (enable_tapelog() < 0) {
            return -1;
        }
    }

    return 0;
}

static int set_tapelog_filename(const char *name, void *param)
{
    if (tapelog_filename != NULL && name != NULL && strcmp(name, tapelog_filename) == 0) {
        return 0;
    }

    if (name != NULL && *name != '\0') {
        if (util_check_filename_access(name) < 0) {
            return -1;
        }
    }

    if (tapelog_enabled && tapelog_destination) {
        disable_tapelog();
        util_string_set(&tapelog_filename, name);
        if (enable_tapelog() < 0) {
            return -1;
        }
    } else {
        util_string_set(&tapelog_filename, name);
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

static const resource_int_t resources_int[] = {
    { "TapeLog", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &tapelog_enabled, set_tapelog_enabled, NULL },
    { "TapeLogDestination", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &tapelog_destination, set_tapelog_destination, NULL },
    { NULL }
};

static const resource_string_t resources_string[] = {
    { "TapeLogfilename", "", RES_EVENT_NO, NULL,
      &tapelog_filename, set_tapelog_filename, NULL },
    { NULL }
};

int tapelog_resources_init(void)
{
    if (resources_register_string(resources_string) < 0) {
        return -1;
    }

    return resources_register_int(resources_int);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] =
{
    { "-tapelog", SET_RESOURCE, 0,
      NULL, NULL, "TapeLog", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_TAPELOG,
      NULL, NULL },
    { "+tapelog", SET_RESOURCE, 0,
      NULL, NULL, "TapeLog", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_TAPELOG,
      NULL, NULL },
    { "-tapelogtofile", SET_RESOURCE, 0,
      NULL, NULL, "TapeLogDestination", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_TAPELOG_LOG_TO_FILE,
      NULL, NULL },
    { "-tapelogtolog", SET_RESOURCE, 0,
      NULL, NULL, "TapeLogDestination", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_TAPELOG_LOG_TO_LOG,
      NULL, NULL },
    { "-tapelogimage", SET_RESOURCE, 1,
      NULL, NULL, "TapeLogfilename", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_TAPELOG_NAME,
      NULL, NULL },
    { NULL }
};

int tapelog_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ---------------------------------------------------------------------*/

static void tapelog_set_motor(int flag)
{
    BYTE val = flag ? 1 : 0;

    if (tapelog_motor == val) {
        return;
    }

    if (tapelog_motor == 2) {
        tapelog_initial_set("motor", val);
    } else {
        tapelog_transition("motor", val);
    }

    tapelog_motor = val;
    tapeport_set_motor_next(flag, tapelog_device.id);
}

static void tapelog_toggle_write_bit(int write_bit)
{
    BYTE val = write_bit ? 1 : 0;

    if (tapelog_write == val) {
        return;
    }

    if (tapelog_write == 2) {
        tapelog_initial_set("write", val);
    } else {
        tapelog_transition("write", val);
    }

    tapelog_write = val;
    tapeport_toggle_write_bit_next(write_bit, tapelog_device.id);
}

static void tapelog_set_tape_sense_passthrough(int sense)
{
    BYTE val = sense ? 1 : 0;

    if (tapelog_sense == val) {
        return;
    }

    if (tapelog_sense == 2) {
        tapelog_initial_set("sense", val);
    } else {
        tapelog_transition("sense", val);
    }

    tapelog_sense = val;
    tapeport_set_tape_sense(sense, tapelog_device.id);
}

void tapelog_trigger_flux_change_passthrough(unsigned int on)
{
    tapeport_trigger_flux_change(on, tapelog_device.id);

    tapelog_transition("read", (BYTE)on);

    tapelog_read = on;
}
