/*
 * charsets.h - Character set conversions.
 *
 * Written by
 *  Jouko Valta (jopi@stekt.oulu.fi)
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

#ifndef _CHARSETS_H
#define _CHARSETS_H

#include "types.h"

#define CS_CBM		0
#define CS_USA		1
#define CS_GER		2
#define CS_DEN1		3
#define CS_FRA		4
#define CS_SWE1		5
#define CS_ITA		6
#define CS_SPA		7
#define CS_DEN2		8
#define CS_SWE2		9
#define CS_FIN		10
#define CS_NOR		11
#define CS_UK		12	/* Ascii */
#define CS_CBMDIN	13	/* CS_CBM differs DIN mode */

#define NUM_LANGUAGES	14

extern char *ctrl1[], *ctrl2[], *cbmkeys[];

extern void petconvstring(char *c, int dir);
extern int p_toascii(int c, int cs);
extern int p_topetcii(int c);
extern char *ctrl_to_str(int clean, BYTE c);
extern char *cbm_to_str(BYTE c);
extern void unix_filename(char *p);

extern int set_locale(char *localename);
extern int str_to_lang(char *name);

#endif  /* _CHARSETS_H */

