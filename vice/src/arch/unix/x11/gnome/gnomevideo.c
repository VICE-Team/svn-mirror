/*
 * ui.c - Gnome based graphical user interface.  
 *
 * Written by
 *  Martin Pottendorfer (Martin.Pottendorfer@alcatel.at
 *  Heavily derived from Oliver Schaertel (orschaer@forwiss.uni-erlangen.de)
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

#include "videoarch.h"
#include "video.h"

extern void (*_refresh_func) ();

static GdkPixmap *pixmap;
static GdkImage *im;
extern GtkWidget *canvas;
extern GdkGC *app_gc;

inline void GDK_PUTIMAGE(Display *d, void *c, GdkGC *gc,
			 GdkImage *image, int src_x, int src_y,
			 int dest_x, int dest_y,
			 unsigned int width, unsigned int height, int b)
{
  gdk_draw_image(pixmap,gc,im,src_x,src_y,
		 dest_x,dest_y,width,height);
  gdk_window_clear_area(canvas->window,dest_x,dest_y,width,height);

  gdk_flush();
}


int video_frame_buffer_alloc(frame_buffer_t * i, unsigned int width,
			     unsigned int height)
{
    int sizeofpixel = sizeof(PIXEL);
    GdkImageType typ;

    if (sizeof(PIXEL2) != sizeof(PIXEL) * 2 ||
	sizeof(PIXEL4) != sizeof(PIXEL) * 4) {
	log_error(video_log, "PIXEL2 / PIXEL4 typedefs have wrong size.");
	return -1;
    }
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


#ifdef MITSHM
    if(use_mitshm)
        typ = GDK_IMAGE_SHARED;
    else
#endif
        typ = GDK_IMAGE_NORMAL;
    {				/* !use_mitshm */
        static GdkColor col;

	/*        PIXEL *data = (PIXEL *) malloc(width * height * sizeofpixel);

	if (!data)
	  return -1;
	*/
        pixmap = gdk_pixmap_new(canvasw->window,
				width,height,depth);

	if (!pixmap)
	  return -1;
	i->gdk_image = gdk_image_new(typ,visual,width,height);

        i->x_image = GDK_IMAGE_XIMAGE(i->gdk_image);
	im = i->gdk_image;

	gdk_window_set_back_pixmap(canvasw->window,pixmap,0);

	if (!i->x_image)
	    return -1;

	_refresh_func = (void (*)()) GDK_PUTIMAGE;

	_refresh_func(_video_gc,
		      i->gdk_image, 0, 0, 0, 0, 200, 200, False);

    }

#ifdef USE_MITSHM
    log_message(video_log, "Successfully initialized%s shared memory.",
                use_mitshm ? ", using" : " without");
    if (!use_mitshm)
	log_warning(video_log, "Performance will be poor.");
#else
    log_message(video_log, "Successfully initialized without shared memory.");
#endif

#if X_DISPLAY_DEPTH == 0
    /* if display depth != 8 we need a temporary buffer */
    if (depth == 8) {
	i->tmpframebuffer = (PIXEL *) i->x_image->data;
	i->tmpframebufferlinesize = i->x_image->bytes_per_line;
	_convert_func = NULL;
    } else {
	i->tmpframebufferlinesize = width;
	i->tmpframebuffer = (PIXEL *) malloc(width * height);
	if (i->x_image->bits_per_pixel == 8)
	    _convert_func = convert_8to8;
	else if (i->x_image->bits_per_pixel == 1)
	    _convert_func = convert_8to1_dither;
	else if (i->x_image->bits_per_pixel == 16)
	    _convert_func = convert_8to16;
	else if (i->x_image->bits_per_pixel == 32)
	    _convert_func = convert_8to32;
	else
	    _convert_func = convert_8toall;
    }
#endif
    return 0;
}

GC video_get_gc(void *not_used)
{
    return (GC) app_gc;
}

void video_add_handlers(Widget w) 
{
}

/* Make the canvas visible. */
void canvas_map(canvas_t s)
{
    fprintf(stderr, "**Function `%s' not implemented.\n", __FUNCTION__);
}

/* Make the canvas not visible. */
void canvas_unmap(canvas_t s)
{
    fprintf(stderr, "**Function `%s' not implemented.\n", __FUNCTION__);
}

