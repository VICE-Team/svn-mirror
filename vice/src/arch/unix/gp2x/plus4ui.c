/*
 * plus4ui.c - Implementation of the Plus4-specific part of the UI.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include <stdio.h>
#include <string.h>

#include "prefs_gp2x.h"
#include "resources.h"
#include "videoarch.h"
#include "vkeyboard_gp2x.h"

/* reserved for future sid cart support */
void plus4ui_handle_sidengine_resource(int value)
{
#if 0
  resources_set_value("SidEngine", (resource_value_t)value);
#endif
}

int plus4ui_handle_X(int Xvalue)
{
  return 0;
}

/* reserved for future sid cart support */
void plus4ui_draw_resid_string(unsigned char *screen, int menu_x, int menu_y)
{
#if 0
  int sidengine;

  resources_get_value("SidEngine", (void *)&sidengine);
  if(sidengine)
  {
    draw_ascii_string(screen, display_width, menu_x, menu_y, "ReSID", menu_fg, menu_bg);
  }
  else
  {
    draw_ascii_string(screen, display_width, menu_x, menu_y, "FastSID", menu_fg, menu_bg);
  }
#endif
}

void plus4ui_draw_memory_string(unsigned char *screen, int menu_x, int menu_y, int mem)
{
}

int plus4ui_set_ramblocks(int value)
{
  return 0;
}

void plus4ui_attach_cart(char *imagefile, int carttype)
{
}

#define PLUS4KB_HEIGHT 9
char *keyb_plus4[]={
	" |||||||||||||||||||||||| ",
	"                          ",
	"  F1 F2 F3 HL             ",
	" esc 1234567890+-=h  del  ",
	" ctrl QWERTYUIOP@x* ctrl  ",
	" rs sh ASDFGHJKL:; rtrn   ",
	" c=  sh ZXCVBNM,./ sh  .  ",
	"          space       ... ",
	"                          ",
	NULL
};

int plus4_keytable[] = {
	2,2, 0,4, /* F1 */
	3,2, 0,4, /* F1 */
	5,2, 0,5, /* F2 */
	6,2, 0,5, /* F2 */
	8,2, 0,6, /* F3 */
	9,2, 0,6, /* F3 */
	11,2, 0,3, /* Help */
	12,2, 0,3, /* Help */

	5,3, 7,0, /* 1 */
	6,3, 7,3, /* 2 */
	7,3, 1,0, /* 3 */
	8,3, 1,3, /* 4 */
	9,3, 2,0, /* 5 */
	10,3, 2,3, /* 6 */
	11,3, 3,0, /* 7 */
	12,3, 3,3, /* 8 */
	13,3, 4,0, /* 9 */
	14,3, 4,3, /* 0 */
	15,3, 6,6, /* + */
	16,3, 5,6, /* - */
	17,3, 6,5, /* = */
	18,3, 7,1, /* clr/home */

	6,4, 7,6, /* q */
	7,4, 1,1, /* w */
	8,4, 1,6, /* e */
	9,4, 2,1, /* r */
	10,4, 2,6, /* t */
	11,4, 3,1, /* y */
	12,4, 3,6, /* u */
	13,4, 4,1, /* i */
	14,4, 4,6, /* o */
	15,4, 5,1, /* p */
	16,4, 0,7, /* @ */
	17,4, 0,2, /* £ */
	18,4, 6,1, /* * */

	7,5, 1,2, /* a */
	8,5, 1,5, /* s */
	9,5, 2,2, /* d */
	10,5, 2,5, /* f */
	11,5, 3,2, /* g */
	12,5, 3,5, /* h */
	13,5, 4,2, /* j */
	14,5, 4,5, /* k */
	15,5, 5,2, /* l */
	16,5, 5,5, /* : */
	17,5, 6,2, /* ; */

	8,6, 1,4, /* z */
	9,6, 2,7, /* x */
	10,6, 2,4, /* c */
	11,6, 3,7, /* v */
	12,6, 3,4, /* b */
	13,6, 4,7, /* n */
	14,6, 4,4, /* m */
	15,6, 5,7, /* , */
	16,6, 5,4, /* . */
	17,6, 6,7, /* / */

	1,3, 6,4, /* esc */
	2,3, 6,4,
	4,3, 6,4,

	1,4, 7,2, /* ctrl ijb mod - not working correctly? c= slows scroll, CTRL no effect on scroll unlike CBM64? */
	2,4, 7,2,
	3,4, 7,2,
	4,4, 7,2,

	1,5, 7,7, /* run/stop */
	2,5, 7,7,

	1,6, 7,5, /* c= */
	2,6, 7,5,

	10,7, 7,4, /* space ijb mod */
	11,7, 7,4, /* space */
	12,7, 7,4, /* space */
	13,7, 7,4, /* space */
	14,7, 7,4, /* space */

	21,3, 0,0, /* del */
	22,3, 0,0,
	23,3, 0,0,

	19,5, 0,1, /* return */
	20,5, 0,1,
	21,5, 0,1,
	22,5, 0,1,

	23,6, 5,3, /* crsr up */
	23,7, 5,0, /* down */
	22,7, 6,0, /* left */
	24,7, 6,3, /* right */

	0
};

void plus4ui_set_keyarr(int status)
{
  keyboard_set_keyarr(1, 7, status);
}

int plus4ui_init(void)
{
  xoffset_centred=0;
  yoffset_centred=40;
  xoffset_uncentred=104;
  yoffset_uncentred=16;
  menu_bg=1;
  menu_fg=123;
  menu_hl=122;
  xoffset=xoffset_centred;
  yoffset=yoffset_centred;

  option_txt[X1]=blank_line;
  option_txt[X2]=blank_line;
  option_txt[X3]=blank_line;
  option_txt[X4]=blank_line;
  option_txt[X5]=blank_line;
  option_txt[X6]=blank_line;
  option_txt[X7]=blank_line;
  option_txt[X8]=blank_line;

  ui_handle_sidengine_resource=plus4ui_handle_sidengine_resource;
  ui_handle_X=plus4ui_handle_X;
  ui_draw_resid_string=plus4ui_draw_resid_string;
  ui_draw_memory_string=plus4ui_draw_memory_string;
  ui_set_ramblocks=plus4ui_set_ramblocks;
  ui_attach_cart=plus4ui_attach_cart;

  keyb=keyb_plus4;
  keytable=plus4_keytable;
  kb_fg=123;
  kb_bg=1;
  cursor_fg=122;
  cursor_bg=122;
  kb_height=PLUS4KB_HEIGHT;
  kb_width=strlen(keyb[0]);
  ui_set_keyarr=plus4ui_set_keyarr;

  machine_ui_done=1;

  return 0;
}

void plus4ui_shutdown(void)
{
}
