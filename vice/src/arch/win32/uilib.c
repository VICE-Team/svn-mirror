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
#include "lib.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "ui.h"
#include "uilib.h"
#include "util.h"
#include "vdrive-internal.h"
#include "vsync.h"
#include "winmain.h"


/* Mingw & pre VC 6 headers doesn't have this definition */
#ifndef OFN_ENABLESIZING
#define OFN_ENABLESIZING    0x00800000
#endif

static char *(*read_content_func)(const char *);
static int *autostart_result;
static char* fontfile;
static int font_loaded;

struct uilib_filefilter_s {
    TCHAR *name;
    TCHAR *pattern;
};
typedef struct uilib_filefilter_s uilib_filefilter_t;

static uilib_filefilter_t uilib_filefilter[] = {
    { TEXT("All files (*.*)"), TEXT("*.*") },
    { TEXT("VICE palette files (*.vpl)"), TEXT("*.vpl") },
    { TEXT("VICE snapshot files (*.vsf)"), TEXT("*.vsf") },
    { TEXT("Disk image files (*.d64;*.d71;*.d80;*.d81;*.d82;*.g64;*.g41;*.x64)"),
      TEXT("*.d64;*.d71;*.d80;*.d81;*.d82;*.g64;*.g41;*.x64") },
    { TEXT("Tape image files (*.t64;*.p00;*.tap;*.prg)"),
      TEXT("*.t64;*.p00;*.tap;*.prg") },
    { TEXT("Zipped files (*.zip;*.bz2;*.gz;*.d6z;*.d7z;*.d8z;*.g6z;*.g4z;*.x6z)"),
      TEXT("*.zip;*.bz2;*.gz;*.d6z;*.d7z;*.d8z;*.g6z;*.g4z;*.x6z") },
    { TEXT("CRT cartridge image files (*.crt)"), TEXT("*.crt") },
    { TEXT("Raw cartridge image files (*.bin)"), TEXT("*.bin") },
    { TEXT("Flip list files (*.vfl)"), TEXT("*.vfl") },
    { TEXT("VICE romset files (*.vrs)"), TEXT("*.vrs") },
    { TEXT("VICE romset archives (*.vra)"), TEXT("*.vra") },
    { NULL, NULL }
};

struct uilib_fs_style_type_s {
    char *(*content_read_function)(const char *);
    LPOFNHOOKPROC hook_proc;
    int TemplateID;
    char *initialdir_resource;
    char *file_resource;
};
typedef struct uilib_fs_style_type_s uilib_fs_style_type_t;

static UINT APIENTRY uilib_select_tape_hook_proc(HWND hwnd, UINT uimsg,
                                                 WPARAM wparam, LPARAM lparam);
static UINT APIENTRY uilib_select_hook_proc(HWND hwnd, UINT uimsg,
                                            WPARAM wparam, LPARAM lparam);

static char *read_disk_image_contents(const char *name);
static char *read_tape_image_contents(const char *name);
static char *read_disk_or_tape_image_contents(const char *name);

static uilib_fs_style_type_t styles[UILIB_SELECTOR_STYLES_NUM + 1] = {
    /* UILIB_SELECTOR_STYLE_DEFAULT */
    { NULL,
      NULL, 0, "InitialDefaultDir", NULL },
    /* UILIB_SELECTOR_STYLE_TAPE */
    { read_tape_image_contents,
      uilib_select_tape_hook_proc, IDD_OPENTAPE_TEMPLATE, "InitialTapeDir",
      NULL },
    /* UILIB_SELECTOR_STYLE_DISK */
    { read_disk_image_contents,
      uilib_select_hook_proc, IDD_OPEN_TEMPLATE, "InitialDiskDir", NULL },
    /* UILIB_SELECTOR_STYLE_DISK_AND_TAPE */
    { read_disk_or_tape_image_contents,
      uilib_select_hook_proc, IDD_OPEN_TEMPLATE, "InitialAutostartDir", NULL },
    /* UILIB_SELECTOR_STYLE_CART */
    { NULL,
      NULL, 0, "InitialCartDir", NULL },
    /* UILIB_SELECTOR_STYLE_SNAPSHOT */
    { NULL,
      NULL, 0, "InitialSnapshotDir", NULL },
    /* UILIB_SELECTOR_STYLE_EVENT_START */
    { NULL,
      NULL, 0, "EventSnapshotDir", "EventStartSnapshot" },
    /* UILIB_SELECTOR_STYLE_EVENT_END */
    { NULL,
      NULL, 0, "EventSnapshotDir", "EventEndSnapshot"  },
    /* DUMMY entry Insert new styles before this */
    { NULL,
      NULL, 0, NULL, NULL }
};

