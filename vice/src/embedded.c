/*
 * embedded.c - Code for embedding data files.
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

#ifdef USE_EMBEDDED
#include <string.h>
#include <stdio.h>

#include "c64mem.h"
#include "driverom.h"
#include "machine.h"

#include "drivedos1541.h"
#include "drived1541ii.h"
#include "petbasic4.h"
#include "petchargen.h"
#include "petedit4b80.h"
#include "petkernal4.h"

#define NL10_ROM_SIZE      0x8000

typedef struct embedded_s {
    char *name;
    int minsize;
    int maxsize;
    size_t size;
    BYTE *esrc;
} embedded_t;

static embedded_t commonfiles[] = {
  { "mps803", 512 * 7, 512 * 7, 512 * 7, NULL },
  { "nl10-cbm", NL10_ROM_SIZE, NL10_ROM_SIZE, NL10_ROM_SIZE, NULL },
  { "dos1541", DRIVE_ROM1541_SIZE, DRIVE_ROM1541_SIZE_EXPANDED, DRIVE_ROM1541_SIZE, drive_rom1541_embedded },
  { "d1541II", DRIVE_ROM1541II_SIZE, DRIVE_ROM1541II_SIZE_EXPANDED, DRIVE_ROM1541II_SIZE, drive_rom1541ii_embedded },
  { "dos1001", DRIVE_ROM1001_SIZE, DRIVE_ROM1001_SIZE, DRIVE_ROM1001_SIZE, NULL },
  { "dos1570", DRIVE_ROM1571_SIZE, DRIVE_ROM1571_SIZE, DRIVE_ROM1571_SIZE, NULL },
  { "dos1571", DRIVE_ROM1571_SIZE, DRIVE_ROM1571_SIZE, DRIVE_ROM1571_SIZE, NULL },
  { "dos1581", DRIVE_ROM1581_SIZE, DRIVE_ROM1581_SIZE, DRIVE_ROM1581_SIZE, NULL },
  { "dos2031", DRIVE_ROM2031_SIZE, DRIVE_ROM2031_SIZE, DRIVE_ROM2031_SIZE, NULL },
  { "dos2040", DRIVE_ROM2040_SIZE, DRIVE_ROM2040_SIZE, DRIVE_ROM2040_SIZE, NULL },
  { "dos3040", DRIVE_ROM3040_SIZE, DRIVE_ROM3040_SIZE, DRIVE_ROM3040_SIZE, NULL },
  { "dos4040", DRIVE_ROM4040_SIZE, DRIVE_ROM4040_SIZE, DRIVE_ROM4040_SIZE, NULL },
  { NULL }
};

static embedded_t c64files[] = {
  { "basic", C64_BASIC_ROM_SIZE, C64_BASIC_ROM_SIZE, C64_BASIC_ROM_SIZE, NULL },
  { "kernal", C64_KERNAL_ROM_SIZE, C64_KERNAL_ROM_SIZE, C64_KERNAL_ROM_SIZE, NULL },
  { "chargen", C64_CHARGEN_ROM_SIZE, C64_CHARGEN_ROM_SIZE, C64_CHARGEN_ROM_SIZE, NULL },
  { NULL }
};

static embedded_t petfiles[] = {
  { "chargen", 0x800, 0x800, 0x800, petchargen_embedded },
  { "basic4", 0x2000, 0x3000, 0x3000, petbasic4_embedded },
  { "kernal4", 0x1000, 0x1000, 0x1000, petkernal4_embedded },
  { "edit4b80", 0x800, 0x1000, 0x800, petedit4b80_embedded },
  { NULL }
};

static size_t embedded_match_file(const char *name, BYTE *dest, int minsize, int maxsize, embedded_t *emb)
{
    int i = 0;

    while (emb[i].name != NULL) {
        if (!strcmp(name, emb[i].name) && minsize == emb[i].minsize && maxsize == emb[i].maxsize) {
            if (emb[i].esrc != NULL) {
                if (emb[i].size != minsize) {
                    memcpy(dest, emb[i].esrc, maxsize);
                } else {
                    memcpy(dest + maxsize - minsize, emb[i].esrc, minsize);
                }
            }
            return emb[i].size;
        }
        i++;
    }
    return 0;
}

size_t embedded_check_file(const char *name, BYTE *dest, int minsize, int maxsize)
{
    size_t retval;
    int i = 0;

    if ((retval = embedded_match_file(name, dest, minsize, maxsize, commonfiles)) != 0) {
        return retval;
    }

    switch (machine_class) {
        case VICE_MACHINE_C64:
        case VICE_MACHINE_C64DTV:
        default:
            if ((retval = embedded_match_file(name, dest, minsize, maxsize, c64files)) != 0) {
                return retval;
            }
            break;
        case VICE_MACHINE_PET:
            if ((retval = embedded_match_file(name, dest, minsize, maxsize, petfiles)) != 0) {
                return retval;
            }
            break;
    }
    return 0;
}
#endif
