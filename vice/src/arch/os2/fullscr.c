/*
 * fullscr.c - Fullscreen Video implementation for Vice/2, using DIVE.
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

#define INCL_WININPUT        // WinSetCapture
#define INCL_WINDIALOGS      // WinProcessDlg
#define INCL_WINPOINTERS     // WinShowPointer
#define INCL_WINWINDOWMGR    // QWL_USER
#define INCL_DOSMODULEMGR    // DosLoadModule

#include <os2.h>

#ifdef WATCOM_COMPILE
#define INCL_MMIOOS2
#else
#define INCL_MMIO
#endif

#include <os2me.h>

#if defined(__IBMC__) || defined(WATCOM_COMPILE)
#include "fullscros2.h"
#include <fourcc.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "log.h"

static log_t fslog = LOG_ERR;

static GDDMODEINFO desktopmode;         // List of all supported video modes
static BOOL fInFullScreenNow;           // Flag to show if the Dive and the Desktop is
                                        // in Fullscreen or windowed mode now
static HMODULE hmodVMAN=NULLHANDLE;     // Handle of loaded VMAN.DLL module
FNVMIENTRY *pfnVMIEntry;                // The entry of VMAN.DLL
static int vmiinit = 0;

static long NumVideoModes;             // Number of supported video modes
static PGDDMODEINFO ModeInfo;          // List of all supported video modes
static PGDDMODEINFO NewModeInfo;       // New video mode for fullscreen
static FOURCC fccColorEncoding;        // Mode to be used
static BOOL fFullScreenMode;           // Flag to show if the application is running in fullscreen mode or not.
                                       // Note, that it doesn't mean that the desktop is in fullscreen mode now,
                                       // because it only shows that when the application gets focus, it should
                                       // run in fullscreen mode (or not).
                                       // To check the actual mode, see fInFullscreenNow!

static int fRate = 50;

///////////////////////////////////////
// SetPointerVisibility
//
// Shows/hides the mouse pointer
//
int pfnShowPtr(BOOL fState)
{
    HWSHOWPTRIN hwspi;

    hwspi.ulLength = sizeof(hwspi);
    hwspi.fShow = fState;

    return pfnVMIEntry(0, VMI_CMD_SHOWPTR, &hwspi, NULL);
}

int BlockPM(HWND hwnd)
{
    //
    // Don't let other applications write to screen anymore!
    //
    WinLockWindowUpdate(HWND_DESKTOP, HWND_DESKTOP);

    WinSetCapture(HWND_DESKTOP, hwnd);

    //
    // Hide mouse pointer
    //
    WinShowPointer(hwnd, TRUE);
    pfnShowPtr(FALSE);

    //      rc = GreDeath(hdc);
    // This is the standard way to tell the graphical engine
    // that a switching to full-screen mode is about to come!
    // (see gradd.inf)

    return 1;
}

HWND QueryDesktopWindow(void)
{
    HAB  hab;
    HWND hwnd;

    hab = WinQueryAnchorBlock(HWND_DESKTOP);
    if (hab == NULLHANDLE) {
        log_error(fslog, "WinQueryAnchorBlock failed.");
        return NULLHANDLE;
    }

    hwnd = WinQueryDesktopWindow(hab, 0);
    if (hwnd == NULLHANDLE) {
        log_error(fslog, "WinQueryDesktopWindow failed.");
        return NULLHANDLE;
    }

    return hwnd;
}

///////////////////////////////////////
// RestorePM
//
// Simulates a switching back from fullscreen mode,
// from the PM's point of view
//
int RestorePM(void)
{
    HDC hdc;
    HWND hwnd;

    //
    // Show pointer again
    //
    pfnShowPtr(TRUE);
    WinSetCapture(HWND_DESKTOP, NULLHANDLE);

    //
    // Let others write to the screen again...
    //
    WinLockWindowUpdate(HWND_DESKTOP, 0);

    hwnd = QueryDesktopWindow();
    if (hwnd == NULLHANDLE) {
        return 0;
    }

    hdc = WinQueryWindowDC(hwnd);
    if (hwnd == NULLHANDLE) {
        log_error(fslog, "WinQueryWindowDC failed.");
        return 0;
    }

    fInFullScreenNow = FALSE;

    //
    // Let everyone redraw itself! (Build the screen)
    //
    WinInvalidateRect(hwnd, NULL, TRUE);

    /*--*///WinSetCapture(HWND_DESKTOP, hClient);

    //  rc = GreResurrection(hdc, 0, NULL);
    // This is the standard way of telling the graphical engine
    // that somebody has switched back from a fullscreen session to PM.

    return 1;
}

