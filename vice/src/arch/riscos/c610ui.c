/*
 * c610ui.c - Implementation of the CBM-II-specific part of the UI.
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

#include "ROlib.h"
#include "ui.h"
#include "c610ui.h"
#include "kbd.h"
#include "resources.h"



char *WimpTaskName = "Vice CBM-II";


static unsigned char CBM2keys[] = {
  0x84, 0x85, 0xff, 0x84,	/* 0 */
  0x85, 0xff, 0x83, 0x41,	/* 4 */
  0xff, 0xff, 0xff, 0xff,	/* 8 */
  0xff, 0xff, 0xff, 0xff,	/* 12 */
  0x92, 0xb1, 0xc1, 0xd1,	/* 16 */
  0xb0, 0xf1, 0xe0, 0x12,	/* 20 */
  0xd2, 0x31, 0x63, 0x42,	/* 24 */
  0xff, 0xff, 0x10, 0xff,	/* 28 */
  0xff, 0xa2, 0xb2, 0xc3,	/* 32 */
  0xe1, 0xf2, 0x01, 0x11,	/* 36 */
  0x12, 0x20, 0x52, 0x62,	/* 40 */
  0x70, 0x25, 0xff, 0x33,	/* 44 */
  0x91, 0xa1, 0xb3, 0xc2,	/* 48 */
  0xd2, 0xe2, 0x02, 0x13,	/* 52 */
  0xa1, 0x30, 0x73, 0x72,	/* 56 */
  0x74, 0x33, 0x40, 0xff,	/* 60 */
  0xff, 0x93, 0xa4, 0xb4,	/* 64 */
  0xd3, 0xe3, 0xf3, 0xa1,	/* 68 */
  0x04, 0x24, 0x71, 0xff,	/* 72 */
  0x55, 0xff, 0xff, 0x15,	/* 76 */
  0xff, 0xa3, 0xa5, 0xc4,	/* 80 */
  0xd4, 0xd5, 0x03, 0x04,	/* 84 */
  0xf1, 0x33, 0x55, 0x61,	/* 88 */
  0xff, 0x21, 0xff, 0xff,	/* 92 */
  0x82, 0x94, 0xe5, 0xb5,	/* 96 */
  0xc5, 0xe4, 0xf4, 0xf5,	/* 100 */
  0x05, 0xff, 0x45, 0x44,	/* 104 */
  0x64, 0xff, 0xff, 0xff,	/* 108 */
  0x70, 0x80, 0x90, 0xa0,	/* 112 */
  0xc0, 0xd0, 0xf0, 0x00,	/* 116 */
  0x22, 0x32, 0x43, 0x53,	/* 120 */
  0x54, 0xff, 0xff, 0xff
};



/* CBM2 keyboard names */
static char CBM2keyBusinessName[] = "Business";
static char CBM2keyGraphicName[] = "Graphic";

char *cbm2_get_keyboard_name(void)
{
  resource_value_t val;
  int idx;

  if (resources_get_value("KeymapIndex", &val) != 0) val = 0;
  idx = (int)val;
  if ((idx & 2) == 0) return CBM2keyBusinessName;
  return CBM2keyGraphicName;
}

int c610_ui_init(void)
{
  CBM2ModelName = "610";

  kbd_init_keymap(2);
  kbd_add_keymap(CBM2keys, 0); kbd_add_keymap(CBM2keys, 1);

  return ui_init_named_app("ViceCBM2", "!vicecbm2");
}



/* FIXME; should be defined in c610mem */
int *mem_read_limit_tab_ptr;


char *pet_get_keyboard_name(void)
{
  return NULL;
}


int pet_set_model(const char *name, void *extra)
{
  return 0;
}
