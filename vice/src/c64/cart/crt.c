/*
 * crt.c - CRT image handling.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "archdep.h"
#include "c64cartmem.h"
#include "cartridge.h"
#include "crt.h"
#include "resources.h"
#include "types.h"

int crttype = 0;

/*
 * CRT image "strings".
 */
static const char CRT_HEADER[] = "C64 CARTRIDGE   ";
static const char CHIP_HEADER[] = "CHIP";
static const char STRING_EXPERT[] = "Expert Cartridge";


int crt_attach(const char *filename, BYTE *rawcart)
{
    BYTE header[0x40], chipheader[0x10];
    int i;
    FILE *fd;

    fd = fopen(filename, MODE_READ);

    if (!fd)
        return -1;

    if (fread(header, 0x40, 1, fd) < 1) {
        fclose(fd);
        return -1;
    }

    if (strncmp((char*)header, "C64 CARTRIDGE   ", 16)) {
        fclose(fd);
        return -1;
    }

    crttype = header[0x17] + header[0x16] * 256;
    switch (crttype) {
      case CARTRIDGE_CRT:
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            fclose(fd);
            return -1;
        }
        if (chipheader[0xc] == 0x80 && chipheader[0xe] != 0
            && chipheader[0xe] <= 0x40) {
            if (fread(rawcart, chipheader[0xe] << 8, 1, fd) < 1) {
                fclose(fd);
                return -1;
            }
            crttype = (chipheader[0xe] <= 0x20) ? CARTRIDGE_GENERIC_8KB
                      : CARTRIDGE_GENERIC_16KB;
            /* try to read next CHIP header in case of 16k Ultimax cart */
            if (fread(chipheader, 0x10, 1, fd) < 1) {
                fclose(fd);
                break;
            }
        }
        if (chipheader[0xc] >= 0xe0 && chipheader[0xe] != 0
            && (chipheader[0xe] + chipheader[0xc]) == 0x100) {
            if (fread(rawcart + ((chipheader[0xc] << 8) & 0x3fff),
                chipheader[0xe] << 8, 1, fd) < 1) {
                fclose(fd);
                return -1;
            }
            crttype = CARTRIDGE_ULTIMAX;
            fclose(fd);
            break;
        }
        fclose(fd);
        return -1;
      case CARTRIDGE_WESTERMANN:
      case CARTRIDGE_FINAL_I:
      case CARTRIDGE_WARPSPEED:
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            fclose(fd);
            return -1;
        }
        if (chipheader[0xc] != 0x80 || chipheader[0xe] != 0x40) {
            fclose(fd);
            return -1;
        }

        if (fread(rawcart, chipheader[0xe] << 8, 1, fd) < 1) {
            fclose(fd);
            return -1;
        }
        fclose(fd);
        break;
      case CARTRIDGE_ACTION_REPLAY:
      case CARTRIDGE_ATOMIC_POWER:
        for (i = 0; i <= 3; i++) {
            if (fread(chipheader, 0x10, 1, fd) < 1) {
                fclose(fd);
                return -1;
            }
            if (chipheader[0xb] > 3) {
                fclose(fd);
                return -1;
            }
            if (fread(&rawcart[chipheader[0xb] << 13], 0x2000, 1, fd) < 1) {                                        fclose(fd);
                return -1;
            }
        }
        fclose(fd);
        break;
      case CARTRIDGE_KCS_POWER:
      case CARTRIDGE_SIMONS_BASIC:
        for (i = 0; i <= 1; i++) {
            if (fread(chipheader, 0x10, 1, fd) < 1) {
                fclose(fd);
                return -1;
            }
            if (chipheader[0xc] != 0x80 && chipheader[0xc] != 0xa0) {
                fclose(fd);
                return -1;
            }
            if (fread(&rawcart[(chipheader[0xc] << 8) - 0x8000], 0x2000,
                      1, fd) < 1) {
                fclose(fd);
                return -1;
            }
        }
        fclose(fd);
        break;
      case CARTRIDGE_FINAL_III:
        for (i = 0; i <= 3; i++) {
            if (fread(chipheader, 0x10, 1, fd) < 1) {
                fclose(fd);
                return -1;
            }
            if (chipheader[0xb] > 3) {
                fclose(fd);
                return -1;
            }
            if (fread(&rawcart[chipheader[0xb] << 14], 0x4000, 1, fd) < 1) {
                fclose(fd);
                return -1;
            }
        }
        fclose(fd);
        break;
      case CARTRIDGE_OCEAN:
      case CARTRIDGE_GS:
      case CARTRIDGE_DINAMIC:
        while (1) {
            if (fread(chipheader, 0x10, 1, fd) < 1) {
                fclose(fd);
                break;
            }
            if (chipheader[0xb] >= 64 || (chipheader[0xc] != 0x80
                && chipheader[0xc] != 0xa0)) {
                fclose(fd);
                return -1;
            }
            if (fread(&rawcart[chipheader[0xb] << 13], 0x2000, 1, fd) < 1) {
                fclose(fd);
                return -1;
            }
        }
        break;
      case CARTRIDGE_FUNPLAY:
        while (1) {
            if (fread(chipheader, 0x10, 1, fd) < 1) {
                fclose(fd);
                break;
            }
            if (chipheader[0xc] != 0x80 && chipheader[0xc] != 0xa0) {
                fclose(fd);
                return -1;
            }
            if (fread(&rawcart[(((chipheader[0xb] >> 2) |
                (chipheader[0xb] & 1)) & 15) << 13], 0x2000, 1, fd) < 1) {
                fclose(fd);
                return -1;
            }
        }
        break;
      case CARTRIDGE_SUPER_GAMES:
        while (1) {
            if (fread(chipheader, 0x10, 1, fd) < 1) {
                fclose(fd);
                break;
            }
            if (chipheader[0xc] != 0x80 && chipheader[0xe] != 0x40
                && chipheader[0xb] > 3) {
                fclose(fd);
                return -1;
            }
            if (fread(&rawcart[chipheader[0xb] << 14], 0x4000, 1, fd) < 1) {
                fclose(fd);
                return -1;
            }
        }
        break;
      case CARTRIDGE_EPYX_FASTLOAD:
      case CARTRIDGE_REX:
      case CARTRIDGE_EXPERT:
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            fclose(fd);
            return -1;
        }
        if (fread(rawcart, 0x2000, 1, fd) < 1) {
            fclose(fd);
            return -1;
        }
        if (crttype == CARTRIDGE_EXPERT) {
            resources_set_value("CartridgeMode",
                                (resource_value_t)CARTRIDGE_MODE_ON);
        }
        break;
      case CARTRIDGE_ZAXXON:
        /* first CHIP header holds $8000-$a000 data */
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            fclose(fd);
            return -1;
        }
        if (chipheader[0xc] != 0x80
            || (chipheader[0xe] != 0x10 && chipheader[0xe] != 0x20)
            || fread(rawcart, chipheader[0xe] << 8, 1, fd) < 1) {
            fclose(fd);
            return -1;
        }
        /* 4kB ROM is mirrored to $9000 */
        if (chipheader[0xe] == 0x10)
            memcpy(&rawcart[0x1000], &rawcart[0x0000], 0x1000);

        /* second/third CHIP headers hold $a000-$c000 banked data */
        for (i = 0; i <= 1; i++) {
            if (fread(chipheader, 0x10, 1, fd) < 1) {
                fclose(fd);
                return -1;
            }
            if (chipheader[0xc] != 0xa0 || chipheader[0xe] != 0x20
                || fread(&rawcart[0x2000+(chipheader[0xb] << 13)],
                         0x2000, 1, fd) < 1) {
                fclose(fd);
                return -1;
            }
        }
        fclose(fd);
        break;
      default:
        fclose(fd);
        return -1;
    }
    return 0;
}