int pfnSetMode(long modeid)
{
    int rc;

    log_message(fslog, "Switching to Mode #%d", modeid&0xff);

    rc = pfnVMIEntry(0, VMI_CMD_SETMODE, &modeid, NULL); // Set old video mode
    if (rc != NO_ERROR) {
        log_error(fslog, "pfnVMIEntry VMI_CMD_SETMODE failed (rc=%d)", rc);
    }

    return rc;
}

void PrintModeInfo(GDDMODEINFO *mode)
{
    const int fcc = mode->fccColorEncoding;

    if (!mode) {
        return;
    }

    log_message(fslog, "Id=%4d, %4dx%4d/%2d (%3d Hz)  FourCC: %c%c%c%c (%d)", mode->ulModeId & 0xff, mode->ulHorizResolution,
                mode->ulVertResolution, mode->ulBpp, mode->ulRefreshRate, fcc, fcc>>8, fcc>>16, fcc>>24, mode->cColors);
}

///////////////////////////////////////
// FindNewModeInfo
//
// Searches for
// FULLSCREENWIDTH x FULLSCREENHEIGHT x FULLSCREENBPP
// videomode in ModeInfo structure, and
// sets the NewModeInfo pointer to point
// to that part of ModeInfo, if found.
//
GDDMODEINFO *FindVideoMode(int w, int h, FOURCC fcc, int r)
{
    long l;
    GDDMODEINFO *mode, *res;
    unsigned int maxw = -1;
    unsigned int maxh = -1;
    unsigned int rate = -1;

    NewModeInfo = NULL;

    if (!ModeInfo) {
        return FALSE;
    }

    mode = ModeInfo;

    for ( l = 0; l < NumVideoModes; mode++, l++) {
        if (mode->ulHorizResolution >= w && mode->ulHorizResolution < maxw && mode->ulVertResolution >= h && mode->ulVertResolution < maxh &&
            mode->ulRefreshRate >= r && mode->ulRefreshRate < rate && mode->fccColorEncoding == fcc) {
            maxw = mode->ulHorizResolution;
            maxh = mode->ulVertResolution;
            rate = mode->ulRefreshRate;
            res = mode;
        }
    }
    if (rate > 0) {
        return res;
    }

    rate = 0;
    maxw = -1;
    maxh = -1;

    mode = ModeInfo;

    for (l = 0; l < NumVideoModes; mode++, l++) {
        if (mode->ulHorizResolution >= w && mode->ulHorizResolution < maxw && mode->ulVertResolution >= h && mode->ulVertResolution < maxh &&
            mode->ulRefreshRate < r && mode->ulRefreshRate > rate && mode->fccColorEncoding == fcc) {
            maxw = mode->ulHorizResolution;
            maxh = mode->ulVertResolution;
            rate = mode->ulRefreshRate;
            res = mode;
        }
    }

    log_debug("Found %dx%d / %dHz", maxw, maxh, rate);

    if (rate > 0) {
        return res;
    }

    log_message(fslog, "Requested Video Mode not found (%dx%d, %c%c%c%c)", w, h, fcc, fcc >> 8, fcc >> 16, fcc >> 24);

    return NULL;
}

