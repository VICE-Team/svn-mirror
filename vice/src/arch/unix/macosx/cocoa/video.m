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
#include "resources.h"
#include "cmdline.h"
#include "translate.h"
#include "log.h"

#import "vicemachinenotifier.h"
#import "vicemachine.h"
#import "vicewindow.h"

// Mac Video Log
log_t video_log = LOG_ERR;

// video parameter struct
static video_param_t video_param;

// ---------- VICE Video Resources ----------

/* tell all canvases to reconfigure after setting new video_param resources */
static void video_reconfigure()
{
    int numCanvases = [theVICEMachine getNumCanvases];
    int i;
    
    for(i=0;i<numCanvases;i++) {
        video_canvas_t *canvas = [theVICEMachine getCanvasForId:i];

        NSData *data = [NSData dataWithBytes:&canvas length:sizeof(video_canvas_t *)];

        // call UI thread to reconfigure canvas
        [[theVICEMachine app] reconfigureCanvas:data];
    }
}

static int set_sync_draw(int val, void *param)
{
    if(val != video_param.sync_draw) {
        video_param.sync_draw = val;
        video_reconfigure();
    }
}

static int set_sync_draw_buffers(int val, void *param)
{
    if(val < 1)
        val = 0;
    else if(val > 16)
        val = 16;

    if(val != video_param.sync_draw_buffers) {            
        video_param.sync_draw_buffers = val;
        video_reconfigure();
    }
}

static resource_int_t resources_int[] =
{
    { "SyncDraw", 0, RES_EVENT_NO, NULL,
       &video_param.sync_draw, set_sync_draw, NULL },
    { "SyncDrawBuffers", 2, RES_EVENT_NO, NULL,
       &video_param.sync_draw_buffers, set_sync_draw_buffers, NULL },
    { NULL }
 };

int video_arch_resources_init(void)
{
    return resources_register_int(resources_int);
}

void video_arch_resources_shutdown(void)
{
}

// ---------- VICE Video Command Line ----------

static const cmdline_option_t cmdline_options[] = {
    { "-syncdraw", SET_RESOURCE, 0,
      NULL, NULL, "SyncDraw", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, T_("Enable draw synchronization to vertical blank") },
    { "+syncdraw", SET_RESOURCE, 0,
      NULL, NULL, "SyncDraw", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, T_("Disable draw synchronization to vertical blank") },
    { "-syncdrawbuffers", SET_RESOURCE, 1,
      NULL, NULL, "SyncDrawBuffers", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<1-8>", T_("Set number of buffers used for sync draw") },
    { NULL }
};

int video_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

// --------- init/exit MacVICE video ----------

int video_init(void)
{
    if (video_log == LOG_ERR)
  	    video_log = log_open("MacVideo");
}

void video_shutdown(void)
{
    if (video_log != LOG_ERR)
        log_close(video_log);
}

void video_arch_canvas_init(struct video_canvas_s *canvas)
{
    canvas->pitch = 0;
    canvas->window = nil;
    canvas->view = nil;
    canvas->canvasId = 0;
    
    canvas->video_param = &video_param;
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
    [[theVICEMachine app] createCanvas:data withSize:NSMakeSize(w,h)];

    // init rendering
    video_canvas_set_palette(canvas,canvas->palette);

    // register canvas in machine controller (to allow access via id)
    canvas->canvasId = [theVICEMachine registerCanvas:canvas];

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

    // get rendering buffer
    VICEGLView *view = canvas->view;
    BYTE *buffer = [view beginMachineDraw];
    if(buffer == NULL) {
        log_message(video_log, "no rendering buffer available!");
        return;
    }
        
    // update rendering buffer
    video_canvas_render(canvas, buffer,
                        w, h, xs, ys, xi, yi, 
                        canvas->pitch, canvas->depth);

    // notify end rendering
    [view endMachineDraw];
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
            | (p->entries[i].blue  >> bbits) << bshift
            | 0xff000000; // alpha

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
