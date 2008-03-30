/*
 * c128ui.c - Implementation of the C128-specific part of the UI.
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

void c128ui_handle_sidengine_resource(int value)
{
  resources_set_int("SidEngine", value);
}

int c128ui_handle_X(int Xvalue)
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

void c128ui_draw_resid_string(unsigned char *screen, int menu_x, int menu_y)
{
  int sidengine;

  resources_get_int("SidEngine", &sidengine);
  if(sidengine)
  {
    draw_ascii_string(screen, display_width, menu_x, menu_y, "ReSID", menu_fg, menu_bg);
  }
  else
  {
    draw_ascii_string(screen, display_width, menu_x, menu_y, "FastSID", menu_fg, menu_bg);
  }
}

void c128ui_draw_memory_string(unsigned char *screen, int menu_x, int menu_y, int mem)
{
}

int c128ui_set_ramblocks(int value)
{
  return 0;
}

void c128ui_attach_cart(char *imagefile, int carttype)
{
  cartridge_attach_image(0, imagefile);
}

#define C128KB_HEIGHT 10
char *keyb_c128[]={
	" ============================= ",
	"                       ~~~~~~  ",
	" esc taa hldn ^v<> f1 f3 f5 f7 ",
	"                               ",
	"    x 1234567890+-xh  del 789+ ",
	"  ctrl QWERTYUIOP@*x rstr 456- ",
	" r/s sh ASDFGHJKL:;= rtrn 123  ",
	" c=  sh  ZXCVBNM,./ sh    0 .  ",
	"          space                ",
	"                               ",
	NULL
};

int c128_keytable[] = {
	1,2, 7,7, /* esc */
	2,2, 7,7,
	3,2, 7,7,
	5,2, 7,2, /* tab */

	6,2, 0,0, /* alt FIXME */
	7,2, -4,1, /* ascii/din */

	9,2, 0,0, /* help FIXME */
	10,2,0,0, /* line feed FIXME */
	11,2, -4,0, /* 40/80 display */
	12,2, 0,0, /* no scroll FIXME */

	14,2, 10,3, /* up */
	15,2, 10,4, /* down */
	16,2, 10,5, /* left */
	17,2, 10,6, /* right */
	23,8, 10,3, /* up */
	23,8, 10,4, /* down */
	22,8, 10,5, /* left */
	24,8, 10,6, /* right */

	19,2, 0,4, /* f1 FIXME? */
	20,2, 0,4, 
	22,2, 0,5, /* f3 */
	23,2, 0,5,
	25,2, 0,6, /* f5 */
	26,2, 0,6,
	28,2, 0,3, /* f7 */
	29,2, 0,3,

	4,4, 0,0, /* <- FIXME */
	6,4, 7,0, /* 1 */
	7,4, 7,3, /* 2 */
	8,4, 1,0, /* 3 */
	9,4, 1,3, /* 4 */
	10,4, 2,0, /* 5 */
	11,4, 2,3, /* 6 */
	12,4, 3,0, /* 7 */
	13,4, 3,3, /* 8 */
	14,4, 4,0, /* 9 */
	15,4, 4,3, /* 0 */
	16,4, 5,0, /* + */
	17,4, 5,3, /* - */
	18,4, 6,0, /* £ */
	19,4, 6,3, /* clr/home */

	7,5, 7,6, /* q */
	8,5, 1,1, /* w */
	9,5, 1,6, /* e */
	10,5, 2,1, /* r */
	11,5, 2,6, /* t */
	12,5, 3,1, /* y */
	13,5, 3,6, /* u */
	14,5, 4,1, /* i */
	15,5, 4,6, /* o */
	16,5, 5,1, /* p */
	17,5, 5,6, /* @ */
	18,5, 6,1, /* * */
	19,5, 6,6, /* up arrow */

	8,6, 1,2, /* a */
	9,6, 1,5, /* s */
	10,6, 2,2, /* d */
	11,6, 2,5, /* f */
	12,6, 3,2, /* g */
	13,6, 3,5, /* h */
	14,6, 4,2, /* j */
	15,6, 4,5, /* k */
	16,6, 5,2, /* l */
	17,6, 5,5, /* : */
	18,6, 6,2, /* ; */
	19,6, 6,5, /* = */

	9,7, 1,4, /* z */
	10,7, 2,7, /* x */
	11,7, 2,4, /* c */
	12,7, 3,7, /* v */
	13,7, 3,4, /* b */
	14,7, 4,7, /* n */
	15,7, 4,4, /* m */
	16,7, 5,7, /* , */
	17,7, 5,4, /* . */
	18,7, 6,7, /* / */

	21,4, 0,0, /* del */
	22,4, 0,0,
	23,4, 0,0,

	2,5, 7,5, /* ctrl */
	3,5, 7,5, 
	4,5, 7,5,
	5,5, 7,5,

	1,6, 0,0, /* run/stop FIXME */
	2,60, 0,0,
	3,60, 0,0,

	1,7, 0,0, /* c= FIXME */
	2,7, 0,0,

	20,5, -3,0, /* rstr */
	21,5, -3,0,
	22,5, -3,0,
	23,5, -3,0,

	21,6, 0,1, /* rtrn */
	22,6, 0,1,
	23,6, 0,1,
	24,6, 0,1,

	10,8, 7,4, /* space */
	11,8, 7,4,
	12,8, 7,4,
	13,8, 7,4,
	14,8, 7,4,

	26,7, 10,1, /* kp0 */
	26,6, 8,7, /* kp1 */
	27,6, 8,4, /* kp2 */
	28,6, 9,7, /* kp3 */
	26,5, 8,5, /* kp4 */
	27,5, 8,2, /* kp5 */
	28,5, 9,5, /* kp6 */
	26,4, 8,6, /* kp7 */
	27,4, 8,1, /* kp8 */
	28,4, 9,6, /* kp9 */
	29,4, 9,1, /* kp+ */
	29,5, 9,2, /* kp- */
	28,7, 10,2, /* kp. */

	0
};

void c128ui_set_keyarr(int status)
{
  keyboard_set_keyarr(1, 7, status);
}

int c128ui_init(void)
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

  ui_handle_sidengine_resource=c128ui_handle_sidengine_resource;
  ui_handle_X=c128ui_handle_X;
  ui_draw_resid_string=c128ui_draw_resid_string;
  ui_draw_memory_string=c128ui_draw_memory_string;
  ui_set_ramblocks=c128ui_set_ramblocks;
  ui_attach_cart=c128ui_attach_cart;

  keyb=keyb_c128;
  keytable=c128_keytable;
  kb_fg=1;
  kb_bg=12;
  cursor_fg=7;
  cursor_bg=7;
  kb_height=C128KB_HEIGHT;
  kb_width=strlen(keyb[0]);
  ui_set_keyarr=c128ui_set_keyarr;

  machine_ui_done=1;

  return 0;
}

void c128ui_shutdown(void)
{
}
