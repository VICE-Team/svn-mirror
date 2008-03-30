/*
 * kbd.h - Acorn keyboard driver.
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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

#define KEYMAP_ENTRIES	128

typedef struct keymap_t {
  const char *default_file;
  unsigned char *normal;
  unsigned char *shifted;
  unsigned char *norm_sflag;
  unsigned char *shift_sflag;
} keymap_t;

extern Joy_Keys JoystickKeys[2];
extern int kbd_update_joykeys(int port);

extern void kbd_init_keymap(int number);
extern int  kbd_add_keymap(keymap_t *map, int number);
extern int  kbd_default_keymap(keymap_t *map);
extern int kbd_load_keymap(const char *filename, int number);
extern int kbd_dump_keymap(const char *filename, int number);

extern int kbd_init(void);
extern const char *kbd_intkey_to_string(int intkey);

extern void kbd_poll(void);

#endif

