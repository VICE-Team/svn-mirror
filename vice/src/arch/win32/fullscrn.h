/*
 * fullscrn.h - Definition of the Win32 fullscreen code.
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

#ifndef VICE_FULLSCRN_H
#define VICE_FULLSCRN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>


typedef struct _DDL {
    struct _DDL *next;
    int isNullGUID;
    GUID guid;
    LPSTR desc;
} DirectDrawDeviceList;

typedef struct _ML {
    struct _ML *next;
    int devicenumber;
    int width;
    int height;
    int bitdepth;
    int refreshrate;
} DirectDrawModeList;


extern void GetCurrentModeParameters(int *width, int *height, int *bitdepth,
                                     int *refreshrate);
extern void ui_fullscreen_init(void);
extern void ui_fullscreen_shutdown(void);
extern void fullscreen_set_res_from_current_display(void);
extern int IsFullscreenEnabled(void);
extern void SuspendFullscreenMode(HWND hwnd);
extern void ResumeFullscreenMode(HWND hwnd);
extern void SuspendFullscreenModeKeep(HWND hwnd);
extern void ResumeFullscreenModeKeep(HWND hwnd);
extern void SwitchFullscreenMode(HWND hwnd);
extern void SwitchToFullscreenMode(HWND hwnd);
extern BOOL CALLBACK dialog_fullscreen_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                            LPARAM lparam);
extern void fullscrn_invalidate_refreshrate(void);

/* DDraw functions */
extern void SwitchToFullscreenModeDDraw(HWND hwnd);
extern void SwitchToWindowedModeDDraw(HWND hwnd);
extern void fullscreen_getmodes_ddraw(void);
extern void fullscreen_get_current_display_ddraw(int *bitdepth, int *width,
                                                 int *height, int *refreshrate);

/* DX9 functions */
extern void SwitchToFullscreenModeDx9(HWND hwnd);
extern void SwitchToWindowedModeDx9(HWND hwnd);
extern void fullscreen_getmodes_dx9(void);
extern void fullscreen_get_current_display_dx9(int *bitdepth, int *width,
                                               int *height, int *refreshrate);


/* FIXME: ugly */
extern int fullscreen_active;
extern int fullscreen_transition;

extern DirectDrawDeviceList *devices;
extern DirectDrawModeList *modes;

#endif

