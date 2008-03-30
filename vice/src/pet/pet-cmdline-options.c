/*
 * pet-cmdline-options.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#include "cmdline.h"
#include "pets.h"


static const cmdline_option_t cmdline_options[] = {
    { "-model", CALL_FUNCTION, 1, pet_set_model, NULL, NULL, NULL,
      "<modelnumber>", "Specify PET model to emulate" },
    { "-kernal", SET_RESOURCE, 1, NULL, NULL, "KernalName", NULL,
      "<name>", "Specify name of Kernal ROM image" },
    { "-basic", SET_RESOURCE, 1, NULL, NULL, "BasicName", NULL,
      "<name>", "Specify name of BASIC ROM image" },
    { "-editor", SET_RESOURCE, 1, NULL, NULL, "EditorName", NULL,
      "<name>", "Specify name of Editor ROM image" },
    { "-chargen", SET_RESOURCE, 1, NULL, NULL, "ChargenName", NULL,
      "<name>", "Specify name of character generator ROM image" },
    { "-rom9", SET_RESOURCE, 1, NULL, NULL, "RomModule9Name", NULL,
      "<name>", "Specify 4K extension ROM name at $9***" },
    { "-romA", SET_RESOURCE, 1, NULL, NULL, "RomModuleAName", NULL,
      "<name>", "Specify 4K extension ROM name at $A***" },
    { "-romB", SET_RESOURCE, 1, NULL, NULL, "RomModuleBName", NULL,
      "<name>", "Specify 4K extension ROM name at $B***" },
    { "-petram9", SET_RESOURCE, 0, NULL, NULL, "Ram9", (void *)1,
      NULL, "Enable PET8296 4K RAM mapping at $9***" },
    { "+petram9", SET_RESOURCE, 0, NULL, NULL, "Ram9", (void *)0,
      NULL, "Disable PET8296 4K RAM mapping at $9***" },
    { "-petramA", SET_RESOURCE, 0, NULL, NULL, "RamA", (void *)1,
      NULL, "Enable PET8296 4K RAM mapping at $A***" },
    { "+petramA", SET_RESOURCE, 0, NULL, NULL, "RamA", (void *)0,
      NULL, "Disable PET8296 4K RAM mapping at $A***" },
    { "-superpet", SET_RESOURCE, 0, NULL, NULL, "SuperPET", (void *)1,
      NULL, "Enable SuperPET I/O" },
    { "+superpet", SET_RESOURCE, 0, NULL, NULL, "SuperPET", (void *)0,
      NULL, "Disable SuperPET I/O" },
    { "-basic1", SET_RESOURCE, 0, NULL, NULL, "Basic1", (void *)1,
      NULL, "Enable ROM 1 Kernal patches" },
    { "+basic1", SET_RESOURCE, 0, NULL, NULL, "Basic1", (void *)0,
      NULL, "Disable ROM 1 Kernal patches" },
    { "-basic1char", SET_RESOURCE, 0, NULL, NULL, "Basic1Chars",
      (void *)1,
      NULL, "Switch upper/lower case charset" },
    { "+basic1char", SET_RESOURCE, 0, NULL, NULL, "Basic1Chars",
      (void *)0,
      NULL, "Do not switch upper/lower case charset" },
    { "-eoiblank", SET_RESOURCE, 0, NULL, NULL, "EoiBlank", (void *)1,
      NULL, "EOI blanks screen" },
    { "+eoiblank", SET_RESOURCE, 0, NULL, NULL, "EoiBlank", (void *)0,
      NULL, "EOI does not blank screen" },
    {  "-emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (void *)1,
      NULL, "Enable emulator identification" },
    {  "+emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (void *)0,
      NULL, "Disable emulator identification" },
#ifdef COMMON_KBD
    { "-keymap", SET_RESOURCE, 1, NULL, NULL, "KeymapIndex", NULL,
      "<number>", N_("Specify index of keymap file") },
    { "-grsymkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapGraphicsSymFile",
      NULL, "<name>",
      N_("Specify name of graphics keyboard symbolic keymap file") },
    { "-grposkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapGraphicsPosFile",
      NULL, "<name>",
      N_("Specify name of graphics keyboard positional keymap file") },
    { "-buksymkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapBusinessUKSymFile",
      NULL, "<name>",
      N_("Specify name of UK business keyboard symbolic keymap file") },
    { "-bukposkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapBusinessUKPosFile",
      NULL, "<name>",
      N_("Specify name of UK business keyboard positional keymap file") },
    { "-bdesymkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapBusinessDESymFile",
      NULL, "<name>",
      N_("Specify name of German business keyboard symbolic keymap file") },
    { "-bdeposkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapBusinessDEPosFile",
      NULL, "<name>",
      N_("Specify name of German business keyboard positional keymap file") },
#endif
    { NULL }
};

int pet_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

