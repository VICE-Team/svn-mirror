/*
 * uilib.c - Common UI elements for the Windows user interface.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
 *  Manfred Spraul <manfreds@colorfullife.com>
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <commctrl.h>
#include <commdlg.h>

#include "archdep.h"
#include "cbmimage.h"
#include "cmdline.h"
#include "diskimage.h"
#include "fullscrn.h"
#include "imagecontents.h"
#include "diskcontents.h"
#include "tapecontents.h"
#include "intl.h"
#include "lib.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "uiapi.h"
#include "uilib.h"
#include "util.h"
#include "vdrive-internal.h"
#include "vsync.h"
#include "winlong.h"
#include "winmain.h"

static int *autostart_result;
static char* fontfile;
static int font_loaded;
static char *res_readonly;

struct uilib_filefilter_s {
    int name;
    TCHAR *pattern;
};
typedef struct uilib_filefilter_s uilib_filefilter_t;

static uilib_filefilter_t uilib_filefilter[] = {
    { IDS_ALL_FILES_FILTER, TEXT("*.*") },
    { IDS_ZIPPED_FILES_FILTER, TEXT("*.zip;*.bz2;*.gz;*.d6z;*.d7z;*.d8z;*.g6z;*.g4z;*.x6z") },
    { IDS_PALETTE_FILES_FILTER, TEXT("*.vpl") },
    { IDS_SNAPSHOT_FILES_FILTER, TEXT("*.vsf") },
    { IDS_PRGP00_FILES_FILTER, TEXT("*.prg;*.p00") },
    { IDS_TAPE_IMAGE_FILES_FILTER, TEXT("*.t64;*.tap") },
    { IDS_DISK_IMAGE_FILES_FILTER, TEXT("*.d64;*.d67;*.d71;*.d80;*.d81;*.d82;*.d1m;*.d2m;*.d4m;*.g64;*.g41;*.p64;*.x64") },
    { IDS_CBM_IMAGE_FILES_FILTER, TEXT("*.d64;*.d67;*.d71;*.d80;*.d81;*.d82;*.d1m;*.d2m;*.d4m;*.g64;*.g41;*.p64;*.x64;*.t64;*.tap;*.prg;*.p00") },
    { IDS_IDE64_IMAGE_FILES_FILTER, TEXT("*.fdd;*.hdd;*.iso;*.cfa") },
    { IDS_CRT_FILES_FILTER, TEXT("*.crt") },
    { IDS_RAW_CART_FILES_FILTER, TEXT("*.bin") },
    { IDS_FLIP_LIST_FILES_FILTER, TEXT("*.vfl") },
    { IDS_ROMSET_FILES_FILTER, TEXT("*.vrs") },
    { IDS_ROMSET_ARCHIVES_FILTER, TEXT("*.vra") },
    { IDS_KEYMAP_FILES_FILTER, TEXT("*.vkm") },
    { 0, NULL }
};

typedef struct uilib_fs_style_type_s {
    LPOFNHOOKPROC hook_proc;
    int TemplateID;
    char *initialdir_resource;
    char *file_resource;
} uilib_fs_style_type_t;

static UINT_PTR APIENTRY uilib_select_tape_hook_proc(HWND hwnd, UINT uimsg, WPARAM wparam, LPARAM lparam);
static UINT_PTR APIENTRY uilib_select_disk_hook_proc(HWND hwnd, UINT uimsg, WPARAM wparam, LPARAM lparam);
static UINT_PTR APIENTRY uilib_select_hook_proc(HWND hwnd, UINT uimsg, WPARAM wparam, LPARAM lparam);

static uilib_fs_style_type_t styles[UILIB_SELECTOR_STYLES_NUM + 1] = {
    /* UILIB_SELECTOR_STYLE_DEFAULT */
    { NULL, 0, "InitialDefaultDir", NULL },
    /* UILIB_SELECTOR_STYLE_TAPE */
    { uilib_select_tape_hook_proc, IDD_OPENTAPE_TEMPLATE, "InitialTapeDir",
      NULL },
    /* UILIB_SELECTOR_STYLE_DISK */
    { uilib_select_disk_hook_proc, IDD_OPENDISK_TEMPLATE, "InitialDiskDir", NULL },
    /* UILIB_SELECTOR_STYLE_DISK_AND_TAPE */
    { uilib_select_hook_proc, IDD_OPEN_TEMPLATE, "InitialAutostartDir", NULL },
    /* UILIB_SELECTOR_STYLE_CART */
    { NULL, 0, "InitialCartDir", NULL },
    /* UILIB_SELECTOR_STYLE_SNAPSHOT */
    { NULL, 0, "InitialSnapshotDir", NULL },
    /* UILIB_SELECTOR_STYLE_EVENT_START */
    { NULL, 0, "EventSnapshotDir", "EventStartSnapshot" },
    /* UILIB_SELECTOR_STYLE_EVENT_END */
    { NULL, 0, "EventSnapshotDir", "EventEndSnapshot"  },
    /* DUMMY entry Insert new styles before this */
    { NULL, 0, NULL, NULL }
};

static TCHAR *ui_file_selector_initialfile[UILIB_SELECTOR_STYLES_NUM];

static void create_content_list(image_contents_t *contents, HWND list)
{
    char *start;
    image_contents_file_list_t *p = contents->file_list;

    start = image_contents_to_string(contents, 0);
    SendMessage(list, LB_ADDSTRING, 0, (LPARAM)start);
    lib_free(start);

    if (p == NULL) {
        SendMessage(list, LB_ADDSTRING, 0, (LPARAM)"(EMPTY IMAGE.)");
    } else do {
        start = image_contents_file_to_string(p, 0);
        SendMessage(list, LB_ADDSTRING, 0, (LPARAM)start);
        lib_free(start);
    } while ((p = p->next) != NULL);

    if (contents->blocks_free >= 0) {
        start = lib_msprintf("%d BLOCKS FREE.", contents->blocks_free);
        SendMessage(list, LB_ADDSTRING, 0, (LPARAM)start);
        lib_free(start);
    }
}

static HFONT hfont;

static uilib_localize_dialog_param select_tape_trans[] = {
    { IDC_IMAGE_CONTENTS, IDS_IMAGE_CONTENTS, 0 },
    { IDC_NEW_TAP_IMAGE, IDS_NEW_TAP_IMAGE, 0 },
    { IDC_BLANK_IMAGE, IDS_BLANK_IMAGE, 0 },
    { 0, 0, 0 }
};

static uilib_localize_dialog_param select_tape_parent_trans[] = {
    { IDOK, IDS_OK, 0 },
    { IDCANCEL, IDS_CANCEL, 0 },
    { 0, 0, 0 }
};

static uilib_dialog_group select_tape_button_group[] = {
    { IDC_BLANK_IMAGE, 1 },
    { 0, 0 }
};

