/*
 * uivideo.c - Implementation of video settings dialog box.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Tibor Biczo <crown@mail.matav.hu>
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

#include "vice.h"

#include <windows.h>
#include <prsht.h>
#include <tchar.h>

#ifdef HAVE_SHLOBJ_H
#include <shlobj.h>
#endif

#include "fullscrn.h"
#include "intl.h"
#include "lib.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "ui.h"
#include "uiapi.h"
#include "uilib.h"
#include "uivideo.h"
#include "winlong.h"
#include "winmain.h"

static const char *vicii_palettes[] = {
    "default",
    "c64hq",
    "c64s",
    "ccs64",
    "frodo",
    "godot",
    "pc64",
    "vice",
    NULL
};

static const char *vic_palettes[] = {
    "default",
    NULL
};

static const char *crtc_palettes[] = {
    "amber",
    "green",
    "white",
    NULL
};

static const char *vdc_palettes[] = {
    "vdc_deft",
    "vdc_comp",
    NULL
};

static const char *ted_palettes[] = {
    "default",
    "vice",
    NULL
};

typedef struct {
    const char **palette_names;
    char *res_PaletteFile_name;
    char *res_ExternalPalette_name;
    int page_title;
    int external_pal;
    char *file_name;
    char *scale2x_resource;
} Chip_Parameters;

static Chip_Parameters chip_param_table[] =
{
    { vicii_palettes, "VICIIPaletteFile", "VICIIExternalPalette",
      IDS_VICII_PALETTE, 0, NULL, "VICIIScale2x" },
    { vic_palettes, "VICPaletteFile", "VICExternalPalette",
      IDS_VIC_PALETTE, 0, NULL, "VICScale2x" },
    { crtc_palettes, "CRTCPaletteFile", "CRTCExternalPalette",
      IDS_CRTC_PALETTE, 0, NULL, NULL },
    { vdc_palettes, "VDCPaletteFile", "VDCExternalPalette",
      IDS_VDC_PALETTE, 0, NULL, NULL },
    { ted_palettes, "TEDPaletteFile", "TEDExternalPalette",
      IDS_TED_PALETTE, 0, NULL, "TEDScale2x" },
};

static HWND palette_dialog_1 = NULL;
static Chip_Parameters *current_chip_1 = NULL;
static Chip_Parameters *current_chip_2 = NULL;

static uilib_localize_dialog_param color_dialog_trans[] = {
    { IDC_VIDEO_COLORS_GAMMA_LABEL, IDS_COLORS_GAMMA, 0 },
    { IDC_VIDEO_COLORS_TINT_LABEL, IDS_COLORS_TINT, 0 },
    { IDC_VIDEO_COLORS_SATURATION_LABEL, IDS_COLORS_SATURATION, 0 },
    { IDC_VIDEO_COLORS_CONTRAST_LABEL, IDS_COLORS_CONTRAST, 0 },
    { IDC_VIDEO_COLORS_BRIGHTNESS_LABEL, IDS_COLORS_BRIGHTNESS, 0 },
    { 0, 0, 0 }
};

static uilib_dialog_group color_left_group[] = {
    { IDC_VIDEO_COLORS_GAMMA_LABEL, 0 },
    { IDC_VIDEO_COLORS_TINT_LABEL, 0 },
    { IDC_VIDEO_COLORS_SATURATION_LABEL, 0 },
    { IDC_VIDEO_COLORS_CONTRAST_LABEL, 0 },
    { IDC_VIDEO_COLORS_BRIGHTNESS_LABEL, 0 },
    { 0, 0 }
};

static uilib_dialog_group color_right_group[] = {
    { IDC_VIDEO_COLORS_GAMMA, 0 },
    { IDC_VIDEO_COLORS_TINT, 0 },
    { IDC_VIDEO_COLORS_SATURATION, 0 },
    { IDC_VIDEO_COLORS_CONTRAST, 0 },
    { IDC_VIDEO_COLORS_BRIGHTNESS, 0 },
    { 0, 0 }
};

static void init_color_dialog(HWND hwnd)
{
    int val;
    double fval;
    TCHAR newval[64];
    int xpos;

    /* translate all dialog items */
    uilib_localize_dialog(hwnd, color_dialog_trans);

    /* adjust the size of the elements in the left group */
    uilib_adjust_group_width(hwnd, color_left_group);

    /* get the max x of the left group */
    uilib_get_group_max_x(hwnd, color_left_group, &xpos);

    /* move the right group to the correct position */
    uilib_move_group(hwnd, color_right_group, xpos + 10);

    resources_get_int("ColorGamma", &val);
    fval = ((double)val) / 1000.0;
    _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_GAMMA, newval);

    resources_get_int("ColorTint", &val);
    fval = ((double)val) / 1000.0;
    _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_TINT, newval);

    resources_get_int("ColorSaturation", &val);
    fval = ((double)val) / 1000.0;
    _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_SATURATION, newval);

    resources_get_int("ColorContrast", &val);
    fval = ((double)val) / 1000.0;
    _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_CONTRAST, newval);

    resources_get_int("ColorBrightness", &val);
    fval = ((double)val) / 1000.0;
    _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_BRIGHTNESS, newval);
}

