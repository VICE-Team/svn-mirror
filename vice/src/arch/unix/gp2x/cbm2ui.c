/*
 * cbm2ui.c - Implementation of the CBM-II specific part of the UI.
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

void cbm2ui_handle_sidengine_resource(int value)
{
  resources_set_value("SidEngine", (resource_value_t)value);
}

int cbm2ui_handle_X(int Xvalue)
{
  return 0;
}

void cbm2ui_draw_resid_string(unsigned char *screen, int menu_x, int menu_y)
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

void cbm2ui_draw_memory_string(unsigned char *screen, int menu_x, int menu_y, int mem)
{
}

int cbm2ui_set_ramblocks(int value)
{
  return 0;
}

void cbm2ui_attach_cart(char *imagefile, int carttype)
{
}

#define CBM2KB_HEIGHT 10
char *keyb_cbm2[]={
	"                       ",
	" xxxxxxxxxx xxxx  xxxx ",
	"                       ",
	" x1234567890xxxx  xxxx ",
	"  xqwertyuiopxxx  789x ",
	"   xasdfghjkl;'r  456x ",
	"    xzxcvbnm,./x  123x ",
	"     x  space     0xxx ",
	"                       ",
	NULL
};

int cbm2_keytable[] = {

	2,3, 9,1, /* 1 */
	3,3, 10,1, /* 2 */
	4,3, 11,1, /* 3 */
	5,3, 12,1, /* 4 */
	6,3, 13,1, /* 5 */
	7,3, 13,2, /* 6 */
	8,3, 14,1, /* 7 */
	9,3, 15,1, /* 8 */
	10,3, 0,1, /* 9 */
	11,3, 1,1, /* 0 */

	3,4, 9,2, /* q */
	4,4, 10,2, /* w */
	5,4, 11,2, /* e */
	6,4, 12,2, /* r */
	7,4, 12,3, /* t */
	8,4, 13,3, /* y */
	9,4, 14,2, /* u */
	10,4, 15,2, /* i */
	11,4, 0,2, /* o */
	12,4, 1,3, /* p */

	4,5, 9,3, /* a */
	5,5, 10,3, /* s */
	6,5, 11,3, /* d */
	7,5, 11,4, /* f */
	8,5, 12,3, /* g */
	9,5, 13,4, /* h */
	10,5, 14,3, /* j */
	11,5, 15,3, /* k */
	12,5, 0,3, /* l */
	13,5, 0,4, /* ; */
	14,5, 0,0, /* FIXME */
	15,5, 2,4, /* return */
	
	
	5,6, 9,4, /* z */
	6,6, 10,4, /* x */
	7,6, 10,5, /* c */
	8,6, 11,5, /* v */
	9,6, 12,5, /* b */
	10,6, 13,5, /* n */
	11,6, 14,4, /* m */
	12,6, 15,4, /* , */
	13,6, 15,5, /* . */
	14,6, 0,5, /* / */

	8,7, 14,5, /* space */
	9,7, 14,5,
	10,7, 14,5,
	11,7, 14,5,
	12,7, 14,5,

	18,7, 4,5, /* kp0 */
	18,6, 4,4, /* 1 */
	19,6, 5,4, /* 2 */
	20,6, 6,4, /* 3 */
	18,5, 4,3, /* 4 */
	19,5, 5,3, /* 5 */
	20,5, 6,3, /* 6 */
	18,4, 4,2, /* 7 */
	19,4, 5,2, /* 8 */
	20,4, 6,2, /* 9 */

	0
};

void cbm2ui_set_keyarr(int status)
{
  keyboard_set_keyarr(8, 4, status);
}

int cbm2ui_init(void)
{
  xoffset_centred=0;
  yoffset_centred=0;
  xoffset_uncentred=104;
  yoffset_uncentred=16;
  menu_bg=1;
  menu_fg=0;
  menu_hl=2;
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

  ui_handle_sidengine_resource=cbm2ui_handle_sidengine_resource;
  ui_handle_X=cbm2ui_handle_X;
  ui_draw_resid_string=cbm2ui_draw_resid_string;
  ui_draw_memory_string=cbm2ui_draw_memory_string;
  ui_set_ramblocks=cbm2ui_set_ramblocks;
  ui_attach_cart=cbm2ui_attach_cart;

  keyb=keyb_cbm2;
  keytable=cbm2_keytable;
  kb_fg=0;
  kb_bg=1;
  cursor_fg=1;
  cursor_bg=0;
  kb_height=CBM2KB_HEIGHT;
  kb_width=strlen(keyb[0]);
  ui_set_keyarr=cbm2ui_set_keyarr;

  machine_ui_done=1;

  return 0;
}

void cbm2ui_shutdown(void)
{
}
