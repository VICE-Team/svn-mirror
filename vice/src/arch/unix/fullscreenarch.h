/*
 * fullscreenarch.h
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

#ifndef _FULLSCREENARCH_H
#define _FULLSCREENARCH_H

struct fullscreenconfig_s {
    const char *device;
};
typedef struct fullscreenconfig_s fullscreenconfig_t;


struct ui_menu_entry_s;

extern int fullscreen_is_enabled;
extern int fullscreen_available(void);

extern void fullscreen_shutdown(void);
extern void fullscreen_suspend(int level);
extern void fullscreen_resume(void);
extern void fullscreen_set_mouse_timeout(void);
extern void fullscreen_mode_callback(const char *device, void *callback);
extern void fullscreen_create_menus(struct ui_menu_entry_s menu[]);
extern int fullscreen_init(void);

#endif

