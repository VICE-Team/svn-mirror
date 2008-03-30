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

#ifndef _FULLSCRN_H
#define _FULLSCRN_H

void GetCurrentModeParameters(int *width, int *height, int *bitdepth, int *refreshrate);
GUID *GetGUIDForActualDevice();
void ui_fullscreen_init(void);
int IsFullscreenEnabled(void);
void SuspendFullscreenMode(HWND hwnd);
void ResumeFullscreenMode(HWND hwnd);
void SwitchFullscreenMode(HWND hwnd);
void SwitchToFullscreenMode(HWND hwnd);
extern void init_fullscreen_dialog(HWND hwnd);
extern BOOL CALLBACK dialog_fullscreen_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

#endif

