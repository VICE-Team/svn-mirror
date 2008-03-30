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
#define INCL_WINSYS          // SV_CYTITLEBAR
#define INCL_WININPUT        // WM_CHAR
#define INCL_DOSPROCESS      // DosSleep
#define INCL_WINFRAMEMGR     // WM_TRANSLATEACCEL
#define INCL_WINWINDOWMGR    // QWL_USER
#define INCL_DOSSEMAPHORES   // HMTX
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
#include "ui_status.h"
#include "utils.h"
#include "dialogs.h"
#include "machine.h"      // machine_canvas_screenshot
#include "cmdline.h"
#include "resources.h"
#include "screenshot.h"  // screenshot_t


#include <dive.h>
#ifdef __IBMC__
#include <fourcc.h>
#endif

#ifdef HAVE_MOUSE
#include "mouse.h"
#endif

#include "version.h"

#define DEBUG(txt)
//#define DEBUG(txt) log_debug(txt)

extern void archdep_create_mutex_sem(HMTX *hmtx, const char *pszName, int fState);

static HMTX  hmtx;
static CHAR  szClientClass [] = "VICE/2 Grafic Area";
static CHAR  szTitleBarText[] = "VICE/2 " VERSION;
static ULONG flFrameFlags =
    FCF_ICON | FCF_TITLEBAR | FCF_SHELLPOSITION | FCF_SYSMENU | FCF_TASKLIST;

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
static int menu;        // flag if menu should be enabled

static int set_stretch_factor(resource_value_t v, void *param)
{
    stretch=(int)v;
    return 0;

}

static int set_menu(resource_value_t v, void *param)
{
    menu=(int)v;
    return 0;
}

static int set_border_type(resource_value_t v, void *param)
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
      (resource_value_t *) &stretch, set_stretch_factor, NULL },
    { "PMBorderType", RES_INTEGER, (resource_value_t) 2,
      (resource_value_t *) &border, set_border_type, NULL },
    { "Menubar", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &menu, set_menu, NULL },
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
    { "-menu", SET_RESOURCE, 0, NULL, NULL, "Menubar", (resource_value_t) 1,
      NULL, "Enable Main Menu Bar" },
    { "+menu", SET_RESOURCE, 0, NULL, NULL, "Menubar", (resource_value_t) 0,
      NULL, "Disable Main Menu Bar" },
    { NULL }
};

int video_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------ */
const int DIVE_RECTLS  = 50;
static HDIVE hDiveInst =  0;  // DIVE instance
static int initialized = FALSE;
static SETUP_BLITTER divesetup;

extern HMTX hmtxKey;

int video_init(void) // initialize Dive
{
    APIRET rc;

    if (rc=DiveOpen(&hDiveInst, FALSE, NULL))
    {
        log_message(LOG_DEFAULT,"video.c: Could not open DIVE (rc=0x%x).",rc);
        return -1;
    }
    // FIXME??? Do we need one sem for every canvas
    // This semaphore serializes the Dive and Frame Buffer access
    //
    archdep_create_mutex_sem(&hmtx, "Video", FALSE);

    divesetup.ulStructLen       = sizeof(SETUP_BLITTER);
    divesetup.fInvert           = FALSE;
    divesetup.ulDitherType      = 0;
    divesetup.fccSrcColorFormat = FOURCC_LUT8;
    divesetup.fccDstColorFormat = FOURCC_SCRN;
    divesetup.pVisDstRects      = xcalloc(DIVE_RECTLS, sizeof(RECTL));

    //
    // this is a dummy setup. It is needed for some graphic
    // drivers that they get valid values for the first time we
    // setup the dive blitter regions
    //
    divesetup.ulSrcWidth  = 1;
    divesetup.ulSrcHeight = 1;
    divesetup.ulSrcPosX   = 0;
    divesetup.ulSrcPosY   = 0;
    divesetup.ulDstWidth  = 1;
    divesetup.ulDstHeight = 1;
    divesetup.lDstPosX    = 0;
    divesetup.lDstPosY    = 0;

    // FIXME
    // Is this the right place to initialize the keyboard semaphore?
    //
    rc=DosCreateMutexSem("\\SEM32\\Vice2\\Keyboard", &hmtxKey, 0, FALSE);
    if (rc==258 /*ERROR_DUPLICATE_NAME*/)
    {
        //
        // we are in a different process (eg. second instance of x64)
        //
        rc=DosOpenMutexSem("\\SEM32\\ViceKey", &hmtxKey);
        if (rc)
            log_debug("video.c: DosOpenMutexSem (rc=%li)", rc);
    }
    else
        if (rc)
            log_debug("video.c: DosCreateMutexSem (rc=%li)", rc);

    initialized = TRUE;

    return 0;
}

