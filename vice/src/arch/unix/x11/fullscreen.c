/*
 * fullscreen.c
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

#include "fullscreen.h"
#include "fullscreenarch.h"
#include "video.h"
#include "videoarch.h"
#include "utils.h"

#ifdef USE_XF86_EXTENSIONS

#define STR_VIDMODE "Vidmode"
#define STR_DGA1    "DGA1"
#define STR_DGA2    "DGA2"

#ifdef USE_XF86_VIDMODE_EXT
#include "vidmode.h"
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
#include "dga1.h"
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
#include "dga2.h"
#endif

int fullscreen_is_enabled;

int fullscreen_available(void) 
{
#ifdef USE_XF86_VIDMODE_EXT
    if (vidmode_available())
        return 1;
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    if (dga1_available())
        return 1;
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    if (dga2_available())
	return 1;
#endif    
    return 0;
}

void fullscreen_shutdown(void)
{
#ifdef USE_XF86_VIDMODE_EXT
    vidmode_shutdown();
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    dga2_shutdown();
#endif
}

void fullscreen_suspend(int level)
{
#ifdef USE_XF86_VIDMODE_EXT
    vidmode_suspend(level);
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    dga1_suspend(level);
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    dga2_suspend(level);
#endif
}

void fullscreen_resume(void)
{
#ifdef USE_XF86_VIDMODE_EXT
    vidmode_resume();
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    dga1_resume();
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    dga2_resume();
#endif
}

void fullscreen_set_mouse_timeout(void)
{
#ifdef USE_XF86_VIDMODE_EXT
    vidmode_set_mouse_timeout();
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    dga2_set_mouse_timeout();
#endif
}

void fullscreen_mode_callback(const char *device, void *callback)
{
#ifdef USE_XF86_VIDMODE_EXT
    if (strcmp(STR_VIDMODE, device) == 0)
        vidmode_mode_callback(callback);
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    if (strcmp(STR_DGA1, device) == 0)
        dga1_mode_callback(callback);
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    if (strcmp(STR_DGA2, device) == 0)
        dga2_mode_callback(callback);
#endif
}

void fullscreen_create_menus(struct ui_menu_entry_s menu[])
{
#ifdef USE_XF86_VIDMODE_EXT
    vidmode_create_menus(menu);
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    dga1_create_menus(menu);
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    dga2_create_menus(menu);
#endif
}

int fullscreen_init(void)
{
#ifdef USE_XF86_VIDMODE_EXT
    if (vidmode_init() < 0)
        return -1;
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    if (dga1_init() < 0)
        return -1;
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    if (dga2_init() < 0)
        return -1;
#endif
    return 0;
}

int fullscreen_init_alloc_hooks(struct video_canvas_s *canvas)
{
#ifdef USE_XF86_DGA2_EXTENSIONS
    if (dga2_init_alloc_hooks(canvas) < 0)
        return -1;
#endif
    return 0;
}

void fullscreen_shutdown_alloc_hooks(struct video_canvas_s *canvas)
{
#ifdef USE_XF86_DGA2_EXTENSIONS
    if (dga2_shutdown_alloc_hooks(canvas) < 0)
        return -1;
#endif
}

static int fullscreen_enable(struct video_canvas_s *canvas, int enable)
{
    if (canvas->fullscreenconfig->device == NULL)
	return 0;
    
#ifdef USE_XF86_VIDMODE_EXT
    if (strcmp(STR_VIDMODE, canvas->fullscreenconfig->device) == 0)
        if (vidmode_enable(canvas, enable) < 0)
            return -1;
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    if (strcmp(STR_DGA1, canvas->fullscreenconfig->device) == 0)
        if (dga1_enable(canvas, enable) < 0)
            return -1;
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    if (strcmp(STR_DGA2, canvas->fullscreenconfig->device) == 0)
        if (dga2_enable(canvas, enable) < 0)
            return -1;
#endif
    fullscreen_is_enabled = canvas->fullscreenconfig->enable = enable;
    return 0;
}

static int fullscreen_double_size(struct video_canvas_s *canvas,
                                  int double_size)
{
    canvas->fullscreenconfig->double_size = double_size;
    return 0;
}

static int fullscreen_double_scan(struct video_canvas_s *canvas,
                                  int double_scan)
{
    canvas->fullscreenconfig->double_scan = double_scan;
    return 0;
}

static int fullscreen_device(struct video_canvas_s *canvas, const char *device)
{
    while (1) {
#ifdef USE_XF86_VIDMODE_EXT
    if (strcmp(STR_VIDMODE, device) == 0)
        break;
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    if (strcmp(STR_DGA1, device) == 0)
        break;
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    if (strcmp(STR_DGA2, device) == 0)
        break;
#endif
        return -1;
    }

    if (canvas->fullscreenconfig->device)
	free (canvas->fullscreenconfig->device);
    
    canvas->fullscreenconfig->device = stralloc(device);

    return 0;
}

#ifdef USE_XF86_VIDMODE_EXT
static int fullscreen_mode_vidmode(struct video_canvas_s *canvas, int mode)
{
    vidmode_mode(canvas, mode);
    return 0;
}
#endif

#ifdef USE_XF86_DGA1_EXTENSIONS
static int fullscreen_mode_dga1(struct video_canvas_s *canvas, int mode)
{
    dga1_mode(canvas, mode);
    return 0;
}
#endif

#ifdef USE_XF86_DGA2_EXTENSIONS
static int fullscreen_mode_dga2(struct video_canvas_s *canvas, int mode)
{
    dga2_mode(canvas, mode);
    return 0;
}
#endif

#endif

void fullscreen_capability(cap_fullscreen_t *cap_fullscreen)
{
    cap_fullscreen->device_num = 0;

#ifdef USE_XF86_VIDMODE_EXT
    cap_fullscreen->device_name[cap_fullscreen->device_num] = STR_VIDMODE;
    cap_fullscreen->enable = fullscreen_enable;
    cap_fullscreen->double_size = fullscreen_double_size;
    cap_fullscreen->double_scan = fullscreen_double_scan;
    cap_fullscreen->device = fullscreen_device;
    cap_fullscreen->mode[cap_fullscreen->device_num] = fullscreen_mode_vidmode;
    cap_fullscreen->device_num += 1;
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    cap_fullscreen->device_name[cap_fullscreen->device_num] = STR_DGA1;
    cap_fullscreen->enable = fullscreen_enable;
    cap_fullscreen->double_size = fullscreen_double_size;
    cap_fullscreen->double_scan = fullscreen_double_scan;
    cap_fullscreen->device = fullscreen_device;
    cap_fullscreen->mode[cap_fullscreen->device_num] = fullscreen_mode_dga1;
    cap_fullscreen->device_num += 1;
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    cap_fullscreen->device_name[cap_fullscreen->device_num] = STR_DGA2;
    cap_fullscreen->enable = fullscreen_enable;
    cap_fullscreen->double_size = fullscreen_double_size;
    cap_fullscreen->double_scan = fullscreen_double_scan;
    cap_fullscreen->device = fullscreen_device;
    cap_fullscreen->mode[cap_fullscreen->device_num] = fullscreen_mode_dga2;
    cap_fullscreen->device_num += 1;
#endif
}

