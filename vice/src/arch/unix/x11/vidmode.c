/*
 * vidmode.c
 *
 * Written by
 *  Martin Pottendorfer <pottendo@utanet.at>
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

/*
*/
#define FS_VIDMODE_DEBUG

#include "vice.h"

#ifdef USE_XF86_VIDMODE_EXT
#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>
#include <string.h>

#include "log.h"
#include "fullscreen-common.h"
#include "resources.h"
#include "types.h"
#include "uimenu.h"
#include "uisettings.h"
#include "utils.h"
#include "videoarch.h"
#include "vidmode.h"
#include "x11ui.h"
#include "raster/raster.h"


UI_MENU_DEFINE_RADIO(SelectedFullscreenMode);

int vm_selected_videomode = -1;
int vm_is_enabled = 0;

/* ---------------------------------------------------------------*/
static log_t vidmode_log = LOG_ERR;
static int vm_mode_count;
static unsigned int vm_index;
static XF86VidModeModeInfo **vm_modes;
static int vm_available = 0;
static Display *vm_display;
static int vm_screen;
static int saved_h, saved_w;

typedef struct {
    int modeindex;
    char *name;
} vm_bestvideomode_t;

static vm_bestvideomode_t *vm_bestmodes = NULL;


static unsigned int vidmode_available_modes(void)
{
#ifdef FS_VIDMODE_DEBUG
    unsigned int i;
    for (i = 0; i < vm_index; i++)
        log_message(vidmode_log, "found mode: %s", vm_bestmodes[i].name);
#endif
    return vm_index;
}

static char *vidmode_mode_name(unsigned int i)
{
    return vm_bestmodes[i].name;
}

/* ---------------------------------------------------------------*/
int vidmode_init(Display *display, int screen)
{
    unsigned int hz, i;

    vidmode_log = log_open("VidMode");

    if (!XF86VidModeGetAllModeLines(display, screen, &vm_mode_count,
        &vm_modes)) {
        log_error(vidmode_log, _("Error getting video mode information - disabling vidmode extension."));
        vm_available = 0;
        return 1;
    }

    for (i = 0; i < vm_mode_count; i++) {
        if (vm_modes[i]->hdisplay <= 800 &&
            vm_modes[i]->hdisplay >= 320 &&
            vm_modes[i]->vdisplay <= 600 &&
            vm_modes[i]->vdisplay >= 200) {
            vm_bestmodes = (vm_bestvideomode_t *)xrealloc(vm_bestmodes,
                           (vm_index + 1) * sizeof(vm_bestvideomode_t));
            vm_bestmodes[vm_index].modeindex = i;
            hz = vm_modes[i]->dotclock * 1000 /
                 (vm_modes[i]->vtotal * vm_modes[i]->htotal);
            vm_bestmodes[vm_index].name = xmsprintf(" %ix%i-%iHz",
                                                    vm_modes[i]->hdisplay,
                                                    vm_modes[i]->vdisplay, hz);
            if (++vm_index > 29)
                break;
        }
    }

    if (vm_mode_count == 0)
        return 0;
    resources_set_value("SelectedFullscreenMode", (resource_value_t)0);
    vm_selected_videomode = 0;
    vm_display = display;
    vm_screen = screen;
    vm_available = 1;
    (void)vidmode_available_modes();
    return 0;
}

int vidmode_set_bestmode(resource_value_t v, void *param)
{
    int i = (int)v;

    if (i < 0 || vm_available == 0)
        return 0;

    log_message(vidmode_log, "selected mode: %s", vm_bestmodes[i].name);
    vm_selected_videomode = i;

    return 0;
}

int vidmode_set_mode(resource_value_t v, void *param)
{
    if (v) {
        int x = 0, y = 0;
        int status_w, status_h;
        extern ui_window_t status_bar;

        XF86VidModeModeInfo *vm;

        log_message(vidmode_log, "Enabling Vidmode with%s",
                    vm_bestmodes[vm_selected_videomode].name);
        vm = vm_modes[vm_bestmodes[vm_selected_videomode].modeindex];

        vm_is_enabled = 1;
        saved_w = fs_cached_raster->canvas->draw_buffer->canvas_width;
        saved_h = fs_cached_raster->canvas->draw_buffer->canvas_height;

        x11ui_get_widget_size(status_bar, &status_w, &status_h);
        log_message(vidmode_log, "Status bar: %dx%d", status_w, status_h);

        fs_cached_raster->canvas->draw_buffer->canvas_width
            = vm->hdisplay + 10;
        fs_cached_raster->canvas->draw_buffer->canvas_height
            = vm->vdisplay-status_h + 10;
        video_viewport_resize(fs_cached_raster->canvas);

        x11ui_move_canvas_window(fs_cached_raster->canvas->emuwindow, 0, 0);
        ui_dispatch_events();
        x11ui_canvas_position(fs_cached_raster->canvas->emuwindow, &x, &y);

        XF86VidModeSwitchToMode(vm_display, vm_screen, vm);
        XF86VidModeSetViewPort(vm_display, vm_screen, x + 5, y + 5);
        XWarpPointer(vm_display,
                     None, DefaultRootWindow(vm_display),
                     0, 0, vm->hdisplay, vm->vdisplay,
                     x + vm->hdisplay / 2,
                     y + vm->vdisplay / 2);

    } else {
        if (!vm_is_enabled)
            return 0;
        vm_is_enabled = 0;
        log_message(vidmode_log, "Disabling Vidmode");
        XF86VidModeSwitchToMode(vm_display, vm_screen, vm_modes[0]);

        fs_cached_raster->canvas->draw_buffer->canvas_width = saved_w;
        fs_cached_raster->canvas->draw_buffer->canvas_height = saved_h;
        video_viewport_resize(fs_cached_raster->canvas);

    }
    return 0;
}

int vidmode_available(void)
{
    return vm_available;
}

void vidmode_create_menus(void)
{
    unsigned int i, amodes;
    ui_menu_entry_t *resolutions_submenu;

    amodes = vidmode_available_modes();

    resolutions_submenu = (ui_menu_entry_t *)xcalloc((size_t)(amodes + 1),
                          sizeof(ui_menu_entry_t));

    for (i = 0; i < amodes ; i++) {
        resolutions_submenu[i].string =
            (ui_callback_data_t)xmsprintf("*%s", vidmode_mode_name(i));
        resolutions_submenu[i].callback =
            (ui_callback_t)radio_SelectedFullscreenMode;
        resolutions_submenu[i].callback_data = (ui_callback_data_t)i;
    }

    for (i = 0; ui_fullscreen_settings_submenu[i].string; i++) {
        if (strncmp(ui_fullscreen_settings_submenu[i].string,
            "VidMode", 7) == 0) {
            if (amodes > 0)
                ui_fullscreen_settings_submenu[i].sub_menu
                    = resolutions_submenu;
            break;
        }
    }
}

#endif

