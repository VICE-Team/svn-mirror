/*
 * fullscreen.h
 *
 * Written by
 *  Oliver Schaertel <orschaer@forwiss.uni-erlangen.de>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#ifndef _FULLSCREEN_H
#define _FULLSCREEN_H

extern void fullscreen_mode_init(void);

extern int fullscreen_mode_on(void);
extern int fullscreen_mode_off(void);

extern int fullscreen_available(void);
extern int fullscreen_vidmode_available(void);
extern int fullscreen_available_modes(void);
extern char *fullscreen_mode_name(int mode);

extern void fullscreen_focus_window_again(void);
extern void fullscreen_set_mouse_timeout(void);

#endif

