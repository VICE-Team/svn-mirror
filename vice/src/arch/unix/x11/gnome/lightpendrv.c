/*
 * lightpendrv.c - Gtk+ 
 *
 * Written by
 *  pottendo <pottendo@gmx.net>
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

#ifdef HAVE_MOUSE
#include <stdio.h>
#include "lightpen.h"
#include "ui.h"
#include "uiarch.h"
#include "videoarch.h"

static GdkCursor *cursor;
static int buttons;

void gtk_init_lightpen(void) {
    cursor = gdk_cursor_new(GDK_PENCIL);
}

void gtk_lightpen_setbutton(int b, int set)
{
    if (set) {
	buttons |= (b == 1) ? LP_HOST_BUTTON_1 : 0;
	buttons |= (b == 3) ? LP_HOST_BUTTON_2 : 0;
    } else {
	buttons &= (b == 1) ? ~(buttons & LP_HOST_BUTTON_1) : 0xff;
	buttons &= (b == 3) ? ~(buttons & LP_HOST_BUTTON_2) : 0xff;
    }
}

/*
#define LP_DEBUG 1
*/
void gtk_lightpen_udate(void) 
{
    int x, y;
    int h, w;
    float fx, fy;
    
    if (lightpen_enabled)  {
	
        gdk_pointer_grab(ui_cached_video_canvas->emuwindow->window, 1, 
			 GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | 
			 GDK_BUTTON_RELEASE_MASK, 
			 ui_cached_video_canvas->emuwindow->window, cursor, GDK_CURRENT_TIME);
	gdk_display_get_window_at_pointer(
	    gtk_widget_get_display(ui_cached_video_canvas->emuwindow), &x, &y);
	gdk_drawable_get_size(ui_cached_video_canvas->emuwindow->window, &w, &h);
	fx = w / (float) ui_cached_video_canvas->geometry->screen_size.width;
	fy = h / (float) (ui_cached_video_canvas->geometry->last_displayed_line - 
			  ui_cached_video_canvas->geometry->first_displayed_line + 1);
	
	x /= fx;
	y /= fy;
	
#ifdef LP_DEBUG
	fprintf(stderr,"pre : x = %i, y = %i, b = %02x, w: %d, h:%d, fx = %f, fy = %f\n", 
		x, y, buttons, w, h, fx, fy); 
#endif
    
	/* fixme for X128/VICII, first parameter to select canvas */
	lightpen_update(0, x, y, buttons);
    } else {
        gdk_pointer_ungrab(GDK_CURRENT_TIME);
	buttons = 0;
    }
}

#endif /* HAVE_MOUSE */