static uilib_localize_dialog_param crt_emulation_dialog_trans[] = {
    { IDC_VIDEO_CRT_SCANLINE_SHADE_LABEL, IDS_CRT_SCANLINE_SHADE, 0 },
    { IDC_VIDEO_CRT_BLUR_LABEL, IDS_CRT_BLUR, 0 },
    { IDC_VIDEO_CRT_ODDLINE_PHASE_LABEL, IDS_CRT_ODDLINE_PHASE, 0 },
    { IDC_VIDEO_CRT_ODDLINE_OFFSET_LABEL, IDS_CRT_ODDLINE_OFFSET, 0 },
    { 0, 0, 0 }
};

static uilib_dialog_group crt_emulation_left_group[] = {
    { IDC_VIDEO_CRT_SCANLINE_SHADE_LABEL, 0 },
    { IDC_VIDEO_CRT_BLUR_LABEL, 0 },
    { IDC_VIDEO_CRT_ODDLINE_PHASE_LABEL, 0 },
    { IDC_VIDEO_CRT_ODDLINE_OFFSET_LABEL, 0 },
    { 0, 0 }
};

static uilib_dialog_group crt_emulation_right_group[] = {
    { IDC_VIDEO_CRT_SCANLINE_SHADE, 0 },
    { IDC_VIDEO_CRT_BLUR, 0 },
    { IDC_VIDEO_CRT_ODDLINE_PHASE, 0 },
    { IDC_VIDEO_CRT_ODDLINE_OFFSET, 0 },
    { 0, 0 }
};

static void init_crt_emulation_dialog(HWND hwnd)
{
    int val;
    double fval;
    TCHAR newval[64];
    int xpos;

    /* translate all dialog items */
    uilib_localize_dialog(hwnd, crt_emulation_dialog_trans);

    /* adjust the size of the elements in the left group */
    uilib_adjust_group_width(hwnd, crt_emulation_left_group);

    /* get the max x of the left group */
    uilib_get_group_max_x(hwnd, crt_emulation_left_group, &xpos);

    /* move the right group to the correct position */
    uilib_move_group(hwnd, crt_emulation_right_group, xpos + 10);

    resources_get_int("PALScanLineShade", &val);
    fval = ((double)val) / 1000.0;
    _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_CRT_SCANLINE_SHADE, newval);

    resources_get_int("PALBlur", &val);
    fval = ((double)val) / 1000.0;
    _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_CRT_BLUR, newval);

    resources_get_int("PALOddLinePhase", &val);
    fval = ((double)val) / 1000.0;
    _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_CRT_ODDLINE_PHASE, newval);

    resources_get_int("PALOddLineOffset", &val);
    fval = ((double)val) / 1000.0;
    _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_CRT_ODDLINE_OFFSET, newval);
}

static uilib_localize_dialog_param palette_dialog_trans[] = {
    { IDC_TOGGLE_VIDEO_EXTPALETTE, IDS_TOGGLE_VIDEO_EXTPALETTE, 0 },
    { IDC_VIDEO_CUSTOM_BROWSE, IDS_BROWSE, 0 },
    { 0, 0, 0 }
};

