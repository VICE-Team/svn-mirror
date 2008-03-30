/*
 * uilib.h - Common UI elements for the Windows user interface.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
 *  Manfred Spraul <manfreds@colorfullife.com>
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

#ifndef _UILIB_H
#define _UILIB_H

#include <windows.h>

/*  Parameter structure for ui_attach_cartridge */
typedef struct {
    WPARAM wparam;
    int type;
    char *title;
    DWORD filter;
} ui_cartridge_params;

char *ui_select_file(HWND hwnd, const char *title, DWORD filterlist, int style, int *autostart);
void ui_set_res_num(char *res, int value, int num);

/*
char *read_disk_image_contents(const char *name);
char *read_tape_image_contents(const char *name);
char *read_disk_or_tape_image_contents(const char *name);
*/

extern void ui_show_text(HWND hParent, const char *szCaption,
                         const char *szHeader, const char *szText);

#define FILE_SELECTOR_DEFAULT_STYLE         0
#define FILE_SELECTOR_TAPE_STYLE            1
#define FILE_SELECTOR_DISK_STYLE            2
#define FILE_SELECTOR_DISK_AND_TAPE_STYLE   3
#define FILE_SELECTOR_CART_STYLE            4
#define FILE_SELECTOR_SNAPSHOT_STYLE        5
#define NUM_OF_FILE_SELECTOR_STYLES         6


#define UI_LIB_FILTER_ALL                   1
#define UI_LIB_FILTER_PALETTE               2
#define UI_LIB_FILTER_SNAPSHOT              4
#define UI_LIB_FILTER_DISK                  8
#define UI_LIB_FILTER_TAPE                  16
#define UI_LIB_FILTER_ZIP                   32
#define UI_LIB_FILTER_CRT                   64
#define UI_LIB_FILTER_BIN                   128

#define UI_LIB_MAX_FILTER_LENGTH        1024


extern int ui_messagebox(LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

#endif

