/*
 * fullscrn-dx9.c - Fullscreen using DirectX9 for Win32
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include <windows.h>

#include "fullscrn.h"
#include "lib.h"
#include "log.h"
#include "statusbar.h"
#include "ui.h"
#include "videoarch.h"

#ifdef HAVE_D3D9_H

#include <d3d9.h>


static HMENU   old_menu;
static RECT    old_rect;
static DWORD   old_style;
static int     old_width;
static int     old_height;
static int     old_bitdepth;
static float   old_refreshrate;


void fullscreen_getmodes_dx9(void)
{
    int adapter, numAdapter, mode, numAdapterModes;
    D3DADAPTER_IDENTIFIER9 d3didentifier;
    D3DDISPLAYMODE displayMode;
    DirectDrawDeviceList *new_device;
    DirectDrawDeviceList *search_device;
    DirectDrawModeList *new_mode;
    DirectDrawModeList *search_mode;

    numAdapter = 0;
    while (D3D_OK == IDirect3D9_GetAdapterIdentifier(d3d, numAdapter, 0, &d3didentifier))
    {
        new_device = lib_malloc(sizeof(DirectDrawDeviceList));
        new_device->next = NULL;
        new_device->desc = lib_stralloc(d3didentifier.Description);

        if (devices == NULL) {
            devices = new_device;
        } else {
            search_device = devices;
            while (search_device->next != NULL) {
                search_device = search_device->next;
            }
            search_device->next = new_device;
        }
        numAdapter++;
    }
    
    for (adapter = 0; adapter < numAdapter; adapter++)
    {
        numAdapterModes = IDirect3D9_GetAdapterModeCount(d3d, adapter, D3DFMT_X8R8G8B8);
    
        for (mode = 0; mode < numAdapterModes; mode++) {
            if (S_OK == IDirect3D9_EnumAdapterModes(d3d, adapter, 
                                                    D3DFMT_X8R8G8B8, mode, &displayMode))
            {
                new_mode = lib_malloc(sizeof(DirectDrawModeList));
                new_mode->next = NULL;
                new_mode->devicenumber = adapter;
                new_mode->width = displayMode.Width;
                new_mode->height = displayMode.Height;
                new_mode->bitdepth = 32;
                new_mode->refreshrate = displayMode.RefreshRate;

                if (modes == NULL) {
                    modes = new_mode;
                } else {
                    search_mode = modes;
                    while (search_mode->next != NULL) {
                        search_mode = search_mode->next;
                    }
                    search_mode->next = new_mode;
                }
            }
        }
    }
}


void SwitchToFullscreenModeDx9(HWND hwnd)
{
    video_canvas_t *c;

    fullscreen_transition = 1;

    c = video_canvas_for_hwnd(hwnd);

    video_device_release_dx9(c);

    statusbar_destroy(hwnd);

    /*  Remove Window stuff that prevents fullscreen display */
    old_style = GetWindowLong(hwnd, GWL_STYLE);
    GetWindowRect(hwnd, &old_rect);
    SetWindowLong(hwnd, GWL_STYLE, old_style & ~WS_SYSMENU & ~WS_CAPTION);
    old_menu = GetMenu(hwnd);
    SetMenu(hwnd, NULL);
    ShowCursor(FALSE);

    ui_set_render_window(c, 1);
    video_device_create_dx9(c, 1);
	video_canvas_refresh_all(c);

    fullscreen_active = 1;
    fullscreen_transition = 0;
}


void SwitchToWindowedModeDx9(HWND hwnd)
{
    video_canvas_t *c;

    fullscreen_transition = 1;

    c = video_canvas_for_hwnd(hwnd);

    video_device_release_dx9(c);

    /* Create statusbar here to get correct dimensions for client window */
    statusbar_create(hwnd);
    ui_set_render_window(c, 0);

    LockWindowUpdate(hwnd);
    SetWindowLong(hwnd, GWL_STYLE, old_style);
    /* Restore  Menu */
    SetMenu(hwnd,old_menu);
    SetWindowPos(hwnd, HWND_NOTOPMOST, old_rect.left, old_rect.top,
                 old_rect.right - old_rect.left, old_rect.bottom - old_rect.top,
                 SWP_NOCOPYBITS);
    ShowCursor(TRUE);
    LockWindowUpdate(NULL);

    video_device_create_dx9(c, 0);
	video_canvas_refresh_all(c);

    fullscreen_transition = 0;
    c->refreshrate = old_refreshrate;
}


void fullscreen_get_current_display_dx9(int *bitdepth, int *width,
                                          int *height, int *refreshrate)
{
    D3DDISPLAYMODE mode;

    if (S_OK == IDirect3D9_GetAdapterDisplayMode(
                            d3d, D3DADAPTER_DEFAULT , &mode))
    {
        *bitdepth = 32;
        *width = mode.Width;
        *height = mode.Height;
        *refreshrate = mode.RefreshRate;
    } else {
        /* provide defaults if GetDisplayMode fails for some reason */
        log_debug("fullscreen_get_current_display_dx9 failed to get mode!");
        *bitdepth = 32;
        *width = 640;
        *height = 480;
        *refreshrate = 0;
    }
}

#else

/* some dummies for compilation without DirectX9 header */

void fullscreen_getmodes_dx9(void)
{
}

void fullscreen_get_current_display_dx9(int *bitdepth, int *width,
                                          int *height, int *refreshrate)
{
}

void SwitchToFullscreenModeDx9(HWND hwnd)
{
}

void SwitchToWindowedModeDx9(HWND hwnd)
{
}

#endif /* HAVE_D3D9_H */
