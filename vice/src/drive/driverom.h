/*
 * driverom.h
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

#ifndef _DRIVEROM_H
#define _DRIVEROM_H

#define DRIVE_ROM1541_SIZE          0x4000
#define DRIVE_ROM1541_SIZE_EXPANDED 0x8000
#define DRIVE_ROM1541II_SIZE        0x4000
#define DRIVE_ROM1551_SIZE          0x4000
#define DRIVE_ROM1571_SIZE          0x8000
#define DRIVE_ROM1581_SIZE          0x8000
#define DRIVE_ROM2031_SIZE          0x4000
#define DRIVE_ROM1001_SIZE          0x4000 /* same as ROM8050 and ROM8250 !*/
#define DRIVE_ROM2040_SIZE          0x2000
#define DRIVE_ROM3040_SIZE          0x3000
#define DRIVE_ROM4040_SIZE          0x3000

#define DRIVE_ROM1541_CHECKSUM      1988651

/* RAM/ROM.  */
extern BYTE drive_rom1541[];
extern BYTE drive_rom1541ii[];
extern BYTE drive_rom1551[];
extern BYTE drive_rom1571[];
extern BYTE drive_rom1581[];
extern BYTE drive_rom2031[];
extern BYTE drive_rom1001[];
extern BYTE drive_rom2040[];
extern BYTE drive_rom3040[];
extern BYTE drive_rom4040[];

/* If nonzero, the ROM image has been loaded.  */
extern unsigned int rom1541_loaded;
extern unsigned int rom1541ii_loaded;
extern unsigned int rom1551_loaded;
extern unsigned int rom1571_loaded;
extern unsigned int rom1581_loaded;
extern unsigned int rom2031_loaded;
extern unsigned int rom1001_loaded;
extern unsigned int rom2040_loaded;
extern unsigned int rom3040_loaded;
extern unsigned int rom4040_loaded;

extern void drive_rom_init(void);
extern void drive_rom_setup_image(unsigned int dnr);
extern void drive_rom_initialize_traps(unsigned int dnr);
extern int drive_rom_load_images(void);
extern int drive_rom_check_loaded(unsigned int type);
extern int drive_rom_do_1541_checksum(void);
extern int drive_rom_load_1541(void);
extern int drive_rom_load_1541ii(void);
extern int drive_rom_load_1551(void);
extern int drive_rom_load_1571(void);
extern int drive_rom_load_1581(void);
extern int drive_rom_load_2031(void);
extern int drive_rom_load_1001(void);
extern int drive_rom_load_2040(void);
extern int drive_rom_load_3040(void);
extern int drive_rom_load_4040(void);

#endif

