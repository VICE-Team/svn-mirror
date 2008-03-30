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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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

char   *ctrl1[] = {
    "",  "", "", "", "", "wht", "", "",
    "dish", "ensh", "\n", "", "\f", "\n", "swlc", "",
    "",  "down", "rvon", "home", "del", "", "", "",
    "",  "",  "", "esc", "red", "rght", "grn", "blu"
};

char   *ctrl2[] = {
    "", "orng",  "",  "",  "",  "F1",  "F3",  "F5",
    "F7",  "F2", "F4",   "F6",   "F8",  "sret", "swuc", "",
    "blk",  "up", "rvof", "clr",  "inst", "brn",  "lred", "gry1",
    "gry2", "lgrn", "lblu", "gry3", "pur", "left", "yel", "cyn"
};


char   *cbmkeys[] = {
    "SHIFT-SPACE", "CBM-K", "CBM-I", "CBM-T", "CBM-@", "CBM-G", "CBM-+",
    "CBM-M", "CBM-POUND",
    "SHIFT-POUND", "CBM-N", "CBM-Q", "CBM-D", "CBM-Z", "CBM-S", "CBM-P",
    "CBM-A", "CBM-E", "CBM-R", "CBM-W", "CBM-H", "CBM-J", "CBM-L", "CBM-Y",
    "CBM-U", "CBM-O", "SHIFT-@", "CBM-F", "CBM-C", "CBM-X", "CBM-V", "CBM-B"
};


