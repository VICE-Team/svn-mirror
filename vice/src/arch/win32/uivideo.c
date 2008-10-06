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

#ifndef DUMMYUNIONNAME
#define DUMMYUNIONNAME  u1
#endif

#include "intl.h"
#include "lib.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "ui.h"
#include "uilib.h"
#include "uivideo.h"
#include "fullscrn.h"
#include "winmain.h"


static char *palette_file = NULL;
static char *palette_file2 = NULL;
static int  res_extpalette;

static const char *vicii_palettes[]=
{
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

static const char *vic_palettes[] =
{
    "default",
    NULL
};

static const char *crtc_palettes[] =
{
    "amber",
    "green",
    "white",
    NULL
};

static const char *vdc_palettes[] =
{
    "vdc_deft",
    NULL
};

static const char *ted_palettes[] =
{
    "default",
    "vice",
    NULL
};

typedef struct {
    const char **palette_names;
    char *res_PaletteFile_name;
    char *res_ExternalPalette_name;
    int  palette_mode;
    char *page_title;
} Chip_Parameters;

static Chip_Parameters chip_param_table[] =
{
    { vicii_palettes, "VICIIPaletteFile", "VICIIExternalPalette",
      UI_VIDEO_PAL, "VICII Palette"},
    { vic_palettes, "VICPaletteFile", "VICExternalPalette",
      UI_VIDEO_PAL, "VIC Palette"},
    { crtc_palettes, "CRTCPaletteFile", NULL,
      UI_VIDEO_RGB, "CRTC Palette"},
    { vdc_palettes, "VDCPaletteFile", NULL,
      UI_VIDEO_RGB, "VDC Palette"},
    { ted_palettes, "TEDPaletteFile", "TEDExternalPalette",
      UI_VIDEO_PAL, "TED Palette"},
};

/*
static void enable_controls_for_video_settings(HWND hwnd, int type)
{
    EnableWindow(GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_NAME),
                 type == IDC_SELECT_VIDEO_CUSTOM);
    EnableWindow(GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_BROWSE),
                 type == IDC_SELECT_VIDEO_CUSTOM);
}
*/

static void init_color_dialog(HWND hwnd)
{
    int val;
    double fval;
    TCHAR newval[64];

    resources_get_int("ColorSaturation", &val);
                      fval = ((double)val) / 1000.0;
                      _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_SAT, newval);

    resources_get_int("ColorContrast", &val);
                      fval = ((double)val) / 1000.0;
                      _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_CON, newval);

    resources_get_int("ColorBrightness", &val);
                      fval = ((double)val) / 1000.0;
                      _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_BRI, newval);

}

static void init_new_pal_dialog(HWND hwnd)
{
    int val;
    double fval;
    TCHAR newval[64];

    resources_get_int("ColorTint", &val);
                      fval = ((double)val) / 1000.0;
                      _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_NEW_PAL_TINT, newval);

    resources_get_int("PALOddLinePhase", &val);
                      fval = ((double)val) / 1000.0;
                      _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_NEW_PAL_PHASE, newval);

    resources_get_int("PALOddLineOffset", &val);
                      fval = ((double)val) / 1000.0;
                      _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_NEW_PAL_OFFSET, newval);

}

static Chip_Parameters *current_chip;
static Chip_Parameters *current_chip2;

