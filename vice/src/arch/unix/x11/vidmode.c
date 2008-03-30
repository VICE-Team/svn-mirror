/*
 * vidmode.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>

#include "dga1.h"
#include "lib.h"
#include "log.h"
#include "fullscreenarch.h"
#include "resources.h"
#include "types.h"
#include "uimenu.h"
#include "uisettings.h"
#include "videoarch.h"
#include "vidmode.h"
#include "x11ui.h"


int vm_is_enabled = 0;
int vm_is_suspended = 0;

static log_t vidmode_log = LOG_ERR;
unsigned int vm_mode_count;
static unsigned int vm_index = 0;
static int vm_available = 0;
static int saved_h, saved_w;

XF86VidModeModeInfo **vm_modes;
vm_bestvideomode_t *vm_bestmodes = NULL;


extern int screen;

int vidmode_init(void)
{
    unsigned int hz, i;
    Display *display;

    vidmode_log = log_open("VidMode");

    display = x11ui_get_display_ptr();

    if (!XF86VidModeGetAllModeLines(display, screen, &vm_mode_count,
        &vm_modes)) {
        log_error(vidmode_log, _("Error getting video mode information - disabling vidmode extension."));
        vm_available = 0;
        return -1;
    }

    for (i = 0; i < vm_mode_count; i++) {
        if (vm_modes[i]->hdisplay <= 800 &&
            vm_modes[i]->hdisplay >= 320 &&
            vm_modes[i]->vdisplay <= 600 &&
            vm_modes[i]->vdisplay >= 200) {
            vm_bestmodes = (vm_bestvideomode_t *)lib_realloc(vm_bestmodes,
                           (vm_index + 1) * sizeof(vm_bestvideomode_t));
            vm_bestmodes[vm_index].modeindex = i;

            if(vm_modes[i]->vtotal * vm_modes[i]->htotal) {
                hz = vm_modes[i]->dotclock * 1000
                     / (vm_modes[i]->vtotal * vm_modes[i]->htotal);
            }
            else 
                hz = 0;
	    
            vm_bestmodes[vm_index].name = lib_msprintf(" %ix%i-%iHz",
                                                       vm_modes[i]->hdisplay,
                                                       vm_modes[i]->vdisplay,
                                                       hz);
            if (++vm_index > 29)
                break;
        }
    }

    if (vm_index == 0)
        return 0;

    vm_available = 1;
    return 0;
}

unsigned int vidmode_available_modes(void)
{
    return vm_index;
}

/* FIXME: Limiting vidmode to one active fullscreen window.  */
static video_canvas_t *active_canvas;

int vidmode_enable(struct video_canvas_s *canvas, int enable)
{
    Display *vm_display;

    if(vm_available == 0)
        return 0;

    vm_display = x11ui_get_display_ptr();

    if (enable) {
        int x = 0, y = 0;
        int status_w, status_h;
        extern ui_window_t status_bar;

        XF86VidModeModeInfo *vm;

        log_message(vidmode_log, "Enabling Vidmode with%s",
                    vm_bestmodes[canvas->fullscreenconfig->mode].name);
        vm = vm_modes[vm_bestmodes[canvas->fullscreenconfig->mode].modeindex];

        saved_w = canvas->draw_buffer->canvas_width;
        saved_h = canvas->draw_buffer->canvas_height;

        x11ui_get_widget_size(status_bar, &status_w, &status_h);
        log_message(vidmode_log, "Status bar: %dx%d", status_w, status_h);

        canvas->draw_buffer->canvas_width = vm->hdisplay + 10;
        canvas->draw_buffer->canvas_height = vm->vdisplay - status_h + 10;

        if (canvas->videoconfig->doublesizex)
            canvas->draw_buffer->canvas_width /= 2;
        if (canvas->videoconfig->doublesizey)
            canvas->draw_buffer->canvas_height /= 2;

        video_viewport_resize(canvas);

        x11ui_move_canvas_window(canvas->emuwindow, 0, 0);
        ui_dispatch_events();
        x11ui_canvas_position(canvas->emuwindow, &x, &y);

        XF86VidModeSwitchToMode(vm_display, screen, vm);
        XF86VidModeSetViewPort(vm_display, screen, x + 5, y + 5);
        XWarpPointer(vm_display,
                     None, DefaultRootWindow(vm_display),
                     0, 0, vm->hdisplay, vm->vdisplay,
                     x + vm->hdisplay / 2,
                     y + vm->vdisplay / 2);
        active_canvas = canvas;
        vm_is_enabled = 1;
        vm_is_suspended = 0;
    } else {
        if (!vm_is_enabled)
            return 0;
        vm_is_enabled = 0;
        log_message(vidmode_log, "Disabling Vidmode");
        XF86VidModeSwitchToMode(vm_display, screen, vm_modes[0]);

        canvas->draw_buffer->canvas_width = saved_w;
        canvas->draw_buffer->canvas_height = saved_h;
        video_viewport_resize(canvas);

    }
    return 0;
}