char *ctrl_to_str(int clean, BYTE c)
{
    static char  buf[16];


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


char *cbm_to_str(BYTE c)
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

/* ------------------------------------------------------------------------- */

/*
 * Commodore's International character sets
 */

static char *langnames[] = {	/* International Charsets */
	"commodore",	/* CBM  */
	"usa",		/* US   */
	"germany",	/* GER  */
	"denmark-1",	/* DEN1 */
	"france",	/* FRA  */
	"sweden-1",	/* SWE1 */
	"italy",	/* ITA  */
	"spain",	/* SPA  */
	"denmark-2",	/* DEN2 */
	"sweden-2",	/* SWE2 */
	"finland",	/* FIN  */
	"norway",	/* NOR  */
	"england"	/* UK   */
	"commodore-din"	/* C-DIN */
};


/* 7-bit */

/* ASCII/ISO 646 family */
static BYTE ASCextens[][11] = {		/* graphics/uppercase -- uppercase/lowercase */
    { 0x23, 0x24, 0x40, 0x5b,0x5c,0x5d,  0x2b,0x7c,0x7d,0x26 },	/* CBM  */
    { 0x23, 0x24, 0x40, 0x5b,0x5c,0x5d,  0x7b,0x7c,0x7d,0x7e },	/* US   */
    { 0x23, 0x24, 0x40, 0x5b,0x5c,0x55,  0x7b,0x7c,0x75,0x7e },	/* GER  */
    { 0x23, 0x24, 0x40, 0x7b,0x7c,0x7d,  0x5b,0x5c,0x5d,0x7e },	/* DEN1 */
    { 0x23, 0x24, 0x40, 0x5b,0x5c,0x5d,  0x7b,0x7c,0x7d,0x7e },	/* FRA  */
    { 0x23, 0x24, 0x40, 0x7b,0x7c,0x7d,  0x5b,0x5c,0x5d,0x7e },	/* SWE1 */
    { 0x23, 0x24, 0x40, 0x5b,0x5c,0x5d,  0x7b,0x7c,0x7d,0x7e },	/* ITA  */
    { 0x23, 0x24, 0x40, 0x5b,0x5c,0x5d,  0x7b,0x7c,0x7d,0x7e },	/* SPA  */
    { 0x23, 0x24, 0x40, 0x5b,0x5c,0x5d,  0x7b,0x7c,0x7d,0x7e },	/* DEN2 */
    { 0x23, 0x24, 0x40, 0x5b,0x5c,0x5d,  0x7b,0x7c,0x7d,0x7e },	/* SWE2 */
    { 0x23, 0x24, 0x40, 0x5b,0x5c,0x5d,  0x7b,0x7c,0x7d,0x7e },	/* UK   */
    { 0x23, 0x24, 0x40, 0x5b,0x5c,0x5d,  0x2b,0x7c,0x7d,0x26 }	/* CBM-DIN */
};


/* 8-bit */

static BYTE ISOextens[][11] = {
    { 0x23, 0x24, 0x40,  0x5B, 0xA3, 0x5D,	0x2B, 0x7C, 0x7D, 0x26 },
    { 0x23, 0x24, 0x40,  0x5B, 0x5C, 0x5D,	0x7B, 0x7C, 0x7D, 0x7E },
    { 0x23, 0x24, 0xA7,  0xC4, 0xD6, 0xDC,	0xE4, 0xF6, 0xFC, 0xDF },
    { 0x23, 0x24, 0x40,  0xE6, 0xF8, 0xE5,	0xC6, 0xD8, 0xC5, 0x7E },
    { 0x23, 0x24, 0xE0,  0xB0, 0xE7, 0xA7,	0xE9, 0xF9, 0xE8, 0x98 },
    { 0x23, 0xA4, 0xC9,  0xE4, 0xF6, 0xE5,	0xC4, 0xD6, 0xC5, 0xFC },
    { 0x23, 0x24, 0x40,  0xB0, 0x5C, 0xE9,	0xE0, 0xF2, 0xE8, 0xEC },
    { 0xA0, 0x24, 0x40,  0xA1, 0xD1, 0xBF,	0x98, 0xF1, 0x7D, 0x7E },
    { 0x23, 0x24, 0x40,  0xC6, 0xD8, 0xC5,	0xE6, 0xF8, 0xE6, 0x7E },
    { 0x23, 0xA4, 0xC9,  0xC4, 0xD6, 0xC5,	0xE4, 0xF6, 0xE5, 0xFC },
    { 0x23, 0x24, 0x40,  0xC4, 0xD6, 0xC5,	0xE4, 0xF6, 0xE5, 0x7E },
    { 0x23, 0xA4, 0xC9,  0xC6, 0xD8, 0xC5,	0xE6, 0xF8, 0xE6, 0xFC },
    { 0xA3, 0x24, 0x40,  0x5B, 0x5C, 0x5D,	0x7B, 0x7C, 0x7D, 0x7E },
    { 0x23, 0x24, 0xA7,  0x5B, 0xA3, 0x5D,	0x2B, 0x7C, 0x7D, 0x26 }
};


/*
 * C64 Uppercase 97-122 (61-7A) and 160-192 (A0-C0) to ASCII
 */

static BYTE ASCgraphics1[] = {
    35,124, 45, 45, 45, 45,124,124, 92, 92, 47, 92, 92, 47, 47, 92,
    35, 95, 35,124, 47, 88, 79, 35,124, 35
};
static BYTE ASCgraphics2[] = {
    32,124, 35, 45, 45,124, 35,124, 35, 47,124,124, 47, 92, 92, 45,
    47, 45, 45,124,124,124,124, 45, 45, 45, 47, 92, 92, 47, 47, 35
};


/* ------------------------------------------------------------------------- */


static BYTE (*extens)[11] = &ASCextens[0];
static BYTE *graphics1 = (BYTE *)ASCgraphics1;
static BYTE *graphics2 = (BYTE *)ASCgraphics2;


int  set_locale(char *localename)
{
    if (!localename && !(localename = getenv("LC_CTYPE")) )
	return (-1);

    if (!strcmp(localename, "Latin1") ||
	!strcmp(localename, "iso_8859_1") ||
	!strcmp(localename, "iso-8859-1")) {

	extens    = ISOextens;
	graphics1 = (BYTE *)ASCgraphics1;
	graphics2 = (BYTE *)ASCgraphics2;
    }
    else {
	extens    = ASCextens;
	graphics1 = (BYTE *)ASCgraphics1;
	graphics2 = (BYTE *)ASCgraphics2;
    }

    return (0);
}


/*
 * PETSCII to ASCII or 8-bit conversion. Alas, there is not just
 * one or two conversions, but a complete chaos...
 *  -- and then there are all the various standard sets...
 */

/*
 * Convert a national CBM character to its standard
 * international equivalent.
 *
 * Parameters:
 *  lang  0-10  CBM language codes
 *  case  0     lowercase, otherwise uppercase
 *     c        byte to convert
 */


int  p_toichar(int lang, int lwcase, BYTE c)
{
    if (lang < 0 || lang > NUM_LANGUAGES) {
	fprintf (stderr, "Fatal error: language not recognized.\n");
	exit (2);
    }


    /* Duplicate codes  C0 - DF */

    if ((c >= 0xc0) && (c <= 0xdf))	/* Same as 96 - 127 */
	c ^= 0xa0;

    else if (c == 255)
	c = 126;


    /* CBM-x Graphics Images  A0 - BF, E0 - FE */

    else if (c >= 0xa0)			/* C0 -- FF is handled by now */
	c = graphics2[c & 0x1f];


    /* @ / Eacute / Section */
#if 0
    if (c == 0x40)
	return (extens1[lang][2]);

    else if (c == 0x60) {
	/* Standard mode: vertical bar; all others: left quote (`) */

    }
#endif


    /* Alphabetics ... */

    if ((c >= 0x41) && (c <= 0x5a)) {
	if (lwcase)
	    c ^= 0x20;
    }
    else if ((c >= 0x5b) && (c <= 0x5d)) {
	if (!lwcase &&
	    (lang == CS_DEN1 || lang == CS_SWE1))
	    c = extens[lang][c -0x5b + 6];
	else
	    c = extens[lang][c -0x58];
    }
    else if ((c >= 0x61) && (c <= 0x7a)) {
	if (lwcase)
	    c ^= 0x20;
	else
	    c = graphics1[c -0x61];
    }
    else if ((c >= 0x7b) && (c <= 0x7e)) {
	c = extens[lang][c -0x7b + 6];
    }

    return (c);
}


/* ------------------------------------------------------------------------- */

/*
 * Language name to code.
 */

int  str_to_lang(char *name)
{
    int     j, token;
    char   *p, *q;

    if (!name || !*name)
	return (-1);

    for (p = name; *p; p++) {
	if (isalpha ((int)*p))
	    *p = tolower(*p);
	else if (*p == '_' || *p == '/')
	    *p = '-';
    }

    for (token = 0; token < NUM_LANGUAGES; token++) {
	for (p = langnames[token], q = (char *)name, j = 0;
	     *p && *q && *p == *q; p++, q++, j++);

	/* found an exact match */
	if (j && !*p) {
	    /*fprintf (stderr, "found %s %2x\n", langnames[token], token);*/
	    return token;
	}
    } /* for */

    return (-1);
}
