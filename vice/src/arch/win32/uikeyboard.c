/*
 * uikeyboard.c - Keyboard settings dialog box.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include <windows.h>
#include <tchar.h>
#include <prsht.h>

#ifndef DUMMYUNIONNAME
#define DUMMYUNIONNAME  u1
#endif

#ifndef MIIM_STRING
/*
 * workaround for MS VS 6 which has WINVER == 0x0400 as default.
 * OTOH, if setting WINVER to 0x0500, it warns that the headers are
 * only beta, and that serious problems can occur.
 */
#define MIIM_STRING      0x00000040
#endif

#include "archdep.h"
#include "intl.h"
#include "keyboard.h"
#include "lib.h"
#include "res.h"
#include "resources.h"
#include "sysfile.h"
#include "system.h"
#include "translate.h"
#include "ui.h"
#include "uikeyboard.h"
#include "uilib.h"
#include "util.h"
#include "winmain.h"

#include "menuid.h"

#define MAXACCEL 1000

static int uikeyboard_mapping_num;
static const uikeyboard_mapping_entry_t *mapping_entry;
static int mapping_idc_dump;
static char **menuitemmodifier;
static int menuitemmodifier_len;

static int mapping_index_get(void)
{
    int index;

    resources_get_int("KeymapIndex", &index);

    if (index >= uikeyboard_mapping_num)
        return mapping_entry[0].idc_select;

    return mapping_entry[index].idc_select;
}

static void enable_mapping_controls(HWND hwnd, int idc_index)
{
    int i;

    for (i = 0; i < uikeyboard_mapping_num; i++) {
        EnableWindow(GetDlgItem(hwnd, mapping_entry[i].idc_filename),
                     idc_index == mapping_entry[i].idc_select);
        EnableWindow(GetDlgItem(hwnd, mapping_entry[i].idc_browse),
                     idc_index == mapping_entry[i].idc_select);
    }
}

static void init_mapping_dialog(HWND hwnd)
{
    int idc_index, i;

    idc_index = mapping_index_get();

    CheckRadioButton(hwnd, mapping_entry[0].idc_select,
                     mapping_entry[uikeyboard_mapping_num - 1].idc_select,
                     idc_index);

    for (i = 0; i < uikeyboard_mapping_num; i++) {
        const char *fname;
        TCHAR *st_fname;

        resources_get_string(mapping_entry[i].res_filename, &fname);
        st_fname = system_mbstowcs_alloc(fname);
        SetDlgItemText(hwnd, mapping_entry[i].idc_filename,
                       fname != NULL ? st_fname : TEXT(""));
        system_mbstowcs_free(st_fname);
    }

    enable_mapping_controls(hwnd, idc_index);
}

static void end_mapping_dialog(HWND hwnd)
{
    int i;

    for (i = 0; i < uikeyboard_mapping_num; i++) {
        TCHAR st[MAX_PATH];
        char s[MAX_PATH];

        GetDlgItemText(hwnd, mapping_entry[i].idc_filename, st, MAX_PATH);
        system_wcstombs(s, st, MAX_PATH);
        resources_set_string(mapping_entry[i].res_filename, s);
        if (IsDlgButtonChecked(hwnd, mapping_entry[i].idc_select)
            == BST_CHECKED)
            resources_set_int("KeymapIndex", i);
    }
}

static void browse_mapping(HWND hwnd, unsigned int index)
{
    uilib_select_browse(hwnd, translate_text(IDS_SELECT_KEYMAP_FILE),
                        UILIB_FILTER_KEYMAP, UILIB_SELECTOR_TYPE_FILE_SAVE,
                        mapping_entry[index].idc_filename);
}

