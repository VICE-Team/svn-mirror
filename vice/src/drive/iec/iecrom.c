/*
 * iecrom.c
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

#include "drive.h"
#include "driverom.h"
#include "iecrom.h"


int iecrom_check_loaded(unsigned int type)
{
    switch (type) {
      case DRIVE_TYPE_1541:
        if (rom1541_loaded < 1 && rom_loaded)
            return -1;
        break;
      case DRIVE_TYPE_1541II:
        if (rom1541ii_loaded < 1 && rom_loaded)
            return -1;
        break;
      case DRIVE_TYPE_1571:
        if (rom1571_loaded < 1 && rom_loaded)
            return -1;
        break;
      case DRIVE_TYPE_1581:
        if (rom1581_loaded < 1 && rom_loaded)
            return -1;
        break;
      default:
        return -1;
    }

    return 0;
}

