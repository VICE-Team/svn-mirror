/*
 * video.c - Video implementation for BeOS.
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

#define EXACT_TYPE_NEEDED

#include "vicewindow.h"
#include <Screen.h>

extern "C" {

#include "vice.h"

#include <stdlib.h>

#include "cmdline.h"
#include "log.h"
#include "palette.h"
#include "raster.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "utils.h"
#include "video.h"

/* #define DEBUG_VIDEO */

#ifdef DEBUG_VIDEO
#define DEBUG(x) log_debug x
#else
#define DEBUG(x)
#endif

void video_resize(void);

/* Main Objects */
int number_of_canvas = 0;
//BBitmap *mainbitmap;
//extern BWindow *mainwindow;
//extern BView *mainview; 


/* ------------------------------------------------------------------------ */
/* Video-related resources.  */

/* Flag: are we in fullscreen mode?  */
int fullscreen_enabled;

static resource_t resources[] = {
    { NULL }
};

int video_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------ */

/* Video-related command-line options.  */
/*
static cmdline_option_t cmdline_options[] = {
    { NULL }
};
*/

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
int video_frame_buffer_alloc(frame_buffer_t *f,
                       unsigned int width,
                       unsigned int height)
{
	*f = (frame_buffer_t) xmalloc(sizeof(struct _frame_buffer));
	(*f)->width = width;
	(*f)->height = height;
    (*f)->buffer = (PIXEL*)xmalloc(width*height*sizeof(PIXEL));
	DEBUG(("video_frame_buffer_alloc mit %dx%d bei %x",width,height,(*f)->buffer));   	
    return 0;
}

void video_frame_buffer_free(frame_buffer_t *f)
{
    frame_buffer_t  tempf;

	if (!f || !*f)
		return;
	tempf = *f;
	DEBUG(("video_frame_buffer_free: %x",tempf->buffer));   	
	*f = NULL;
	free(tempf->buffer);
	free(tempf);
}

void video_frame_buffer_clear(frame_buffer_t *f, PIXEL value)
{
	memset((*f)->buffer,value,(*f)->height*(*f)->width);
}

/* ------------------------------------------------------------------------ */
/* Canvas functions.  */

canvas_t canvas_create(const char *title, unsigned int *width,
                       unsigned int *height, int mapped,
                       canvas_redraw_t exposure_handler,
                       const palette_t *palette, PIXEL *pixel_return)
{
	canvas_t new_canvas;
    DEBUG(("Creating canvas width=%d height=%d", *width, *height));

    new_canvas = (canvas_t)xmalloc(sizeof(struct _canvas));
    if (!new_canvas)
	return (canvas_t) NULL;

    new_canvas->title = stralloc(title);
    canvas_set_palette(new_canvas, palette, pixel_return);

    new_canvas->width = *width;
    new_canvas->height = *height;
    new_canvas->palette = palette; 
    
    new_canvas->exposure_handler = exposure_handler;
	
	new_canvas->vicewindow = 
		new ViceWindow(BRect(0,0,*width-1,*height-1),title);

	number_of_canvas++;
	new_canvas->vicewindow->MoveTo(number_of_canvas*30,number_of_canvas*30);
    return new_canvas;
}

/* Destroy `s'.  */
void canvas_destroy(canvas_t c)
{
	delete c->vicewindow;
	free(c->title);
}

/* Make `s' visible.  */
void canvas_map(canvas_t c)
{
}

/* Make `s' unvisible.  */
void canvas_unmap(canvas_t c)
{
}

/* Change the size of `s' to `width' * `height' pixels.  */
void canvas_resize(canvas_t c, unsigned int width, unsigned int height)
{
	c->vicewindow->Resize(width,height);
	DEBUG(("canvas_resize to %d x %d",width,height));
}

/* Set the palette of `c' to `p', and return the pixel values in
   `pixel_return[].  */
int canvas_set_palette(canvas_t c, const palette_t *p, PIXEL *pixel_return)
{
	int i;
	DEBUG(("Allocating colors"));
	for (i = 0; i < p->num_entries; i++)
	{
		pixel_return[i] = BScreen().IndexForColor(
			(uint8) p->entries[i].red,
			(uint8) p->entries[i].green,
			(uint8) p->entries[i].blue
		);
	}
    return 0;
}

/* ------------------------------------------------------------------------ */
void canvas_refresh(canvas_t c, frame_buffer_t f,
                    unsigned int xs, unsigned int ys,
                    unsigned int xi, unsigned int yi,
                    unsigned int w, unsigned int h)
{
	int y;
	for (y=0; y<h; y++)
	{
		memcpy((PIXEL*)c->vicewindow->bitmap->Bits()
			+ (yi+y) * c->vicewindow->bitmap->BytesPerRow() + xi,
			(PIXEL*)f->buffer + (ys+y) * FRAME_BUFFER_LINE_SIZE(f) + xs,w);
	}
	c->vicewindow->DrawBitmap();
}

}