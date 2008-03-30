/*
 * sidcart-resources.c - SID cartridge resources.
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

#include "hardsid.h"
#include "machine.h"
#ifdef HAVE_PARSID
#include "parsid.h"
#endif
#include "resources.h"
#include "sidcart-resources.h"
#include "sidcart.h"
#include "sound.h"
#include "types.h"

/* Resource handling -- Added by Ettore 98-04-26.  */

/* FIXME: We need sanity checks!  And do we really need all of these
   `close_sound()' calls?  */

static int sid_filters_enabled;       /* app_resources.sidFilters */
static int sid_model;                 /* app_resources.sidModel */
static int sid_engine;
#ifdef HAVE_HARDSID
static unsigned int sid_hardsid_main;
static unsigned int sid_hardsid_right;
#endif
#ifdef HAVE_PARSID
int parsid_port=0;
#endif

int sidcart_enabled;
int sidcart_address;
int sidcart_clock;

static int set_sid_engine(int engine, void *param)
{
    if (engine != SID_ENGINE_FASTSID
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

static int set_sidcart_enabled(int val, void *param)
{
    sidcart_enabled = val;
    sound_state_changed = 1;
    return 0;
}

static int set_sid_address(int val, void *param)
{
    sidcart_address = val;
    return 0;
}

static int set_sid_clock(int val, void *param)
{
    sidcart_clock = val;
    return 0;
}

static int set_sid_filters_enabled(int val, void *param)
{
    sid_filters_enabled = val;
    sid_state_changed = 1;
    return 0;
}

static int set_sid_model(int val, void *param)
{
    sid_model = val;
    sid_state_changed = 1;
    return 0;
}

#ifdef HAVE_HARDSID
static int set_sid_hardsid_main(int val, void *param)
{
    sid_hardsid_main = (unsigned int)val;
    hardsid_set_device(0, sid_hardsid_main);

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
    { "SidCart", 0, RES_EVENT_SAME, NULL,
      &sidcart_enabled, set_sidcart_enabled, NULL },
    { "SidEngine", SID_ENGINE_FASTSID,
      RES_EVENT_STRICT, (resource_value_t)SID_ENGINE_FASTSID,
      &sid_engine, set_sid_engine, NULL },
    { "SidFilters", 1, RES_EVENT_SAME, NULL,
      &sid_filters_enabled, set_sid_filters_enabled, NULL },
    { "SidModel", 0, RES_EVENT_SAME, NULL,
      &sid_model, set_sid_model, NULL },
    { "SidAddress", 0, RES_EVENT_SAME, NULL,
      &sidcart_address, set_sid_address, NULL },
    { "SidClock", 1, RES_EVENT_SAME, NULL,
      &sidcart_clock, set_sid_clock, NULL },
#ifdef HAVE_HARDSID
    { "SidHardSIDMain", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &sid_hardsid_main, set_sid_hardsid_main, NULL },
#endif
#ifdef HAVE_PARSID
    { "SidParSIDport", 1, RES_EVENT_STRICT, (resource_value_t)1,
      &parsid_port, set_sid_parsid_port, NULL },
#endif
    { NULL }
};

int sidcart_resources_init(void)
{
    return resources_register_int(resources_int);
}
