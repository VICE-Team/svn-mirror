/*
 * tapeimage.c - Common low-level tape image access.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "datasette.h"
#include "log.h"
#include "t64.h"
#include "tap.h"
#include "tape.h"
#include "ui.h"


/* T64 tape currently attached.  */
t64_t *attached_t64_tape = NULL;

/* TAP tape currently attached.  */
tap_t *attached_tap_tape = NULL;

static log_t tape_image_log = LOG_ERR;

/* Detach.  */
int tape_image_detach(unsigned int unit)
{
    int retval;

    if (attached_t64_tape != NULL) {
        log_message(tape_image_log,
                    "Detaching T64 image `%s'.", attached_t64_tape->file_name);

        /* Gone.  */
        retval = t64_close(attached_t64_tape);
        attached_t64_tape = NULL;
        ui_display_tape_current_image("");

        /* Tape detached: release play button.  */
        datasette_set_tape_sense(0);

        return retval;
    }

    if (attached_tap_tape != NULL) {
        log_message(tape_image_log,
                    "Detaching TAP image `%s'.", attached_tap_tape->file_name);

        /* Gone.  */
        retval = tap_close(attached_tap_tape);
        attached_tap_tape = NULL;
        ui_display_tape_current_image("");
        datasette_set_tape_image(NULL);

        tape_traps_install();

        return retval;
    }

    return 0;
}

/* Attach.  */
int tape_image_attach(unsigned int unit, const char *name)
{
    t64_t *new_t64_tape;
    tap_t *new_tap_tape;

    if (!name || !*name)
        return -1;

    new_t64_tape = t64_open(name);
    if (new_t64_tape != NULL) {
        tape_image_detach(1);
        attached_t64_tape = new_t64_tape;
        ui_display_tape_current_image(name);

        log_message(tape_image_log, "T64 image '%s' attached.", name);

        /* Tape attached: press play button.  */
        datasette_set_tape_sense(1);

        return 0;
    }

    new_tap_tape = tap_open(name);
    if (new_tap_tape != NULL) {
        tape_image_detach(1);
        attached_tap_tape = new_tap_tape;
        ui_display_tape_current_image(name);

        datasette_set_tape_image(new_tap_tape);

        log_message(tape_image_log, "TAP image '%s' attached.", name);
        log_message(tape_image_log, "TAP image version: %i, system: %i.",
                    new_tap_tape->version, new_tap_tape->system);

        tape_traps_deinstall();

        return 0;
    }

    return -1;
}

int tape_tap_attched(void)
{
    if (attached_tap_tape != NULL)
        return 1;

    return 0;
}

void tape_image_init(void)
{
    tape_image_log = log_open("Tape Access");
}

