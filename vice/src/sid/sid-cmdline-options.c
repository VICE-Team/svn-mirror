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
#include "translate.h"

static const cmdline_option_t sidcart_cmdline_options[] = {
    { "-sidengine", SET_RESOURCE, 1,
      NULL, NULL, "SidEngine", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_ENGINE, IDCLS_SPECIFY_SIDCART_ENGINE,
      NULL, NULL },
    { "-sidcart", SET_RESOURCE, 1,
      NULL, NULL, "SidCart", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_SIDCART,
      NULL, NULL },
    { "+sidcart", SET_RESOURCE, 0,
      NULL, NULL, "SidCart", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_SIDCART,
      NULL, NULL },
    { NULL }
};

static const cmdline_option_t sidengine_cmdline_options[] = {
    { "-sidengine", SET_RESOURCE, 1,
      NULL, NULL, "SidEngine", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_ENGINE, IDCLS_SPECIFY_SID_ENGINE,
      NULL, NULL },
    { NULL }
};

#ifdef HAVE_RESID
static const cmdline_option_t resid_cmdline_options[] = {
    { "-residsamp", SET_RESOURCE, 1,
      NULL, NULL, "SidResidSampling", (void *)0,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_METHOD, IDCLS_RESID_SAMPLING_METHOD,
      NULL, NULL },
    { "-residpass", SET_RESOURCE, 1,
      NULL, NULL, "SidResidPassband", (void *)90,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_PERCENT, IDCLS_PASSBAND_PERCENTAGE,
      NULL, NULL },
    { "-residgain", SET_RESOURCE, 1,
      NULL, NULL, "SidResidGain", (void *)97,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_PERCENT, IDCLS_RESID_GAIN_PERCENTAGE,
      NULL, NULL },
    { NULL }
};
#endif

static const cmdline_option_t common_cmdline_options[] = {
    { "-sidstereo", SET_RESOURCE, 0,
      NULL, NULL, "SidStereo", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_SECOND_SID,
      NULL, NULL },
    { "-sidstereoaddress", SET_RESOURCE, 1,
      NULL, NULL, "SidStereoAddressStart", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_BASE_ADDRESS, IDCLS_SPECIFY_SID_2_ADDRESS,
      NULL, NULL },
    { "-sidmodel", SET_RESOURCE, 1,
      NULL, NULL, "SidModel", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_MODEL, IDCLS_SPECIFY_SID_MODEL,
      NULL, NULL },
    { "-sidfilters", SET_RESOURCE, 0,
      NULL, NULL, "SidFilters", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_SID_FILTERS,
      NULL, NULL },
    { "+sidfilters", SET_RESOURCE, 0,
      NULL, NULL, "SidFilters", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_SID_FILTERS,
      NULL, NULL },
    { NULL }
};

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
