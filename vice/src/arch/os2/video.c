/*
 * video.c - Video implementation for Vice/2, using DIVE.
 *
 * Written by
 *  Thomas Bretz (tbretz@gsi.de)
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

#define INCL_GPI
#define INCL_DOSSEMAPHORES
#include "vice.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __EMX__
#include <graph.h>
#endif

#include "video.h"

#include "ui.h"
#include "ui_status.h"
#include "utils.h"
#include "resources.h"
#include "cmdline.h"
#include "log.h"
#include "kbd.h"

#include <dive.h>
#ifdef __IBMC__
#include <fourcc.h>
#endif

static CHAR  szClientClass [] = "VICE/2 Gfx";
static CHAR  szTitleBarText[] = "VICE/2 1.0";
static ULONG flFrameFlags =
    FCF_TITLEBAR | FCF_SYSMENU | FCF_SHELLPOSITION | FCF_TASKLIST;

// TODO: put this stuff somewhere else!
ULONG  ulBuffer; // DIVE buffer number
BYTE  *fbData;
HMTX   hmtx;

/* ------------------------------------------------------------------------ */

/*#define DEBUG_VIDEO*/

/* Debugging stuff.  */
#ifdef DEBUG_VIDEO
static void video_debug(const char *format, ...)
{
        char tmp[1024];
        va_list args;

        va_start(args, format);
        vsprintf(tmp, format, args);
        va_end(args);
        log_debug(tmp);
}
#define DEBUG(x) video_debug x
#else
#define DEBUG(x)
#endif

static log_t video_log = LOG_ERR;

// ----------------------------
// My Log function
void vidlog(char *s, int i) {
    FILE *fl;
    fl=fopen("output","a");
    fprintf(fl,"%s 0x%x\n",s,i);
    fclose(fl);
}

/* ------------------------------------------------------------------------ */
/* Video-related resources.  */
static int stretch;  // Strech factor for window (1,2,3,...)

static int set_stretch_factor(resource_value_t v)
{
    stretch=(int)v;
    return 0;
}

static resource_t resources[] = {
    { "WindowStretchFactor", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &stretch, set_stretch_factor},
    { NULL }
};

int video_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-stretch", SET_RESOURCE, 1, NULL, NULL, "WindowStretchFactor", NULL,
      "<number>", "Specify stretch factor for PM Windows (1,2,3,...)" },
    { NULL },
};

int video_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------ */
static HDIVE hDiveInst = 0;  // DIVE instance

static void video_close(void)
{   // close Dive
    DiveClose(hDiveInst);
}
SETUP_BLITTER divesetup;
RECTL         rectl[50];

int video_init(void) /* Initialization. Don't need this with OS/2 */
{   // initialize Dive
    if (DiveOpen(&hDiveInst, FALSE, NULL)) vidlog("Could not open DIVE", 0);
    atexit(video_close);

    divesetup.ulStructLen       = sizeof(SETUP_BLITTER);
    divesetup.fInvert           = FALSE;
    divesetup.ulDitherType      = 0;
    divesetup.fccSrcColorFormat = FOURCC_LUT8;
    divesetup.fccDstColorFormat = FOURCC_SCRN;
    divesetup.pVisDstRects      = rectl;
    //    vidlog("DiveSetup",divesetup.pVisDstRects);
    DiveSetupBlitter(hDiveInst, &divesetup);
    return 0;
}

int testw, testh;
/* ------------------------------------------------------------------------ */
/* Frame buffer functions.  */
int frame_buffer_alloc(frame_buffer_t *f, UINT width, UINT height)
{
    (*f) = (frame_buffer_t) malloc(sizeof(struct _frame_buffer));
    (*f)->bitmap = (char*)malloc(width*height*sizeof(BYTE));
    (*f)->width  = width;  testw=width;
    (*f)->height = height; testh=height;

    fbData = (*f)->bitmap; //TODO: get rid of this!

    vidlog("FrameBufferAllocated",0);
    return 0;
}

void frame_buffer_clear(frame_buffer_t *f, BYTE value)
{
    memset((*f)->bitmap, value, ((*f)->width)*((*f)->height)*sizeof(BYTE));
}

void frame_buffer_free(frame_buffer_t *f)
{
    if (!f || !*f) return;  // frame buffer not allocated
    free((*f)->bitmap);
    free(*f);
    vidlog("FrameBufferFreed",0);
}

/* ------------------------------------------------------------------------ */
/* PM Window mainloop */