static void init_palette_dialog(HWND hwnd, Chip_Parameters *chip_type)
{
    int n;
    const char *path;
    TCHAR *st_path;
    HWND filename_hwnd;
    int xstart, xpos, size;

    if (chip_type == current_chip_1) {
        palette_dialog_1 = hwnd;
    }

    /* translate all dialog items */
    uilib_localize_dialog(hwnd, palette_dialog_trans);

    /* adjust the size of the external palette element */
    uilib_adjust_element_width(hwnd, IDC_TOGGLE_VIDEO_EXTPALETTE);

    /* get the min x of the palette combo element */
    uilib_get_element_min_x(hwnd, IDC_VIDEO_CUSTOM_NAME, &xstart);

    /* get the max x of the external palette element */
    uilib_get_element_max_x(hwnd, IDC_TOGGLE_VIDEO_EXTPALETTE, &xpos);

    if (xpos + 10 > xstart) {
        /* get the size of the palette combo element */
        uilib_get_element_size(hwnd, IDC_VIDEO_CUSTOM_NAME, &size);

        /* move and resize the palette combo element */
        uilib_move_and_set_element_width(hwnd, IDC_VIDEO_CUSTOM_NAME, xpos + 10, size - ((xpos + 10) - xstart));
    }

    filename_hwnd = GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_NAME);
    SendMessage(filename_hwnd, CB_RESETCONTENT, 0, 0);
    n = 0 ;
    while (chip_type->palette_names[n] != NULL) {
        SendMessage(filename_hwnd, CB_ADDSTRING, 0, (LPARAM)chip_type->palette_names[n]);
        n++;
    }
    resources_get_string(chip_type->res_PaletteFile_name, &path);
    lib_free(chip_type->file_name);
    chip_type->file_name = lib_stralloc(path);
    st_path = system_mbstowcs_alloc(path);
    SetDlgItemText(hwnd, IDC_VIDEO_CUSTOM_NAME, st_path);
    system_mbstowcs_free(st_path);

    resources_get_int(chip_type->res_ExternalPalette_name, &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_VIDEO_EXTPALETTE, n ? BST_CHECKED : BST_UNCHECKED);
    chip_type->external_pal = n;
}

static uilib_localize_dialog_param render_filter_dialog_trans[] = {
    {IDC_VIDEO_RENDER_FILTER_LABEL, IDS_RENDER_FILTER, 0},
    {0, 0, 0}
};

static uilib_dialog_group render_filter_left_group[] = {
    {IDC_VIDEO_RENDER_FILTER_LABEL,  0},
    {0, 0}
};

static uilib_dialog_group render_filter_right_group[] = {
    {IDC_VIDEO_RENDER_FILTER,  0},
    {0, 0}
};

static void init_render_filter_dialog(HWND hwnd, Chip_Parameters *chip_type)
{
    HWND setting_hwnd;
    int xpos;
    int res_value;

    /* translate all dialog items */
    uilib_localize_dialog(hwnd, render_filter_dialog_trans);

    /* adjust the size of the elements in the left group */
    uilib_adjust_group_width(hwnd, render_filter_left_group);

    /* get the max x of the left group */
    uilib_get_group_max_x(hwnd, render_filter_left_group, &xpos);

    /* set the position of the right group */
    uilib_move_group(hwnd, render_filter_right_group, xpos + 10);

    setting_hwnd = GetDlgItem(hwnd, IDC_VIDEO_RENDER_FILTER);
    SendMessage(setting_hwnd, CB_ADDSTRING, 0, (LPARAM)translate_text(IDS_NONE));
    SendMessage(setting_hwnd, CB_ADDSTRING, 0, (LPARAM)translate_text(IDS_CRT_EMULATION));
    if (chip_type->scale2x_resource != NULL) {
        SendMessage(setting_hwnd, CB_ADDSTRING, 0, (LPARAM)translate_text(IDS_SCALE2X));
    }

    resources_get_int("PALEmulation", &res_value);
    if (res_value == 1) {
        SendMessage(setting_hwnd, CB_SETCURSEL, (WPARAM)1, 0);
    } else {
        if (chip_type->scale2x_resource != NULL) {
            resources_get_int(chip_type->scale2x_resource, &res_value);
            if (res_value == 1) {
                SendMessage(setting_hwnd, CB_SETCURSEL, (WPARAM)2, 0);
            } else {
                SendMessage(setting_hwnd, CB_SETCURSEL, (WPARAM)0, 0);
            }
        } else {
            SendMessage(setting_hwnd, CB_SETCURSEL, (WPARAM)0, 0);
        }
    }
}

