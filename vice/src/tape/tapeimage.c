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
#include <stdlib.h>
#include <string.h>

#include "datasette.h"
#include "log.h"
#include "t64.h"
#include "tap.h"
#include "tape.h"
#include "ui.h"
#include "utils.h"


static log_t tape_image_log = LOG_ERR;

int tape_image_close(tape_image_t *tape_image)
{
    int retval = 0;

    switch (tape_image->type) {
      case TAPE_TYPE_T64:
        retval = t64_close((t64_t *)tape_image->data);
        break;
      case TAPE_TYPE_TAP:
        retval = tap_close((tap_t *)tape_image->data);
        break;
    }

    free(tape_image->name);
    tape_image->name = NULL;

    return retval;
}

int tape_image_open(tape_image_t *tape_image)
{
    t64_t *new_t64_tape;
    tap_t *new_tap_tape;

    new_t64_tape = t64_open(tape_image->name, &tape_image->read_only);
    if (new_t64_tape != NULL) {
        tape_image->data = (void *)new_t64_tape;
        tape_image->type = TAPE_TYPE_T64;
        return 0;
    }

    new_tap_tape = tap_open(tape_image->name, &tape_image->read_only);
    if (new_tap_tape != NULL) {
        tape_image->data = (void *)new_tap_tape;
        tape_image->type = TAPE_TYPE_TAP;
        return 0;
    }

    return -1;
}

/* Detach.  */
int tape_image_detach(unsigned int unit)
{
    int retval = 0;

    if (unit != 1)
        return -1;

    if (tape_image_dev1->name == NULL)
        return 0;

    switch (tape_image_dev1->type) {
      case TAPE_TYPE_T64:
        log_message(tape_image_log,
                    "Detaching T64 image `%s'.", tape_image_dev1->name);
        /* Tape detached: release play button.  */
        datasette_set_tape_sense(0);
        break;
      case TAPE_TYPE_TAP:
        log_message(tape_image_log,
                    "Detaching TAP image `%s'.", tape_image_dev1->name);
        datasette_set_tape_image(NULL);

        tape_traps_install();
        break;
      default:
        log_error(tape_image_log, "Unknown tape type %i.",
                  tape_image_dev1->type);
    }

    retval = tape_image_close(tape_image_dev1);

    ui_display_tape_current_image("");

    return retval;
}

/* Attach.  */
int tape_image_attach(unsigned int unit, const char *name)
{
    tape_image_t tape_image;

    if (unit != 1)
        return -1;

    if (!name || !*name)
        return -1;

    tape_image.name = stralloc(name);
    tape_image.read_only = 0;

    if (tape_image_open(&tape_image) < 0) {
        free(tape_image.name);
        log_error(tape_image_log, "Cannot open file `%s'", name);
        return -1;
    }

    tape_image_detach(unit);

    memcpy(tape_image_dev1, &tape_image, sizeof(tape_image_t));

    ui_display_tape_current_image(tape_image_dev1->name);

    switch (tape_image_dev1->type) {
      case TAPE_TYPE_T64:
        log_message(tape_image_log, "T64 image '%s' attached.", name);
        /* Tape attached: press play button.  */
        datasette_set_tape_sense(1);
        break;
      case TAPE_TYPE_TAP:
        datasette_set_tape_image((tap_t *)tape_image_dev1->data);
        log_message(tape_image_log, "TAP image '%s' attached.", name);
        log_message(tape_image_log, "TAP image version: %i, system: %i.",
                    ((tap_t *)tape_image_dev1->data)->version,
                    ((tap_t *)tape_image_dev1->data)->system);
        tape_traps_deinstall();
        break;
      default:
        log_error(tape_image_log, "Unknown tape type %i.",
                  tape_image_dev1->type);
        return -1;
    }

    return 0;
}

void tape_image_init(void)
{
    tape_image_log = log_open("Tape Access");
}

