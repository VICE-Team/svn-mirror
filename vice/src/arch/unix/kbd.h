/*
 * kbd.h - Unix specfic keyboard driver.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README file for copyright notice.
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

#ifndef _KBD_H
#define _KBD_H

#include "ui.h"

extern void kbd_arch_init(void);

extern signed long kbd_arch_keyname_to_keynum(char *keyname);
extern const char *kbd_arch_keynum_to_keyname(signed long keynum);

#define KBD_C64_SYM   "x11_sym.vkm"
#define KBD_C64_POS   "x11_pos.vkm"
#define KBD_C128_SYM  "x11_sym.vkm"
#define KBD_C128_POS  "x11_pos.vkm"
#define KBD_VIC20_SYM "x11_sym.vkm"
#define KBD_VIC20_POS "x11_pos.vkm"
#define KBD_PET_BUKS  "x11_buks.vkm"
#define KBD_PET_BUKP  "x11_bukp.vkm"
#define KBD_PET_BDES  "x11_bdes.vkm"
#define KBD_PET_BDEP  "x11_bdep.vkm"
#define KBD_PET_BGRS  "x11_bgrs.vkm"
#define KBD_PET_BGRP  "x11_bgrp.vkm"
#define KBD_PLUS4_SYM "x11_sym.vkm"
#define KBD_PLUS4_POS "x11_pos.vkm"
#define KBD_C610_BUKS "x11_buks.vkm"
#define KBD_C610_BUKP "x11_bukp.vkm"
#define KBD_C610_BDES "x11_bdes.vkm"
#define KBD_C610_BDEP "x11_bdep.vkm"
#define KBD_C610_BGRS "x11_bgrs.vkm"
#define KBD_C610_BGRP "x11_bgrp.vkm"

extern void x11kbd_press(ui_keysym_t key);
extern void x11kbd_release(ui_keysym_t key);
extern void x11kbd_enter_leave(void);
extern void x11kbd_focus_change(void);

#endif

