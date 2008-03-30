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

#define INCL_GPI             // RGNRECT
#define INCL_WINSYS          // SV_CYTITLEBAR
#define INCL_WINHELP         // WinCreateHelpInstance
#define INCL_WININPUT        // WM_CHAR
#define INCL_WINMENUS        // PU_*
#define INCL_WINERRORS       // WInGetLastError
#define INCL_WINSTDDRAG      // PDRAGINFO in dragndrop.h
#define INCL_DOSPROCESS      // DosSleep
#define INCL_WINSTATICS      // SS_TEXT
#define INCL_WINFRAMEMGR     // WM_TRANSLATEACCEL
#define INCL_WINWINDOWMGR    // QWL_USER
#define INCL_DOSSEMAPHORES   // HMTX
#include <os2.h>

#define INCL_MMIO
#define INCL_MM_OS2          // DiveBlitImageLines
#include <os2me.h>

#ifdef __IBMC__
#include "fullscr.h"
#include <fourcc.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __EMX__
#include <graph.h>
#endif

#include "video.h"
#include "videoarch.h"

#include "lib.h"
#include "log.h"
#include "proc.h"
#include "util.h"
#include "vsync.h"
#include "dialogs.h"         // IDM_VICE2
#include "menubar.h"
#include "dragndrop.h"
#include "cmdline.h"
#include "resources.h"

#include "machine.h"         // machine_canvas_screenshot
#include "screenshot.h"      // screenshot_t

#ifdef HAVE_MOUSE
#include "mousedrv.h"
#endif

#include "version.h"

//#define DIRECT_ACCESS

#ifdef DIRECT_ACCESS
#define divecaps.fccColorEncoding
#define divecaps.ulDepth
//#else
//#define SRC_COLORFORMAT  FOURCC_LUT8
//#define SRC_COLORFORMAT  FOURCC_R565  // 16bit
//#define SRC_COLORFORMAT  FOURCC_RGB3  // 24bit
//#define SRC_COLORFORMAT  FOURCC_RGB4  // 32bit
#endif

//
//  VICE Speed Test... Warp Mode: Speed(%), Frames per Second (fps)
//
//                            Vice1.7   DIRECT    LUT8     R565
//  -------------------------------------------------------------
//   1x         Video Cached  1600,74  1570,72  1560,72  1550,71
//   1x                       1160,50   930,43  1160,50  1090,50
//   2xStretch  Video Cached  1500,70           1500,69  1500,69
//   2xStretch                 660,30            640,29   630,29
//   DScan/Size Video Cached           1520,70  1510,69  1490,68
//   DScan/Size                         660,31   610,28   570,27
//
//  Maybe I can make this switchable...
//

#define DEBUG(txt)
//#define DEBUG(txt) log_debug(txt)

static log_t vidlog = LOG_ERR;

extern void archdep_create_mutex_sem(HMTX *hmtx, const char *pszName, int fState);

//static HMTX  hmtx;
static CHAR  szClientClass [] = "VICE/2 Grafic Area";
static CHAR  szTitleBarText[] = "VICE/2 " VERSION;
static ULONG flFrameFlags =
    FCF_ICON | FCF_MENU | FCF_TITLEBAR | FCF_ACCELTABLE |
    FCF_SHELLPOSITION | FCF_SYSMENU | FCF_TASKLIST |
    FCF_AUTOICON | FCF_MINBUTTON | FCF_CLOSEBUTTON;

/* ------------------------------------------------------------------------ */
/* Video-related resources.  */

int stretch;            // Strech factor for window (1,2,3,...)
static int border;      // PM Border Type
static int menu;        // flag if menu should be enabled
static int status=0;    // flag if status should be enabled
static HWND *hwndlist = NULL;

extern void WmVrnDisabled(HWND hwnd);
extern void WmVrnEnabled(HWND hwnd);

static inline video_canvas_t *GetCanvas(HWND hwnd)
{
    return (video_canvas_t *)WinQueryWindowPtr(hwnd, QWL_USER);
}

static void AddToWindowList(HWND hwnd)
{
    int i=0;

    HWND *newlist;

    if (!hwndlist)
    {
        //
        // If no list exists yet, creat an empty one
        //
        hwndlist = malloc(sizeof(HWND));
        hwndlist[0] = NULLHANDLE;
    }

    //
    // Count valid entries in list
    //
    while (hwndlist[i])
        i++;

    //
    // create new list
    //
    newlist = malloc(sizeof(HWND)*(i+2));

    //
    // copy old list to new list an delete old list
    //
    memcpy(newlist, hwndlist, sizeof(HWND)*i);
    lib_free (hwndlist);

    hwndlist = newlist;

    //
    // add new entry to list
    //
    hwndlist[i]   = hwnd;
    hwndlist[i+1] = NULLHANDLE;
}

static int set_stretch_factor(resource_value_t v, void *param)
{
    int i=0;

    if (!hwndlist || stretch==(int)v)
    {
        stretch=(int)v;
        return 0;
    }

    //
    // set new stretch factor
    //
    stretch=(int)v;

    i = 0;
    while (hwndlist[i])
    {
        video_canvas_t *c = GetCanvas(hwndlist[i]);

        if (!DosRequestMutexSem(c->hmtx, SEM_INDEFINITE_WAIT))
        {
            //
            // Disable drawing into window -> already in video_canvas_resize
            //
            // WmVrnDisabled(hwndlist[i]);

            //
            // resize canvas
            //
            const int dsx = c->videoconfig->doublesizex + 1;
            const int dsy = c->videoconfig->doublesizey + 1;

            video_canvas_resize(c, c->width/dsx, c->height/dsy);

            DosReleaseMutexSem(c->hmtx);
        }

        //
        // set visible region (start blitting again)
        // (this is done by canvas_resize)
        // WmVrnEnabled(hwndlist[i]);
        i++;
    }

    return 0;
}

void CanvasDisplaySpeed(int speed, int frame_rate, int warp_enabled)
{
    int i=0;

    if (!hwndlist)
        return;

    if (!FullscreenIsNow())
    {
        i = 0;
        while (hwndlist[i])
        {
            const video_canvas_t *c = GetCanvas(hwndlist[i++]);

            char *txt=lib_msprintf("%s - %d%% - %dfps %s",
                                c->title, speed, frame_rate,
                                warp_enabled?"(Warp)":"");
            WinSetWindowText(c->hwndTitlebar, txt);
            lib_free(txt);
        }
    }
    /*
    else
    {
        extern FNVMIENTRY *pfnVMIEntry;         // The entry of VMAN.DLL
        TEXTBLTINFO txt;
        memset(&txt, 0, sizeof(TEXTBLTINFO));

        txt.ulLength = sizeof(TEXTBLTINFO);


        (*pfnVMIEntry)(0, 18/*VMI_CMD_TEXT/, &txt, 0);
    }
    */
}

/* ------------------------------------------------------------------------ */
static UINT border_size_x(void)
{
    switch (border)
    {
    case 1:
        return WinQuerySysValue(HWND_DESKTOP, SV_CXBORDER);
    case 2:
        return WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME);
    }
    return 0;
}

static UINT border_size_y(void)
{
    switch (border)
    {
    case 1:
        return WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER);
    case 2:
        return WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME);
    }
    return 0;
}

static UINT statusbar_height(void)
{
    const UINT fntht  = 11;
    const UINT bdry   = WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER);
    const UINT offset = border==2 ? 0 : 2;

    return fntht + 2*bdry + 2 + offset + 3;
}

