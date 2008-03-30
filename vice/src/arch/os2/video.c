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
#include "vice.h"

#define INCL_GPI
#define INCL_WINSYS         // SV_CYTITLEBAR
#define INCL_WININPUT       // WM_CHAR
//#define INCL_DOSPROFILE     // DosTmrQueryTime
#define INCL_DOSPROCESS     // DosSleep
#define INCL_WINWINDOWMGR   // QWL_USER
#define INCL_DOSSEMAPHORES  // HMTX
#include <os2.h>
#define INCL_MMIO
#include <os2me.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __EMX__
#include <graph.h>
#endif

#include "video.h"

#include "log.h"
#include "proc.h"
#include "ui.h"
#ifdef __X128__
#include "vdc.h"      // vdc_free()
#endif
#include "ui_status.h"
#include "utils.h"
#include "machine.h"
#include "cmdline.h"
#include "resources.h"

#include <dive.h>
#ifdef __IBMC__
#include <fourcc.h>
#endif

#ifdef HAVE_MOUSE
#include "mouse.h"
#endif

#include "version.h"

extern void archdep_create_mutex_sem(HMTX *hmtx, const char *pszName, int fState);

static HMTX  hmtx;
static CHAR  szClientClass [] = "VICE/2 Grafic Area";
static CHAR  szTitleBarText[] = "VICE/2 " VERSION;
static ULONG flFrameFlags =
    FCF_TITLEBAR | FCF_SHELLPOSITION | FCF_SYSMENU | FCF_TASKLIST;

/* ------------------------------------------------------------------------ */
/* Xvic workaround  */

#ifdef __XVIC__
const int FBMULT = 3;
#else
const int FBMULT = 0;
#endif

/* ------------------------------------------------------------------------ */
/* Video-related resources.  */

int stretch;            // Strech factor for window (1,2,3,...)
static int border;      // PM Border Type
static int posx, posy;  // Position of window at startup
static int autopos;     //

static int set_stretch_factor(resource_value_t v)
{
    stretch=(int)v;
    return 0;
}

static int set_posx(resource_value_t v)
{
    if (!autopos)
        posx=(int)v;
    return 0;
}

static int set_posy(resource_value_t v)
{
    if (!autopos)
        posy=(int)v;
    return 0;
}

static int set_autopos(resource_value_t v)
{
    autopos=(int)v;
    if (autopos)
        posx=posy=~0;
    return 0;
}

static int set_border_type(resource_value_t v)
{
    switch ((int)v)
    {
    case 1:
        flFrameFlags |= FCF_BORDER;
        border = 1;
        break;
    case 2:
        flFrameFlags |= FCF_DLGBORDER;
        border = 2;
        break;
    default:
        border = 0;
    }
    return 0;
}

static resource_t resources[] = {
    { "WindowStretchFactor", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &stretch, set_stretch_factor},
    { "PMBorderType", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &border, set_border_type},
    { "WindowPosX", RES_INTEGER, (resource_value_t) ~0,
      (resource_value_t *) &posx, set_posx},
    { "WindowPosY", RES_INTEGER, (resource_value_t) ~0,
      (resource_value_t *) &posy, set_posy},
    { "AutoWindowPos", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &autopos, set_autopos},
    { NULL }
};

int video_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-stretch", SET_RESOURCE, 1, NULL, NULL, "WindowStretchFactor", NULL,
      "<number>", "Specify stretch factor for PM Windows (1,2,3,...)" },
    { "-border",  SET_RESOURCE, 1, NULL, NULL, "PMBorderType", NULL,
      "<number>", "Specify window border type (1=small, 2=dialog, else=no border)" },
    { "-windowpos", SET_RESOURCE, 0, NULL, NULL,
      "AutoWindowPos", (resource_value_t) 1, NULL,
      "Use window position which is stored in the configuration file (default)" },
    { "+windowpos", SET_RESOURCE, 0, NULL, NULL,
      "AutoWindowPos", (resource_value_t) 0, NULL,
      "Ignore window position which is stored in the configuration file" },

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

    video_free();
#ifdef __X128__
    vdc_free();
#endif

    if (rc=DiveClose(hDiveInst))
        log_message(LOG_DEFAULT, "video.c: Dive closed (rc=0x%x).", rc);

    free(divesetup.pVisDstRects);
}

int video_init(void) // initialize Dive
{
    APIRET rc;

    if (rc=DiveOpen(&hDiveInst, FALSE, NULL))
    {
        log_message(LOG_DEFAULT,"video.c: Could not open DIVE (rc=0x%x).",rc);
        return -1;
    }

    // make sure, that the dive instance is closed at shutdown
    atexit(video_close);

    // FIXME??? Do we need one sem for every canvas?
    archdep_create_mutex_sem(&hmtx, "Video", FALSE);

    divesetup.ulStructLen       = sizeof(SETUP_BLITTER);
    divesetup.fInvert           = FALSE;
    divesetup.ulDitherType      = 0;
    divesetup.fccSrcColorFormat = FOURCC_LUT8;
    divesetup.fccDstColorFormat = FOURCC_SCRN;
    divesetup.pVisDstRects      = xcalloc(1, DIVE_RECTLS*sizeof(RECTL));

    return 0;
}