/*
 * This function writes Expert .crt images ONLY!!!
 */
int crt_save(const char *filename)
{
    FILE *fd;
    BYTE header[0x40], chipheader[0x10];

    fd = fopen(filename, MODE_WRITE);

    if (!fd)
        return -1;

    /*
     * Initialize headers to zero.
     */
    memset(header, 0x0, 0x40);
    memset(chipheader, 0x0, 0x10);

    /*
     * Construct CRT header.
     */
    strcpy(header, CRT_HEADER);

    /*
     * fileheader-length (= 0x0040)
     */
    header[0x10] = 0x00;
    header[0x11] = 0x00;
    header[0x12] = 0x00;
    header[0x13] = 0x40;

    /*
     * Version (= 0x0100)
     */
    header[0x14] = 0x01;
    header[0x15] = 0x00;

    /*
     * Hardware type (= CARTRIDGE_EXPERT)
     */
    header[0x16] = 0x00;
    header[0x17] = CARTRIDGE_EXPERT;

    /*
     * Exrom line
     */
    header[0x18] = 0x01;            /* ? */

    /*
     * Game line
     */
    header[0x19] = 0x01;            /* ? */

    /*
     * Set name.
     */
    strcpy(&header[0x20], STRING_EXPERT);

    /*
     * Write CRT header.
     */
    if (fwrite(header, sizeof(BYTE), 0x40, fd) != 0x40) {
        fclose(fd);
        return -1;
    }

    /*
     * Construct chip packet.
     */
    strcpy(chipheader, CHIP_HEADER);

    /*
     * Packet length. (= 0x2010; 0x10 + 0x2000)
     */
    chipheader[0x04] = 0x00;
    chipheader[0x05] = 0x00;
    chipheader[0x06] = 0x20;
    chipheader[0x07] = 0x10;

    /*
     * Chip type. (= FlashROM?)
     */
    chipheader[0x08] = 0x00;
    chipheader[0x09] = 0x02;

    /*
     * Bank nr. (= 0)
     */
    chipheader[0x0a] = 0x00;
    chipheader[0x0b] = 0x00;

    /*
     * Address. (= 0x8000)
     */
    chipheader[0x0c] = 0x80;
    chipheader[0x0d] = 0x00;

    /*
     * Length. (= 0x2000)
     */
    chipheader[0x0e] = 0x20;
    chipheader[0x0f] = 0x00;

    /*
     * Write CHIP header.
     */
    if (fwrite(chipheader, sizeof(BYTE), 0x10, fd) != 0x10) {
        fclose(fd);
        return -1;
    }

    /*
     * Write CHIP packet data.
     */
    if (fwrite(export_ram0, sizeof(char), 0x2000, fd) != 0x2000) {
        fclose(fd);
        return -1;
    }

    fclose(fd);

    return 0;
}