static void dump_mapping(HWND hwnd)
{
    TCHAR *st_name;

    if ((st_name = uilib_select_file(hwnd, translate_text(IDS_SAVE_KEYMAP_FILE),
        UILIB_FILTER_KEYMAP, UILIB_SELECTOR_TYPE_FILE_SAVE,
        UILIB_SELECTOR_STYLE_DEFAULT)) != NULL) {
        char *name;

        name = system_wcstombs_alloc(st_name);

        util_add_extension(&name, "vkm");

        if (keyboard_keymap_dump(name) != 0)
            ui_error(translate_text(IDS_CANNOT_WRITE_KEYMAP_FILE));
        system_wcstombs_free(name);
        lib_free(st_name);
    }
}

static BOOL CALLBACK mapping_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                         LPARAM lparam)
{
    switch (msg) {
      case WM_NOTIFY:
        switch (((NMHDR FAR *)lparam)->code) {
          case PSN_KILLACTIVE:
            end_mapping_dialog(hwnd);
            return TRUE;
        }
        return FALSE;
      case WM_COMMAND:
        {
            int i;

            for (i = 0; i < uikeyboard_mapping_num; i++) {
                if (LOWORD(wparam) == mapping_entry[i].idc_select)
                    enable_mapping_controls(hwnd, LOWORD(wparam));
                if (LOWORD(wparam) == mapping_entry[i].idc_browse)
                    browse_mapping(hwnd, i);
            }
            if (LOWORD(wparam) == mapping_idc_dump)
                dump_mapping(hwnd);
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_mapping_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}

void uikeyboard_settings_dialog(HWND hwnd,
                                uikeyboard_config_t *uikeyboard_config)
{
    PROPSHEETPAGE psp[1];
    PROPSHEETHEADER psh;

    uikeyboard_mapping_num = uikeyboard_config->num_mapping;
    mapping_entry = uikeyboard_config->mapping_entry;
    mapping_idc_dump = uikeyboard_config->idc_dump;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
    psp[0].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
    psp[0].pszTemplate = MAKEINTRESOURCE(translate_res(uikeyboard_config->idd_mapping));
    psp[0].pszIcon = NULL;
#else
    psp[0].DUMMYUNIONNAME.pszTemplate
        = MAKEINTRESOURCE(translate_res(uikeyboard_config->idd_mapping));
    psp[0].u2.pszIcon = NULL;
#endif
    psp[0].lParam = 0;
    psp[0].pfnCallback = NULL;

    psp[0].pfnDlgProc = mapping_dialog_proc;
    psp[0].pszTitle = translate_text(IDS_MAPPING);

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = translate_text(IDS_KEYBOARD_SETTINGS);
    psh.nPages = 1;
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

HACCEL uikeyboard_create_accelerator_table(void)
{
    FILE *fshortcuts;
    char *complete_path;
    char buffer[1000];
    char *p, *menustr, *metastr, *keystr, *displaystr;
    int i;

    ACCEL accellist[MAXACCEL];
    int accelnum = 0;

    menuitemmodifier_len = 0;
    for (i = 0; idmlist[i].str != NULL; i++)
        if (idmlist[i].cmd >= menuitemmodifier_len)
            menuitemmodifier_len = idmlist[i].cmd + 1;

    menuitemmodifier = (char**) lib_calloc(menuitemmodifier_len, sizeof(char*));
    memset(menuitemmodifier, 0, menuitemmodifier_len * sizeof(char*));

    fshortcuts = sysfile_open("win_shortcuts.vsc", &complete_path, MODE_READ_TEXT);
    lib_free(complete_path);
    if (fshortcuts == NULL)
        return NULL;

    /* read the shortcut table */
    do {
        buffer[0] = 0;
        if (fgets(buffer, 999, fshortcuts)) {

            if (strlen(buffer) == 0)
                break;

            buffer[strlen(buffer) - 1] = 0; /* remove newline */
	        /* remove comments */
	        if((p = strchr(buffer, '#')))
	            *p=0;

            metastr = strtok(buffer, " \t:");
            keystr = strtok(NULL, " \t:");
            menustr = strtok(NULL, " \t:");
            displaystr = strtok(NULL, " \t:");
	        if (displaystr && (p = strchr(displaystr, '#')))
	            *p=0;

            if (metastr && keystr && menustr) {
                for (i = 0; idmlist[i].str; i++) {
                    if (strcmp(idmlist[i].str, menustr) == 0)
                        break;
                }

                if (idmlist[i].str) {
                    ACCEL accel;

                    accel.fVirt = FVIRTKEY | FNOINVERT;
                    if (strstr(strlwr(metastr), "ctrl") != NULL)
                        accel.fVirt |= FCONTROL;
                    if (strstr(strlwr(metastr), "alt") != NULL)
                        accel.fVirt |= FALT;

                    if (keystr[0] == '\'' && keystr[2] == '\'') {
                        accel.key = keystr[1];
                        if (displaystr == NULL || displaystr[0] == 0) {
                            displaystr = keystr + 1;
                            keystr[2] = 0;
                        }
                    } else {
                        accel.key = (unsigned short)strtol(keystr, NULL, 0);
                    }

                    accel.cmd = idmlist[i].cmd;

                    if (accel.key > 0 && accel.cmd > 0 && accelnum < MAXACCEL)
                        accellist[accelnum++] = accel;

                    if (displaystr != NULL) {
                        p = util_concat("\t",
                                    ((accel.fVirt & FCONTROL) ? "Ctrl+" : ""),
                                    ((accel.fVirt & FALT) ? "Alt+" : ""),
                                    displaystr, NULL);

                        menuitemmodifier[accel.cmd] = p;
                    }
                }
            }
        }
    } while (!feof(fshortcuts));
    fclose(fshortcuts);

    return CreateAcceleratorTable(accellist, accelnum);
}


#if 0
void uikeyboard_menu_shortcuts(HMENU menu)
{
    int i;
    MENUITEMINFO mii;
    LPTSTR  buf, newbuf;

    for (i = 0; idmlist[i].cmd > 0; i++) {
        if (menuitemmodifier[idmlist[i].cmd] != NULL) {
            mii.fMask = MIIM_STRING;
            mii.dwTypeData = NULL;
            mii.cbSize = sizeof(MENUITEMINFO);
            if (GetMenuItemInfo(menu, idmlist[i].cmd, FALSE, &mii)) {
                mii.cch++;
                buf = lib_malloc(mii.cch);
                mii.dwTypeData = buf;
                if (GetMenuItemInfo(menu, idmlist[i].cmd, FALSE, &mii)) {
                    newbuf = util_concat(buf, menuitemmodifier[idmlist[i].cmd], NULL);
                    mii.dwTypeData = newbuf;
                    SetMenuItemInfo(menu, idmlist[i].cmd, FALSE, &mii);
                    lib_free(newbuf);
                }
                lib_free(buf);
            }
        }
    }
}
#endif

/* using MIIM_STRING doesn't work for win9x/winnt4, so trying an older way */
void uikeyboard_menu_shortcuts(HMENU menu)
{
    int i;
    int stringsize;
    LPTSTR  buf, newbuf;

    for (i = 0; idmlist[i].cmd > 0; i++) {
        if (menuitemmodifier[idmlist[i].cmd] != NULL) {
            stringsize = GetMenuString(menu, idmlist[i].cmd, NULL, 0, MF_BYCOMMAND);
            if (stringsize != 0) {
                stringsize++;
                buf = lib_malloc(stringsize);
                if (GetMenuString(menu, idmlist[i].cmd, buf, stringsize, MF_BYCOMMAND)) {
                    newbuf = util_concat(buf, menuitemmodifier[idmlist[i].cmd], NULL);
                    ModifyMenu(menu, idmlist[i].cmd, MF_BYCOMMAND | MF_STRING, idmlist[i].cmd, newbuf);
                    lib_free(newbuf);
                }
                lib_free(buf);
            }
        }
    }
}


void uikeyboard_shutdown(void)
{
    int i;

    for (i = 0; i < menuitemmodifier_len; i++)
        lib_free(menuitemmodifier[i]);
    lib_free(menuitemmodifier);
}