/* ------------------------------------------------------------------------ */
/* Frame buffer functions.  */
int video_frame_buffer_alloc(video_frame_buffer_t **f, UINT width, UINT height)
{
    APIRET rc;

    (*f) = (video_frame_buffer_t*) xmalloc(sizeof(struct video_frame_buffer_s));
    (*f)->bitmap = (char*) xmalloc(width*height*sizeof(BYTE));
    (*f)->width  = width;
    (*f)->height = height;
    log_message(LOG_DEFAULT,"video.c: Frame buffer allocated (%lix%li)", width, height);

    if (rc=DiveAllocImageBuffer(hDiveInst, &((*f)->ulBuffer), FOURCC_LUT8,
                                width, height, 0, (*f)->bitmap))
        log_message(LOG_DEFAULT,"video.c: Error DiveAllocImageBuffer (rc=0x%x).",rc);

    return 0;
}

void video_frame_buffer_clear(video_frame_buffer_t *f, PIXEL value)
{   // raster_force_repaint, we needn't this
    //    memset((*f)->bitmap, value, ((*f)->width)*((*f)->height)*sizeof(BYTE));
}

void video_frame_buffer_free(video_frame_buffer_t *f)
{
    ULONG rc;

    // This is if video_close calls video_free before a frame_buffer is allocated
    if (!f)
        return;

    if (rc=DiveFreeImageBuffer (hDiveInst, f->ulBuffer))
        log_message(LOG_DEFAULT,"video.c: Error DiveFreeImageBuffer (rc=0x%x).", rc);

    // this must be set to zero before the next image buffer could be allocated
    f->ulBuffer = 0;

    // if f exist also f->bitmap must exist... see above
    free(f->bitmap);
    free(f);
}

/* ------------------------------------------------------------------------ */
/* PM Window mainloop */

int PM_winActive;

typedef struct _kstate
{
    //    BYTE numlock;
    BYTE scrllock;
    BYTE capslock;
} kstate;

static kstate vk_vice;
static kstate vk_desktop;

extern int use_leds;
extern HMTX hmtxKey;

void wmCreate(void)
{
    BYTE keyState[256];
    log_debug("KEYSEM rc=%li",DosCreateMutexSem("\\SEM32\\ViceKey", &hmtxKey, 0, TRUE));
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
    vk_desktop.scrllock   = keyState[VK_SCRLLOCK];
    vk_desktop.capslock   = keyState[VK_CAPSLOCK];
    if (use_leds)
    {
        keyState[VK_SCRLLOCK] = 0;   // switch off
        keyState[VK_CAPSLOCK] = 0;   // switch off
    }
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    PM_winActive = TRUE;
    DosReleaseMutexSem(hmtxKey);
}

void wmDestroy(void)
{
    BYTE keyState[256];

    DosRequestMutexSem(hmtxKey, SEM_INDEFINITE_WAIT);
    PM_winActive = FALSE;
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
    keyState[VK_SCRLLOCK] = vk_desktop.scrllock;
    keyState[VK_CAPSLOCK] = vk_desktop.capslock;
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    DosReleaseMutexSem(hmtxKey);
}

