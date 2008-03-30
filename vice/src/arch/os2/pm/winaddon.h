/*
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

#ifndef _WINADDON_H
#define _WINADDON_H

#define INCL_WINSYS
#define INCL_WININPUT
#define INCL_WINRECTANGLES
#define INCL_WINWINDOWMGR

#ifdef __IBMC__
#define INCL_GPIPRIMITIVES
#endif

#include <os2.h>
#include <string.h>

#define BORDER_LOWER 0
#define BORDER_RAISE 1

void WinDraw3dLine  (HPS hps, RECTL *rectl, int raise);
void WinDraw3dBorder(HPS hps, RECTL *rectl, int width, int raise);
void drawBar(HPS hps, RECTL rectl, float val, float cut, char *txt);

#endif
