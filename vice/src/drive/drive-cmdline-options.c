/*
 * drive-cmdline-options.c - Hardware-level disk drive emulation,
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
#include "drive-cmdline-options.h"
#include "drive.h"
#include "lib.h"
#include "machine-drive.h"

#ifdef HAS_TRANSLATION
#include "translate.h"

static const cmdline_option_trans_t cmdline_options[] = {
    { "-truedrive", SET_RESOURCE, 0, NULL, NULL, "DriveTrueEmulation",
      (void *)1, 0, IDCLS_ENABLE_TRUE_DRIVE },
    { "+truedrive", SET_RESOURCE, 0, NULL, NULL, "DriveTrueEmulation",
      (void *)0, 0, IDCLS_DISABLE_TRUE_DRIVE },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] = {
    { "-truedrive", SET_RESOURCE, 0, NULL, NULL, "DriveTrueEmulation",
      (void *)1, NULL, "Enable hardware-level emulation of disk drives" },
    { "+truedrive", SET_RESOURCE, 0, NULL, NULL, "DriveTrueEmulation",
      (void *)0, NULL, "Disable hardware-level emulation of disk drives" },
    { NULL }
};
#endif

#ifdef HAS_TRANSLATION
static cmdline_option_trans_t cmd_drive[] = {
    { NULL, SET_RESOURCE, 1, NULL, NULL, NULL, NULL, IDCLS_P_TYPE,
      IDCLS_SET_DRIVE_TYPE },
    { NULL, SET_RESOURCE, 1, NULL, NULL, NULL, NULL, IDCLS_P_METHOD,
      IDCLS_SET_DRIVE_EXTENSION_POLICY},
    { NULL }
};
#else
static cmdline_option_t cmd_drive[] = {
    { NULL, SET_RESOURCE, 1, NULL, NULL, NULL, NULL, "<type>",
      "Set drive type (0: no drive)" },
    { NULL, SET_RESOURCE, 1, NULL, NULL, NULL, NULL, "<method>",
      "Set drive 40 track extension policy (0: never, 1: ask, 2: on access)"},
    { NULL }
};
#endif

int drive_cmdline_options_init(void)
{
    unsigned int dnr, i;

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        cmd_drive[0].name = lib_msprintf("-drive%itype", dnr + 8);
        cmd_drive[0].resource_name
            = lib_msprintf("Drive%iType", dnr + 8);
        cmd_drive[1].name = lib_msprintf("-drive%iextend", dnr + 8);
        cmd_drive[1].resource_name
            = lib_msprintf("Drive%iExtendImagePolicy", dnr + 8);

#ifdef HAS_TRANSLATION
        if (cmdline_register_options_trans(cmd_drive) < 0)
#else
        if (cmdline_register_options(cmd_drive) < 0)
#endif
            return -1;

        for (i = 0; i < 2; i++) {
            lib_free((char *)cmd_drive[i].name);
            lib_free((char *)cmd_drive[i].resource_name);
        }
    }

    return machine_drive_cmdline_options_init()
#ifdef HAS_TRANSLATION
        | cmdline_register_options_trans(cmdline_options);
#else
        | cmdline_register_options(cmdline_options);
#endif
}
