/*
 * fullscrn-ddraw.c - Fullscreen related support functions for Win32
 *
 * Written by
 *  Tibor Biczo <crown@matavnet.hu>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <ddraw.h>
#include <mmsystem.h>
#include <prsht.h>

#include "fullscrn.h"
#include "intl.h"
#include "lib.h"
#include "log.h"
#include "palette.h"
#include "res.h"
#include "resources.h"
#include "translate.h"
#include "ui.h"
#include "videoarch.h"
#include "winmain.h"
#include "statusbar.h"


extern void init_palette(const palette_t *p, PALETTEENTRY *ape);

// ----------------------------------------------

#ifndef HAVE_GUIDLIB
const GUID IID_IDirectDraw2 = { 0xB3A6F3E0, 0x2B43, 0x11CF,
                              { 0xA2, 0xDE, 0x00, 0xAA, 0x00, 0xB9, 0x33, 0x56}
                              };
#endif

static LPDIRECTDRAW DirectDrawObject;
static LPDIRECTDRAW2 DirectDrawObject2;
static float fullscreen_refreshrate_buffer = -1.0f;


#define CHECK_DDRESULT(ddresult) \
{                                \
    if (ddresult != DD_OK) {     \
        ui_error(translate_text(IDS_DIRECTDRAW_ERROR), ddresult, \
        dd_error(ddresult));     \
    }                            \
}

static BOOL WINAPI DDEnumCallbackFunction(GUID FAR *lpGUID,
                                          LPSTR lpDriverDescription,
                                          LPSTR lpDriverName, LPVOID lpContext)
{
    DirectDrawDeviceList *new_device;
    DirectDrawDeviceList *search_device;

    new_device = lib_malloc(sizeof(DirectDrawDeviceList));
    new_device->next = NULL;

    if (lpGUID != NULL) {
        memcpy(&new_device->guid, lpGUID, sizeof(GUID));
        new_device->isNullGUID = 0;
    } else {
        new_device->isNullGUID = 1;
    }

    new_device->desc = lib_stralloc(lpDriverDescription);

    if (devices == NULL) {
        devices = new_device;
    } else {
        search_device = devices;
        while (search_device->next != NULL) {
            search_device = search_device->next;
        }
        search_device->next = new_device;
    }

/*
    log_debug("--------- DirectDraw Device ---");
    log_debug("GUID: %16x",lpGUID);
    log_debug("Desc: %s",lpDriverDescription);
    log_debug("Name: %s",lpDriverName);
*/
    return DDENUMRET_OK;
}
 
static HRESULT WINAPI ModeCallBack(LPDDSURFACEDESC desc, LPVOID context)
{
    DirectDrawModeList *new_mode;
    DirectDrawModeList *search_mode;

    new_mode=lib_malloc(sizeof(DirectDrawModeList));
    new_mode->next = NULL;
    new_mode->devicenumber = *(int *)context;
    new_mode->width = new_mode->height = new_mode->bitdepth
        = new_mode->refreshrate=0;

    if (desc->dwFlags & (DDSD_WIDTH)) {
//        log_debug("Width:       %d", desc->dwWidth);
        new_mode->width=desc->dwWidth;
    }

    if (desc->dwFlags & (DDSD_HEIGHT)) {
//        log_debug("Height:      %d", desc->dwHeight);
        new_mode->height = desc->dwHeight;
    }

    if (desc->dwFlags & (DDSD_PIXELFORMAT)) {
//        log_debug("Bitdepth:    %d", desc->ddpfPixelFormat.dwRGBBitCount);
//        log_debug("Red mask:    %04x", desc->ddpfPixelFormat.dwRBitMask);
//        log_debug("Blue mask:   %04x", desc->ddpfPixelFormat.dwBBitMask);
//        log_debug("Green mask:  %04x", desc->ddpfPixelFormat.dwGBitMask);
#ifdef _ANONYMOUS_UNION
        new_mode->bitdepth = desc->ddpfPixelFormat.dwRGBBitCount;
#else
        new_mode->bitdepth = desc->ddpfPixelFormat.u1.dwRGBBitCount;
#endif
    }

    if (desc->dwFlags & (DDSD_REFRESHRATE)) {
//        log_debug("Refreshrate: %d", desc->dwRefreshRate);
#ifdef _ANONYMOUS_UNION
        new_mode->refreshrate = desc->dwRefreshRate;
#else
        new_mode->refreshrate = desc->u2.dwRefreshRate;
#endif
    }
    if (modes == NULL) {
        modes = new_mode;
    } else {
        search_mode = modes;
        while (search_mode->next != NULL) {
            search_mode = search_mode->next;
        }
        search_mode->next = new_mode;
    }

    return DDENUMRET_OK;
}


