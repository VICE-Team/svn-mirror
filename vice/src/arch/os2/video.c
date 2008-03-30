/*
 * video.c - Video implementation for Vice/2, using DIVE.
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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

#include "cmdline.h"
#include "log.h"
#include "kbd.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "ui_status.h"
#include "utils.h"
#include "vicii.h"  // video_free()

#include <dive.h>
#ifdef __IBMC__
#include <fourcc.h>
#endif

#include "version.h"

static HMTX  hmtx;
static CHAR  szClientClass [] = "VICE/2 Grafic Area";
static CHAR  szTitleBarText[] = "VICE/2 " VERSION;
static ULONG flFrameFlags =
    FCF_TITLEBAR | FCF_SYSMENU | FCF_SHELLPOSITION | FCF_TASKLIST;

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
const int DIVE_RECTLS  = 50;
static HDIVE hDiveInst =  0;  // DIVE instance
static SETUP_BLITTER divesetup;

static void video_close(void)
{   // close Dive
    APIRET rc;
    //    if (ulBuffer) { // share this with frame_buffer_free
    //        DiveFreeImageBuffer (hDiveInst, ulBuffer);
    //        ulBuffer=0;
    //        log_message(LOG_DEFAULT, "video.c: Dive image buffer freed (video_close).");
    //    }
    free(divesetup.pVisDstRects);
    rc=DiveClose(hDiveInst);
    if (rc) log_message(LOG_DEFAULT, "video.c: Dive closed (rc=%li).",rc);
}

int video_init(void) // initialize Dive
{
    APIRET rc;
    if (rc=DiveOpen(&hDiveInst, FALSE, NULL)) {
        log_message(LOG_DEFAULT,"video.c: Could not open DIVE (rc=%li).",rc);
        return -1;
    }
    atexit(video_close);

    divesetup.ulStructLen       = sizeof(SETUP_BLITTER);
    divesetup.fInvert           = FALSE;
    divesetup.ulDitherType      = 0;
    divesetup.fccSrcColorFormat = FOURCC_LUT8;
    divesetup.fccDstColorFormat = FOURCC_SCRN;
    divesetup.pVisDstRects      = xcalloc(1, DIVE_RECTLS*sizeof(RECTL));
    DiveSetupBlitter(hDiveInst, &divesetup);
    return 0;
}

/* ------------------------------------------------------------------------ */
/* Frame buffer functions.  */
int video_frame_buffer_alloc(frame_buffer_t *f, UINT width, UINT height)
{
    static int first=TRUE;
    APIRET rc;
    if (first)  // be shure that image_buffer is freed (BEFORE video_close)
    {
        atexit(video_free);
        first=FALSE;
    }

    (*f) = (frame_buffer_t) xmalloc(sizeof(struct _frame_buffer));
    (*f)->bitmap = (char*) xmalloc(width*height*sizeof(BYTE));
    (*f)->width  = width;
    (*f)->height = height;
    log_message(LOG_DEFAULT,"video.c: Frame buffer allocated (%lix%li)",width,height);
    if (rc=DiveAllocImageBuffer(hDiveInst, &((*f)->ulBuffer), FOURCC_LUT8,
                                width, height, 0, (*f)->bitmap))
        log_message(LOG_DEFAULT,"video.c: Could not allocate Dive image buffer (rc=%li).",rc);
    //    log_message(LOG_DEFAULT,"video.c: Dive image buffer allocated.");

    return 0;
}

void video_frame_buffer_clear(frame_buffer_t *f, PIXEL value)
{   // raster_force_repaint, we needn't this
    //    memset((*f)->bitmap, value, ((*f)->width)*((*f)->height)*sizeof(BYTE));
    //    log_message(LOG_DEFAULT,"video.c: Frame buffer cleared");
}

