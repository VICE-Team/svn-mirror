/*
 * raster-resources.c - Raster-based video chip emulation helper, resources.
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

#include "raster.h"
#include "resources.h"
#include "types.h"
#include "utils.h"


#ifdef __MSDOS__
#define DEFAULT_VideoCache_VALUE 0
#else
#define DEFAULT_VideoCache_VALUE 1
#endif

struct raster_resource_chip_s {
    raster_t *raster;
    int video_cache_enabled;
};
typedef struct raster_resource_chip_s raster_resource_chip_t;


int raster_set_video_cache_enabled(resource_value_t v, void *param)
{
    raster_resource_chip_t *raster_resource_chip;

    raster_resource_chip = (raster_resource_chip_t *)param;

    if ((int)v > 0)
        raster_resource_chip->video_cache_enabled = (int)v;

    raster_enable_cache(raster_resource_chip->raster,
                        raster_resource_chip->video_cache_enabled);

    return 0;
}

static const char *rname_chip[] = { "VideoCache", NULL };

static resource_t resources_chip[] =
{
    { NULL, RES_INTEGER, (resource_value_t)DEFAULT_VideoCache_VALUE,
      (resource_value_t *)NULL, raster_set_video_cache_enabled, NULL },
    { NULL }
};

int raster_resources_chip_init(const char *chipname, raster_t *raster)
{
    unsigned int i;
    raster_resource_chip_t *raster_resource_chip;

    raster_resource_chip
        = (raster_resource_chip_t *)xmalloc(sizeof(raster_resource_chip_t));

    raster_resource_chip->raster = raster;

    for (i = 0; rname_chip[i] != NULL; i++) {
        resources_chip[i].name = concat(chipname, rname_chip[i], NULL);
        resources_chip[i].param = (void *)raster_resource_chip;
    }

    resources_chip[0].value_ptr
        = (resource_value_t *)&(raster_resource_chip->video_cache_enabled);

    return resources_register(resources_chip);
}

