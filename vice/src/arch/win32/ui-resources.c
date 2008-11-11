/*
 * uiresources.c - Windows resources.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Tibor Biczo <crown@mail.matav.hu>
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
#include "lib.h"
#include "res.h"
#include "resources.h"
#include "translate.h"
#include "types.h"
#include "ui.h"
#include "uilib.h"
#include "util.h"

/* UI-related resources.  */

struct {
    int fullscreendevice;
    int fullscreenbitdepth;
    int fullscreenwidth;
    int fullscreenheight;
    int fullscreenrefreshrate;
    int fullscreenenabled;
    int save_resources_on_exit;
    int confirm_on_exit;
    int single_cpu;
    int vblank_sync;
    int window_xpos[2];
    int window_ypos[2];
    char *monitor_dimensions;
    char *initialdir[UILIB_SELECTOR_STYLES_NUM];
    int alwaysontop;
} ui_resources;

static int set_fullscreen_device(int val, void *param)
{
    ui_resources.fullscreendevice = val;
    return 0;
}

static int set_fullscreen_bitdepth(int val, void *param)
{
    ui_resources.fullscreenbitdepth = val;
    return 0;
}

static int set_fullscreen_width(int val, void *param)
{
    ui_resources.fullscreenwidth = val;
    return 0;
}

static int set_fullscreen_height(int val, void *param)
{
    ui_resources.fullscreenheight = val;
    return 0;
}

static int set_fullscreen_refreshrate(int val, void *param)
{
    ui_resources.fullscreenrefreshrate = val;
    return 0;
}

static int set_fullscreen_enabled(int val, void *param)
{
    ui_resources.fullscreenenabled = val;
    return 0;
}

static int set_save_resources_on_exit(int val, void *param)
{
    ui_resources.save_resources_on_exit = val;
    return 0;
}

static int set_confirm_on_exit(int val, void *param)
{
    ui_resources.confirm_on_exit = val;
    return 0;
}

static int set_single_cpu(int val, void *param)
{
    DWORD process_affinity;
    DWORD system_affinity;

    ui_resources.single_cpu = (int)val;

    if (GetProcessAffinityMask(GetCurrentProcess(), &process_affinity,
        &system_affinity)) {
        /* Check if multi CPU system or not */
        if ((system_affinity & (system_affinity - 1))) {
            if (ui_resources.single_cpu == 1) {
                //  Set it to first CPU
                SetThreadAffinityMask(GetCurrentThread(),system_affinity ^ (system_affinity & (system_affinity - 1)));
            } else {
                //  Set it to all CPU
                SetThreadAffinityMask(GetCurrentThread(),system_affinity);
            }
        }
    }
    return 0;
}

static int set_monitor_dimensions(const char *name, void *param)
{
    if (ui_resources.monitor_dimensions != NULL && name != NULL)
        if (strcmp(name, ui_resources.monitor_dimensions) == 0)
            return 0;

    util_string_set(&ui_resources.monitor_dimensions, name ? name : "");

    return 0;
}

static int set_initial_dir(const char *name, void *param)
{
    int index = (int)param;

    if (ui_resources.initialdir[index] != NULL && name != NULL)
        if (strcmp(name, ui_resources.initialdir[index]) == 0)
            return 0;

    util_string_set(&ui_resources.initialdir[index], name ? name : "");

    return 0;
}

static int set_window_xpos(int val, void *param)
{
    ui_resources.window_xpos[(int) param] = val;
    return 0;
}

static int set_window_ypos(int val, void *param)
{
    ui_resources.window_ypos[(int) param] = val;
    return 0;
}

static int set_vblank_sync(int val, void *param)
{
    ui_resources.vblank_sync = val;
    return 0;
}

static int set_alwaysontop(int val, void *param)
{
    ui_resources.alwaysontop = val;
    ui_set_alwaysontop(ui_resources.alwaysontop);
    return 0;
}

