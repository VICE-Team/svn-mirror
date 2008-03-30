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

#include "hardsid.h"
#include "machine.h"
#ifdef HAVE_PARSID
#include "parsid.h"
#endif
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
#ifdef HAVE_RESID
static int sid_resid_sampling;
static int sid_resid_passband;
static int sid_resid_gain;
#endif
int sid_stereo;
int checking_sid_stereo;
unsigned int sid_stereo_address_start;
unsigned int sid_stereo_address_end;
static int sid_engine;
#ifdef HAVE_HARDSID
static unsigned int sid_hardsid_main;
static unsigned int sid_hardsid_right;
#endif
#ifdef HAVE_PARSID
int parsid_port=0;
#endif

static int set_sid_engine(int engine, void *param)
{
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
#ifdef HAVE_PARSID
        && engine != SID_ENGINE_PARSID_PORT1
        && engine != SID_ENGINE_PARSID_PORT2
        && engine != SID_ENGINE_PARSID_PORT3
#endif
        )
        return -1;

    if (sid_engine_set(engine) < 0)
        return -1;

    sid_engine = engine;

#ifdef HAVE_PARSID
    if (engine == SID_ENGINE_PARSID_PORT1)
      parsid_port=1;
    if (engine == SID_ENGINE_PARSID_PORT2)
      parsid_port=2;
    if (engine == SID_ENGINE_PARSID_PORT3)
      parsid_port=3;
#endif

    sound_state_changed = 1;

    return 0;
}

static int set_sid_filters_enabled(int val, void *param)
{
    sid_filters_enabled = val;
    sid_state_changed = 1;
    return 0;
}

static int set_sid_stereo(int val, void *param)
{
    sid_stereo = val;
    sound_state_changed = 1;
    return 0;
}

int sid_set_sid_stereo_address(int val, void *param)
{
    unsigned int sid2_adr;

    sid2_adr = (unsigned int)val;

    if (machine_sid2_check_range(sid2_adr) < 0)
        return -1;

    sid_stereo_address_start = sid2_adr;
    sid_stereo_address_end = sid_stereo_address_start + 32;
    return 0;
}

static int set_sid_model(int val, void *param)
{
    sid_model = val;
    sid_state_changed = 1;
    return 0;
}

#ifdef HAVE_RESID
static int set_sid_resid_sampling(int val, void *param)
{
    sid_resid_sampling = val;
    sid_state_changed = 1;
    return 0;
}

static int set_sid_resid_passband(int i, void *param)
{
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

static int set_sid_resid_gain(int i, void *param)
{
    if (i < 90)
        i = 90;
    else if (i > 100)
        i = 100;

    sid_resid_gain = i;
    sid_state_changed = 1;
    return 0;
}

#endif

#ifdef HAVE_HARDSID
static int set_sid_hardsid_main(int val, void *param)
{
    sid_hardsid_main = (unsigned int)val;
    hardsid_set_device(0, sid_hardsid_main);

    return 0;
}

static int set_sid_hardsid_right(int val, void *param)
{
    sid_hardsid_right = (unsigned int)val;
    hardsid_set_device(1, sid_hardsid_right);

    return 0;
}
#endif

#ifdef HAVE_PARSID
static int set_sid_parsid_port(int val, void *param)
{
    if (val == parsid_port)
        return 0;

    if (sid_engine == SID_ENGINE_PARSID_PORT1
        || sid_engine == SID_ENGINE_PARSID_PORT2
        || sid_engine == SID_ENGINE_PARSID_PORT3) {
        if (parsid_check_port(val) < 0)
            return -1;
        else {
            if (val == 1)
                sid_engine = SID_ENGINE_PARSID_PORT1;
            if (val == 2)
                sid_engine = SID_ENGINE_PARSID_PORT2;
            if (val == 3)
                sid_engine = SID_ENGINE_PARSID_PORT3;
        }
    }
    parsid_port = val;

    return 0;
}
#endif

static const resource_int_t resources_int[] = {
#ifdef HAVE_RESID
    { "SidEngine", SID_ENGINE_RESID,
      RES_EVENT_STRICT, (resource_value_t)SID_ENGINE_RESID,
      &sid_engine, set_sid_engine, NULL },
#else
    { "SidEngine", SID_ENGINE_FASTSID,
      RES_EVENT_STRICT, (resource_value_t)SID_ENGINE_RESID, /* FIXME: deadl. */
      &sid_engine, set_sid_engine, NULL },
#endif
    { "SidFilters", 1, RES_EVENT_SAME, NULL,
      &sid_filters_enabled, set_sid_filters_enabled, NULL },
    { "SidModel", 0, RES_EVENT_SAME, NULL,
      &sid_model, set_sid_model, NULL },
    { "SidStereo", 0, RES_EVENT_SAME, NULL,
      &sid_stereo, set_sid_stereo, NULL },
#ifdef HAVE_RESID
    { "SidResidSampling", 0, RES_EVENT_NO, NULL,
      &sid_resid_sampling, set_sid_resid_sampling, NULL },
    { "SidResidPassband", 90, RES_EVENT_NO, NULL,
      &sid_resid_passband, set_sid_resid_passband, NULL },
    { "SidResidGain", 97, RES_EVENT_NO, NULL,
      &sid_resid_gain, set_sid_resid_gain, NULL },
#endif
#ifdef HAVE_HARDSID
    { "SidHardSIDMain", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &sid_hardsid_main, set_sid_hardsid_main, NULL },
    { "SidHardSIDRight", 1, RES_EVENT_NO, NULL,
      &sid_hardsid_right, set_sid_hardsid_right, NULL },
#endif
#ifdef HAVE_PARSID
    { "SidParSIDport", 1, RES_EVENT_STRICT, (resource_value_t)1,
      &parsid_port, set_sid_parsid_port, NULL },
#endif
    { NULL }
};

int sid_resources_init(void)
{
    return resources_register_int(resources_int);
}
