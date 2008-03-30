/*
 * video.h - Common video API.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#ifndef _VIDEO_H
#define _VIDEO_H

#include "types.h"
#include "viewport.h"


/* These constants are used to configure the video output.  */

/* PAL is for PAL based video outputs, like VIC, VIC-II or TED */
/* RGB is for anything which doesn't need any color filtering  */
/* for display, like monochrome or rgbi (CRTC and VDC)         */

#define VIDEO_RENDER_NULL       0
#define VIDEO_RENDER_PAL_1X1    1
#define VIDEO_RENDER_PAL_2X2    2
#define VIDEO_RENDER_RGB_1X1    3
#define VIDEO_RENDER_RGB_1X2    4
#define VIDEO_RENDER_RGB_2X2    5

struct canvas_refresh_s
{
    BYTE *draw_buffer;
    int draw_buffer_line_size;
#ifdef __OS2__
    int bufh;
#endif
    int x;
    int y;
};
typedef struct canvas_refresh_s canvas_refresh_t;

struct draw_buffer_s {
    BYTE *draw_buffer;
    unsigned int draw_buffer_width;
    unsigned int draw_buffer_height;
    unsigned int draw_buffer_pitch;
    unsigned int canvas_width;
    unsigned int canvas_height;
};
typedef struct draw_buffer_s draw_buffer_t;

struct video_render_config_s {
    int rendermode;             /* what renderers are allowed? */
    int doublesizex;            /* doublesizex enabled?        */
    int doublesizey;            /* doublesizey enabled?        */
    int doublescan;             /* doublescan enabled?         */
    DWORD physical_colors[256];
};
typedef struct video_render_config_s video_render_config_t;

extern void video_render_initconfig(video_render_config_t *config);
extern void video_render_setphysicalcolor(video_render_config_t *config,
                                          int index, DWORD color, int depth);
extern void video_render_setrawrgb(int index, DWORD r, DWORD g, DWORD b);
extern void video_render_initraw(void);

/**************************************************************/

struct video_canvas_s;
struct viewport_s;
struct geometry_s;
struct palette_s;

extern int video_init_cmdline_options(void);
extern int video_init(void);

extern int video_canvas_create(struct video_canvas_s *canvas,
                               unsigned int *width, unsigned int *height,
                               int mapped, const struct palette_s *palette);
extern void video_arch_canvas_init(struct video_canvas_s *canvas);
extern struct video_canvas_s *video_canvas_init(video_render_config_t
                                                *videoconfig);
extern void video_canvas_refresh(struct video_canvas_s *canvas,
                                 unsigned int xs, unsigned int ys,
                                 unsigned int xi, unsigned int yi,
                                 unsigned int w, unsigned int h);
extern int video_canvas_set_palette(struct video_canvas_s *canvas,
                                    const struct palette_s *palette);
extern void video_canvas_destroy(struct video_canvas_s *canvas);
extern void video_canvas_map(struct video_canvas_s *canvas);
extern void video_canvas_unmap(struct video_canvas_s *canvas);
extern void video_canvas_resize(struct video_canvas_s *canvas,
                                unsigned int width, unsigned int height);
extern void video_canvas_render(struct video_canvas_s *canvas, BYTE *trg,
                                int width, int height, int xs, int ys,
                                int xt, int yt, int pitcht, int depth);
extern void video_canvas_refresh_all(struct video_canvas_s *canvas);
extern void video_canvas_redraw_size(struct video_canvas_s *canvas,
                                     unsigned int width, unsigned int height);
extern void video_viewport_get(struct video_canvas_s *canvas,
                               struct viewport_s **viewport,
                               struct geometry_s **geometry);
extern void video_viewport_resize(struct video_canvas_s *canvas);

typedef struct video_draw_buffer_callback_s {
    int (*draw_buffer_alloc)(struct video_canvas_s *canvas, BYTE **draw_buffer,
                             unsigned int fb_width, unsigned int fb_height,
                             unsigned int *fb_pitch);
    void (*draw_buffer_free)(struct video_canvas_s *canvas, BYTE *draw_buffer);
    void (*draw_buffer_clear)(struct video_canvas_s *canvas, BYTE *draw_buffer,
                             BYTE value, unsigned int fb_width,
                             unsigned int fb_height, unsigned int fb_pitch);
} video_draw_buffer_callback_t;

/* These constants tell the video layer what */
/* resources should be registered */

#define VIDEO_RESOURCES_MONOCHROME 1 /* pet and cbm2 */
#define VIDEO_RESOURCES_PAL        2 /* c64, c128, vic20 */
#define VIDEO_RESOURCES_PAL_NOFAKE 3 /* plus4 (fake pal emu not possible here) */

struct cap_render_s {
    unsigned int sizex;
    unsigned int sizey;
    unsigned int rmode;
};
typedef struct cap_render_s cap_render_t;

struct cap_fullscreen_s {
    unsigned int device_num;
    char **device_name;
    int (*mode)(struct video_canvas_s *canvas, int mode);
};
typedef struct cap_fullscreen_s cap_fullscreen_t;

struct video_chip_cap_s {
    unsigned int dsize_allowed;
    unsigned int dscan_allowed;
    cap_render_t single_mode;
    cap_render_t double_mode;
    cap_fullscreen_t fullscreen;
};
typedef struct video_chip_cap_s video_chip_cap_t;

struct raster_s;

extern int video_resources_init(int mode);
extern int video_resources_chip_init(const char *chipname,
                                     struct raster_s *raster,
                                     video_chip_cap_t *video_chip_cap);
extern int video_cmdline_options_chip_init(const char *chipname);
extern int video_arch_init_resources(void);

extern void video_fullscreen_cap(cap_fullscreen_t *cap_fullscreen);

/* Video render interface */

/* Commodore VIC/VIC-II/TED related color/palette types */
typedef struct video_cbm_color_s {
    float luminance;        /* luminance                      */
    float angle;            /* angle on color wheel           */
    int direction;          /* +1 (pos), -1 (neg) or 0 (grey) */
    char *name;             /* name of this color             */
} video_cbm_color_t;

typedef struct video_cbm_palette_s {
    unsigned int num_entries;           /* number of colors in palette */
    video_cbm_color_t *entries;         /* array of colors             */
    float saturation; /* base saturation of all colors except the grey tones */
    float phase;      /* color phase (will be added to all color angles) */
} video_cbm_palette_t;

struct raster_s;

extern void video_color_set_palette(video_cbm_palette_t *palette);
extern int video_color_update_palette(void);
extern void video_color_set_raster(struct raster_s *raster);
extern int video_render_get_fake_pal_state(void);

#endif