static UINT_PTR APIENTRY uilib_select_tape_hook_proc(HWND hwnd, UINT uimsg, WPARAM wparam, LPARAM lparam)
{
    HWND preview;
    image_contents_t *contents;
    char filename[256];
    int index;
    int append_extension = 0;
    char *extension;
    HWND parent_hwnd;
    int xpos;
    int xstart;

    preview = GetDlgItem(hwnd, IDC_PREVIEW);
    switch (uimsg) {
        case WM_INITDIALOG:
            parent_hwnd = GetParent(hwnd);

            /* translate all dialog items */
            uilib_localize_dialog(hwnd, select_tape_trans);
            uilib_localize_dialog(parent_hwnd, select_tape_parent_trans);

            /* adjust the button group */
            uilib_adjust_group_width(hwnd, select_tape_button_group);

            /* get the max x of the button element */
            uilib_get_element_max_x(hwnd, IDC_BLANK_IMAGE, &xpos);

            /* get the min x of the button element */
            uilib_get_element_min_x(hwnd, IDC_BLANK_IMAGE, &xstart);

            /* move and resize the surrounding group element */
            uilib_move_and_set_element_width(hwnd, IDC_NEW_TAP_IMAGE, xstart - 10, xpos - xstart + 20);

            if (font_loaded) {
                hfont = CreateFont(-12,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "cbm-directory-charset/ck!");
            } else {
                /*  maybe there's a better font-definition (FIXME) */
                /*  I think it's OK now (Tibor) */
                hfont = CreateFont(-12, -7, 0, 0, 400, 0, 0, 0, 0, 0, 0, DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, NULL);
            }

            if (hfont) {
                SendDlgItemMessage(hwnd, IDC_PREVIEW,WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0));
            }
            break;
        case WM_NOTIFY:
            if (((OFNOTIFY*)lparam)->hdr.code == CDN_SELCHANGE) {
                SendMessage(preview, LB_RESETCONTENT, 0, 0);
                if (SendMessage(((OFNOTIFY*)lparam)->hdr.hwndFrom, CDM_GETFILEPATH, 256, (LPARAM)filename) >= 0) {
                    if (!(GetFileAttributes(filename) & FILE_ATTRIBUTE_DIRECTORY)) {
                        contents = tapecontents_read(filename);
                        if (contents != NULL) {
                            create_content_list(contents, preview);
                            image_contents_destroy(contents);
                        }
                    }
                }
            } else if (((OFNOTIFY*)lparam)->hdr.code == CDN_FOLDERCHANGE) {
                SendMessage(preview, LB_RESETCONTENT, 0, 0);
                SetWindowText(GetDlgItem(GetParent(hwnd), 0x0480), TEXT(""));
            }
            break;
        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case IDC_BLANK_IMAGE:
                    if (SendMessage(GetParent(hwnd), CDM_GETSPEC, 256, (LPARAM)filename) <= 1) {
                        ui_error(translate_text(IDS_PLEASE_ENTER_A_FILENAME));
                        return -1;
                    }
                    if (strchr(filename,'.') == NULL) {
                        append_extension = 1;
                    } else {
                        /*  Find last dot in name */
                        extension = strrchr(filename, '.');
                        /*  Skip dot */
                        extension++;
                        /*  Figure out if it's a standard extension */
                        if (strncasecmp(extension, "tap", 3) == 0) {
                        }
                    }
                    if (SendMessage(GetParent(hwnd), CDM_GETFILEPATH, 256, (LPARAM)filename) >= 0) {
                        if (append_extension) {
                            strcat(filename, ".");
                            strcat(filename, "tap");
                        }
                        if (util_file_exists(filename)) {
                            int ret;

                            ret = ui_messagebox(translate_text(IDS_OVERWRITE_EXISTING_IMAGE), translate_text(IDS_VICE_QUESTION), MB_YESNO | MB_ICONQUESTION);
                            if (ret != IDYES) {
                                return -1;
                            }
                        }
                        if (cbmimage_create_image(filename, DISK_IMAGE_TYPE_TAP)) {
                            ui_error(translate_text(IDS_CANNOT_CREATE_IMAGE));
                            return -1;
                        }
                    }
                    break;
            }
            switch (HIWORD(wparam)) {
                case LBN_DBLCLK:
                    if (autostart_result != NULL) {
                        index = (int)SendMessage((HWND)lparam, LB_GETCURSEL, 0, 0);
                        if (SendMessage(GetParent(hwnd), CDM_GETFILEPATH, 256, (LPARAM)filename) >= 0) {
                            *autostart_result = index;
                            SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(IDOK, BN_CLICKED), (LPARAM)GetDlgItem(GetParent(hwnd), IDOK));
                        }
                    }
                    break;
            }
            break;
        case WM_DESTROY:
            if (hfont != NULL) {
                DeleteObject(hfont);
                hfont = NULL;
            }
            break;
    }
    return 0;
}

static TCHAR *image_type_name[] = {
    TEXT("d64"),
    TEXT("d67"),
    TEXT("d71"),
    TEXT("d80"),
    TEXT("d81"),
    TEXT("d82"),
    TEXT("d1m"),
    TEXT("d2m"),
    TEXT("d4m"),
    TEXT("g64"),
    TEXT("p64"),
    TEXT("x64"),
    NULL
};

static int image_type[] = {
    DISK_IMAGE_TYPE_D64,
    DISK_IMAGE_TYPE_D67,
    DISK_IMAGE_TYPE_D71,
    DISK_IMAGE_TYPE_D80,
    DISK_IMAGE_TYPE_D81,
    DISK_IMAGE_TYPE_D82,
    DISK_IMAGE_TYPE_D1M,
    DISK_IMAGE_TYPE_D2M,
    DISK_IMAGE_TYPE_D4M,
    DISK_IMAGE_TYPE_G64,
    DISK_IMAGE_TYPE_P64,
    DISK_IMAGE_TYPE_X64
};

static uilib_localize_dialog_param select_disk_hook_trans[] = {
    { IDC_IMAGE_CONTENTS, IDS_IMAGE_CONTENTS, 0 },
    { IDC_TOGGLE_ATTACH_READONLY, IDS_TOGGLE_ATTACH_READONLY, 0 },
    { IDC_NEW_IMAGE, IDS_NEW_IMAGE, 0 },
    { IDC_NAME, IDS_NAME, 0 },
    { IDC_ID, IDS_ID, 0 },
    { IDC_TYPE, IDS_TYPE, 0 },
    { IDC_BLANK_IMAGE, IDS_BLANK_IMAGE, 0 },
    { 0, 0, 0 }
};

static uilib_dialog_group select_disk_right_group[] = {
    { IDC_TOGGLE_ATTACH_READONLY, 1 },
    { IDC_NAME, 0 },
    { IDC_ID, 0 },
    { IDC_TYPE, 0 },
    { IDC_BLANK_IMAGE, 1 },
    { 0, 0 }
};

static uilib_dialog_group select_disk_id_group[] = {
    { IDC_ID, 0 },
    { IDC_BLANK_IMAGE_ID, 0 },
    { 0, 0 }
};

