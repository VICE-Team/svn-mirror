#ifndef VICE_VIDEOARCH_H
#define VICE_VIDEOARCH_H

#define INCL_DOSSEMAPHORES

#include <os2.h>
#include <dive.h>

#include "video.h"
#include "palette.h"

#define CANVAS_USES_TRIPLE_BUFFERING(c) 0

struct video_draw_buffer_callback_s;

typedef struct video_frame_buffer_s {
    int width;
    int height;
    BYTE *bitmap;
} video_frame_buffer_t;

#define VIDEO_FRAME_BUFFER_LINE_START(f,n) ((f->bitmap) + ((f->width) * sizeof(BYTE)) * n)
#define VIDEO_FRAME_BUFFER_SIZE(f) (f->width)
#define VIDEO_FRAME_BUFFER_START(f) (f->bitmap)

typedef void (*canvas_redraw_t)(UINT width, UINT height);

struct palette_s;

typedef struct video_canvas_s {
    unsigned int initialized;
    unsigned int created;
    struct video_render_config_s *videoconfig;
    struct draw_buffer_s *draw_buffer;
    struct viewport_s *viewport;
    struct geometry_s *geometry;
    struct palette_s *palette;
    HWND hwndFrame;        // Handle to Frame of Window = WinQueryWindow(hwndClient, QW_PARENT)
    HWND hwndClient;       // Handle to Paint Area of Window
    HWND hwndMenu;         // Handle to menu bar
    HWND hwndTitlebar;     // Hwndle to titlebar
    HWND hwndPopupMenu;    // Handle to popup menu
    HMTX hmtx;
    char *title;
    UINT width;            // width of canvas graphic area
    UINT height;           // width of canvas graphic area
    BYTE stretch;          // actual stretch factor of canvas
    BOOL vrenabled;        // only BlitImage when Visible Region Enabled
    BYTE *bitmaptrg;
    ULONG ulBuffer; // DIVE buffer number
    BYTE bDepth;
    HDIVE hDiveInst;
    BYTE *pVram;
    SETUP_BLITTER divesetup;
    struct video_draw_buffer_callback_s *video_draw_buffer_callback;
};

typedef struct video_canvas_s video_canvas_t;

extern void video_show_modes(HWND hwnd);

#endif
