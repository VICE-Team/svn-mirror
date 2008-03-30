/*
 * c64-cmdline-options.c
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

#include "c64-cmdline-options.h"
#include "cmdline.h"
#include "machine.h"

#ifdef HAS_TRANSLATION
#include "translate.h"
#endif

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] =
{
    { "-pal", SET_RESOURCE, 0, NULL, NULL, "MachineVideoStandard",
      (void *)MACHINE_SYNC_PAL, 0, IDCLS_USE_PAL_SYNC_FACTOR },
    { "-ntsc", SET_RESOURCE, 0, NULL, NULL, "MachineVideoStandard",
      (void *)MACHINE_SYNC_NTSC, 0, IDCLS_USE_NTSC_SYNC_FACTOR },
    { "-ntscold", SET_RESOURCE, 0, NULL, NULL, "MachineVideoStandard",
      (void *)MACHINE_SYNC_NTSCOLD,
      0, IDCLS_USE_OLD_NTSC_SYNC_FACTOR },
    { "-kernal", SET_RESOURCE, 1, NULL, NULL, "KernalName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_KERNAL_ROM_NAME },
    { "-basic", SET_RESOURCE, 1, NULL, NULL, "BasicName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_BASIC_ROM_NAME },
    { "-chargen", SET_RESOURCE, 1, NULL, NULL, "ChargenName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_CHARGEN_ROM_NAME },
    { "-emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (void *)1,
      0, IDCLS_ENABLE_EMULATOR_ID },
    { "+emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (void *)0,
      0, IDCLS_DISABLE_EMULATOR_ID },
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
      IDCLS_P_NUMBER, IDCLS_SPECIFY_INDEX_KEYMAP_FILE_0_2 },
    { "-symkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapSymFile", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_SYM_KEYMAP_FILE_NAME },
    { "-symdekeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapSymDeFile", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_NAME_SYM_DE_KEYMAP },
    { "-poskeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapPosFile", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_POS_KEYMAP_FILE_NAME },
#endif
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] =
{
    { "-pal", SET_RESOURCE, 0, NULL, NULL, "MachineVideoStandard",
      (void *)MACHINE_SYNC_PAL, NULL, N_("Use PAL sync factor") },
    { "-ntsc", SET_RESOURCE, 0, NULL, NULL, "MachineVideoStandard",
      (void *)MACHINE_SYNC_NTSC, NULL, N_("Use NTSC sync factor") },
    { "-ntscold", SET_RESOURCE, 0, NULL, NULL, "MachineVideoStandard",
      (void *)MACHINE_SYNC_NTSCOLD,
      NULL, N_("Use old NTSC sync factor") },
    { "-kernal", SET_RESOURCE, 1, NULL, NULL, "KernalName", NULL,
      N_("<name>"), N_("Specify name of Kernal ROM image") },
    { "-basic", SET_RESOURCE, 1, NULL, NULL, "BasicName", NULL,
      N_("<name>"), N_("Specify name of BASIC ROM image") },
    { "-chargen", SET_RESOURCE, 1, NULL, NULL, "ChargenName", NULL,
      N_("<name>"), N_("Specify name of character generator ROM image") },
    { "-emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (void *)1,
      NULL, N_("Enable emulator identification") },
    { "+emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (void *)0,
      NULL, N_("Disable emulator identification") },
    { "-kernalrev", SET_RESOURCE, 1, NULL, NULL, "KernalRev", NULL,
      N_("<revision>"), N_("Patch the Kernal ROM to the specified <revision>") },
#ifdef HAVE_RS232
    { "-acia1", SET_RESOURCE, 0, NULL, NULL, "Acia1Enable", (void *)1,
      NULL, N_("Enable the $DE** ACIA RS232 interface emulation") },
    { "+acia1", SET_RESOURCE, 0, NULL, NULL, "Acia1Enable", (void *)0,
      NULL, N_("Disable the $DE** ACIA RS232 interface emulation") },
#endif
#ifdef COMMON_KBD
    { "-keymap", SET_RESOURCE, 1, NULL, NULL, "KeymapIndex", NULL,
      N_("<number>"), N_("Specify index of keymap file (0=sym, 1=symDE, 2=pos)") },
    { "-symkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapSymFile", NULL,
      N_("<name>"), N_("Specify name of symbolic keymap file") },
    { "-symdekeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapSymDeFile", NULL,
      N_("<name>"), N_("Specify name of symbolic German keymap file") },
    { "-poskeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapPosFile", NULL,
      N_("<name>"), N_("Specify name of positional keymap file") },
#endif
    { NULL }
};
#endif

int c64_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