/*
static HWND AddStatusFrame(HWND hwnd, UINT x, UINT width, int frame)
{
    const UINT fntht  = 11;
    const UINT bdry   = WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER);
    const UINT offset = border==2 ? 0 : 2;

    return WinCreateWindow(hwnd, WC_FRAME, NULL,
                           WS_VISIBLE|(frame?FS_BORDER:0),
                           x+offset, offset,
                           width, fntht + 2*bdry + 2,
                           NULLHANDLE, HWND_TOP, -1, NULL, NULL);
}

static HWND AddStatusTxt(HWND hwnd, UINT x, UINT width, int id, char *txt)
{
    const UINT fntht  = 11;
    const UINT bdrx   = WinQuerySysValue(HWND_DESKTOP, SV_CXBORDER);
    const UINT bdry   = WinQuerySysValue(HWND_DESKTOP, SV_CYBORDER);

    char *font = "11.System VIO";

    HWND thwnd=WinCreateWindow(hwnd, WC_STATIC, txt,
                               WS_VISIBLE|SS_TEXT|DT_VCENTER|DT_CENTER,
                               x+2*bdrx, bdry,
                               width-4*bdrx, fntht,
                               NULLHANDLE, HWND_TOP,
                               id, //FID_STATUS,
                               NULL, NULL);

    WinSetFont(thwnd, font);

    return thwnd;
}
*/
static UINT GetWindowHeight(video_canvas_t *c)
{
    UINT height = c->height;

    height *= c->stretch;
    height += WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
    height += 2*border_size_y();
    if (menu)   height += WinQuerySysValue(HWND_DESKTOP, SV_CYMENU)+1;  // FIXME: +1 ?
    if (status) height += statusbar_height();

    return height;
}

static UINT GetWindowWidth(video_canvas_t *c)
{
    UINT width = c->width;

    width *= c->stretch;
#if defined __XVIC__
    width *= 2;
#endif
    width += 2*border_size_x();

    return width;
}

/* ------------------------------------------------------------------------ */
/*
static void status_resize(video_canvas_t *c)
{
    SWP swp;
    WinQueryWindowPos(WinWindowFromID(c->hwndFrame, FID_STATUS), &swp);

    log_debug("Size 1: %3d %3d", swp.x, swp.y);

    // FIXME
    WinSetWindowPos(WinWindowFromID(c->hwndFrame, FID_STATUS), 0,
                    border_size_x(), border_size_y(),     // Position (x,y)
                    c->width*stretch, statusbar_height(), // Size (width,height)
                    SWP_SIZE|SWP_MOVE);

    WinQueryWindowPos(WinWindowFromID(c->hwndFrame, FID_STATUS), &swp);
    log_debug("Size 2: %3d %3d", swp.x, swp.y);
}

static int set_status(resource_value_t v, void *hwnd)
{
    SWP swp;
    video_canvas_t *c = (video_canvas_t *)WinQueryWindowPtr((HWND)hwnd, QWL_USER);

    status = (int)v;

    if (!hwnd || !c)
        return 0;

    //
    // correct window size and position
    //
    WinQueryWindowPos(c->hwndFrame, &swp);
    WinSetWindowPos(c->hwndFrame, 0,
                    swp.x, swp.y+(status?-statusbar_height():statusbar_height()),
                    canvas_fullwidth (c->width),
                    canvas_fullheight(c->height),
                    SWP_SIZE|SWP_MOVE|SWP_MINIMIZE);

    //
    // show or hide the menu bar
    //
    WinShowWindow(WinWindowFromID(c->hwndFrame, FID_STATUS), status);

    //
    // update frame (show or hide menubar physically)
    //
    // REMARK: instead of updating the frame I minimaze the
    // window and maximize it again. This makes sure that
    // the menubar is shown and not only activated. I don't know
    // why, but it seems to work well. (I don't use HIDE and
    // SHOW because this two functions are animated)
    //
    // WinSendMsg(c->hwndFrame, WM_UPDATEFRAME, (void*)FCF_MENU, NULL);

    WinSetWindowPos(c->hwndFrame, 0, 0, 0, 0, 0, SWP_RESTORE);

    return 0;
}
*/
static int set_menu(resource_value_t v, void *param)
{
    int i=0;

    menu = (int)v;

    if (!hwndlist)
        return 0;

    while (hwndlist[i])
    {
        video_canvas_t *c = GetCanvas(hwndlist[i++]);
        //
        // correct window size
        //
        WinSetWindowPos(c->hwndFrame, 0, 0, 0,
                        GetWindowWidth(c), GetWindowHeight(c),
                        SWP_SIZE|SWP_MINIMIZE);

        //
        // show or hide the menu bar
        //
        WinSetParent(c->hwndMenu, menu?c->hwndFrame:HWND_OBJECT, FALSE);

        //
        // update frame (show or hide menubar physically)
        //
        // REMARK: instead of updating the frame I minimaze the
        // window and maximize it again. This makes sure that
        // the menubar is shown and not only activated. I don't know
        // why, but it seems to work well. (I don't use HIDE and
        // SHOW because this two functions are animated)
        //
        // WinSendMsg(c->hwndFrame, WM_UPDATEFRAME, (void*)FCF_MENU, NULL);

        WinSetWindowPos(c->hwndFrame, 0, 0, 0, 0, 0, SWP_RESTORE);
    }
    return 0;
}

static int set_border_type(resource_value_t v, void *param)
{
    switch ((int)v)
    {
    case 1:
        flFrameFlags &= ~FCF_DLGBORDER;
        flFrameFlags |= FCF_BORDER;
        border = 1;
        break;
    case 2:
        flFrameFlags &= ~FCF_BORDER;
        flFrameFlags |= FCF_DLGBORDER;
        border = 2;
        break;
    default:
        flFrameFlags &= ~FCF_BORDER;
        flFrameFlags &= ~FCF_DLGBORDER;
        border = 0;
    }
    return 0;
}

static int logwin;
static int set_logging(resource_value_t v, void *param)
{
    logwin = (int)v;
    return 0;
}

static const resource_t resources1[] = {
#ifndef DIRECT_ACCESS
    { "WindowStretchFactor", RES_INTEGER, (resource_value_t)1,
      (void *)&stretch, set_stretch_factor, NULL },
#endif
    { "PMBorderType", RES_INTEGER, (resource_value_t)2,
      (void *)&border, set_border_type, NULL },
    { "Menubar", RES_INTEGER, (resource_value_t)1,
      (void *)&menu, set_menu, NULL },
/*
    { "Statusbar", RES_INTEGER, (resource_value_t)1,
      (void *)&status, set_status, NULL },
*/
      { NULL }
};

static const resource_t resources2[] = {
    { "Logwin", RES_INTEGER, (resource_value_t)1,
      (void *)&logwin, set_logging, NULL },
    NULL
};

int video_arch_resources_init(void)
{
    return (vsid_mode?0:resources_register(resources1)) |
        resources_register(resources2);
}

void video_arch_resources_shutdown(void)
{
}

static const cmdline_option_t cmdline_options1[] = {
#ifndef DIRECT_ACCESS
    { "-stretch", SET_RESOURCE, 1, NULL, NULL, "WindowStretchFactor", NULL,
      "<number>", "Specify stretch factor for PM Windows (1,2,3,...)" },
#endif
    { "-border",  SET_RESOURCE, 1, NULL, NULL, "PMBorderType", NULL,
      "<number>", "Specify window border type (1=small, 2=dialog, else=no border)" },
    { "-menu", SET_RESOURCE, 0, NULL, NULL, "Menubar", (resource_value_t) 1,
      NULL, "Enable Main Menu Bar" },
    { "+menu", SET_RESOURCE, 0, NULL, NULL, "Menubar", (resource_value_t) 0,
      NULL, "Disable Main Menu Bar" },
/*
    { "-status", SET_RESOURCE, 0, NULL, NULL, "Statusbar", (resource_value_t) 1,
      NULL, "Enable Status Bar" },
    { "+status", SET_RESOURCE, 0, NULL, NULL, "Statusbar", (resource_value_t) 0,
      NULL, "Disable Status Bar" },
*/
    NULL
};

static const cmdline_option_t cmdline_options2[] = {
    { "-logwin", SET_RESOURCE, 0, NULL, NULL, "Logwin", (resource_value_t) 1,
      NULL, "Enable Logging Window" },
    { "+logwin", SET_RESOURCE, 0, NULL, NULL, "Logwin", (resource_value_t) 0,
      NULL, "Disable Logging Window" },
    NULL
};

int video_init_cmdline_options(void)
{
    return (vsid_mode?0:cmdline_register_options(cmdline_options1)) |
        cmdline_register_options(cmdline_options2);
}

/* ------------------------------------------------------------------------ */
const int DIVE_RECTLS = 50;
static DIVE_CAPS divecaps;

