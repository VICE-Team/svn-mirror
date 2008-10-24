/*
 * c64dtv-cmdline-options.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "c64dtv-cmdline-options.h"
#include "cmdline.h"
#include "machine.h"
#include "translate.h"

static const cmdline_option_t cmdline_options[] =
{
    { "-pal", SET_RESOURCE, 0,
      NULL, NULL, "MachineVideoStandard", (void *)MACHINE_SYNC_PAL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_USE_PAL_SYNC_FACTOR,
      NULL, NULL },
    { "-ntsc", SET_RESOURCE, 0,
      NULL, NULL, "MachineVideoStandard", (void *)MACHINE_SYNC_NTSC,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_USE_NTSC_SYNC_FACTOR,
      NULL, NULL },
    { "-kernal", SET_RESOURCE, 1,
      NULL, NULL, "KernalName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_KERNAL_ROM_NAME,
      NULL, NULL },
    { "-basic", SET_RESOURCE, 1,
      NULL, NULL, "BasicName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_BASIC_ROM_NAME,
      NULL, NULL },
    { "-chargen", SET_RESOURCE, 1,
      NULL, NULL, "ChargenName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_CHARGEN_ROM_NAME,
      NULL, NULL },
    { "-emuid", SET_RESOURCE, 0,
      NULL, NULL, "EmuID", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_EMULATOR_ID,
      NULL, NULL },
    { "+emuid", SET_RESOURCE, 0,
      NULL, NULL, "EmuID", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_EMULATOR_ID,
      NULL, NULL },
    { "-kernalrev", SET_RESOURCE, 1,
      NULL, NULL, "KernalRev", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_REVISION, IDCLS_PATCH_KERNAL_TO_REVISION,
      NULL, NULL },
#ifdef COMMON_KBD
    { "-keymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapIndex", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NUMBER, IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2,
      NULL, NULL },
    { "-symkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapSymFile", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME,
      NULL, NULL },
    { "-symdekeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapSymDeFile", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP,
      NULL, NULL },
    { "-poskeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapPosFile", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME,
      NULL, NULL },
#endif
    { "-hummerusrjoy", SET_RESOURCE, 0,
      NULL, NULL, "HummerUserportJoy", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_HUMMER_USERPORT_JOY,
      NULL, NULL },
    { "+hummerusrjoy", SET_RESOURCE, 0,
      NULL, NULL, "HummerUserportJoy", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_HUMMER_USERPORT_JOY,
      NULL, NULL },
    { "-hummerusrjoyport", SET_RESOURCE, 1,
      NULL, NULL, "HummerUserportJoyPort", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_PORT, IDCLS_HUMMER_JOY_PORT,
      NULL, NULL },
    { NULL }
};

int c64dtv_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}