void video_close(void)
{
    //
    // video_close is called from the main thread, that means, that
    // vice is not blitting at the same moment.
    //
    APIRET rc;

    DEBUG("VIDEO CLOSE 0");

    if (rc=DiveClose(hDiveInst))
        log_message(LOG_DEFAULT, "video.c: Dive closed (rc=0x%x).", rc);
    else
        log_message(LOG_DEFAULT, "video.c: Dive closed.");
    hDiveInst = 0;

    free(divesetup.pVisDstRects);

    DEBUG("VIDEO CLOSE 1");
}

/* ------------------------------------------------------------------------ */
/* Frame buffer functions.  */
int video_frame_buffer_alloc(video_frame_buffer_t **f, UINT width, UINT height)
{
    APIRET rc;

    (*f) = (video_frame_buffer_t*) xcalloc(1, sizeof(video_frame_buffer_t));
    (*f)->bitmap = (char*) xcalloc(width*height, sizeof(BYTE));
    (*f)->width  = width;
    (*f)->height = height;

    if (rc=DiveAllocImageBuffer(hDiveInst, &((*f)->ulBuffer), FOURCC_LUT8,
                                width, height, 0, (*f)->bitmap))
        log_message(LOG_DEFAULT,"video.c: Error DiveAllocImageBuffer (rc=0x%x).", rc);
    else
        log_message(LOG_DEFAULT,"video.c: Frame buffer allocated (%lix%li)", width, height);

    return 0;
}

void video_frame_buffer_clear(video_frame_buffer_t *f, PIXEL value)
{   // raster_force_repaint, we needn't this
    //    memset((*f)->bitmap, value, ((*f)->width)*((*f)->height)*sizeof(BYTE));
}