extern HMTX hmtxKey;

void KbdOpenMutex(void)
{
    ULONG rc;

    //
    // FIXME
    // Is this the right place to initialize the keyboard semaphore?
    //
    rc=DosCreateMutexSem("\\SEM32\\Vice2\\Keyboard", &hmtxKey, 0, FALSE);
    if (rc==258 /*ERROR_DUPLICATE_NAME*/)
    {
        //
        // we are in a different process (eg. second instance of x64)
        //
        rc=DosOpenMutexSem("\\SEM32\\Vice2\\Keyboard", &hmtxKey);
        if (!rc)
            log_error(vidlog, "DosOpenMutexSem (rc=%li)", rc);
        return;
    }

    if (rc)
        log_error(vidlog, "DosCreateMutexSem (rc=%li)", rc);
}

static int PM_winActive;

typedef struct _kstate
{
    //    BYTE numlock;
    BYTE scrllock;
    BYTE capslock;
} kstate;

static kstate vk_vice;
static kstate vk_desktop;

extern int use_leds;

void KbdInit(void)
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

void KbdDestroy(void)
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

int video_init(void) // initialize Dive
{
    APIRET rc;

    vidlog = log_open("Video");

    if (vsid_mode)
        return 0;

    log_message(vidlog, "Query Dive capabilities...");

    divecaps.ulStructLen    = sizeof(DIVE_CAPS);
    divecaps.ulFormatLength = 0;

    rc = DiveQueryCaps(&divecaps, DIVE_BUFFER_SCREEN);

    if (rc!=DIVE_SUCCESS && rc!=DIVE_ERR_INSUFFICIENT_LENGTH)
    {
        log_error(vidlog, "DiveQueryCaps failed (rc=0x%x)", rc);
        return -1;
    }

    rc = divecaps.fccColorEncoding;
    log_message(vidlog, "Detected Display = %dx%dx%d (%c%c%c%c)",
                divecaps.ulHorizontalResolution,
                divecaps.ulVerticalResolution,
                divecaps.ulDepth,
                rc, rc>>8, rc>>16, rc>>24);
    log_message(vidlog, "%d Planes, %dkB VRAM%s%s",
                divecaps.ulPlaneCount,
                divecaps.ulApertureSize/1024,
                divecaps.fScreenDirect?", Direct access supported":"",
                divecaps.fBankSwitched?", Bank switch required":"");

    if (divecaps.fccColorEncoding!=FOURCC_LUT8 &&
        divecaps.fccColorEncoding!=FOURCC_R565 &&
        divecaps.fccColorEncoding!=FOURCC_RGB3 &&
        divecaps.fccColorEncoding!=FOURCC_RGB4)
        divecaps.fccColorEncoding = FOURCC_RGB3;

    rc = divecaps.fccColorEncoding;
    log_message(vidlog, "Vice internal rendering switched to %c%c%c%c.",
                rc, rc>>8, rc>>16, rc>>24);

    log_message(vidlog, "Initializing Fullscreen Capabilities...");

#ifndef __X128__
    if (FullscreenInit(divecaps.fccColorEncoding)!=NO_ERROR)
        FullscreenFree();
/*    else
        FullscreenPrintModes();*/
#endif

    //
    // Init Keyboard Led handling
    //
    KbdOpenMutex();
    KbdInit();

    return 0;
}

void video_shutdown(void)
{
}

void video_close(void)
{
    FullscreenFree();
    KbdDestroy();
}

/* ------------------------------------------------------------------------ */
/* PM Window mainloop */

static ULONG GetDepth(FOURCC fcc)
{
    //
    // Should be used only for 'valid' modes: LUT8, R565, RGB3, RGB4
    //
    switch (fcc)
    {
    case FOURCC_LUT8: return 8;
    case FOURCC_R565:
    case FOURCC_R555:
    case FOURCC_R664: return 16;
    case FOURCC_RGB3:
    case FOURCC_BGR3:// return 24;
    case FOURCC_RGB4:
    case FOURCC_BGR4: return 32;
    }
    return 0;
}

static ULONG GetRed(FOURCC fcc, ULONG n)
{
    switch (fcc)
    {
    case FOURCC_R565: return (n>>3)<<11;
    case FOURCC_R555: return (n>>3)<<10;
    case FOURCC_R664: return (n>>2)<<10;
    case FOURCC_RGB3:
    case FOURCC_RGB4: return n<<16;
    case FOURCC_BGR3:
    case FOURCC_BGR4: return n<<16;
    }
    return 0;
}

static ULONG GetGreen(FOURCC fcc, ULONG n)
{
    switch (fcc)
    {
    case FOURCC_R565: return (n>>2)<<5;
    case FOURCC_R555: return (n>>3)<<5;
    case FOURCC_R664: return (n>>2)<<4;
    case FOURCC_RGB3:
    case FOURCC_RGB4: return n<<8;
    case FOURCC_BGR3:
    case FOURCC_BGR4: return n<<8;
    }
    return 0;
}

static ULONG GetBlue(FOURCC fcc, ULONG n)
{
    switch (fcc)
    {
    case FOURCC_R565:
    case FOURCC_R555: return (n>>3);
    case FOURCC_R664: return (n>>4);
    case FOURCC_RGB3:
    case FOURCC_RGB4: return n;
    case FOURCC_BGR3:
    case FOURCC_BGR4: return n;
    }
    return 0;
}

static void VideoInitRenderer(video_canvas_t *c)
{
    const FOURCC fcc = c->divesetup.fccSrcColorFormat;

    int i;

    video_render_initconfig(c->videoconfig);

    // video_render_setrawrgb: index, r, g, b
    for (i=0; i<0x100; i++)
        video_render_setrawrgb(i,
                               GetRed(fcc, i),
                               GetGreen(fcc, i),
                               GetBlue(fcc, i));

    video_render_initraw();
}

MRESULT WmCreate(HWND hwnd)
{
    ULONG rc;

    /*video_canvas_t *canvas_new = (video_canvas_t *)calloc(1, sizeof(video_canvas_t));*/
    video_canvas_t *canvas_new = video_canvas_init();

    archdep_create_mutex_sem(&canvas_new->hmtx, "Video", FALSE);

    //
    // Setup divesetup, so that dive doesn't crash when trying to call
    // DiveSetupBlitter. This can happen as soon as hDiveInst != 0
    //
    canvas_new->hDiveInst = 0;

    canvas_new->divesetup.ulStructLen       = sizeof(SETUP_BLITTER);
    canvas_new->divesetup.ulDitherType      = 0; // 0=1x1, 1=2x2
    canvas_new->divesetup.fInvert           = 0; // Bit0=vertical, Bit1=horz
    canvas_new->divesetup.fccSrcColorFormat = divecaps.fccColorEncoding;
    canvas_new->bDepth                      = GetDepth(divecaps.fccColorEncoding);
    canvas_new->divesetup.fccDstColorFormat = FOURCC_SCRN;
    canvas_new->divesetup.pVisDstRects      = calloc(DIVE_RECTLS, sizeof(RECTL));

    //
    // this is a dummy setup. It is needed for some graphic
    // drivers that they get valid values for the first time we
    // setup the dive blitter regions
    //
    canvas_new->divesetup.ulSrcWidth  = 1;
    canvas_new->divesetup.ulSrcHeight = 1;
    canvas_new->divesetup.ulSrcPosX   = 0;
    canvas_new->divesetup.ulSrcPosY   = 0;
    canvas_new->divesetup.ulDstWidth  = 1;
    canvas_new->divesetup.ulDstHeight = 1;
    canvas_new->divesetup.lDstPosX    = 0;
    canvas_new->divesetup.lDstPosY    = 0;

    //
    // No try to open a dive instance for this window.
    //
    if (rc=DiveOpen(&canvas_new->hDiveInst, FALSE, &canvas_new->pVram))
    {
        log_error(vidlog, "DiveOpen failed (rc=0x%x)", rc);

        //
        // Stop window creation
        //
        return (MRESULT)TRUE;
    }

    log_message(vidlog, "Dive instance #%d opened successfully (Vram=%p)",
                canvas_new->hDiveInst, canvas_new->pVram);

    //
    // Now initialize and setuip vice renderers
    //
    VideoInitRenderer(canvas_new);

    //
    // set user data pointer to newly created canvas structure
    //
    WinSetWindowPtr(hwnd, QWL_USER, canvas_new);

    //
    // Continue window creation
    //
    return FALSE;
}

