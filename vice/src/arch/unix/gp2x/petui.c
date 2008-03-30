/*
 * petui.c - Implementation of the PET-specific part of the UI.
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
void petui_handle_sidengine_resource(int value)
{
#if 0
  resources_set_int("SidEngine", value);
#endif
}

int petui_handle_X(int Xvalue)
{
  return 0;
}

/* reserved for future sid cart support */
void petui_draw_resid_string(unsigned char *screen, int menu_x, int menu_y)
{
#if 0
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
#endif
}

void petui_draw_memory_string(unsigned char *screen, int menu_x, int menu_y, int mem)
{
}

int petui_set_ramblocks(int value)
{
  return 0;
}

void petui_attach_cart(char *imagefile, int carttype)
{
}

#define PETKB_HEIGHT 7
char *keyb_pet[]={
	"                   ",
	" !@#$%&'()\\x  c-|d ",
	" QWERTYUIOPx  789/ ",
	" ASDFGHJKL:r  456* ",
	" ZXCVBNM,;?r  123+ ",
	" so@[]spV>rs  0.-= ",
	"                   ",
	NULL
};

int pet_keytable[] = {

	1,1, 1,0, /* ! ijb actually numbers when unshifted */
	2,1, 0,0, /* " ijb mod */
	3,1, 9,1, /* # */
	4,1, 1,1, /* $ */
	5,1, 0,1, /* % */
	6,1, 9,2, /* & ijb mod */
	7,1, 1,2, /* ' ijb mod */
	8,1, 0,2, /* ( */
	9,1, 9,3, /* ) */
	10,1, 4,4, /* backslash */

	11,1, 0,0, /* x FIXME */

	14,1, 0,0, /* FIXME */
	15,1, 0,0, /* */
	16,1, 0,0, /* */
	17,1, 0,0, /* */

	1,2, 5,0, /* q */
	2,2, 4,1, /* w */
	3,2, 5,1, /* e */
	4,2, 4,2, /* r */
	5,2, 5,2, /* t */
	6,2, 4,3, /* y */
	7,2, 5,3, /* u */
	8,2, 4,5, /* i */
	9,2, 5,5, /* o */
	10,2, 4,6, /* p */
	11,2, 8,1, /* x */

	14,2, 1,2, /* 7 */
	15,2, 0,2, /* 8 */
	16,2, 9,3, /* 9 */
	17,2, 8,6, /* / */

	1,3, 3,0, /* a */
	2,3, 2,1, /* s */
	3,3, 3,1, /* d */
	4,3, 2,2, /* f */
	5,3, 3,2, /* g */
	6,3, 2,3, /* h */
	7,3, 3,3, /* j */
	8,3, 2,5, /* k */
	9,3, 3,5, /* l */
	10,3, 9,5, /* : */
	11,3, 3,4, /* return */

	14,3, 1,1, /* 4 */
	15,3, 0,1, /* 5 */
	16,3, 9,2, /* 6 */
	17,3, 9,5, /* * ijb actually colon needs to be shifted */

	1,4, 7,0, /* z */
	2,4, 8,1, /* x */
	3,4, 6,1, /* c */
	4,4, 7,1, /* v */
	5,4, 6,2, /* b */
	6,4, 7,2, /* n */
	7,4, 8,3, /* m */
	8,4, 7,3, /* , */
	9,4, 2,6, /* ; */
	10,4, 8,6, /* ? */
	11,4, 3,4, /* return */

	14,4, 1,0, /* 1 */
	15,4, 0,0, /* 2 */
	16,4, 9,1, /* 3 */
	17,4, 2,6, /* + ijb actually semi colon needs to be shifted */

	14,5, 1,3, /* 0 ijb mod */
	15,5, 6,3, /* . ijb mod */ 
	16,5, 0,3, /* - ijb mod */
	17,5, 0,3, /* = ijb mod fixme shifted - */

	3,5, 3,6, /* @ ijb mod */
	4,5, 5,6, /* [ ijb mod */
	5,5, 2,4, /* ] ijb mod */
	6,5, 8,2, /* space ijb mod */
	7,5, 8,2, /* space ijb mod */
	8,5, 5,4, /* crsr down ijb mod */
	9,5, 0,5, /* crsr right ijb mod */
	10,5, 0,0, /* runstop fixme */
	11,5, 0,0, /* runstop fixme */

#if 0
	4,5, 8,2, /* space */
	5,5, 8,2, /* */
	6,5, 8,2, /* */
	7,5, 8,2, /* */
	8,5, 8,2, /* */
/* keys found  1,4 is 7(diff keypad?)  0,4 is 8 (diff keypad?)   1,5, is an up arrow */
/*  0,6 odd screen effect? moves it up */
#endif
	0
};

void petui_set_keyarr(int status)
{
  keyboard_set_keyarr(6, 0, status);
}

int petui_init(void)
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

  ui_handle_sidengine_resource=petui_handle_sidengine_resource;
  ui_handle_X=petui_handle_X;
  ui_draw_resid_string=petui_draw_resid_string;
  ui_draw_memory_string=petui_draw_memory_string;
  ui_set_ramblocks=petui_set_ramblocks;
  ui_attach_cart=petui_attach_cart;

  keyb=keyb_pet;
  keytable=pet_keytable;
  kb_fg=0;
  kb_bg=1;
  cursor_fg=1;
  cursor_bg=0;
  kb_height=PETKB_HEIGHT;
  kb_width=strlen(keyb[0]);
  ui_set_keyarr=petui_set_keyarr;

  machine_ui_done=1;

  return 0;
}

void petui_shutdown(void)
{
}