void fullscreen_getmodes_ddraw(void)
{
    HRESULT ddresult;
    DirectDrawDeviceList *search_device;
    int i;

    /*  Enumerate DirectDraw Devices */
    ddresult = DirectDrawEnumerate(DDEnumCallbackFunction, NULL);

    /*  List all available modes for all available devices */
    search_device = devices;
    i = 0;
    while (search_device != NULL) {
//        log_debug("--- Video modes for device %s", search_device->desc);
//        log_debug("MODEPROBE_Create");
        if (search_device->isNullGUID) {
            ddresult = DirectDrawCreate(NULL, &DirectDrawObject, NULL);
        } else {
            ddresult = DirectDrawCreate(&search_device->guid,
                                        &DirectDrawObject, NULL);
        }
        CHECK_DDRESULT(ddresult);
//        log_debug("MODEPROBE_SetCooperativeLevel");
        ddresult = IDirectDraw_SetCooperativeLevel(DirectDrawObject,
                                                   ui_get_main_hwnd(),
                                                   DDSCL_EXCLUSIVE
                                                   | DDSCL_FULLSCREEN);
        CHECK_DDRESULT(ddresult);
//        log_debug("MODEPROBE_ObtainDirectDraw2");
        ddresult = IDirectDraw_QueryInterface(DirectDrawObject,
                                              (GUID *)&IID_IDirectDraw2,
                                              (LPVOID *)&DirectDrawObject2);
        CHECK_DDRESULT(ddresult);
//        log_debug("MODEPROBE_EnumDisplayModes");
        ddresult = IDirectDraw2_EnumDisplayModes(DirectDrawObject2,
                                                 DDEDM_REFRESHRATES, NULL, &i,
                                                 ModeCallBack);
        CHECK_DDRESULT(ddresult);
        IDirectDraw2_Release(DirectDrawObject2);
        DirectDrawObject2 = NULL;
        IDirectDraw_Release(DirectDrawObject);
        DirectDrawObject = NULL;
        search_device = search_device->next;
        i++;
    }

    /*  This is here because some Matrox G200 drivers don't leave the window
        in its previous state */
    ShowWindow(ui_get_main_hwnd(), SW_HIDE);
}


static GUID *GetGUIDForActualDevice()
{
    int device;
    DirectDrawDeviceList *search_device;

    resources_get_int("FullscreenDevice", &device);
    search_device = devices;
    while (search_device != NULL) {
        if (device == 0) {
            if (search_device->isNullGUID) {
                return NULL;
            } else {
                return (&search_device->guid);
            }
        }
        device--;
        search_device = search_device->next;
    }
    return NULL;
}


static HMENU   old_menu;
static RECT    old_rect;
static DWORD   old_style;
static int     old_width;
static int     old_height;
static int     old_bitdepth;
static int     old_client_width;
static int     old_client_height;
static float   old_refreshrate;

int fullscreen_active;
int fullscreen_transition = 0;