void VideoBufferFree(video_canvas_t *c)
{
    ULONG rc;

    if (!c->bitmaptrg)
        return;

    if (rc=DiveFreeImageBuffer(c->hDiveInst, c->ulBuffer))
        log_error(vidlog,"DiveFreeImageBuffer (rc=0x%x).", rc);
    else
        log_message(vidlog,"Dive buffer #%d freed.", c->ulBuffer);

    lib_free(c->bitmaptrg);
}

void WmDestroy(HWND hwnd)
{
    ULONG rc;

    video_canvas_t *c = GetCanvas(hwnd);

    if (!c)
        return;
}

void WmSetSelection(MPARAM mp1)
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

void WmVrnEnabled(HWND hwnd)
{
    HPS  hps;
    HRGN hrgn;

    video_canvas_t *c = GetCanvas(hwnd);

    DEBUG("WM VRN ENABLED 0");

    if (!c->hDiveInst)
        return;

    hps  = WinGetPS(hwnd);
    hrgn = GpiCreateRegion(hps, 0L, NULL);

    DEBUG("WM VRN 0");

    if (hrgn) // this should be controlled again (clr/home, stretch 3)
    {
        RGNRECT rgnCtl;

        WinQueryVisibleRegion(hwnd, hrgn);

        rgnCtl.ircStart    = 1;
        rgnCtl.crc         = DIVE_RECTLS;
        rgnCtl.ulDirection = RECTDIR_LFRT_BOTTOP;

        if (GpiQueryRegionRects(hps, hrgn, NULL, &rgnCtl, c->divesetup.pVisDstRects))
        {
            SWP    swp;
            POINTL pointl;

            WinQueryWindowPos(hwnd, &swp);
            pointl.x = swp.x;
            pointl.y = swp.y;
            WinMapWindowPoints(c->hwndFrame, HWND_DESKTOP, &pointl, 1);

            c->divesetup.lScreenPosX   = pointl.x;
#ifndef DIRECT_ACCESS
            c->divesetup.lScreenPosY   = pointl.y;
#else
            c->divesetup.lScreenPosY   = divecaps.ulVerticalResolution-(pointl.y+c->height);
#endif
            c->divesetup.ulNumDstRects = rgnCtl.crcReturned;

            // FIXME? Correct?
            if (FullscreenIsInFS())
            {
                const int w = FullscreenQueryHorzRes();
                const int h = FullscreenQueryVertRes();

                c->divesetup.ulNumDstRects = 1;
                c->divesetup.pVisDstRects[0].xLeft   = 0;
                c->divesetup.pVisDstRects[0].xRight  = w;
                c->divesetup.pVisDstRects[0].yBottom = 0;
                c->divesetup.pVisDstRects[0].yTop    = h;
                c->divesetup.lScreenPosY = (h-c->stretch*c->height)/2;
                c->divesetup.lScreenPosX = (w-c->stretch*c->width
#if defined __XVIC__
    *2
#endif
                                           )/2;
            }
            //DiveSetupBlitter(c->hDiveInst, &c->divesetup);
        }
        GpiDestroyRegion(hps, hrgn);
    }
    WinReleasePS(hps);

    c->vrenabled=TRUE;

    //
    // blit the whole visible area to the screen at next blit time
    //
    //c->exposure_handler(c->width, c->height);

    DEBUG("WM VRN ENABLED 1");
}

void WmVrnDisabled(HWND hwnd)
{
    video_canvas_t *c = GetCanvas(hwnd);

    ULONG rc;

    DEBUG("WM VRN DISABLED 0");

    if (!c->hDiveInst)
        return;

    //
    // Now stop drawing into this canvas
    //
    c->vrenabled=FALSE;

    rc = DiveSetupBlitter(c->hDiveInst, NULL);
    if (rc!=DIVE_SUCCESS)
    {
        log_error(vidlog, "WmVrnDisabled: Call to DiveSetupBlitter failed, rc = 0x%x", rc);
        return;
    }

    DEBUG("WM VRN DISABLED 1");
}

void VideoCanvasBlit(video_canvas_t *c,
                     UINT xs, UINT ys, UINT xi, UINT yi, UINT w, UINT h)
{
    ULONG rc;
    ULONG numlines     = 0;
    ULONG scanlinesize = 0;
    BYTE *targetbuffer = NULL;

    const int dsx = c->videoconfig->doublesizex + 1;
    const int dsy = c->videoconfig->doublesizey + 1;

    const UINT  linesz = dsx*c->draw_buffer->draw_buffer_width;
    const UINT  bufh   = dsy*c->draw_buffer->draw_buffer_height;

    //
    // xs, xy is double sized
    // w,  h  is double sized
    // xi, yi is double sized
    //
    if (xs+w > linesz)
    {
        log_message(vidlog, "Debug - fbuf read x: %d + %d > %d, c->width=%d", xs, w, linesz, c->width);
        w = linesz-xs;
    }
    if (ys+h > bufh)
    {
        log_message(vidlog, "Debug - fbuf read y: %d + %d > %d, c->height=%d", ys, h, bufh, c->height);
        h = bufh-ys;
    }
    if (xs<0)
    {
        log_message(vidlog, "Debug - fbuf read x: %d < 0", xs);
        w += xs;
        xs = 0;
    }
    if (ys<0)
    {
        log_message(vidlog, "Debug - fbuf read y: %d < 0", ys);
        h += ys;
        ys = 0;

    }
    if (xi+w > c->width)
    {
        log_message(vidlog, "Debug - scr write x x%d: %d + %d > %d", dsx, xi, w, c->width);
        w = c->width-xi;
    }
    if (yi+h > c->height)
    {
        log_message(vidlog, "Debug - scr write y x%d: %d + %d > %d", dsy, yi, h, c->height);
        h = c->height-yi;
    }
    if (xi<0)
    {
        log_message(vidlog, "Debug - scr write x: %d < 0", xi);
        w += xi;
        xi = 0;
    }
    if (yi<0)
    {
        log_message(vidlog, "Debug - scr write y: %d < 0", yi);
        h += yi;
        yi = 0;
    }

#ifndef DIRECT_ACCESS
    //
    // calculate source and destinations
    //
    c->divesetup.ulSrcWidth  = w;         //c->width;//w;
    c->divesetup.ulSrcHeight = h;         //c->height;//h;
    c->divesetup.ulSrcPosX   = xi;        //0; //xs;
    c->divesetup.ulSrcPosY   = yi;        //0; //ys;
    c->divesetup.ulDstHeight = stretch*h; //c->height;//h;
    c->divesetup.ulDstWidth  = stretch*w  //c->width; //w;
#ifdef __XVIC__
        *2
#endif
        ;
    c->divesetup.lDstPosX    = stretch*xi  //0
#ifdef __XVIC__
        *2
#endif
        ;
    c->divesetup.lDstPosY    = (c->height-(yi+h))*stretch; //0

    //        if (status)
    //            divesetup.lDstPosY += statusbar_height();

    //
    // now setup the draw areas
    // (all other values are set already by WM_VRNENABLED)
    //
    // FIXME: maybe this should only be done when
    //        video cache is enabled
    rc = DiveSetupBlitter(c->hDiveInst, &c->divesetup);
    if (rc!=DIVE_SUCCESS)
    {
        log_error(vidlog, "VideoCanvasBlit: Call to DiveSetupBlitter failed, rc = 0x%x", rc);
        return;
    }

    //
    // Dependant on the underlaying hardware here we get
    // the targetbuffer pointer, the scanlinesize and
    // the number of scanlines
    //
    rc = DiveBeginImageBufferAccess(c->hDiveInst, c->ulBuffer,
                                    &targetbuffer, &scanlinesize,
                                    &numlines);
    if (rc!=DIVE_SUCCESS)
    {
        log_error(vidlog, "Call to DiveBeginImageBufferAccess failed, rc = 0x%x", rc);
        return;
    }
    video_canvas_render(c,
                        targetbuffer,      // c->bitmaptrg,        // bitmap target (screen/dive)
                        w, h,              // f->width, f->height, // bitmap width, height (2copy)
                        xs/dsx, ys/dsy,    // 0, 0,                // top, left source
                        xi, yi,            // 0, 0,                // top, left target
                        scanlinesize,      // fccColorEncoding,    // c->width,            // line size target
                        c->bDepth);

    rc = DiveEndImageBufferAccess(c->hDiveInst, c->ulBuffer);
    if (rc!=DIVE_SUCCESS)
    {
        log_error(vidlog, "Call to DiveEndImageBufferAccess failed, rc = 0x%x", rc);
        return;
    }

    //
    // and blit the image to the screen
    //
    rc = DiveBlitImage(c->hDiveInst, c->ulBuffer, DIVE_BUFFER_SCREEN);
    if (rc!=DIVE_SUCCESS)
    {
        log_error(vidlog, "Call to DiveBlitImage failed, rc = 0x%x", rc);
        return;
    }
    /*
     {
     BYTE mask[500];
     int i;
     for (i=0; i<500; i+=2)
     mask[i] = xi&1 ? 0x1 | 0x4 | 0x10 | 0x40 :  0x2 | 0x8 | 0x20 | 0x80;
     DiveBlitImageLines(c->hDiveInst, c->ulBuffer, DIVE_BUFFER_SCREEN, mask);
     }
     */
#else
    {
        RECTL rectl =
        {
            c->divesetup.lScreenPosX,          c->divesetup.lScreenPosY,
            c->divesetup.lScreenPosX+c->width, c->divesetup.lScreenPosY+c->height
        };
        rc = DiveAcquireFrameBuffer(c->hDiveInst, &rectl);
    }
    if (rc!=DIVE_SUCCESS)
    {
        log_error(vidlog, "Call to DiveAcquireFrameBuffer failed, rc = 0x%x", rc);
        return;
    }

    if (c->divesetup.lScreenPosX+xi+w >= divecaps.ulHorizontalResolution)
        w = divecaps.ulHorizontalResolution - (c->divesetup.lScreenPosX + xi);

    if (c->divesetup.lScreenPosY+yi+h >= divecaps.ulVerticalResolution)
        h = divecaps.ulVerticalResolution - (c->divesetup.lScreenPosY + yi);

    video_canvas_render(c,
                        c->pVram,
                        w, h,
                        xs/dsx, ys/dsy,
                        c->divesetup.lScreenPosX+xi,
                        c->divesetup.lScreenPosY+yi,
                        divecaps.ulScanLineBytes,
                        c->bDepth);

    rc = DiveDeacquireFrameBuffer(c->hDiveInst);
    if (rc!=DIVE_SUCCESS)
    {
        log_error(vidlog, "Call to DiveDeacquireFrameBuffer failed, rc = 0x%x", rc);
        return;
    }
#endif
}

