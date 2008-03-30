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

#include "winaddon.h"

void WinDraw3dLine  (HPS hps, RECTL *rectl, int raise)
{
    //    if (rectl->yTop != rectl->yBottom+1) return;
    WinFillRect(hps, rectl, raise?SYSCLR_SHADOWHILITEBGND:SYSCLR_SHADOWHILITEFGND);
    rectl->yTop++;
    rectl->yBottom++;
    WinFillRect(hps, rectl, raise?SYSCLR_SHADOWHILITEFGND:SYSCLR_SHADOWHILITEBGND);
}

void WinDraw3dBorder(HPS hps, RECTL *rectl, int width, int raise)
{
//    WinDrawBorder(hps, rectl, 2, 2, 0, 0, DB_AREAATTRS);
    rectl->yBottom += width;
    rectl->xRight  -= width;
    WinFillRect(hps, rectl, raise?SYSCLR_SHADOWHILITEFGND:SYSCLR_SHADOWHILITEBGND);
    rectl->yTop    -= width;
    rectl->xLeft   += width;
    rectl->xRight  += width;
    rectl->yBottom -= width;
    WinFillRect(hps, rectl, raise?SYSCLR_SHADOWHILITEBGND:SYSCLR_SHADOWHILITEFGND);
    rectl->xRight  -= width;
    rectl->yBottom += width;
    WinFillRect(hps, rectl, SYSCLR_BUTTONMIDDLE);
//    WinFillRect(hps, rectl, SYSCLR_SHADOWTEXT);
}


void drawBar(HPS hps, RECTL rectl, float val, float cut, char *txt)
{
//    WinDraw3dBorder(hps, &rectl, 3, BORDER_RAISE);
//    WinFillRect(hps, &rectl,SYSCLR_BUTTONDARK);       // Draw Backgrnd

    int left =rectl.xLeft +1;
    int right=rectl.xRight-1;

    WinDrawBorder(hps, &rectl, 2, 2, 0, 0, DB_AREAATTRS);

    rectl.yBottom +=1;
    rectl.yTop    -=1;

    rectl.xLeft   +=1;
    rectl.xRight   = left+(val<cut ? val/cut : 1)*(right-left);
    WinFillRect(hps, &rectl, 2);                      // Draw Left Side

    rectl.xLeft    = rectl.xRight;
    rectl.xRight   = right;
    WinFillRect(hps, &rectl, SYSCLR_FIELDBACKGROUND); // Draw Right Side

    rectl.xLeft    = left;
    rectl.xRight   = right;
    GpiSetMix(hps, FM_NOTXORSRC);                     // Draw Text in bar
    WinDrawText(hps, strlen(txt), txt, &rectl, 0, 0,
                DT_TEXTATTRS|DT_VCENTER|DT_CENTER);
}