void FullscreenPrintModes(void)
{
    long l;
    GDDMODEINFO *mode = ModeInfo;

    log_message(fslog, "Available GRADD Video Modes: %d", NumVideoModes);

    if (!mode) {
        return;
    }

    for (l = 0; l < NumVideoModes; mode++, l++) {
        PrintModeInfo(mode);
    }
}

///////////////////////////////////////
// SwitchToFullscreen
//
// Switches to fullscreen-mode
//
int SwitchToFullscreen(HWND hwnd, GDDMODEINFO *mode)
{
    int rc;

    APERTURE     aperture;
    FBINFO       fbinfo;

    if (!hmodVMAN) {
        return -1;
    }

    log_message(fslog, "Switching to fullscreen.");

    //
    // Setup Aperture and FBINFO for FSDIVE
    //
    memset(&fbinfo, 0, sizeof(FBINFO));

    fbinfo.ulLength = sizeof(FBINFO);
    fbinfo.ulCaps = 0;
    fbinfo.ulBPP = mode->ulBpp;
    fbinfo.ulXRes = mode->ulHorizResolution;
    fbinfo.ulYRes = mode->ulVertResolution;
    fbinfo.ulScanLineBytes = mode->ulScanLineSize;
    fbinfo.ulNumENDIVEDrivers = 0; // unknown
    fbinfo.fccColorEncoding = mode->fccColorEncoding;

    aperture.ulPhysAddr = (long)mode->pbVRAMPhys;
    aperture.ulApertureSize = mode->ulApertureSize;
    aperture.ulScanLineSize = mode->ulScanLineSize;
    aperture.rctlScreen.yBottom = mode->ulVertResolution - 1;
    aperture.rctlScreen.xRight = mode->ulHorizResolution - 1;
    aperture.rctlScreen.yTop = 0;
    aperture.rctlScreen.xLeft = 0;

    fInFullScreenNow = TRUE; // in this fullscreen mode now!

    if (!BlockPM(hwnd)) {
        log_error(fslog, "Block PM update failed.");
        return -1;
    }

    //
    // Set new video mode
    //
    rc = pfnSetMode(mode->ulModeId);
    if (rc != NO_ERROR) {
        log_error(fslog, "Switching mode failed!");
        RestorePM();
        return rc;
    }

    DiveFullScreenInit(&aperture, &fbinfo); // Tell DIVE that it can work

    return NO_ERROR;
}

int SwitchIntoFullscreen(HWND hwnd)
{
    SWP swp;
    GDDMODEINFO *mode;
    FOURCC fcc = fccColorEncoding;

    WinQueryWindowPos(hwnd, &swp);

    log_message(fslog, "Search for best fullscreen mode %dx%dx%d (>=%dHz, %c%c%c%c):", swp.cx, swp.cy, 16, fRate, fcc, fcc >> 8, fcc >> 16, fcc >> 24);

    mode = FindVideoMode(swp.cx, swp.cy, fcc, fRate);
    if (!mode) {
        log_error(fslog, "No matching video mode found!");
        return FALSE;
    }

    PrintModeInfo(mode);
    return SwitchToFullscreen(hwnd, mode);
}

int FullscreenIsInFS()
{
    return fFullScreenMode && hmodVMAN;
}

int FullscreenIsNow()
{
    return fInFullScreenNow && hmodVMAN;
}


///////////////////////////////////////
// SwitchBackToDesktop
//
// Switches back from fullscreen-mode
//
int SwitchBackToDesktop(void)
{
    log_message(fslog, "Switching back to desktop...");

    DiveFullScreenTerm();
    pfnSetMode(desktopmode.ulModeId);

    RestorePM();

    return TRUE;
}

void FullscreenDisable(void)
{
    if (!fFullScreenMode) {
        return;
    }

    SwitchBackToDesktop();
    fFullScreenMode = fInFullScreenNow;
}

