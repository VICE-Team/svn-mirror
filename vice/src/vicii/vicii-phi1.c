/*
 * vicii-phi1.c - Memory interface for the MOS6569 (VIC-II) emulation,
 *                PHI1 support.
 *
 * Written by
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

#include <stdlib.h>

#include "maincpu.h"
#include "types.h"
#include "vicii-phi1.h"
#include "viciitypes.h"


BYTE vicii_read_phi1(void)
{
#if 1
    return (BYTE)(rand() & 0xff);
#else

    switch (maincpu_clk % vic_ii.cycles_per_line) {
      case 0:
        break;
      case 1:
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        break;
      case 6:
        break;
      case 7:
        break;
      case 8:
        break;
      case 9:
        break;
      case 10:
        break;
      case 11:
        break;
      case 12:
        break;
      case 13:
        break;
      case 14:
        break;
      case 15:
        break;
      case 16:
        break;
      case 17:
        break;
      case 18:
        break;
      case 19:
        break;
      case 20:
        break;
      case 21:
        break;
      case 22:
        break;
      case 23:
        break;
      case 24:
        break;
      case 25:
        break;
      case 26:
        break;
      case 27:
        break;
      case 28:
        break;
      case 29:
        break;
      case 30:
        break;
      case 31:
        break;
      case 32:
        break;
      case 33:
        break;
      case 34:
        break;
      case 35:
        break;
      case 36:
        break;
      case 37:
        break;
      case 38:
        break;
      case 39:
        break;
      case 40:
        break;
      case 41:
        break;
      case 42:
        break;
      case 43:
        break;
      case 44:
        break;
      case 45:
        break;
      case 46:
        break;
      case 47:
        break;
      case 48:
        break;
      case 49:
        break;
      case 50:
        break;
      case 51:
        break;
      case 52:
        break;
      case 53:
        break;
      case 54:
        break;
      case 55:
        break;
      case 56:
        break;
      case 57:
        break;
      case 58:
        break;
      case 59:
        break;
      case 60:
        break;
      case 61:
        break;
      case 62:
        break;
    }

    return (BYTE)(rand() & 0xff);
#endif
}