void video_frame_buffer_free(video_frame_buffer_t *f)
{
    ULONG rc;
    //
    // This is if video_close calls video_free before a frame_buffer is allocated
    //
    if (!f)
        return;

    DEBUG("FRAME BUFFER FREE 0");

    if (rc=DiveFreeImageBuffer(hDiveInst, f->ulBuffer))
        log_message(LOG_DEFAULT,"video.c: Error DiveFreeImageBuffer (rc=0x%x).", rc);
    //
    // this must be set to zero before the next image buffer could be allocated
    // FIXME: Why? Could this be?
    f->ulBuffer = 0;
    //
    // if f is valid also f->bitmap must be valid, see video_frame_buffer_alloc
    //
    free(f->bitmap);
    free(f);

    DEBUG("FRAME BUFFER FREE 1");
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

void wmCreate(void)
{
    BYTE keyState[256];
    DosRequestMutexSem(hmtxKey, SEM_INDEFINITE_WAIT);
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
    //
    // store desktop LED status
    //
    vk_desktop.scrllock = keyState[VK_SCRLLOCK];
    vk_desktop.capslock = keyState[VK_CAPSLOCK];
    //
    // if allowed by user: switch LEDs off
    //
    if (use_leds)
    {
        keyState[VK_SCRLLOCK] = 0;   // switch off
        keyState[VK_CAPSLOCK] = 0;   // switch off
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    }
    PM_winActive = TRUE;
    DosReleaseMutexSem(hmtxKey);
}

void wmDestroy(void)
{
    BYTE keyState[256];
    DosRequestMutexSem(hmtxKey, SEM_INDEFINITE_WAIT);
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
    //
    // restore desktop LED status
    //
    keyState[VK_SCRLLOCK] = vk_desktop.scrllock;
    keyState[VK_CAPSLOCK] = vk_desktop.capslock;
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    PM_winActive = FALSE;
    DosReleaseMutexSem(hmtxKey);
}

void wmSetSelection(MPARAM mp1)
{
    BYTE keyState[256];
    DosRequestMutexSem(hmtxKey, SEM_INDEFINITE_WAIT);
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
    //
    // restore vice keyboard LED status (user switched to vice window)
    //
    if (mp1 && !PM_winActive)
    {
        //
        // store system LED status for later usage
        //
        vk_desktop.scrllock = keyState[VK_SCRLLOCK];
        vk_desktop.capslock = keyState[VK_CAPSLOCK];
        //
        // set new status for LEDs if allowed by user
        //
        if (use_leds)
        {
            keyState[VK_SCRLLOCK] = vk_vice.scrllock;  // restore warp led status
            keyState[VK_CAPSLOCK] = 0;                 // drive led off
            WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
        }
        PM_winActive=TRUE;
    }
    //
    // restore system keyboard LED status (user switched away from vice window)
    //
    if (!mp1 && PM_winActive)
    {
        //
        // store SCRLLOCK status for later use
        //
        vk_vice.scrllock = keyState[VK_SCRLLOCK];
        //
        // restore system LED status
        //
        keyState[VK_SCRLLOCK] = vk_desktop.scrllock;
        keyState[VK_CAPSLOCK] = vk_desktop.capslock;
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
        PM_winActive=FALSE;
    }
    DosReleaseMutexSem(hmtxKey);
}

void wmVrn(HWND hwnd)
{
    HPS  hps  = WinGetPS(hwnd);
    HRGN hrgn = GpiCreateRegion(hps, 0L, NULL);

    DEBUG("WM VRN 0");

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

    DEBUG("WM VRN 1");
}

void wmVrnEnabled(HWND hwnd)
{
    canvas_t *c = (canvas_t *)WinQueryWindowPtr(hwnd,QWL_USER);

    DEBUG("WM VRN ENABLED 0");

    DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
    if (!hDiveInst || !initialized)
    {
        DosReleaseMutexSem(hmtx);
        return;
    }
    wmVrn(hwnd);
    c->vrenabled=TRUE;
    DosReleaseMutexSem(hmtx);
    //
    // blit the whole visible area to the screen at next blit time
    //
    c->exposure_handler(c->width, c->height);

    DEBUG("WM VRN ENABLED 1");
}

void wmVrnDisabled(HWND hwnd)
{
    canvas_t *c = (canvas_t *)WinQueryWindowPtr(hwnd,QWL_USER);

    DEBUG("WM VRN DISABLED 0");

    DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
    if (!hDiveInst)
    {
        DosReleaseMutexSem(hmtx);
        return;
    }
    //
    // FIXME: This is system wide...
    //
    DiveSetupBlitter(hDiveInst, NULL);
    //
    // ...and this is only for one canvas.
    //
    c->vrenabled=FALSE;
    DosReleaseMutexSem(hmtx);

    DEBUG("WM VRN DISABLED 1");
}

void wmPaint(HWND hwnd)
{
    screenshot_t geom;

    //
    // get pointer to actual canvas from user data area
    //
    canvas_t *c = (canvas_t *)WinQueryWindowPtr(hwnd, QWL_USER);

    DEBUG("WM_PAINT 0");

    //
    // if the canvas isn't setup yet return
    //
    if (!c)
        return;

    DEBUG("WM_PAINT 1");

    //
    // get the frame_buffer and geometry from the machine
    //
    if (machine_canvas_screenshot(&geom, c) < 0)
        return;

    {
        const int x  = geom.x_offset;
        const int y  = geom.first_displayed_line;
        const int cx = geom.max_width & ~3;
        const int cy = geom.last_displayed_line - geom.first_displayed_line;

        DEBUG(("WM_PAINT 2: x=%d y=%d cx=%d cy=%d", x, y, cx, cy));

        //
        // enable the visible region - it is disabled
        //
        wmVrnEnabled(hwnd);

        //
        // blit to canvas (canvas_refresh should be thread safe by itself)
        //
        canvas_refresh(c, geom.frame_buffer, x, y, 0, 0, cx, cy);
    }
}

MRESULT EXPENTRY PM_winProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_CREATE:       wmCreate();                break;
    case WM_PAINT:        wmPaint(hwnd);             break;
    case WM_CHAR:         kbd_proc(hwnd, mp1, mp2);  break;
    case WM_CLOSE:
    case WM_DESTROY:      wmDestroy();               break;
    case WM_SETSELECTION: wmSetSelection(mp1);       break;
    case WM_VRNDISABLED:  wmVrnDisabled(hwnd);       break;
    case WM_VRNENABLED:   wmVrnEnabled(hwnd);        break;

    case WM_TRANSLATEACCEL:
        //
        // let only 'Alt'ed keys pass
        //
        if (SHORT1FROMMP(((QMSG*)mp1)->mp1)&KC_ALT)
            break;
        else
            return FALSE;

    case WM_COMMAND:
        menu_action(hwnd, SHORT1FROMMP(mp1), mp2);
        break;

    case WM_MENUSELECT:
        menu_select(HWNDFROMMP(mp2), SHORT1FROMMP(mp1));
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
        /*
         case WM_SETFOCUS: vidlog("WM_SETFOCUS",mp1); break;
         case WM_ACTIVATE: vidlog("WM_ACTIVATE",mp1); break;
         case WM_FOCUSCHANGE: vidlog("WM_FOCUSCHANGE",mp1); break;
         case WM_MOVE:
         WinQueryWindowPos(WinQueryWindow(hwnd, QW_PARENT), &pos);
         break;
         */
    }
    return WinDefWindowProc (hwnd, msg, mp1, mp2);
}

