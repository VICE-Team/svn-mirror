/*
 * uilib.c - Common UI elements for the Windows user interface.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include <windows.h>
#include <windowsx.h>

#include "resources.h"
#include "ui.h"
#include "uilib.h"
#include "utils.h"
#include "winmain.h"

char *ui_select_file(const char *title, const char *filter, HWND hwnd)
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
    ofn.Flags = (OFN_EXPLORER
                 | OFN_HIDEREADONLY
                 | OFN_NOTESTFILECREATE
                 | OFN_FILEMUSTEXIST
                 /* | OFN_NOCHANGEDIR */
                 | OFN_SHAREAWARE);
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;
    if (GetOpenFileName(&ofn))
        return stralloc(name);
    else
        return NULL;
}

void ui_set_res_num(char *res, int value, int num)
{
    char tmp[256];

    sprintf(tmp, res, num);
    resources_set_value(tmp, (resource_value_t *) value);
}

