/*
 * c64ui.c - Implementation of the C64-specific part of the UI.
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

void c64ui_handle_sidengine_resource(int value)
{
  resources_set_value("SidEngine", (resource_value_t)value);
}

int c64ui_handle_X(int Xvalue)
{
  if (Xvalue==1)
    return 1;
  if (Xvalue==2)
  {
    cartridge_detach_image();
    return 0;
  }
  if (Xvalue==3)
  {
    cartridge_trigger_freeze();
    return 0;
  }
  return 0;
}

void c64ui_draw_resid_string(unsigned char *screen, int menu_x, int menu_y)
{
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
}

void c64ui_draw_memory_string(unsigned char *screen, int menu_x, int menu_y, int mem)
{
}

int c64ui_set_ramblocks(int value)
{
  return 0;
}

void c64ui_attach_cart(char *imagefile, int carttype)
{
  cartridge_attach_image(0, imagefile);
}

#define C64KB_HEIGHT 8
char *keyb_c64vic[]={
	" =---=====================*= ",
	"                             ",
	"    \x1f 1234567890+-\x1ch del  F1 ",
	"  ctrl QWERTYUIOP@*\x1e rstr F3 ",
	"  r/s   ASDFGHJKL:;= rtrn F5 ",
	"  c=     ZXCVBNM,./v>     F7 ",
	"          space              ",
	"                             ",
	NULL
};

int c64_keytable[] = {
		/* x, y, row,column */
		4, 2, 7,1, /* <- */
		6, 2, 7,0, /* 1 */
		7, 2, 7,3, /* 2 */
		8, 2, 1,0, /* 3 */
		9, 2, 1,3, /* 4 */
		10, 2, 2,0, /* 5 */
		11, 2, 2,3, /* 6 */
		12, 2, 3,0, /* 7 */
		13, 2, 3,3, /* 8 */
		14, 2, 4,0, /* 9 */
		15, 2, 4,3, /* 0 */
		16, 2, 5,0, /* + */
		17, 2, 5,3, /* - */
		18, 2, 6,0, /* £ */
		19, 2, 6,3, /* c/h */

		7, 3, 7,6, /* q */
		8, 3, 1,1, /* w */
		9, 3, 1,6, /* e */
		10, 3, 2,1, /* r */
		11, 3, 2,6, /* t */
		12, 3, 3,1, /* y */
		13, 3, 3,6, /* u */
		14, 3, 4,1, /* i */
		15, 3, 4,6, /* o */
		16, 3, 5,1, /* p */
		17, 3, 5,6, /* @ */
		18, 3, 6,1, /* * */
		19, 3, 6,6, /* up arrow */

		2,3, 7,2, /* ctrl ijb mod */
		3,3, 7,2, /* ctrl ijb mod */
		4,3, 7,2, /* ctrl ijb mod */
		5,3, 7,2, /* ctrl ijb mod */

		8, 4, 1,2, /* a */
		9, 4, 1,5, /* s */
		10, 4, 2,2, /* d */
		11, 4, 2,5, /* f */
		12, 4, 3,2, /* g */
		13, 4, 3,5, /* h */
		14, 4, 4,2, /* j */
		15, 4, 4,5, /* k */
		16, 4, 5,2, /* l */
		17, 4, 5,5, /* : */
		18, 4, 6,2, /* ; */
		19, 4, 6,5, /* = */

		2, 5, 7,5, /* c= */
		3, 5, 7,5, /* c= */
		9, 5, 1,4, /* z */
		10, 5, 2,7, /* x */
		11, 5, 2,4, /* c */
		12, 5, 3,7, /* v */
		13, 5, 3,4, /* b */
		14, 5, 4,7, /* n */
		15, 5, 4,4, /* m */
		16, 5, 5,7, /* , */
		17, 5, 5,4, /* . */
		18, 5, 6,7, /* / */
		19, 5, 0,7, /* down */
		20, 5, 0,2, /* right */

		2, 4, 7,7, /* run/stop */
		3, 4, 7,7, /* */
		4, 4, 7,7, /* */

		10,6, 7,4, /* space */
		11,6, 7,4, /* space */
		12,6, 7,4, /* space */
		13,6, 7,4, /* space */
		14,6, 7,4, /* space */

		21, 4, 0,1, /* return */
		22, 4, 0,1, /* return */
		23, 4, 0,1, /* return */
		24, 4, 0,1, /* return */

#if 0
		23, 5, 0|128,7, /* crsr up */
		23, 6, 0,7, /* crsr down */
		22, 6, 0|128,2, /* crsr left */
		24, 6, 0,2, /* crsr right */
#endif

		21, 2, 0,0, /* delete */
		22, 2, 0,0, /* delete */
		23, 2, 0,0, /* delete */

		26, 2, 0,4, /* f1 */
		27, 2, 0,4, /* f1 */
		26, 3, 0,5, /* f3 */
		27, 3, 0,5, /* f3 */
		26, 4, 0,6, /* f5 */
		27, 4, 0,6, /* f5 */
		26, 5, 0,3, /* f7 */
		27, 5, 0,3, /* f7 */

		0 
};

void c64ui_set_keyarr(int status)
{
  keyboard_set_keyarr(1, 7, status);
}

int c64ui_init(void)
{
  xoffset_centred=104+32;
  yoffset_centred=16+16;
  xoffset_uncentred=104;
  yoffset_uncentred=16;
  menu_bg=C64_WHITE;
  menu_fg=C64_BLACK;
  menu_hl=C64_YELLOW;
  xoffset=xoffset_centred;
  yoffset=yoffset_centred;

  /* ATTACH_CART */
  option_txt[X1]=        "Attach cartridge...              ";
  /* DETACH_CART */
  option_txt[X2]=        "Detach cartridge                 ";
  /* FREEZE_CART */
  option_txt[X3]=        "Cartridge freeze                 ";
  option_txt[X4]=blank_line;
  option_txt[X5]=blank_line;
  option_txt[X6]=blank_line;
  option_txt[X7]=blank_line;
  option_txt[X8]=blank_line;

  ui_handle_sidengine_resource=c64ui_handle_sidengine_resource;
  ui_handle_X=c64ui_handle_X;
  ui_draw_resid_string=c64ui_draw_resid_string;
  ui_draw_memory_string=c64ui_draw_memory_string;
  ui_set_ramblocks=c64ui_set_ramblocks;
  ui_attach_cart=c64ui_attach_cart;

  keyb=keyb_c64vic;
  keytable=c64_keytable;
  kb_fg=1;
  kb_bg=9;
  cursor_fg=7;
  cursor_bg=7;
  kb_height=C64KB_HEIGHT;
  kb_width=strlen(keyb[0]);
  ui_set_keyarr=c64ui_set_keyarr;

  machine_ui_done=1;

  return 0;
}

void c64ui_shutdown(void)
{
}