void FullscreenChangeMode(HWND hwnd)
{
    int rc;
    SWP swp;
    APERTURE aperture;
    FBINFO fbinfo;
    GDDMODEINFO *mode;
    FOURCC fcc = fccColorEncoding;

    if (!hmodVMAN || !fFullScreenMode) {
        return;
    }

    WinQueryWindowPos(hwnd, &swp);

    log_message(fslog, "Search for best fullscreen mode %dx%d (>=%dHz, %c%c%c%c):", swp.cx, swp.cy, fRate, fcc, fcc >> 8, fcc >> 16, fcc >> 24);

    mode = FindVideoMode(swp.cx, swp.cy, fccColorEncoding, fRate);
    if (!mode) {
        log_error(fslog, "No matching video mode found!");
        return;
    }
    PrintModeInfo(mode);

    //
    // Setup Aperture and FBINFO for FSDIVE
    //
    memset(&fbinfo, 0, sizeof(FBINFO));

    fbinfo.ulLength = sizeof(FBINFO);
    fbinfo.ulCaps = 0;
    fbinfo.ulBPP = mode->ulBpp;
    fbinfo.ulXRes = mode->ulHorizResolution;
    fbinfo.ulYRes = mode->ulVertResolution;
    fbinfo.ulScanLineBytes = mode->ulScanLineSize;
    fbinfo.ulNumENDIVEDrivers = 0; // unknown
    fbinfo.fccColorEncoding = mode->fccColorEncoding;

    aperture.ulPhysAddr = (long)mode->pbVRAMPhys;
    aperture.ulApertureSize = mode->ulApertureSize;
    aperture.ulScanLineSize = mode->ulScanLineSize;
    aperture.rctlScreen.yBottom = mode->ulVertResolution  - 1;
    aperture.rctlScreen.xRight = mode->ulHorizResolution - 1;
    aperture.rctlScreen.yTop = 0;
    aperture.rctlScreen.xLeft = 0;

    DiveFullScreenTerm();

    //
    // Set new video mode
    //
    rc = pfnSetMode(mode->ulModeId);
    if (rc != NO_ERROR) {
        log_error(fslog, "Switching mode failed!");
        pfnSetMode(desktopmode.ulModeId);
        RestorePM();
        return;
    }

    DiveFullScreenInit(&aperture, &fbinfo); // Tell DIVE that it can work

    WinSendMsg(hwnd, WM_PAINT, 0, 0);
}

void FullscreenChangeRate(HWND hwnd, int state)
{
    unsigned int rate;
    GDDMODEINFO *mode = ModeInfo;

    if (!hmodVMAN || !fFullScreenMode || !mode) {
        return;
    }

    if (state >= 0) {
        int l;

        for (rate = -1, l = 0; l < NumVideoModes; mode++, l++) {
            if (mode->ulRefreshRate > fRate && mode->ulRefreshRate < rate) {
                rate = mode->ulRefreshRate;
            }
        }
        if (rate == 0 || rate == -1) {
            log_message(fslog, "Coudn't find higher refresh rate");
            return;
        }
    } else {
        int l;

        for (rate = 0, l = 0; l < NumVideoModes; mode++, l++) {
            if (mode->ulRefreshRate < fRate && mode->ulRefreshRate > rate) {
                rate = mode->ulRefreshRate;
            }
        }
        if (rate == 0) {
            log_message(fslog, "Coudn't find lower refresh rate");
            return;
        }
    }

    fRate = rate;
    FullscreenChangeMode(hwnd);
}

int pfnQueryModes(long mode, void *data)
{
    int rc = pfnVMIEntry(0, VMI_CMD_QUERYMODES, &mode, data);

    if (rc == NO_ERROR) {
        return NO_ERROR;
    }

    log_error(fslog, "pfnVMIEntry(mode=%d) VMI_CMD_QUERYMODES (rc=%d)", mode, rc);
    return rc;
}

