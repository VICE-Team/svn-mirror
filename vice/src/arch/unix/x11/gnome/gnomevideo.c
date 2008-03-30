/*
 * ui.c - Gnome based graphical user interface.  
 *
 * Written by
 *  Martin Pottendorfer <Martin.Pottendorfer@alcatel.at>
 *  Heavily derived from Oliver Schaertel <orschaer@forwiss.uni-erlangen.de>
 *  GTK+ port.
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

#include <gdk/gdkx.h>
#include <string.h>

#include "log.h"
#include "videoarch.h"
#include "video.h"

#include "ui.h"
#include "uiarch.h"
#include "utils.h"

static log_t gnomevideo_log = LOG_ERR;

void video_init_arch(void)
{
    if (gnomevideo_log == LOG_ERR)
        gnomevideo_log = log_open("GnomeVideo");
}
extern GtkWidget *canvas;
extern GdkGC *app_gc;

inline void GDK_PUTIMAGE(Display *d, GdkPixmap *drawable, GdkGC *gc,
			 GdkImage *image, int src_x, int src_y,
			 int dest_x, int dest_y,
			 unsigned int width, unsigned int height, int b,
			 video_frame_buffer_t *fb, canvas_t *c)
{
  gdk_draw_image(drawable, gc, fb->gdk_image, src_x, src_y,
		 dest_x, dest_y, width, height);
  gdk_window_clear_area(c->emuwindow->window, dest_x, dest_y, width, height);

  gdk_flush();
}


int video_frame_buffer_alloc(video_frame_buffer_t **ip, unsigned int width,
			     unsigned int height)
{
    int sizeofpixel = sizeof(PIXEL);
    GdkImageType typ;
    int depth;
    video_frame_buffer_t *i;

    i = (video_frame_buffer_t *)xmalloc(sizeof(video_frame_buffer_t));
    memset(i, 0, sizeof(video_frame_buffer_t));
    *ip = i;

    if (sizeof(PIXEL2) != sizeof(PIXEL) * 2 ||
	sizeof(PIXEL4) != sizeof(PIXEL) * 4) {
	log_error(gnomevideo_log, "PIXEL2 / PIXEL4 typedefs have wrong size.");
	return -1;
    }

    depth = ui_get_display_depth();

    /* Round up to 32-bit boundary. */
    width = (width + 3) & ~0x3;

#if X_DISPLAY_DEPTH == 0
    /* sizeof(PIXEL) is not always what we are using. I guess this should
       be checked from the XImage but I'm lazy... */
    if (depth > 8)
	sizeofpixel *= 2;
    if (depth > 16)
	sizeofpixel *= 2;
#endif

    typ = GDK_IMAGE_FASTEST;
    i->gdk_image = gdk_image_new(typ, visual, width, height);
    i->x_image = GDK_IMAGE_XIMAGE(i->gdk_image);
    if (!i->x_image)
	return -1;

    if (i->canvas)
    {
	/* reusage of existing canvas, so reallocate drawable */
	i->canvas->width = width;
	i->canvas->height = height;
	/* destroy the old pixmap here ?
	  e.g. 	gdk_window_destroy(GDK_WINDOW(i->canvas->drawable));
	  FIXME!
	*/
	ui_finish_canvas(i->canvas);
    }
 
    video_refresh_func((void (*)(void))GDK_PUTIMAGE);

    if (video_convert_func(i, depth, width, height) < 0)
        return -1;

    log_message(gnomevideo_log,
                "Successfully initialized video.");

    return 0;
}

GC video_get_gc(void *not_used)
{
    return (GC) app_gc;
}

void video_add_handlers(ui_window_t w) 
{
}

/* Make the canvas visible. */
void canvas_map(canvas_t *s)
{
    fprintf(stderr, "**Function `%s' not implemented.\n", __FUNCTION__);
}

/* Make the canvas not visible. */
void canvas_unmap(canvas_t *s)
{
    fprintf(stderr, "**Function `%s' not implemented.\n", __FUNCTION__);
}

void ui_finish_canvas(canvas_t *c)
{
    int depth;

    depth = ui_get_display_depth();

    c->drawable = gdk_pixmap_new(c->emuwindow->window, 
				 c->width, c->height, depth);
    gdk_window_set_back_pixmap(c->emuwindow->window, 
			       c->drawable, 0);
}

