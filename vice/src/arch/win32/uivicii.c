/*
 * uivicii.c - Implementation of VIC-II settings dialog box.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include "utils.h"
#include "winmain.h"

static char *palette_file=NULL;
static int  res_checksscoll;
static int  res_checksbcoll;
static int  res_newlum;
static int  res_extpalette;

static void enable_controls_for_vicii_settings(HWND hwnd, int type)
{
    EnableWindow(GetDlgItem(hwnd, IDC_VICII_CUSTOM_NAME),
                 type == IDC_SELECT_VICII_CUSTOM);
    EnableWindow(GetDlgItem(hwnd, IDC_VICII_CUSTOM_BROWSE),
                 type == IDC_SELECT_VICII_CUSTOM);
}

/*
static void init_palette_dialog(HWND hwnd)
{
    int n;
    const char *pname;

    resources_get_value("PaletteFile", (resource_value_t *) &pname);
    palette_file = stralloc(pname);

    n = IDC_SELECT_VICII_CUSTOM;

    if (strcmp(pname, "default") == 0)
        n = IDC_SELECT_VICII_DEFAULT;
    if (strcmp(pname, "c64s") == 0)
        n = IDC_SELECT_VICII_C64S;
    if (strcmp(pname, "ccs64") == 0)
        n = IDC_SELECT_VICII_CCS64;
    if (strcmp(pname, "frodo") == 0)
        n = IDC_SELECT_VICII_FRODO;
    if (strcmp(pname, "godot") == 0)
        n = IDC_SELECT_VICII_GODOT;
    if (strcmp(pname, "pc64") == 0)
        n = IDC_SELECT_VICII_PC64;

    if (n == IDC_SELECT_VICII_CUSTOM)
        SetDlgItemText(hwnd, IDC_VICII_CUSTOM_NAME, pname != NULL ? pname : "");

    CheckRadioButton(hwnd, IDC_SELECT_VICII_DEFAULT, IDC_SELECT_VICII_PC64, n);
    enable_controls_for_vicii_settings(hwnd, n);
}
*/

static void init_sprite_dialog(HWND hwnd)
{
    int n;

    resources_get_value("CheckSsColl", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_VICII_SSC, n ? BST_CHECKED : BST_UNCHECKED);
    res_checksscoll=n;

    resources_get_value("CheckSbColl", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_VICII_SBC, n ? BST_CHECKED : BST_UNCHECKED);
    res_checksbcoll=n;
}

static void init_colour_dialog(HWND hwnd)
{
    int n,val;
	double fval;
	char newval[64];
	char *path;

    resources_get_value("NewLuminances", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_VICII_NEWLUM, n ? BST_CHECKED : BST_UNCHECKED);
    res_newlum=n;

    resources_get_value("ExternalPalette", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_VICII_EXTPALETTE, n ? BST_CHECKED : BST_UNCHECKED);
    res_extpalette=n;

    resources_get_value("ColorSaturation", (resource_value_t *) &val);
	fval=((double)val)/1000.0;
	sprintf(newval,"%.3f",(float)fval);
    SetDlgItemText(hwnd, IDC_VICII_COLOURS_SAT, newval);

    resources_get_value("ColorContrast", (resource_value_t *) &val);
	fval=((double)val)/1000.0;
	sprintf(newval,"%.3f",(float)fval);
    SetDlgItemText(hwnd, IDC_VICII_COLOURS_CON, newval);

    resources_get_value("ColorBrightness", (resource_value_t *) &val);
	fval=((double)val)/1000.0;
	sprintf(newval,"%.3f",(float)fval);
    SetDlgItemText(hwnd, IDC_VICII_COLOURS_BRI, newval);

    resources_get_value("ColorGamma", (resource_value_t *) &val);
	fval=((double)val)/1000.0;
	sprintf(newval,"%.3f",(float)fval);
    SetDlgItemText(hwnd, IDC_VICII_COLOURS_GAM, newval);

    resources_get_value("PaletteFile", (resource_value_t *) &path);
    palette_file = stralloc(path);
    SetDlgItemText(hwnd, IDC_VICII_CUSTOM_NAME, path);
}

static void update_palettename(char *name)
{
    free(palette_file);
    palette_file = stralloc(name);
}

/*
static BOOL CALLBACK dialog_palette_proc(HWND hwnd, UINT msg,
                                         WPARAM wparam, LPARAM lparam)
{
    int type;
    extern int querynewpalette;

    switch (msg) {
        case WM_NOTIFY:
            if (((NMHDR FAR *)lparam)->code==PSN_APPLY) {
                if (resources_set_value("PaletteFile", (resource_value_t) palette_file) < 0) {
                    ui_error("Could not load palette file.");
                    SetWindowLong (hwnd, DWL_MSGRESULT, TRUE);
                    return TRUE;
                }
                free(palette_file);
                querynewpalette = 1;
                SetWindowLong (hwnd, DWL_MSGRESULT, FALSE);
                return TRUE;
            }
            return FALSE;
        case WM_INITDIALOG:
            init_palette_dialog(hwnd);
            return TRUE;
        case WM_COMMAND:
            type = LOWORD(wparam);
            switch (type) {
                case IDC_SELECT_VICII_DEFAULT:
                    update_palettename("default");
                    enable_controls_for_vicii_settings(hwnd, type);
                    break;
                case IDC_SELECT_VICII_CUSTOM:
                    {
                    char s[100];
                        GetDlgItemText(hwnd, IDC_VICII_CUSTOM_NAME, (LPSTR)s, 100);
                        update_palettename(s);
                        enable_controls_for_vicii_settings(hwnd, type);
                        break;
                    }
                case IDC_SELECT_VICII_C64S:
                    update_palettename("c64s");
                    enable_controls_for_vicii_settings(hwnd, type);
                    break;
                case IDC_SELECT_VICII_CCS64:
                    update_palettename("ccs64");
                    enable_controls_for_vicii_settings(hwnd, type);
                    break;
                case IDC_SELECT_VICII_FRODO:
                    update_palettename("frodo");
                    enable_controls_for_vicii_settings(hwnd, type);
                    break;
                case IDC_SELECT_VICII_GODOT:
                    update_palettename("godot");
                    enable_controls_for_vicii_settings(hwnd, type);
                    break;
                case IDC_SELECT_VICII_PC64:
                    update_palettename("pc64");
                    enable_controls_for_vicii_settings(hwnd, type);
                    break;
                case IDC_VICII_CUSTOM_BROWSE:
                    {
                    char *s;
                        if ((s = ui_select_file(hwnd,"Load VICE palette file",
                            "VICE palette files (*.vpl)\0*.vpl\0"
                            "All files (*.*)\0*.*\0", FILE_SELECTOR_DEFAULT_STYLE,NULL)) != NULL) {
                            update_palettename(s);
                            SetDlgItemText(hwnd, IDC_VICII_CUSTOM_NAME, s);
                            free(s);
                        }
                    }
                    break;
                case IDC_VICII_CUSTOM_NAME:
                    {
                    char s[100];
                        GetDlgItemText(hwnd, IDC_VICII_CUSTOM_NAME, (LPSTR)s, 100);
                        update_palettename(s);
                        break;
                    }
            }
            return TRUE;
    }
    return FALSE;
}
*/

