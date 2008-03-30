/*
 * vdc-resources.c - Resources for the MOS 8563 (VDC) emulation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Markus Brenner <markus@brenner.de>
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

#include "raster-resources.h"
#include "resources.h"
#include "utils.h"
#include "vdc-resources.h"
#include "vdc.h"
#include "vdctypes.h"


vdc_resources_t vdc_resources;


static int set_palette_file_name(resource_value_t v, void *param)
{
    util_string_set (&vdc_resources.palette_file_name, (char *)v);
    if (vdc.initialized)
        return vdc_load_palette(vdc_resources.palette_file_name);

    return 0;
}

static int set_64kb_expansion(resource_value_t v, void *param)
{
    vdc_resources.vdc_64kb_expansion = (int)v;
    vdc.vdc_address_mask = vdc_resources.vdc_64kb_expansion
                           ? 0xffff : 0x3fff;
    return 0;
}

static int set_vdc_revision(resource_value_t v, void *param)
{
    unsigned int revision;

    revision = (unsigned int)v;

    if (revision > 2)
        return -1;

    vdc.revision = revision;

    return 0;
}

static resource_t resources[] =
{
    { "VDC_PaletteFile", RES_STRING, (resource_value_t)"vdc_deft",
      (resource_value_t *)&vdc_resources.palette_file_name,
      set_palette_file_name, NULL },
    { "VDC_64KB", RES_INTEGER, (resource_value_t)1,
      (resource_value_t *)&vdc_resources.vdc_64kb_expansion,
      set_64kb_expansion, NULL },
    { "VDC_Revision", RES_INTEGER, (resource_value_t)2,
      (resource_value_t *)&vdc.revision,
      set_vdc_revision, NULL },
    { NULL }
};


static int set_double_size_enabled(resource_value_t v, void *param)
{
    vdc_resources.double_size_enabled = (int)v;

    vdc.force_resize = 1;
    vdc.force_repaint = 1;

    return 0;
}

static int set_double_scan_enabled(resource_value_t v, void *param)
{
    vdc_resources.double_scan_enabled = (int)v;

    if (vdc.initialized)
        raster_enable_double_scan(&vdc.raster,
                                  vdc_resources.double_scan_enabled);

    vdc.force_repaint = 1;

    return 0;
}

static resource_t resources_2x[] =
{
    { "VDC_DoubleSize", RES_INTEGER, (resource_value_t)1,
      (resource_value_t *)&vdc_resources.double_size_enabled,
      set_double_size_enabled, NULL },
    { "VDC_DoubleScan", RES_INTEGER, (resource_value_t)1,
      (resource_value_t *)&vdc_resources.double_scan_enabled,
      set_double_scan_enabled, NULL },
    { NULL }
};


int vdc_resources_init(void)
{
    if (resources_register(resources_2x) < 0)
        return -1;

    if (raster_resources_chip_init("VDC", &vdc.raster) < 0)
        return -1;

    return resources_register(resources);
}

