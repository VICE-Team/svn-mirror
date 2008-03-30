/*
 * petui.c - PET-specific user interface.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Ettore Perazzoli (ettore@comm2000.it)
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
#include <windowsx.h>

#include "petui.h"
#include "res.h"
#include "ui.h"
#include "uipetset.h"
#include "winmain.h"

void pet_ui_specific(WPARAM wparam, HWND hwnd)
{
    switch (wparam) {
      case IDM_PET_SETTINGS:
        ui_pet_settings_dialog(hwnd);
    }
}

int pet_ui_init(void)
{
    ui_register_machine_specific(pet_ui_specific);
    return 0;
}

