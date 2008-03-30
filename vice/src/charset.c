/*
 * charset.c - Character set conversions.
 *
 * Written by
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Andreas Boose <viceteam@t-online.de>
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
#include <string.h>
#include <stdlib.h>

#include "charset.h"
#include "log.h"
#include "types.h"
#include "utils.h"

/* ------------------------------------------------------------------------- */

BYTE *charset_petconvstring(BYTE *c, int dir)
{
    BYTE *p;

    p = c;

    switch (dir) {
      case 0: /* To petscii.  */
        while (*p)
            *(p++) = charset_p_topetcii(*p);
        break;

      case 1: /* To ascii. */
      case 2: /* To ascii, convert also screencodes. */
        dir--;
        while (*p)
            *(p++) = charset_p_toascii(*p, dir);
        break;
      default:
        log_error(LOG_DEFAULT, "Unkown conversion rule.");
    }
    return c;
}

BYTE charset_p_toascii(BYTE c, int cs)
{
    switch (c) {
      case 0x0a:
      case 0x0d:
        return (BYTE)(cs ? (c | 0x40) : '\n'); /* Pet screen codes or CR/LF */
      case 0x40:
      case 0x60:
        return c;
      case 0xa0:                                /* CBM: Shifted Space */
      case 0xe0:
        return ' ';
      default:
        switch (c & 0xe0) {
          case 0x40: /* 41 - 7E */
          case 0x60:
            return (BYTE)(c ^ 0x20);

          case 0xc0: /* C0 - DF */
            return (BYTE)(c ^ 0x80);

          case 0x00: /* 01 - 1F */
            if (cs && c) /* Pet screen codes */
              return (BYTE)(c | 0x40);

      }
    }

    return ((isprint(c) ? c : '.'));
}


BYTE charset_p_topetcii(BYTE c)
{
    if ((c >= 0x5b) && (c <= 0x7e))
        return (BYTE)(c ^ 0x20);
    else if ((c >= 'A') && (c <= 'Z'))          /* C0 - DF */
        return (BYTE)(c | 0x80);
    return c;
}

BYTE charset_petcii_to_screencode(BYTE code, unsigned int reverse_mode)
{
    if (code >= 0x40 && code <= 0x5f)
        return (BYTE)(code - 0x40);
    if (code >= 0x60 && code <= 0x7f)
        return (BYTE)(code - 0x20);
    if (code >= 0xa0 && code <= 0xbf)
        return (BYTE)(code - 0x40);
    if (code >= 0xc0 && code <= 0xfe)
        return (BYTE)(code - 0x80);
    if (code == 0xff)
        return 0x5e;
    return code;
}

void charset_petcii_to_screencode_line(const BYTE *line, BYTE **buf,
                                       unsigned int *len)
{
    size_t linelen, i;

    linelen = strlen((const char *)line);
    *buf = (BYTE *)xmalloc(linelen);

    for (i = 0; i < linelen; i++) {
        (*buf)[i] = charset_petcii_to_screencode(line[i], 0);
    }
    *len = (unsigned int)linelen;
}

