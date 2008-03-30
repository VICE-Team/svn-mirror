/*
 * drive-cmdline-options.c - Hardware-level Commodore disk drive emulation,
 *                           command line options module.
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

#include "cmdline.h"
#include "drive.h"
#include "machine-drive.h"


static const cmdline_option_t cmdline_options[] = {
    { "-truedrive", SET_RESOURCE, 0, NULL, NULL, "DriveTrueEmulation",
      (void *)1,
      NULL, "Enable hardware-level emulation of disk drives" },
    { "+truedrive", SET_RESOURCE, 0, NULL, NULL, "DriveTrueEmulation",
      (void *)0,
      NULL, "Disable hardware-level emulation of disk drives" },
    { "-drive8type", SET_RESOURCE, 1, NULL, NULL, "Drive8Type",
      (void *)DRIVE_TYPE_1541, "<type>",
      "Set drive type (0: no drive)" },
    { "-drive9type", SET_RESOURCE, 1, NULL, NULL, "Drive9Type",
      (void *)DRIVE_TYPE_NONE, "<type>",
      "Set drive type (0: no drive)" },
    { "-drive8extend", SET_RESOURCE, 1, NULL, NULL, "Drive8ExtendImagePolicy",
      (void *)DRIVE_EXTEND_NEVER, "<method>",
      "Set drive 8 40 track extension policy (0: never, 1: ask, 2: on access)"},
    { "-drive9extend", SET_RESOURCE, 1, NULL, NULL, "Drive9ExtendImagePolicy",
      (void *)DRIVE_EXTEND_NEVER, "<method>",
      "Set drive 9 40 track extension policy (0: never, 1: ask, 2: on access)"},
    { NULL }
};

int drive_cmdline_options_init(void)
{
    return machine_drive_cmdline_options_init()
        | cmdline_register_options(cmdline_options);
}

