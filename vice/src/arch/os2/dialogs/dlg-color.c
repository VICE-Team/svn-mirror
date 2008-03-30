/*
 * dlg-joystick.c - The joystick-dialog.
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

//#define INCL_WININPUT     // WM_CHAR
//#define INCL_WINBUTTONS
#define INCL_WINDIALOGS     // WinSendDlgItemMsg
//#define INCL_WINSTDSPIN
#define INCL_WINSTDSLIDER   // SL?_*
//#define INCL_WINFRAMEMGR  // WM_TANSLATEACCEL
#include "vice.h"

#include <os2.h>

#include "dialogs.h"
#include "dlg-color.h"

//#include <ctype.h>        // isprint
//#include <stdlib.h>       // free

#include "log.h"
//#include "utils.h"        // xmsprintf
//#include "joystick.h"
#include "resources.h"
#include "snippets\pmwin2.h"

void SetSliderPos(HWND hwnd, USHORT id, SHORT val)
{
    WinSendDlgItemMsg(hwnd, id, SLM_SETSLIDERINFO,
                      MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE),
                      MPFROMSHORT(val));
}

void SetSliderTxt(HWND hwnd, USHORT id, SHORT val, const char *txt)
{
    WinSendDlgItemMsg(hwnd, id, SLM_SETSCALETEXT,
                      MPFROMSHORT(val), MPFROMP(txt));
}

static MRESULT EXPENTRY pm_color(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_INITDLG:
        {
            int val;
            SetSliderTxt(hwnd, ID_SATURATION, 0, "0");
            SetSliderTxt(hwnd, ID_SATURATION, 100, "1.0");
            SetSliderTxt(hwnd, ID_SATURATION, 200, "2.0");

            resources_get_value("ColorSaturation", (resource_value_t*)&val);
            SetSliderPos(hwnd, ID_SATURATION, val/10);
            resources_get_value("ColorContrast", (resource_value_t*)&val);
            SetSliderPos(hwnd, ID_CONTRAST, val/10);
            resources_get_value("ColorBrightness", (resource_value_t*)&val);
            SetSliderPos(hwnd, ID_BRIGHTNESS, val/10);
            resources_get_value("ColorGamma", (resource_value_t*)&val);
            SetSliderPos(hwnd, ID_GAMMA, val/10);
        }
        break;

    case WM_CONTROL:
        if (SHORT2FROMMP(mp1) != SLN_CHANGE &&
            SHORT2FROMMP(mp1) != SLN_SLIDERTRACK)
            break;

        switch (SHORT1FROMMP(mp1))
        {
        case ID_SATURATION:
            resources_set_value("ColorSaturation", (resource_value_t*)((int)mp2*10));
            break;
        case ID_CONTRAST:
            resources_set_value("ColorContrast",   (resource_value_t*)((int)mp2*10));
            break;
        case ID_BRIGHTNESS:
            resources_set_value("ColorBrightness", (resource_value_t*)((int)mp2*10));
            break;
        case ID_GAMMA:
            resources_set_value("ColorGamma",      (resource_value_t*)((int)mp2*10));
            break;
        }
        break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void color_dialog(HWND hwnd)
{
    static HWND hwnd2 = NULLHANDLE;

    if (WinIsWindowVisible(hwnd2))
        return;

    hwnd2 = WinLoadStdDlg(hwnd, pm_color, DLG_COLOR, NULL);
}