static uilib_dialog_group select_disk_type_group[] = {
    { IDC_TYPE, 0 },
    { IDC_BLANK_IMAGE_TYPE, 0 },
    { 0, 0 }
};

static uilib_dialog_group select_disk_filling_group[] = {
    { IDC_BLANK_IMAGE_NAME, 0 },
    { IDC_TYPE, 0 },
    { IDC_BLANK_IMAGE_TYPE, 0 },
    { IDC_BLANK_IMAGE, 0 },
    { 0, 0 }
};

static UINT_PTR APIENTRY uilib_select_disk_hook_proc(HWND hwnd, UINT uimsg, WPARAM wparam, LPARAM lparam)
{
    HWND preview;
    HWND image_type_list;
    image_contents_t *contents;
    char filename[256];
    TCHAR st_filename[256];
    int counter;
    int msg_type;
    int append_extension = 0;
    int is_it_standard_extension = 0;
    char *extension;
    int index;
    LV_FINDINFO find;
    LV_ITEM item;
    HWND parent_hwnd;
    int xpos;
    int xstart;
    RECT rect;

    preview = GetDlgItem(hwnd, IDC_PREVIEW);
    switch (uimsg) {
        case WM_INITDIALOG:
            parent_hwnd = GetParent(hwnd);

            /* translate all dialog items */
            uilib_localize_dialog(hwnd, select_disk_hook_trans);

            SetWindowText(GetDlgItem(parent_hwnd, IDOK), translate_text(IDS_ATTACH));
            SetWindowText(GetDlgItem(parent_hwnd, IDCANCEL), translate_text(IDS_CANCEL));

            /* adjust the size of the elements in the right group */
            uilib_adjust_group_width(hwnd, select_disk_right_group);

            /* get the max x of the ID group */
            uilib_get_group_max_x(hwnd, select_disk_id_group, &xpos);

            /* move the type group to the correct position */
            uilib_move_group(hwnd, select_disk_type_group, xpos + 10);

            /* get the max x of the type group */
            uilib_get_group_max_x(hwnd, select_disk_filling_group, &xpos);

            /* get the min x of the name element */
            uilib_get_element_min_x(hwnd, IDC_BLANK_IMAGE_NAME, &xstart);

            /* move and set the size of the new image group element */
            uilib_move_and_set_element_width(hwnd, IDC_NEW_IMAGE, xstart - 10, xpos - xstart + 20);

            /* get the max x of the new image group element */
            uilib_get_element_min_x(hwnd, IDC_NEW_IMAGE, &xpos);

            /* resize the dialog window to fit */
            GetWindowRect(hwnd, &rect);
            if (xpos + 10 > rect.right) {
                MoveWindow(hwnd, rect.left, rect.top, xpos + 10, rect.bottom - rect.top, TRUE);
            }

            image_type_list = GetDlgItem(hwnd, IDC_BLANK_IMAGE_TYPE);
            for (counter = 0; image_type_name[counter]; counter++) {
                SendMessage(image_type_list, CB_ADDSTRING, 0, (LPARAM)image_type_name[counter]);
            }
            SendMessage(image_type_list, CB_SETCURSEL, (WPARAM)0, 0);

            /* Try to use the cbm font */
            if (font_loaded) {
                hfont = CreateFont(-12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "cbm-directory-charset/ck!");
            } else {
                /*  maybe there's a better font-definition (FIXME) */
                /*  I think it's OK now (Tibor) */
                hfont = CreateFont(-12, -7, 0, 0, 400, 0, 0, 0, 0, 0, 0, DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, NULL);
            }
            if (hfont) {
                SendDlgItemMessage(hwnd, IDC_PREVIEW, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0));
            }
            SetDlgItemText(hwnd, IDC_BLANK_IMAGE_NAME, TEXT("vice"));
            SetDlgItemText(hwnd, IDC_BLANK_IMAGE_ID, TEXT("1a"));
            if (res_readonly != NULL) {
                int ro;

                resources_get_int(res_readonly, &ro);
                CheckDlgButton(hwnd, IDC_TOGGLE_ATTACH_READONLY, ro ? BST_CHECKED : BST_UNCHECKED);
            } else {
                EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_ATTACH_READONLY),  FALSE);
            }
            break;
        case WM_NOTIFY:
            if (((OFNOTIFY *)lparam)->hdr.code == CDN_SELCHANGE) {
                SendMessage(preview, LB_RESETCONTENT, 0, 0);
                if (SendMessage(((OFNOTIFY*)lparam)->hdr.hwndFrom, CDM_GETFILEPATH, 256, (LPARAM)st_filename) >= 0) {
                    if (!(GetFileAttributes(st_filename) & FILE_ATTRIBUTE_DIRECTORY)) {
                        system_wcstombs(filename, st_filename, 256);
                        contents = diskcontents_filesystem_read(filename);
                        if (contents != NULL) {
                            create_content_list(contents, preview);
                            image_contents_destroy(contents);
                        }
                    }
                } else if (((OFNOTIFY *)lparam)->hdr.code == CDN_FOLDERCHANGE) {
                    SendMessage(preview, LB_RESETCONTENT, 0, 0);
                    SetWindowText(GetDlgItem(GetParent(hwnd), 0x0480), "");
                }
            }
            break;
        case WM_COMMAND:
            msg_type = LOWORD(wparam);
            switch (msg_type) {
                case IDC_TOGGLE_ATTACH_READONLY:
                    if (res_readonly) {
                        resources_set_int(res_readonly, (IsDlgButtonChecked(hwnd, IDC_TOGGLE_ATTACH_READONLY) == BST_CHECKED));
                    }
                    break;
                case IDC_BLANK_IMAGE:
                    if (SendMessage(GetParent(hwnd), CDM_GETSPEC, 256, (LPARAM)st_filename) <= 1) {
                        ui_error(translate_text(IDS_PLEASE_ENTER_A_FILENAME));
                        return -1;
                    }
                    if (_tcschr(st_filename, '.') == NULL) {
                        append_extension = 1;
                        is_it_standard_extension = 1;
                    } else {
                        /*  Find last dot in name */
                        extension = _tcsrchr(st_filename, '.');
                        /*  Skip dot */
                        extension++;
                        /*  Figure out if it's a standard extension */
                        for (counter = 0; image_type_name[counter]; counter++) {
                            if (strncasecmp(extension, image_type_name[counter], (int)strlen(image_type_name[counter])) == 0) {
                                is_it_standard_extension = 1;
                                break;
                            }
                        }
                    }
                    if (SendMessage(GetParent(hwnd), CDM_GETFILEPATH, 256, (LPARAM)st_filename) >= 0) {
                        char disk_name[32];
                        char disk_id[3];
                        char *format_name;

                        counter = (int)SendMessage(GetDlgItem(hwnd, IDC_BLANK_IMAGE_TYPE), CB_GETCURSEL, 0, 0);
                        if (append_extension) {
                            _tcscat(st_filename, TEXT("."));
                            _tcscat(st_filename, image_type_name[counter]);
                        }
                        system_wcstombs(filename, st_filename, 256);
                        if (util_file_exists(st_filename)) {
                            int ret;

                            ret = ui_messagebox(translate_text(IDS_OVERWRITE_EXISTING_IMAGE), translate_text(IDS_VICE_QUESTION), MB_YESNO | MB_ICONQUESTION);
                            if (ret != IDYES) {
                                return -1;
                            }
                        }
                        GetDlgItemText(hwnd, IDC_BLANK_IMAGE_NAME, disk_name, 17);
                        GetDlgItemText(hwnd, IDC_BLANK_IMAGE_ID, disk_id, 3);
                        format_name = lib_msprintf("%s,%s", disk_name, disk_id);
                        if (vdrive_internal_create_format_disk_image(st_filename, format_name, image_type[counter]) < 0) {
                            ui_error(translate_text(IDS_CANNOT_CREATE_IMAGE));
                            lib_free(format_name);
                            return -1;
                        }
                        lib_free(format_name);
                        /*  Select filter:
                            If we have a standard extension, select the disk filters,
                            but leave at 'All files' if it was already there, otherwise
                            select 'All files'
                         */
                        index = (int)SendMessage(GetDlgItem(GetParent(hwnd), 0x470), CB_GETCOUNT, 0, 0);
                        if (is_it_standard_extension) {
                            if (index - 1 != SendMessage(GetDlgItem(GetParent(hwnd), 0x470), CB_GETCURSEL, 0, 0)) {
                                SendMessage(GetDlgItem(GetParent(hwnd), 0x470), CB_SETCURSEL, 0, 0);
                            }
                        } else {
                            SendMessage(GetDlgItem(GetParent(hwnd), 0x470), CB_SETCURSEL, index - 1, 0);
                        }
                        /*  Notify main window about filter change */
                        SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(0x470, CBN_SELENDOK), (LPARAM)GetDlgItem(GetParent(hwnd), 0x470));

                        /*  Now find filename in ListView & select it */
                        SendMessage(GetParent(hwnd), CDM_GETSPEC, 256, (LPARAM)st_filename);
                        if (append_extension) {
                            _tcscat(st_filename, TEXT("."));
                            _tcscat(st_filename, image_type_name[counter]);
                        }
                        find.flags = LVFI_STRING;
                        find.psz = st_filename;
                        index = (int)SendMessage(GetDlgItem(GetDlgItem(GetParent(hwnd), 0x461), 1), LVM_FINDITEM, -1, (LPARAM)&find);
                        item.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
                        item.state = LVIS_SELECTED | LVIS_FOCUSED;
                        SendMessage(GetDlgItem(GetDlgItem(GetParent(hwnd), 0x461), 1), LVM_SETITEMSTATE, index, (LPARAM)&item);
                    }
                    break;
            }
            switch (HIWORD(wparam)) {
                case LBN_DBLCLK:
                    if (autostart_result != NULL) {
                        index = (int)SendMessage((HWND)lparam, LB_GETCURSEL, 0, 0);
                        if (SendMessage(GetParent(hwnd), CDM_GETFILEPATH, 256, (LPARAM)st_filename) >= 0) {
                            *autostart_result = index;
                            SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(IDOK,BN_CLICKED), (LPARAM)GetDlgItem(GetParent(hwnd), IDOK));
                        }
                    }
                    break;
            }
            break;
        case WM_DESTROY:
            if (hfont != NULL) {
                DeleteObject(hfont);
                hfont = NULL;
            }
            break;
    }
    return 0;
}

