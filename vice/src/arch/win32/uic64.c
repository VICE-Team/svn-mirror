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
#include "cia.h"
#include "intl.h"
#include "machine.h"
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

static uilib_localize_dialog_param c64_model_dialog[] = {
    { 0, IDS_C64_MODEL_CAPTION, -1 },
    { IDC_C64MODEL_LABEL, IDS_C64_MODEL, 0 },
    { IDC_C64VICII_LABEL, IDS_VICII_MODEL, 0 },
    { IDC_C64LUMINANCES, IDS_NEW_LUMINANCES, 0 },
    { IDC_C64SID_LABEL, IDS_SID_MODEL, 0 },
    { IDC_C64CIA1_LABEL, IDS_CIA1_MODEL, 0 },
    { IDC_C64CIA2_LABEL, IDS_CIA2_MODEL, 0 },
    { IDC_C64GLUELOGIC_LABEL, IDS_GLUE_LOGIC, 0 },
    { IDOK, IDS_OK, 0 },
    { IDCANCEL, IDS_CANCEL, 0 },
    { 0, 0, 0 }
};

static uilib_dialog_group c64_model_leftgroup[] = {
    { IDC_C64MODEL_LABEL, 0 },
    { IDC_C64VICII_LABEL, 0 },
    { IDC_C64SID_LABEL, 0 },
    { IDC_C64CIA1_LABEL, 0 },
    { IDC_C64CIA2_LABEL, 0 },
    { IDC_C64GLUELOGIC_LABEL, 0 },
    { 0, 0 }
};

static uilib_dialog_group c64_model_rightgroup[] = {
    { IDC_C64MODEL_LIST, 0 },
    { IDC_C64VICII_LIST, 0 },
    { IDC_C64LUMINANCES, 1 },
    { IDC_C64SID_LIST, 0 },
    { IDC_C64CIA1_LIST, 0 },
    { IDC_C64CIA2_LIST, 0 },
    { IDC_C64GLUELOGIC_LIST, 0 },
    { 0, 0 }
};

static int move_buttons_group[] = {
    IDOK,
    IDCANCEL,
    0
};

static const TCHAR *ui_c64model[] = {
    TEXT("C64 PAL"),
    TEXT("C64C PAL"),
    TEXT("C64 old PAL"),
    TEXT("C64 NTSC"),
    TEXT("C64C NTSC"),
    TEXT("C64 old NTSC"),
    TEXT("Drean"),
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
    C64MODEL_C64_PAL_N,
    C64MODEL_UNKNOWN,
    -1
};

static const int ui_c64cia[] = {
   IDS_6526_OLD,
   IDS_6526A_NEW,
   0
};

static const int ui_c64cia_values[] = {
    CIA_MODEL_6526,
    CIA_MODEL_6526A,
    -1
};

static const int ui_c64gluelogic[] = {
    IDS_DISCRETE,
    IDS_CUSTOM_IC,
    0
};

static const int ui_c64gluelogic_values[] = {
    0,
    1,
    -1
};

static const int ui_c64vicii[] = {
    IDS_6569_PAL,
    IDS_8565_PAL,
    IDS_6569R1_OLD_PAL,
    IDS_6567_NTSC,
    IDS_8562_NTSC,
    IDS_6567R56A_OLD_NTSC,
    IDS_6572_PAL_N,
    0
};

static const int ui_c64vicii_values[] = {
    VICII_MODEL_6569,
    VICII_MODEL_8565,
    VICII_MODEL_6569R1,
    VICII_MODEL_6567,
    VICII_MODEL_8562,
    VICII_MODEL_6567R56A,
    VICII_MODEL_6572,
    -1
};

static const TCHAR *ui_c64video_standard[] = {
    TEXT("PAL-G"),
    TEXT("NTSC-M"),
    TEXT("Old NTSC-M"),
    TEXT("PAL-N"),
    0
};

