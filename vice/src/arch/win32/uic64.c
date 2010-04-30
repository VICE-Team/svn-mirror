/*
 * uic64.c - Implementation of the C64 specific settings dialog box.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>

#include "c64model.h"
#include "res.h"
#include "resources.h"
#include "sid.h"
#include "system.h"
#include "translate.h"
#include "ui.h"
#include "uic64.h"
#include "uidatasette.h"
#include "uilib.h"
#include "vicii.h"
#include "winmain.h"

enum {
    CONTROL_UPDATE_EVERYTHING,
    CONTROL_UPDATE_C64MODEL,
    CONTROL_UPDATE_MODELCHANGE
};

static const TCHAR *ui_c64model[] = {
    TEXT("C64 PAL"),
    TEXT("C64C PAL"),
    TEXT("C64 old PAL"),
    TEXT("C64 NTSC"),
    TEXT("C64C NTSC"),
    TEXT("C64 old NTSC"),
    TEXT("Custom"),
    NULL
};

static const int ui_c64model_values[] = {
    C64MODEL_C64_PAL,
    C64MODEL_C64C_PAL,
    C64MODEL_C64_OLD_PAL,
    C64MODEL_C64_NTSC,
    C64MODEL_C64C_NTSC,
    C64MODEL_C64_OLD_NTSC,
    C64MODEL_UNKNOWN,
    -1
};

static const TCHAR *ui_c64vicii[] = {
    TEXT("6569 (PAL)"),
    TEXT("8565 (PAL)"),
    TEXT("6569R1 (old PAL)"),
    TEXT("6567 (NTSC)"),
    TEXT("8562 (NTSC)"),
    TEXT("6567R56A (old NTSC)"),
    NULL
};

static const int ui_c64vicii_values[] = {
    VICII_MODEL_6569,
    VICII_MODEL_8565,
    VICII_MODEL_6569R1,
    VICII_MODEL_6567,
    VICII_MODEL_8562,
    VICII_MODEL_6567R56A,
    -1
};

/* FIXME: duplicated from uisid.c */
static const TCHAR *ui_c64sid_engine_model[] = {
    TEXT("6581 (FastSID)"),
    TEXT("8580 (FastSID)"),
#ifdef HAVE_RESID
    TEXT("6581 (ReSID)"),
    TEXT("8580 (ReSID)"),
    TEXT("8580 + digi boost (ReSID)"),
#endif
#ifdef HAVE_CATWEASELMKIII
    TEXT("Catweasel MK3"),
#endif
#ifdef HAVE_HARDSID
    TEXT("HardSID"),
#endif
#ifdef HAVE_PARSID
    TEXT("ParSID on Port 1"),
    TEXT("ParSID on Port 2"),
    TEXT("ParSID on Port 3"),
#endif
#ifdef HAVE_RESID_FP
    TEXT("6581R3 4885 (reSID-fp)"),
    TEXT("6581R3 0486S (reSID-fp)"),
    TEXT("6581R3 3984 (reSID-fp)"),
    TEXT("6581R4AR 3789 (reSID-fp)"),
    TEXT("6581R3 4485 (reSID-fp)"),
    TEXT("6581R4 1986S (reSID-fp)"),
    TEXT("8580R5 3691 (reSID-fp)"),
    TEXT("8580R5 3691 + digi boost (reSID-fp)"),
    TEXT("8580R5 1489 (reSID-fp)"),
    TEXT("8580R5 1489 + digi boost (reSID-fp)"),
#endif
    NULL
};

static const int ui_c64sid_engine_model_values[] = {
    SID_FASTSID_6581,
    SID_FASTSID_8580,
#ifdef HAVE_RESID
    SID_RESID_6581,
    SID_RESID_8580,
    SID_RESID_8580D,
#endif
#ifdef HAVE_CATWEASELMKIII
    SID_CATWEASELMKIII,
#endif
#ifdef HAVE_HARDSID
    SID_HARDSID,
#endif
#ifdef HAVE_PARSID
    SID_PARSID_PORT1,
    SID_PARSID_PORT2,
    SID_PARSID_PORT3,
#endif
#ifdef HAVE_RESID_FP
    SID_RESIDFP_6581R3_4885,
    SID_RESIDFP_6581R3_0486S,
    SID_RESIDFP_6581R3_3984,
    SID_RESIDFP_6581R4AR_3789,
    SID_RESIDFP_6581R3_4485,
    SID_RESIDFP_6581R4_1986S,
    SID_RESIDFP_8580R5_3691,
    SID_RESIDFP_8580R5_3691D,
    SID_RESIDFP_8580R5_1489,
    SID_RESIDFP_8580R5_1489D,
#endif
    -1
};

