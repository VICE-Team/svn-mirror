/*
 * uiapi.c
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui.h"
#include "cmdline.h"
#include "lib.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "uilib.h"
#include "util.h"
#include "fullscreenarch.h"

/* UI-related resources.  */

struct {
    int fullscreenbitdepth;
    int fullscreenwidth;
    int fullscreenheight;
    int fullscreenenabled;
    int save_resources_on_exit;
    int confirm_on_exit;
    char *monitor_dimensions;
    char *initialdir[UILIB_SELECTOR_STYLES_NUM];
} ui_resources;

static int set_fullscreen_bitdepth(resource_value_t v, void *param)
{
    ui_resources.fullscreenbitdepth = (int)v;
    return 0;
}

static int set_fullscreen_width(resource_value_t v, void *param)
{
    ui_resources.fullscreenwidth = (int)v;
    return 0;
}

static int set_fullscreen_height(resource_value_t v, void *param)
{
    ui_resources.fullscreenheight = (int)v;
    return 0;
}

static int set_fullscreen_enabled(resource_value_t v, void *param)
{
    ui_resources.fullscreenenabled = (int)v;

    video_arch_fullscreen_toggle(ui_resources.fullscreenenabled);

    return 0;
}

static int set_save_resources_on_exit(resource_value_t v, void *param)
{
    ui_resources.save_resources_on_exit = (int)v;
    return 0;
}

static int set_confirm_on_exit(resource_value_t v, void *param)
{
    ui_resources.confirm_on_exit = (int)v;
    return 0;
}

static int set_monitor_dimensions(resource_value_t v, void *param)
{
    const char *name = (const char *)v;
    if (ui_resources.monitor_dimensions != NULL && name != NULL)
        if (strcmp(name, ui_resources.monitor_dimensions) == 0)
            return 0;
    util_string_set(&ui_resources.monitor_dimensions, name ? name : "");
    return 0;
}

static int set_initial_dir(resource_value_t v, void *param)
{
    const char *name = (const char *)v;
    int index = (int)param;
    if (ui_resources.initialdir[index] != NULL && name != NULL)
        if (strcmp(name, ui_resources.initialdir[index]) == 0)
            return 0;
    util_string_set(&ui_resources.initialdir[index], name ? name : "");
    return 0;
}

static const resource_t resources[] = {
    { "FullscreenBitdepth", RES_INTEGER, (resource_value_t)8,
      RES_EVENT_NO, NULL,
      (void *)&ui_resources.fullscreenbitdepth, set_fullscreen_bitdepth, NULL },
    { "FullscreenWidth", RES_INTEGER, (resource_value_t)640,
      RES_EVENT_NO, NULL,
      (void *)&ui_resources.fullscreenwidth, set_fullscreen_width, NULL },
    { "FullscreenHeight", RES_INTEGER, (resource_value_t)480,
      RES_EVENT_NO, NULL,
      (void *)&ui_resources.fullscreenheight, set_fullscreen_height, NULL },
    { "FullscreenEnabled", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&ui_resources.fullscreenenabled, set_fullscreen_enabled, NULL },
    { "SaveResourcesOnExit", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&ui_resources.save_resources_on_exit,
      set_save_resources_on_exit, NULL },
    { "ConfirmOnExit", RES_INTEGER, (resource_value_t)1,
      RES_EVENT_NO, NULL,
      (void *)&ui_resources.confirm_on_exit, set_confirm_on_exit, NULL },
    { "MonitorDimensions", RES_STRING, (resource_value_t)"",
      RES_EVENT_NO, NULL,
      (void *)&ui_resources.monitor_dimensions, set_monitor_dimensions, NULL },
    { "InitialDefaultDir", RES_STRING, (resource_value_t)"",
      RES_EVENT_NO, NULL,
      (void *)&ui_resources.initialdir[0], set_initial_dir, (void *)0 },
    { "InitialTapeDir", RES_STRING, (resource_value_t)"",
      RES_EVENT_NO, NULL,
      (void *)&ui_resources.initialdir[1], set_initial_dir, (void *)1 },
    { "InitialDiskDir", RES_STRING, (resource_value_t)"",
      RES_EVENT_NO, NULL,
      (void *)&ui_resources.initialdir[2], set_initial_dir, (void *)2 },
    { "InitialAutostartDir", RES_STRING, (resource_value_t)"",
      RES_EVENT_NO, NULL,
      (void *)&ui_resources.initialdir[3], set_initial_dir, (void *)3 },
    { "InitialCartDir", RES_STRING, (resource_value_t)"",
      RES_EVENT_NO, NULL,
      (void *)&ui_resources.initialdir[4], set_initial_dir, (void *)4 },
    { "InitialSnapshotDir", RES_STRING, (resource_value_t)"",
      RES_EVENT_NO, NULL,
      (void *)&ui_resources.initialdir[5], set_initial_dir, (void *)5 },
    { NULL }
};

int ui_resources_init(void)
{
    return resources_register(resources);
}

void ui_resources_shutdown(void)
{
    int i;

    if (ui_resources.monitor_dimensions != NULL)
        lib_free(ui_resources.monitor_dimensions);

    for (i = 0; i < UILIB_SELECTOR_STYLES_NUM; i++)
        if (ui_resources.initialdir[i] != NULL)
            lib_free(ui_resources.initialdir[i]);
}

/* ------------------------------------------------------------------------ */

/* UI-related command-line options.  */

static const cmdline_option_t cmdline_options[] = {
    { "-saveres", SET_RESOURCE, 0, NULL, NULL,
      "SaveResourcesOnExit", (resource_value_t)1,
      NULL, "Save settings (resources) on exit" },
    { "+saveres", SET_RESOURCE, 0, NULL, NULL,
      "SaveResourcesOnExit", (resource_value_t)0,
      NULL, "Never save settings (resources) on exit" },
    { "-confirmexit", SET_RESOURCE, 0, NULL, NULL,
      "ConfirmOnExit", (resource_value_t)0,
      NULL, "Confirm quiting VICE" },
    { "+confirmexit", SET_RESOURCE, 0, NULL, NULL,
      "ConfirmOnExit", (resource_value_t)1,
      NULL, "Never confirm quiting VICE" },
    { NULL }
};

int ui_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

int ui_init(int *argc, char **argv)
{
  return 0;
}

int ui_init_finish(void)
{
  return 0;
}

int ui_init_finalize(void)
{
  return 0;
}

void ui_shutdown(void)
{
}

/* Print an error message.  */
void ui_error(const char *format,...)
{
  va_list ap;

  va_start(ap, format);
  vfprintf(stderr, format, ap);
}

/* Show a CPU JAM dialog.  */
ui_jam_action_t ui_jam_dialog(const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  vfprintf(stderr, format, ap);

  return UI_JAM_RESET;
}

/* Update all menu entries.  */
void ui_update_menus(void)
{
}

/* Recording UI */
void ui_display_playback(int playback_status, char *version)
{
}

void ui_display_recording(int recording_status)
{
}

void ui_display_event_time(unsigned int current, unsigned int total)
{
}

/* joystick UI */
void ui_display_joyport(BYTE *joyport)
{
}

void ui_display_statustext(const char *text, int fade_out)
{
  printf("%s\n", text);
}