static const int ui_c64video_standard_values[] = {
    MACHINE_SYNC_PAL,
    MACHINE_SYNC_NTSC,
    MACHINE_SYNC_NTSCOLD,
    MACHINE_SYNC_PALN,
    0
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
static int c64_model, machine_video_standard;
static int is_sc;

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
        if (is_sc) {
            for (i = 0; ui_c64vicii[i] != 0; i++) {
                _stprintf(st, TEXT("%s"), translate_text(ui_c64vicii[i]));
                SendMessage(sub_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
                if (ui_c64vicii_values[i] == vicii_model) {
                    active_value = i;
                }
            }
        } else {
            for (i = 0; ui_c64video_standard[i] != 0; i++) {
                _stprintf(st, TEXT("%s"), ui_c64video_standard[i]);
                SendMessage(sub_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
                if (ui_c64video_standard_values[i] == machine_video_standard) {
                    active_value = i;
                }
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

        sub_hwnd = GetDlgItem(hwnd, IDC_C64CIA1_LIST);
        SendMessage(sub_hwnd, CB_RESETCONTENT, 0, 0);
        for (i = 0; ui_c64cia[i] != 0; i++) {
            _stprintf(st, TEXT("%s"), translate_text(ui_c64cia[i]));
            SendMessage(sub_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
            if (ui_c64cia_values[i] == cia1_model) {
                active_value = i;
            }
        }
        SendMessage(sub_hwnd, CB_SETCURSEL, (WPARAM)active_value, 0);

        sub_hwnd = GetDlgItem(hwnd, IDC_C64CIA2_LIST);
        SendMessage(sub_hwnd, CB_RESETCONTENT, 0, 0);
        for (i = 0; ui_c64cia[i] != 0; i++) {
            _stprintf(st, TEXT("%s"), translate_text(ui_c64cia[i]));
            SendMessage(sub_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
            if (ui_c64cia_values[i] == cia2_model) {
                active_value = i;
            }
        }
        SendMessage(sub_hwnd, CB_SETCURSEL, (WPARAM)active_value, 0);

        sub_hwnd = GetDlgItem(hwnd, IDC_C64GLUELOGIC_LIST);
        EnableWindow(sub_hwnd, is_sc);
        if (is_sc) {
            SendMessage(sub_hwnd, CB_RESETCONTENT, 0, 0);
            for (i = 0; ui_c64gluelogic[i] != 0; i++) {
                _stprintf(st, TEXT("%s"), translate_text(ui_c64gluelogic[i]));
                SendMessage(sub_hwnd, CB_ADDSTRING, 0, (LPARAM)st);
                if (ui_c64cia_values[i] == glue_logic) {
                    active_value = i;
                }
            }
            SendMessage(sub_hwnd, CB_SETCURSEL, (WPARAM)active_value, 0);
        }

        CheckDlgButton(hwnd, IDC_C64LUMINANCES, new_luma ? BST_CHECKED : BST_UNCHECKED);
    }
}

static void init_c64model_dialog(HWND hwnd)
{
    int xpos;
    RECT rect;

     /* translate all dialog items */
    uilib_localize_dialog(hwnd, c64_model_dialog);

    /* adjust the size of the elements in the left group */
    uilib_adjust_group_width(hwnd, c64_model_leftgroup);

    /* get the max x of the left group */
    uilib_get_group_max_x(hwnd, c64_model_leftgroup, &xpos);

    /* move the right group to the correct position */
    uilib_move_group(hwnd, c64_model_rightgroup, xpos + 30);

    /* get the max x of the right group */
    uilib_get_group_max_x(hwnd, c64_model_rightgroup, &xpos);

    /* set the width of the dialog to 'surround' all the elements */
    GetWindowRect(hwnd, &rect);
    MoveWindow(hwnd, rect.left, rect.top, xpos + 20, rect.bottom - rect.top, TRUE);

    /* recenter the buttons in the newly resized dialog window */
    uilib_center_buttons(hwnd, move_buttons_group, 0);

    resources_get_int("SidModel", &sid_model);
    resources_get_int("SidEngine", &sid_engine);
    resources_get_int("CIA1Model", &cia1_model);
    resources_get_int("CIA2Model", &cia2_model);
    resources_get_int("VICIINewLuminances", &new_luma);

    if (resources_get_int("VICIIModel", &vicii_model) < 0) {
        resources_get_int("MachineVideoStandard", &machine_video_standard);
        c64_model = c64model_get_temp(machine_video_standard, sid_model, glue_logic,
                                  cia1_model, cia2_model, new_luma);
        is_sc = 0;
    } else {
        resources_get_int("GlueLogic", &glue_logic);
        c64_model = c64model_get_temp(vicii_model, sid_model, glue_logic,
                                  cia1_model, cia2_model, new_luma);
        is_sc = 1;
    }


    uic64_update_controls(hwnd, CONTROL_UPDATE_EVERYTHING);
}

static void uic64_set_resources(void)
{
    if (is_sc) {
        resources_set_int("VICIIModel", vicii_model);
        resources_set_int("GlueLogic", glue_logic);
    } else {
        resources_set_int("MachineVideoStandard", machine_video_standard);
    }
    sid_set_engine_model(sid_engine, sid_model);
    resources_set_int("CIA1Model", cia1_model);
    resources_set_int("CIA2Model", cia2_model);
    resources_set_int("VICIINewLuminances", new_luma);
}

static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    int command;
    int sidengmodel;
    int new_c64model;
    int new_vicii = vicii_model;
    int new_cia1 = cia1_model;
    int new_cia2 = cia2_model;
    int new_sidengmod = (sid_engine << 8) | sid_model;
    int new_glue = glue_logic;
    int new_new_luma = new_luma;
    int new_machine_video_standard = machine_video_standard;
    int *vicii_or_video, *new_vicii_or_video;

    if (is_sc) {
        new_vicii_or_video = &new_vicii;
        vicii_or_video = &vicii_model;
    } else {
        new_vicii_or_video = &new_machine_video_standard;
        vicii_or_video = &machine_video_standard;
    }

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
                        sidengmodel = (sid_engine << 8) | sid_model;
                        if (is_sc) {
                            c64model_set_temp(c64_model, &vicii_model, &sidengmodel, &glue_logic,
                                          &cia1_model, &cia2_model, &new_luma);
                        } else {
                            c64model_set_temp(c64_model, &machine_video_standard, &sidengmodel, &glue_logic,
                                          &cia1_model, &cia2_model, &new_luma);
                        }
                        sid_engine = (sidengmodel >> 8);
                        sid_model = (sidengmodel & 0xff);
                        uic64_update_controls(hwnd, CONTROL_UPDATE_MODELCHANGE);
                    }
                    return TRUE;
                case IDC_C64VICII_LIST:
                    if (is_sc) {
                        *new_vicii_or_video = ui_c64vicii_values[SendMessage(
                                    GetDlgItem(hwnd, IDC_C64VICII_LIST), CB_GETCURSEL, 0, 0)];
                    } else {
                        *new_vicii_or_video = ui_c64video_standard_values[SendMessage(
                                    GetDlgItem(hwnd, IDC_C64VICII_LIST), CB_GETCURSEL, 0, 0)];
                    }
                    break;
                case IDC_C64SID_LIST:
                    new_sidengmod = ui_c64sid_engine_model_values[SendMessage(
                                    GetDlgItem(hwnd, IDC_C64SID_LIST), CB_GETCURSEL, 0, 0)];
                    break;
                case IDC_C64CIA1_LIST:
                    new_cia1 = ui_c64cia_values[SendMessage(
                                    GetDlgItem(hwnd, IDC_C64CIA1_LIST), CB_GETCURSEL, 0, 0)];
                    break;
                case IDC_C64CIA2_LIST:
                    new_cia2 = ui_c64cia_values[SendMessage(
                                    GetDlgItem(hwnd, IDC_C64CIA2_LIST), CB_GETCURSEL, 0, 0)];
                    break;
                case IDC_C64GLUELOGIC_LIST:
                    new_glue = ui_c64gluelogic_values[SendMessage(
                                    GetDlgItem(hwnd, IDC_C64GLUELOGIC_LIST), CB_GETCURSEL, 0, 0)];
                    break;
                case IDC_C64LUMINANCES:
                    new_new_luma = (new_luma == 0);
                    break;
                default:
                    return FALSE;
            }

            if ((*new_vicii_or_video != *vicii_or_video)
             || (new_cia1 != cia1_model)
             || (new_cia2 != cia2_model)
             || (new_sidengmod != ((sid_engine << 8) | sid_model))
             || (new_glue != glue_logic)
             || (new_new_luma != new_luma)) {
                *vicii_or_video = *new_vicii_or_video;
                cia1_model = new_cia1;
                cia2_model = new_cia2;
                sid_engine = (new_sidengmod >> 8);
                sid_model = (new_sidengmod & 0xff);
                glue_logic = new_glue;
                new_luma = new_new_luma;

                c64_model = c64model_get_temp(*vicii_or_video, sid_model, glue_logic,
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
