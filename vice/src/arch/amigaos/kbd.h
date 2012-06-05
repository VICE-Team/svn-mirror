/*
 * kbd.h
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

#ifndef VICE_KBD_H
#define VICE_KBD_H

#include "types.h"

extern void kbd_arch_init(void);

extern signed long kbd_arch_keyname_to_keynum(char *keyname);
extern const char *kbd_arch_keynum_to_keyname(signed long keynum);
extern void kbd_initialize_numpad_joykeys(int *joykeys);

#define KBD_C64_SYM_US  "amiga_sym.vkm"
#define KBD_C64_SYM_DE  "amiga_sym.vkm"
#define KBD_C64_POS     "amiga_pos.vkm"
#define KBD_C128_SYM    "amiga_sym.vkm"
#define KBD_C128_POS    "amiga_pos.vkm"
#define KBD_VIC20_SYM   "amiga_sym.vkm"
#define KBD_VIC20_POS   "amiga_pos.vkm"
#define KBD_PET_SYM_UK  "amiga_buks.vkm"
#define KBD_PET_POS_UK  "amiga_bukp.vkm"
#define KBD_PET_SYM_DE  "amiga_bdes.vkm"
#define KBD_PET_POS_DE  "amiga_bdep.vkm"
#define KBD_PET_SYM_GR  "amiga_bgrs.vkm"
#define KBD_PET_POS_GR  "amiga_bgrp.vkm"
#define KBD_PLUS4_SYM   "amiga_sym.vkm"
#define KBD_PLUS4_POS   "amiga_pos.vkm"
#define KBD_CBM2_SYM_UK "amiga_buks.vkm"
#define KBD_CBM2_POS_UK "amiga_bukp.vkm"
#define KBD_CBM2_SYM_DE "amiga_bdes.vkm"
#define KBD_CBM2_POS_DE "amiga_bdep.vkm"
#define KBD_CBM2_SYM_GR "amiga_bgrs.vkm"
#define KBD_CBM2_POS_GR "amiga_bgrp.vkm"

#define KBD_INDEX_C64_DEFAULT   KBD_INDEX_C64_POS
#define KBD_INDEX_C128_DEFAULT  KBD_INDEX_C128_POS
#define KBD_INDEX_VIC20_DEFAULT KBD_INDEX_VIC20_POS
#define KBD_INDEX_PET_DEFAULT   KBD_INDEX_PET_BUKP
#define KBD_INDEX_PLUS4_DEFAULT KBD_INDEX_PLUS4_POS
#define KBD_INDEX_CBM2_DEFAULT  KBD_INDEX_CBM2_BUKP

#endif