static uilib_localize_dialog_param select_hook_trans[] = {
    { IDC_IMAGE_CONTENTS, IDS_IMAGE_CONTENTS, 0 },
    { IDC_TOGGLE_ATTACH_READONLY, IDS_TOGGLE_ATTACH_READONLY, 0 },
    { 0, 0, 0 }
};

static UINT_PTR APIENTRY uilib_select_hook_proc(HWND hwnd, UINT uimsg, WPARAM wparam, LPARAM lparam)
{
    HWND preview;
    image_contents_t *contents;
    char filename[256];
    TCHAR st_filename[256];
    int msg_type;
    int index;
    HWND parent_hwnd;
    int xpos;
    RECT rect;

    preview = GetDlgItem(hwnd, IDC_PREVIEW);
    switch (uimsg) {
        case WM_INITDIALOG:
            parent_hwnd = GetParent(hwnd);
            uilib_localize_dialog(hwnd, select_hook_trans);
            SetWindowText(GetDlgItem(parent_hwnd, IDOK), translate_text(IDS_ATTACH));
            SetWindowText(GetDlgItem(parent_hwnd, IDCANCEL), translate_text(IDS_CANCEL));
            uilib_adjust_element_width(hwnd, IDC_TOGGLE_ATTACH_READONLY);
            uilib_get_element_max_x(hwnd, IDC_TOGGLE_ATTACH_READONLY, &xpos);

            /* set the width of the dialog to 'surround' all the elements */
            GetWindowRect(parent_hwnd, &rect);
            if (xpos + 10 > rect.right) {
                MoveWindow(parent_hwnd, rect.left, rect.top, xpos + 10, rect.bottom - rect.top, TRUE);
            }

            /* Try to use the cbm font */
            if (font_loaded) {
                hfont = CreateFont(-12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "cbm-directory-charset/ck!");
            } else {
                /*  maybe there's a better font-definition (FIXME) */
                /*  I think it's OK now (Tibor) */
                hfont = CreateFont(-12, -7, 0, 0, 400, 0, 0, 0, 0, 0, 0, DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, NULL);
            }

            if (hfont) {
                SendDlgItemMessage(hwnd, IDC_PREVIEW, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0));
            }
            if (res_readonly != NULL) {
                int ro;

                resources_get_int(res_readonly, &ro);
                CheckDlgButton(hwnd, IDC_TOGGLE_ATTACH_READONLY, ro ? BST_CHECKED : BST_UNCHECKED);
            } else {
                EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_ATTACH_READONLY),  FALSE);
            }
            break;
        case WM_NOTIFY:
            if (((OFNOTIFY *)lparam)->hdr.code == CDN_SELCHANGE) {
                SendMessage(preview, LB_RESETCONTENT, 0, 0);
                if (SendMessage(((OFNOTIFY*)lparam)->hdr.hwndFrom, CDM_GETFILEPATH, 256, (LPARAM)st_filename) >= 0) {
                    if (!(GetFileAttributes(st_filename) & FILE_ATTRIBUTE_DIRECTORY)) {
                        system_wcstombs(filename, st_filename, 256);
                        contents = diskcontents_filesystem_read(filename);
                        if (contents == NULL) {
                            contents = tapecontents_read(filename);
                        }
                        if (contents != NULL) {
                            create_content_list(contents, preview);
                            image_contents_destroy(contents);
                        }
                    }
                } else if (((OFNOTIFY *)lparam)->hdr.code == CDN_FOLDERCHANGE) {
                    SendMessage(preview, LB_RESETCONTENT, 0, 0);
                    SetWindowText(GetDlgItem(GetParent(hwnd), 0x0480), "");
                }
            }
            break;
        case WM_COMMAND:
            msg_type = LOWORD(wparam);
            switch (msg_type) {
                case IDC_TOGGLE_ATTACH_READONLY:
                    if (res_readonly) {
                        resources_set_int(res_readonly, (IsDlgButtonChecked(hwnd, IDC_TOGGLE_ATTACH_READONLY) == BST_CHECKED));
                    }
                    break;
            }
            switch (HIWORD(wparam)) {
                case LBN_DBLCLK:
                    if (autostart_result != NULL) {
                        index = (int)SendMessage((HWND)lparam, LB_GETCURSEL, 0, 0);
                        if (SendMessage(GetParent(hwnd), CDM_GETFILEPATH, 256, (LPARAM)st_filename) >= 0) {
                            *autostart_result = index;
                            SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(IDOK,BN_CLICKED), (LPARAM)GetDlgItem(GetParent(hwnd), IDOK));
                        }
                    }
                    break;
            }
            break;
        case WM_DESTROY:
            if (hfont != NULL) {
                DeleteObject(hfont);
                hfont = NULL;
            }
            break;
    }
    return 0;
}

