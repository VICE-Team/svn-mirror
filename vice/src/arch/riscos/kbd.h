/*
 * kbd.h - Acorn keyboard driver.
 *
 * Written by
 *  Andreas Dehmel <zarquon@t-online.de>
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

#ifndef _KBD_RO_H
#define _KBD_RO_H

#include "vice.h"

#include "types.h"

extern void kbd_arch_init(void);

extern signed long kbd_arch_keyname_to_keynum(char *keyname);
extern const char *kbd_arch_keynum_to_keyname(signed long keynum);

#define KBD_C64_SYM_US "ros_sym/vkm"
#define KBD_C64_SYM_DE "ros_sym/vkm"
#define KBD_C64_POS    "ros_pos/vkm"
#define KBD_C128_SYM   "ros_sym/vkm"
#define KBD_C128_POS   "ros_pos/vkm"
#define KBD_VIC20_SYM  "ros_sym/vkm"
#define KBD_VIC20_POS  "ros_pos/vkm"
#define KBD_PET_SYM_UK "ros_buks/vkm"
#define KBD_PET_POS_UK "ros_bukp/vkm"
#define KBD_PET_SYM_DE "ros_bdes/vkm"
#define KBD_PET_POS_DE "ros_bdep/vkm"
#define KBD_PET_SYM_GR "ros_bgrs/vkm"
#define KBD_PET_POS_GR "ros_bgrp/vkm"
#define KBD_PLUS4_SYM  "ros_sym/vkm"
#define KBD_PLUS4_POS  "ros_pos/vkm"
#define KBD_CBM2_SYM_UK  "ros_buks/vkm"
#define KBD_CBM2_POS_UK  "ros_bukp/vkm"
#define KBD_CBM2_SYM_DE  "ros_bdes/vkm"
#define KBD_CBM2_POS_DE  "ros_bdep/vkm"
#define KBD_CBM2_SYM_GR  "ros_bgrs/vkm"
#define KBD_CBM2_POS_GR  "ros_bgrp/vkm"

#define KBD_INDEX_C64_DEFAULT   KBD_INDEX_C64_POS
#define KBD_INDEX_C128_DEFAULT  KBD_INDEX_C128_POS
#define KBD_INDEX_VIC20_DEFAULT KBD_INDEX_VIC20_POS
#define KBD_INDEX_PET_DEFAULT   KBD_INDEX_PET_BUKP
#define KBD_INDEX_PLUS4_DEFAULT KBD_INDEX_PLUS4_POS
#define KBD_INDEX_CBM2_DEFAULT  KBD_INDEX_CBM2_BUKP

#define KEYMAP_ENTRIES	128

typedef struct kbd_keymap_s {
  const char *default_file;
  unsigned char *normal;
  unsigned char *shifted;
  unsigned char *norm_sflag;
  unsigned char *shift_sflag;
} kbd_keymap_t;

extern Joy_Keys JoystickKeys[2];
extern int kbd_update_joykeys(int port);

extern void kbd_init_keymap(int number);
extern int  kbd_add_keymap(kbd_keymap_t *map, int number);
extern int  kbd_default_keymap(kbd_keymap_t *map);
extern int kbd_load_keymap(const char *filename, int number);
extern int kbd_dump_keymap(const char *filename, int number);

extern int kbd_init(void);
extern const char *kbd_intkey_to_string(int intkey);

extern void kbd_poll(void);

#endif

