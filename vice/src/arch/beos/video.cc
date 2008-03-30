/*
 * video.c - Video implementation for BeOS.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#define EXACT_TYPE_NEEDED

#include "vicewindow.h"
#include <Bitmap.h>
#include <Screen.h>
#include <stdlib.h>

extern "C" {
#include "vice.h"
#include "cmdline.h"
#include "log.h"
#include "palette.h"
#include "raster.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "uiapi.h"
#include "utils.h"
#include "video.h"
#include "videoarch.h"
}

/* #define DEBUG_VIDEO */

#ifdef DEBUG_VIDEO
#define DEBUG(x) log_debug x
#else
#define DEBUG(x)
#endif

void video_resize(void);

/* Main Objects */
int number_of_canvas = 0;

/* ------------------------------------------------------------------------ */
/* Video-related resources.  */

/* Flag: are we in fullscreen mode?  */
int fullscreen_enabled;

static resource_t resources[] = {
    { NULL }
};

int video_arch_init_resources(void)
{
    return resources_register(resources);
}

int video_init_cmdline_options(void)
{
    return 0;
}

/* ------------------------------------------------------------------------ */
/* Initialization.  */
int video_init(void)
{

    return 0;
}

/* ------------------------------------------------------------------------ */
/* Frame buffer functions.  */
int video_frame_buffer_alloc(video_frame_buffer_t **f,
                       unsigned int width,
                       unsigned int height)
{
	*f = (video_frame_buffer_t *) xmalloc(sizeof(video_frame_buffer_t));
	(*f)->width = width;
	(*f)->height = height;
	(*f)->buffer = (PIXEL*) xmalloc(width*height);
	DEBUG(("video_frame_buffer_alloc: %dx%d at %x",width,height,(*f)->buffer)); 
	  	
    return 0;
}

void video_frame_buffer_free(video_frame_buffer_t *f)
{
	if (!f)
		return;

	DEBUG(("video_frame_buffer_free: %x",f->buffer)); 
	free(f->buffer);
	free(f);
}

void video_frame_buffer_clear(video_frame_buffer_t *f, PIXEL value)
{
	memset(f->buffer, value, f->height * f->width);
}

/* ------------------------------------------------------------------------ */
/* Canvas functions.  */

static void canvas_create_bitmap(video_canvas_t *c,
							     unsigned int width,
							     unsigned int height)
{
	color_space use_colorspace;

    switch (c->depth) {
    	case 8:
    		use_colorspace = B_CMAP8;
    		break;
    	case 16:
    		use_colorspace = B_RGB16;
    		break;
    	case 32:
		default:
    		use_colorspace = B_RGB32;
	}
	
	c->vicewindow->bitmap = new BBitmap(BRect(0,0,width-1,height-1),
			use_colorspace,false,true);
}    


video_canvas_t *video_canvas_create(const char *title, unsigned int *width,
                              unsigned int *height, int mapped,
                              void_t exposure_handler,
                              const palette_t *palette, PIXEL *pixel_return,
                              struct video_frame_buffer_s *fb)
{
    video_canvas_t *new_canvas;
    DEBUG(("Creating canvas width=%d height=%d", *width, *height));

    new_canvas = (video_canvas_t *)xmalloc(sizeof(struct video_canvas_s));
    if (!new_canvas)
	return (video_canvas_t *) NULL;

    video_render_initconfig(&new_canvas->videoconfig);
	new_canvas->title = stralloc(title);
    switch (BScreen().ColorSpace()) {
    	case B_CMAP8:
    		new_canvas->depth = 8;
    		break;
    	case B_RGB15:
    	case B_RGB16:
    		new_canvas->depth = 16;
    		break;
    	case B_RGB32:
		default:
			new_canvas->depth = 32;
	}
    video_canvas_set_palette(new_canvas, palette, pixel_return);

    new_canvas->width = *width;
    new_canvas->height = *height;
    new_canvas->palette = palette;

    new_canvas->exposure_handler = (canvas_redraw_t)exposure_handler;
	
	new_canvas->vicewindow = 
		new ViceWindow(BRect(0,0,*width-1,*height-1),title);
		
	canvas_create_bitmap(new_canvas, *width, *height);

	number_of_canvas++;
	new_canvas->vicewindow->MoveTo(number_of_canvas*30,number_of_canvas*30);

    return new_canvas;
}


/* Destroy `s'.  */
void video_canvas_destroy(video_canvas_t *c)
{
	if (c == NULL)
		return;

	delete c->vicewindow->bitmap;
	delete c->vicewindow;
	free(c->title);
	free(c);
}


/* Change the size of `s' to `width' * `height' pixels.  */
void video_canvas_resize(video_canvas_t *c, unsigned int width,
                         unsigned int height)
{
	delete c->vicewindow->bitmap;
	canvas_create_bitmap(c, width, height);
    
	c->vicewindow->Resize(width,height);
	c->width = width;
	c->height = height;
	DEBUG(("video_canvas_resize to %d x %d",width,height));
}

/* Set the palette of `c' to `p', and return the pixel values in
   `pixel_return[].  */
int video_canvas_set_palette(video_canvas_t *c, const palette_t *p, PIXEL *pixel_return)
{
	int i;
	c->palette = p;
	DEBUG(("Allocating colors"));
	for (i = 0; i < p->num_entries; i++)
	{
		DWORD col;
		pixel_return[i] = i;

		switch (c->depth) {
			case 8:		/* CMAP8 */
				col = BScreen().IndexForColor(
							p->entries[i].red,
							p->entries[i].green,
							p->entries[i].blue);
				break;
			case 16:	/* RGB 5:6:5 */
				col = (p->entries[i].red >> 3) << 11
							|	(p->entries[i].green >> 2) << 5
							|	(p->entries[i].blue >> 3);
				break;
			case 32:	/* RGB 8:8:8 */
			default:
				col = p->entries[i].red << 16
							|	p->entries[i].green << 8
							|	p->entries[i].blue;
		}
		video_render_setphysicalcolor(&c->videoconfig, i, col, c->depth);
	}
    return 0;
}

/* ------------------------------------------------------------------------ */
void video_canvas_refresh(video_canvas_t *c, video_frame_buffer_t *f,
                          unsigned int xs, unsigned int ys,
                          unsigned int xi, unsigned int yi,
                          unsigned int w, unsigned int h)
{
	w = MIN(w, c->width - xi);
	h = MIN(h, c->height - yi);

	video_render_main(&c->videoconfig,
                          (BYTE *)(VIDEO_FRAME_BUFFER_START(f)),
                          (BYTE *)(c->vicewindow->bitmap->Bits()),
                          w, h,
                          xs, ys,
                          xi, yi,
                          VIDEO_FRAME_BUFFER_LINE_SIZE(f),
                          c->vicewindow->bitmap->BytesPerRow(),
                          c->depth);

	c->vicewindow->DrawBitmap(c->vicewindow->bitmap,xi,yi,xi,yi,w,h);
}
