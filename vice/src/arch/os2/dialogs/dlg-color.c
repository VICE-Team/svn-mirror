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

static MRESULT EXPENTRY pm_color(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
        case WM_INITDLG:
            {
                int val;

                SetSliderTxt(hwnd, ID_GAMMA, 0, "0");
                SetSliderTxt(hwnd, ID_GAMMA, 100, "2.0");
                SetSliderTxt(hwnd, ID_GAMMA, 200, "4.0");
                resources_get_int("VICIIColorGamma", &val);
                SetSliderPos(hwnd, ID_GAMMA, val / 20);

                SetSliderTxt(hwnd, ID_TINT, 0, "0");
                SetSliderTxt(hwnd, ID_TINT, 100, "1.0");
                SetSliderTxt(hwnd, ID_TINT, 200, "1.0");
                resources_get_int("VICIIColorTint", &val);
                SetSliderPos(hwnd, ID_TINT, val / 10);

                SetSliderTxt(hwnd, ID_SATURATION, 0, "0");
                SetSliderTxt(hwnd, ID_SATURATION, 100, "1.0");
                SetSliderTxt(hwnd, ID_SATURATION, 200, "1.0");
                resources_get_int("VICIIColorSaturation", &val);
                SetSliderPos(hwnd, ID_SATURATION, val / 10);

                SetSliderTxt(hwnd, ID_CONTRAST, 0, "0");
                SetSliderTxt(hwnd, ID_CONTRAST, 100, "1.0");
                SetSliderTxt(hwnd, ID_CONTRAST, 200, "1.0");
                resources_get_int("VICIIColorContrast", &val);
                SetSliderPos(hwnd, ID_CONTRAST, val / 10);

                SetSliderTxt(hwnd, ID_BRIGHTNESS, 0, "0");
                SetSliderTxt(hwnd, ID_BRIGHTNESS, 100, "1.0");
                SetSliderTxt(hwnd, ID_BRIGHTNESS, 200, "1.0");
                resources_get_int("VICIIColorBrightness", &val);
                SetSliderPos(hwnd, ID_BRIGHTNESS, val / 10);
            }
            break;
        case WM_COMMAND:
            if (LONGFROMMP(mp1) != ID_DEFAULT) {
                break;
            }
            emulator_pause();
            resources_set_int("VICIIColorGamma", 2200);
            resources_set_int("VICIIColorTint", 1000);
            resources_set_int("VICIIColorSaturation", 1000);
            resources_set_int("VICIIColorContrast", 1000);
            resources_set_int("VICIIColorBrightness", 1000);
            emulator_resume();
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
                    resources_set_int("VICIIColorGamma", (int)mp2 * 20);
                    break;
                case ID_TINT:
                    resources_set_int("VICIIColorTint", (int)mp2 * 10);
                    break;
                case ID_SATURATION:
                    resources_set_int("VICIIColorSaturation", (int)mp2 * 10);
                    break;
                case ID_CONTRAST:
                    resources_set_int("VICIIColorContrast", (int)mp2 * 10);
                    break;
                case ID_BRIGHTNESS:
                    resources_set_int("VICIIColorBrightness", (int)mp2 * 10);
                    break;
#endif
            }
            break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void color_dialog(HWND hwnd, int vicii, int vdc, int crtc, int ted, int vic)
{
    static HWND hwnd2 = NULLHANDLE;

    if (vicii) {
        gamma_res = "VICIIColorGamma";
        tint_res = "VICIIColorTint";
        saturation_res = "VICIIColorSaturation";
        contrast_res = "VICIIColorContrast";
        brightness_res = "VICIIColorBrightness";
    } else if (vdc) {
        gamma_res = "VDCColorGamma";
        tint_res = "VDCColorTint";
        saturation_res = "VDCColorSaturation";
        contrast_res = "VDCColorContrast";
        brightness_res = "VDCColorBrightness";
    } else if (crtc) {
        gamma_res = "CrtcColorGamma";
        tint_res = "CrtcColorTint";
        saturation_res = "CrtcColorSaturation";
        contrast_res = "CrtcColorContrast";
        brightness_res = "CrtcColorBrightness";
    } else if (ted) {
        gamma_res = "TEDColorGamma";
        tint_res = "TEDColorTint";
        saturation_res = "TEDColorSaturation";
        contrast_res = "TEDColorContrast";
        brightness_res = "TEDColorBrightness";
    } else {
        gamma_res = "VICColorGamma";
        tint_res = "VICColorTint";
        saturation_res = "VICColorSaturation";
        contrast_res = "VICColorContrast";
        brightness_res = "VICColorBrightness";
    }

    if (WinIsWindowVisible(hwnd2)) {
        return;
    }

    hwnd2 = WinLoadStdDlg(hwnd, pm_color, DLG_COLOR, NULL);
}