static BOOL CALLBACK dialog_sprite_proc(HWND hwnd, UINT msg,
                                        WPARAM wparam, LPARAM lparam)
{
    int type;

    switch (msg) {
        case WM_NOTIFY:
            if (((NMHDR FAR *)lparam)->code==PSN_APPLY) {
                resources_set_value("CheckSsColl", (resource_value_t) res_checksscoll);
                resources_set_value("CheckSbColl", (resource_value_t) res_checksbcoll);
                SetWindowLong (hwnd, DWL_MSGRESULT, FALSE);
                return TRUE;
            }
            return FALSE;
        case WM_INITDIALOG:
            init_sprite_dialog(hwnd);
            return TRUE;
        case WM_COMMAND:
            type = LOWORD(wparam);
            switch (type) {
                case IDC_TOGGLE_VICII_SSC:
                    res_checksscoll = !res_checksscoll;
                    break;
                case IDC_TOGGLE_VICII_SBC:
                    res_checksbcoll = !res_checksbcoll;
                    break;
            }
            return TRUE;
    }
    return FALSE;
}

extern void vic_ii_pal_initfilter(float saturation,float brightness,float contrast,float gamma);

static BOOL CALLBACK dialog_colour_proc(HWND hwnd, UINT msg,
                                        WPARAM wparam, LPARAM lparam)
{
    int type,ival;
    char s[100];
    extern int querynewpalette;

    switch (msg) {
        case WM_NOTIFY:
            if (((NMHDR FAR *)lparam)->code==PSN_APPLY) {
                GetDlgItemText(hwnd, IDC_VICII_COLOURS_SAT, (LPSTR)s, 100);
                ival=(int)(atof(s)*1000.0+0.5);
                resources_set_value("ColorSaturation", (resource_value_t) ival);

                GetDlgItemText(hwnd, IDC_VICII_COLOURS_CON, (LPSTR)s, 100);
                ival=(int)(atof(s)*1000.0+0.5);
                resources_set_value("ColorContrast", (resource_value_t) ival);

                GetDlgItemText(hwnd, IDC_VICII_COLOURS_BRI, (LPSTR)s, 100);
                ival=(int)(atof(s)*1000.0+0.5);
                resources_set_value("ColorBrightness", (resource_value_t) ival);

                GetDlgItemText(hwnd, IDC_VICII_COLOURS_GAM, (LPSTR)s, 100);
                ival=(int)(atof(s)*1000.0+0.5);
                resources_set_value("ColorGamma", (resource_value_t) ival);

                resources_set_value("NewLuminances", (resource_value_t) res_newlum);
                resources_set_value("ExternalPalette", (resource_value_t) res_extpalette);

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
            init_colour_dialog(hwnd);
            return TRUE;
        case WM_COMMAND:
            type = LOWORD(wparam);
            switch (type) {
                case IDC_TOGGLE_VICII_NEWLUM:
					res_newlum = !res_newlum;
                    break;
                case IDC_TOGGLE_VICII_EXTPALETTE:
					res_extpalette = !res_extpalette;
                    break;
                case IDC_VICII_COLOURS_SAT:
                case IDC_VICII_COLOURS_CON:
                case IDC_VICII_COLOURS_BRI:
                case IDC_VICII_COLOURS_GAM:
                    break;
                case IDC_VICII_CUSTOM_BROWSE:
                    {
                    char *s;
                        if ((s = ui_select_file(hwnd,"Load VICE palette file",
                            UI_LIB_FILTER_ALL | UI_LIB_FILTER_PALETTE,
                            FILE_SELECTOR_DEFAULT_STYLE,NULL)) != NULL) {
                            update_palettename(s);
                            SetDlgItemText(hwnd, IDC_VICII_CUSTOM_NAME, s);
                            free(s);
                        }
                    }
                    break;
                case IDC_VICII_CUSTOM_NAME:
                    {
                    char s[100];
                        GetDlgItemText(hwnd, IDC_VICII_CUSTOM_NAME, (LPSTR)s, 100);
                        update_palettename(s);
                        break;
                    }
            }
            return TRUE;
    }
    return FALSE;
}

void ui_vicii_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance,(LPCTSTR)IDD_VICII_DIALOG,hwnd,dialog_proc);
}