typedef struct _kstate {
    BYTE numlock;
    BYTE scrllock;
    BYTE capslock;
} kstate;

static kstate vk_vice;
static kstate vk_desktop;

//int PM_winActive;

void wmCreate()
{
    BYTE keyState[256];
    DosCreateMutexSem("\\SEM32\\ViceKey", &hmtxKey, 0, TRUE); // gfx init begin    _beginthread(PM_mainloop,NULL,0x4000,&canvas_new);
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
    //    vidlog("#numlock", keyState[VK_NUMLOCK]);
    //    vidlog("#capslock", keyState[VK_CAPSLOCK]);
    //    vidlog("#scrllock", keyState[VK_SCRLLOCK]);
    //    vk_desktop.numlock    = keyState[VK_NUMLOCK];
    vk_desktop.scrllock   = keyState[VK_SCRLLOCK];
    vk_desktop.capslock   = keyState[VK_CAPSLOCK];
    //    keyState[VK_NUMLOCK]  &= ~1;
    keyState[VK_SCRLLOCK] &= ~1;
    keyState[VK_CAPSLOCK] &= ~1;
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    PM_winActive=TRUE;
    DosReleaseMutexSem(hmtxKey);
}

//extern int warp_mode_enabled;

void wmDestroy(void)
{  // it seems, that restoring key-state doesn't work here.
    BYTE keyState[256];
    DosRequestMutexSem(hmtxKey, SEM_INDEFINITE_WAIT);
    PM_winActive=0;
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
    //    keyState[VK_NUMLOCK]  = vk_desktop.numlock;
    keyState[VK_SCRLLOCK] = vk_desktop.scrllock;
    keyState[VK_CAPSLOCK] = vk_desktop.capslock;
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    DosReleaseMutexSem(hmtxKey);
}

void wmSetSelection(MPARAM mp1)
{  // I also need to change the state, not only the leds.
    BYTE keyState[256];
    DosRequestMutexSem(hmtxKey, SEM_INDEFINITE_WAIT);
    if (mp1) {  // Window is selected
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
        //        vk_desktop.numlock    = keyState[VK_NUMLOCK];
        vk_desktop.scrllock   = keyState[VK_SCRLLOCK];
        vk_desktop.capslock   = keyState[VK_CAPSLOCK];
        //    vidlog("+numlock", keyState[VK_NUMLOCK]);
        //    vidlog("+capslock", keyState[VK_CAPSLOCK]);
        //    vidlog("+scrllock", keyState[VK_SCRLLOCK]);
        //        keyState[VK_NUMLOCK]  = vk_vice.numlock;
        keyState[VK_SCRLLOCK] = vk_vice.scrllock;
        keyState[VK_CAPSLOCK] &= ~1; //vk_vice.capslock;
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
        PM_winActive=1;
    }
    else {     // Window is deselected
        PM_winActive=0;
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
        //        vk_vice.numlock       = keyState[VK_NUMLOCK];
        vk_vice.scrllock      = keyState[VK_SCRLLOCK];
        //        vk_vice.capslock      = keyState[VK_CAPSLOCK];
        //    vidlog("-numlock", keyState[VK_NUMLOCK]);
        //    vidlog("-capslock", keyState[VK_CAPSLOCK]);
        //    vidlog("-scrllock", keyState[VK_SCRLLOCK]);
        //        keyState[VK_NUMLOCK]  = vk_desktop.numlock;
        keyState[VK_SCRLLOCK] = vk_desktop.scrllock;
        keyState[VK_CAPSLOCK] = vk_desktop.capslock;
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    }
    DosReleaseMutexSem(hmtxKey);
}

void wmVrn(HWND hwnd, int exposure)
{
    HPS      hps;
    HRGN     hrgn;
    RGNRECT  rgnCtl;
    POINTL   pointl;
    SWP      swp;
    canvas_t c = (canvas_t)WinQueryWindowPtr(hwnd,QWL_USER); // Ptr to usr resources
    //    DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
    hps  = WinGetPS(hwnd);
    hrgn = GpiCreateRegion(hps, 0L, NULL);
    if (hrgn) {
        WinQueryVisibleRegion(hwnd, hrgn);
        rgnCtl.ircStart    = 1;
        rgnCtl.crc         = 50;
        rgnCtl.ulDirection = RECTDIR_LFRT_TOPBOT;
        if (GpiQueryRegionRects(hps, hrgn, NULL, &rgnCtl, rectl)) {
            WinQueryWindowPos(hwnd, &swp);
            pointl.x = swp.x;
            pointl.y = swp.y;
            WinMapWindowPoints(WinQueryWindow(hwnd, QW_PARENT), HWND_DESKTOP, (POINTL*)&pointl, 1);
            divesetup.lScreenPosX   = pointl.x;
            divesetup.lScreenPosY   = pointl.y;
            divesetup.ulNumDstRects = rgnCtl.crcReturned;
            DiveSetupBlitter(hDiveInst, &divesetup);
            //            c->vrenabled=1;
            if (exposure) c->exposure_handler(c->width, c->height); // update whole window next time!
        }
        GpiDestroyRegion(hps, hrgn);
    }
    WinReleasePS(hps);
    //    DosReleaseMutexSem(hmtx);
}

