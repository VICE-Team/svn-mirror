/*
 * vic20ui.c - Implementation of the VIC20-specific part of the UI.
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

#include "cartridge.h"
#include "prefs_gp2x.h"
#include "resources.h"
#include "videoarch.h"
#include "vkeyboard_gp2x.h"

/* reserved for future sid cart support */
void vic20ui_handle_sidengine_resource(int value)
{
#if 0
  resources_set_value("SidEngine", (resource_value_t)value);
#endif
}

int vic20ui_handle_X(int Xvalue)
{
  if (Xvalue==7)
  {
    cartridge_detach_image();
    return 1;
  }
  return 1;
}

int vic20ui_set_ramblocks(int value)
{
  resources_set_value("RamBlock0", (resource_value_t)value);
  resources_set_value("RamBlock1", (resource_value_t)value);
  resources_set_value("RamBlock2", (resource_value_t)value);
  resources_set_value("RamBlock3", (resource_value_t)value);
  resources_set_value("RamBlock5", (resource_value_t)value);

  return 1;
}

/* reserved for future sid cart support */
void vic20ui_draw_resid_string(unsigned char *screen, int menu_x, int menu_y)
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

void vic20ui_draw_memory_string(unsigned char *screen, int menu_x, int menu_y, int mem)
{
  char *mem_str;

  if (mem==0) mem_str="none";
  else if(mem==5) mem_str="all";
  draw_ascii_string(screen, display_width, menu_x, menu_y, mem_str, menu_fg, menu_bg);
}

void vic20ui_attach_cart(char *imagefile, int carttype)
{
  if (carttype==0)
    cartridge_attach_image(CARTRIDGE_VIC20_DETECT, imagefile);
  if (carttype==2)
    cartridge_attach_image(CARTRIDGE_VIC20_16KB_2000, imagefile);
  if (carttype==4)
    cartridge_attach_image(CARTRIDGE_VIC20_16KB_4000, imagefile);
  if (carttype==6)
    cartridge_attach_image(CARTRIDGE_VIC20_16KB_6000, imagefile);
  if (carttype==0xa)
    cartridge_attach_image(CARTRIDGE_VIC20_8KB_A000, imagefile);
  if (carttype==0xb)
    cartridge_attach_image(CARTRIDGE_VIC20_4KB_B000, imagefile);
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

int vic20_keytable[] = {
		4, 2, 0,1, /* <- */
		6, 2, 0,0, /* 1 */
		7, 2, 0,7, /* 2 */
		8, 2, 1,0, /* 3 */
		9, 2, 1,7, /* 4 */
		10, 2, 2,0, /* 5 */
		11, 2, 2,7, /* 6 */
		12, 2, 3,0, /* 7 */
		13, 2, 3,7, /* 8 */
		14, 2, 4,0, /* 9 */
		15, 2, 4,7, /* 0 */
		16, 2, 5,0, /* + */
		17, 2, 5,7, /* - */
		18, 2, 6,0, /* £ */
		19, 2, 6,7, /* c/h */

		7, 3, 0,6, /* q */
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

		2,3, 0,2, /* ctrl ijb mod */
		3,3, 0,2, /* ctrl ijb mod */
		4,3, 0,2, /* ctrl ijb mod */
		5,3, 0,2, /* ctrl ijb mod */

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

		2, 5, 0,5, /* c= FIXME */
		3, 5, 0,5, /* c= FIXME */
		9, 5, 1,4, /* z */
		10, 5, 2,3, /* x */
		11, 5, 2,4, /* c */
		12, 5, 3,3, /* v */
		13, 5, 3,4, /* b */
		14, 5, 4,3, /* n */
		15, 5, 4,4, /* m */
		16, 5, 5,3, /* , */
		17, 5, 5,4, /* . */
		18, 5, 6,3, /* / */
		19, 5, 7,3, /* down */
		20, 5, 7,2, /* right */

		3, 4, 7,7, /* run/stop FIXME */
		3, 4, 7,7, /* */
		4, 4, 7,7, /* */

		10,6, 0,4, /* space */
		11,6, 0,4, /* space */
		12,6, 0,4, /* space */
		13,6, 0,4, /* space */
		14,6, 0,4, /* space */

		21, 4, 7,1, /* return */
		22, 4, 7,1, /* return */
		23, 4, 7,1, /* return */
		24, 4, 7,1, /* return */

#if 0
		23, 5, 7|128,3, /* crsr up */
		23, 6, 7,3, /* crsr down */
		22, 6, 7|128,2, /* crsr left */
		24, 6, 7,2, /* crsr right */
#endif

		21, 2, 7,0, /* delete */
		22, 2, 7,0, /* delete */
		23, 2, 7,0, /* delete */

		26, 2, 7,4, /* f1 */
		27, 2, 7,4, /* f1 */
		26, 3, 7,5, /* f3 */
		27, 3, 7,5, /* f3 */
		26, 4, 7,6, /* f5 */
		27, 4, 7,6, /* f5 */
		26, 5, 7,7, /* f7 */
		27, 5, 7,7, /* f7 */
		0
};

void vic20ui_set_keyarr(int status)
{
  keyboard_set_keyarr(1, 3, status);
}

int vic20ui_init(void)
{
  xoffset_centred=0;
  yoffset_centred=48;
  xoffset_uncentred=584;
  yoffset_uncentred=32;
  menu_bg=C64_WHITE;
  menu_fg=C64_BLACK;
  menu_hl=C64_YELLOW;
  xoffset=xoffset_centred;
  yoffset=yoffset_centred;

  option_txt[X1]=        "Smart-attach cartridge...        ";
  option_txt[X2]=        "Attach cartridge $2000...        ";
  option_txt[X3]=        "Attach cartridge $4000...        ";
  option_txt[X4]=        "Attach cartridge $6000...        ";
  option_txt[X5]=        "Attach cartridge $A000...        ";
  option_txt[X6]=        "Attach cartridge $B000...        ";
  option_txt[X7]=        "Detach cartridge                 ";
  option_txt[X8]=        "Memory expansions                ";

  ui_handle_sidengine_resource=vic20ui_handle_sidengine_resource;
  ui_handle_X=vic20ui_handle_X;
  ui_draw_resid_string=vic20ui_draw_resid_string;
  ui_draw_memory_string=vic20ui_draw_memory_string;
  ui_set_ramblocks=vic20ui_set_ramblocks;
  ui_attach_cart=vic20ui_attach_cart;

  keyb=keyb_c64vic;
  keytable=vic20_keytable;
  kb_fg=1;
  kb_bg=0;
  cursor_fg=7;
  cursor_bg=7;
  kb_height=C64KB_HEIGHT;
  kb_width=strlen(keyb[0]);
  ui_set_keyarr=vic20ui_set_keyarr;

  machine_ui_done=1;

  return 0;
}

void vic20ui_shutdown(void)
{
}
