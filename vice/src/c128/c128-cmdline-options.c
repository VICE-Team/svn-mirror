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

#ifdef HAS_TRANSLATION
#include "translate.h"

static const cmdline_option_t cmdline_options[] = {
    { "-pal", SET_RESOURCE, 0, NULL, NULL, "MachineVideoStandard",
      (void *)MACHINE_SYNC_PAL, 0, IDCLS_USE_PAL_SYNC_FACTOR },
    { "-ntsc", SET_RESOURCE, 0, NULL, NULL, "MachineVideoStandard",
      (void *)MACHINE_SYNC_NTSC, 0, IDCLS_USE_NTSC_SYNC_FACTOR },
    { "-kernal", SET_RESOURCE, 1, NULL, NULL, "KernalIntName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_INT_KERNEL_NAME },
    { "-kernalde", SET_RESOURCE, 1, NULL, NULL, "KernalDEName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_DE_KERNEL_NAME },
    { "-kernalfi", SET_RESOURCE, 1, NULL, NULL, "KernalFIName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_FI_KERNEL_NAME },
    { "-kernalfr", SET_RESOURCE, 1, NULL, NULL, "KernalFRName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_FR_KERNEL_NAME },
    { "-kernalit", SET_RESOURCE, 1, NULL, NULL, "KernalITName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_IT_KERNEL_NAME },
    { "-kernalno", SET_RESOURCE, 1, NULL, NULL, "KernalNOName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_NO_KERNEL_NAME },
    { "-kernalse", SET_RESOURCE, 1, NULL, NULL, "KernalSEName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_SV_KERNEL_NAME },
    { "-basiclo", SET_RESOURCE, 1, NULL, NULL, "BasicLoName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_BASIC_ROM_NAME_LOW },
    { "-basichi", SET_RESOURCE, 1, NULL, NULL, "BasicHiName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_BASIC_ROM_NAME_HIGH },
    { "-chargen", SET_RESOURCE, 1, NULL, NULL, "ChargenIntName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_INT_CHARGEN_ROM_NAME },
    { "-chargde", SET_RESOURCE, 1, NULL, NULL, "ChargenDEName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_DE_CHARGEN_ROM_NAME },
    { "-chargfr", SET_RESOURCE, 1, NULL, NULL, "ChargenFRName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_FR_CHARGEN_ROM_NAME },
    { "-chargse", SET_RESOURCE, 1, NULL, NULL, "ChargenSEName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_SV_CHARGEN_ROM_NAME },
    { "-kernal64", SET_RESOURCE, 1, NULL, NULL, "Kernal64Name", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_C64_MODE_KERNAL_NAME },
    { "-basic64", SET_RESOURCE, 1, NULL, NULL, "Basic64Name", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_C64_MODE_BASIC_NAME },
    { "-emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (void *)1,
      0, IDCLS_ENABLE_EMULATOR_ID },
    { "+emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (void *)0,
      0, IDCLS_DISABLE_EMULATOR_ID },
    { "-ieee488", SET_RESOURCE, 0, NULL, NULL, "IEEE488", (void *)1,
      0, IDCLS_ENABLE_IEE488 },
    { "+ieee488", SET_RESOURCE, 0, NULL, NULL, "IEEE488", (void *)0,
      0, IDCLS_DISABLE_IEE488 },
    { "-kernalrev", SET_RESOURCE, 1, NULL, NULL, "KernalRev", NULL,
      IDCLS_P_REVISION, IDCLS_PATCH_KERNAL_TO_REVISION },
#ifdef HAVE_RS232
    { "-acia1", SET_RESOURCE, 0, NULL, NULL, "Acia1Enable", (void *)1,
      0, IDCLS_ENABLE_DEXX_ACIA_RS232_EMU },
    { "+acia1", SET_RESOURCE, 0, NULL, NULL, "Acia1Enable", (void *)0,
      0, IDCLS_DISABLE_DEXX_ACIA_RS232_EMU },
#endif
#ifdef COMMON_KBD
    { "-keymap", SET_RESOURCE, 1, NULL, NULL, "KeymapIndex", NULL,
      IDCLS_P_NUMBER, IDCLS_SPECIFY_KEYMAP_FILE_INDEX },
    { "-symkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapSymFile", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME },
    { "-poskeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapPosFile", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME },
#endif
    { NULL }
};

#else

static const cmdline_option_t cmdline_options[] = {
    { "-pal", SET_RESOURCE, 0, NULL, NULL, "MachineVideoStandard",
      (void *)MACHINE_SYNC_PAL, NULL, "Use PAL sync factor" },
    { "-ntsc", SET_RESOURCE, 0, NULL, NULL, "MachineVideoStandard",
      (void *)MACHINE_SYNC_NTSC, NULL, "Use NTSC sync factor" },
    { "-kernal", SET_RESOURCE, 1, NULL, NULL, "KernalIntName", NULL,
      "<name>", "Specify name of international Kernal ROM image" },
    { "-kernalde", SET_RESOURCE, 1, NULL, NULL, "KernalDEName", NULL,
      "<name>", "Specify name of German Kernal ROM image" },
    { "-kernalfi", SET_RESOURCE, 1, NULL, NULL, "KernalFIName", NULL,
      "<name>", "Specify name of Finnish Kernal ROM image" },
    { "-kernalfr", SET_RESOURCE, 1, NULL, NULL, "KernalFRName", NULL,
      "<name>", "Specify name of French Kernal ROM image" },
    { "-kernalit", SET_RESOURCE, 1, NULL, NULL, "KernalITName", NULL,
      "<name>", "Specify name of Italian Kernal ROM image" },
    { "-kernalno", SET_RESOURCE, 1, NULL, NULL, "KernalNOName", NULL,
      "<name>", "Specify name of Norwegain Kernal ROM image" },
    { "-kernalse", SET_RESOURCE, 1, NULL, NULL, "KernalSEName", NULL,
      "<name>", "Specify name of Swedish Kernal ROM image" },
    { "-basiclo", SET_RESOURCE, 1, NULL, NULL, "BasicLoName", NULL,
      "<name>", "Specify name of BASIC ROM image (lower part)" },
    { "-basichi", SET_RESOURCE, 1, NULL, NULL, "BasicHiName", NULL,
      "<name>", "Specify name of BASIC ROM image (higher part)" },
    { "-chargen", SET_RESOURCE, 1, NULL, NULL, "ChargenIntName", NULL,
      "<name>", "Specify name of international character generator ROM image" },
    { "-chargde", SET_RESOURCE, 1, NULL, NULL, "ChargenDEName", NULL,
      "<name>", "Specify name of German character generator ROM image" },
    { "-chargfr", SET_RESOURCE, 1, NULL, NULL, "ChargenFRName", NULL,
      "<name>", "Specify name of French character generator ROM image" },
    { "-chargse", SET_RESOURCE, 1, NULL, NULL, "ChargenSEName", NULL,
      "<name>", "Specify name of Swedish character generator ROM image" },
    { "-kernal64", SET_RESOURCE, 1, NULL, NULL, "Kernal64Name", NULL,
      "<name>", "Specify name of C64 mode Kernal ROM image" },
    { "-basic64", SET_RESOURCE, 1, NULL, NULL, "Basic64Name", NULL,
      "<name>", "Specify name of C64 mode BASIC ROM image" },
    { "-emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (void *)1,
      NULL, "Enable emulator identification" },
    { "+emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (void *)0,
      NULL, "Disable emulator identification" },
    { "-ieee488", SET_RESOURCE, 0, NULL, NULL, "IEEE488", (void *)1,
      NULL, "Enable the IEEE488 interface emulation" },
    { "+ieee488", SET_RESOURCE, 0, NULL, NULL, "IEEE488", (void *)0,
      NULL, "Disable the IEEE488 interface emulation" },
    { "-kernalrev", SET_RESOURCE, 1, NULL, NULL, "KernalRev", NULL,
      "<revision>", "Patch the Kernal ROM to the specified <revision>" },
#ifdef HAVE_RS232
    { "-acia1", SET_RESOURCE, 0, NULL, NULL, "Acia1Enable", (void *)1,
      NULL, "Enable the $DE** ACIA RS232 interface emulation" },
    { "+acia1", SET_RESOURCE, 0, NULL, NULL, "Acia1Enable", (void *)0,
      NULL, "Disable the $DE** ACIA RS232 interface emulation" },
#endif
#ifdef COMMON_KBD
    { "-keymap", SET_RESOURCE, 1, NULL, NULL, "KeymapIndex", NULL,
      "<number>", N_("Specify index of keymap file (0=symbol, 1=positional)") },
    { "-symkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapSymFile", NULL,
      "<name>", N_("Specify name of symbolic keymap file") },
    { "-poskeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapPosFile", NULL,
      "<name>", N_("Specify name of positional keymap file") },
#endif
    { NULL }
};

#endif

int c128_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

