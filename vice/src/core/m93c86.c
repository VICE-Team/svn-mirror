/*
 * m93c86.c
 *
 * Written by
 *  Groepaz/Hitmen <groepaz@gmx.net>
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

/* #define M93C86DEBUG */

#include "vice.h"

#include <stdio.h>
#include <string.h> /* for memset */

#include "log.h"
#include "m93c86.h"
#include "snapshot.h"
#include "types.h"
#include "util.h"

#ifdef M93C86DEBUG
#define LOG(_x_) log_debug _x_
#else
#define LOG(_x_)
#endif

/* FIXME get rid of this */
#define M93C86_SIZE (2048)

/* Image file */
static FILE *m93c86_image_file = NULL;

static BYTE m93c86_data[M93C86_SIZE];

BYTE m93c86_read_data(void)
{
}

void m93c86_write_data(BYTE value)
{
}

void m93c86_write_select(BYTE value)
{
}

void m93c86_write_clock(BYTE value)
{
}

int m93c86_open_image(char *name, int rw)
{
    char *m93c86_image_filename = name;

    if (m93c86_image_filename != NULL) {
        /* FIXME */
    } else {
        /* FIXME */
        log_debug("eeprom card image name not set");
        return 0;
    }

    if (m93c86_image_file != NULL) {
        m93c86_close_image(rw);
    }

    if (rw) {
        m93c86_image_file = fopen(m93c86_image_filename, "rb+");
    }

    if (m93c86_image_file == NULL) {
        m93c86_image_file = fopen(m93c86_image_filename, "rb");

        if (m93c86_image_file == NULL) {
            log_debug("could not open eeprom card image: %s", m93c86_image_filename);
            return -1;
        } else {
            if (fread(m93c86_data, 1, M93C86_SIZE, m93c86_image_file) == 0) {
                log_debug("could not read eeprom card image: %s", m93c86_image_filename);
            }
            fseek(m93c86_image_file, 0, SEEK_SET);
            log_debug("opened eeprom card image (ro): %s", m93c86_image_filename);
        }
    } else {
        if (fread(m93c86_data, 1, M93C86_SIZE, m93c86_image_file) == 0) {
            log_debug("could not read eeprom card image: %s", m93c86_image_filename);
        }
        fseek(m93c86_image_file, 0, SEEK_SET);
        log_debug("opened eeprom card image (rw): %s", m93c86_image_filename);
    }
    return 0;
}

void m93c86_close_image(int rw)
{
    /* unmount EEPROM image */
    if (m93c86_image_file != NULL) {
        if (rw) {
            fseek(m93c86_image_file, 0, SEEK_SET);
            if (fwrite(m93c86_data, 1, M93C86_SIZE, m93c86_image_file) == 0) {
                log_debug("could not write eeprom card image");
            }
        }
        fclose(m93c86_image_file);
        m93c86_image_file = NULL;
    }
}

/* ---------------------------------------------------------------------*/
/*    snapshot support functions                                             */

#define CART_DUMP_VER_MAJOR   0
#define CART_DUMP_VER_MINOR   0
#define SNAP_MODULE_NAME  "M93C86"

/* FIXME: implement snapshot support */
int m93c86_snapshot_write_module(snapshot_t *s)
{
    return -1;
#if 0
    snapshot_module_t *m;

    m = snapshot_module_create(s, SNAP_MODULE_NAME,
                               CART_DUMP_VER_MAJOR, CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
#endif
}

int m93c86_snapshot_read_module(snapshot_t *s)
{
    return -1;
#if 0
    BYTE vmajor, vminor;
    snapshot_module_t *m;

    m = snapshot_module_open(s, SNAP_MODULE_NAME, &vmajor, &vminor);
    if (m == NULL) {
        return -1;
    }

    if ((vmajor != CART_DUMP_VER_MAJOR) || (vminor != CART_DUMP_VER_MINOR)) {
        snapshot_module_close(m);
        return -1;
    }

    if (0) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
#endif
}
