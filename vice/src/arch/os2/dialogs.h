/*
 * dialogs.h - The dialog windows.
 *
 * Written by
 *  Thomas Bretz (tbretz@gsi.de)
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

#ifndef _DIALOGS_H
#define _DIALOGS_H

#include <os2.h>

#define ID_NONE      -1
#define DID_CLOSE    DID_OK

// Joystick Dialog
#define DLG_JOYSTICK  0x1000
#define CB_JOY11      0x111
#define CB_JOY12      0x211
#define CB_JOY21      0x112
#define CB_JOY22      0x212
#define ID_SWAP       0x110

// Sound Dialog
#define DLG_SOUND     0x1010
#define CB_SOUND      0x1011
#define CS_VOLUME     0x1012
#define SPB_BUFFER    0x1013
#define CB_SIDFILTER  0x1014
#ifdef HAVE_RESID
  #define CB_RESID      0x1015
#endif
#define RB_8000HZ     0x1F40
#define RB_11025HZ    0x2B11
#define RB_22050HZ    0x5622
#define RB_44100HZ    0xAC44
#define RB_OFF        0x1030
#define RB_2X         0x1031
#define RB_4X         0x1032
#define RB_8X         0x1033
#define RB_6581       0x1040
#define RB_8580       0x1041

// Drive Dialog
#define DLG_DRIVE     0x1020
#define CB_TRUEDRIVE  0x1021

#define DLG_ABOUT     0x1050

void drive_dialog   (HWND hwnd);
void sound_dialog   (HWND hwnd);
void joystick_dialog(HWND hwnd);
void about_dialog   (HWND hwnd);

#endif