typedef struct filter_per_list_s {
    DWORD filterlist;
    DWORD active_filter;
    struct filter_per_list_s *next;
} filter_per_list_t;

static filter_per_list_t filter_history;
static filter_per_list_t *filter_history_last;

static void update_filter_history(DWORD current_filter, DWORD last_filterlist)
{
    filter_per_list_t *fl = filter_history.next;

    if (filter_history_last == NULL) {
        filter_history_last = &filter_history;
    }

    while (fl && fl->filterlist != last_filterlist) {
        fl = fl->next;
    }

    if (fl) {
        fl->active_filter = current_filter;
    } else {
        fl = lib_malloc(sizeof(filter_per_list_t));
        fl->filterlist = last_filterlist;
        fl->active_filter = current_filter;
        fl->next = NULL;
        filter_history_last->next = fl;
        filter_history_last = fl;
    }
}

static DWORD get_last_active_filter(DWORD last_filterlist)
{
    filter_per_list_t *fl = filter_history.next;

    while (fl && fl->filterlist != last_filterlist) {
        fl = fl->next;
    }

    if (fl) {
        return fl->active_filter;
    } else {
        return 0;
    }
}

static TCHAR *set_filter(DWORD filterlist, DWORD *filterindex)
{
    DWORD i;
    DWORD b;
    TCHAR *filter;
    DWORD current_len, name_len, pattern_len;

    filter = lib_malloc(sizeof(TCHAR));

    *filter = TEXT('\0');

    current_len = 1;

    /* create the strings for the file filters */
    for (i = 0, b = 1; uilib_filefilter[i].name != 0; i++, b <<= 1) {
        if (filterlist & b) {
            name_len = (DWORD)(_tcslen(translate_text(uilib_filefilter[i].name)) + 1) * sizeof(TCHAR);
            pattern_len = (DWORD)(_tcslen(uilib_filefilter[i].pattern) + 1) * sizeof(TCHAR);
            filter = lib_realloc(filter, current_len + name_len + pattern_len);
            memmove(filter + name_len + pattern_len, filter, current_len);
            memcpy(filter, translate_text(uilib_filefilter[i].name), name_len);
            memcpy(filter + name_len, uilib_filefilter[i].pattern, pattern_len);
            current_len += name_len + pattern_len;
        }
    }

    /* search for the most recent file filter */
    *filterindex = get_last_active_filter(filterlist);

    if (*filterindex == 0) {
        *filterindex = 1;    /* not in history: choose first filter */
    }

    return filter;
}

/* ENUMLOGFONT *lpelf   - logical-font data */
/* NEWTEXTMETRIC *lpntm - physical-font data */
/* DWORD FontType       - type of font */
/* LPARAM lParam        - application-defined data */

static int CALLBACK EnumFontProc(ENUMLOGFONT *lpelf, NEWTEXTMETRIC *lpntm, DWORD FontType, LPARAM lParam)
{
    (*(int *)lParam)++;
    return 1;
}

TCHAR *uilib_select_file_autostart(HWND hwnd, const TCHAR *title, DWORD filterlist, unsigned int type, int style, int *autostart, char *resource_readonly)
{
    TCHAR st_name[MAX_PATH];
    char name[MAX_PATH];
    const char *initialdir = NULL;
    const char *initialfile;
    TCHAR *filter;
    DWORD filterindex;
    OPENFILENAME ofn;
    BOOL result;
    char *ret = NULL;

    if (styles[style].initialdir_resource != NULL) {
        resources_get_string(styles[style].initialdir_resource, &initialdir);
    }

    if (type == UILIB_SELECTOR_TYPE_DIR_EXIST) {
        _tcscpy(st_name, TEXT("FilenameNotUsed"));
    } else {
        _tcscpy(st_name, TEXT(""));
    }

    initialfile = ui_file_selector_initialfile[style];
    if (styles[style].file_resource != NULL) {
        resources_get_string(styles[style].file_resource, &initialfile);
    }

    if (initialfile != NULL) {
        _tcscpy(st_name, initialfile);
    }

    if (fontfile == NULL) {
        fontfile = util_concat(archdep_boot_path(), "\\fonts\\cbm-directory-charset.fon", NULL);
        font_loaded = 0;
        EnumFontFamilies(GetDC(NULL), "cbm-directory-charset/ck!", (FONTENUMPROC)EnumFontProc, (LPARAM)&font_loaded);
        if (font_loaded == 0) {
            font_loaded = AddFontResource(fontfile);
        }
    }

    filter = set_filter(filterlist, &filterindex);

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = winmain_instance;
    ofn.lpstrFilter = filter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = filterindex;
    ofn.lpstrFile = st_name;
    ofn.nMaxFile = sizeof(st_name);
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = initialdir;
    ofn.lpstrTitle = title;
    ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOTESTFILECREATE | OFN_SHAREAWARE | OFN_ENABLESIZING;
    if (styles[style].TemplateID != 0) {
        ofn.Flags |= OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;
        ofn.lpfnHook = styles[style].hook_proc;
        ofn.lpTemplateName = MAKEINTRESOURCE(styles[style].TemplateID);
    } else {
        ofn.lpfnHook = NULL;
        ofn.lpTemplateName = NULL;
    }
    if (type == UILIB_SELECTOR_TYPE_FILE_LOAD) {
        ofn.Flags |= OFN_FILEMUSTEXIST;
    }

    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;

    autostart_result = autostart;
    res_readonly = resource_readonly;
    vsync_suspend_speed_eval();

    if (type == UILIB_SELECTOR_TYPE_FILE_SAVE) {
        result = GetSaveFileName(&ofn);
    } else {
        result = GetOpenFileName(&ofn);
    }

    update_filter_history(ofn.nFilterIndex, filterlist);

    if (result) {
        char *tmpdir, *tmpfile;

        lib_free(ui_file_selector_initialfile[style]);
        system_wcstombs(name, st_name, MAX_PATH);
        util_fname_split(name, &tmpdir, &tmpfile);
        if (styles[style].file_resource != NULL) {
            resources_set_string(styles[style].file_resource, tmpfile);
        }
        ui_file_selector_initialfile[style] = system_mbstowcs_alloc(tmpfile);
        resources_set_string(styles[style].initialdir_resource, tmpdir);
        ret = system_wcstombs_alloc(st_name);

        lib_free(tmpdir);
        lib_free(tmpfile);
    }

    lib_free(filter);

    return ret;
}

