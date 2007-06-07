/*
 * video.m - MacVICE video interface
 *
 * Written by
 *  Christian Vogelgsang <chris@vogelgsang.org>
 *  Michael Klein <michael.klein@puffin.lb.shuttle.de>
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

#include "videoarch.h"
#include "palette.h"

#import "vicemachinenotifier.h"
#import "vicemachine.h"
#import "vicewindow.h"

// VICE Video Resources

int video_arch_resources_init(void)
{
    return 0;
}

void video_arch_resources_shutdown(void)
{
}

// VICE interface:

int video_init_cmdline_options(void)
{
    return 0;
}

int video_init(void)
{
   return 0;
}

void video_shutdown(void)
{
}

void video_arch_canvas_init(struct video_canvas_s *canvas)
{
    canvas->window = nil;
    canvas->video_draw_buffer_callback = NULL;
}

// ---------- VICE Canvas ----------

video_canvas_t *video_canvas_create(struct video_canvas_s *canvas,
                                    unsigned int *width,
                                    unsigned int *height,
                                    int mapped)
{
    int w = *width;
    int h = *height;
    
    // encapsulate canvas ptr
    video_canvas_t *canvasPtr = canvas;
    NSData *data = [NSData dataWithBytes:&canvasPtr length:sizeof(video_canvas_t *)];

    // call UI thread to create canvas
    [[theVICEMachine app] createCanvas:data withRect:NSMakeRect(100,100,w,h)];

    // init rendering
    video_canvas_set_palette(canvas,canvas->palette);

    // re-post all required notifications for new window
    [[theVICEMachine machineNotifier] notifyNewWindow];

    return canvas;
}

void video_canvas_destroy(video_canvas_t *canvas)
{
    // encapsulate canvas ptr
    video_canvas_t *canvasPtr = canvas;
    NSData *data = [NSData dataWithBytes:&canvasPtr length:sizeof(video_canvas_t *)];

    // call UI thread to destroy canvas
    [[theVICEMachine app] destroyCanvas:data];

    video_canvas_shutdown(canvas);
}

// VICE wants to change the size of the canvas -> adapt View
void video_canvas_resize(video_canvas_t * canvas,
                         unsigned int width,
                         unsigned int height)
{
    if (canvas->videoconfig->doublesizex)
        width *= 2;
    if (canvas->videoconfig->doublesizey)
        height *= 2;
    if (canvas->width == width && canvas->height == height)
        return;

    canvas->width = width;
    canvas->height = height;

    // encapsulate canvas ptr
    video_canvas_t *canvasPtr = canvas;
    NSData *data = [NSData dataWithBytes:&canvasPtr length:sizeof(video_canvas_t *)];

    // call UI thread to resize canvas
    [[theVICEMachine app] resizeCanvas:data withSize:NSMakeSize(width,height)];
}

void video_canvas_refresh(video_canvas_t *canvas,
                          unsigned int xs, unsigned int ys,
                          unsigned int xi, unsigned int yi,
                          unsigned int w, unsigned int h)
{
    if (canvas->videoconfig->doublesizex) {
        xi *= 2;
        w *= 2;
    }
    if (canvas->videoconfig->doublesizey) {
        yi *= 2;
        h *= 2;
    }
    w = MIN(w, canvas->width - xi);
    h = MIN(h, canvas->height - yi);

    // render into texture buffer
    video_canvas_render(canvas, canvas->buffer,
                        w, h, xs, ys, xi, yi, 
                        canvas->pitch, canvas->depth);

#if 1
    // call updateCanvas selector in main thread - non-blocking
    VICEWindow *window = canvas->window;
    [window performSelectorOnMainThread:@selector(updateCanvas:) 
                             withObject:nil 
                          waitUntilDone:NO];
#else
    // call updateCanvas via proxy object - blocks if main app blocks
    // encapsulate canvas ptr
    video_canvas_t *canvasPtr = canvas;    
    NSData *data = [NSData dataWithBytes:&canvasPtr length:sizeof(video_canvas_t *)];
    // call UI thread to resize canvas
    [[theVICEMachine app] updateCanvas:data];
#endif
}

// ----- Palette Stuff -----

int video_canvas_set_palette(video_canvas_t *c, palette_t *p)
{
    int i;
    int rshift = 0;
    int rbits = 0;
    int gshift = 0;
    int gbits = 0;
    int bshift = 0;
    int bbits = 0;
    DWORD rmask = 0;
    DWORD gmask = 0;
    DWORD bmask = 0;

    c->palette = p;
    for (i = 0; i < p->num_entries; i++)
    {
        DWORD col;

        switch (c->depth)
        {
            case 16:    /* RGB 5:5:5 */
                /* TODO */
                rbits = 3; rshift = 10; rmask = 0x1f;
                gbits = 3; gshift = 5; gmask = 0x1f;
                bbits = 3; bshift = 0; bmask = 0x1f;
                break;
            case 32:    /* RGB 8:8:8 */
            default:
                rbits = 0; rshift = 16; rmask = 0xff;
                gbits = 0; gshift = 8; gmask = 0xff;
                bbits = 0; bshift = 0; bmask = 0xff;
        }
        col = (p->entries[i].red   >> rbits) << rshift
            | (p->entries[i].green >> gbits) << gshift
            | (p->entries[i].blue  >> bbits) << bshift;

        video_render_setphysicalcolor(c->videoconfig, i, col, c->depth);
    }
    if (c->depth > 8)
    {
        for (i = 0; i < 256; i++)
        {
            video_render_setrawrgb(i,
                                   ((i & (rmask << rbits)) >> rbits) << rshift,
                                   ((i & (gmask << gbits)) >> gbits) << gshift,
                                   ((i & (bmask << bbits)) >> bbits) << bshift);
        }
        video_render_initraw();
    }

    return 0;
}

// ----- Color Stuff -----

int uicolor_alloc_color(unsigned int red, unsigned int green,
                        unsigned int blue, unsigned long *color_pixel,
                        BYTE *pixel_return)
{
    return 0;
}

void uicolor_convert_color_table(unsigned int colnr, BYTE *data,
                                 long color_pixel, void *c)
{
}

void uicolor_free_color(unsigned int red, unsigned int green,
                        unsigned int blue, unsigned long color_pixel)
{
}
