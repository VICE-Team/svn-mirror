/*
 * sid-cmdline-options.c - SID command line options.
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include "resources.h"
#include "sid-cmdline-options.h"

static cmdline_option_t cmdline_options[] = {
    { "-sidmodel", SET_RESOURCE, 1, NULL, NULL, "SidModel", NULL,
      "<model>", "Specify SID model (1: 8580, 0: 6581)" },
    { "-sidfilters", SET_RESOURCE, 0, NULL, NULL, "SidFilters",
      (resource_value_t) 1,
      NULL, "Emulate SID filters" },
    { "+sidfilters", SET_RESOURCE, 0, NULL, NULL, "SidFilters",
      (resource_value_t)0,
      NULL, "Do not emulate SID filters" },
#ifdef HAVE_RESID
    { "-resid", SET_RESOURCE, 0, NULL, NULL, "SidUseResid",
      (resource_value_t)1,
      NULL, "Use reSID emulation" },
    { "+resid", SET_RESOURCE, 0, NULL, NULL, "SidUseResid",
      (resource_value_t)0,
      NULL, "Use fast SID emulation" },
    { "-residsamp <method>", SET_RESOURCE, 1, NULL, NULL, "SidResidSampling",
      (resource_value_t)0, NULL,
      "reSID sampling method (0: fast, 1: interpolating, 2: resampling)" },
    { "-residpass <percent>", SET_RESOURCE, 1, NULL, NULL, "SidResidPassband",
      (resource_value_t)90, NULL,
      "reSID resampling passband in percentage of total bandwidth (0 - 90)" },
#endif
    { NULL }
};

int sid_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

