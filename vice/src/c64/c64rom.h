/*
 * c64rom.h
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

#ifndef VICE_C64ROM_H
#define VICE_C64ROM_H

/* unique IDs to refer to a kernal, used for the KernalRev resource */
#define C64_KERNAL_UNKNOWN -1
#define C64_KERNAL_REV1 1       /* 901227-01 */
#define C64_KERNAL_REV2 2       /* 901227-02 */
#define C64_KERNAL_REV3 3       /* 901227-03 */
#define C64_KERNAL_SX64 67      /* 251104-04 */
#define C64_KERNAL_4064 100     /* 901246-01 */

#define C64_KERNAL_JAP     10   /* FIXME */
#define C64_KERNAL_GS64    11   /* FIXME */
#define C64_KERNAL_MAX     12   /* FIXME */
#define C64_KERNAL_REV3SWE 13   /* FIXME */

/* simple additive checksum */
#define C64_BASIC_CHECKSUM         15702

#define C64_KERNAL_CHECKSUM_R01    54525       /* 901227-01 */
#define C64_KERNAL_CHECKSUM_R02    50955       /* 901227-02 */
#define C64_KERNAL_CHECKSUM_R03    50954       /* 901227-03 */
#define C64_KERNAL_CHECKSUM_R03swe 50633
#define C64_KERNAL_CHECKSUM_R43    50955       /* 251104-04 */
#define C64_KERNAL_CHECKSUM_R64    49680       /* 901246-01 (educator) */
#define C64_KERNAL_CHECKSUM_GS64   46538
#define C64_KERNAL_CHECKSUM_JAP    53635

/* the value located at 0xff80 */
#define C64_KERNAL_ID_R01    0xaa       /* 901227-01 */
#define C64_KERNAL_ID_R02    0x00       /* 901227-02 */
#define C64_KERNAL_ID_R03    0x03       /* 901227-03 */
#define C64_KERNAL_ID_R03swe 0x03
#define C64_KERNAL_ID_R43    0x43       /* 251104-04 */
#define C64_KERNAL_ID_R64    0x64       /* 901246-01 (educator) */
#define C64_KERNAL_ID_GS64   0x03
#define C64_KERNAL_ID_JAP    0x00

extern int c64rom_load_kernal(const char *rom_name, uint8_t *new_kernal);
extern int c64rom_load_basic(const char *rom_name);
extern int c64rom_load_chargen(const char *rom_name);

extern int c64rom_get_kernal_checksum(void);
extern int c64rom_get_kernal_chksum_id(uint16_t *sumout, int *idout);
extern int c64rom_get_basic_checksum(void);

extern int c64rom_isloaded(void);

extern int c64rom_cartkernal_active;

#endif
