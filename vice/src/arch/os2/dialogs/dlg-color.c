/*
 * dlg-color.c - The color-dialog.
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
    WinSendDlgItemMsg(hwnd, id, SLM_SETSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE), MPFROMSHORT(val));
}

static void SetSliderTxt(HWND hwnd, USHORT id, SHORT val, const char *txt)
{
    WinSendDlgItemMsg(hwnd, id, SLM_SETSCALETEXT, MPFROMSHORT(val), MPFROMP(txt));
}

static const char *gamma_res = NULL;
static const char *tint_res = NULL;
static const char *saturation_res = NULL;
static const char *contrast_res = NULL;
static const char *brightness_res = NULL;

static const char *scanline_shade_res = NULL;
static const char *blur_res = NULL;
static const char *oddlines_phase_res = NULL;
static const char *oddlines_offset_res = NULL;

static MRESULT EXPENTRY pm_color(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
        case WM_INITDLG:
            {
                int val;

                SetSliderTxt(hwnd, ID_GAMMA, 0, "0");
                SetSliderTxt(hwnd, ID_GAMMA, 100, "2.0");
                SetSliderTxt(hwnd, ID_GAMMA, 200, "4.0");
                resources_get_int(gamma_res, &val);
                SetSliderPos(hwnd, ID_GAMMA, val / 20);

                SetSliderTxt(hwnd, ID_TINT, 0, "0");
                SetSliderTxt(hwnd, ID_TINT, 100, "1.0");
                SetSliderTxt(hwnd, ID_TINT, 200, "2.0");
                resources_get_int(tint_res, &val);
                SetSliderPos(hwnd, ID_TINT, val / 10);

                SetSliderTxt(hwnd, ID_SATURATION, 0, "0");
                SetSliderTxt(hwnd, ID_SATURATION, 100, "1.0");
                SetSliderTxt(hwnd, ID_SATURATION, 200, "2.0");
                resources_get_int(saturation_res, &val);
                SetSliderPos(hwnd, ID_SATURATION, val / 10);

                SetSliderTxt(hwnd, ID_CONTRAST, 0, "0");
                SetSliderTxt(hwnd, ID_CONTRAST, 100, "1.0");
                SetSliderTxt(hwnd, ID_CONTRAST, 200, "2.0");
                resources_get_int(contrast_res, &val);
                SetSliderPos(hwnd, ID_CONTRAST, val / 10);

                SetSliderTxt(hwnd, ID_BRIGHTNESS, 0, "0");
                SetSliderTxt(hwnd, ID_BRIGHTNESS, 100, "1.0");
                SetSliderTxt(hwnd, ID_BRIGHTNESS, 200, "2.0");
                resources_get_int(brightness_res, &val);
                SetSliderPos(hwnd, ID_BRIGHTNESS, val / 10);
            }
            break;
        case WM_COMMAND:
            if (LONGFROMMP(mp1) != ID_DEFAULT) {
                break;
            }
            resources_set_int(gamma_res, 2200);
            resources_set_int(tint_res, 1000);
            resources_set_int(saturation_res, 1000);
            resources_set_int(contrast_res, 1000);
            resources_set_int(brightness_res, 1000);
            SetSliderPos(hwnd, ID_GAMMA, 110);
            SetSliderPos(hwnd, ID_TINT, 100);
            SetSliderPos(hwnd, ID_SATURATION, 100);
            SetSliderPos(hwnd, ID_CONTRAST, 100);
            SetSliderPos(hwnd, ID_BRIGHTNESS, 100);
            return FALSE;
        case WM_CONTROL:
            if (SHORT2FROMMP(mp1) != SLN_CHANGE && SHORT2FROMMP(mp1) != SLN_SLIDERTRACK) {
                break;
            }
            switch (SHORT1FROMMP(mp1)) {
#if 0
                case ID_GAMMA:
                    resources_set_int(gamma_res, (int)mp2 * 20);
                    break;
                case ID_TINT:
                    resources_set_int(tint_res, (int)mp2 * 10);
                    break;
                case ID_SATURATION:
                    resources_set_int(saturation_res, (int)mp2 * 10);
                    break;
                case ID_CONTRAST:
                    resources_set_int(contrast_res, (int)mp2 * 10);
                    break;
                case ID_BRIGHTNESS:
                    resources_set_int(brightness_res, (int)mp2 * 10);
                    break;
#endif
            }
            break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

static MRESULT EXPENTRY pm_crt(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

static const char *oddlines_offset_res = NULL;

    switch (msg) {
        case WM_INITDLG:
            {
                int val;

                SetSliderTxt(hwnd, ID_SCANLINE_SHADE, 0, "0");
                SetSliderTxt(hwnd, ID_SCANLINE_SHADE, 100, "0.5");
                SetSliderTxt(hwnd, ID_SCANLINE_SHADE, 200, "1.0");
                resources_get_int(scanline_shade_res, &val);
                SetSliderPos(hwnd, ID_SCANLINE_SHADE, val / 5);

                SetSliderTxt(hwnd, ID_BLUR, 0, "0");
                SetSliderTxt(hwnd, ID_BLUR, 100, "0.5");
                SetSliderTxt(hwnd, ID_BLUR, 200, "1.0");
                resources_get_int(blur_res, &val);
                SetSliderPos(hwnd, ID_BLUR, val / 5);

                SetSliderTxt(hwnd, ID_ODDLINES_PHASE, 0, "0");
                SetSliderTxt(hwnd, ID_ODDLINES_PHASE, 100, "1.0");
                SetSliderTxt(hwnd, ID_ODDLINES_PHASE, 200, "2.0");
                resources_get_int(oddlines_phase_res, &val);
                SetSliderPos(hwnd, ID_ODDLINES_PHASE, val / 10);

                SetSliderTxt(hwnd, ID_ODDLINES_OFFSET, 0, "0");
                SetSliderTxt(hwnd, ID_ODDLINES_OFFSET, 100, "1.0");
                SetSliderTxt(hwnd, ID_ODDLINES_OFFSET, 200, "2.0");
                resources_get_int(oddlines_offset_res, &val);
                SetSliderPos(hwnd, ID_ODDLINES_OFFSET, val / 10);
            }
            break;
        case WM_COMMAND:
            if (LONGFROMMP(mp1) != ID_DEFAULT) {
                break;
            }
            resources_set_int(scanline_shade_res, 667);
            resources_set_int(blur_res, 500);
            resources_set_int(oddlines_phase_res, 1250);
            resources_set_int(oddlines_offset_res, 750);
            SetSliderPos(hwnd, ID_SCANLINE_SHADE, (int)(667 / 5));
            SetSliderPos(hwnd, ID_BLUR, 100);
            SetSliderPos(hwnd, ID_ODDLINES_PHASE, 125);
            SetSliderPos(hwnd, ID_ODDLINES_OFFSET, 75);
            return FALSE;
        case WM_CONTROL:
            if (SHORT2FROMMP(mp1) != SLN_CHANGE && SHORT2FROMMP(mp1) != SLN_SLIDERTRACK) {
                break;
            }
            switch (SHORT1FROMMP(mp1)) {
#if 0
                case ID_SCANLINE_SHADE:
                    resources_set_int(scanline_shade_res, (int)mp2 * 5);
                    break;
                case ID_BLUR:
                    resources_set_int(blur_res, (int)mp2 * 5);
                    break;
                case ID_ODDLINES_PHASE:
                    resources_set_int(oddlines_phase_res, (int)mp2 * 10);
                    break;
                case ID_ODDLINES_OFFSET:
                    resources_set_int(oddlines_offset_res, (int)mp2 * 10);
                    break;
#endif
            }
            break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void color_dialog(HWND hwnd, const char *gamma, const char *tint, const char *saturation, const char *contrast, const char *brightness)
{
    static HWND hwnd2 = NULLHANDLE;

    if (WinIsWindowVisible(hwnd2)) {
        return;
    }

    gamma_res = gamma;
    tint_res = tint;
    saturation_res = saturation;
    contrast_res = contrast;
    brightness_res = brightness;

    hwnd2 = WinLoadStdDlg(hwnd, pm_color, DLG_COLOR, NULL);
}

void crt_dialog(HWND hwnd, const char *scanline_shade, const char *blur, const char *oddlines_phase, const char *oddlines_offset)
{
    static HWND hwnd3 = NULLHANDLE;

    if (WinIsWindowVisible(hwnd3)) {
        return;
    }

    scanline_shade_res = scanline_shade;
    blur_res = blur;
    oddlines_phase_res = oddlines_phase;
    oddlines_offset_res = oddlines_offset;

    hwnd3 = WinLoadStdDlg(hwnd, pm_crt, DLG_CRT, NULL);
}

