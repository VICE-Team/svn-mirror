/*
 * video.c - Video implementation for OS/2.
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

#define INCL_DOSSEMAPHORES
//#define INCL_WINSYS
//#define INCL_WINPALETTE
//#define INCL_WININPUT
//#define INCL_WINWINDOWMGR
//#define INCL_WINFRAMEMGR
//#define INCL_GPILOGCOLORTABLE
#include "vice.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __EMX__
#include <graph.h>
#endif

#include "video.h"

#include "ui.h"
#include "lib.h"
#include "resources.h"
#include "cmdline.h"
#include "log.h"
#include "kbd.h"
#include "translate.h"

static CHAR  szClientClass [] = "VICE/2 Gfx";
static CHAR  szTitleBarText[] = "VICE/2 1.0";
static ULONG flFrameFlags =
    FCF_TITLEBAR | FCF_SYSMENU | FCF_SHELLPOSITION | FCF_TASKLIST;

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

static int set_stretch_factor(int val, void *param)
{
    stretch = val;

    return 0;
}

static const resource_int_t resources_int[] = {
    { "WindowStretchFactor", 1, RES_EVENT_NO, NULL,
      &stretch, set_stretch_factor, NULL },
    { NULL }
};

int video_init_resources(void)
{
    return resources_register_int(resources_int);
}

static const cmdline_option_t cmdline_options[] = {
    { "-stretch", SET_RESOURCE, 1,
      NULL, NULL, "WindowStretchFactor", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<number>", "Specify stretch factor for PM Windows (1,2,3,...)" },
    { NULL },
};

int video_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------ */

int video_init(void) /* Initialization. Don't need this with OS/2 */
{
    return 0;
}

/* ------------------------------------------------------------------------ */
/* Frame buffer functions.  */
int frame_buffer_alloc(video_frame_buffer_t *f, UINT width, UINT height)
{
    //    log("frame_buffer_alloc",1);
    (*f) = (video_frame_buffer_t) malloc(sizeof(struct video_frame_buffer_s));
    //    log("bitmap alloc",1);
    (*f)->bitmap = (char*)malloc(width*height*sizeof(BYTE));
    (*f)->width  = width;
    (*f)->height = height;
    //    (*f)->count  = width*height;
    vidlog("FrameBufferAllocated",0);
    return 0;
}

void frame_buffer_clear(video_frame_buffer_t *f, BYTE value)
{
    memset((*f)->bitmap, value, ((*f)->width)*((*f)->height)*sizeof(BYTE));
}

void frame_buffer_free(video_frame_buffer_t *f)
{
    if (!f || !*f)       return;  // frame buffer not allocated
    //    log("bitmap free",1);
    lib_free((*f)->bitmap);
    //    log("frame_buffer_free",1);
    lib_free(*f);
    vidlog("FrameBufferFreed",0);
}

/* ------------------------------------------------------------------------ */
/* PM Window mainloop */
HMTX hmtx = NULLHANDLE;
APIRET rc;


void EXPENTRY wmPaint(HWND hwnd)
{
    video_canvas_t c=(video_canvas_t)WinQueryWindowPtr(hwnd,QWL_USER); // Ptr to usr resources
    HPS    hps=WinGetPS(hwnd);                   // Handle to Presentation Space

    // next time call cavas_refresh, refresh the whole canvas
    if  (c->init_ready) c->exposure_handler(c->width, c->height);
    else c->init_ready=TRUE;  // First call=canvas initialize ended

    WinBeginPaint(hwnd, hps, NULL); // Set Region in hps to blit;
    WinEndPaint  (hps); // must be done every time WM_PAINT was sent
}

typedef struct _kstate {
    BYTE numlock;
    BYTE scrllock;
    BYTE capslock;
} kstate;

static kstate vk_vice;
static kstate vk_desktop;

int PM_winActive;

void wmCreate(void)
{
    BYTE keyState[256];
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
    //    vk_desktop.numlock    = keyState[VK_NUMLOCK];
    vk_desktop.scrllock   = keyState[VK_SCRLLOCK];
    vk_desktop.capslock   = keyState[VK_CAPSLOCK];
    //    keyState[VK_NUMLOCK]  &= ~1;
    keyState[VK_SCRLLOCK] &= ~1;
    keyState[VK_CAPSLOCK] &= ~1;
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    PM_winActive=TRUE;
}

//extern int warp_mode_enabled;

