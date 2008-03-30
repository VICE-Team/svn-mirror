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

#ifdef HAVE_SHLOBJ_H
#include <shlobj.h>
#endif

#ifndef DUMMYUNIONNAME
#define DUMMYUNIONNAME  u1
#endif

#include "res.h"
#include "resources.h"
#include "ui.h"
#include "uilib.h"
#include "uivideo.h"
#include "fullscreen.h"
#include "utils.h"
#include "winmain.h"

static char *palette_file=NULL;
static int  res_extpalette;

static char *palettes[8]=
{
	"default",
	"c64s",
	"ccs64",
	"frodo",
	"godot",
	"pc64",
	"vice",
	NULL
};

static char *modes[4]=
{
	"Fast PAL",
	"Y/C cable (sharp)",
	"Composite (blurry)",
	NULL
};

static void enable_controls_for_video_settings(HWND hwnd, int type)
{
    EnableWindow(GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_NAME),
                 type == IDC_SELECT_VIDEO_CUSTOM);
    EnableWindow(GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_BROWSE),
                 type == IDC_SELECT_VIDEO_CUSTOM);
}

static void init_color_dialog(HWND hwnd)
{
    int val;
	double fval;
	char newval[64];

    resources_get_value("ColorSaturation", (resource_value_t *) &val);
	fval=((double)val)/1000.0;
	sprintf(newval,"%.3f",(float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_SAT, newval);

    resources_get_value("ColorContrast", (resource_value_t *) &val);
	fval=((double)val)/1000.0;
	sprintf(newval,"%.3f",(float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_CON, newval);

    resources_get_value("ColorBrightness", (resource_value_t *) &val);
	fval=((double)val)/1000.0;
	sprintf(newval,"%.3f",(float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_BRI, newval);

}

static void init_advanced_dialog(HWND hwnd)
{
    int n,val;
	double fval;
	char newval[64];
	char *path;
	HWND filename_hwnd;

    resources_get_value("ColorGamma", (resource_value_t *) &val);
	fval=((double)val)/1000.0;
	sprintf(newval,"%.3f",(float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_GAM, newval);

    /* As long as 'phase' isn't implemented, set a constant entry  */ 
    SetDlgItemText(hwnd, IDC_VIDEO_COLORS_PHA, "N/A");

    resources_get_value("PALScanLineShade", (resource_value_t *) &val);
	fval=((double)val)/1000.0;
	sprintf(newval,"%.3f",(float)fval);
    SetDlgItemText(hwnd, IDC_VIDEO_ADVANCED_SHADE, newval);

    filename_hwnd=GetDlgItem(hwnd, IDC_VIDEO_ADVANCED_MODE);
    SendMessage(filename_hwnd,CB_RESETCONTENT,0,0);
	n=0;
	while (modes[n] != NULL)
	{
        SendMessage(filename_hwnd,CB_ADDSTRING,0,(LPARAM)modes[n]);
		n++;
	}
    resources_get_value("PALMode", (resource_value_t *) &val);
    SendMessage(filename_hwnd, CB_SETCURSEL, (WPARAM)val, 0);

    resources_get_value("ExternalPalette", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_VIDEO_EXTPALETTE, n ? BST_CHECKED : BST_UNCHECKED);
    res_extpalette=n;

    filename_hwnd=GetDlgItem(hwnd, IDC_VIDEO_CUSTOM_NAME);
    SendMessage(filename_hwnd,CB_RESETCONTENT,0,0);
	n=0;
	while (palettes[n] != NULL)
	{
        SendMessage(filename_hwnd,CB_ADDSTRING,0,(LPARAM)palettes[n]);
		n++;
	}
    resources_get_value("PaletteFile", (resource_value_t *) &path);
    palette_file = stralloc(path);
    SetDlgItemText(hwnd, IDC_VIDEO_CUSTOM_NAME, path);
}

static void update_palettename(char *name)
{
    free(palette_file);
    palette_file = stralloc(name);
}

static BOOL CALLBACK dialog_color_proc(HWND hwnd, UINT msg,
                                       WPARAM wparam, LPARAM lparam)
{
    int type,ival;
    char s[100];
    extern int querynewpalette;

    switch (msg) {
        case WM_NOTIFY:
            if (((NMHDR FAR *)lparam)->code==PSN_APPLY) {
                GetDlgItemText(hwnd, IDC_VIDEO_COLORS_SAT, (LPSTR)s, 100);
                ival=(int)(atof(s)*1000.0+0.5);
                resources_set_value("ColorSaturation", (resource_value_t) ival);

                GetDlgItemText(hwnd, IDC_VIDEO_COLORS_CON, (LPSTR)s, 100);
                ival=(int)(atof(s)*1000.0+0.5);
                resources_set_value("ColorContrast", (resource_value_t) ival);

                GetDlgItemText(hwnd, IDC_VIDEO_COLORS_BRI, (LPSTR)s, 100);
                ival=(int)(atof(s)*1000.0+0.5);
                resources_set_value("ColorBrightness", (resource_value_t) ival);

                querynewpalette = 1;
                SetWindowLong (hwnd, DWL_MSGRESULT, FALSE);
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

static BOOL CALLBACK dialog_advanced_proc(HWND hwnd, UINT msg,
                                          WPARAM wparam, LPARAM lparam)
{
    int type,ival;
    char s[100];
    extern int querynewpalette;

    switch (msg) {
        case WM_NOTIFY:
            if (((NMHDR FAR *)lparam)->code==PSN_APPLY) {
                GetDlgItemText(hwnd, IDC_VIDEO_COLORS_GAM, (LPSTR)s, 100);
                ival=(int)(atof(s)*1000.0+0.5);
                resources_set_value("ColorGamma", (resource_value_t) ival);

                resources_set_value("ExternalPalette", (resource_value_t) res_extpalette);

                GetDlgItemText(hwnd, IDC_VIDEO_ADVANCED_SHADE, (LPSTR)s, 100);
                ival=(int)(atof(s)*1000.0+0.5);
                resources_set_value("PALScanLineShade", (resource_value_t) ival);

				ival = SendMessage(GetDlgItem(hwnd, IDC_VIDEO_ADVANCED_MODE), CB_GETCURSEL, 0, 0);
                resources_set_value("PALMode", (resource_value_t) ival);

                querynewpalette = 1;
                if (resources_set_value("PaletteFile", (resource_value_t) palette_file) < 0) {
                    ui_error("Could not load palette file.");
                    resources_set_value("ExternalPalette", (resource_value_t) res_extpalette);
                    SetWindowLong (hwnd, DWL_MSGRESULT, TRUE);
                    return TRUE;
                }
                free(palette_file);
                palette_file=NULL;
                resources_set_value("ExternalPalette", (resource_value_t) res_extpalette);
                SetWindowLong (hwnd, DWL_MSGRESULT, FALSE);
                return TRUE;
            }
            return FALSE;
    case WM_INITDIALOG:
            init_advanced_dialog(hwnd);
            return TRUE;
        case WM_COMMAND:
            type = LOWORD(wparam);
            switch (type) {
                case IDC_TOGGLE_VIDEO_EXTPALETTE:
					res_extpalette = !res_extpalette;
                    break;
                case IDC_VIDEO_COLORS_GAM:
                    break;
                case IDC_VIDEO_CUSTOM_BROWSE:
                    {
                    char *s;
                        if ((s = ui_select_file(hwnd,"Load VICE palette file",
                            UI_LIB_FILTER_ALL | UI_LIB_FILTER_PALETTE,
                            FILE_SELECTOR_DEFAULT_STYLE,NULL)) != NULL) {
                            update_palettename(s);
                            SetDlgItemText(hwnd, IDC_VIDEO_CUSTOM_NAME, s);
                            free(s);
							res_extpalette = 1;
							CheckDlgButton(hwnd, IDC_TOGGLE_VIDEO_EXTPALETTE, BST_CHECKED);
                        }
                    }
                    break;
                case IDC_VIDEO_CUSTOM_NAME:
                    {
                    char s[100];
                        GetDlgItemText(hwnd, IDC_VIDEO_CUSTOM_NAME, (LPSTR)s, 100);
                        update_palettename(s);
/*
						res_extpalette = 1;
						CheckDlgButton(hwnd, IDC_TOGGLE_VIDEO_EXTPALETTE, BST_CHECKED);
*/
                        break;
                    }
            }
            return TRUE;
    }
    return FALSE;
}

void ui_video_settings_dialog(HWND hwnd, int videosettings)
{
PROPSHEETPAGE       psp[3];
PROPSHEETHEADER     psh;
int                 i;

    for (i = 0; i < 3; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
        psp[i].hInstance = winmain_instance;
#ifdef HAVE_UNNAMED_UNIONS
        psp[i].pszIcon = NULL;
#else
        psp[i].u2.pszIcon = NULL;
#endif
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }

	if (videosettings == UI_VIDEO_PAL)
	{
	    psp[0].pfnDlgProc = dialog_color_proc;
	    psp[0].pszTitle = "Colors";
	    psp[1].pfnDlgProc = dialog_fullscreen_proc;
	    psp[1].pszTitle = "Fullscreen";
	    psp[2].pfnDlgProc = dialog_advanced_proc;
	    psp[2].pszTitle = "Advanced";

#ifdef HAVE_UNNAMED_UNIONS
	    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_VIDEO_COLORS_DIALOG);
	    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_FULLSCREEN_SETTINGS_DIALOG);
	    psp[2].pszTemplate = MAKEINTRESOURCE(IDD_VIDEO_ADVANCED_DIALOG);
#else
	    psp[0].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_VIDEO_COLORS_DIALOG);
	    psp[1].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_FULLSCREEN_SETTINGS_DIALOG);
	    psp[2].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_VIDEO_ADVANCED_DIALOG);
#endif
	    psh.nPages = 3;
	}
	else
	{
	    psp[0].pfnDlgProc = dialog_fullscreen_proc;
	    psp[0].pszTitle = "Fullscreen";

#ifdef HAVE_UNNAMED_UNIONS
	    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_FULLSCREEN_SETTINGS_DIALOG);
#else
	    psp[0].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_FULLSCREEN_SETTINGS_DIALOG);
#endif
	    psh.nPages = 1;
	}

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = "Video settings";
#ifdef HAVE_UNNAMED_UNIONS
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

