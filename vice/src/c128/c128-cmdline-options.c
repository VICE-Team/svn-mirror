/*
 * c128-cmdline-options.c
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

#include "c128-cmdline-options.h"
#include "cmdline.h"
#include "machine.h"
#include "translate.h"

static const cmdline_option_t cmdline_options[] = {
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
      NULL, NULL, "KernalIntName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_INT_KERNEL_NAME,
      NULL, NULL },
    { "-kernalde", SET_RESOURCE, 1,
      NULL, NULL, "KernalDEName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_DE_KERNEL_NAME,
      NULL, NULL },
    { "-kernalfi", SET_RESOURCE, 1,
      NULL, NULL, "KernalFIName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_FI_KERNEL_NAME,
      NULL, NULL },
    { "-kernalfr", SET_RESOURCE, 1,
      NULL, NULL, "KernalFRName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_FR_KERNEL_NAME,
      NULL, NULL },
    { "-kernalit", SET_RESOURCE, 1,
      NULL, NULL, "KernalITName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_IT_KERNEL_NAME,
      NULL, NULL },
    { "-kernalno", SET_RESOURCE, 1,
      NULL, NULL, "KernalNOName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_NO_KERNEL_NAME,
      NULL, NULL },
    { "-kernalse", SET_RESOURCE, 1,
      NULL, NULL, "KernalSEName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_SV_KERNEL_NAME,
      NULL, NULL },
    { "-basiclo", SET_RESOURCE, 1,
      NULL, NULL, "BasicLoName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_BASIC_ROM_NAME_LOW,
      NULL, NULL },
    { "-basichi", SET_RESOURCE, 1,
      NULL, NULL, "BasicHiName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH,
      NULL, NULL },
    { "-chargen", SET_RESOURCE, 1,
      NULL, NULL, "ChargenIntName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME,
      NULL, NULL },
    { "-chargde", SET_RESOURCE, 1,
      NULL, NULL, "ChargenDEName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME,
      NULL, NULL },
    { "-chargfr", SET_RESOURCE, 1,
      NULL, NULL, "ChargenFRName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME,
      NULL, NULL },
    { "-chargse", SET_RESOURCE, 1,
      NULL, NULL, "ChargenSEName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME,
      NULL, NULL },
    { "-kernal64", SET_RESOURCE, 1,
      NULL, NULL, "Kernal64Name", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_C64_MODE_KERNAL_NAME,
      NULL, NULL },
    { "-basic64", SET_RESOURCE, 1,
      NULL, NULL, "Basic64Name", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_C64_MODE_BASIC_NAME,
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
    { "-ieee488", SET_RESOURCE, 0,
      NULL, NULL, "IEEE488", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_IEE488,
      NULL, NULL },
    { "+ieee488", SET_RESOURCE, 0,
      NULL, NULL, "IEEE488", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_IEE488,
      NULL, NULL },
    { "-kernalrev", SET_RESOURCE, 1,
      NULL, NULL, "KernalRev", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_REVISION, IDCLS_PATCH_KERNAL_TO_REVISION,
      NULL, NULL },
#ifdef HAVE_RS232
    { "-acia1", SET_RESOURCE, 0,
      NULL, NULL, "Acia1Enable", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_DEXX_ACIA_RS232_EMU,
      NULL, NULL },
    { "+acia1", SET_RESOURCE, 0,
      NULL, NULL, "Acia1Enable", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_DEXX_ACIA_RS232_EMU,
      NULL, NULL },
#endif
#ifdef COMMON_KBD
    { "-keymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapIndex", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NUMBER, IDCLS_SPECIFY_KEYMAP_FILE_INDEX,
      NULL, NULL },
    { "-symkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapSymFile", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME,
      NULL, NULL },
    { "-poskeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapPosFile", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME,
      NULL, NULL },
#endif
    { NULL }
};

int c128_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}