void WmPaint(HWND hwnd)
{
    canvas_refresh_t ref;

    //
    // get pointer to actual canvas from user data area
    //
    video_canvas_t *c = GetCanvas(hwnd);

    DEBUG("WM_PAINT 0");

    //
    // if the canvas isn't setup yet or we are in a different
    // mode than the present videomode return
    //
    if (!c || !FullscreenIsInMode())
        return;

    DEBUG("WM_PAINT 1");

    //
    // Make sure that we are not in a conflict with the emulation
    // thread calling video_canvas_refresh (wmVrnEnabled)
    //
    if (DosRequestMutexSem(c->hmtx, SEM_IMMEDIATE_RETURN))
        return;
/*
 #ifdef DIRECT_ACCESS
 */
    //
    // get the frame_buffer and geometry from the machine
    //
    if (machine_canvas_async_refresh(&ref, c) >= 0)
    {
        DEBUG("WM_PAINT 2");

        //
        // enable the visible region - it is disabled
        //
        WmVrnEnabled(hwnd);

        DEBUG("WM_PAINT 3");

        //
        // blit to canvas (canvas_refresh should be thread safe by itself)
        //
        VideoCanvasBlit(c, ref.x, ref.y, 0, 0, c->width, c->height);
    }
/*
#else
    //
    // calculate source and destinations
    //
    c->divesetup.ulSrcWidth  = c->width;
    c->divesetup.ulSrcHeight = c->height;
    c->divesetup.ulSrcPosX   = 0;
    c->divesetup.ulSrcPosY   = 0;
    c->divesetup.lDstPosX    = 0;
    c->divesetup.lDstPosY    = 0;
    c->divesetup.ulDstHeight = stretch*c->height;
    c->divesetup.ulDstWidth  = stretch*c->width
#ifdef __XVIC__
        *2
#endif
        ;

    //
    // now setup the draw areas
    // (all other values are set already by WM_VRNENABLED)
    //
    DiveSetupBlitter(c->hDiveInst, &c->divesetup);

    //
    // and blit the image to the screen
    //
    DiveBlitImage(c->hDiveInst, c->ulBuffer, DIVE_BUFFER_SCREEN);
#endif
*/
    DosReleaseMutexSem(c->hmtx);

    DEBUG("WM_PAINT 4");
}

static int WmTranslateAccel(HWND hwnd, MPARAM mp1)
{
    QMSG *qmsg = (QMSG*)mp1;

    //
    // if key is pressed together with alt let the acceltable
    // process the pressed key
    //
    if (SHORT1FROMMP(qmsg->mp1)&KC_ALT)
        return TRUE;

    //
    // if the focus of the actual canvas (eg. F1 pressed in
    // open menu) isn't the actual focus let the acceltable
    // process the pressed key
    //
    if (hwnd != qmsg->hwnd)
        return TRUE;

    //
    // don't let the acceltable translate the key (eg. to get
    // F1 passed to WM_CHAR)
    //
    return FALSE;
}

static void DisplayPopupMenu(HWND hwnd, POINTS *pts)
{
    const video_canvas_t *c = GetCanvas(hwnd);

    WinPopupMenu(hwnd, hwnd, c->hwndPopupMenu, pts->x, pts->y, 0,
                 PU_MOUSEBUTTON2DOWN|PU_HCONSTRAIN|PU_VCONSTRAIN);
}

MRESULT EXPENTRY CanvasWinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
        //
        // --- Open / Close ---
        //
    case WM_CREATE:
        return WmCreate(hwnd);

        /*
         WM_CLOSE sends WM_QUIT
         WM_DESTROY after window disappeared
         */
    //case WM_QUIT:
    case WM_CLOSE:
    //case WM_DESTROY:
        // #define WM_DESTROY 0x0002
        // #define WM_CLOSE   0x0029
        // #define WM_QUIT    0x002a
        FullscreenDisable();
        WmDestroy(hwnd);
        break; /* return FALSE */

        //
        // --- Video / Paint ---
        //
    case WM_PAINT:
        WmPaint(hwnd);
        break;

    case WM_VRNDISABLED:
        WmVrnDisabled(hwnd);
        return FALSE;

    case WM_VRNENABLED:
        WmVrnEnabled(hwnd);
        return FALSE;

        //
        // --- Menubar ---
        //
    case WM_COMMAND:
        menu_action(hwnd, SHORT1FROMMP(mp1));
        return FALSE;

    case WM_MENUSELECT:
        menu_select(HWNDFROMMP(mp2), SHORT1FROMMP(mp1));
        return (MRESULT)TRUE; /* dismiss menu */

        //
        // --- Keyboard / Mouse ---
        //
    case WM_SETSELECTION:
        WmSetSelection(mp1);
        break;

    case DM_DRAGOVER:
    case DM_DROP:
        return DragDrop(hwnd, msg, mp1);

    case WM_TRANSLATEACCEL:
        if (WmTranslateAccel(hwnd, mp1))
            break;
        return FALSE;

    case WM_CHAR:
        //CHAR: 3e010166 236b00 - Alt F4
        //CHAR: 3e011166 236b00
        kbd_proc(hwnd, mp1, mp2);

        if ((SHORT1FROMMP(mp1)&(KC_ALT|KC_VIRTUALKEY|KC_KEYUP))!=(KC_ALT|KC_VIRTUALKEY|KC_KEYUP))
            break;

        vsync_suspend_speed_eval();
        switch (SHORT2FROMMP(mp2))
        {
        case VK_HOME:
            FullscreenSwitch(hwnd);
            break;
        case VK_PAGEUP:
            FullscreenChangeRate(hwnd, +1);
            break;
        case VK_PAGEDOWN:
            FullscreenChangeRate(hwnd, -1);
            break;
        }
        break;

    case WM_ACTIVATE:     // activation/deactivation of window
        if (!mp1)         // Switching away from the applicatoin
            FullscreenDeactivate(hwnd);
        else              // Switching to the application
            FullscreenActivate(hwnd);
        break;