void wmDestroy(void)
{  // it seems, that restoring key-state doesn't work here.
    BYTE keyState[256];
    PM_winActive=0;
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
    //    keyState[VK_NUMLOCK]  = vk_desktop.numlock;
    keyState[VK_SCRLLOCK] = vk_desktop.scrllock;
    keyState[VK_CAPSLOCK] = vk_desktop.capslock;
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
}

void wmSetSelection(MPARAM mp1)
{  // I also need to change the state, not only the leds.
    BYTE keyState[256];
    if (mp1) {  // Window is selected
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
        //        vk_desktop.numlock    = keyState[VK_NUMLOCK];
        vk_desktop.scrllock   = keyState[VK_SCRLLOCK];
        vk_desktop.capslock   = keyState[VK_CAPSLOCK];
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
        //        keyState[VK_NUMLOCK]  = vk_desktop.numlock;
        keyState[VK_SCRLLOCK] = vk_desktop.scrllock;
        keyState[VK_CAPSLOCK] = vk_desktop.capslock;
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    }

}

MRESULT EXPENTRY PM_winProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_CREATE:       wmCreate      ();          break;
    case WM_PAINT:        wmPaint       (hwnd);      break;
    case WM_CHAR:         wmChar        (hwnd, mp1); break;
    case WM_DESTROY:      wmDestroy     ();          break;
    case WM_SETSELECTION: wmSetSelection(mp1);       break;
    //    case WM_SETFOCUS: log("WM_SETFOCUS",mp1); break;
    //    case WM_ACTIVATE: log("WM_ACTIVATE",mp1); break;
    //    case WM_FOCUSCHANGE: log("WM_FOCUSCHANGE",mp1); break;

    }
    return WinDefWindowProc (hwnd, msg, mp1, mp2);
}


void PM_mainloop(VOID *arg)
{
    SIZEL sizelHps = {0,0};
    HAB   hab;  // Anchor Block to PM
    HMQ   hmq;  // Handle to Msg Queue
    HDC   hdc;  // Handle to Device (Window-Screen)
    QMSG  qmsg; // Msg Queue Event
    video_canvas_t *ptr=(video_canvas_t*)arg;

    hab = WinInitialize(0);            // Initialize PM
    hmq = WinCreateMsgQueue(hab, 0);   // Create Msg Queue

    // 2048 Byte Memory (Used eg for the Anchor Blocks
    WinRegisterClass(hab, szClientClass, PM_winProc, CS_SIZEREDRAW, 2048);

    (*ptr)->hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                   WS_ANIMATE, &flFrameFlags,
                                   szClientClass, szTitleBarText, 0L, 0, 0,
                                   &((*ptr)->hwndClient));

    WinSetWindowPos((*ptr)->hwndFrame, HWND_TOP, 0, 0,
                    (*ptr)->width*stretch,
                    (*ptr)->height*stretch+
                    WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR), // +1 with gcc?
                    SWP_SIZE|SWP_SHOW|SWP_ZORDER|SWP_ACTIVATE);    // Make visible, resize, top window
    WinSetWindowPtr((*ptr)->hwndClient, QWL_USER, (VOID*)(*ptr));
    // --------------------
    // maybe ---> WM_CREATE
    // --------------------
    hdc         = WinOpenWindowDC((*ptr)->hwndFrame);
    (*ptr)->hps = GpiCreatePS(WinQueryAnchorBlock((*ptr)->hwndFrame),
                              hdc, &sizelHps,
                              PU_PELS|GPIF_DEFAULT|GPIT_MICRO|GPIA_ASSOC); // GPIT_NORMAL does also work with vac++

    (*ptr)->pbmi = lib_calloc(1, 16+sizeof(RGB2)*256);
    (*ptr)->pbmi->cbFix      = 16; // Size of cbFix, cPlanes, cBitCount = Begin of RGB2
    (*ptr)->pbmi->cPlanes    =  1;
    (*ptr)->pbmi->cBitCount  =  8; // Using 8-bit color mode
    (*ptr)->palette=(RGB2*)((ULONG)(*ptr)->pbmi+(*ptr)->pbmi->cbFix);

    vidlog("pbmiAllocated",0);
    (*ptr)->pbmi_initialized = TRUE;    // All stuff for pbmi created
    //    DosReleaseMutexSem(hmtx); // gfx init end
    //-----------------------
    
    while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg (hab, &qmsg) ;

    //    (*ptr)->pbmi_initialized = FALSE;
    DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
    GpiDestroyPS((*ptr)->hps);
    WinDestroyWindow ((*ptr)->hwndFrame); // why was this commented out? --> WM_CLOSE ??
    WinDestroyMsgQueue(hmq);  // Destroy Msg Queue
    WinTerminate (hab);       // Release Anchor to PM
    lib_free((*ptr)->pbmi);   // is this the right moment to do this???
    //    lib_free(*ptr); // Vice crashes... why??? This must be done in the main thread!
    exit(0);                  // Kill VICE, All went OK
}


