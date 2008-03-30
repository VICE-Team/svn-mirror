/*
 * petmem.h - PET memory handling.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef _PETMEM_H
#define _PETMEM_H

#include "types.h"

#define PET_RAM_SIZE		0x8000
#define PET_ROM_SIZE		0x8000
#define PET_CHARGEN_ROM_SIZE	0x1000

#define	PET2001_CHECKSUM	55094
#define	PET3032_CHECKSUM_A	31319
#define	PET3032_CHECKSUM_B	33410
#define	PET4032_CHECKSUM_A	1643
#define	PET4032_CHECKSUM_B	14731
#define	PET8032_CHECKSUM_A	8647	/* my kernel (a.fachat) */
#define	PET8032_CHECKSUM_B	8227	/* jouko's kernel */

extern int pet_mem_init_resources(void);
extern int pet_mem_init_cmdline_options(void);

#endif