static void init_advanced_dialog(HWND hwnd, Chip_Parameters *chip_type)
{
    int n, val;
    double fval;
    TCHAR newval[64];
    const char *path;
    TCHAR *st_path;
    HWND filename_hwnd;

    current_chip = chip_type;

    resources_get_int("ColorGamma", &val);
                        fval = ((double)val) / 1000.0;
                        _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_GAM, newval);

    /* As long as 'phase' isn't implemented, set a constant entry  */
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_PHA, TEXT("N/A"));

    resources_get_int("PALScanLineShade", &val);
                        fval = ((double)val) / 1000.0;
                        _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_ADVANCED_SHADE, newval);

    resources_get_int("PALBlur", &val);
                        fval = ((double)val) / 1000.0;
                        _stprintf(newval, TEXT("%.3f"), (float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_ADVANCED_BLUR, newval);

    if (chip_type->res_ExternalPalette_name) {
        resources_get_int(chip_type->res_ExternalPalette_name, &n);
        CheckDlgButton(hwnd, IDC_TOGGLE_VIDEO_EXTPALETTE, n
                       ? BST_CHECKED : BST_UNCHECKED);
        res_extpalette = n;
    } else {
        res_extpalette = 0;
        EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_VIDEO_EXTPALETTE), FALSE);
    }

    EnableWindow(GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_BROWSE), res_extpalette);
    EnableWindow(GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_NAME), res_extpalette);

    filename_hwnd = GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_NAME);
    SendMessage(filename_hwnd, CB_RESETCONTENT, 0, 0);
    n = 0 ;
    while (chip_type->palette_names[n] != NULL) {
        SendMessage(filename_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)chip_type->palette_names[n]);
        n++;
    }
    resources_get_string(chip_type->res_PaletteFile_name, &path);
    palette_file = lib_stralloc(path);
    st_path = system_mbstowcs_alloc(path);
    SetDlgItemText(hwnd, IDC_VIDEO_CUSTOM_NAME, st_path);
    system_mbstowcs_free(st_path);
}

static void init_palette_dialog(HWND hwnd, Chip_Parameters *chip_type)
{
    int n;
    const char *path;
    TCHAR *st_path;
    HWND filename_hwnd;

    current_chip2 = chip_type;

    filename_hwnd = GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_NAME);
    SendMessage(filename_hwnd, CB_RESETCONTENT, 0, 0);
    n = 0 ;
    while (chip_type->palette_names[n] != NULL) {
        SendMessage(filename_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)chip_type->palette_names[n]);
        n++;
    }
    resources_get_string(chip_type->res_PaletteFile_name, &path);
    palette_file2 = lib_stralloc(path);
    st_path = system_mbstowcs_alloc(path);
    SetDlgItemText(hwnd, IDC_VIDEO_CUSTOM_NAME, st_path);
    system_mbstowcs_free(st_path);
}

static void update_palettename(char *name)
{
    lib_free(palette_file);
    palette_file = lib_stralloc(name);
}

static void update_palettename2(char *name)
{
    lib_free(palette_file2);
    palette_file2 = lib_stralloc(name);
}

static BOOL CALLBACK dialog_color_proc(HWND hwnd, UINT msg,
                                       WPARAM wparam, LPARAM lparam)
{
    int type, ival;
    float tf;
    TCHAR s[100];
    extern int querynewpalette;

    switch (msg) {
      case WM_NOTIFY:
        if (((NMHDR FAR *)lparam)->code == (UINT)PSN_APPLY) {
            GetDlgItemText(hwnd, IDC_VIDEO_COLORS_SAT, s, 100);
            _stscanf(s, TEXT("%f"), &tf);
            ival = (int)(tf * 1000.0 + 0.5);
            resources_set_int("ColorSaturation", ival);
            GetDlgItemText(hwnd, IDC_VIDEO_COLORS_CON, s, 100);
            _stscanf(s, TEXT("%f"), &tf);
            ival = (int)(tf * 1000.0 + 0.5);
            resources_set_int("ColorContrast", ival);
            GetDlgItemText(hwnd, IDC_VIDEO_COLORS_BRI, s, 100);
            _stscanf(s, TEXT("%f"), &tf);
            ival = (int)(tf * 1000.0 + 0.5);
            resources_set_int("ColorBrightness", ival);
            querynewpalette = 1;
            SetWindowLong(hwnd, DWL_MSGRESULT, FALSE);
            return TRUE;
        }
        return FALSE;
      case WM_INITDIALOG:
        init_color_dialog(hwnd);
        return TRUE;
      case WM_COMMAND:
        type = LOWORD(wparam);
        switch (type) {
          case IDC_VIDEO_COLORS_SAT:
          case IDC_VIDEO_COLORS_CON:
          case IDC_VIDEO_COLORS_BRI:
            break;
        }
        return TRUE;
    }
    return FALSE;
}