///////////////////////////////////////
// QueryCurrentMode
//
// Queries the current video mode, and
// stores it in DesktopModeInfo
//
int FullscreenQueryCurrentMode(GDDMODEINFO *mode)
{
    int rc = pfnVMIEntry(0, VMI_CMD_QUERYCURRENTMODE, NULL, mode);

    if (rc != NO_ERROR) {
        log_error(fslog, "pfnVMIEntry VMI_CMD_QUERYCURRENTMODE (rc=%d)", rc);
    }

    return rc;
}

void UninitModeInfo(void)
{
    lib_free(ModeInfo);
    ModeInfo = NULL;
    NumVideoModes = 0;
}

///////////////////////////////////////
// QueryModeInfo
//
// Queries the number of available video
// modes to NumVideoModes, allocates memory
// for information of all the video modes,
// and queries video mode informations into
// ModeInfo.
//
int InitModeInfo(void)
{
    // Query available video modes
    int rc = pfnQueryModes(QUERYMODE_NUM_MODES, &NumVideoModes);

    if (rc == NO_ERROR) {
        ModeInfo = (PGDDMODEINFO)calloc(NumVideoModes, sizeof(GDDMODEINFO));

        rc = pfnQueryModes(QUERYMODE_MODE_DATA, ModeInfo);
        if (rc == NO_ERROR) {
            return NO_ERROR;
        }

        log_error(fslog, "pfnVMIEntry VMI_CMD_QUERYMODES (rc=%d)", rc);
    }

    UninitModeInfo();

    return -1;
}

int FullscreenInit(FOURCC fcc)
{
    int rc;
    INITPROCOUT ipo;

    fInFullScreenNow = FALSE;

    fslog = log_open("Fullscreen");

    //
    // Load VMAN.DLL
    //
    log_message(fslog, "Loading vman.dll.");
    rc = DosLoadModule(NULL, 0, "VMAN", &hmodVMAN);
    if (rc != NO_ERROR) {
        // No VMAN.DLL... Maybe no GRADD driver installed???
        log_error(fslog, "DosLoadModule vman.dll failed (rc=%d)", rc);
        hmodVMAN = NULLHANDLE;
    }

    if (!hmodVMAN) {
        return -1;
    }

    log_message(fslog, "Query ProcAddr for 'VMIEntry'");
    rc = DosQueryProcAddr(hmodVMAN, 0, "VMIEntry", (PFN *)(&pfnVMIEntry)); // Query entry point address
    if (rc != NO_ERROR) {
        log_error(fslog, "DosQueryProcAddr VMIEntry failed (rc=%d)", rc);
        return -1;
    }

    // Send "Hi! New process is here!" info to VMAN
    log_message(fslog, "Connect Vice/2 to VMI");
    rc = pfnVMIEntry(0, VMI_CMD_INITPROC, NULL, (PVOID) &ipo);
    if (rc != NO_ERROR) {
        log_error(fslog, "pfnVMIEntry VMI_CMD_INITPROC failed (rc=%d)", rc);
        return -1;
    }
    vmiinit = 1;

    rc = InitModeInfo();
    if (rc != NO_ERROR) {
        return -1;
    }

    rc = FullscreenQueryCurrentMode(&desktopmode);
    if (rc != NO_ERROR) {
        return -1;
    }

    log_message(fslog, "Fullscreen Initialized.");
    log_message(fslog, "Desktop Video Mode:");
    PrintModeInfo(&desktopmode);

    fccColorEncoding = fcc;

    return NO_ERROR;
}

int FullscreenFree(void)
{
    int rc;

    UninitModeInfo();

    if (vmiinit) {
        log_message(fslog, "Destroy VMI Entry.");
        rc = pfnVMIEntry(0, VMI_CMD_TERMPROC, NULL, NULL);
        if (rc != NO_ERROR) {
            log_error(fslog, "pfnVMIEntry VMI_CMD_TERMPROC failed (rc=%d)", rc);
        }
        vmiinit = 0;
    }
    if (hmodVMAN) {
        log_message(fslog, "Unload vman.dll.");
        rc = DosFreeModule(hmodVMAN);
        if (rc != NO_ERROR) {
            log_error(fslog, "DosFreeModule vman.dll failed (rc=%d)", rc);
        }
        hmodVMAN = NULLHANDLE;
    }
    return 1;
}