void wmVrnEnabled(HWND hwnd)
{
    canvas_t c = (canvas_t)WinQueryWindowPtr(hwnd,QWL_USER); // Ptr to usr resources
    DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
    wmVrn(hwnd, 1);
    c->vrenabled=TRUE;
    DosReleaseMutexSem(hmtx);
}

void wmVrnDisabled(HWND hwnd)
{
    canvas_t c = (canvas_t)WinQueryWindowPtr(hwnd,QWL_USER); // Ptr to usr resources
    DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
    DiveSetupBlitter(hDiveInst, NULL);
    c->vrenabled=FALSE;
    DosReleaseMutexSem(hmtx);
}

MRESULT EXPENTRY PM_winProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    canvas_t c = (canvas_t)WinQueryWindowPtr(hwnd,QWL_USER); // Ptr to usr resources
    switch (msg)
    {
        //    case WM_CREATE:       wmCreate      ();          break;
        //    case WM_PAINT:        wmPaint       (hwnd);      break;
    case WM_CHAR:         wmChar        (hwnd, mp1); break;
    case WM_DESTROY:      wmDestroy     ();          break;
    case WM_SETSELECTION: wmSetSelection(mp1);       break;
    // Simulate VRNDISABLED by writing in no region
    case WM_VRNDISABLED:  wmVrnDisabled (hwnd);      break; //(hwnd, 0); c->vrenabled=0; break;
    //    case WM_VRNDISABLED: wmVrnDisabled  (hwnd);      break;
    case WM_VRNENABLED:   wmVrnEnabled  (hwnd);      break;
    //    case WM_SETFOCUS: vidlog("WM_SETFOCUS",mp1); break;
    //    case WM_ACTIVATE: vidlog("WM_ACTIVATE",mp1); break;
    //    case WM_FOCUSCHANGE: vidlog("WM_FOCUSCHANGE",mp1); break;
    }
    return WinDefWindowProc (hwnd, msg, mp1, mp2);
}

void PM_mainloop(VOID *arg)
{
    HAB   hab;  // Anchor Block to PM
    HMQ   hmq;  // Handle to Msg Queue
    QMSG  qmsg; // Msg Queue Event
    canvas_t *ptr=(canvas_t*)arg;

    hab = WinInitialize(0);            // Initialize PM
    hmq = WinCreateMsgQueue(hab, 0);   // Create Msg Queue

    // 2048 Byte Memory (Used eg for the Anchor Blocks
    WinRegisterClass(hab, szClientClass, PM_winProc, CS_SIZEREDRAW, 2048);

    (*ptr)->hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                   WS_VISIBLE, &flFrameFlags,
                                   szClientClass, szTitleBarText, 0L, 0, 0,
                                   &((*ptr)->hwndClient));

    WinSetWindowPos((*ptr)->hwndFrame, HWND_TOP, 0, 0,
                    (*ptr)->width*stretch,
                    (*ptr)->height*stretch+
                    WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR), // +1 with gcc?
                    SWP_SIZE|SWP_SHOW|SWP_ZORDER|SWP_ACTIVATE);    // Make visible, resize, top window
    WinSetWindowPtr((*ptr)->hwndClient, QWL_USER, (VOID*)(*ptr));

    (*ptr)->palette = xcalloc(1, sizeof (RGB2) * 256);

    // allocate DIVE image buffer
    if (DiveAllocImageBuffer(hDiveInst, &ulBuffer, FOURCC_LUT8,
                             testw,     testh,     0,           fbData))
        vidlog("Could not allocate DIVE buffer", 0);
    WinSetVisibleRegionNotify((*ptr)->hwndClient, TRUE);
    vidlog("pbmiAllocated",0);
    (*ptr)->vrenabled        = TRUE;

    while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg (hab, &qmsg) ;

    WinDestroyWindow ((*ptr)->hwndFrame);
    WinDestroyMsgQueue(hmq);  // Destroy Msg Queue
    WinTerminate (hab);       // Release Anchor to PM
    //    free((*ptr)->palette);       // cannot be destroyed because main thread is already working!!
    //    free(*ptr);
    DiveFreeImageBuffer (hDiveInst, ulBuffer);

    exit(0);                  // Kill VICE, All went OK
}


