/*
 * videoarch.h - RISC OS graphics routines.
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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

#ifndef _VIDEOARCH_H
#define _VIDEOARCH_H


#include "vice.h"

#include "types.h"
#include "ui.h"
#include "video.h"

#include "wlsprite.h"


struct video_draw_buffer_callback_s;

typedef struct video_redraw_desc_s {
  graph_env ge;
  int xs, ys;
  int w, h;
  int *block;
} video_redraw_desc_t;

struct video_frame_buffer_s {
  unsigned int width, height, depth, pitch;
  int paldirty, transdirty;
  BYTE *framedata;
  BYTE *paldata;
  sprite_plotenv_t normplot;
  sprite_plotenv_t palplot;
  unsigned int *bplot_trans;
};

typedef struct video_frame_buffer_s video_frame_buffer_t;

struct video_canvas_s;
struct palette_s;

/* currently active redraw function */
typedef void video_redraw_core_func(struct video_canvas_s *canvas, struct video_redraw_desc_s *vrd);

struct video_canvas_s {
  unsigned int initialized;
  unsigned int created;
  char *name;
  unsigned int width, height;
  unsigned int scale;
  int shiftx, shifty;
  RO_Window *window;
  video_frame_buffer_t fb;
  unsigned int num_colours;
  unsigned int *current_palette;
  int last_video_render_depth;
  video_redraw_core_func *redraw_wimp;
  video_redraw_core_func *redraw_full;
  struct video_render_config_s *videoconfig;
  struct draw_buffer_s *draw_buffer;
  struct viewport_s *viewport;
  struct geometry_s *geometry;
  struct video_draw_buffer_callback_s *video_draw_buffer_callback;
  struct palette_s *palette;
};

typedef struct video_canvas_s video_canvas_t;

typedef struct canvas_list_t {
  video_canvas_t *canvas;
  struct canvas_list_t *next;
} canvas_list_t;


#define CANVAS_USES_TRIPLE_BUFFERING(c) 0

#define PAL_EMU_DEPTH_NONE	0
#define PAL_EMU_DEPTH_AUTO	1
#define PAL_EMU_DEPTH_8		2
#define PAL_EMU_DEPTH_16	3
#define PAL_EMU_DEPTH_32	4


extern void canvas_mode_change(void);
extern video_canvas_t *canvas_for_handle(int handle);
extern unsigned int canvas_number_for_handle(int handle);
extern void canvas_next_active(int moveCaret);
extern int canvas_get_number(void);

extern void video_canvas_redraw_core(video_canvas_t *canvas, video_redraw_desc_t *vrd);
extern void video_canvas_update_extent(video_canvas_t *canvas);
extern void video_canvas_update_size(video_canvas_t *canvas);

extern int video_full_screen_on(int *sprites);
extern int video_full_screen_off(void);
extern int video_full_screen_refresh(void);
extern void video_full_screen_speed(int percent, int framerate, int warp);
extern void video_full_screen_drive_leds(unsigned int drive);
extern void video_full_screen_init_status(void);
extern void video_full_screen_plot_status(void);
extern void video_full_screen_display_image(unsigned int num, const char *img);

extern void video_register_callbacks(void);
extern void video_pos_screen_to_canvas(video_canvas_t *canvas, int *block, int x, int y, int *cx, int *cy);

extern canvas_list_t *CanvasList;
extern video_canvas_t *ActiveCanvas;

extern int FullScreenMode;
extern int FullScreenStatLine;

#endif
