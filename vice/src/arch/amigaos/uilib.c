/*
 * uilib.c
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __VBCC__
#define __USE_INLINE__

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#endif

#include "private.h"
#include "uilib.h"
#include "lib.h"
#include "imagecontents.h"

#include "mui/filereq.h"

#if 0 /* doesn't seem to be used anywhere (yet?) */

struct uilib_filefilter_s {
    const char *name;
    const char *pattern;
};
typedef struct uilib_filefilter_s uilib_filefilter_t;

static uilib_filefilter_t uilib_filefilter[] = {
    { "All files (*.*)", "*.*" },
    { "VICE palette files (*.vpl)", "*.vpl" },
    { "VICE snapshot files (*.vsf)", "*.vsf" },
    { "Disk image files (*.d64;*.d71;*.d80;*.d81;*.d82;*.g64;*.g41;*.x64)",
      "*.d64;*.d71;*.d80;*.d81;*.d82;*.g64;*.g41;*.x64" },
    { "Tape image files (*.t64;*.p00;*.tap;*.prg)",
      "*.t64;*.p00;*.tap;*.prg" },
    { "Zipped files (*.zip;*.bz2;*.gz;*.d6z;*.d7z;*.d8z;*.g6z;*.g4z;*.x6z)",
      "*.zip;*.bz2;*.gz;*.d6z;*.d7z;*.d8z;*.g6z;*.g4z;*.x6z" },
    { "CRT cartridge image files (*.crt)", "*.crt" },
    { "Raw cartridge image files (*.bin)", "*.bin" },
    { "VICE flip list files (*.vfl)", "*.vfl" },
    { "VICE romset files (*.vrs)", "*.vrs" },
    { "VICE romset archives (*.vra)", "*.vra" },
    { "VICE keymap files (*.vkm)", "*.vkm" },
    { NULL, NULL }
};

#endif

struct uilib_fs_style_type_s {
    char *(*content_read_function)(const char *);
    void *hook_proc;
    int TemplateID;
    char *initialdir_resource;
    char *file_resource;
};
typedef struct uilib_fs_style_type_s uilib_fs_style_type_t;

static char *read_disk_image_contents(const char *name);
static char *read_tape_image_contents(const char *name);
static char *read_disk_or_tape_image_contents(const char *name);

static void uilib_select_tape_hook_proc(void){} /* FIXME */
static void uilib_select_hook_proc(void){} /* FIXME */

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

static char *ui_file_selector_initialfile[UILIB_SELECTOR_STYLES_NUM];

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

char *uilib_select_file_autostart(const char *title,
                                   unsigned int filterlist, unsigned int type,
                                   int style, int *autostart,
                                   char *resource_readonly)
{
    char *name = NULL;
    char *initialdir = NULL;
    char *initialfile = NULL;
//    char *filter = NULL;
//    unsigned int filterindex;
//    OPENFILENAME ofn;
//    int result;
    char *ret = NULL;

    if (styles[style].initialdir_resource != NULL)
        resources_get_value(styles[style].initialdir_resource,
            (void *)&initialdir);

    initialfile = ui_file_selector_initialfile[style];
    if (styles[style].file_resource != NULL)
        resources_get_value(styles[style].file_resource,
            (void *)&initialfile);

#if 0
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
    res_readonly = resource_readonly;
    vsync_suspend_speed_eval();

    if (type == UILIB_SELECTOR_TYPE_FILE_SAVE)
        result = GetSaveFileName(&ofn);
    else
        result = GetOpenFileName(&ofn);

    update_filter_history(ofn.nFilterIndex);
#endif

    name = ui_filereq(title, styles[style].TemplateID, initialdir, initialfile, styles[style].content_read_function, autostart, resource_readonly);

    if (name != NULL) {
        char newdir[1024], *ptr = PathPart(name);
        memcpy(newdir, name, (int)(ptr - name));
        newdir[(int)(ptr - name)] = '\0';

        if (ui_file_selector_initialfile[style] != NULL)
            lib_free(ui_file_selector_initialfile[style]);
        if (styles[style].file_resource != NULL)
            resources_set_value(styles[style].file_resource, FilePart(name));
        ui_file_selector_initialfile[style] = lib_stralloc(FilePart(name));

        resources_set_value(styles[style].initialdir_resource, newdir);
        ret = lib_stralloc(name);
    }

//    lib_free(filter);

    return ret;
}

char *uilib_select_file(const char *title, unsigned int filterlist,
                         unsigned int type, int style)
{
    return uilib_select_file_autostart(title, filterlist, type, style,
                                       NULL, NULL);
}