static int vicii_model, sid_model, sid_engine;
static int glue_logic, cia1_model, cia2_model, new_luma;
static int c64_model;


static void uic64_update_controls(HWND hwnd, int mode)
{
    int i, active_value;
    HWND sub_hwnd;
    TCHAR st[256];

    if (mode != CONTROL_UPDATE_MODELCHANGE) {
        sub_hwnd = GetDlgItem(hwnd, IDC_C64MODEL_LIST);
        SendMessage(sub_hwnd, CB_RESETCONTENT, 0, 0);
        for (i = 0; ui_c64model[i]; i++) {
            _stprintf(st, TEXT("%s"), ui_c64model[i]);
            SendMessage(sub_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
            if (ui_c64model_values[i] == c64_model) {
                active_value = i;
            }
        }
        SendMessage(sub_hwnd, CB_SETCURSEL, (WPARAM)active_value, 0);
    }

    if (mode != CONTROL_UPDATE_C64MODEL) {
        sub_hwnd = GetDlgItem(hwnd, IDC_C64VICII_LIST);
        SendMessage(sub_hwnd, CB_RESETCONTENT, 0, 0);
        for (i = 0; ui_c64vicii[i]; i++) {
            _stprintf(st, TEXT("%s"), ui_c64vicii[i]);
            SendMessage(sub_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
            if (ui_c64vicii_values[i] == vicii_model) {
                active_value = i;
            }
        }
        SendMessage(sub_hwnd, CB_SETCURSEL, (WPARAM)active_value, 0);

        sub_hwnd = GetDlgItem(hwnd, IDC_C64SID_LIST);
        SendMessage(sub_hwnd, CB_RESETCONTENT, 0, 0);
        for (i = 0; ui_c64sid_engine_model[i]; i++) {
            _stprintf(st, TEXT("%s"), ui_c64sid_engine_model[i]);
            SendMessage(sub_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
            if (ui_c64sid_engine_model_values[i] == ((sid_engine << 8 ) | sid_model)) {
                active_value = i;
            }
        }
        SendMessage(sub_hwnd, CB_SETCURSEL, (WPARAM)active_value, 0);

        sub_hwnd = GetDlgItem(hwnd, IDC_C64SID_LIST);
        CheckDlgButton(hwnd, IDC_C64LUMINANCES, new_luma ? BST_CHECKED : BST_UNCHECKED);

        CheckRadioButton(hwnd, IDC_C64CIA1_OLD, IDC_C64CIA1_NEW,
                            cia1_model ? IDC_C64CIA1_NEW : IDC_C64CIA1_OLD);

        CheckRadioButton(hwnd, IDC_C64CIA2_OLD, IDC_C64CIA2_NEW,
                            cia2_model ? IDC_C64CIA2_NEW : IDC_C64CIA2_OLD);

        CheckRadioButton(hwnd, IDC_C64GLUE_DISCRETE, IDC_C64GLUE_IC,
                            glue_logic ? IDC_C64GLUE_IC : IDC_C64GLUE_DISCRETE);
    }
}


static void init_c64model_dialog(HWND hwnd)
{
    resources_get_int("VICIIModel", &vicii_model);
    resources_get_int("SidModel", &sid_model);
    resources_get_int("SidEngine", &sid_engine);
    resources_get_int("GlueLogic", &glue_logic);
    resources_get_int("CIA1Model", &cia1_model);
    resources_get_int("CIA2Model", &cia2_model);
    resources_get_int("VICIINewLuminances", &new_luma);

    c64_model = c64model_get_temp(vicii_model, sid_model, glue_logic,
                                  cia1_model, cia2_model, new_luma);

    uic64_update_controls(hwnd, CONTROL_UPDATE_EVERYTHING);
}

static void uic64_set_resources(void)
{
    resources_set_int("VICIIModel", vicii_model);
    sid_set_engine_model(sid_engine, sid_model);
    resources_set_int("GlueLogic", glue_logic);
    resources_set_int("CIA1Model", cia1_model);
    resources_set_int("CIA2Model", cia2_model);
    resources_set_int("VICIINewLuminances", new_luma);
}

static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    int command;
    int new_c64model;
    int new_vicii = vicii_model;
    int new_cia1 = cia1_model;
    int new_cia2 = cia2_model;
    int new_sidengmod = (sid_engine << 8) | sid_model;
    int new_glue = glue_logic;
    int new_new_luma = new_luma;

    switch (msg) {
        case WM_COMMAND:
            command=LOWORD(wparam);
            switch (command) {
                case IDOK:
                    uic64_set_resources();
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    break;
                case IDC_C64MODEL_LIST:
                    new_c64model = ui_c64model_values[SendMessage(
                                    GetDlgItem(hwnd, IDC_C64MODEL_LIST), CB_GETCURSEL, 0, 0)];
                    if (new_c64model != c64_model) {
                        c64_model = new_c64model;
                        c64model_set_temp(c64_model, &vicii_model, &sid_model, &glue_logic,
                                          &cia1_model, &cia2_model, &new_luma);
                        sid_engine = (sid_model >> 8);
                        sid_model = (sid_model & 0xff);
                        uic64_update_controls(hwnd, CONTROL_UPDATE_MODELCHANGE);
                    }
                    return TRUE;
                case IDC_C64VICII_LIST:
                    new_vicii = ui_c64vicii_values[SendMessage(
                                    GetDlgItem(hwnd, IDC_C64VICII_LIST), CB_GETCURSEL, 0, 0)];
                    break;
                case IDC_C64SID_LIST:
                    new_sidengmod = ui_c64sid_engine_model_values[SendMessage(
                                    GetDlgItem(hwnd, IDC_C64SID_LIST), CB_GETCURSEL, 0, 0)];
                    break;
                case IDC_C64CIA1_OLD:
                    new_cia1 = 0;
                    break;
                case IDC_C64CIA1_NEW:
                    new_cia1 = 1;
                    break;
                case IDC_C64CIA2_OLD:
                    new_cia2 = 0;
                    break;
                case IDC_C64CIA2_NEW:
                    new_cia2 = 1;
                    break;
                case IDC_C64GLUE_DISCRETE:
                    new_glue = 0;
                    break;
                case IDC_C64GLUE_IC:
                    new_glue = 1;
                    break;
                case IDC_C64LUMINANCES:
                    new_new_luma = (new_luma == 0);
                    break;
                default:
                    return FALSE;
            }

            if ((new_vicii != vicii_model)
             || (new_cia1 != cia1_model)
             || (new_cia2 != cia2_model)
             || (new_sidengmod != ((sid_engine << 8) | sid_model))
             || (new_glue != glue_logic)
             || (new_new_luma != new_luma)) {
                vicii_model = new_vicii;
                cia1_model = new_cia1;
                cia2_model = new_cia2;
                sid_engine = (new_sidengmod >> 8);
                sid_model = (new_sidengmod & 0xff);
                glue_logic = new_glue;
                new_luma = new_new_luma;

                c64_model = c64model_get_temp(vicii_model, sid_model, glue_logic,
                                          cia1_model, cia2_model, new_luma);
                uic64_update_controls(hwnd, CONTROL_UPDATE_C64MODEL);
            }
            return TRUE;
        case WM_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;
        case WM_INITDIALOG:
            system_init_dialog(hwnd);
            init_c64model_dialog(hwnd);
            return TRUE;
    }
    return FALSE;
}


void ui_c64model_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, MAKEINTRESOURCE(IDD_C64MODEL_SETTINGS_DIALOG), hwnd, dialog_proc);
}