static INT_PTR CALLBACK dialog_color_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    int type;
    int ivalgamma;
    int ivaltint;
    int ivalsaturation;
    int ivalcontrast;
    int ivalbrightness;

    float tf;
    TCHAR s[100];

    switch (msg) {
        case WM_NOTIFY:
            if (((NMHDR FAR *)lparam)->code == (UINT)PSN_APPLY) {
                GetDlgItemText(hwnd, IDC_VIDEO_COLORS_GAMMA, s, 100);
                _stscanf(s, TEXT("%f"), &tf);
                ivalgamma = (int)(tf * 1000.0 + 0.5);
                if (ivalgamma < 0) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_COLORS_GAMMA), 0.0f);
                    ivalgamma = 0;
                }
                if (ivalgamma > 4000) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_COLORS_GAMMA), 4.0f);
                    ivalgamma = 4000;
                }

                GetDlgItemText(hwnd, IDC_VIDEO_COLORS_TINT, s, 100);
                _stscanf(s, TEXT("%f"), &tf);
                ivaltint = (int)(tf * 1000.0 + 0.5);
                if (ivaltint < 0) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_COLORS_TINT), 0.0f);
                    ivaltint = 0;
                }
                if (ivaltint > 2000) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_COLORS_TINT), 2.0f);
                    ivaltint = 2000;
                }

                GetDlgItemText(hwnd, IDC_VIDEO_COLORS_SATURATION, s, 100);
                _stscanf(s, TEXT("%f"), &tf);
                ivalsaturation = (int)(tf * 1000.0 + 0.5);
                if (ivalsaturation < 0) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_COLORS_SATURATION), 0.0f);
                    ivalsaturation = 0;
                }
                if (ivalsaturation > 2000) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_COLORS_SATURATION), 2.0f);
                    ivalsaturation = 2000;
                }

                GetDlgItemText(hwnd, IDC_VIDEO_COLORS_CONTRAST, s, 100);
                _stscanf(s, TEXT("%f"), &tf);
                ivalcontrast = (int)(tf * 1000.0 + 0.5);
                if (ivalcontrast < 0) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_COLORS_CONTRAST), 0.0f);
                    ivalcontrast = 0;
                }
                if (ivalcontrast > 2000) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_COLORS_CONTRAST), 2.0f);
                    ivalcontrast = 2000;
                }

                GetDlgItemText(hwnd, IDC_VIDEO_COLORS_BRIGHTNESS, s, 100);
                _stscanf(s, TEXT("%f"), &tf);
                ivalbrightness = (int)(tf * 1000.0 + 0.5);
                if (ivalbrightness < 0) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_COLORS_BRIGHTNESS), 0.0f);
                    ivalbrightness = 0;
                }
                if (ivalbrightness > 2000) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_COLORS_BRIGHTNESS), 2.0f);
                    ivalbrightness = 2000;
                }

                resources_set_int("ColorGamma", ivalgamma);
                resources_set_int("ColorTint", ivaltint);
                resources_set_int("ColorSaturation", ivalsaturation);
                resources_set_int("ColorContrast", ivalcontrast);
                resources_set_int("ColorBrightness", ivalbrightness);
                querynewpalette = 1;
                SetWindowLongPtr(hwnd, DWLP_MSGRESULT, FALSE);
                return TRUE;
            }
            return FALSE;
        case WM_INITDIALOG:
            init_color_dialog(hwnd);
            return TRUE;
        case WM_COMMAND:
            type = LOWORD(wparam);
            switch (type) {
                case IDC_VIDEO_COLORS_GAMMA:
                case IDC_VIDEO_COLORS_TINT:
                case IDC_VIDEO_COLORS_SATURATION:
                case IDC_VIDEO_COLORS_CONTRAST:
                case IDC_VIDEO_COLORS_BRIGHTNESS:
                    break;
            }
            return TRUE;
    }
    return FALSE;
}

