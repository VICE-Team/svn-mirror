/*
 * raster-canvas.c - Raster-based video chip emulation helper.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "videoarch.h"

#include "lib.h"
#include "machine.h"
#include "raster-canvas.h"
#include "raster.h"
#include "video.h"
#include "viewport.h"
#include "vsync.h"


void raster_canvas_handle_end_of_frame(raster_t *raster)
{
    draw_buffer_t *draw_buffer = raster->canvas->draw_buffer;
    
    if (video_disabled_mode) {
        return;
    }

    if (vsync_should_skip_frame(raster->canvas)) {
        return;
    }

    if (!raster->canvas->viewport->update_canvas) {
        return;
    }

    video_canvas_refresh_all(raster->canvas);

    /* potentially swap the draw buffer pointers */
    draw_buffer->draw_buffer = draw_buffer->padded_allocations[raster->canvas->videoconfig->interlace_field] + draw_buffer->padded_allocations_offset;
}

void raster_canvas_init(raster_t *raster)
{
    raster->update_area = lib_malloc(sizeof(raster_canvas_area_t));

    raster->update_area->is_null = 1;
}

void raster_canvas_shutdown(raster_t *raster)
{
    lib_free(raster->update_area);
}
