/*
 * cbm2-cmdline-options.c
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

#include "cbm2-cmdline-options.h"
#include "cbm2mem.h"
#include "cmdline.h"
#include "translate.h"

static const cmdline_option_t cmdline_options[] = {
    { "-model", CALL_FUNCTION, 1,
     cbm2_set_model, NULL, NULL, NULL,
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDCLS_P_MODELNUMBER, IDCLS_SPECIFY_CBM2_MODEL,
     NULL, NULL },
    { "-usevicii", SET_RESOURCE, 0,
      NULL, NULL, "UseVicII", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_SPECIFY_TO_USE_VIC_II,
      NULL, NULL },
    { "+usevicii", SET_RESOURCE, 0,
      NULL, NULL, "UseVicII", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_SPECIFY_TO_USE_CRTC,
      NULL, NULL },
    { "-modelline", SET_RESOURCE, 1,
      NULL, NULL, "ModelLine", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_LINENUMBER, IDCLS_SPECIFY_CBM2_MODEL_HARDWARE,
      NULL, NULL },
    { "-ramsize", SET_RESOURCE, 1,
      NULL, NULL, "RamSize", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_RAMSIZE, IDCLS_SPECIFY_SIZE_OF_RAM,
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
    { "-cart1", SET_RESOURCE, 1,
      NULL, NULL, "Cart1Name", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_CART_ROM_1000_NAME,
      NULL, NULL },
    { "-cart2", SET_RESOURCE, 1,
      NULL, NULL, "Cart2Name", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_CART_ROM_2000_NAME,
      NULL, NULL },
    { "-cart4", SET_RESOURCE, 1,
      NULL, NULL, "Cart4Name", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_CART_ROM_4000_NAME,
      NULL, NULL },
    { "-cart6", SET_RESOURCE, 1,
      NULL, NULL, "Cart6Name", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_CART_ROM_6000_NAME,
      NULL, NULL },
    { "-ram08", SET_RESOURCE, 0,
      NULL, NULL, "Ram08", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_RAM_MAPPING_IN_0800,
      NULL, NULL, },
    { "+ram08", SET_RESOURCE, 0,
      NULL, NULL, "Ram08", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_RAM_MAPPING_IN_0800,
      NULL, NULL },
    { "-ram1", SET_RESOURCE, 0,
      NULL, NULL, "Ram1", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_RAM_MAPPING_IN_1000,
      NULL, NULL },
    { "+ram1", SET_RESOURCE, 0,
      NULL, NULL, "Ram1", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_RAM_MAPPING_IN_1000,
      NULL, NULL },
    { "-ram2", SET_RESOURCE, 0,
      NULL, NULL, "Ram2", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_RAM_MAPPING_IN_2000,
      NULL, NULL },
    { "+ram2", SET_RESOURCE, 0,
      NULL, NULL, "Ram2", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_RAM_MAPPING_IN_2000,
      NULL, NULL },
    { "-ram4", SET_RESOURCE, 0,
      NULL, NULL, "Ram4", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_RAM_MAPPING_IN_4000,
      NULL, NULL },
    { "+ram4", SET_RESOURCE, 0,
      NULL, NULL, "Ram4", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_RAM_MAPPING_IN_4000,
      NULL, NULL },
    { "-ram6", SET_RESOURCE, 0,
      NULL, NULL, "Ram6", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_RAM_MAPPING_IN_6000,
      NULL, NULL },
    { "+ram6", SET_RESOURCE, 0,
      NULL, NULL, "Ram6", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_RAM_MAPPING_IN_6000,
      NULL, NULL },
    { "-ramC", SET_RESOURCE, 0,
      NULL, NULL, "RamC", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_RAM_MAPPING_IN_C000,
      NULL, NULL },
    { "+ramC", SET_RESOURCE, 0,
      NULL, NULL, "RamC", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_RAM_MAPPING_IN_C000,
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
#ifdef COMMON_KBD
    { "-keymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapIndex", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NUMBER, IDCLS_SPECIFY_KEYMAP_INDEX,
      NULL, NULL },
    { "-grsymkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapGraphicsSymFile", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_GFX_SYM_KEYMAP_NAME,
      NULL, NULL },
    { "-grposkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapGraphicsPosFile", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_GFX_POS_KEYMAP_NAME,
      NULL, NULL },
    { "-buksymkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapBusinessUKSymFile", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_BUK_SYM_KEYMAP_NAME,
      NULL, NULL },
    { "-bukposkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapBusinessUKPosFile", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_BUK_POS_KEYMAP_NAME,
      NULL, NULL },
    { "-bdesymkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapBusinessDESymFile", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_BDE_SYM_KEYMAP_NAME,
      NULL, NULL },
    { "-bdeposkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapBusinessDEPosFile", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_BDE_POS_KEYMAP_NAME,
      NULL, NULL },
#endif
    { NULL }
};

int cbm2_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}