TCHAR *uilib_select_file(HWND hwnd, const TCHAR *title, DWORD filterlist, unsigned int type, int style)
{
    return uilib_select_file_autostart(hwnd, title, filterlist, type, style, NULL, NULL);
}

void uilib_select_browse(HWND hwnd, const TCHAR *title, DWORD filterlist, unsigned int type, int idc)
{
    TCHAR *st_name;

    st_name = uilib_select_file(hwnd, title, filterlist, type, UILIB_SELECTOR_STYLE_DEFAULT);
    if (st_name != NULL) {
        SetDlgItemText(hwnd, idc, st_name);
        lib_free(st_name);
    }
}

BOOL CALLBACK GetParentEnumProc(HWND hwnd, LPARAM lParam)
{
    DWORD dwWndThread = GetWindowThreadProcessId(hwnd,NULL);

    if (dwWndThread == GetCurrentThreadId()) {
        *(HWND *)lParam = hwnd;
        return FALSE;
    }

    return TRUE;	
}

HWND GetParentHWND()
{
    HWND hwndOut = NULL;

    EnumWindows(GetParentEnumProc, (LPARAM)&hwndOut);

    if (hwndOut == NULL) {
        return NULL;
    }

    return GetLastActivePopup(hwndOut);
}

/* HWND hwndDlg  - handle to dialog box */
/* UINT uMsg     - message */
/* WPARAM wParam - first message parameter */
/* LPARAM lParam - second message parameter */

INT_PTR CALLBACK TextDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct TEXTDLGDATA {
    char *szCaption;
    char *szHeader;
    char *szText;
};

void ui_show_text(HWND hWnd, const char* szCaption, const char* szHeader, const char* szText)
{
    struct TEXTDLGDATA info;
    char * szRNText;
    int i, j;

    szRNText = (char*)HeapAlloc(GetProcessHeap(), 0, 2 * lstrlen(szText) + 1);
    i = j =0;
    while (szText[i] != '\0') {
        if (szText[i] == '\n') {
            szRNText[j++] = '\r';
        }
        szRNText[j++] = szText[i++];
    }
    szRNText[j] = '\0';

    info.szCaption = (char *)szCaption;
    info.szHeader = (char *)szHeader;
    info.szText = szRNText;

    DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_TEXTDLG), hWnd, TextDlgProc, (LPARAM)&info);
    HeapFree(GetProcessHeap(), 0, szRNText);
}

// FIXME: the client area with the scroll bars 
//		disabled would be larger, this function
//		is not perfect.
void AutoHideScrollBar(HWND hWnd, int fnBar)
{
    BOOL bResult;
    SCROLLINFO scInfo;
    UINT uiDiff;
	
    scInfo.cbSize = sizeof(scInfo);
    scInfo.fMask = SIF_RANGE | SIF_PAGE;
    bResult = GetScrollInfo(hWnd, fnBar, &scInfo);

    if (!bResult) {
        return;
    }

    uiDiff = scInfo.nMax-scInfo.nMin;
    if (scInfo.nPage > uiDiff) {
        ShowScrollBar(hWnd, fnBar, 0);
    }
}

INT_PTR CALLBACK TextDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_INITDIALOG:
            {
                struct TEXTDLGDATA* pInfo = (struct TEXTDLGDATA*) lParam;

                SetWindowText(hwndDlg,pInfo->szCaption);
                SetDlgItemText(hwndDlg, IDC_HEADER, pInfo->szHeader);
                SetDlgItemText(hwndDlg, IDOK, translate_text(IDS_OK));

                SetDlgItemText(hwndDlg, IDC_TEXT, pInfo->szText);
                SendDlgItemMessage(hwndDlg, IDC_TEXT, EM_SETREADONLY, 1, 0);
                AutoHideScrollBar(GetDlgItem(hwndDlg, IDC_TEXT), SB_HORZ);
                AutoHideScrollBar(GetDlgItem(hwndDlg, IDC_TEXT), SB_VERT);
                return TRUE;
            }
        case WM_CTLCOLORSTATIC:
            // The text box should use the normal colors, but the contents must
            // be read-only.
            // A read-only text box uses WM_CTLCOLORSTATIC, but a read-write
            // text box uses WM_CTLCOLOREDIT.
            if ((HWND)lParam == GetDlgItem(hwndDlg, IDC_TEXT)) {
                // the return value is passed directly,
                // SetWindowLong(DWL_MSGRESULT) is ignored.
                return DefDlgProc(hwndDlg, WM_CTLCOLOREDIT, wParam, lParam);
            } else {
                return FALSE;
            }
        case WM_CLOSE:
            EndDialog(hwndDlg,0);
            return TRUE;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDCANCEL:
                case IDOK:
                    EndDialog(hwndDlg, 0);
                    return TRUE;
            }
            break;
    }
    return FALSE;
}

void uilib_show_options(HWND param)
{
    char *options;

    options = cmdline_options_string();
    ui_show_text(param, translate_text(IDS_COMMAND_LINE_OPTIONS), translate_text(IDS_COMMAND_OPTIONS_AVAIL), options);
    lib_free(options);
}

void uilib_shutdown(void)
{
    int i;
    filter_per_list_t *f1, *f2;

    for (i = 0; i < UILIB_SELECTOR_STYLES_NUM; i++) {
        lib_free(ui_file_selector_initialfile[i]);
    }

    lib_free(fontfile);

    f1 = filter_history.next;

    while (f1) {
        f2 = f1;
        f1 = f1->next;
        lib_free(f2);
    }
}

int uilib_cpu_is_smp(void)
{
    DWORD_PTR process_affinity;
    DWORD_PTR system_affinity;

    if (GetProcessAffinityMask(GetCurrentProcess(), &process_affinity, &system_affinity)) {
        /* Check if multi CPU system or not */
        if ((system_affinity & (system_affinity - 1))) {
            return 1;
        }
    }
    return 0;
}

static uilib_dialogbox_param_t *uilib_dialogbox_param;

