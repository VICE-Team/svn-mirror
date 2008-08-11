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
#include "sid.h"
#include "sid-cmdline-options.h"

#ifdef HAS_TRANSLATION
#include "translate.h"
#endif

#ifdef HAS_TRANSLATION
static const cmdline_option_t sidcart_cmdline_options[] = {
    { "-sidengine", SET_RESOURCE, 1, NULL, NULL, "SidEngine", NULL,
      IDCLS_P_ENGINE, IDCLS_SPECIFY_SIDCART_ENGINE },
    { "-sidenable", SET_RESOURCE, 1, NULL, NULL, "SidCart", NULL,
      0, IDCLS_ENABLE_SIDCART },
    { NULL }
};
#else
static const cmdline_option_t sidcart_cmdline_options[] = {
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
    { "-sidenable", SET_RESOURCE, 1, NULL, NULL, "SidCart", NULL,
    NULL, N_("Enable SID Cartridge") },
    { NULL }
};
#endif

#ifdef HAS_TRANSLATION
static const cmdline_option_t sidengine_cmdline_options[] = {
    { "-sidengine", SET_RESOURCE, 1, NULL, NULL, "SidEngine", NULL,
      IDCLS_P_ENGINE, IDCLS_SPECIFY_SID_ENGINE },
    { NULL }
};
#else
static const cmdline_option_t sidengine_cmdline_options[] = {
    { "-sidengine", SET_RESOURCE, 1, NULL, NULL, "SidEngine", NULL,
    N_("<engine>"),
#ifdef HAVE_RESID
#  ifdef HAVE_CATWEASELMKIII
#    ifdef HAVE_HARDSID
#      ifdef HAVE_PARSID
         N_("Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID, 4: ParSID)")
#      else
         N_("Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 3: HardSID)")
#      endif
#    else
#      ifdef HAVE_PARSID
         N_("Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel, 4: ParSID)")
#      else
         N_("Specify SID engine (0: FastSID, 1: ReSID, 2: Catweasel)")
#      endif
#    endif
#  else
#    ifdef HAVE_HARDSID
#      ifdef HAVE_PARSID
         N_("Specify SID engine (0: FastSID, 1: ReSID, 3: HardSID, 4: ParSID)")
#      else
         N_("Specify SID engine (0: FastSID, 1: ReSID, 3: HardSID)")
#      endif
#    else
#      ifdef HAVE_PARSID
         N_("Specify SID engine (0: FastSID, 1: ReSID, 4: ParSID)")
#      else
         N_("Specify SID engine (0: FastSID, 1: ReSID)")
#      endif
#    endif
#  endif
#else
#  ifdef HAVE_CATWEASELMKIII
#    ifdef HAVE_HARDSID
#      ifdef HAVE_PARSID
         N_("Specify SID engine (0: FastSID, 2: Catweasel, 3: HardSID, 4: ParSID)")
#      else
         N_("Specify SID engine (0: FastSID, 2: Catweasel, 3: HardSID)")
#      endif
#    else
#      ifdef HAVE_PARSID
         N_("Specify SID engine (0: FastSID, 2: Catweasel, 4: ParSID)")
#      else
         N_("Specify SID engine (0: FastSID, 2: Catweasel)")
#      endif
#    endif
#  else
#    ifdef HAVE_HARDSID
#      ifdef HAVE_PARSID
         N_("Specify SID engine (0: FastSID, 3: HardSID, 4: ParSID)")
#      else
         N_("Specify SID engine (0: FastSID, 3: HardSID)")
#      endif
#    else
#      ifdef HAVE_PARSID
         N_("Specify SID engine (0: FastSID, 4: ParSID)")
#      else
         N_("Specify SID engine (0: FastSID)")
#      endif
#    endif
#  endif
#endif

    },
    { NULL }
};
#endif

#ifdef HAVE_RESID
#ifdef HAS_TRANSLATION
static const cmdline_option_t resid_cmdline_options[] = {
    { "-residsamp", SET_RESOURCE, 1, NULL, NULL, "SidResidSampling",
      (void *)0, IDCLS_P_METHOD,
      IDCLS_RESID_SAMPLING_METHOD },
    { "-residpass", SET_RESOURCE, 1, NULL, NULL, "SidResidPassband",
      (void *)90, IDCLS_P_PERCENT,
      IDCLS_PASSBAND_PERCENTAGE },
    { "-residgain", SET_RESOURCE, 1, NULL, NULL, "SidResidGain",
      (void *)97, IDCLS_P_PERCENT,
      IDCLS_RESID_GAIN_PERCENTAGE },
    { NULL }
};
#else
static const cmdline_option_t resid_cmdline_options[] = {
    { "-residsamp", SET_RESOURCE, 1, NULL, NULL, "SidResidSampling",
      (void *)0, N_("<method>"),
      N_("reSID sampling method (0: fast, 1: interpolating, 2: resampling, 3: fast resampling)") },
    { "-residpass", SET_RESOURCE, 1, NULL, NULL, "SidResidPassband",
      (void *)90, N_("<percent>"),
      N_("reSID resampling passband in percentage of total bandwidth (0 - 90)") },
    { "-residgain", SET_RESOURCE, 1, NULL, NULL, "SidResidGain",
      (void *)97, N_("<percent>"),
      N_("reSID gain in percent (90 - 100)") },
    { NULL }
};
#endif
#endif

#ifdef HAS_TRANSLATION
static const cmdline_option_t common_cmdline_options[] = {
    { "-sidstereo", SET_RESOURCE, 0, NULL, NULL, "SidStereo",
      (void *)1,
      0, IDCLS_ENABLE_SECOND_SID },
    { "-sidstereoaddress", SET_RESOURCE, 1, NULL, NULL,
      "SidStereoAddressStart", NULL,
      IDCLS_P_BASE_ADDRESS, IDCLS_SPECIFY_SID_2_ADDRESS },
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
static const cmdline_option_t common_cmdline_options[] = {
    { "-sidstereo", SET_RESOURCE, 0, NULL, NULL, "SidStereo",
      (void *)1,
      NULL, N_("Enable second SID") },
    { "-sidstereoaddress", SET_RESOURCE, 1, NULL, NULL,
      "SidStereoAddressStart", NULL,
      N_("<base address>"), N_("Specify base address for 2nd SID") },
    { "-sidmodel", SET_RESOURCE, 1, NULL, NULL, "SidModel", NULL,
      N_("<model>"), N_("Specify SID model (0: 6581, 1: 8580, 2: 8580 + digi boost, 4: DTV)") },
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
    if (cmdline_register_options(sidcart_cmdline_options)<0)
        return -1;

    return cmdline_register_options(common_cmdline_options);
}

int sid_cmdline_options_init(void)
{
    if (cmdline_register_options(sidengine_cmdline_options)<0)
        return -1;

#ifdef HAVE_RESID
    if (cmdline_register_options(resid_cmdline_options)<0)
        return -1;
#endif

    return cmdline_register_options(common_cmdline_options);
}
