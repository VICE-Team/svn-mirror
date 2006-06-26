/*
 * vkeyboard_gp2x.c
 *
 * Written by
 *  Mike Dawson <mike@gp2x.org>
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

#include "uitext_gp2x.h"
#include "input_gp2x.h"
#include "prefs_gp2x.h"
#include "ui_gp2x.h"
#include <string.h>
#include "keyboard.h"

int cursor_x=15;
int cursor_y=20;
int vkeyb_x=64;
int vkeyb_y=160;

int gotkeypress=0;
int vkey_pressed=0;
int vkey_released=0;
int button_deselected=0;
int vkey_row;
int vkey_column;
int shift_gotkeypress=0;
int shift_vkey_pressed=0;
int shift_vkey_released=0;
int shift_button_deselected=0;
int shift_vkey_row;
int shift_vkey_column;

int kb_fg, kb_bg;
int cursor_fg, cursor_bg;

int initialised=0;
char **keyb;
int kb_height, kb_width;
int *keytable;

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

void draw_vkeyb(unsigned char *screen) {
	int i;

	if(!initialised) {
		initialised=1;
		if(machine_type==C64) {
			keyb=keyb_c64vic;
			keytable=c64_keytable;
			kb_fg=1; kb_bg=9;
			cursor_fg=7; cursor_bg=7;
			kb_height=C64KB_HEIGHT;
			kb_width=strlen(keyb[0]);
		} else if(machine_type==C128) {
			keyb=keyb_c128;
			keytable=c128_keytable;
			kb_fg=1; kb_bg=12;
			cursor_fg=7; cursor_bg=7;
			kb_height=C128KB_HEIGHT;
			kb_width=strlen(keyb[0]);
		} else if(machine_type==CBM2) {
			keyb=keyb_cbm2;
			keytable=cbm2_keytable;
			kb_fg=0; kb_bg=1;
			cursor_fg=1; cursor_bg=0;
			kb_height=CBM2KB_HEIGHT;
			kb_width=strlen(keyb[0]);
		} else if(machine_type==PET) {
			keyb=keyb_pet;
			keytable=pet_keytable;
			kb_fg=0; kb_bg=1;
			cursor_fg=1; cursor_bg=0;
			kb_height=PETKB_HEIGHT;
			kb_width=strlen(keyb[0]);
		} else if(machine_type==PLUS4) {
			keyb=keyb_plus4;
			keytable=plus4_keytable;
			kb_fg=123; kb_bg=1;
			cursor_fg=122; cursor_bg=122;
			kb_height=PLUS4KB_HEIGHT;
			kb_width=strlen(keyb[0]);
		} else if(machine_type==VIC20) {
			keyb=keyb_c64vic;
			keytable=vic20_keytable;
			kb_fg=1; kb_bg=0;
			cursor_fg=7; cursor_bg=7;
			kb_height=C64KB_HEIGHT;
			kb_width=strlen(keyb[0]);
		}
	}

	/* keyboard */
	for(i=0; keyb[i]; i++) 
        	draw_ascii_string(screen, display_width, vkeyb_x, vkeyb_y+(i*8), keyb[i], kb_fg, kb_bg);

	/* cursor */
	draw_ascii_string(screen, display_width, 8*cursor_x, 8*cursor_y, "+", cursor_fg, cursor_bg);

	if(input_down) {
		input_down=0;
		if(input_b&&cursor_y==vkeyb_y/8) {
			if(vkeyb_y+(kb_height*8)<240) {
				vkeyb_y+=8;
				cursor_y++;
			}
		} else {
			if(cursor_y<(240/8)-1) cursor_y++;
		}
	} else if(input_up) {
		input_up=0;
		if(input_b&&cursor_y==vkeyb_y/8) {
			if(vkeyb_y>0) {
				vkeyb_y-=8;
				cursor_y--;
			}
		} else {
			if(cursor_y>0) cursor_y--;
		}
	} 
	if(input_left) {
		input_left=0;
		if(input_b&&cursor_y==vkeyb_y/8) {
			if(vkeyb_x>0) {
				vkeyb_x-=8;
				cursor_x--;
			}
		} else {
			if(cursor_x>0) cursor_x--;
		}
	} else if(input_right) {
		input_right=0;
		if(input_b&&cursor_y==vkeyb_y/8) {
			if(vkeyb_x+(kb_width*8)<display_width) {
				vkeyb_x+=8;
				cursor_x++;
			}
		} else {
			if(cursor_x<(display_width/8)-1) cursor_x++;
		}
	}
	/* b=normal keypress */
	if(!input_b) button_deselected=1;
	if(!input_b && gotkeypress) {
		gotkeypress=0;
		vkey_pressed=0;
		vkey_released=1;
	} else if(input_b && button_deselected) {
		button_deselected=0;
		for(i=0; keytable[i]; i+=4) {
			if((cursor_x-(vkeyb_x/8)==keytable[i]) 
				&& (cursor_y-(vkeyb_y/8)==keytable[i+1])) {
#if 0
				vkey=keytable[i+2];
				if(shifted) vkey|=128;
#endif
				vkey_row=keytable[i+2];
				vkey_column=keytable[i+3];
				vkey_pressed=1;
				gotkeypress=1;
				break;
			}
		}
	}
	if(vkey_pressed) {
		vkey_pressed=0;
		keyboard_set_keyarr(vkey_row, vkey_column, 1);
	} else if(vkey_released) {
		vkey_released=0;
		keyboard_set_keyarr(vkey_row, vkey_column, 0);
	}

	/* y=shifted keypress */
	if(!input_y) shift_button_deselected=1;
	if(!input_y && shift_gotkeypress) {
		shift_gotkeypress=0;
		shift_vkey_pressed=0;
		shift_vkey_released=1;
	} else if(input_y && shift_button_deselected) {
		shift_button_deselected=0;
		for(i=0; keytable[i]; i+=4) {
			if((cursor_x-(vkeyb_x/8)==keytable[i]) 
				&& (cursor_y-(vkeyb_y/8)==keytable[i+1])) {
				shift_vkey_row=keytable[i+2];
				shift_vkey_column=keytable[i+3];
				shift_vkey_pressed=1;
				shift_gotkeypress=1;
				break;
			}
		}
	}
	if(shift_vkey_pressed) {
		shift_vkey_pressed=0;
		if(machine_type==C64) keyboard_set_keyarr(1, 7, 1);
		if(machine_type==C128) keyboard_set_keyarr(1, 7, 1);
		if(machine_type==VIC20) keyboard_set_keyarr(1, 3, 1);
		if(machine_type==PLUS4) keyboard_set_keyarr(1, 7, 1);
		if(machine_type==PET) keyboard_set_keyarr(6, 0, 1);
		if(machine_type==CBM2) keyboard_set_keyarr(8, 4, 1);
		keyboard_set_keyarr(shift_vkey_row, shift_vkey_column, 1);
	} else if(shift_vkey_released) {
		shift_vkey_released=0;
		keyboard_set_keyarr(shift_vkey_row, shift_vkey_column, 0);
		if(machine_type==C64) keyboard_set_keyarr(1, 7, 0);
		if(machine_type==C128) keyboard_set_keyarr(1, 7, 0);
		if(machine_type==VIC20) keyboard_set_keyarr(1, 3, 0);
		if(machine_type==PLUS4) keyboard_set_keyarr(1, 7, 0);
		if(machine_type==PET) keyboard_set_keyarr(6, 0, 0);
		if(machine_type==CBM2) keyboard_set_keyarr(8, 4, 0);
	}
}
