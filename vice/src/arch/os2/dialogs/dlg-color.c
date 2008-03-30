/*
 * dlg-color.c - The color-dialog.
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

#define INCL_DOSPROCESS     // DosSleep
#define INCL_WINDIALOGS     // WinSendDlgItemMsg
#define INCL_WINSTDSLIDER   // SL?_*
#include "vice.h"

#include <os2.h>

#include "dialogs.h"
#include "dlg-color.h"

#include "log.h"
#include "resources.h"
#include "interrupt.h"         // interrupt_maincpu_trigger_trap
#include "snippets\pmwin2.h"


static int canvas_set_value(const char *name, int val)
{
    return resources_set_int(name, val);
}

static void SetSliderPos(HWND hwnd, USHORT id, SHORT val)
{
    WinSendDlgItemMsg(hwnd, id, SLM_SETSLIDERINFO,
                      MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE),
                      MPFROMSHORT(val));
}

static void SetSliderTxt(HWND hwnd, USHORT id, SHORT val, const char *txt)
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
            SetSliderTxt(hwnd, ID_SATURATION,   0, "0");
            SetSliderTxt(hwnd, ID_SATURATION, 100, "1.0");
            SetSliderTxt(hwnd, ID_SATURATION, 200, "2.0");

            resources_get_int("ColorSaturation", &val);
            SetSliderPos(hwnd, ID_SATURATION, val/10);
            resources_get_int("ColorContrast", &val);
            SetSliderPos(hwnd, ID_CONTRAST, val/10);
            resources_get_int("ColorBrightness", &val);
            SetSliderPos(hwnd, ID_BRIGHTNESS, val/10);
            resources_get_int("ColorGamma", &val);
            SetSliderPos(hwnd, ID_GAMMA, val/10);
            resources_get_int("PALScanLineShade", &val);
            SetSliderPos(hwnd, ID_SCANLINE, val/5);
            resources_get_int("PALBlur", &val);
            SetSliderPos(hwnd, ID_BLUR, val/5);
        }
        break;

    case WM_COMMAND:
        if (LONGFROMMP(mp1) != ID_DEFAULT)
            break;

        canvas_set_value("ColorSaturation", 1000);
        canvas_set_value("ColorContrast",   1000);
        canvas_set_value("ColorBrightness", 1000);
        canvas_set_value("ColorGamma",       880);
        canvas_set_value("PALScanLineShade", 667);
        canvas_set_value("PALBlur",          500);

        SetSliderPos(hwnd, ID_SATURATION, 100);
        SetSliderPos(hwnd, ID_CONTRAST,   100);
        SetSliderPos(hwnd, ID_BRIGHTNESS, 100);
        SetSliderPos(hwnd, ID_GAMMA,       88);
        SetSliderPos(hwnd, ID_SCANLINE,   133);
        SetSliderPos(hwnd, ID_BLUR,       100);

        return FALSE;

    case WM_CONTROL:
        if (SHORT2FROMMP(mp1) != SLN_CHANGE &&
            SHORT2FROMMP(mp1) != SLN_SLIDERTRACK)
            break;

        switch (SHORT1FROMMP(mp1))
        {
        case ID_SATURATION:
            canvas_set_value("ColorSaturation",  (int)mp2*10);
            break;
        case ID_CONTRAST:
            canvas_set_value("ColorContrast",    (int)mp2*10);
            break;
        case ID_BRIGHTNESS:
            canvas_set_value("ColorBrightness",  (int)mp2*10);
            break;
        case ID_GAMMA:
            canvas_set_value("ColorGamma",       (int)mp2*10);
            break;
        case ID_SCANLINE:
            canvas_set_value("PALScanLineShade", (int)mp2* 5);
            break;
        case ID_BLUR:
            canvas_set_value("PALBlur",          (int)mp2* 5);
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
