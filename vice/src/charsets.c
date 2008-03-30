/*
 * charsets.c - Character set conversions.
 *
 * Written by
 *  Jouko Valta <jopi@stekt.oulu.fi>
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "charsets.h"
#include "log.h"
#include "types.h"

/* ------------------------------------------------------------------------- */

char *petconvstring(char *c, int dir)
{
    char *p = c;

    switch (dir) {
      case 0: /* To petscii.  */
        while (*p) *(p++) = p_topetcii(*p);
        break;

      case 1: /* To ascii. */
      case 2: /* To ascii, convert also screencodes. */
        dir--;
        while (*p) *(p++) = p_toascii(*p, dir);
        break;
      default:
        log_error(LOG_DEFAULT, "Unkown conversion rule.");
    }
    return c;
}

int p_toascii(int c, int cs)
{
    c &= 0xff;

    switch (c) {
      case 0x0a:
      case 0x0d:
        return (cs ? (c | 0x40) : '\n'); /* Pet screen codes or CR/LF */
      case 0x40:
      case 0x60:
        return c;
      case 0xa0:				/* CBM: Shifted Space */
      case 0xe0:
        return ' ';
      default:
        switch (c & 0xe0) {
          case 0x40: /* 41 - 7E */
          case 0x60:
            return (c ^ 0x20);

          case 0xc0: /* C0 - DF */
            return (c ^ 0x80);
 
          case 0x00: /* 01 - 1F */
            if (cs && c) /* Pet screen codes */
              return (c | 0x40);

      }
    }

    return ((isprint(c) ? c : '.'));
}


int p_topetcii(int c)
{
    if ((c >= 0x5b) && (c <= 0x7e))
        return (c ^ 0x20);
    else if ((c >= 'A') && (c <= 'Z'))		/* C0 - DF */
        return (c | 0x80);
    return c;
}