void video_frame_buffer_free(frame_buffer_t *f)
{
    //    if (ulBuffer) {  // share this with video_close
    ULONG rc=DiveFreeImageBuffer (hDiveInst, (*f)->ulBuffer);
    (*f)->ulBuffer = 0;
    if (!rc) log_message(LOG_DEFAULT,"video.c: Dive image buffer freed (frame_buffer_free).");
    //    }
    if ((*f)->bitmap) free((*f)->bitmap);
    if (*f)           free(*f);
    //log_message(LOG_DEFAULT,"video.c: Frame buffer freed.");
}

/* ------------------------------------------------------------------------ */
/* PM Window mainloop */

typedef struct _kstate
{
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

void wmVrn(HWND hwnd)
{
    HPS   hps  = WinGetPS(hwnd);
    HRGN  hrgn = GpiCreateRegion(hps, 0L, NULL);
    if (hrgn) {  // this should be controlled again (clr/home, stretch 3)
        RGNRECT rgnCtl;
        WinQueryVisibleRegion(hwnd, hrgn);
        rgnCtl.ircStart    = 1;
        rgnCtl.crc         = DIVE_RECTLS;
        rgnCtl.ulDirection = RECTDIR_LFRT_BOTTOP;
        if (GpiQueryRegionRects(hps, hrgn, NULL, &rgnCtl, divesetup.pVisDstRects))
        {
            SWP     swp;
            POINTL  pointl;
            WinQueryWindowPos(hwnd, &swp);
            pointl.x = swp.x;
            pointl.y = swp.y;
            WinMapWindowPoints(WinQueryWindow(hwnd, QW_PARENT),
                               HWND_DESKTOP, (POINTL*)&pointl, 1);
            divesetup.lScreenPosX   = pointl.x;
            divesetup.lScreenPosY   = pointl.y;
            divesetup.ulNumDstRects = rgnCtl.crcReturned;
            DiveSetupBlitter(hDiveInst, &divesetup);
        }
        GpiDestroyRegion(hps, hrgn);
    }
    WinReleasePS(hps);
}

void wmVrnEnabled(HWND hwnd)
{
    canvas_t c = (canvas_t)WinQueryWindowPtr(hwnd,QWL_USER); // Ptr to usr resources
    DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
    wmVrn(hwnd);
    c->vrenabled=TRUE;
    DosReleaseMutexSem(hmtx);
    c->exposure_handler(c->width, c->height); // update whole window next time!
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
    //            canvas_t c = (canvas_t)WinQueryWindowPtr(hwnd,QWL_USER); // Ptr to usr resources
    switch (msg)
    {
        //    case WM_CREATE:       wmCreate      ();          break;

        // After 'make visible': VRNDISABLED, PAINT, VRNENABLED
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
//#include "sound.h"

void PM_mainloop(VOID *arg)
{
    APIRET rc;
    HAB   hab;  // Anchor Block to PM
    HMQ   hmq;  // Handle to Msg Queue
    QMSG  qmsg; // Msg Queue Event
    canvas_t *ptr=(canvas_t*)arg;

    //    archdep_setup_signals(0); // signals are not shared between threads!

    hab = WinInitialize(0);            // Initialize PM
    hmq = WinCreateMsgQueue(hab, 0);   // Create Msg Queue

    // 2048 Byte Memory (Used eg for the Anchor Blocks)
    WinRegisterClass(hab, szClientClass, PM_winProc, CS_SIZEREDRAW, 2048);

    (*ptr)->hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                   WS_VISIBLE, &flFrameFlags,
                                   szClientClass, szTitleBarText, 0L, 0, 0,
                                   &((*ptr)->hwndClient));

    WinSetWindowPos((*ptr)->hwndFrame, HWND_TOP, 0, 0,
                    (*ptr)->width *stretch,
                    (*ptr)->height*stretch+
                    WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR), // +1 with gcc?
                    SWP_SIZE|SWP_SHOW|SWP_ZORDER|SWP_ACTIVATE);    // Make visible, resize, top window
    WinSetWindowPtr((*ptr)->hwndClient, QWL_USER, (VOID*)(*ptr));

    (*ptr)->palette = xcalloc(1, 256*sizeof(RGB2));

    // DiveAllocImageBuffer(hDiveInst, &ulBuffer, FOURCC_LUT8, testw, testh, 0, fbData))

    WinSetVisibleRegionNotify((*ptr)->hwndClient, TRUE);
    (*ptr)->vrenabled = TRUE;

    while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg (hab, &qmsg) ;

    resources_set_value("Sound", (resource_value_t)FALSE);

    rc=WinDestroyWindow ((*ptr)->hwndFrame);
    if (rc) log_message(LOG_DEFAULT,"video.c: Graphic window destroyed (rc=%li)",rc);
    if (!WinDestroyMsgQueue(hmq)) log_message(LOG_DEFAULT,"video.c: Error! Msg Queue destroy.");
    if (!WinTerminate (hab)) log_message(LOG_DEFAULT,"video.c: Error! PM anchor release.");
    //    machine_shutdown();  // detach all disks
    //    sound_close();
    //      free((*ptr)->palette);       // cannot be destroyed because main thread is already working!!
    //      free(*ptr);
    exit(0); // Kill VICE, All went OK
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

    if (palette->num_entries > 255) {
        log_error(LOG_DEFAULT, "video.c: Too many colors requested.");
       return (canvas_t) NULL;
    }

    canvas_new = (canvas_t)xcalloc(1,sizeof(struct _canvas));
    if (!canvas_new) return (canvas_t) NULL;

    canvas_new->width            = *width;
    canvas_new->height           = *height;
    canvas_new->exposure_handler =  exposure_handler;
    canvas_new->vrenabled        =  FALSE;  // pbmi not yet initialized

    DosCreateMutexSem("\\SEM32\\Vice2\\Graphic", &hmtx, 0, FALSE); // gfx init begin    _beginthread(PM_mainloop,NULL,0x4000,&canvas_new);

    _beginthread(PM_mainloop,NULL,0x4000,&canvas_new);

    while (!canvas_new->vrenabled) DosSleep(1);
    canvas_set_palette(canvas_new, palette, pixel_return);

    return canvas_new;
}

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
    ULONG rc;
    //    if (!(c->pbmi_initialized)) return;
    for (i=0; i<p->num_entries; i++) {
        c->palette[i].bRed  =p->entries[i].red;
        c->palette[i].bGreen=p->entries[i].green;
        c->palette[i].bBlue =p->entries[i].blue;
        pixel_return[i]=i;
    }
    rc=DiveSetSourcePalette(hDiveInst, 0, 256, (BYTE*)c->palette);
    if (rc) log_message(LOG_DEFAULT,"video.c: Palette set (rc=%i).",rc);
    return 0;
}

/* ------------------------------------------------------------------------ */
void canvas_refresh(canvas_t c, frame_buffer_t f,
                    unsigned int xs, unsigned int ys,
                    unsigned int xi, unsigned int yi,
                    unsigned int w,  unsigned int h)
{
    if (DosRequestMutexSem(hmtx, SEM_IMMEDIATE_RETURN)) return;
    if (c->vrenabled)
    {
        divesetup.ulSrcWidth  = w;
        divesetup.ulSrcHeight = h;
        divesetup.ulSrcPosX   = xs;
        divesetup.ulSrcPosY   = ys;
        divesetup.ulDstWidth  = w *stretch;
        divesetup.ulDstHeight = h *stretch;
        divesetup.lDstPosX    = xi*stretch;
        divesetup.lDstPosY    = (c->height-(yi+h))*stretch;
        wmVrn(c->hwndClient);                               // setup draw areas
        DiveBlitImage(hDiveInst, f->ulBuffer, DIVE_BUFFER_SCREEN); // draw the image
    }
    DosReleaseMutexSem(hmtx);
};

