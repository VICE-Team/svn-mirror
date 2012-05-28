/*
 * dlg-color.h - The dialog windows.
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

// Color Dialog
#define ID_SATURATION      0x11
#define ID_BRIGHTNESS      0x12
#define ID_GAMMA           0x13
#define ID_CONTRAST        0x14
#define ID_TINT            0x15
#define ID_SCANLINE_SHADE  0x16
#define ID_BLUR            0x17
#define ID_ODDLINES_PHASE  0x18
#define ID_ODDLINES_OFFSET 0x19
#define ID_DEFAULT         0x1a

extern void color_dialog(HWND hwnd, const char *gamma, const char *tint, const char *saturation, const char *contrast, const char *brightness);
extern void crt_dialog(HWND hwnd, const char *scanline_shade, const char *blur, const char *oddlines_phase, const char *oddlines_offset);

