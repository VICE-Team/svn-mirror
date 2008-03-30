/*
 * sid-resources.c - SID resources.
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

#include "machine.h"
#include "resources.h"
#include "sid-resources.h"
#include "sid.h"
#include "sound.h"
#include "types.h"


/* Resource handling -- Added by Ettore 98-04-26.  */

/* FIXME: We need sanity checks!  And do we really need all of these
   `close_sound()' calls?  */

static int sid_filters_enabled;       /* app_resources.sidFilters */
static int sid_model;                 /* app_resources.sidModel */
static int sid_resid_sampling;
static int sid_resid_passband;
int sid_stereo;
unsigned int sid_stereo_address_start;
unsigned int sid_stereo_address_end;
static int sid_engine;

static int set_sid_engine(resource_value_t v, void *param)
{
    int engine;

    engine = (int)v;

    if (engine != SID_ENGINE_FASTSID
#ifdef HAVE_RESID
        && engine != SID_ENGINE_RESID
#endif
#ifdef HAVE_CATWEASELMKIII
        && engine != SID_ENGINE_CATWEASELMKIII
#endif
#ifdef HAVE_HARDSID
        && engine != SID_ENGINE_HARDSID
#endif
        )
        return -1;

    if (sid_engine_set(engine) < 0)
        return -1;

    sid_engine = engine;

    sound_state_changed = 1;

    return 0;
}

static int set_sid_filters_enabled(resource_value_t v, void *param)
{
    sid_filters_enabled = (int)v;
    sid_state_changed = 1;
    return 0;
}

static int set_sid_stereo(resource_value_t v, void *param)
{
    sid_stereo = (int)v;
    sound_state_changed = 1;
    return 0;
}

int sid_set_sid_stereo_address(resource_value_t v, void *param)
{
    unsigned int sid2_adr;

    sid2_adr = (unsigned int)v;

    if (machine_sid2_check_range(sid2_adr) < 0)
        return -1;

    sid_stereo_address_start = sid2_adr;
    sid_stereo_address_end = sid_stereo_address_start + 32;
    return 0;
}

static int set_sid_model(resource_value_t v, void *param)
{
    sid_model = (int)v;
    sid_state_changed = 1;
    return 0;
}

#ifdef HAVE_RESID
static int set_sid_resid_sampling(resource_value_t v, void *param)
{
    sid_resid_sampling = (int)v;
    sid_state_changed = 1;
    return 0;
}

static int set_sid_resid_passband(resource_value_t v, void *param)
{
    int i = (int)v;

    if (i < 0) {
        i = 0;
    }
    else if (i > 90) {
        i = 90;
    }

    sid_resid_passband = i;
    sid_state_changed = 1;
    return 0;
}
#endif

static const resource_t resources[] = {
    { "SidEngine", RES_INTEGER, (resource_value_t)SID_ENGINE_FASTSID,
      (void *)&sid_engine, set_sid_engine, NULL },
    { "SidFilters", RES_INTEGER, (resource_value_t)1,
      (void *)&sid_filters_enabled, set_sid_filters_enabled, NULL },
    { "SidModel", RES_INTEGER, (resource_value_t)0,
      (void *)&sid_model, set_sid_model, NULL },
    { "SidStereo", RES_INTEGER, (resource_value_t)0,
      (void *)&sid_stereo, set_sid_stereo, NULL },
#ifdef HAVE_RESID
    { "SidResidSampling", RES_INTEGER, (resource_value_t)0,
      (void *)&sid_resid_sampling, set_sid_resid_sampling, NULL },
    { "SidResidPassband", RES_INTEGER, (resource_value_t)90,
      (void *)&sid_resid_passband, set_sid_resid_passband, NULL },
#endif
    { NULL }
};

int sid_resources_init(void)
{
    return resources_register(resources);
}