void SwitchToFullscreenModeDDraw(HWND hwnd)
{
    int w, h, wnow, hnow;
    int fullscreen_width;
    int fullscreen_height;
    int bitdepth;
    int refreshrate;
    video_canvas_t *c;
    HRESULT ddresult;
    DDSURFACEDESC desc2;
    GUID *device_guid;
    int i;
    HDC hdc;

    fullscreen_transition = 1;
    //  Get fullscreen parameters
    GetCurrentModeParameters(&fullscreen_width, &fullscreen_height, &bitdepth,
                             &refreshrate);
    //  Get the Canvas for this window
    c = video_canvas_for_hwnd(hwnd);

    memset(&desc2, 0, sizeof(desc2));
    desc2.dwSize = sizeof(desc2);
    ddresult = IDirectDraw2_GetDisplayMode(c->dd_object2,&desc2);
    old_width = desc2.dwWidth;
    old_height = desc2.dwHeight;
#ifdef _ANONYMOUS_UNION
    old_bitdepth = desc2.ddpfPixelFormat.dwRGBBitCount;;
#else
    old_bitdepth = desc2.ddpfPixelFormat.u1.dwRGBBitCount;;
#endif
    old_refreshrate = c->refreshrate; /* save this, because recalculating takes time */

    IDirectDrawSurface_Release(c->temporary_surface);
    IDirectDrawSurface_Release(c->primary_surface);
    IDirectDraw_Release(c->dd_object2);
    IDirectDraw_Release(c->dd_object);

    statusbar_destroy(hwnd);

    //  Remove Window Styles
    old_style = GetWindowLong(hwnd, GWL_STYLE);
    GetWindowRect(hwnd, &old_rect);
    SetWindowLong(hwnd, GWL_STYLE, old_style & ~WS_SYSMENU & ~WS_CAPTION);
    //  Remove Menu
    old_menu = GetMenu(hwnd);
    SetMenu(hwnd, NULL);
    //  Cover screen with window
    wnow = GetSystemMetrics(SM_CXSCREEN);
    hnow = GetSystemMetrics(SM_CYSCREEN);
    w = (fullscreen_width > wnow) ? fullscreen_width : wnow;
    h = (fullscreen_height > hnow) ? fullscreen_height : hnow;
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, w, h, SWP_NOCOPYBITS);
    ShowCursor(FALSE);

    device_guid = GetGUIDForActualDevice();
    ddresult = DirectDrawCreate(device_guid, &c->dd_object, NULL);
    ddresult = IDirectDraw_SetCooperativeLevel(c->dd_object, c->hwnd,
                                               DDSCL_EXCLUSIVE
                                               | DDSCL_FULLSCREEN);
    ddresult = IDirectDraw_QueryInterface(c->dd_object,
                                          (GUID *)&IID_IDirectDraw2,
                                          (LPVOID *)&c->dd_object2);

    //  Set cooperative level
    ddresult = IDirectDraw_SetCooperativeLevel(c->dd_object, c->hwnd,
                                               DDSCL_EXCLUSIVE
                                               | DDSCL_FULLSCREEN);
    //  Set Mode
    ddresult = IDirectDraw2_SetDisplayMode(c->dd_object2, fullscreen_width,
                                           fullscreen_height, bitdepth,
                                           refreshrate,0);
    //  Adjust window size
    old_client_width = c->client_width;
    old_client_height = c->client_height;
    c->client_width = fullscreen_width;
    c->client_height = fullscreen_height;

    if (fullscreen_refreshrate_buffer < 0.0f) {
        /* if no refreshrate is buffered, recalculate (1 second) */
        for (i = 0; i < 50; i++)
            IDirectDraw2_WaitForVerticalBlank(c->dd_object2,
                                              DDWAITVB_BLOCKBEGIN, 0);
        c->refreshrate = video_refresh_rate(c);
        fullscreen_refreshrate_buffer = c->refreshrate;
    } else {
        c->refreshrate = fullscreen_refreshrate_buffer;
    }

    video_create_single_surface(c, fullscreen_width, fullscreen_height);

    c->depth = bitdepth;

    /* Create palette.  */
    if (c->depth == 8) {
        PALETTEENTRY ape[256];
        HRESULT result;

        init_palette(c->palette, ape);

        result = IDirectDraw2_CreatePalette(c->dd_object2, DDPCAPS_8BIT,
                                            ape, &c->dd_palette, NULL);
        if (result != DD_OK) {
        }
    }

    video_set_palette(c);
    video_set_physical_colors(c);

    IDirectDrawSurface_GetDC(c->primary_surface, &hdc);
    video_canvas_update(c->hwnd, hdc, 0, 0, fullscreen_width,
                        fullscreen_height);
    IDirectDrawSurface_ReleaseDC(c->primary_surface, hdc);
    fullscreen_active = 1;

    fullscreen_transition = 0;
}

