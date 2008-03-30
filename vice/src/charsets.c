/*
 * charsets.c - Character set conversions.
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

#include "vice.h"

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#endif

#include "charsets.h"

/* ------------------------------------------------------------------------- */

void    petconvstring(char *c, int dir)
{
    char *p = c;

    switch (dir) {
      case 0:		/* to petscii */
	for (; *p; p++)
	    *p = p_topetcii(*p);
	break;

      case 1:		/* to ascii */
      case 2:		/* to ascii, convert also screencodes */
	--dir;
	for (; *p; p++) {
	    *p = p_toascii(*p, dir);
	}
    }
}

void  unix_filename(char *p)
{
    while(*p) {
	if (*p == '/')
	    *p= '_';
	p++;
    }
}
 
int   p_toascii(int c, int cs)
{
    c &= 0xff;

    switch (c) {
    case 0x0a:
    case 0x0d:
      return (cs ? (c | 0x40) : '\n');	/* Pet screen codes or CR/LF */

    case 0x40:
    case 0x60:
      return (c);

    case 0xa0:				/* CBM: Shifted Space */
    case 0xe0:
      return (' ');

    case 0xff:
      return (0x7e);

    default:
      switch (c & 0xe0) {
      case 0x40:			/* 41 - 7E */
      case 0x60:
	return (c ^ 0x20);

      case 0xc0:			/* C0 - DF */
	return (c ^ 0x80);

      case 0x00:			/* 01 - 1F */
	if (cs && c)			/* Pet screen codes */
	  return (c | 0x40);

      }  /* switch */
    }  /* switch */

    return ((isprint(c) ? c : '.'));
}


int   p_topetcii(int c)
{
    if (c == 0x7e)		/*  '~' is ASCII for 'pi' */
	return (0xff);

    else if ((c >= 0x5b) && (c <= 0x7e))
	return (c ^ 0x20);

    else if ((c >= 'A') && (c <= 'Z'))		/* C0 - DF */
	return (c | 0x80);

    return (c);
}


/* ------------------------------------------------------------------------- */

/*
 * Control Codes
 */

 /*
  * Printer's control code symbols
  */

const char *ctrl1[] = {
    "",  "", "", "", "", "wht", "", "",
    "dish", "ensh", "\n", "", "\f", "\n", "swlc", "",
    "",  "down", "rvon", "home", "del", "", "", "",
    "",  "",  "", "esc", "red", "rght", "grn", "blu"
};

const char *ctrl2[] = {
    "", "orng",  "",  "",  "",  "F1",  "F3",  "F5",
    "F7",  "F2", "F4",   "F6",   "F8",  "sret", "swuc", "",
    "blk",  "up", "rvof", "clr",  "inst", "brn",  "lred", "gry1",
    "gry2", "lgrn", "lblu", "gry3", "pur", "left", "yel", "cyn"
};


const char *cbmkeys[] = {
    "SHIFT-SPACE", "CBM-K", "CBM-I", "CBM-T", "CBM-@", "CBM-G", "CBM-+",
    "CBM-M", "CBM-POUND",
    "SHIFT-POUND", "CBM-N", "CBM-Q", "CBM-D", "CBM-Z", "CBM-S", "CBM-P",
    "CBM-A", "CBM-E", "CBM-R", "CBM-W", "CBM-H", "CBM-J", "CBM-L", "CBM-Y",
    "CBM-U", "CBM-O", "SHIFT-@", "CBM-F", "CBM-C", "CBM-X", "CBM-V", "CBM-B"
};


const char *ctrl_to_str(int clean, BYTE c)
{
    static char buf[16];

    if (c == 0x0a || c == 0xc || c == 0xd)
	return (ctrl1[c]);

    /* The rest of control codes must be skipped when clean is set */

    if (!clean) {
	if ((c < 0x20) && *ctrl1[c])
	    sprintf(buf, "(%s)", ctrl1[c]);
	else if (((c & 0x7f) < 0x20) && *ctrl2[c & 0x1f])
	    sprintf(buf, "(%s)", ctrl2[c & 0x1f]);
	else
	    sprintf(buf, "($%02x)", c & 0xff);

	return (buf);
    } /* !clean */

    return (NULL);
}


const char *cbm_to_str(BYTE c)
{
    static char  buf[16];

    switch (c) {
    case 0x60:
      return "(SHIFT-*)";

    case 0x7c:
      return "(CBM--)";		/* Conflicts with Scandinavian Chars */

    case 0x7f:
      return "(CBM-*)";
/*
    case 0xff:
      return "(pi)";
*/
    default:
      if ((c > 0xa0 && c <= 0xbf) || (c > 0xe0 && c <= 0xfe))
	sprintf (buf, "(%s)", cbmkeys[c & 0x1f]);
      else
	return (NULL);
    }  /* switch */

    return (buf);
}