static BOOL CALLBACK dialog_new_pal_proc(HWND hwnd, UINT msg,
                                       WPARAM wparam, LPARAM lparam)
{
    int type, ival;
    float tf;
    TCHAR s[100];
    extern int querynewpalette;

    switch (msg) {
      case WM_NOTIFY:
        if (((NMHDR FAR *)lparam)->code == PSN_APPLY) {
            GetDlgItemText(hwnd, IDC_VIDEO_NEW_PAL_TINT, s, 100);
            _stscanf(s, TEXT("%f"), &tf);
            ival = (int)(tf * 1000.0 + 0.5);
            resources_set_int("ColorTint", ival);
            GetDlgItemText(hwnd, IDC_VIDEO_NEW_PAL_PHASE, s, 100);
            _stscanf(s, TEXT("%f"), &tf);
            ival = (int)(tf * 1000.0 + 0.5);
            resources_set_int("PALOddLinePhase", ival);
            GetDlgItemText(hwnd, IDC_VIDEO_NEW_PAL_OFFSET, s, 100);
            _stscanf(s, TEXT("%f"), &tf);
            ival = (int)(tf * 1000.0 + 0.5);
            resources_set_int("PALOddLineOffset", ival);
            querynewpalette = 1;
            SetWindowLong(hwnd, DWL_MSGRESULT, FALSE);
            return TRUE;
        }
        return FALSE;
      case WM_INITDIALOG:
        init_new_pal_dialog(hwnd);
        return TRUE;
      case WM_COMMAND:
        type = LOWORD(wparam);
        switch (type) {
          case IDC_VIDEO_NEW_PAL_TINT:
          case IDC_VIDEO_NEW_PAL_PHASE:
          case IDC_VIDEO_NEW_PAL_OFFSET:
            break;
        }
        return TRUE;
    }
    return FALSE;
}

static BOOL CALLBACK dialog_advanced_proc(HWND hwnd, UINT msg,
                                          WPARAM wparam, LPARAM lparam)
{
    int type, ival;
    float tf;
    TCHAR s[100];
    extern int querynewpalette;

    switch (msg) {
      case WM_NOTIFY:
        if (((NMHDR FAR *)lparam)->code == (UINT)PSN_APPLY) {
            GetDlgItemText(hwnd, IDC_VIDEO_COLORS_GAM, s, 100);
            _stscanf(s, TEXT("%f"), &tf);
            ival = (int)(tf * 1000.0 + 0.5);
            resources_set_int("ColorGamma", ival);

            resources_set_int(current_chip->res_ExternalPalette_name,
                              res_extpalette);

            GetDlgItemText(hwnd, IDC_VIDEO_ADVANCED_SHADE, s, 100);
            _stscanf(s, TEXT("%f"), &tf);
            ival = (int)(tf * 1000.0 + 0.5);
            resources_set_int("PALScanLineShade", ival);

            GetDlgItemText(hwnd, IDC_VIDEO_ADVANCED_BLUR, s, 100);
            _stscanf(s, TEXT("%f"), &tf);
            ival = (int)(tf * 1000.0 + 0.5);
            resources_set_int("PALBlur", ival);

            querynewpalette = 1;
            if (resources_set_string(current_chip->res_PaletteFile_name,
                palette_file) < 0) {
                ui_error(translate_text(IDS_COULD_NOT_LOAD_PALETTE));
                resources_set_int(current_chip->res_ExternalPalette_name,
                                  res_extpalette);
                SetWindowLong (hwnd, DWL_MSGRESULT, TRUE);
                return TRUE;
            }
            lib_free(palette_file);
            palette_file = NULL;
            resources_set_int(current_chip->res_ExternalPalette_name,
                              res_extpalette);
            SetWindowLong(hwnd, DWL_MSGRESULT, FALSE);
            return TRUE;
        }
        return FALSE;
      case WM_INITDIALOG:
        init_advanced_dialog(hwnd, (Chip_Parameters*)((PROPSHEETPAGE*)lparam)->lParam);
        return TRUE;
      case WM_COMMAND:
        type = LOWORD(wparam);
        switch (type) {
          case IDC_TOGGLE_VIDEO_EXTPALETTE:
            res_extpalette = !res_extpalette;
            EnableWindow(GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_BROWSE),
                         res_extpalette);
            EnableWindow(GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_NAME),
                         res_extpalette);
            break;
          case IDC_VIDEO_COLORS_GAM:
            break;
          case IDC_VIDEO_CUSTOM_BROWSE:
            {
                TCHAR *st_name;

                if ((st_name = uilib_select_file(hwnd,
                    translate_text(IDS_LOAD_VICE_PALETTE_FILE),
                    UILIB_FILTER_ALL | UILIB_FILTER_PALETTE,
                    UILIB_SELECTOR_TYPE_FILE_LOAD,
                    UILIB_SELECTOR_STYLE_DEFAULT)) != NULL) {
                    char *name;

                    SetDlgItemText(hwnd, IDC_VIDEO_CUSTOM_NAME, st_name);
                    name = system_wcstombs_alloc(st_name);
                    update_palettename(name);
                    system_wcstombs_free(name);
                    res_extpalette = 1;
                    CheckDlgButton(hwnd, IDC_TOGGLE_VIDEO_EXTPALETTE,
                                   BST_CHECKED);
                    lib_free(st_name);
                }
            }
            break;
          case IDC_VIDEO_CUSTOM_NAME:
            {
                TCHAR st[100];
                char s[100];

                GetDlgItemText(hwnd, IDC_VIDEO_CUSTOM_NAME, st, 100);
                system_wcstombs(s, st, 100);
                update_palettename(s);

                res_extpalette = 1;
                CheckDlgButton(hwnd, IDC_TOGGLE_VIDEO_EXTPALETTE, BST_CHECKED);

                break;
            }
        }
        return TRUE;
    }
    return FALSE;
}

