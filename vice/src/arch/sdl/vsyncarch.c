/*
 * vsyncarch.c - End-of-frame handling for SDL
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *
 * Based on code by
 *  Dag Lem <resid@nimrod.no>
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

#include "joy.h"
#include "kbdbuf.h"
#include "lightpendrv.h"
#include "machine.h"
#include "raster.h"
#include "ui.h"
#include "uistatusbar.h"
#include "videoarch.h"
#include "vkbd.h"
#include "vsidui_sdl.h"
#include "vsyncapi.h"

#include "vice_sdl.h"

/* ------------------------------------------------------------------------- */

static int pause_pending = 0;

void vsyncarch_timing_sync(void)
{
}

void vsyncarch_presync(void)
{
    if (sdl_vkbd_state & SDL_VKBD_ACTIVE) {
        while (sdl_vkbd_process(ui_dispatch_events())) {
        }
#ifdef HAVE_SDL_NUMJOYSTICKS
        sdl_vkbd_process(sdljoy_autorepeat());
#endif
    } else {
        ui_dispatch_events();
    }

    if ((sdl_vkbd_state & SDL_VKBD_REPAINT) || (uistatusbar_state & UISTATUSBAR_REPAINT) || (sdl_vsid_state & SDL_VSID_REPAINT)) {
        if (!console_mode) {
            raster_force_repaint(sdl_active_canvas->parent_raster);
        }
        sdl_vkbd_state &= ~SDL_VKBD_REPAINT;
        uistatusbar_state &= ~UISTATUSBAR_REPAINT;
    }

    sdl_lightpen_update();

    if (!console_mode) {
        raster_force_repaint(sdl_active_canvas->parent_raster);
    }
}

void vsyncarch_postsync(void)
{
    /* this function is called once a frame, so this
       handles single frame advance */
    if (pause_pending) {
        ui_pause_enable();
        pause_pending = 0;
    }
}

void vsyncarch_advance_frame(void)
{
    ui_pause_disable();
    pause_pending = 1;
}