int vidmode_mode(struct video_canvas_s *canvas, int mode)
{
    if (mode < 0 || vm_available == 0)
        return 0;

    log_message(vidmode_log, "Selected mode: %s", vm_bestmodes[mode].name);
    canvas->fullscreenconfig->mode = mode;

    return 0;
}

void vidmode_shutdown(void)
{
    unsigned int i;

    if (vm_is_enabled > 0 && vm_is_suspended == 0) {
        log_message(vidmode_log, "Disabling Vidmode");
        XF86VidModeSwitchToMode(x11ui_get_display_ptr(), screen, vm_modes[0]);
    }

    if (vm_available == 0)
        return;

    for (i = 0; i < vm_index; i++)
        lib_free(vm_bestmodes[i].name);

    lib_free(vm_bestmodes);
}

void vidmode_suspend(int level)
{
    if (vm_is_enabled == 0)
        return;

    if (level > 0)
        return;

    if (vm_is_suspended > 0)
        return;

    vm_is_suspended = 1;
    vidmode_enable(active_canvas, 0);
}

void vidmode_resume(void)
{
   if (vm_is_enabled == 0)
        return;

   if (vm_is_suspended == 0)
      return;

   vm_is_suspended = 0;
   vidmode_enable(active_canvas, 1);
}

void vidmode_set_mouse_timeout(void)
{

}

int vidmode_available(void)
{
    return vm_available;
}

static void *mode_callback;

void vidmode_mode_callback(void *callback)
{
    mode_callback = callback;
}

void vidmode_menu_create(struct ui_menu_entry_s menu[])
{
    unsigned int i, amodes;
    ui_menu_entry_t *resolutions_submenu;

    amodes = vidmode_available_modes();

    resolutions_submenu = (ui_menu_entry_t *)lib_calloc((size_t)(amodes + 1),
                          sizeof(ui_menu_entry_t));

    for (i = 0; i < amodes ; i++) {
        resolutions_submenu[i].string =
            (ui_callback_data_t)lib_msprintf("*%s", vm_bestmodes[i].name);
        resolutions_submenu[i].callback = (ui_callback_t)mode_callback;
        resolutions_submenu[i].callback_data = (ui_callback_data_t)i;
    }

    for (i = 0; menu[i].string; i++) {
        if (strncmp(menu[i].string, "VidMode", 7) == 0) {
            if (amodes > 0)
                menu[i].sub_menu = resolutions_submenu;
            break;
        }
    }
}

void vidmode_menu_shutdown(struct ui_menu_entry_s menu[])
{
    unsigned int i, amodes;
    ui_menu_entry_t *resolutions_submenu = NULL;

    amodes = vidmode_available_modes();

    if (amodes == 0)
        return;

    for (i = 0; menu[i].string; i++) {
        if (strncmp(menu[i].string, "VidMode", 7) == 0) {
            resolutions_submenu = menu[i].sub_menu;
            break;
        }
    }

    menu[i].sub_menu = NULL;

    if (resolutions_submenu != NULL) {
        for (i = 0; i < amodes ; i++)
            lib_free(resolutions_submenu[i].string);
    }

    lib_free(resolutions_submenu);
}