static BOOL CALLBACK dialog_palette_proc(HWND hwnd, UINT msg,
                                          WPARAM wparam, LPARAM lparam)
{
    int type;
    extern int querynewpalette;

    switch (msg) {
      case WM_NOTIFY:
        if (((NMHDR FAR *)lparam)->code == (UINT)PSN_APPLY) {
            querynewpalette = 1;
            if (resources_set_string(current_chip2->res_PaletteFile_name,
                palette_file2) < 0) {
                ui_error(translate_text(IDS_COULD_NOT_LOAD_PALETTE));
                SetWindowLong (hwnd, DWL_MSGRESULT, TRUE);
                return TRUE;
            }
            lib_free(palette_file2);
            palette_file2 = NULL;
            SetWindowLong (hwnd, DWL_MSGRESULT, FALSE);
            return TRUE;
        }
        return FALSE;
      case WM_INITDIALOG:
        init_palette_dialog(hwnd,
                            (Chip_Parameters*)((PROPSHEETPAGE*)lparam)->lParam);
        return TRUE;
      case WM_COMMAND:
        type = LOWORD(wparam);
        switch (type) {
          case IDC_VIDEO_CUSTOM_BROWSE:
            {
                TCHAR *st_name;

                if ((st_name = uilib_select_file(hwnd,
                    translate_text(IDS_LOAD_VICE_PALETTE_FILE),
                    UILIB_FILTER_ALL | UILIB_FILTER_PALETTE,
                    UILIB_SELECTOR_TYPE_FILE_LOAD,
                    UILIB_SELECTOR_STYLE_DEFAULT)) != NULL) {
                    char *name;

                    SetDlgItemText(hwnd, IDC_VIDEO_CUSTOM_NAME, st_name);
                    name = system_wcstombs_alloc(st_name);
                    update_palettename2(name);
                    system_wcstombs_free(name);
                    lib_free(st_name);
                }
            }
            break;
          case IDC_VIDEO_CUSTOM_NAME:
            {
                TCHAR st[100];
                char s[100];

                GetDlgItemText(hwnd, IDC_VIDEO_CUSTOM_NAME, st, 100);
                system_wcstombs(s, st, 100);
                update_palettename2(s);

                break;
            }
        }
        return TRUE;
    }
    return FALSE;
}

