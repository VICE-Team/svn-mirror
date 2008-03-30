#ifndef _VIDEOARCH_H
#define _VIDEOARCH_H

/* BYTE is already defined in os2def.h    */
/* ADDRESS is already defined in os2def.h */
/* #define ADDRESS 0                      */
#undef BYTE
#undef ADDRESS
#define INCL_GPIBITMAPS
#include <os2.h>
#undef ADDRESS
#define ADDRESS WORD

#include "palette.h"

#define CANVAS_USES_TRIPLE_BUFFERING(c) 0

typedef struct video_frame_buffer_s {
    int   width;
    int   height;
    BYTE *bitmap;
    ULONG ulBuffer; // DIVE buffer number
} video_frame_buffer_t;

/* This is necessary because DIVE calculates the optimum line size itself
   it seems to be x*sizeof(ULONG), see DiveAllocImageBuffer */
extern const int FBMULT;

#define VIDEO_FRAME_BUFFER_LINE_START(f,n)  ((f->bitmap) + ((f->width) \
                                            * sizeof(BYTE) + FBMULT) * n)
#define VIDEO_FRAME_BUFFER_SIZE(f)          (f->width)
#define VIDEO_FRAME_BUFFER_START(f)         (f->bitmap)

typedef void (*canvas_redraw_t)(UINT width, UINT height);

typedef struct canvas_s {
    //    HPS   hps;
    //    BOOL  init_ready;       // dont't use exposure_handler to early
    HWND  hwndFrame;        // Handle to Frame of Window
    HWND  hwndClient;       // Handle to Paint Area of Window
    char *title;
    UINT  width;            // width of canvas graphic area
    UINT  height;           // width of canvas graphic area
    RGB2 *palette;          // pointer to structure which stores colorinfo
    BOOL  vrenabled;        // only BlitImage when Visible Region Enabled
    canvas_redraw_t exposure_handler;
} canvas_t;

#endif