static INT_PTR CALLBACK dialog_crt_emulation_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    int type;
    int ivalscanlineshade;
    int ivalblur;
    int ivaloddlinephase;
    int ivaloddlineoffset;
    float tf;
    TCHAR s[100];

    switch (msg) {
        case WM_NOTIFY:
            if (((NMHDR FAR *)lparam)->code == PSN_APPLY) {
                GetDlgItemText(hwnd, IDC_VIDEO_CRT_SCANLINE_SHADE, s, 100);
                _stscanf(s, TEXT("%f"), &tf);
                ivalscanlineshade = (int)(tf * 1000.0 + 0.5);
                if (ivalscanlineshade < 0) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_CRT_SCANLINE_SHADE), 0.0f);
                    ivalscanlineshade = 0;
                }
                if (ivalscanlineshade > 1000) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_CRT_SCANLINE_SHADE), 1.0f);
                    ivalscanlineshade = 1000;
                }

                GetDlgItemText(hwnd, IDC_VIDEO_CRT_BLUR, s, 100);
                _stscanf(s, TEXT("%f"), &tf);
                ivalblur = (int)(tf * 1000.0 + 0.5);
                if (ivalblur < 0) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_CRT_BLUR), 0.0f);
                    ivalblur = 0;
                }
                if (ivalblur > 1000) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_CRT_BLUR), 1.0f);
                    ivalblur = 1000;
                }

                GetDlgItemText(hwnd, IDC_VIDEO_CRT_ODDLINE_PHASE, s, 100);
                _stscanf(s, TEXT("%f"), &tf);
                ivaloddlinephase = (int)(tf * 1000.0 + 0.5);
                if (ivaloddlinephase < 0) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_CRT_ODDLINE_PHASE), 0.0f);
                    ivaloddlinephase = 0;
                }
                if (ivaloddlinephase > 2000) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_CRT_ODDLINE_PHASE), 2.0f);
                    ivaloddlinephase = 2000;
                }

                GetDlgItemText(hwnd, IDC_VIDEO_CRT_ODDLINE_OFFSET, s, 100);
                _stscanf(s, TEXT("%f"), &tf);
                ivaloddlineoffset = (int)(tf * 1000.0 + 0.5);
                if (ivaloddlineoffset < 0) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_CRT_ODDLINE_OFFSET), 0.0f);
                    ivaloddlineoffset = 0;
                }
                if (ivaloddlineoffset > 2000) {
                    ui_error(translate_text(IDS_VAL_F_FOR_S_OUT_RANGE_USE_F), tf, translate_text(IDS_CRT_ODDLINE_OFFSET), 2.0f);
                    ivaloddlineoffset = 2000;
                }

                resources_set_int("PALScanLineShade", ivalscanlineshade);
                resources_set_int("PALBlur", ivalblur);
                resources_set_int("PALOddLinePhase", ivaloddlinephase);
                resources_set_int("PALOddLineOffset", ivaloddlineoffset);
                querynewpalette = 1;
                SetWindowLongPtr(hwnd, DWLP_MSGRESULT, FALSE);
                return TRUE;
            }
            return FALSE;
        case WM_INITDIALOG:
            init_crt_emulation_dialog(hwnd);
            return TRUE;
        case WM_COMMAND:
            type = LOWORD(wparam);
            switch (type) {
                case IDC_VIDEO_CRT_SCANLINE_SHADE:
                case IDC_VIDEO_CRT_BLUR:
                case IDC_VIDEO_CRT_ODDLINE_PHASE:
                case IDC_VIDEO_CRT_ODDLINE_OFFSET:
                    break;
            }
            return TRUE;
    }
    return FALSE;
}