void ui_video_settings_dialog(HWND hwnd, int chip_type1, int chip_type2)
{
    PROPSHEETPAGE psp[5];
    PROPSHEETHEADER psh;
    int i;
    Chip_Parameters *chip_param;

    for (i = 0; i < 5; i++) {
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

    if (chip_param->palette_mode == UI_VIDEO_PAL) {
        psp[0].pfnDlgProc = dialog_fullscreen_proc;
        psp[0].pszTitle = translate_text(IDS_FULLSCREEN);
        psp[1].pfnDlgProc = dialog_advanced_proc;
        psp[1].pszTitle = system_mbstowcs_alloc(chip_param->page_title);
        psp[1].lParam = (LPARAM)chip_param;
        psp[2].pfnDlgProc = dialog_new_pal_proc;
        psp[2].pszTitle = translate_text(IDS_NEW_PAL);
        psp[3].pfnDlgProc = dialog_color_proc;
        psp[3].pszTitle = translate_text(IDS_COLORS);

#ifdef _ANONYMOUS_UNION
        psp[0].pszTemplate
            = MAKEINTRESOURCE(translate_res(IDD_FULLSCREEN_SETTINGS_DIALOG));
        psp[1].pszTemplate = MAKEINTRESOURCE(translate_res(IDD_VIDEO_ADVANCED_DIALOG));
        psp[2].pszTemplate = MAKEINTRESOURCE(translate_res(IDD_VIDEO_NEW_PAL_DIALOG));
        psp[3].pszTemplate = MAKEINTRESOURCE(translate_res(IDD_VIDEO_COLORS_DIALOG));
#else
        psp[0].DUMMYUNIONNAME.pszTemplate
            = MAKEINTRESOURCE(translate_res(IDD_FULLSCREEN_SETTINGS_DIALOG));
        psp[1].DUMMYUNIONNAME.pszTemplate
            = MAKEINTRESOURCE(translate_res(IDD_VIDEO_ADVANCED_DIALOG));
        psp[2].DUMMYUNIONNAME.pszTemplate
            = MAKEINTRESOURCE(translate_res(IDD_VIDEO_NEW_PAL_DIALOG));
        psp[3].DUMMYUNIONNAME.pszTemplate
            = MAKEINTRESOURCE(translate_res(IDD_VIDEO_COLORS_DIALOG));
#endif
        psh.nPages = 4;
    } else {
        psp[0].pfnDlgProc = dialog_fullscreen_proc;
        psp[0].pszTitle = translate_text(IDS_FULLSCREEN);
        psp[1].pfnDlgProc = dialog_palette_proc;
        psp[1].pszTitle = system_mbstowcs_alloc(chip_param->page_title);
        psp[1].lParam = (LPARAM)chip_param;

#ifdef _ANONYMOUS_UNION
        psp[0].pszTemplate
            = MAKEINTRESOURCE(translate_res(IDD_FULLSCREEN_SETTINGS_DIALOG));
        psp[1].pszTemplate
            = MAKEINTRESOURCE(translate_res(IDD_VIDEO_PALETTE_DIALOG));
#else
        psp[0].DUMMYUNIONNAME.pszTemplate
            = MAKEINTRESOURCE(translate_res(IDD_FULLSCREEN_SETTINGS_DIALOG));
        psp[1].DUMMYUNIONNAME.pszTemplate
            = MAKEINTRESOURCE(translate_res(IDD_VIDEO_PALETTE_DIALOG));
#endif
        psh.nPages = 2;
    }

    if (chip_type2 != UI_VIDEO_CHIP_NONE) {
        int index = psh.nPages;
    
        chip_param = &chip_param_table[chip_type2];

        psp[index].pfnDlgProc = dialog_palette_proc;
        psp[index].pszTitle = system_mbstowcs_alloc(chip_param->page_title);
        psp[index].lParam = (LPARAM)chip_param;

#ifdef _ANONYMOUS_UNION
        psp[index].pszTemplate
            = MAKEINTRESOURCE(translate_res(IDD_VIDEO_PALETTE_DIALOG));
#else
        psp[index].DUMMYUNIONNAME.pszTemplate
            = MAKEINTRESOURCE(translate_res(IDD_VIDEO_PALETTE_DIALOG));
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