/* ------------------------------------------------------------------------ */
/* Canvas functions.  */
/* Create a `canvas_t' with tile `win_name', of widht `*width' x `*height'
   pixels, exposure handler callback `exposure_handler' and palette
   `palette'.  If specified width/height is not possible, return an
   alternative in `*width' and `*height'; return the pixel values for the
   requested palette in `pixel_return[]'.  */
canvas_t canvas_create(const char *title, UINT *width,
                       UINT *height, int mapped,
                       canvas_redraw_t exposure_handler,
                       const palette_t *palette, PIXEL *pixel_return)
{
    canvas_t canvas_new;

    DEBUG(("Creating canvas width=%d height=%d", *width, *height));

    if (palette->num_entries > 255) {
       log_error(video_log, "Too many colors requested.");
       return (canvas_t) NULL;
    }

    vidlog("canvas alloc",1);
    canvas_new = (canvas_t)xcalloc(1,sizeof(struct _canvas));
    if (!canvas_new) return (canvas_t) NULL;

    canvas_new->init_ready       =  FALSE;  // canvas_new not yet initialized
    canvas_new->width            = *width;
    canvas_new->height           = *height;
    canvas_new->exposure_handler =  exposure_handler;
    canvas_new->vrenabled        =  FALSE;  // pbmi not yet initialized

    DosCreateMutexSem("\\SEM32\\ViceGfx", &hmtx, 0, FALSE); // gfx init begin    _beginthread(PM_mainloop,NULL,0x4000,&canvas_new);

    _beginthread(PM_mainloop,NULL,0x4000,&canvas_new);

    while (!canvas_new->vrenabled) DosSleep(1);
    canvas_set_palette(canvas_new, palette, pixel_return);

    return canvas_new;
}

//void canvas_destroy(canvas_t c)
//{   /* Destroy `s'.  */
//    if (!c) return;
//}

void canvas_map(canvas_t c)
{   /* Make `s' visible.  */
}

void canvas_unmap(canvas_t c)
{   /* Make `s' unvisible.  */
}

void canvas_resize(canvas_t c, UINT width, UINT height)
{
}

/* Set the palette of `c' to `p', and return the pixel values in
   `pixel_return[].  */
int canvas_set_palette(canvas_t c, const palette_t *p, PIXEL *pixel_return)
{
    int i;
    //    if (!(c->pbmi_initialized)) return;
    for (i=0; i<p->num_entries; i++) {
        c->palette[i].bRed  =p->entries[i].red;
        c->palette[i].bGreen=p->entries[i].green;
        c->palette[i].bBlue =p->entries[i].blue;
        pixel_return[i]=i;
    }
    DiveSetSourcePalette(hDiveInst, 0, 256, (BYTE*)c->palette);
    return 0;
}

/* ------------------------------------------------------------------------ */
void canvas_refresh(canvas_t c, frame_buffer_t f,
                    int xs, int ys, int xi, int yi, int w, int h)
{
    if (!(c->vrenabled)) return;
    DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
    divesetup.ulSrcWidth  = w;
    divesetup.ulSrcHeight = h;
    divesetup.ulSrcPosX   = xs;
    divesetup.ulSrcPosY   = ys;
    divesetup.ulDstWidth  = w *stretch;
    divesetup.ulDstHeight = h *stretch;
    divesetup.lDstPosX    = xi*stretch;
    divesetup.lDstPosY    = (c->height-yi-h)*stretch;
    wmVrn(c->hwndClient, 0);

    //  divesetup.ulStructLen       = sizeof(SETUP_BLITTER)-sizeof(PRECTL)-sizeof(ULONG)-2*sizeof(LONG);
    //  DiveSetupBlitter(hDiveInst, &divesetup);
    //  divesetup.ulStructLen       = sizeof(SETUP_BLITTER);

    // draw the image
    DiveBlitImage(hDiveInst, ulBuffer, DIVE_BUFFER_SCREEN);
    DosReleaseMutexSem(hmtx);
};