void SwitchToWindowedModeDDraw(HWND hwnd)
{
    video_canvas_t *c;
    HRESULT ddresult;
    /*DDSURFACEDESC desc;*/
    DDSURFACEDESC desc2;
    HDC hdc;

    fullscreen_transition = 1;

    //  Get the Canvas for this window
    c = video_canvas_for_hwnd(hwnd);

    IDirectDrawSurface_Release(c->temporary_surface);
    IDirectDrawSurface_Release(c->primary_surface);
    ddresult = IDirectDraw_SetCooperativeLevel(c->dd_object, NULL,
                                               DDSCL_NORMAL);
    IDirectDraw_RestoreDisplayMode(c->dd_object);
    IDirectDraw_Release(c->dd_object2);
    IDirectDraw_Release(c->dd_object);

    LockWindowUpdate(hwnd);
    SetWindowLong(hwnd, GWL_STYLE, old_style);
    //  Restore Menu
    SetMenu(hwnd,old_menu);
    SetWindowPos(hwnd, HWND_TOP, old_rect.left, old_rect.top,
                 old_rect.right - old_rect.left, old_rect.bottom - old_rect.top,
                 SWP_NOCOPYBITS);
    ShowCursor(TRUE);
    c->client_width = old_client_width;
    c->client_height = old_client_height;
    LockWindowUpdate(NULL);

    statusbar_create(hwnd);

    ddresult = DirectDrawCreate(NULL, &c->dd_object, NULL);
    ddresult = IDirectDraw_SetCooperativeLevel(c->dd_object, NULL,
                                               DDSCL_NORMAL);
    ddresult = IDirectDraw_QueryInterface(c->dd_object,
                                          (GUID *)&IID_IDirectDraw2,
                                          (LPVOID *)&c->dd_object2);

    memset(&desc2,0,sizeof(desc2));
    desc2.dwSize = sizeof(desc2);
    ddresult = IDirectDraw2_GetDisplayMode(c->dd_object2, &desc2);

    video_create_single_surface(c, desc2.dwWidth, desc2.dwHeight);

    c->depth = old_bitdepth;


    /* Create palette.  */
    if (c->depth == 8) {
        PALETTEENTRY ape[256];
        HRESULT result;

        init_palette(c->palette, ape);

        result = IDirectDraw2_CreatePalette(c->dd_object2, DDPCAPS_8BIT,
                                            ape, &c->dd_palette, NULL);
        if (result != DD_OK) {
        }
    }

    video_set_palette(c);
    video_set_physical_colors(c);


    IDirectDrawSurface_GetDC(c->primary_surface, &hdc);
    video_canvas_update(c->hwnd, hdc, 0, 0, c->client_width, c->client_height);
    IDirectDrawSurface_ReleaseDC(c->primary_surface, hdc);
    fullscreen_active = 0;

    fullscreen_transition = 0;

    c->refreshrate = old_refreshrate;
}


void fullscreen_get_current_display_ddraw(int *bitdepth, int *width,
                                          int *height, int *refreshrate)
{
    /* Provide standard values for now (as the old code did it) */
    *bitdepth = 8;
    *width = 640;
    *height = 480;
    *refreshrate = 0;
}


void fullscrn_invalidate_refreshrate(void)
{
    fullscreen_refreshrate_buffer = -1.0f;
}