static INT_PTR CALLBACK uilib_dialogbox_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    HWND element;
    int xpos;
    RECT rect;

    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case IDOK:
                    GetDlgItemText(hwnd, uilib_dialogbox_param->idc_dialog, uilib_dialogbox_param->string, UILIB_DIALOGBOX_MAX);
                    uilib_dialogbox_param->updated = 1;
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    return TRUE;
            }
            return FALSE;
        case WM_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;
        case WM_INITDIALOG:
            SetDlgItemText(hwnd, uilib_dialogbox_param->idc_dialog, uilib_dialogbox_param->string);
            element = GetDlgItem(hwnd, uilib_dialogbox_param->idc_dialog_trans);
            SetWindowText(element, uilib_dialogbox_param->idc_dialog_trans_text);
            element = GetDlgItem(hwnd, IDOK);
            SetWindowText(element, translate_text(IDS_OK));
            element = GetDlgItem(hwnd, IDCANCEL);
            SetWindowText(element, translate_text(IDS_CANCEL));
            SetWindowText(hwnd, uilib_dialogbox_param->idd_dialog_caption);
            uilib_adjust_element_width(hwnd, uilib_dialogbox_param->idc_dialog_trans);
            uilib_get_element_max_x(hwnd, uilib_dialogbox_param->idc_dialog_trans, &xpos);
            uilib_move_element(hwnd, uilib_dialogbox_param->idc_dialog, xpos + 10);
            uilib_get_element_max_x(hwnd, uilib_dialogbox_param->idc_dialog, &xpos);
            GetWindowRect(hwnd, &rect);
            MoveWindow(hwnd, rect.left, rect.top, xpos + 20, rect.bottom - rect.top, TRUE);
            return TRUE;
    }
    return FALSE;
}

void uilib_dialogbox(uilib_dialogbox_param_t *param)
{
    uilib_dialogbox_param = param;
    uilib_dialogbox_param->updated = 0;
    DialogBox(winmain_instance, (LPCTSTR)(UINT_PTR)(uilib_dialogbox_param->idd_dialog), uilib_dialogbox_param->hwnd, uilib_dialogbox_dialog_proc);
}

void uilib_get_general_window_extents(HWND hwnd, int *xsize, int *ysize)
{
    HDC hdc;
    HFONT hFont;
    HFONT hOldFont;
    int strlen;
    char *buffer;
    SIZE  size;

    hFont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
    strlen = (int)SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
    buffer = malloc(strlen + 1);
    GetWindowText(hwnd, buffer, strlen + 1);

    hdc = GetDC(hwnd);
    hOldFont = (HFONT)SelectObject(hdc, hFont);

    GetTextExtentPoint32(hdc, buffer, strlen, &size);

    free(buffer);

    SelectObject(hdc, hOldFont);
    ReleaseDC(hwnd, hdc);

    *xsize = size.cx;
    *ysize = size.cy;
}

void uilib_get_group_extent(HWND hwnd, uilib_dialog_group *group, int *xsize, int *ysize)
{
    HWND element;
    int x;
    int y;

    if (xsize && ysize) {
        *xsize = 0;
        *ysize = 0;
        while (group->idc) {
            element = GetDlgItem(hwnd, group->idc);
            uilib_get_general_window_extents(element, &x, &y);
            if (group->element_type == 1) {
                x += 20;
            }
            if (*xsize < x) {
                *xsize = x;
            }
            *ysize += y;
            group++;
        }
    }
}

void uilib_get_group_max_x(HWND hwnd, uilib_dialog_group *group, int *xpos)
{
    HWND element;
    RECT element_rect;

    if (xpos) {
        *xpos = 0;
        while (group->idc) {
            element = GetDlgItem(hwnd, group->idc);
            GetClientRect(element, &element_rect);
            MapWindowPoints(element, hwnd, (POINT*)&element_rect, 2);

            if (*xpos < element_rect.right) {
                *xpos = element_rect.right;
            }
            group++;
        }
    }
}

void uilib_get_group_width(HWND hwnd, uilib_dialog_group *group, int *width)
{
    HWND element;
    RECT element_rect;

    if (width) {
        *width = 0;
        while (group->idc) {
            element = GetDlgItem(hwnd, group->idc);
            GetClientRect(element, &element_rect);
            MapWindowPoints(element, hwnd, (POINT*)&element_rect, 2);

            if (*width < element_rect.right - element_rect.left) {
                *width = element_rect.right - element_rect.left;
            }
            group++;
        }
    }
}

void uilib_get_group_min_x(HWND hwnd, uilib_dialog_group *group, int *xpos)
{
    HWND element;
    RECT element_rect;

    if (xpos) {
        *xpos = 0xffffff;
        while (group->idc) {
            element = GetDlgItem(hwnd, group->idc);
            GetClientRect(element, &element_rect);
            MapWindowPoints(element, hwnd, (POINT*)&element_rect, 2);

            if (*xpos > element_rect.left) {
                *xpos = element_rect.left;
            }
            group++;
        }
    }
}

void uilib_move_and_adjust_group_width(HWND hwnd, uilib_dialog_group *group, int xpos)
{
    HWND element;
    RECT element_rect;
    int xsize;
    int ysize;

    while (group->idc) {
        element = GetDlgItem(hwnd, group->idc);
        GetClientRect(element, &element_rect);
        MapWindowPoints(element, hwnd, (POINT*)&element_rect, 2);
        uilib_get_general_window_extents(element, &xsize, &ysize);
        if (group->element_type == 1) {
            xsize += 20;
        }
        MoveWindow(element, xpos, element_rect.top, xsize, element_rect.bottom - element_rect.top, TRUE);
        group++;
    }
}

void uilib_move_group(HWND hwnd, uilib_dialog_group *group, int xpos)
{
    HWND element;
    RECT element_rect;

    while (group->idc) {
        element = GetDlgItem(hwnd, group->idc);
        GetClientRect(element, &element_rect);
        MapWindowPoints(element, hwnd, (POINT*)&element_rect, 2);
        MoveWindow(element, xpos, element_rect.top, element_rect.right - element_rect.left, element_rect.bottom - element_rect.top, TRUE);
        group++;
    }
}

void uilib_adjust_group_width(HWND hwnd, uilib_dialog_group *group)
{
    HWND element;
    RECT element_rect;
    int xsize;
    int ysize;

    while (group->idc) {
        element = GetDlgItem(hwnd, group->idc);
        GetClientRect(element, &element_rect);
        MapWindowPoints(element, hwnd, (POINT*)&element_rect, 2);
        uilib_get_general_window_extents(element, &xsize, &ysize);
        if (group->element_type == 1) {
            xsize += 20;
        }
        MoveWindow(element, element_rect.left, element_rect.top, xsize, element_rect.bottom - element_rect.top, TRUE);
        group++;
    }
}

