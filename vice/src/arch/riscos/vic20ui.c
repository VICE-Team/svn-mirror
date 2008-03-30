/*
 * vic20ui.c - Implementation of the VIC20-specific part of the UI.
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

#include <string.h>

#include "ROlib.h"
#include "ui.h"
#include "c64ui.h"
#include "kbd.h"




char *WimpTaskName = "Vice VIC20";


static unsigned char VICkeys[] = {
  0x13, 0x02, 0x05, 0x13,	/* 0 */
  0x02, 0x05, 0x64, 0x02,	/* 4 */
  0x05, 0xff, 0xff, 0xff,	/* 8 */
  0xff, 0xff, 0xff, 0xff,	/* 12 */
  0x06, 0x10, 0x17, 0x20,	/* 16 */
  0x77, 0x37, 0xff, 0x57,	/* 20 */
  0x27, 0x72, 0x27, 0x30,	/* 24 */
  0xff, 0xff, 0xff, 0xff,	/* 28 */
  0xff, 0x11, 0x16, 0x26,	/* 32 */
  0x30, 0x41, 0x40, 0x47,	/* 36 */
  0x57, 0x73, 0x37, 0x40,	/* 40 */
  0x03, 0x01, 0x60, 0x70,	/* 44 */
  0x00, 0x07, 0x22, 0x21,	/* 48 */
  0x27, 0x36, 0x46, 0x51,	/* 52 */
  0x56, 0x73, 0x50, 0x57,	/* 56 */
  0xff, 0x70, 0x67, 0xff,	/* 60 */
  0xff, 0x12, 0x23, 0x25,	/* 64 */
  0x31, 0x42, 0x45, 0x07,	/* 68 */
  0x62, 0x71, 0xff, 0xff,	/* 72 */
  0xff, 0xff, 0xff, 0x55,	/* 76 */
  0xff, 0x15, 0x24, 0x32,	/* 80 */
  0x35, 0x43, 0x52, 0x62,	/* 84 */
  0x61, 0x70, 0xff, 0xff,	/* 88 */
  0xff, 0x65, 0xff, 0xff,	/* 92 */
  0x02, 0x14, 0x04, 0x33,	/* 96 */
  0x34, 0x44, 0x53, 0x54,	/* 100 */
  0x63, 0xff, 0xff, 0xff,	/* 104 */
  0xff, 0xff, 0xff, 0xff,	/* 108 */
  0x03, 0x74, 0x75, 0x76,	/* 112 */
  0xff, 0xff, 0xff, 0xff,	/* 116 */
  0x66, 0x72, 0xff, 0xff,	/* 120 */
  0xff, 0xff, 0xff, 0xff	/* 124 */
};


int vic20_ui_init(void)
{
  kbd_init_keymap(2);
  kbd_add_keymap(VICkeys, 0); kbd_add_keymap(VICkeys, 1);
  return ui_init_named_app("ViceVIC", "!vicevic");
}



/* Dummies */
char *pet_get_keyboard_name(void)
{
  return NULL;
}


int pet_set_model(const char *name, void *extra)
{
  return 0;
}

char *cbm2_get_keyboard_name(void)
{
  return NULL;
}


int cbm2_set_model(const char *name, void *extra)
{
  return 0;
}

CLOCK vic_ii_fetch_clk, vic_ii_draw_clk;

int int_rasterfetch(long offset)
{
  return 0;
}

void cartridge_trigger_freeze(void)
{
}
