/*
 * sid-cmdline-options.c - SID command line options.
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Ettore Perazzoli <ettore@comm2000.it>
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
#include "sid-cmdline-options.h"


static const cmdline_option_t cmdline_options[] = {
    { "-sidengine", SET_RESOURCE, 1, NULL, NULL, "SidEngine", NULL,
      "<engine>", "Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel)" },
    { "-sidstereo", SET_RESOURCE, 0, NULL, NULL, "SidStereo",
      (void *)1,
      NULL, "Enable second SID" },
    { "-sidstereoaddress", SET_RESOURCE, 1, NULL, NULL,
      "SidStereoAddressStart", NULL,
      "<base address>", "Specify base address for 2nd SID" },
    { "-sidmodel", SET_RESOURCE, 1, NULL, NULL, "SidModel", NULL,
      "<model>", "Specify SID model (0: 6581, 1: 8580, 2: 8580 + digi boost)" },
    { "-sidfilters", SET_RESOURCE, 0, NULL, NULL, "SidFilters",
      (void *)1,
      NULL, "Emulate SID filters" },
    { "+sidfilters", SET_RESOURCE, 0, NULL, NULL, "SidFilters",
      (void *)0,
      NULL, "Do not emulate SID filters" },
#ifdef HAVE_RESID
    { "-residsamp <method>", SET_RESOURCE, 1, NULL, NULL, "SidResidSampling",
      (void *)0, NULL,
      "reSID sampling method (0: fast, 1: interpolating, 2: resampling)" },
    { "-residpass <percent>", SET_RESOURCE, 1, NULL, NULL, "SidResidPassband",
      (void *)90, NULL,
      "reSID resampling passband in percentage of total bandwidth (0 - 90)" },
#endif
    { NULL }
};

int sid_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

