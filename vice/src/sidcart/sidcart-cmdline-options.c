/*
 * sidcart-cmdline-options.c - SID cartridge command line options.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include "sidcart-cmdline-options.h"

#ifdef HAS_TRANSLATION
#include "translate.h"

static const cmdline_option_t cmdline_options[] = {
    { "-sidengine", SET_RESOURCE, 1, NULL, NULL, "SidEngine", NULL,
      IDCLS_P_ENGINE, IDCLS_SPECIFY_SID_ENGINE },
    { "-sidmodel", SET_RESOURCE, 1, NULL, NULL, "SidModel", NULL,
      IDCLS_P_MODEL, IDCLS_SPECIFY_SID_MODEL },
    { "-sidfilters", SET_RESOURCE, 0, NULL, NULL, "SidFilters",
      (void *)1,
      0, IDCLS_ENABLE_SID_FILTERS },
    { "+sidfilters", SET_RESOURCE, 0, NULL, NULL, "SidFilters",
      (void *)0,
      0, IDCLS_DISABLE_SID_FILTERS },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] = {
    { "-sidengine", SET_RESOURCE, 1, NULL, NULL, "SidEngine", NULL,
    N_("<engine>"),
#ifdef HAVE_CATWEASELMKIII
#  ifdef HAVE_HARDSID
#    ifdef HAVE_PARSID
       N_("Specify SID engine (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID)")
#    else
       N_("Specify SID engine (0: FastSID, 2: Catweasel, 3: HardSID)")
#    endif
#  else
#    ifdef HAVE_PARSID
       N_("Specify SID engine (0: FastSID, 2: Catweasel, 4: ParSID)")
#    else
       N_("Specify SID engine (0: FastSID, 2: Catweasel)")
#    endif
#  endif
#else
#  ifdef HAVE_HARDSID
#    ifdef HAVE_PARSID
       N_("Specify SID engine (0: FastSID, 3: HardSID, 4: ParSID)")
#    else
       N_("Specify SID engine (0: FastSID, 3: HardSID)")
#    endif
#  else
#    ifdef HAVE_PARSID
       N_("Specify SID engine (0: FastSID, 4: ParSID)")
#    else
       N_("Specify SID engine (0: FastSID)")
#    endif
#  endif
#endif

    },
    { "-sidmodel", SET_RESOURCE, 1, NULL, NULL, "SidModel", NULL,
      N_("<model>"), N_("Specify SID model (0: 6581, 1: 8580, 2: 8580 + digi boost)") },
    { "-sidfilters", SET_RESOURCE, 0, NULL, NULL, "SidFilters",
      (void *)1,
      NULL, N_("Emulate SID filters") },
    { "+sidfilters", SET_RESOURCE, 0, NULL, NULL, "SidFilters",
      (void *)0,
      NULL, N_("Do not emulate SID filters") },
    { NULL }
};
#endif

int sidcart_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}