#ifdef HAVE_MOUSE
    case WM_BUTTON2DOWN:
        if (!menu)
        {
            DisplayPopupMenu(hwnd, (POINTS*)&mp1);
            return FALSE;
        }
     // ** FALTHROU **
    case WM_MOUSEMOVE:
    case WM_BUTTON1DOWN:
    case WM_BUTTON1UP:
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

static void InitHelp(HWND hwndFrame, int id, PSZ title, PSZ libname)
{
    HELPINIT hini;

    HWND hwndHelp;
    HAB  hab = WinQueryAnchorBlock(hwndFrame);

    //
    // initialize help init structure
    //
    memset(&hini, 0, sizeof(hini));

    hini.cb                 = sizeof(HELPINIT);
    hini.phtHelpTable       = (PHELPTABLE)MAKELONG(id, 0xFFFF);
    hini.pszHelpWindowTitle = (PSZ)title;
    hini.pszHelpLibraryName = (PSZ)libname;
#ifdef HLPDEBUG
    hini.fShowPanelId       = CMIC_SHOW_PANEL_ID;
#else
    hini.fShowPanelId       = CMIC_HIDE_PANEL_ID;
#endif

    //
    // creating help instance
    //
    hwndHelp = WinCreateHelpInstance(hab, &hini);

    if (!hwndHelp || hini.ulReturnCode)
    {
        log_error(vidlog, "WinCreateHelpInstance failed (rc=%d)",
                  hini.ulReturnCode);
        return; // error
    }

    //
    // associate help instance with main frame
    //
    if (!WinAssociateHelpInstance(hwndHelp, hwndFrame))
    {
        log_error(vidlog, "WinAssociateInstance failed.");
        return; // error
    }
}

extern int trigger_shutdown;

static void InitMenuBar(video_canvas_t *c)
{
    //
    // Load popup menu from resource file (this is attached to the frame)
    //
    c->hwndPopupMenu = WinLoadMenu(c->hwndFrame, NULLHANDLE, IDM_VICE2);

    //
    // detach it from the frame window
    //
    WinSetParent(c->hwndPopupMenu, HWND_OBJECT, FALSE);

    //
    // initialize handle to menubar
    //
    c->hwndMenu = WinWindowFromID(c->hwndFrame, FID_MENU);

    //
    // set visibility state of menubar to actual state
    // WM_UPDATEFRAME also hides the loaded popup menu
    // the size is also corrected
    //
    set_menu((void*)menu, (void*)c->hwndClient);
}
/*
void InitStatusBar(video_canvas_t *c)
{
    HWND hwnd=WinCreateWindow(c->hwndFrame,       // Parent window
                              WC_FRAME,           // Class name
                              NULL,               // Window text
                              0,                  // Frame control
                              border_size_x(), border_size_y(), // Position
                              c->width,
                              statusbar_height(), // Size (width,height)
                              NULLHANDLE,         // Owner window
                              HWND_TOP,           // Sibling window
                              FID_STATUS,         // Window id
                              NULL,               // Control data
                              NULL);              // Pres parameters

    HWND fhwnd = AddStatusFrame(hwnd, 0, 55, TRUE);
    HWND stat  = AddStatusFrame(hwnd, c->width-110, 110, FALSE);

    AddStatusTxt(fhwnd, 0, 55, -1, "Status Bar");

    AddStatusTxt(stat,  0, 55, ID_SPEEDDISP,   "99999%");
    AddStatusTxt(stat, 55, 55, ID_REFRATEDISP, "999fps");

    log_debug("speed3: %x", WinWindowFromID(stat, ID_SPEEDDISP));

    //WinSetDlgText(c->hwndFrame, FID_STATUS, "Test");

    set_status((void*)status, (void*)c->hwndClient);
}
*/

void VideoBufferAlloc(video_canvas_t *c)
{
    ULONG rc;

    //
    // FIXME: bitmaptrg needs some additional lines!
    //
    c->bitmaptrg = (char*)calloc(c->height+4, c->width*c->bDepth/8);

    //
    // allocate image buffer, I might be faster to specify 0 for
    // the scanlinesize, but then we need a correction for
    // every time we have a line size which is not
    // devidable by 4
    //
    // if ulBuffer != 0 the call to DiveAllocImageBuffer fails
    //
    c->ulBuffer = 0;
    rc=DiveAllocImageBuffer(c->hDiveInst, &c->ulBuffer,
                            c->divesetup.fccSrcColorFormat,
                            c->width, c->height, 0, c->bitmaptrg);

    //
    // check for error oocursion
    //
    if (rc)
        log_error(vidlog, "DiveAllocImageBuffer (rc=0x%x).", rc);
    else
        log_message(vidlog, "Dive buffer #%d allocated (%lix%li)",
                    c->ulBuffer, c->width, c->height);
}

/*
 Old exposure handler:
 ---------------------
 canvas->draw_buffer->canvas_width=width;
 canvas->draw_buffer->canvas_height=height;
 video_viewport_resize(canvas);
 */

struct canvas_init_s
{
    UINT                   width;
    UINT                   height;
    UINT                   stretch;
    char                  *title;
    video_canvas_t        *canvas;
//    canvas_redraw_t        expose;
    video_render_config_t *videoconfig;
    draw_buffer_t         *draw_buffer;
    viewport_t            *viewport;
    geometry_t            *geometry;
    const palette_t       *palette;
};

typedef struct canvas_init_s canvas_init_t;

