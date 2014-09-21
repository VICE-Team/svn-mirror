/*
 * mousedrv.c - Mouse handling for OS/2
 *
 * Written by
 *  Thomas Bretz <tbretz@uni-sw.gwdg.de>
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "vice.h"

#define INCL_WININPUT
#define INCL_WINPOINTERS

#include "mouse.h"
#include "mousedrv.h"
#include "fullscr.h"
#include "cmdline.h"
#include "resources.h"
#include "translate.h"
#include "vsyncapi.h"

static int hide_mouseptr;
static int visible=TRUE;
static SHORT _mouse_x, _mouse_y; // [-32768, 32768]
static unsigned long mouse_timestamp = 0;

/* ----------------------------------------------------------- */

void mousedrv_mouse_changed(void)
{
    /* -- FIXME: rash while startup --
     mouse_button_left(0);
     mouse_button_right(0);
     */
}

static int set_hide_mouseptr(int val, void *param)
{
    hide_mouseptr = val ? 1 : 0;

    if (!hide_mouseptr && !visible && !FullscreenIsNow()) { // do we have to show the ptr again?
        WinSetCapture(HWND_DESKTOP, NULLHANDLE);
        WinShowPointer(HWND_DESKTOP, TRUE);
        visible = TRUE;
    }
    return 0;
}

static const resource_int_t resources_int[] = {
    { "HideMousePtr", 0, RES_EVENT_NO, NULL,
      &hide_mouseptr, set_hide_mouseptr, NULL },
    { NULL }
};

int mousedrv_resources_init(void)
{
    return resources_register_int(resources_int);
}

/* ----------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] = {
    { "-hidemouseptr", SET_RESOURCE, 0,
      NULL, NULL, "HideMousePtr", (resource_value_t) 1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Enable hiding of mouse pointer inside the window" },
    { "+hidemouseptr", SET_RESOURCE, 0,
      NULL, NULL, "HideMousePtr", (resource_value_t) 0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Disable hiding of mouse pointer inside the window" },
    { NULL }
};

int mousedrv_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

void mousedrv_init(void)
{
}

const int mouse_step = 15;

extern int stretch;  // video.c

inline int mousedrv_get_x(void)
{
#ifndef __XVIC__
    return (_mouse_x / stretch) << 1;
#else
    return _mouse_x / stretch;
#endif
}

inline int mousedrv_get_y(void)
{
    return (_mouse_y / stretch) << 1;
}

unsigned long mousedrv_get_timestamp(void)
{
    return mouse_timestamp;
}

/* ----------------- OS/2 specific ------------------------- */

void mouse_button(HWND hwnd, ULONG msg, MPARAM mp1)
{
    if (!_mouse_enabled) {
        return;
    }

    switch (msg) {
        case WM_MOUSEMOVE:
            _mouse_x = SHORT1FROMMP(mp1);
            _mouse_y = SHORT2FROMMP(mp1);
            mouse_timestamp = vsyncarch_gettime();
            {
                SWP swp;

                WinQueryWindowPos(hwnd, &swp);
                //
                // check whether the pointer is outside or inside the window
                //

                if (FullscreenIsNow()) {
                    visible = TRUE;
                }

                if (_mouse_x >= 0 && _mouse_x < swp.cx && _mouse_y >= 0 && _mouse_y < swp.cy) {
                    //
                    // FIXME: Don't capture the mouse pointer if it is in front
                    // of a client dialog!
                    //
                    if (WinQueryCapture(HWND_DESKTOP)!= hwnd && hide_mouseptr && !FullscreenIsNow()) {
                        WinSetCapture(HWND_DESKTOP, hwnd);
                    }

                    if (visible && hide_mouseptr && !FullscreenIsNow()) {
                        WinShowPointer(HWND_DESKTOP, FALSE);
                        visible = FALSE;
                    }
                } else {
                    if (WinQueryCapture(HWND_DESKTOP) == hwnd && !FullscreenIsNow()) {
                        WinSetCapture(HWND_DESKTOP, NULLHANDLE);
                    }

                    if (!visible && !FullscreenIsNow()) {
                        WinShowPointer(HWND_DESKTOP, TRUE);
                        visible = TRUE;
                    }

                    //
                    // don't use 'outside'-values which appears one times
                    // if the mouse pointer leaves the window
                    //
                    if (_mouse_x < 0) {
                        _mouse_x = 0;
                    } else {
                        if (_mouse_x >= swp.cx) {
                            _mouse_x = swp.cx - 1;
                        }
                    }

                    if (_mouse_y < 0) {
                       _mouse_y = 0;
                    } else {
                        if (_mouse_y >= swp.cy) {
                            _mouse_y = swp.cy - 1;
                        }
                    }
                }
            }
            return;
        case WM_BUTTON1DOWN:
            mouse_button_left(1);
            return;
        case WM_BUTTON1UP:
            mouse_button_left(0);
            return;
        case WM_BUTTON2DOWN:
            mouse_button_right(1);
            return;
        case WM_BUTTON2UP:
            mouse_button_right(0);
            return;
        case WM_BUTTON3DOWN:
            mouse_button_middle(1);
            return;
        case WM_BUTTON3UP:
            mouse_button_middle(0);
            return;
    }
}
