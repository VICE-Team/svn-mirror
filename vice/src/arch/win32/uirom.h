/*
 * uirom.h - Implementation of the ROM settings dialog box.
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

#ifndef _UIROM_H
#define _UIROM_H

#include <tchar.h>

struct uirom_settings_s {
    const TCHAR *realname;
    const char *resname;
    unsigned int idc_filename;
    unsigned int idc_browse;
};
typedef struct uirom_settings_s uirom_settings_t;

extern void uirom_settings_dialog(HWND hwnd, unsigned int idd_dialog,
                                  const uirom_settings_t *uirom_settings);

#endif