void wmSetSelection(MPARAM mp1)
{
    BYTE keyState[256];
    DosRequestMutexSem(hmtxKey, SEM_INDEFINITE_WAIT);
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
    if (mp1)
    {
        if (!PM_winActive)
        {
            vk_desktop.scrllock   = keyState[VK_SCRLLOCK];
            vk_desktop.capslock   = keyState[VK_CAPSLOCK];
            if (use_leds)
            {
                keyState[VK_SCRLLOCK] = vk_vice.scrllock;  // warp led
                keyState[VK_CAPSLOCK] = 0;                 // drive led off
            }
            WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
            PM_winActive=TRUE;
        }
    }
    else
    {
        if (PM_winActive)
        {
            vk_vice.scrllock      = keyState[VK_SCRLLOCK];
            keyState[VK_SCRLLOCK] = vk_desktop.scrllock;
            keyState[VK_CAPSLOCK] = vk_desktop.capslock;
            WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
            PM_winActive=FALSE;
        }
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
    canvas_t *c = (canvas_t *)WinQueryWindowPtr(hwnd,QWL_USER); // Ptr to usr resources
    DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
    wmVrn(hwnd);
    c->vrenabled=TRUE;
    DosReleaseMutexSem(hmtx);
    c->exposure_handler(c->width, c->height); // update whole window next time!
}

void wmVrnDisabled(HWND hwnd)
{
    canvas_t *c = (canvas_t *)WinQueryWindowPtr(hwnd,QWL_USER); // Ptr to usr resources
    DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
    DiveSetupBlitter(hDiveInst, NULL);
    c->vrenabled=FALSE;
    DosReleaseMutexSem(hmtx);
}

//extern void kbd_proc(HWND hwnd, MPARAM mp1);

MRESULT EXPENTRY PM_winProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_CREATE:       wmCreate();           break;
    // After 'make visible': VRNDISABLED, PAINT, VRNENABLED
    //    case WM_PAINT: wmPaint(hwnd); break;
    case WM_CHAR:         kbd_proc(hwnd, mp1, mp2);  break;
    case WM_CLOSE:
    case WM_DESTROY:      wmDestroy();          break;
    case WM_SETSELECTION: wmSetSelection(mp1);  break;
    case WM_VRNDISABLED:  wmVrnDisabled(hwnd);  break;
    case WM_VRNENABLED:   wmVrnEnabled(hwnd);   break;
    case WM_MOVE:
        {
            SWP swp;
            WinQueryWindowPos(WinQueryWindow(hwnd, QW_PARENT), &swp);
            posx=swp.x;
            posy=swp.y;
        }
        break;
#ifdef HAVE_MOUSE
    case WM_MOUSEMOVE:
    case WM_BUTTON1DOWN:
    case WM_BUTTON1UP:
    case WM_BUTTON2DOWN:
    case WM_BUTTON2UP:
        mouse_button(hwnd, msg, mp1);
        break;
#endif
    //    case WM_SETFOCUS: vidlog("WM_SETFOCUS",mp1); break;
    //    case WM_ACTIVATE: vidlog("WM_ACTIVATE",mp1); break;
    //    case WM_FOCUSCHANGE: vidlog("WM_FOCUSCHANGE",mp1); break;
    }
    return WinDefWindowProc (hwnd, msg, mp1, mp2);
}

UINT canvas_fullheight(UINT height)
{
    height *= stretch;
    height += WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
    if (border==1) height += 2*WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER);
    if (border==2) height += 2*WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME);
    return height;
}

UINT canvas_fullwidth(UINT width)
{
    width *= stretch;
#if defined __XVIC__
    width *= 2;
#endif
    if (border==1) width += 2*WinQuerySysValue(HWND_DESKTOP, SV_CXBORDER);
    if (border==2) width += 2*WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME);
    return width;
}

extern int trigger_shutdown;

void PM_mainloop(VOID *arg)
{
    APIRET rc;
    HAB   hab;  // Anchor Block to PM
    HMQ   hmq;  // Handle to Msg Queue
    QMSG  qmsg; // Msg Queue Event
    canvas_t *c=(canvas_t *)arg;

    //    archdep_setup_signals(0); // signals are not shared between threads!

    hab = WinInitialize(0);            // Initialize PM
    hmq = WinCreateMsgQueue(hab, 0);   // Create Msg Queue

    // 2048 Byte Memory (Used eg for the Anchor Blocks)
    WinRegisterClass(hab, szClientClass, PM_winProc,
                     CS_MOVENOTIFY|CS_SIZEREDRAW, 2048);

    c->hwndFrame = WinCreateStdWindow(HWND_DESKTOP, WS_VISIBLE,
                                      &flFrameFlags, szClientClass,
                                      c->title, 0L, 0, 0,
                                      &(c->hwndClient));

    WinSetWindowPos(c->hwndFrame, HWND_TOP, posx, posy,
                    canvas_fullwidth (c->width),
                    canvas_fullheight(c->height),
                    SWP_SIZE|SWP_SHOW|SWP_ZORDER|SWP_ACTIVATE|
                    (posx==-1&&posy==-1?0:SWP_MOVE));    // Make visible, resize, top window
    WinSetWindowPtr(c->hwndClient, QWL_USER, (VOID*)c);

    WinSetVisibleRegionNotify(c->hwndClient, TRUE);
    c->vrenabled = TRUE;

    if (rc=DosSetPriority(PRTYS_THREAD, PRTYC_REGULAR, +1, 0))
        log_debug("video.c: Error DosSetPriority (rc=%li)", rc);

    // this makes reactions faster when main thread is hut down
    while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg (hab, &qmsg);

    log_debug("Window '%s': Quit!", c->title);
    free(c->title);

    /*
     ----------------------------------------------------------
     Is this right, that I couldn't call this for xpet & xcbm2?
     ----------------------------------------------------------
     if (WinDestroyWindow ((*ptr)->hwndFrame))
     log_message(LOG_DEFAULT,"video.c: Error! Graphic window destroy. (rc=%li)",rc);*/

    /*
     ----------------------------------------------------------
     Do I need another solution for x128?
     ---------------------------------------------------------- */
    // Make sure, that the emulation thread doesn't
    // try to blit the screen anymore
//#ifndef __X128__
    DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
//#endif

    if (!WinDestroyMsgQueue(hmq))
        log_message(LOG_DEFAULT,"video.c: Error! Destroying Msg Queue.");
    if (!WinTerminate (hab))
        log_message(LOG_DEFAULT,"video.c: Error! Releasing PM anchor.");

    trigger_shutdown = 1;

    DosSleep(5000); // wait 5 seconds
    log_debug("Brutal Exit!");
    exit(0);        // end VICE in all cases
}