/* ------------------------------------------------------------------------ */
/* Canvas functions.  */
/* Create a `video_canvas_t' with tile `win_name', of widht `*width' x `*height'
   pixels, exposure handler callback `exposure_handler' and palette
   `palette'.  If specified width/height is not possible, return an
   alternative in `*width' and `*height'; return the pixel values for the
   requested palette in `pixel_return[]'.  */

video_canvas_t video_canvas_create(const char *title, UINT *width,
                             UINT *height, int mapped,
                             canvas_redraw_t exposure_handler,
                             const palette_t *palette,
                             struct video_frame_buffer_s *fb)
{
    video_canvas_t canvas_new;

    DEBUG(("Creating canvas width=%d height=%d", *width, *height));

    if (palette->num_entries > 255) {
       log_error(video_log, "Too many colors requested.");
       return (video_canvas_t) NULL;
    }

    vidlog("canvas alloc",1);
    canvas_new = lib_calloc(1,sizeof(struct _canvas));
    if (!canvas_new) return (video_canvas_t) NULL;

    canvas_new->init_ready       =  FALSE;  // canvas_new not yet initialized
    canvas_new->width            = *width;
    canvas_new->height           = *height;
    canvas_new->pbmi_initialized =  FALSE;  // pbmi not yet initialized

    rc=DosCreateMutexSem("\\SEM32\\GFX", &hmtx, 0, FALSE); // gfx init begin
    vidlog("Create: ",rc);
    _beginthread(PM_mainloop,NULL,0x4000,&canvas_new);

    while (!canvas_new->pbmi_initialized) DosSleep(1);
    video_canvas_set_palette(canvas_new, palette);

    canvas_new->exposure_handler = exposure_handler;
    return canvas_new;
}

//void video_canvas_destroy(video_canvas_t c)
//{   /* Destroy `s'.  */
//    if (!c) return;
//}

void video_canvas_map(video_canvas_t c)
{   /* Make `s' visible.  */
}

void video_canvas_unmap(video_canvas_t c)
{   /* Make `s' unvisible.  */
}

void video_canvas_resize(video_canvas_t c, UINT width, UINT height)
{
}

/* Set the palette of `c' to `p', and return the pixel values in
   `pixel_return[].  */
int video_canvas_set_palette(video_canvas_t c, const palette_t *p)
{
    int i;
    if (p == NULL) {
        return 0; /* no palette, nothing to do */
    }
    //    if (!(c->pbmi_initialized)) return;
    for (i=0; i<p->num_entries; i++) {
        c->palette[i].bRed  =p->entries[i].red;
        c->palette[i].bGreen=p->entries[i].green;
        c->palette[i].bBlue =p->entries[i].blue;
#if 0
        pixel_return[i]=i;
#endif
    }
    return 0;
}

/* ------------------------------------------------------------------------ */
void video_canvas_refresh(video_canvas_t c, BYTE *draw_buffer,
                          unsigned int draw_buffer_line_size,
                          video_frame_buffer_t f,
                          int xs, int ys, int xi, int yi, int w, int h)
{
    int H=c->height-yi; // Change coordinate system (up<->down)
    POINTL pointl[]={
        {stretch* xi,      stretch* H-1 }, // lt,dn Target Area // (0,+1) gcc?
        {stretch*(xi+w)-1, stretch*(H-h)}, // rt,up             // (0,+1) gcc?
        {xs,  ys  },                         // lt,up Source Area
        {xs+w,ys+h}                          // rt-dn
        };

    //    if (!(c->pbmi_initialized)) return;

    c->pbmi->cx=f->width;                    // GpiDrawBits must know
    c->pbmi->cy=f->height;                   // size of bitmap-array
    DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
    GpiDrawBits(c->hps, f->bitmap, c->pbmi, 4, pointl, ROP_SRCCOPY, BBO_IGNORE); //BBO_AND);
    DosReleaseMutexSem(hmtx);
};

