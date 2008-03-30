/*
 * ui.c - Common UI routines.
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

#include "fullscreenarch.h"
#include "interrupt.h"
#include "ui.h"
#include "uiattach.h"
#include "uicommands.h"
#include "uifliplist.h"
#include "uiromset.h"
#include "uiscreenshot.h"
#include "types.h"
#include "vsync.h"


static int is_paused = 0;

static void pause_trap(WORD addr, void *data)
{
    ui_display_paused(1);
    is_paused = 1;
    vsync_suspend_speed_eval();
    while (is_paused)
        ui_dispatch_next_event();
}

void ui_pause_emulation(int flag)
{
    if (flag) {
        interrupt_maincpu_trigger_trap(pause_trap, 0);
    } else {
        ui_display_paused(0);
        is_paused = 0;
    }
}

int ui_emulation_is_paused(void)
{
    return is_paused;
}

void ui_common_init(void)
{
    uiromset_menu_init();
}

void ui_common_shutdown(void)
{
#ifdef USE_XF86_EXTENSIONS
    fullscreen_shutdown();
#endif

    uiattach_shutdown();
    uicommands_shutdown();
    uifliplist_shutdown();
    uiscreenshot_shutdown();
}

void ui_display_joyport(BYTE *joyport)
{
}

void ui_display_event_time(unsigned int current, unsigned int total)
{
}