int FullscreenSwitch(HWND hwnd)
{
    if (!hmodVMAN) {
        return 0;
    }

    WinSetVisibleRegionNotify(hwnd, FALSE);
    WinSendMsg(hwnd, WM_VRNDISABLED, 0, 0);

    if (fFullScreenMode) {
        SwitchBackToDesktop();
    } else {
        SwitchIntoFullscreen(hwnd);
    }

    fFullScreenMode = fInFullScreenNow;

    WinSetVisibleRegionNotify(hwnd, TRUE);
    WinSendMsg(hwnd, WM_PAINT, 0, 0);

    return fFullScreenMode;
}

void FullscreenDeactivate(hwnd)
{
    if (!fInFullScreenNow || !hmodVMAN) {
        return;
    }

    WinShowWindow(WinQueryWindow(hwnd, QW_PARENT), FALSE);     // hide window
    WinSendMsg(hwnd, WM_VRNDISABLED, 0, 0);
    WinSetVisibleRegionNotify(hwnd, FALSE); // turn VRN off
    SwitchBackToDesktop();
}

int FullscreenActivate(HWND hwnd)
{
    if (!fFullScreenMode || !hmodVMAN) {
        return 0;
    }

    SwitchIntoFullscreen(hwnd);         // and do switching!
    WinShowWindow(WinQueryWindow(hwnd, QW_PARENT), TRUE);       // make window visible
    WinSetVisibleRegionNotify(hwnd, TRUE);  // turn on VRN
    WinSendMsg(hwnd, WM_VRNENABLED, 0, 0);
    WinSendMsg(hwnd, WM_PAINT, 0, 0);
    return 1;
}

int FullscreenIsInMode(void)
{
    return fFullScreenMode == fInFullScreenNow || !hmodVMAN;
}

int FullscreenQueryHorzRes(void)
{
    GDDMODEINFO mode;

    FullscreenQueryCurrentMode(&mode);
    return mode.ulHorizResolution;
}

int FullscreenQueryVertRes(void)
{
    GDDMODEINFO mode;
    FullscreenQueryCurrentMode(&mode);
    return mode.ulVertResolution;
}

// ------------------------- Vice specific ---------------------------
#include "lib.h"
#include "dialogs.h"

void video_show_modes(HWND hwnd)
{
    long l;
    GDDMODEINFO *mode = ModeInfo;

    if (!hmodVMAN) {
        return;
    }

    //
    // open dialog
    //
    hwnd = fsmodes_dialog(hwnd);

    if (!hwnd) {
        return;
    }

    //
    // fill dialog with text
    //

    log_message(fslog, "Available GRADD Video Modes: %d", NumVideoModes);

    if (!mode) {
        return;
    }

    for (l = 0; l < NumVideoModes; mode++, l++) {
        const int fcc = mode->fccColorEncoding;
        char *txt = lib_msprintf("Id=%4d, %4dx%4d/%2d (%3d Hz)  FourCC: %c%c%c%c (%d)", 
                                 mode->ulModeId & 0xff, mode->ulHorizResolution, mode->ulVertResolution,
                                 mode->ulBpp, mode->ulRefreshRate, fcc, fcc >> 8, fcc >> 16, fcc >> 24, mode->cColors);
        WinSendMsg(hwnd, WM_INSERT, txt, (void*)TRUE);
        lib_free(txt);
    }

    //
    // MAINLOOP
    //
    WinProcessDlg(hwnd);

    //
    // WinProcessDlg() does NOT destroy the window on return! Do it here,
    // otherwise the window procedure won't ever get a WM_DESTROY,
    // which we may want :-)
    //
    WinDestroyWindow(hwnd);
}