void uilib_set_group_width(HWND hwnd, uilib_dialog_group *group, int size)
{
    HWND element;
    RECT element_rect;

    while (group->idc) {
        element = GetDlgItem(hwnd, group->idc);
        GetClientRect(element, &element_rect);
        MapWindowPoints(element, hwnd, (POINT*)&element_rect, 2);
        MoveWindow(element, element_rect.left, element_rect.top, size, element_rect.bottom - element_rect.top, TRUE);
        group++;
    }
}

void uilib_move_and_adjust_element_width(HWND hwnd, int idc, int xpos)
{
    HWND element;
    RECT element_rect;
    int xsize;
    int ysize;

    element = GetDlgItem(hwnd, idc);
    GetClientRect(element, &element_rect);
    MapWindowPoints(element, hwnd, (POINT*)&element_rect, 2);
    uilib_get_general_window_extents(element, &xsize, &ysize);
    MoveWindow(element, xpos, element_rect.top, xsize, element_rect.bottom - element_rect.top, TRUE);
}

void uilib_move_and_set_element_width(HWND hwnd, int idc, int xpos, int new_xsize)
{
    HWND element;
    RECT element_rect;
    int xsize;
    int ysize;

    element = GetDlgItem(hwnd, idc);
    GetClientRect(element, &element_rect);
    MapWindowPoints(element, hwnd, (POINT*)&element_rect, 2);
    uilib_get_general_window_extents(element, &xsize, &ysize);
    MoveWindow(element, xpos, element_rect.top, new_xsize, element_rect.bottom - element_rect.top, TRUE);
}

void uilib_move_element(HWND hwnd, int idc, int xpos)
{
    HWND element;
    RECT element_rect;

    element = GetDlgItem(hwnd, idc);
    GetClientRect(element, &element_rect);
    MapWindowPoints(element, hwnd, (POINT*)&element_rect, 2);
    MoveWindow(element, xpos, element_rect.top, element_rect.right - element_rect.left, element_rect.bottom - element_rect.top, TRUE);
}

void uilib_adjust_element_width(HWND hwnd, int idc)
{
    HWND element;
    RECT element_rect;
    int xsize;
    int ysize;

    element = GetDlgItem(hwnd, idc);
    GetClientRect(element, &element_rect);
    MapWindowPoints(element, hwnd, (POINT*)&element_rect, 2);
    uilib_get_general_window_extents(element, &xsize, &ysize);
    MoveWindow(element, element_rect.left, element_rect.top, xsize + 20, element_rect.bottom - element_rect.top, TRUE);
}

void uilib_set_element_width(HWND hwnd, int idc, int xsize)
{
    HWND element;
    RECT element_rect;

    element = GetDlgItem(hwnd, idc);
    GetClientRect(element, &element_rect);
    MapWindowPoints(element, hwnd, (POINT*)&element_rect, 2);
    MoveWindow(element, element_rect.left, element_rect.top, xsize, element_rect.bottom - element_rect.top, TRUE);
}

void uilib_get_element_width(HWND hwnd, int idc, int *width)
{
    HWND temp_hwnd;
    RECT element_rect;
    int xsize;
    int ysize;

    temp_hwnd = GetDlgItem(hwnd, idc);
    GetClientRect(temp_hwnd, &element_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&element_rect, 2);
    uilib_get_general_window_extents(temp_hwnd, &xsize, &ysize);
    *width = xsize;
}

void uilib_get_element_size(HWND hwnd, int idc, int *width)
{
    HWND temp_hwnd;
    RECT element_rect;

    temp_hwnd = GetDlgItem(hwnd, idc);
    GetClientRect(temp_hwnd, &element_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&element_rect, 2);
    *width = element_rect.right - element_rect.left;
}

void uilib_get_element_max_x(HWND hwnd, int idc, int *width)
{
    HWND temp_hwnd;
    RECT element_rect;

    temp_hwnd = GetDlgItem(hwnd, idc);
    GetClientRect(temp_hwnd, &element_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&element_rect, 2);
    *width = element_rect.right;
}

void uilib_get_element_min_x(HWND hwnd, int idc, int *width)
{
    HWND temp_hwnd;
    RECT element_rect;

    temp_hwnd = GetDlgItem(hwnd, idc);
    GetClientRect(temp_hwnd, &element_rect);
    MapWindowPoints(temp_hwnd, hwnd, (POINT*)&element_rect, 2);
    *width = element_rect.left;
}

void uilib_localize_dialog(HWND hwnd, uilib_localize_dialog_param *param)
{
    HWND element;

    while (param->idc || param->ids) {
        if (param->element_type == -1) {
            SetWindowText(hwnd, translate_text(param->ids));
        } else if (param->element_type == 0) {
            element = GetDlgItem(hwnd, param->idc);
            SetWindowText(element, translate_text(param->ids));
        }
        param++;
    }
}

void uilib_center_buttons(HWND hwnd, int *buttons, int resize)
{
    int i;    
    int size = 0;
    HWND temp_hwnd;
    RECT element_rect;
    RECT rect;
    int distance;
    int xpos;

    for (i = 0; buttons[i] != 0; i++) {
        temp_hwnd = GetDlgItem(hwnd, buttons[i]);
        GetClientRect(temp_hwnd, &element_rect);
        MapWindowPoints(temp_hwnd, hwnd, (POINT*)&element_rect, 2);
        if (size < element_rect.right - element_rect.left) {
            size = element_rect.right - element_rect.left;
        }
    }
    GetWindowRect(hwnd, &rect);
    distance = ((rect.right - rect.left) - (size * i)) / (i + 1);
    if (distance < 10) {
        distance = 10;
        MoveWindow(hwnd, rect.left, rect.top, (distance * (i + 1)) + (size * i), rect.bottom - rect.top, TRUE);
    }
    xpos = distance;
    for (i = 0; buttons[i] != 0; i++) {
        if (resize) {
            uilib_move_and_set_element_width(hwnd, buttons[i], xpos, size);
        } else {
            uilib_move_element(hwnd, buttons[i], xpos);
        }

        xpos += size + distance;
    }
}

int uilib_get_center_distance_group(HWND hwnd, int *group_boxes)
{
    int i;
    int size = 0;
    HWND temp_hwnd;
    RECT element_rect;
    RECT rect;
    int distance;

    for (i = 0; group_boxes[i] != 0; i++) {
        temp_hwnd = GetDlgItem(hwnd, group_boxes[i]);
        GetClientRect(temp_hwnd, &element_rect);
        MapWindowPoints(temp_hwnd, hwnd, (POINT*)&element_rect, 2);
        if (size < element_rect.right - element_rect.left) {
            size = element_rect.right - element_rect.left;
        }
    }
    GetWindowRect(hwnd, &rect);
    distance = ((rect.right - rect.left) - (size * i)) / (i + 1);
    if (distance < 10) {
        distance = 10;
        MoveWindow(hwnd, rect.left, rect.top, (distance * (i + 1)) + (size * i), rect.bottom - rect.top, TRUE);
    }
    return distance;
}