static const resource_string_t resources_string[] = {
    { "MonitorDimensions", "", RES_EVENT_NO, NULL,
      &ui_resources.monitor_dimensions, set_monitor_dimensions, NULL },
    { "InitialDefaultDir", "", RES_EVENT_NO, NULL,
      &ui_resources.initialdir[0], set_initial_dir, (void *)0 },
    { "InitialTapeDir", "", RES_EVENT_NO, NULL,
      &ui_resources.initialdir[1], set_initial_dir, (void *)1 },
    { "InitialDiskDir", "", RES_EVENT_NO, NULL,
      &ui_resources.initialdir[2], set_initial_dir, (void *)2 },
    { "InitialAutostartDir", "", RES_EVENT_NO, NULL,
      &ui_resources.initialdir[3], set_initial_dir, (void *)3 },
    { "InitialCartDir", "", RES_EVENT_NO, NULL,
      &ui_resources.initialdir[4], set_initial_dir, (void *)4 },
    { "InitialSnapshotDir", "", RES_EVENT_NO, NULL,
      &ui_resources.initialdir[5], set_initial_dir, (void *)5 },
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "FullscreenDevice", 0, RES_EVENT_NO, NULL,
      &ui_resources.fullscreendevice, set_fullscreen_device, NULL },
    { "FullscreenBitdepth", 8, RES_EVENT_NO, NULL,
      &ui_resources.fullscreenbitdepth, set_fullscreen_bitdepth, NULL },
    { "FullscreenWidth", 640, RES_EVENT_NO, NULL,
      &ui_resources.fullscreenwidth, set_fullscreen_width, NULL },
    { "FullscreenHeight", 480, RES_EVENT_NO, NULL,
      &ui_resources.fullscreenheight, set_fullscreen_height, NULL },
    { "FullscreenRefreshRate", 0, RES_EVENT_NO, NULL,
      &ui_resources.fullscreenrefreshrate, set_fullscreen_refreshrate, NULL },
    { "FullscreenEnabled", 0, RES_EVENT_NO, NULL,
      &ui_resources.fullscreenenabled, set_fullscreen_enabled, NULL },
    { "SaveResourcesOnExit", 0, RES_EVENT_NO, NULL,
      &ui_resources.save_resources_on_exit, set_save_resources_on_exit, NULL },
    { "ConfirmOnExit", 1, RES_EVENT_NO, NULL,
      &ui_resources.confirm_on_exit, set_confirm_on_exit, NULL },
    { "SingleCPU", 0, RES_EVENT_NO, NULL,
      &ui_resources.single_cpu, set_single_cpu, NULL },
    { "Window0Xpos", CW_USEDEFAULT, RES_EVENT_NO, NULL,
      &ui_resources.window_xpos[0], set_window_xpos, (void *)0 },
    { "Window0Ypos", CW_USEDEFAULT, RES_EVENT_NO, NULL,
      &ui_resources.window_ypos[0], set_window_ypos, (void *)0 },
    { "Window1Xpos", CW_USEDEFAULT, RES_EVENT_NO, NULL,
      &ui_resources.window_xpos[1], set_window_xpos, (void *)1 },
    { "Window1Ypos", CW_USEDEFAULT, RES_EVENT_NO, NULL,
      &ui_resources.window_ypos[1], set_window_ypos, (void *)1 },
    { "VBLANKSync", 0, RES_EVENT_NO, NULL,
      &ui_resources.vblank_sync, set_vblank_sync, NULL },
    { "AlwaysOnTop", 0, RES_EVENT_NO, NULL,
      &ui_resources.alwaysontop, set_alwaysontop, NULL },
    { NULL }
};

int ui_resources_init(void)
{
    translate_resources_init();

    if (resources_register_string(resources_string) < 0)
        return -1;

    return resources_register_int(resources_int);
}

void ui_resources_shutdown(void)
{
    int i;

    translate_resources_shutdown();
    if (ui_resources.monitor_dimensions != NULL)
        lib_free(ui_resources.monitor_dimensions);

    for (i = 0; i < UILIB_SELECTOR_STYLES_NUM; i++)
        if (ui_resources.initialdir[i] != NULL)
            lib_free(ui_resources.initialdir[i]);
}

int ui_vblank_sync_enabled()
{
    return ui_resources.vblank_sync;
}

/* ------------------------------------------------------------------------ */

/* UI-related command-line options.  */

static const cmdline_option_t cmdline_options[] = {
    { "-saveres", SET_RESOURCE, 0,
      NULL, NULL, "SaveResourcesOnExit", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDS_SAVE_SETTINGS_ON_EXIT,
      NULL, NULL },
    { "+saveres", SET_RESOURCE, 0,
      NULL, NULL, "SaveResourcesOnExit", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDS_NO_SAVE_SETTINGS_ON_EXIT,
      NULL, NULL },
    { "-confirmexit", SET_RESOURCE, 0,
      NULL, NULL, "ConfirmOnExit", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDS_CONFIRM_QUIT_VICE,
      NULL, NULL },
    { "+confirmexit", SET_RESOURCE, 0,
      NULL, NULL, "ConfirmOnExit", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDS_NO_CONFIRM_QUIT_VICE,
      NULL, NULL },
    { "-singlecpu", SET_RESOURCE, 0,
      NULL, NULL, "SingleCPU", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDS_USE_ALL_CPU_SMP,
      NULL, NULL },
    { "+singlecpu", SET_RESOURCE, 0,
      NULL, NULL, "SingleCPU", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDS_USE_FIRST_CPU_SMP,
      NULL, NULL },
    { NULL }
};

int ui_cmdline_options_init(void)
{
    translate_cmdline_options_init();
    return cmdline_register_options(cmdline_options);
}
