/*
 * debug.c - Various debugging options.
 *
 * Written by
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

#include "debug.h"
#include "resources.h"

debug_t debug;

static int set_do_core_dumps(resource_value_t v, void *param)
{
    debug.do_core_dumps = (int)v;
    return 0;
}

#ifdef DEBUG
static int set_maincpu_traceflg(resource_value_t v, void *param)
{
    debug.maincpu_traceflg = (int)v;
    return 0;
}

static int set_drive_traceflg(resource_value_t v, void *param)
{
    debug.drivecpu_traceflg[(int)param] = (int)v;
    return 0;
}
#endif

/* Debug-related resources. */
static resource_t resources[] = {
    { "DoCoreDump", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&debug.do_core_dumps,
      set_do_core_dumps, NULL },
#ifdef DEBUG
    { "MainCPU_TRACE", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&debug.maincpu_traceflg,
      set_maincpu_traceflg, NULL },
    { "Drive0CPU_TRACE", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&debug.drivecpu_traceflg[0],
      set_drive_traceflg, (void *)0 },
    { "Drive1CPU_TRACE", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&debug.drivecpu_traceflg[1],
      set_drive_traceflg, (void *)1 },
#endif
    { NULL }
};

int debug_resources_init(void)
{
    return resources_register(resources);
}

