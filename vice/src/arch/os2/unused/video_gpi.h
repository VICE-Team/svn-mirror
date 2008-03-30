#ifndef _VIDEO_H
#define _VIDEO_H

#include "vice.h"

#include "palette.h"

#define CANVAS_USES_TRIPLE_BUFFERING(c) 0

typedef struct video_frame_buffer_s {
    int width;
    int height;
    BYTE *bitmap;
} *video_frame_buffer_t;

#define VIDEO_FRAME_BUFFER_POINTER_FIXUP(x) ((*x))
#define VIDEO_FRAME_BUFFER_SIZE(f)          (f->width)
#define VIDEO_FRAME_BUFFER_LINE_START(f,n)  (((f->bitmap)+(f->width)*n*sizeof(BYTE)))
#define VIDEO_FRAME_BUFFER_START(f)         (f->bitmap)

typedef void (*canvas_redraw_t)(UINT width, UINT height);

typedef struct _canvas {
    HWND  hwndFrame;        // Handle to Frame of Window
    HWND  hwndClient;       // Handle to Paint Area of Window
    HPS   hps;              // Handle to Area of screen which could be paint
    UINT  width;            // width of canvas graphic area
    UINT  height;           // width of canvas graphic area
    RGB2 *palette;          // pointer to structure which stores paletteinfo
    BOOL  init_ready;       // dont't use exposure_handler to 'frueh'
    BOOL  pbmi_initialized; // don't use pbmi before initialized
    PBITMAPINFO2 pbmi;       // information structure about bitmap format
    canvas_redraw_t exposure_handler;
} *video_canvas_t;

/* ------------------------------------------------------------------------ */

extern int video_init_resources(void);
extern int video_init_cmdline_options(void);
extern int video_init(void);

extern int  frame_buffer_alloc(video_frame_buffer_t *i, UINT width,
                               UINT height);
extern void frame_buffer_free(video_frame_buffer_t *i);
extern void frame_buffer_clear(video_frame_buffer_t *i, BYTE value);

extern video_canvas_t video_canvas_create(const char *win_name, UINT *width, UINT *height,
                              int mapped, canvas_redraw_t exposure_handler,
                              const palette_t *palette,
                              struct video_frame_buffer_s *fb);
extern void video_canvas_map(video_canvas_t s);
extern void video_canvas_unmap(video_canvas_t s);
extern void video_canvas_resize(video_canvas_t s, UINT width, UINT height);
extern int  video_canvas_set_palette(video_canvas_t c, const palette_t *p);

extern void video_canvas_refresh(video_canvas_t c, BYTE *draw_buffer,
                                 unsigned int draw_buffer_line_size,
                                 video_frame_buffer_t f,
                                 int xs, int ys, int xi, int yi, int w, int h);

void wmCreate();
void wmDestroy();


#endif
