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
    { "-basic", SET_RESOURCE, 1, NULL, NULL, "BasicName", NULL,
      "<name>", "Specify name of BASIC ROM image" },
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
    { "-acia1", SET_RESOURCE, 0, NULL, NULL, "AciaDE", (void *)1,
      NULL, "Enable the $DE** ACIA RS232 interface emulation" },
    { "+acia1", SET_RESOURCE, 0, NULL, NULL, "AciaDE", (void *)0,
      NULL, "Disable the $DE** ACIA RS232 interface emulation" },
#if 0
    { "-acia2", SET_RESOURCE, 0, NULL, NULL, "AciaD7", (void *)1,
      NULL, "Enable the $D7** ACIA RS232 interface emulation" },
    { "+acia2", SET_RESOURCE, 0, NULL, NULL, "AciaD7", (void *)0,
      NULL, "Disable the $D7** ACIA RS232 interface emulation" },
#endif
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

int c128_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

