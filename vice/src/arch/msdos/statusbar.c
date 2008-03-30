/*
 * statusbar.c
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@arcormail.de>
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

#include <allegro.h>

#include "vice.h"

#include "log.h"
#include "resources.h"
#include "statusbar.h"
#include "videoarch.h"

/* the bitmap with the status informations */
BITMAP *status_bitmap = NULL;

/* behind the bitmap with the status informations */
BITMAP *behind_status_bitmap = NULL;

/* where has the status-bitmap to be copied? 
   screen or triple-buffer pages? */
BITMAP *bitmaps_to_update[2];
int nr_of_bitmaps = 0;

/* to center the statusbar we need the vga-width */
int vga_width = 0;

int statusbar_enabled(void) {
    int val;
    resources_get_value("ShowStatusbar",(resource_value_t) &val);
    return val;
}

int statusbar_init(void) 
{
    status_bitmap = create_bitmap(STATUSBAR_WIDTH,STATUSBAR_HEIGHT);
    behind_status_bitmap = create_bitmap(STATUSBAR_WIDTH,STATUSBAR_HEIGHT);
    if (!status_bitmap || !behind_status_bitmap) {
        log_error(LOG_ERR,"Cannot allocate statusbar bitmap");
	    return -1;
    } else {
        clear(behind_status_bitmap);
        clear(status_bitmap);
        rect(status_bitmap,0,0,STATUSBAR_WIDTH-1,
            STATUSBAR_HEIGHT-1,STATUSBAR_COLOR_WHITE);
    }
    return 0;

}


void statusbar_exit(void) 
{
    if (status_bitmap) {
        destroy_bitmap(status_bitmap);
        status_bitmap = NULL;
    }
    if (behind_status_bitmap) {
        destroy_bitmap(behind_status_bitmap);
        status_bitmap = NULL;
    }
}


void statusbar_reset_bitmaps_to_update(void) 
{
    nr_of_bitmaps = 0;
}


void statusbar_append_bitmap_to_update(BITMAP *b) 
{
    bitmaps_to_update[nr_of_bitmaps++] = b;
}


static void statusbar_to_screen(BITMAP* bitmap)
{
    int b;
    if (bitmap == NULL)
        return;

    for (b = 0; b < nr_of_bitmaps; b++)
    {
        blit(bitmap, bitmaps_to_update[b], 0, 0,
            (vga_width-STATUSBAR_WIDTH)/2, 0,
            STATUSBAR_WIDTH,STATUSBAR_HEIGHT);
    }
}


/* print the status_bitmap to screen */
void statusbar_update() 
{
    if (!video_in_gfx_mode() 
        || !statusbar_enabled()
        || (status_bitmap == NULL))
        return;

    statusbar_to_screen(status_bitmap);
}


void statusbar_disable()
{
    if (!video_in_gfx_mode() || (behind_status_bitmap == NULL))
        return;

    statusbar_to_screen(behind_status_bitmap);
    raster_mode_change();

}


void statusbar_set_width(int w)
{
    vga_width = w;
}