void CanvasMainLoop(VOID *arg)
{
    APIRET rc;
    SWP    swp;
    QMSG   qmsg; // Msg Queue Event
    HWND   hwndFrame;
    HWND   hwndClient;

    video_canvas_t *c;

    canvas_init_t *ini = (canvas_init_t*)arg;

    // archdep_setup_signals(0); // signals are not shared between threads!

    HAB hab = WinInitialize(0);            // Initialize Anchor to PM
    HMQ hmq = WinCreateMsgQueue(hab, 0);   // Create Msg Queue Handle

    //
    // 16 Byte Memory (Used eg for the Anchor Blocks)
    //  CS_MOVENOTIFY, CS_SIZEREDRAW skipped
    //  CS_SYNCPAINT:       send WM_PAINT messages immediately
    //   don't send WM_PAINT befor canvas is resized
    //  CS_BYTEALIGNWINDOW: 0x2000
    //
    WinRegisterClass(hab, szClientClass, CanvasWinProc, CS_SYNCPAINT, 0x10);

    //
    // create window on desktop, FIXME: WS_ANIMATE looks sometimes strange
    //
    hwndFrame = WinCreateStdWindow(HWND_DESKTOP, WS_ANIMATE|WS_VISIBLE,
                                   &flFrameFlags, szClientClass,
                                   ini->title, 0L, 0, IDM_VICE2,
                                   &hwndClient);

    if (!hwndFrame)
    {
        ERRORID id = WinGetLastError(hab);
        log_error(vidlog, "WinCreateStdWindow failed (WinGetLastError=0x%x)", id);
    }

    c = GetCanvas(hwndClient);

    if (!c)
    {
        ini->canvas = (void*)-1;
        return;
    }

    c->hwndFrame   = hwndFrame;
    c->hwndClient  = hwndClient;
    c->title       = ini->title;
    c->width       = ini->width;
    c->height      = ini->height;
    c->stretch     = ini->stretch;
    c->videoconfig = ini->videoconfig;
    c->draw_buffer = ini->draw_buffer;
    c->viewport    = ini->viewport;
    c->geometry    = ini->geometry;
    c->palette     = ini->palette;

    VideoBufferAlloc(c);

    //
    // Add to vice internal window list
    // (needed to handle the two windows of x128
    //
    AddToWindowList(hwndClient);

    //
    // get actual window size and position
    //
    WinQueryWindowPos(hwndFrame, &swp);

    //
    // initialize titlebare usage
    //
    c->hwndTitlebar = WinWindowFromID(hwndFrame, FID_TITLEBAR);

    //
    // initialize help system
    //
    InitHelp(hwndFrame, IDM_VICE2, "Vice/2 Help", "doc\\vice2.hlp");

    //
    // initialize menu bar
    //
    InitMenuBar(c);

    //
    // initialize status bar
    //
    // InitStatusBar(c);

    //
    // bring window to top, set size and position, set focus
    // correct for window height
    //
    WinSetWindowPos(hwndFrame, HWND_TOP,
                    swp.x, swp.y+swp.cy-GetWindowHeight(c), 0, 0,
                    SWP_SHOW|SWP_ZORDER|SWP_ACTIVATE|SWP_MOVE);

    //
    // set visible region notify on, enable visible region
    //
    WinSetVisibleRegionNotify(hwndClient, TRUE);
    WinSendMsg(hwndClient, WM_VRNENABLED, 0, 0);

    //
    // this makes reactions faster when shutdown of main thread is triggered
    //
    if (rc=DosSetPriority(PRTYS_THREAD, PRTYC_REGULAR, +1, 0))
        log_error(vidlog, "DosSetPriority (rc=%li)", rc);

    log_message(vidlog, "'%s' completely initialized.", ini->title);

    //
    // Now wake up the emulation thread by setting ini->canvas
    //
    ini->canvas = c;

    //
    // MAINLOOP
    // returns when a WM_QUIT Msg goes through the queue
    //
    while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg(hab, &qmsg);

    //
    // make sure, that the state of the VRN doesn't change anymore,
    // don't blit anymore
    //

    // FIXME: The disabeling can be done in WM_CLOSE???
    // Doesn't seem to 'fast enough' ???
    WinSetVisibleRegionNotify(c->hwndClient, FALSE);
    WmVrnDisabled(c->hwndClient);

    log_message(vidlog, "Window '%s': Quit!", c->title);

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
    log_message(vidlog, "Triggering Shutdown of Emulation Thread.");
    trigger_shutdown = 1;

    log_message(vidlog, "Releasing PM.");

    //log_message(vidlog, "Destroying Msg Queue.");
    if (!WinDestroyMsgQueue(hmq))
        log_error(vidlog, "Destroying Msg Queue.");
    //log_message(vidlog, "Releasing PM anchor.");
    if (!WinTerminate (hab))
        log_error(vidlog, "Releasing PM anchor.");

    log_message(vidlog, "PM released.");

    DosSleep(5000); // wait 5 seconds

    //
    // FIXME, this happens for x128 eg at my laptop when video_close is called
    //
    log_error(vidlog, "Brutal Exit!");

    //
    // if the triggered shutdown hangs make sure that the program ends
    //
    exit(0);        // end VICE in all cases
}

/* ------------------------------------------------------------------------ */
/* Canvas functions.  */
void video_arch_canvas_init(struct video_canvas_s *canvas)
{
    canvas->video_draw_buffer_callback = NULL;
}

/*
 Create a `video_canvas_t' with tile `win_name', of width `*width'
 and `*height'pixels and palette `palette'. If specified width/height
 is not possible, return an alternative in `*width' and `*height';
 return the pixel values for the requested palette in `pixel_return[]'.
*/
video_canvas_t *video_canvas_create(video_canvas_t *canvas, UINT *width,
                                    UINT *height, int mapped)
{
    canvas_init_t canvini;

    log_message(vidlog, "Creation of '%s' (%ix%i) requested%s.",
                canvas->viewport->title, *width, *height,
                vsid_mode ? " (vsid mode)" : "");

    *strrchr(canvas->viewport->title, ' ') = 0; // FIXME?

    canvini.title       =  util_concat(szTitleBarText, " - ",
                                       canvas->viewport->title + 6, NULL);
    canvini.width       = *width;
    canvini.height      = *height;
    canvini.stretch     =  stretch;
    canvini.canvas      =  NULL;
    canvini.videoconfig = canvas->videoconfig;
    canvini.draw_buffer = canvas->draw_buffer;
    canvini.viewport    = canvas->viewport;
    canvini.geometry    = canvas->geometry;
    canvini.palette     = canvas->palette;

    if (canvas->videoconfig->doublesizex)
        canvini.width *= 2;

    if (canvas->videoconfig->doublesizey)
        canvini.height *= 2;

    _beginthread(CanvasMainLoop, NULL, 0x4000, &canvini);

    //
    // Wait until the canvas as either created successfully or
    // the canvas creation failed
    //
    while (!canvini.canvas)
        DosSleep(1);

    //
    // If initialization of canvas failed
    // canvas_ini.canvas is set to -1
    //
    if (canvini.canvas == (void*)-1)
        return NULL;

    log_message(vidlog, "Canvas '%s' (%ix%i) created: hwnd=0x%x.",
                canvas->viewport->title, *width, *height,
                canvini.canvas->hwndClient);

    video_canvas_set_palette(canvini.canvas, canvas->palette);

    canvini.canvas->initialized = 1;

    return canvini.canvas;
}

void video_canvas_destroy(video_canvas_t *c)
{
    ULONG rc;

    //
    // Save values needed later
    //
    HMTX  hmtx  = c->hmtx;
    HDIVE hdive = c->hDiveInst;

    //
    // This stops WM_PAINT
    //
    WinSetWindowPtr(c->hwndClient, QWL_USER, NULL);

    //
    // Set sending of WM_VRN* messages
    //
    WinSetVisibleRegionNotify(c->hwndClient, FALSE);

    //
    // Disable blitter (not really needed)
    //
    c->vrenabled=FALSE;

    DiveSetupBlitter(hdive, NULL);

    //
    // Free video buffer (needs c->hDiveInst)
    //
    VideoBufferFree(c);

    //
    // makes sure, that no WM_VRN* messages and canvas_refresh is
    // processed anymore. Not really needed: for convinience
    //
    c->hDiveInst = 0;

    //
    // No close the dive instance of this canvas
    //
    if (rc=DiveClose(hdive))
        log_error(vidlog, "Closing Dive instance #%d (DiveClose, rc=0x%x)", hdive, rc);
    else
        log_message(vidlog, "Dive instance #%d successfully closed.", hdive);

    //
    // Free the rectangles used for blitting
    //
    lib_free(c->divesetup.pVisDstRects);

    log_message(vidlog, "Destroying data structure for '%s'.", c->title);

    //
    // Free title and canvas structure
    //
    lib_free(c->title);
    video_canvas_shutdown(c);
    lib_free(c);

    //    DosReleaseMutexSem(hmtx);

    //
    // Close Mutex Semaphore
    //
    DosCloseMutexSem(hmtx);
}

