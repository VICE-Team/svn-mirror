/*
 * raster-snapshot.c
 *
 * Written by
 *  David Hogan <david.q.hogan@gmail.com>
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

#include <stddef.h>

#include "raster-snapshot.h"

#include "lib.h"
#include "raster.h"
#include "snapshot.h"
#include "videoarch.h"

int raster_snapshot_write(snapshot_module_t *m, raster_t *raster)
{
    struct draw_buffer_s *draw_buffer = raster->canvas->draw_buffer;

    if (0
        || SMW_DW(m, raster->current_line) < 0
        || SMW_DW(m, draw_buffer->width) < 0
        || SMW_DW(m, draw_buffer->height) < 0
        || SMW_DW(m, draw_buffer->width) < 0    /* was pitch, but that was removed, put in width for compatibility */
        ) {
        return -1;
    }

    if (0
        || SMW_BA(m, draw_buffer->padded_allocations[0], draw_buffer->padded_allocations_size_bytes) < 0
        ) {
        return -1;
    }

    /* If the chip supports interlaced output, store the additional field */

    if (raster->canvas->videoconfig->cap->interlace_allowed) {
        if (0
            || SMW_BA(m, draw_buffer->padded_allocations[1], draw_buffer->padded_allocations_size_bytes) < 0
            || SMW_DW(m, raster->canvas->videoconfig->interlace_field) < 0
            ) {
            return -1;
        }
    }

    return 0;
}

int raster_snapshot_read(snapshot_module_t *m, raster_t *raster)
{
    unsigned int unused_value;
    struct draw_buffer_s *draw_buffer = raster->canvas->draw_buffer;

    if (0
        || SMR_DW(m, &raster->current_line) < 0
        || SMR_DW(m, &draw_buffer->width) < 0
        || SMR_DW(m, &draw_buffer->height) < 0
        || SMR_DW(m, &unused_value) < 0 /* was pitch, value no longer used */
        ) {
        return -1;
    }

    raster_calculate_padding_size(draw_buffer->width,
                                  draw_buffer->height,
                                  &draw_buffer->padded_allocations_size_bytes,
                                  &draw_buffer->padded_allocations_offset);

    draw_buffer->padded_allocations[0] = lib_realloc(draw_buffer->padded_allocations[0], draw_buffer->padded_allocations_size_bytes);
    draw_buffer->draw_buffer = draw_buffer->padded_allocations[0] + draw_buffer->padded_allocations_offset;

    if (0
        || SMR_BA(m, draw_buffer->padded_allocations[0], draw_buffer->padded_allocations_size_bytes) < 0
        ) {
        return -1;
    }

    /*
     * Interlaced video chips retain two draw buffers
     */

    if (raster->canvas->videoconfig->cap->interlace_allowed) {
        draw_buffer->padded_allocations[1] = lib_realloc(draw_buffer->padded_allocations[1], draw_buffer->padded_allocations_size_bytes);

        if (0
            || SMR_BA(m, draw_buffer->padded_allocations[1], draw_buffer->padded_allocations_size_bytes) < 0
            || SMR_DW_INT(m, &raster->canvas->videoconfig->interlace_field) < 0
            ) {
            return -1;
        }

        /* Update the current draw buffer based on the interlace field */
        draw_buffer->draw_buffer = draw_buffer->padded_allocations[raster->canvas->videoconfig->interlace_field & 1] + draw_buffer->padded_allocations_offset;
    }

    return 0;
}