static INT_PTR CALLBACK dialog_palette_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    int type;
    Chip_Parameters *chip_type = (hwnd == palette_dialog_1) ? current_chip_1 : current_chip_2;

    switch (msg) {
        case WM_NOTIFY:
            if (((NMHDR FAR *)lparam)->code == (UINT)PSN_APPLY) {
                TCHAR st[MAX_PATH];
                GetDlgItemText(hwnd, IDC_VIDEO_CUSTOM_NAME, st, 100);
                lib_free(chip_type->file_name);
                chip_type->file_name = system_wcstombs_alloc(st);
                querynewpalette = 1;
                if (resources_set_string(chip_type->res_PaletteFile_name, chip_type->file_name) < 0) {
                    ui_error(translate_text(IDS_COULD_NOT_LOAD_PALETTE));
                    resources_set_int(chip_type->res_ExternalPalette_name, 0);
                    SetWindowLongPtr(hwnd, DWLP_MSGRESULT, TRUE);
                    lib_free(chip_type->file_name);
                    chip_type->file_name = NULL;
                    return TRUE;
                }
                lib_free(chip_type->file_name);
                chip_type->file_name = NULL;
                resources_set_int(chip_type->res_ExternalPalette_name, chip_type->external_pal);
                palette_dialog_1 = NULL;
                SetWindowLongPtr(hwnd, DWLP_MSGRESULT, FALSE);
                return TRUE;
            }
            return FALSE;
        case WM_INITDIALOG:
            init_palette_dialog(hwnd, (Chip_Parameters*)((PROPSHEETPAGE*)lparam)->lParam);
            return TRUE;
        case WM_COMMAND:
            type = LOWORD(wparam);
            switch (type) {
                case IDC_TOGGLE_VIDEO_EXTPALETTE:
                    chip_type->external_pal = !chip_type->external_pal;
                    EnableWindow(GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_BROWSE), chip_type->external_pal);
                    EnableWindow(GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_NAME), chip_type->external_pal);
                    break;
                case IDC_VIDEO_CUSTOM_BROWSE:
                    {
                        TCHAR *st_name;

                        if ((st_name = uilib_select_file(hwnd, translate_text(IDS_LOAD_VICE_PALETTE_FILE), UILIB_FILTER_ALL | UILIB_FILTER_PALETTE,
                                                         UILIB_SELECTOR_TYPE_FILE_LOAD, UILIB_SELECTOR_STYLE_DEFAULT)) != NULL) {
                            char *name;

                            SetDlgItemText(hwnd, IDC_VIDEO_CUSTOM_NAME, st_name);
                            name = system_wcstombs_alloc(st_name);
                            lib_free(chip_type->file_name);
                            chip_type->file_name = name;
                            chip_type->external_pal = 1;
                            CheckDlgButton(hwnd, IDC_TOGGLE_VIDEO_EXTPALETTE, BST_CHECKED);
                            lib_free(st_name);
                        }
                    }
                    break;
                case IDC_VIDEO_CUSTOM_NAME:
                    {
                        TCHAR st[100];

                        GetDlgItemText(hwnd, IDC_VIDEO_CUSTOM_NAME, st, 100);
                        lib_free(chip_type->file_name);
                        chip_type->file_name = system_wcstombs_alloc(st);

                        chip_type->external_pal = 1;
                        CheckDlgButton(hwnd, IDC_TOGGLE_VIDEO_EXTPALETTE, BST_CHECKED);

                        break;
                    }
            }
            return TRUE;
    }
    return FALSE;
}

