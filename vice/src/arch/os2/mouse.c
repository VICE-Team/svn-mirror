/*
 * mouse.c - Mouse handling for OS/2
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

#define INCL_WININPUT
#define INCL_WINPOINTERS

#include "mouse.h"
#include "cmdline.h"
#include "keyboard.h"
#include "resources.h"

int _mouse_enabled;
static int hide_mouseptr;
static int visible=TRUE;
static SHORT _mouse_x, _mouse_y; // -32768-32768

/* ----------------------------------------------------------- */

static int set_mouse_enabled(resource_value_t v, void *param)
{
    _mouse_enabled = (int) v;
    if (!_mouse_enabled)
        joystick_value[1] &= ~1 & ~16;
    return 0;
}

static int set_hide_mouseptr(resource_value_t v, void *param)
{
    hide_mouseptr = (int) v;
    if (!hide_mouseptr && !visible)
    { // do we have to show the ptr again?
        WinSetCapture(HWND_DESKTOP, NULLHANDLE);
        WinShowPointer(HWND_DESKTOP, TRUE);
        visible=TRUE;
    }
    return 0;
}

static resource_t resources[] = {
    { "Mouse", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &_mouse_enabled, set_mouse_enabled, NULL },
    { "HideMousePtr", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &hide_mouseptr, set_hide_mouseptr, NULL },
    { NULL }
};

int mouse_init_resources(void)
{
    return resources_register(resources);
}

/* ----------------------------------------------------------- */

static cmdline_option_t cmdline_options[] = {
    { "-mouse", SET_RESOURCE, 0, NULL, NULL,
      "Mouse", (resource_value_t) 1, NULL,
      "Enable emulation of the 1351 proportional mouse" },
    { "+mouse", SET_RESOURCE, 0, NULL, NULL,
      "Mouse", (resource_value_t) 0, NULL,
      "Disable emulation of the 1351 proportional mouse" },
    { "-hidemouseptr", SET_RESOURCE, 0, NULL, NULL,
      "HideMousePtr", (resource_value_t) 1, NULL,
      "Enable hiding of mouse pointer inside the window" },
    { "+hidemouseptr", SET_RESOURCE, 0, NULL, NULL,
      "HideMousePtr", (resource_value_t) 0, NULL,
      "Disable hiding of mouse pointer inside the window" },
    { NULL }
};

int mouse_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

void mouse_init(void)
{
}

const int mouse_step  = 15;

extern int stretch;  // video.c

inline BYTE mouse_get_x(void)
{
    static SHORT last_mouse_x=0;

    if (last_mouse_x - _mouse_x > mouse_step)
        last_mouse_x -= mouse_step;
    else
        if(_mouse_x - last_mouse_x > mouse_step)
            last_mouse_x += mouse_step;
        else
            last_mouse_x = _mouse_x;

    return ((last_mouse_x<<1)/stretch) & 0x7e;
}

inline BYTE mouse_get_y(void)
{
    static SHORT last_mouse_y=0;

    if (last_mouse_y - _mouse_y > mouse_step)
        last_mouse_y -= mouse_step;
    else
        if(_mouse_y - last_mouse_y > mouse_step)
            last_mouse_y += mouse_step;
        else
            last_mouse_y = _mouse_y;

    return ((last_mouse_y<<1)/stretch) & 0x7e;
}

/* ----------------- OS/2 specific ------------------------- */

void mouse_button(HWND hwnd, ULONG msg, MPARAM mp1)
{
    switch (msg)
    {
    case WM_MOUSEMOVE:
        _mouse_x = SHORT1FROMMP(mp1);
        _mouse_y = SHORT2FROMMP(mp1);
        {
            SWP swp;
            WinQueryWindowPos(hwnd, &swp);
            if (_mouse_x>=0 && _mouse_x<swp.cx &&
                _mouse_y>=0 && _mouse_y<swp.cy)
            { // pointer is inside the window
                if (visible && _mouse_enabled && hide_mouseptr)
                {
                    WinSetCapture(HWND_DESKTOP, hwnd);
                    WinShowPointer(HWND_DESKTOP, FALSE);
                    visible=FALSE;
                }
            }
            else
            { // pointer is outside of the window
                if (!visible)
                {
                    WinSetCapture(HWND_DESKTOP, NULLHANDLE);
                    WinShowPointer(HWND_DESKTOP, TRUE);
                    visible=TRUE;
                }
                // don't use 'outside'-values
                if (_mouse_x<0) _mouse_x=0;
                else
                    if (_mouse_x>=swp.cx) _mouse_x=swp.cx-1;
                if (_mouse_y<0) _mouse_y=0;
                else
                    if (_mouse_y>=swp.cy) _mouse_y=swp.cy-1;
            }
        }
        return;
    case WM_BUTTON1DOWN:
        joystick_value[1] |= 16;
        return;
    case WM_BUTTON1UP:
        joystick_value[1] &= ~16;
        return;
    case WM_BUTTON2DOWN:
        joystick_value[1] |= 1;
        return;
    case WM_BUTTON2UP:
        joystick_value[1] &= ~1;
        return;
    }
}
