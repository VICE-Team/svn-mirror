/*
 * ieee-cmdline-options.c
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
#include "ieee-cmdline-options.h"

#ifdef HAS_TRANSLATION
#include "translate.h"

static const cmdline_option_t cmdline_options[] = {
    { "-dos2031", SET_RESOURCE, 1, NULL, NULL, "DosName2031", "dos2031",
      IDCLS_P_NAME, IDCLS_SPECIFY_2031_DOS_ROM_NAME },
    { "-dos2040", SET_RESOURCE, 1, NULL, NULL, "DosName2040", "dos2040",
      IDCLS_P_NAME, IDCLS_SPECIFY_2040_DOS_ROM_NAME },
    { "-dos3040", SET_RESOURCE, 1, NULL, NULL, "DosName3040", "dos3040",
      IDCLS_P_NAME, IDCLS_SPECIFY_3040_DOS_ROM_NAME },
    { "-dos4040", SET_RESOURCE, 1, NULL, NULL, "DosName4040", "dos4040",
      IDCLS_P_NAME, IDCLS_SPECIFY_4040_DOS_ROM_NAME },
    { "-dos1001", SET_RESOURCE, 1, NULL, NULL, "DosName1001", "dos1001",
      IDCLS_P_NAME, IDCLS_SPECIFY_1001_DOS_ROM_NAME },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] = {
    { "-dos2031", SET_RESOURCE, 1, NULL, NULL, "DosName2031", "dos2031",
      N_("<name>"), N_("Specify name of 2031 DOS ROM image") },
    { "-dos2040", SET_RESOURCE, 1, NULL, NULL, "DosName2040", "dos2040",
      N_("<name>"), N_("Specify name of 2040 DOS ROM image") },
    { "-dos3040", SET_RESOURCE, 1, NULL, NULL, "DosName3040", "dos3040",
      N_("<name>"), N_("Specify name of 3040 DOS ROM image") },
    { "-dos4040", SET_RESOURCE, 1, NULL, NULL, "DosName4040", "dos4040",
      N_("<name>"), N_("Specify name of 4040 DOS ROM image") },
    { "-dos1001", SET_RESOURCE, 1, NULL, NULL, "DosName1001", "dos1001",
      N_("<name>"), N_("Specify name of 1001/8050/8250 DOS ROM image") },
    { NULL }
};
#endif

int ieee_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