/* ------------------------------------------------------------------------ */
/* Canvas functions.  */
/* Create a `canvas_t' with tile `win_name', of widht `*width' x `*height'
   pixels, exposure handler callback `exposure_handler' and palette
   `palette'.  If specified width/height is not possible, return an
   alternative in `*width' and `*height'; return the pixel values for the
   requested palette in `pixel_return[]'.  */
canvas_t *canvas_create(const char *title, UINT *width,
                        UINT *height, int mapped,
                        canvas_redraw_t exposure_handler,
                        const palette_t *palette, PIXEL *pixel_return)
{
    canvas_t *canvas_new;

    if (palette->num_entries > 255)
    {
        log_error(LOG_DEFAULT, "video.c: Too many colors requested.");
        return (canvas_t *) NULL;
    }

    canvas_new = (canvas_t *)xcalloc(1,sizeof(struct canvas_s));

    canvas_new->title            =  concat(szTitleBarText, " - ", title+6, NULL);
    canvas_new->width            = *width;
    canvas_new->height           = *height;
    canvas_new->exposure_handler =  exposure_handler;
    canvas_new->vrenabled        =  FALSE;  // pbmi not yet initialized

    _beginthread(PM_mainloop, NULL, 0x4000, canvas_new);

    while (!canvas_new->vrenabled) // wait until canvas initialized
        DosSleep(1);

    log_debug("video.c: Canvas '%s' (%ix%i) created.", title, *width, *height);

    canvas_set_palette(canvas_new, palette, pixel_return);

    return canvas_new;
}

void canvas_map(canvas_t *c)
{   /* Make `s' visible.  */
    // WinShowWindow(c->hwndFrame, 1);
}

void canvas_unmap(canvas_t *c)
{   /* Make `s' unvisible.  */
    // WinShowWindow(c->hwndFrame, 0);
}

void canvas_resize(canvas_t *c, UINT width, UINT height)
{
    if (c->width==width && c->height==height)
        return;

    if (!WinSetWindowPos(c->hwndFrame, 0, 0, 0,
                         canvas_fullwidth(width), canvas_fullheight(height),
                         SWP_SIZE))
    {
        log_debug("video.c: Error resizing canvas (%ix%i).", width, height);
        return;
    }
    log_debug("video.c: canvas resized (%ix%i --> %ix%i)", c->width, c->height, width, height);
    c->width  = width;
    c->height = height;
    c->exposure_handler(width, height); // update whole window next time!
}

/* Set the palette of `c' to `p', and return the pixel values in
   `pixel_return[].  */
int canvas_set_palette(canvas_t *c, const palette_t *p, PIXEL *pixel_return)
{
    int i;
    ULONG rc;

    RGB2 *palette = xcalloc(1, p->num_entries*sizeof(RGB2));

    for (i=0; i<p->num_entries; i++)
    {
        palette[i].bRed  =p->entries[i].red;
        palette[i].bGreen=p->entries[i].green;
        palette[i].bBlue =p->entries[i].blue;
        pixel_return[i]=i;
    }

    rc=DiveSetSourcePalette(hDiveInst, 0, p->num_entries, (BYTE*)palette);
    if (rc)
        log_message(LOG_DEFAULT,"video.c: Error DiveSetSourcePalette (rc=0x%x).",rc);

    free(palette);

    return 0;
}

/* ------------------------------------------------------------------------ */
void canvas_refresh(canvas_t *c, video_frame_buffer_t *f,
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
        divesetup.ulDstWidth  = w *stretch
#if defined __XVIC__
            *2
#endif
            ;
        divesetup.ulDstHeight = h *stretch;
        divesetup.lDstPosX    = xi*stretch
#if defined __XVIC__
            *2
#endif
            ;
        divesetup.lDstPosY    = (c->height-(yi+h))*stretch;
        wmVrn(c->hwndClient);            // setup draw areas

        DiveBlitImage(hDiveInst, f->ulBuffer, DIVE_BUFFER_SCREEN); // draw the image
    }
    DosReleaseMutexSem(hmtx);
};

