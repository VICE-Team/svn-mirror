#ifndef _VIDEOARCH_H
#define _VIDEOARCH_H

#include "vice.h"

#include "palette.h"

#define CANVAS_USES_TRIPLE_BUFFERING(c) 0

typedef struct _frame_buffer {
    int   width;
    int   height;
    BYTE *bitmap;
    ULONG ulBuffer; // DIVE buffer number
} *frame_buffer_t;

#define FRAME_BUFFER_SIZE(f)         (f->width)
#define FRAME_BUFFER_LINE_START(f,n) (((f->bitmap)+(f->width)*n*sizeof(BYTE)))
#define FRAME_BUFFER_START(f)        (f->bitmap)

typedef void (*canvas_redraw_t)(UINT width, UINT height);

typedef struct _canvas {
    //    HPS   hps;
    //    BOOL  init_ready;       // dont't use exposure_handler to early
    HWND  hwndFrame;        // Handle to Frame of Window
    HWND  hwndClient;       // Handle to Paint Area of Window
    UINT  width;            // width of canvas graphic area
    UINT  height;           // width of canvas graphic area
    RGB2 *palette;          // pointer to structure which stores colorinfo
    BOOL  vrenabled;        // only BlitImage when Visible Region Enabled
    canvas_redraw_t exposure_handler;
} *canvas_t;

#endif