static TCHAR *ui_file_selector_initialfile[UILIB_SELECTOR_STYLES_NUM];


static char *read_disk_image_contents(const char *name)
{
    return image_contents_read_string(IMAGE_CONTENTS_DISK, name, 0,
                                      IMAGE_CONTENTS_STRING_PETSCII);
}

static char *read_tape_image_contents(const char *name)
{
    return image_contents_read_string(IMAGE_CONTENTS_TAPE, name, 0,
                                      IMAGE_CONTENTS_STRING_PETSCII);
}

static char *read_disk_or_tape_image_contents(const char *name)
{
    return image_contents_read_string(IMAGE_CONTENTS_AUTO, name, 0,
                                      IMAGE_CONTENTS_STRING_PETSCII);
}

static void create_content_list(char *text, HWND list)
{
    char *start;
    char buffer[256];
    int index;

    if (text == NULL)
        return;

    start = text;
    index = 0;
    while (1) {
        if (*start == 0x0a) {
            buffer[index] = 0;
            index = 0;
            SendMessage(list, LB_ADDSTRING, 0, (LPARAM)buffer);
        } else if (*start == 0x0d) {
        } else if (*start == 0) {
            break;
        } else {
            buffer[index++] = *start;
        }
        start++;
    }
}

static HFONT hfont;

