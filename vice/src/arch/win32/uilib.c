/*
 * uilib.c - Common UI elements for the Windows user interface.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include <string.h>
#include <windows.h>
#include <windowsx.h>

#include "imagecontents.h"
#include "resources.h"
#include "ui.h"
#include "uilib.h"
#include "utils.h"
#include "winmain.h"
#include "res.h"
#include "diskimage.h"

static char *(*read_content_func)(const char *);


char *read_disk_image_contents(const char *name)
{
image_contents_t    *contents;
char                *s;

    contents=image_contents_read_disk(name);
    if (contents==NULL) {
        return NULL;
    }
    s=image_contents_to_string(contents);
    image_contents_destroy(contents);
    return s;
}

char *read_tape_image_contents(const char *name)
{
image_contents_t    *contents;
char                *s;

    contents=image_contents_read_tape(name);
    if (contents==NULL) {
        return NULL;
    }
    s=image_contents_to_string(contents);
    image_contents_destroy(contents);
    return s;
}

char *read_disk_or_tape_image_contents(const char *name)
{
char    *tmp;

    tmp=read_disk_image_contents(name);
    if (tmp==NULL) {
        tmp=read_tape_image_contents(name);
    }
    return tmp;
}

static void create_content_list(char *text, HWND list)
{
char    *start;
char    buffer[256];
int     index;

    if (text==NULL) return;
    start=text;
    index=0;
    while (1) {
        if (*start==0x0a) {
            buffer[index]=0;
            index=0;
            SendMessage(list,LB_ADDSTRING,0,(LPARAM)buffer);
        } else if (*start==0x0d) {
        } else if (*start==0) {
            break;
        } else {
            buffer[index++]=*start;
        }
        start++;
    }

}


static UINT APIENTRY hook_proc(HWND hwnd, UINT uimsg, WPARAM wparam, LPARAM lparam)
{
    
    HWND    preview;
    HWND    image_type_list;
    char    *contents;
    char    filename[256];
    char    *image_type_name[] = 
        { "D64","D71","D80","D81","D82","G64","X64",NULL };
    int     image_type[] = {
        DISK_IMAGE_TYPE_D64,
        DISK_IMAGE_TYPE_D71,
        DISK_IMAGE_TYPE_D80,
        DISK_IMAGE_TYPE_D81,
        DISK_IMAGE_TYPE_D82,
        DISK_IMAGE_TYPE_GCR,
        DISK_IMAGE_TYPE_X64
    };
    int counter;
    int msg_type;
    int append_extension = 0;

    preview=GetDlgItem(hwnd,IDC_PREVIEW);
    switch (uimsg) {
        case WM_INITDIALOG:
            SetWindowText(GetDlgItem(GetParent(hwnd),IDOK),"&Attach");
            image_type_list=GetDlgItem(hwnd,IDC_BLANK_IMAGE_TYPE);
            for (counter = 0; image_type_name[counter]; counter++) {
                SendMessage(image_type_list,CB_ADDSTRING,0,
                    (LPARAM)image_type_name[counter]);
            }
            SendMessage(image_type_list,CB_SETCURSEL,(WPARAM)0,0);
            break;
        case WM_NOTIFY:
            if (((OFNOTIFY*)lparam)->hdr.code==CDN_SELCHANGE) {
                SendMessage(preview,LB_RESETCONTENT,0,0);
                if (SendMessage(((OFNOTIFY*)lparam)->hdr.hwndFrom,CDM_GETFILEPATH,256,(LPARAM)filename)>=0) {
                    if (read_content_func!=NULL) {
                        contents=read_content_func(filename);
                        create_content_list(contents,preview);
                    }
                }
            }
            break;
        case WM_COMMAND:
            msg_type = LOWORD(wparam);
            switch (msg_type) {
                case IDC_BLANK_IMAGE:
                    if (SendMessage(GetParent(hwnd),
                        CDM_GETSPEC,256,(LPARAM)filename)<=1) {
                        ui_error("Please enter a filename.");
                        return -1;
                    }
                    if (strchr(filename,'.') == NULL)
                        append_extension = 1;
                    if (SendMessage(GetParent(hwnd),
                        CDM_GETFILEPATH,256,(LPARAM)filename)>=0)
                    {
                        counter = 
                            SendMessage(GetDlgItem(hwnd,IDC_BLANK_IMAGE_TYPE),
                                CB_GETCURSEL,0,0);
                        if (append_extension) {
                            strcat(filename,".");
                            strcat(filename,image_type_name[counter]);
                        }
                        if (fopen(filename,"r")) {
                            int ret;
                            ret = MessageBox(hwnd, "Overwrite existing image?",
                                "VICE question", MB_YESNO | MB_ICONQUESTION);
                            if (ret != IDYES)
                                return -1;
                        }
                        if (disk_image_create(filename,image_type[counter])<0)
                        {
                            ui_error("Cannot create image");
                            return -1;
                        }
                        /* FIXME: Try to redraw Filelist and Filename */
                    }
                    break;
            }
            break;
    }
    return 0;
}


char *ui_select_file(const char *title, const char *filter, char*(*content_read_function)(const char *), HWND hwnd)
{
    char name[1024] = "";
    OPENFILENAME ofn;

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = winmain_instance;
    ofn.lpstrFilter = filter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = name;
    ofn.nMaxFile = sizeof(name);
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    if (content_read_function!=NULL) {
        ofn.Flags = (OFN_EXPLORER
                     | OFN_HIDEREADONLY
                     | OFN_NOTESTFILECREATE
                     | OFN_FILEMUSTEXIST
                     | OFN_ENABLEHOOK
                     | OFN_ENABLETEMPLATE
                     | OFN_SHAREAWARE);
        ofn.lpfnHook = hook_proc;
        ofn.lpTemplateName = MAKEINTRESOURCE(IDD_OPEN_TEMPLATE);
    } else {
        ofn.Flags = (OFN_EXPLORER
                     | OFN_HIDEREADONLY
                     | OFN_NOTESTFILECREATE
                     | OFN_FILEMUSTEXIST
                     | OFN_SHAREAWARE);
        ofn.lpfnHook = NULL;
        ofn.lpTemplateName = NULL;
    }
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;

    read_content_func=content_read_function;
    if (GetOpenFileName(&ofn)) {
        return stralloc(name);
    } else {
        return NULL;
    }
}

void ui_set_res_num(char *res, int value, int num)
{
    char tmp[256];

    sprintf(tmp, res, num);
    resources_set_value(tmp, (resource_value_t *) value);
}