static INT_PTR CALLBACK dialog_render_filter_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    int type;
    int index;
    int crt;
    int scale2x;

    switch (msg) {
        case WM_NOTIFY:
            if (((NMHDR FAR *)lparam)->code == (UINT)PSN_APPLY) {
                index = (int)SendMessage(GetDlgItem(hwnd, IDC_VIDEO_RENDER_FILTER), CB_GETCURSEL, 0, 0);
                switch (index) {
                    case 0:
                    default:
                        crt = 0;
                        scale2x = 0;
                        break;
                    case 1:
                        crt = 1;
                        scale2x = 0;
                        break;
                    case 2:
                        crt = 0;
                        scale2x = 1;
                        break;
                }
                resources_set_int("PALEmulation", crt);
                if (current_chip_1->scale2x_resource != NULL) {
                    resources_set_int(current_chip_1->scale2x_resource, scale2x);
                }
                SetWindowLongPtr(hwnd, DWLP_MSGRESULT, FALSE);
                return TRUE;
            }
            return FALSE;
        case WM_INITDIALOG:
            init_render_filter_dialog(hwnd, (Chip_Parameters*)((PROPSHEETPAGE*)lparam)->lParam);
            return TRUE;
        case WM_COMMAND:
            type = LOWORD(wparam);
            switch (type) {
                case IDC_VIDEO_RENDER_FILTER:
                    return FALSE;
            }
            return TRUE;
    }
    return FALSE;
}

void ui_video_settings_dialog(HWND hwnd, int chip_type1, int chip_type2)
{
    PROPSHEETPAGE psp[6];
    PROPSHEETHEADER psh;
    int i;
    Chip_Parameters *chip_param;

    for (i = 0; i < 6; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
        psp[i].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
        psp[i].pszIcon = NULL;
#else
        psp[i].u2.pszIcon = NULL;
#endif
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }

    chip_param = &chip_param_table[chip_type1];

    psp[0].pfnDlgProc = dialog_fullscreen_proc;
    psp[0].pszTitle = translate_text(IDS_FULLSCREEN);
    psp[1].pfnDlgProc = dialog_palette_proc;
    psp[1].pszTitle = system_mbstowcs_alloc(translate_text(chip_param->page_title));
    psp[1].lParam = (LPARAM)chip_param;
    current_chip_1 = chip_param;
    psp[2].pfnDlgProc = dialog_crt_emulation_proc;
    psp[2].pszTitle = translate_text(IDS_CRT_EMULATION);
    psp[3].pfnDlgProc = dialog_color_proc;
    psp[3].pszTitle = translate_text(IDS_COLORS);
    psp[4].pfnDlgProc = dialog_render_filter_proc;
    psp[4].pszTitle = translate_text(IDS_RENDER_FILTER);
    psp[4].lParam = (LPARAM)chip_param;

#ifdef _ANONYMOUS_UNION
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_FULLSCREEN_SETTINGS_DIALOG);
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_VIDEO_PALETTE_DIALOG);
    psp[2].pszTemplate = MAKEINTRESOURCE(IDD_VIDEO_CRT_EMULATION_DIALOG);
    psp[3].pszTemplate = MAKEINTRESOURCE(IDD_VIDEO_COLORS_DIALOG);
    psp[4].pszTemplate = MAKEINTRESOURCE(IDD_RENDER_FILTER_DIALOG);
#else
    psp[0].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_FULLSCREEN_SETTINGS_DIALOG);
    psp[1].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_VIDEO_PALETTE_DIALOG);
    psp[2].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_VIDEO_CRT_EMULATION_DIALOG);
    psp[3].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_VIDEO_COLORS_DIALOG);
    psp[4].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_RENDER_FILTER_DIALOG);
#endif
    psh.nPages = 5;

    if (chip_type2 != UI_VIDEO_CHIP_NONE) {
        chip_param = &chip_param_table[chip_type2];
        current_chip_2 = chip_param;

        psp[5].pfnDlgProc = dialog_palette_proc;
        psp[5].pszTitle = system_mbstowcs_alloc(translate_text(chip_param->page_title));
        psp[5].lParam = (LPARAM)chip_param;

#ifdef _ANONYMOUS_UNION
        psp[5].pszTemplate = MAKEINTRESOURCE(IDD_VIDEO_PALETTE_DIALOG);
#else
        psp[5].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_VIDEO_PALETTE_DIALOG);
#endif
        psh.nPages++;
    }

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = translate_text(IDS_VIDEO_SETTINGS);
#ifdef _ANONYMOUS_UNION
    psh.pszIcon = NULL;
    psh.nStartPage = 0;
    psh.ppsp = psp;
#else
    psh.DUMMYUNIONNAME.pszIcon = NULL;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
#endif
    psh.pfnCallback = NULL;

    PropertySheet(&psh);
}