static UINT APIENTRY uilib_select_tape_hook_proc(HWND hwnd, UINT uimsg,
                                                 WPARAM wparam, LPARAM lparam)
{
    HWND preview;
    char *contents;
    char filename[256];
    int index;
    int append_extension = 0;
    char *extension;

    preview = GetDlgItem(hwnd, IDC_PREVIEW);
    switch (uimsg) {
      case WM_INITDIALOG:
        SetWindowText(GetDlgItem(GetParent(hwnd), IDOK), TEXT("&Attach"));

        if (font_loaded)
            hfont = CreateFont(-12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, "cbm-directory-charset/ck!");
        else
            /*  maybe there's a better font-definition (FIXME) */
            /*  I think it's OK now (Tibor) */
            hfont = CreateFont(-12, -7, 0, 0, 400, 0, 0, 0, 0, 0, 0,
                DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, NULL);

        if (hfont)
            SendDlgItemMessage(hwnd, IDC_PREVIEW,WM_SETFONT,
                (WPARAM)hfont, MAKELPARAM(TRUE, 0));
        break;
      case WM_NOTIFY:
        if (((OFNOTIFY*)lparam)->hdr.code == CDN_SELCHANGE) {
            SendMessage(preview, LB_RESETCONTENT, 0, 0);
            if (SendMessage(((OFNOTIFY*)lparam)->hdr.hwndFrom,
                CDM_GETFILEPATH, 256, (LPARAM)filename) >= 0) {
                if (!(GetFileAttributes(filename)
                    & FILE_ATTRIBUTE_DIRECTORY)) {
                    if (read_content_func != NULL) {
                        contents = read_content_func(filename);
                        create_content_list(contents, preview);
                        lib_free(contents);
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
            if (SendMessage(GetParent(hwnd),
                CDM_GETSPEC, 256, (LPARAM)filename) <= 1) {
                ui_error("Please enter a filename.");
                return -1;
            }
            if (strchr(filename,'.') == NULL) {
                append_extension = 1;
            } else {
                /*  Find last dot in name */
                extension = strrchr(filename,'.');
                /*  Skip dot */
                extension++;
                /*  Figure out if it's a standard extension */
                if (strncasecmp(extension, "tap", 3) == 0) {
                }
            }
            if (SendMessage(GetParent(hwnd),
                CDM_GETFILEPATH, 256, (LPARAM)filename) >= 0) {
                if (append_extension) {
                    strcat(filename, ".");
                    strcat(filename, "tap");
                }
                if (util_file_exists(filename)) {
                    int ret;
                    ret = ui_messagebox(TEXT("Overwrite existing image?"),
                                        TEXT("VICE question"),
                                        MB_YESNO | MB_ICONQUESTION);
                    if (ret != IDYES)
                        return -1;
                }
                if (cbmimage_create_image(filename, DISK_IMAGE_TYPE_TAP)) {
                    ui_error("Cannot create image");
                    return -1;
                }
            }
            break;
        }

        switch (HIWORD(wparam)) {
          case LBN_DBLCLK:
            if (autostart_result != NULL) {
                index = SendMessage((HWND)lparam, LB_GETCURSEL, 0, 0);
                if (SendMessage(GetParent(hwnd),
                    CDM_GETFILEPATH, 256, (LPARAM)filename) >= 0) {
                    *autostart_result = index;
                    SendMessage(GetParent(hwnd), WM_COMMAND,
                                MAKELONG(IDOK, BN_CLICKED),
                                (LPARAM)GetDlgItem(GetParent(hwnd), IDOK));
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

static TCHAR *image_type_name[] = 
    { TEXT("d64"), TEXT("d71"), TEXT("d80"), TEXT("d81"), TEXT("d82"),
      TEXT("g64"), TEXT("x64"), NULL };

static int image_type[] = {
    DISK_IMAGE_TYPE_D64,
    DISK_IMAGE_TYPE_D71,
    DISK_IMAGE_TYPE_D80,
    DISK_IMAGE_TYPE_D81,
    DISK_IMAGE_TYPE_D82,
    DISK_IMAGE_TYPE_G64,
    DISK_IMAGE_TYPE_X64
};

static UINT APIENTRY uilib_select_hook_proc(HWND hwnd, UINT uimsg,
                                            WPARAM wparam, LPARAM lparam)
{
    HWND preview;
    HWND image_type_list;
    char *contents;
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

    preview = GetDlgItem(hwnd, IDC_PREVIEW);
    switch (uimsg) {
      case WM_INITDIALOG:
        SetWindowText(GetDlgItem(GetParent(hwnd), IDOK), "&Attach");
        image_type_list = GetDlgItem(hwnd, IDC_BLANK_IMAGE_TYPE);
        for (counter = 0; image_type_name[counter]; counter++) {
            SendMessage(image_type_list, CB_ADDSTRING, 0,
                (LPARAM)image_type_name[counter]);
        }
        SendMessage(image_type_list, CB_SETCURSEL, (WPARAM)0, 0);

        /* Try to use the cbm font */
        if (font_loaded)
            hfont = CreateFont(-12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, "cbm-directory-charset/ck!");
        else
            /*  maybe there's a better font-definition (FIXME) */
            /*  I think it's OK now (Tibor) */
            hfont = CreateFont(-12, -7, 0, 0, 400, 0, 0, 0, 0, 0, 0,
                               DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, NULL);

        if (hfont) {
            SendDlgItemMessage(hwnd, IDC_PREVIEW, WM_SETFONT,
                (WPARAM)hfont, MAKELPARAM(TRUE, 0));
        }
        SetDlgItemText(hwnd, IDC_BLANK_IMAGE_NAME, TEXT("vice"));
        SetDlgItemText(hwnd, IDC_BLANK_IMAGE_ID, TEXT("1a"));
        break;
      case WM_NOTIFY:
        if (((OFNOTIFY *)lparam)->hdr.code == CDN_SELCHANGE) {
            SendMessage(preview, LB_RESETCONTENT, 0, 0);
            if (SendMessage(((OFNOTIFY*)lparam)->hdr.hwndFrom,
                CDM_GETFILEPATH, 256, (LPARAM)st_filename) >= 0) {
                     if (!(GetFileAttributes(st_filename)
                         & FILE_ATTRIBUTE_DIRECTORY)) {
                         if (read_content_func != NULL) {
                             system_wcstombs(filename, st_filename, 256);
                             contents = read_content_func(filename);
                             create_content_list(contents, preview);
                             lib_free(contents);
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
          case IDC_BLANK_IMAGE:
            if (SendMessage(GetParent(hwnd),
                CDM_GETSPEC, 256, (LPARAM)st_filename) <= 1) {
                ui_error("Please enter a filename.");
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
                    if (strncasecmp(extension, image_type_name[counter],
                        strlen(image_type_name[counter])) == 0) {
                        is_it_standard_extension = 1;
                        break;
                    }
                }
            }
            if (SendMessage(GetParent(hwnd),
                CDM_GETFILEPATH, 256, (LPARAM)st_filename) >= 0) {
                char disk_name[32];
                char disk_id[3];
                char *format_name;

                counter = SendMessage(GetDlgItem(hwnd, IDC_BLANK_IMAGE_TYPE),
                                                 CB_GETCURSEL, 0, 0);
                if (append_extension) {
                    _tcscat(st_filename, TEXT("."));
                    _tcscat(st_filename, image_type_name[counter]);
                }
                system_wcstombs(filename, st_filename, 256);
                if (util_file_exists(st_filename)) {
                    int ret;
                    ret = ui_messagebox(TEXT("Overwrite existing image?"),
                                        TEXT("VICE question"),
                                        MB_YESNO | MB_ICONQUESTION);
                    if (ret != IDYES)
                        return -1;
                }
                GetDlgItemText(hwnd, IDC_BLANK_IMAGE_NAME, disk_name, 17);
                GetDlgItemText(hwnd, IDC_BLANK_IMAGE_ID, disk_id, 3);
                format_name = lib_msprintf("%s,%s", disk_name, disk_id);
                if (vdrive_internal_create_format_disk_image(st_filename,
                    format_name, image_type[counter]) < 0) {
                    ui_error("Cannot create image");
                    lib_free(format_name);
                    return -1;
                }
                lib_free(format_name);
                /*  Select filter:
                    If we have a standard extension, select the disk filters,
                    but leave at 'All files' if it was already there, otherwise
                    select 'All files'
                */
                index = SendMessage(GetDlgItem(GetParent(hwnd), 0x470),
                                    CB_GETCOUNT, 0, 0);
                if (is_it_standard_extension) {
                    if (index - 1 != SendMessage(GetDlgItem(GetParent(hwnd),
                        0x470), CB_GETCURSEL, 0, 0)) {
                        SendMessage(GetDlgItem(GetParent(hwnd), 0x470),
                                    CB_SETCURSEL, 0, 0);
                    }
                } else {
                    SendMessage(GetDlgItem(GetParent(hwnd), 0x470),
                                CB_SETCURSEL, index - 1, 0);
                }
                /*  Notify main window about filter change */
                SendMessage(GetParent(hwnd), WM_COMMAND,
                            MAKELONG(0x470, CBN_SELENDOK),
                            (LPARAM)GetDlgItem(GetParent(hwnd), 0x470));

                /*  Now find filename in ListView & select it */
                SendMessage(GetParent(hwnd), CDM_GETSPEC, 256,
                            (LPARAM)st_filename);
                if (append_extension) {
                    _tcscat(st_filename, TEXT("."));
                    _tcscat(st_filename, image_type_name[counter]);
                }
                find.flags = LVFI_STRING;
                find.psz = st_filename;
                index = SendMessage(GetDlgItem(GetDlgItem(GetParent(hwnd),
                        0x461), 1), LVM_FINDITEM, -1, (LPARAM)&find);
                item.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
                item.state = LVIS_SELECTED | LVIS_FOCUSED;
                SendMessage(GetDlgItem(GetDlgItem(GetParent(hwnd), 0x461), 1),
                            LVM_SETITEMSTATE, index, (LPARAM)&item);
            }
            break;
        }
        switch (HIWORD(wparam)) {
          case LBN_DBLCLK:
            if (autostart_result != NULL) {
                index = SendMessage((HWND)lparam, LB_GETCURSEL, 0, 0);
                if (SendMessage(GetParent(hwnd),
                    CDM_GETFILEPATH, 256, (LPARAM)st_filename) >= 0) {
                    *autostart_result = index;
                    SendMessage(GetParent(hwnd), WM_COMMAND,
                                MAKELONG(IDOK,BN_CLICKED),
                                (LPARAM)GetDlgItem(GetParent(hwnd), IDOK));
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

/* The following stuff implements a history for the file filter.    */
/* The filter selected by the user in the OpenFile window is        */
/* gets the default one in the next OpenFile window                 */
/* As the same window is used for different file types and          */
/* filter lists some more work has to be done...                    */
#define UI_LIB_FILTER_HISTORY_LENGTH   8
static DWORD last_filterlist;
static DWORD filter_history[UI_LIB_FILTER_HISTORY_LENGTH];
static DWORD filter_history_current;

static DWORD get_index_from_filterbit(DWORD filterbit, DWORD filterlist)
{
    DWORD b;
    int j = 0;

    for (b = 1; b <= filterbit; b <<= 1) {
        if (filterlist & b)
            j++;
        if (b == filterbit)
            break;
    }

    return j;
}

static void update_filter_history(DWORD current_filter)
{
    int i;
    DWORD b;
    for (i = 0, b = 1; uilib_filefilter[i].name != NULL; i++, b <<= 1) {
        if ((b & last_filterlist) 
            && (get_index_from_filterbit(b, last_filterlist)
            == current_filter)) {
            filter_history[filter_history_current++] = b;
            if (filter_history_current >= UI_LIB_FILTER_HISTORY_LENGTH)
                filter_history_current = 0;
            break;
        }
    }
}

static TCHAR *set_filter(DWORD filterlist, DWORD *filterindex)
{
    DWORD i, j, k, l;
    DWORD b;
    TCHAR *filter, *current;

    filter = lib_malloc(UILIB_FILTER_LENGTH_MAX * sizeof(TCHAR));

    current = filter;

    last_filterlist = filterlist;

    *filterindex = 0;

    /* create the strings for the file filters */
    for (i = 0, b = 1; uilib_filefilter[i].name != NULL; i++, b <<= 1) {
        if (filterlist & b) {
            j = _tcslen(uilib_filefilter[i].name) + 1;
            memcpy(current, uilib_filefilter[i].name, j * sizeof(TCHAR));
            current += j;

            j = _tcslen(uilib_filefilter[i].pattern) + 1;
            memcpy(current, uilib_filefilter[i].pattern, j * sizeof(TCHAR));
            current += j;
        }
    }

    *current = TEXT('\0');

    /* search for the most recent file filter */
    for (k = 1; k <= UI_LIB_FILTER_HISTORY_LENGTH && *filterindex == 0; k++) {
        l = (filter_history_current - k) % UI_LIB_FILTER_HISTORY_LENGTH;
        if (filter_history[l] & filterlist)
            *filterindex = get_index_from_filterbit(filter_history[l],
                                                    filterlist);
    }
    if (*filterindex == 0)
        *filterindex = 1;    /* not in history: choose first filter */

    return filter;
}

static int CALLBACK EnumFontProc(
  ENUMLOGFONT *lpelf,    // logical-font data
  NEWTEXTMETRIC *lpntm,  // physical-font data
  DWORD FontType,        // type of font
  LPARAM lParam          // application-defined data
)
{
    (*(int *)lParam)++;
    return 1;
}


TCHAR *uilib_select_file_autostart(HWND hwnd, const TCHAR *title,
                                   DWORD filterlist, unsigned int type,
                                   int style, int *autostart)
{
    TCHAR st_name[MAX_PATH];
    char name[MAX_PATH];
    char *initialdir = NULL;
    char *initialfile;
    TCHAR *filter;
    DWORD filterindex;
    OPENFILENAME ofn;
    int result;
    char *ret = NULL;

    if (styles[style].initialdir_resource != NULL)
        resources_get_value(styles[style].initialdir_resource,
            (void *)&initialdir);

    if (type == UILIB_SELECTOR_TYPE_DIR_EXIST)
        _tcscpy(st_name, TEXT("FilenameNotUsed"));
    else
        _tcscpy(st_name, TEXT(""));

    initialfile = ui_file_selector_initialfile[style];
    if (styles[style].file_resource != NULL)
        resources_get_value(styles[style].file_resource,
            (void *)&initialfile);

    if (initialfile != NULL)
        _tcscpy(st_name, initialfile);

    if (fontfile == NULL) {
        fontfile = util_concat(archdep_boot_path(), 
                               "\\fonts\\cbm-directory-charset.fon", NULL);
        font_loaded = 0;
        EnumFontFamilies(GetDC(NULL), "cbm-directory-charset/ck!",
            (FONTENUMPROC)EnumFontProc, (LPARAM)&font_loaded);
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
    ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOTESTFILECREATE
                | OFN_SHAREAWARE | OFN_ENABLESIZING;
    if (styles[style].TemplateID != 0) {
        ofn.Flags |= OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;
        ofn.lpfnHook = styles[style].hook_proc;
        ofn.lpTemplateName = MAKEINTRESOURCE(styles[style].TemplateID);
    } else {
        ofn.lpfnHook = NULL;
        ofn.lpTemplateName = NULL;
    }
    if (type == UILIB_SELECTOR_TYPE_FILE_LOAD)
        ofn.Flags |= OFN_FILEMUSTEXIST;

    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;

    read_content_func = styles[style].content_read_function;
    autostart_result = autostart;
    vsync_suspend_speed_eval();

    if (type == UILIB_SELECTOR_TYPE_FILE_SAVE)
        result = GetSaveFileName(&ofn);
    else
        result = GetOpenFileName(&ofn);

    update_filter_history(ofn.nFilterIndex);

    if (result) {
        char *tmpdir, *tmpfile;

        if (ui_file_selector_initialfile[style] != NULL)
            lib_free(ui_file_selector_initialfile[style]);
        system_wcstombs(name, st_name, 1024);
        util_fname_split(name, &tmpdir, &tmpfile);
        if (styles[style].file_resource != NULL)
            resources_set_value(styles[style].file_resource, tmpfile);
        ui_file_selector_initialfile[style] = system_mbstowcs_alloc(tmpfile);
        resources_set_value(styles[style].initialdir_resource, tmpdir);
        ret = system_wcstombs_alloc(st_name);

        lib_free(tmpdir);
        lib_free(tmpfile);
    }

    lib_free(filter);

    return ret;
}

TCHAR *uilib_select_file(HWND hwnd, const TCHAR *title, DWORD filterlist,
                         unsigned int type, int style)
{
    return uilib_select_file_autostart(hwnd, title, filterlist, type, style,
                                       NULL);
}

void uilib_select_browse(HWND hwnd, const TCHAR *title, DWORD filterlist,
                         unsigned int type, int idc)
{
    TCHAR *st_name;

    st_name = uilib_select_file(hwnd, title, filterlist, type,
                                UILIB_SELECTOR_STYLE_DEFAULT);
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

    if(hwndOut == NULL)
        return NULL;

    return GetLastActivePopup(hwndOut);
}

BOOL CALLBACK TextDlgProc(HWND hwndDlg,		// handle to dialog box
			  UINT uMsg,		// message
			  WPARAM wParam,	// first message parameter
			  LPARAM lParam);	// second message parameter
struct TEXTDLGDATA {
    char *szCaption;
    char *szHeader;
    char *szText;
};

void ui_show_text(HWND hWnd,
		const char* szCaption,
		const char* szHeader,
		const char* szText)
{
    struct TEXTDLGDATA info;
    char * szRNText;
    int i, j;

    szRNText = (char*)HeapAlloc(GetProcessHeap(), 0, 2 * lstrlen(szText) + 1);
    i = j =0;
    while (szText[i] != '\0') {
        if(szText[i] == '\n')
            szRNText[j++] = '\r';
        szRNText[j++] = szText[i++];
    }
    szRNText[j] = '\0';

    info.szCaption = (char *)szCaption;
    info.szHeader = (char *)szHeader;
    info.szText = szRNText;

//  if(hWnd == HWND_AUTO)
//      hWnd = GetParentHWND();
    DialogBoxParam(GetModuleHandle(NULL),

// GetModuleHandle(NULL) returns the instance handle
// of the executable that created the current process.
// Win32: module handle == instance handle == task [Win3.1 legacy]

                   MAKEINTRESOURCE(IDD_TEXTDLG),
                   hWnd,
                   TextDlgProc,
                   (LPARAM)&info);
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

    if (!bResult)
        return;

    uiDiff = scInfo.nMax-scInfo.nMin;
    if (scInfo.nPage > uiDiff)
        ShowScrollBar(hWnd, fnBar, 0);
}



BOOL CALLBACK TextDlgProc(HWND hwndDlg,         // handle to dialog box
                          UINT uMsg,            // message
                          WPARAM wParam,        // first message parameter
                          LPARAM lParam)        // second message parameter
{
    switch (uMsg) {
      case WM_INITDIALOG:
        {
            struct TEXTDLGDATA* pInfo = (struct TEXTDLGDATA*) lParam;
            SetWindowText(hwndDlg,pInfo->szCaption);
            SetDlgItemText(hwndDlg, IDC_HEADER, pInfo->szHeader);

            SetDlgItemText(hwndDlg, IDC_TEXT, pInfo->szText);
            SendDlgItemMessage(hwndDlg,
                               IDC_TEXT,
                               EM_SETREADONLY,
                               1,	// wParam: read-only flag
                               0);	// lParam: unused.
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
    ui_show_text(param, "Command line options",
                 "Which command line options are available?", options);
    lib_free(options);
}

void uilib_shutdown(void)
{
    int i;

    for (i = 0; i < UILIB_SELECTOR_STYLES_NUM; i++)
        if (ui_file_selector_initialfile[i] != NULL)
            lib_free(ui_file_selector_initialfile[i]);

    lib_free(fontfile);
}

static uilib_dialogbox_param_t *uilib_dialogbox_param;

static BOOL CALLBACK uilib_dialogbox_dialog_proc(HWND hwnd, UINT msg,
                                                 WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
      case WM_COMMAND:
        switch (LOWORD(wparam)) {
          case IDOK:
            GetDlgItemText(hwnd, uilib_dialogbox_param->idc_dialog,
                           uilib_dialogbox_param->string, UILIB_DIALOGBOX_MAX);
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
        SetDlgItemText(hwnd, uilib_dialogbox_param->idc_dialog,
                       uilib_dialogbox_param->string);
        return TRUE;
    }
    return FALSE;
}


void uilib_dialogbox(uilib_dialogbox_param_t *param)
{
    uilib_dialogbox_param = param;
    uilib_dialogbox_param->updated = 0;
    DialogBox(winmain_instance, (LPCTSTR)(uilib_dialogbox_param->idd_dialog),
              uilib_dialogbox_param->hwnd, uilib_dialogbox_dialog_proc);

}

