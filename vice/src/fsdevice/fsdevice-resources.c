/*
 * fsdevice-resources.c - File system device, resources.
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

#include "archdep.h"
#include "lib.h"
#include "resources.h"
#include "utils.h"


int fsdevice_convert_p00_enabled[4];
int fsdevice_save_p00_enabled[4];
int fsdevice_hide_cbm_files_enabled[4];
char *fsdevice_dir[4] = { NULL, NULL, NULL, NULL };


static int set_fsdevice_convert_p00(resource_value_t v, void *param)
{
    fsdevice_convert_p00_enabled[(int)param - 8] = (int)v;
    return 0;
}

static int set_fsdevice_dir(resource_value_t v, void *param)
{
    const char *name = (const char *)v;

    util_string_set(&fsdevice_dir[(int)param - 8], name ? name : "");
    return 0;
}

static int set_fsdevice_save_p00(resource_value_t v, void *param)
{
    fsdevice_save_p00_enabled[(int)param - 8] = (int)v;
    return 0;
}

static int set_fsdevice_hide_cbm_files(resource_value_t v, void *param)
{
    int val = (int)v;

    if (val && !fsdevice_convert_p00_enabled[(int)param - 8])
        return -1;

    fsdevice_hide_cbm_files_enabled[(int)param - 8] = val;
    return 0;
}

/* ------------------------------------------------------------------------- */

static const resource_t resources[] = {
    { "FSDevice8ConvertP00", RES_INTEGER, (resource_value_t)1,
      (void *)&fsdevice_convert_p00_enabled[0],
      set_fsdevice_convert_p00, (void *)8 },
    { "FSDevice9ConvertP00", RES_INTEGER, (resource_value_t)1,
      (void *)&fsdevice_convert_p00_enabled[1],
      set_fsdevice_convert_p00, (void *)9 },
    { "FSDevice10ConvertP00", RES_INTEGER, (resource_value_t)1,
      (void *)&fsdevice_convert_p00_enabled[2],
      set_fsdevice_convert_p00, (void *)10 },
    { "FSDevice11ConvertP00", RES_INTEGER, (resource_value_t)1,
      (void *)&fsdevice_convert_p00_enabled[3],
      set_fsdevice_convert_p00, (void *)11 },
    { "FSDevice8Dir", RES_STRING, (resource_value_t)FSDEVICE_DEFAULT_DIR,
      (void *)&fsdevice_dir[0], set_fsdevice_dir, (void *)8 },
    { "FSDevice9Dir", RES_STRING, (resource_value_t)FSDEVICE_DEFAULT_DIR,
      (void *)&fsdevice_dir[1], set_fsdevice_dir, (void *)9 },
    { "FSDevice10Dir", RES_STRING, (resource_value_t)FSDEVICE_DEFAULT_DIR,
      (void *)&fsdevice_dir[2], set_fsdevice_dir, (void *)10 },
    { "FSDevice11Dir", RES_STRING, (resource_value_t)FSDEVICE_DEFAULT_DIR,
      (void *)&fsdevice_dir[3], set_fsdevice_dir, (void *)11 },
    { "FSDevice8SaveP00", RES_INTEGER, (resource_value_t)1,
      (void *)&fsdevice_save_p00_enabled[0],
      set_fsdevice_save_p00, (void *)8 },
    { "FSDevice9SaveP00", RES_INTEGER, (resource_value_t)1,
      (void *)&fsdevice_save_p00_enabled[1],
      set_fsdevice_save_p00, (void *)9 },
    { "FSDevice10SaveP00", RES_INTEGER, (resource_value_t)1,
      (void *)&fsdevice_save_p00_enabled[2],
      set_fsdevice_save_p00, (void *)10 },
    { "FSDevice11SaveP00", RES_INTEGER, (resource_value_t)0,
      (void *)&fsdevice_save_p00_enabled[3],
      set_fsdevice_save_p00, (void *)11 },
    { "FSDevice8HideCBMFiles", RES_INTEGER, (resource_value_t)0,
      (void *)&fsdevice_hide_cbm_files_enabled[0],
      set_fsdevice_hide_cbm_files, (void *)8 },
    { "FSDevice9HideCBMFiles", RES_INTEGER, (resource_value_t)0,
      (void *)&fsdevice_hide_cbm_files_enabled[1],
      set_fsdevice_hide_cbm_files, (void *)9 },
    { "FSDevice10HideCBMFiles", RES_INTEGER, (resource_value_t)0,
      (void *)&fsdevice_hide_cbm_files_enabled[2],
      set_fsdevice_hide_cbm_files, (void *)10 },
    { "FSDevice11HideCBMFiles", RES_INTEGER, (resource_value_t)0,
      (void *)&fsdevice_hide_cbm_files_enabled[3],
      set_fsdevice_hide_cbm_files, (void *)11 },
    { NULL }
};

int fsdevice_resources_init(void)
{
    return resources_register(resources);
}

void fsdevice_resources_shutdown(void)
{
    lib_free(fsdevice_dir[0]);
    lib_free(fsdevice_dir[1]);
    lib_free(fsdevice_dir[2]);
    lib_free(fsdevice_dir[3]);
}

