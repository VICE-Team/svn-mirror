#ifndef _VIDEOARCH_H
#define _VIDEOARCH_H

/* BYTE is already defined in os2def.h    */
/* ADDRESS is already defined in os2def.h */
/* #define ADDRESS 0                      */
#undef BYTE
#undef ADDRESS
#define INCL_GPIBITMAPS
#define INCL_DOSSEMAPHORES
#include <os2.h>
#include <dive.h>
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
   it seems to be x*sizeof(ULONG), see DiveAllocImageBuffer               */
extern const int FBMULT;

#define VIDEO_FRAME_BUFFER_LINE_START(f,n)  ((f->bitmap) + ((f->width) \
                                            * sizeof(BYTE) + FBMULT) * n)
#define VIDEO_FRAME_BUFFER_SIZE(f)          (f->width)
#define VIDEO_FRAME_BUFFER_START(f)         (f->bitmap)

typedef void (*canvas_redraw_t)(UINT width, UINT height);

typedef struct canvas_s
{
    HWND  hwndFrame;        // Handle to Frame of Window
    HWND  hwndClient;       // Handle to Paint Area of Window
    HMTX  hmtx;
    char *title;
    UINT  width;            // width of canvas graphic area
    UINT  height;           // width of canvas graphic area
    UCHAR palette;          // number of 16-block of palette entries
    BOOL  vrenabled;        // only BlitImage when Visible Region Enabled
    SETUP_BLITTER sb;
    canvas_redraw_t exposure_handler;
};

typedef struct canvas_s canvas_t;

#endif