UINT canvas_fullheight(UINT height)
{
    height *= stretch;
    height += WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
    if (menu)      height += WinQuerySysValue(HWND_DESKTOP, SV_CYMENU)+1;  // FIXME: +1 ?
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

/*
static void InitHelp(HWND hwndFrame, int id, PSZ title, PSZ libname)
{
    HAB = WInQueryAnchorBlock(hwndFrame);

    HELPINIT hini;

    //
    // initialize help init structure
    //
    memset(&hini, 0, sizeof(hini));
    hini.cb                       = sizeof(HELPINIT);
    hini.phtHelpTable             = (PHELPTABLE)MAKELONG(id, 0xFFFF);
    hini.pszHelpWindowTitle       = (PSZ)title;
    hini.pszHelpLibraryName       = (PSZ)libname;
#ifdef DEBUG
    hini.fShowPanelId             = CMIC_SHOW_PANEL_ID;
#else
    hini.fShowPanelId             = CMIC_HIDE_PANEL_ID;
#endif

    //
    // creating help instance
    //
    hwndHelp = WinCreateHelpInstance(hab, &hini);

    if (hwndHelp == NULLHANDLE || hini.ulReturnCode)
        return; // error

    //
    // associate help instance with main frame
    //
    if (!WinAssociateHelpInstance(hwndHelp, hwndFrame))
        return; // error
}
*/

void PM_mainloop(VOID *arg)
{
    APIRET rc;
    SWP    swp;
    HAB    hab;  // Anchor Block to PM
    HMQ    hmq;  // Handle to Msg Queue
    QMSG   qmsg; // Msg Queue Event

    canvas_t *c=(canvas_t *)arg;

    // archdep_setup_signals(0); // signals are not shared between threads!

    hab = WinInitialize(0);            // Initialize PM
    hmq = WinCreateMsgQueue(hab, 0);   // Create Msg Queue

    //
    // 2048 Byte Memory (Used eg for the Anchor Blocks)
    //  CS_MOVENOTIFY, CS_SIZEREDRAW skipped
    //  CS_SYNCPAINT: send WM_PAINT messages
    //
    WinRegisterClass(hab, szClientClass, PM_winProc, CS_SYNCPAINT, 2048);

    //
    // display menu bar if requested
    //
    if (menu)
        flFrameFlags |= FCF_MENU;

    //
    // create window on desktop, FIXME: WS_ANIMATE looks sometimes strange
    //
    c->hwndFrame = WinCreateStdWindow(HWND_DESKTOP, WS_ANIMATE|WS_VISIBLE,
                                      &flFrameFlags, szClientClass,
                                      c->title, 0L, 0, menu?IDM_VICE2:0,
                                      &(c->hwndClient));

    // InitHelp(c->hwndFrame, IDM_VICE2, "Vice/2 Help", "vice2.hlp");

    //
    // bring window to top, set size and position, set focus
    // correct for window height
    //
    WinQueryWindowPos(c->hwndFrame, &swp);
    WinSetWindowPos(c->hwndFrame, HWND_TOP,
                    swp.x, swp.y+swp.cy-canvas_fullheight(c->height),
                    canvas_fullwidth (c->width),
                    canvas_fullheight(c->height),
                    SWP_SIZE|SWP_SHOW|SWP_ZORDER|SWP_ACTIVATE|SWP_MOVE);

    //
    // set user data pointer
    //
    WinSetWindowPtr(c->hwndClient, QWL_USER, (VOID*)c);

    //
    // set visible region notify on, enable visible region
    //
    WinSetVisibleRegionNotify(c->hwndClient, TRUE);
    WinSendMsg(c->hwndClient, WM_VRNENABLED, 0, 0); //c->vrenabled = TRUE;

    //
    // this makes reactions faster when shutdown of main thread is triggered
    //
    if (rc=DosSetPriority(PRTYS_THREAD, PRTYC_REGULAR, +1, 0))
        log_debug("video.c: Error DosSetPriority (rc=%li)", rc);

    //
    // MAINLOOP
    //
    while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg (hab, &qmsg);

    //
    // make sure, that the state of the VRN doesn't change anymore, don't blit anymore
    //

    // FIXME: The disabeling can be done in WM_CLOSE???
    WinSetVisibleRegionNotify(c->hwndClient, FALSE);
    initialized = FALSE;
    wmVrnDisabled(c->hwndClient);

    log_debug("Window '%s': Quit!", c->title);
    free(c->title);

    /*
     ----------------------------------------------------------
     Is this right, that I couldn't call this for xpet & xcbm2?
     ----------------------------------------------------------
     if (WinDestroyWindow ((*ptr)->hwndFrame))
     */

    //
    // FIXME: For x128 this must be triggered before the Msg Queue is destroyed
    // because sometimes WinDestroyMsgQueue seems to hang. For the second
    // window the msg queue is never destroyed. Is it really necessary
    // to destroy the msg queue?
    //
    log_debug("Triggering Shutdown of Emulation Thread.");
    trigger_shutdown = 1;

    log_debug("Destroying Msg Queue.");
    if (!WinDestroyMsgQueue(hmq))
        log_message(LOG_DEFAULT,"video.c: Error! Destroying Msg Queue.");
    log_debug("Releasing PM Anchor.");
    if (!WinTerminate (hab))
        log_message(LOG_DEFAULT,"video.c: Error! Releasing PM anchor.");

    DosSleep(5000); // wait 5 seconds

    //
    // FIXME, this happens for x128 eg at my laptop when video_close is called
    //
    log_debug("Brutal Exit!");
    //
    // if the triggered shutdown hangs make sure that the program ends
    //
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
                        UINT *height, int mapped, canvas_redraw_t exposure_handler,
                        const palette_t *palette, PIXEL *pixel_return)
{
    canvas_t *canvas_new;

    if (palette->num_entries > 0x10)
    {
        log_error(LOG_DEFAULT, "video.c: Error! More than 16 colors requested for '%s'", title);
        return (canvas_t *) NULL;
    }

    canvas_new = (canvas_t *)xcalloc(1, sizeof(canvas_t));

    canvas_new->title            =  concat(szTitleBarText, " - ", title+6, NULL);
    canvas_new->width            = *width;
    canvas_new->height           = *height;
    canvas_new->stretch          =  stretch;
    canvas_new->palette          =  0;
    canvas_new->exposure_handler =  exposure_handler;
    canvas_new->vrenabled        =  FALSE;

    if (vsid_mode)
        return canvas_new;

    _beginthread(PM_mainloop, NULL, 0x4000, canvas_new);

    while (!canvas_new->vrenabled) // wait until canvas initialized
        DosSleep(1);

    log_debug("video.c: Canvas '%s' (%ix%i) created.", title, *width, *height);

    canvas_set_palette(canvas_new, palette, pixel_return);

    return canvas_new;
}

void canvas_destroy(canvas_t *c)
{
	/* FIXME: Just a dummy so far */
}


void canvas_map(canvas_t *c)
{
    WinShowWindow(c->hwndFrame, TRUE);
}

void canvas_unmap(canvas_t *c)
{
    WinShowWindow(c->hwndFrame, FALSE);
}

void canvas_resize(canvas_t *c, UINT width, UINT height)
{
    //
    // if this function is called from outside the main thread
    // make sure that the visible region notify is turned off
    // before and back on after.
    //
    SWP swp;

    //
    // if nothing has changed do nothing
    //
    if (c->width==width && c->height==height && c->stretch==stretch)
        return;

    //
    // make anchor left, top corner
    //
    WinQueryWindowPos(c->hwndFrame, &swp);
    if (!WinSetWindowPos(c->hwndFrame, 0,
                         swp.x, swp.y+swp.cy-canvas_fullheight(height),
                         canvas_fullwidth (width),
                         canvas_fullheight(height),
                         SWP_SIZE|SWP_MOVE))
    {
        log_debug("video.c: Error resizing canvas (%ix%i).", width, height);
        return;
    }
    log_debug("video.c: canvas resized (%ix%i --> %ix%i)", c->width, c->height, width, height);

    c->width   = width;
    c->height  = height;
    c->stretch = stretch;
    c->exposure_handler(width, height); // update whole window next time!
}

/* Set the palette of `c' to `p', and return the pixel values in
   `pixel_return[].  */
int canvas_set_palette(canvas_t *c, const palette_t *p, PIXEL *pixel_return)
{
    //
    // FIXME, this sets the palette for both x128 windows...
    //
    int i;
    ULONG rc;
    int offset;

    //
    // number of already used blocks of color data
    //
    static int blocks = 0;

    RGB2 *palette = xcalloc(p->num_entries, sizeof(RGB2));

    //
    // if this is the first or a new block of color data
    //
    if (!blocks || !c->palette)
    {
        if (blocks==0x10)
        {
            log_message(LOG_DEFAULT, "video.c: All 256 palette entries are in use.");
            free(palette);
            return -1;
        }
        blocks++;
        c->palette = blocks;
    }

    //
    // calculate offset for actual block of color data
    //
    offset = (c->palette-1) * 0x10;

    for (i=0; i<p->num_entries; i++)
    {
        palette[i].bRed   = p->entries[i].red;
        palette[i].bGreen = p->entries[i].green;
        palette[i].bBlue  = p->entries[i].blue;

        pixel_return[i] = i + offset;
    }

    rc=DiveSetSourcePalette(hDiveInst, offset, p->num_entries, (BYTE*)palette);
    if (rc)
        log_message(LOG_DEFAULT,"video.c: Error DiveSetSourcePalette (rc=0x%x).",rc);
    else
        log_message(LOG_DEFAULT,"video.c: Palette realized.",rc);

    free(palette);

    return 0;
}

/* ------------------------------------------------------------------------ */
void canvas_refresh(canvas_t *c, video_frame_buffer_t *f,
                    unsigned int xs, unsigned int ys,
                    unsigned int xi, unsigned int yi,
                    unsigned int w,  unsigned int h)
{
    if (DosRequestMutexSem(hmtx, SEM_IMMEDIATE_RETURN) || !hDiveInst || !initialized)
        return;

    DEBUG("CANVAS REFRESH 0");

    if (c->vrenabled)
    {
        //
        // calculate source and destinations
        //
        divesetup.ulSrcWidth  = w;
        divesetup.ulSrcHeight = h;
        divesetup.ulSrcPosX   = xs;
        divesetup.ulSrcPosY   = ys;
        divesetup.ulDstWidth  = w *stretch
#if defined __XVIC__
            *2
#endif
            ;
        divesetup.ulDstHeight = h*stretch;
        divesetup.lDstPosX    = xi*stretch
#if defined __XVIC__
            *2
#endif
            ;
        divesetup.lDstPosY    = (c->height-(yi+h))*stretch;

        //
        // now setup the draw areas
        // (all other values are set already by WM_VRNENABLED)
        //
        // Because x128 has two canvases we have
        // to setup the absolute drawing area
        //
        // FIXME: maybe this should only be done when
        //        video cache is enabled
#ifdef __X128__
        wmVrn(c->hwndClient);
#else
        DiveSetupBlitter(hDiveInst, &divesetup);
#endif

        //
        // and blit the image to the screen
        //
        DiveBlitImage(hDiveInst, f->ulBuffer, DIVE_BUFFER_SCREEN);
    }
    DosReleaseMutexSem(hmtx);

    DEBUG("CANVAS REFRESH 1");
};

