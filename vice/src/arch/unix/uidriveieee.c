/*
 * uidriveieee.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#include "uidrive.h"
#include "uidriveieee.h"
#include "uimenu.h"


UI_MENU_DEFINE_TOGGLE(DriveTrueEmulation)


/* This menu is for the PET/CBM2 */
static ui_menu_entry_t par_drive_settings_submenu[] = {
    { N_("*Enable true drive emulation"),
      (ui_callback_t)toggle_DriveTrueEmulation, NULL, NULL },
    { "--" },
    { N_("Drive #8 floppy disk type"),
      NULL, NULL, set_drive0_type_submenu },
#if 0
    { N_("Drive #8 40-track image support"),
      NULL, NULL, set_drive0_extend_image_policy_submenu },
    { N_("Drive #8 idle method"),
      NULL, NULL, set_drive0_idle_method_submenu },
#endif
    { "--" },
    { N_("Drive #9 floppy disk type"),
      NULL, NULL, set_drive1_type_submenu },
#if 0
    { N_("Drive #9 40-track image support"),
      NULL, NULL, set_drive1_extend_image_policy_submenu },
    { N_("Drive #9 idle method"),
      NULL, NULL, set_drive1_idle_method_submenu },
#endif
    { NULL }
};


/* PET/CBM2 */
ui_menu_entry_t ui_par_drive_settings_menu[] = {
    { N_("Drive settings"),
      NULL, NULL, par_drive_settings_submenu },
    { NULL }
};