void video_canvas_resize(video_canvas_t *c, UINT wnew, UINT hnew)
{
    //
    // if this function is called from the outside of
    // the emulation thread make sure that it is enclosed in
    // a mutex semaphore to avoid conflicts between
    // dive_alloc/free and video_canvas_refresh
    //
    SWP swp;
    ULONG rc;

    UINT wold = c->width;
    UINT hold = c->height;
    UINT sold = c->stretch;

    if (c->videoconfig->doublesizex)
        wnew *= 2;

    if (c->videoconfig->doublesizey)
        hnew *= 2;

    //
    // if nothing has changed do nothing
    //
    if (wold==wnew && hold==hnew && sold==stretch)
        return;

    //
    // Disable all drawing into window, we can change either
    // the stretching factor (the function which does it disables
    // drawing) or the canvas size (this is done by the emulation
    // thread which means that at the same time no drawing will happen)
    //
    WinSetVisibleRegionNotify(c->hwndClient, FALSE); // turn VRN off
    WmVrnDisabled(c->hwndClient);

    //
    // resize canvas description
    //
    // Do it before. WM_PAINT, called from WinSetWindowPos, needs it.
    //
    c->stretch = stretch;

    //
    // resize the buffer from which we blit to the window, too
    //
    if (wold!=wnew || hold!=hnew)
    {
        c->width  = wnew;
        c->height = hnew;

        VideoBufferFree(c);
        VideoBufferAlloc(c);
    }

    //
    // make anchor left, top corner and resize window physically
    //
    WinQueryWindowPos(c->hwndFrame, &swp);
    if (!WinSetWindowPos(c->hwndFrame, 0,
                         swp.x, swp.y+swp.cy-GetWindowHeight(c),
                         GetWindowWidth(c), GetWindowHeight(c),
                         SWP_SIZE|SWP_MOVE))
    {
        log_error(vidlog, "Resizing canvas to %ix%i * %d.", wnew, hnew,
                  stretch);

        c->stretch = sold;

        if (wold!=wnew || hold!=hnew)
        {
            c->width   = wold;
            c->height  = hold;

            VideoBufferFree(c);
            VideoBufferAlloc(c);
        }

        return;
    }

    if (FullscreenIsNow())
    {
        vsync_suspend_speed_eval();
        FullscreenChangeMode(c->hwndClient);
    }

    //
    // reenable drawing into window (call exposure_handler, too)
    // this calls also the exposure handler
    //
    WmVrnEnabled(c->hwndClient);
    WinSetVisibleRegionNotify(c->hwndClient, TRUE); // turn VRN on

    log_message(vidlog, "Canvas resized (%ix%i * %i --> %ix%i * %i)",
                wold, hold, sold, wnew, hnew, stretch);

//    c->exposure(c->width, c->height);
}

void VideoConvertPalette8(video_canvas_t *c, int num)
{
    int i;
    for (i=0; i<num; i++)
        video_render_setphysicalcolor(c->videoconfig, i, i, 8);
}

void VideoConvertPalette(video_canvas_t *c, int num, palette_entry_t *src) //, RGB2 *trg)
{
    ULONG rc;

    ULONG ulTrg=0;
    ULONG ulSrc=0;

    char target[0x400]; // max: 256*4b
    char source[0x400]; // max: 256*4b

    HDIVE inst=0;
    SETUP_BLITTER setup;

    if (num>0x100)
    {
        log_error(vidlog, "VideoConvertPalette - More than 256 (%d) palette entries requested.", num);
        return;
    }

    memset(&setup, 0, sizeof(SETUP_BLITTER));

    setup.ulStructLen          = sizeof(SETUP_BLITTER);
    setup.pVisDstRects         = calloc(1, sizeof(RECTL));
    setup.fccSrcColorFormat    = FOURCC_RGB3;
    setup.fccDstColorFormat    = c->divesetup.fccSrcColorFormat;
    setup.ulSrcHeight          = 1;
    setup.ulDstHeight          = 1;
    setup.ulNumDstRects        = 1;
    setup.pVisDstRects->yTop   = 1;
    setup.pVisDstRects->xRight = num;
    setup.ulSrcWidth           = num;
    setup.ulDstWidth           = num;

    rc = DiveOpen(&inst, TRUE, NULL);
    if (rc)
    {
        log_error(vidlog, "VideoConvertPalette - DiveOpen failed, rc=0x%x", rc);
        return;
    }

    rc=DiveAllocImageBuffer(inst, &ulSrc, FOURCC_RGB3, num, 1, 0, source);
    if (rc)
        log_error(vidlog, "VideoConvertPalette - DiveAllocImageBuffer (src) failed, rc=0x%x", rc);
    else
    {
        rc=DiveAllocImageBuffer(inst, &ulTrg, c->divesetup.fccSrcColorFormat,
                                num, 1, 0, target);
        if (rc)
            log_error(vidlog, "VideoConvertPalette - DiveAllocImageBuffer (trg) failed, rc=0x%x", rc);
        else
        {
            int i;
            for (i=0; i<num; i++)
            {
                source[i*3]   = src[i].red;
                source[i*3+1] = src[i].green;
                source[i*3+2] = src[i].blue;
            }

            rc = DiveSetupBlitter(inst, &setup);
            if (rc)
                log_error(vidlog, "VideoConvertPalette - DiveSetupBlitter failed, rc=0x%x", rc);
            else
            {
                int i;
                DWORD color;
                int bytes = c->bDepth/8;

                rc = DiveBlitImage(inst, ulSrc, ulTrg);
                if (rc)
                    log_error(vidlog, "VideoConvertPalette - DiveBlitImage failed, rc=0x%x", rc);
                else
                {
                    for (i=0; i<num; i++)
                    {
                        int b;

                        color = 0;
                        for (b=0; b<bytes; b++)
                            color |= (ULONG)target[i*bytes+b] << (b*8);

                        video_render_setphysicalcolor(c->videoconfig, i, color,
                                                      c->bDepth);
                    }
                }
            }

            rc=DiveFreeImageBuffer(inst, ulTrg);
            if (rc)
                log_error(vidlog, "VideoConvertPalette - DiveFreeImageBuffer (trg) failed, rc=0x%x", rc);
            lib_free(setup.pVisDstRects);
        }

        rc=DiveFreeImageBuffer(inst, ulSrc);
        if (rc)
            log_error(vidlog, "VideoConvertPalette - DiveFreeImageBuffer (src) failed, rc=0x%x", rc);
    }

    rc = DiveClose(inst);
    if (rc)
        log_error(vidlog, "VideoConvertPalette - DiveClose failed, rc=0x%x", rc);
}

/* Set the palette of `c' to `p'.  */
int video_canvas_set_palette(video_canvas_t *c, palette_t *p)
{
    int i;

    if (c->bDepth==8)
    {
        ULONG rc;

        //
        // number of already used blocks of color data
        //
        RGB2 *palette = calloc(p->num_entries, sizeof(RGB2));

        /*
         log_debug("Setting %d pallette entries, ret=0x%p.", p->num_entries,
         pixel_return);
         */

        for (i=0; i<p->num_entries; i++)
        {
            palette[i].bRed   = p->entries[i].red;
            palette[i].bGreen = p->entries[i].green;
            palette[i].bBlue  = p->entries[i].blue;
        }

        rc=DiveSetSourcePalette(c->hDiveInst, 0, p->num_entries, (BYTE*)palette);

        if (rc)
            log_error(vidlog, "DiveSetSourcePalette (rc=0x%x).",rc);
        else
            log_message(vidlog, "%d palette entries realized.", p->num_entries);

        lib_free(palette);
    }

#if 0
    for (i=0; i<p->num_entries; i++)
        pixel_return[i] = i;
#endif

    if (c->bDepth==8)
        VideoConvertPalette8(c, p->num_entries);
    else
        VideoConvertPalette(c, p->num_entries, p->entries);

    if (c->bDepth!=8)
        WmPaint(c->hwndClient);

    return 0;
}

/* ------------------------------------------------------------------------ */
void video_canvas_refresh(video_canvas_t *c,
                          unsigned int xs, unsigned int ys,
                          unsigned int xi, unsigned int yi,
                          unsigned int w, unsigned int h)
{
    // if (DosRequestMutexSem(c->hmtx, SEM_IMMEDIATE_RETURN) || !c->hDiveInst)
    //     return;

    if (!c->hDiveInst)
        return;

    if (DosRequestMutexSem(c->hmtx, SEM_INDEFINITE_WAIT))
        return;

    DEBUG("CANVAS REFRESH 0");

    //
    // I tried to call DiveSetupBlitter onyl if the values
    // changed, but it doesn't speed up anything
    //
    if (c->vrenabled)
    {
        if (c->videoconfig->doublesizex)
        {
            xs *= 2;
            xi *= 2;
            w  *= 2;
        }

        if (c->videoconfig->doublesizey)
        {
            ys *= 2;
            yi *= 2;
            h  *= 2;
        }
        VideoCanvasBlit(c, xs, ys, xi, yi, w, h);
    }
    //else log_debug("drawing skipped");

    DosReleaseMutexSem(c->hmtx);

    DEBUG("CANVAS REFRESH 1");
};

void fullscreen_capability(cap_fullscreen_t *cap_fullscreen)
{
    cap_fullscreen->device_num = 0;
}
