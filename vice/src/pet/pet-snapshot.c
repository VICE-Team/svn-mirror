/*
 * pet-snapshot.c - PET snapshot handling.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

/*
 * FIXME: the rom_*_loaded flag stuff is not clear enough.
 *
 */

#include "vice.h"

#include <stdio.h>

#include "autostart.h"
#include "kbdbuf.h"
#include "log.h"
#include "mem.h"
#include "pet.h"
#include "petmem.h"
#include "pets.h"
#include "resources.h"
#include "snapshot.h"
#include "tape.h"
#include "types.h"


static log_t pet_snapshot_log = LOG_ERR;

/*
 * PET memory dump should be 4-32k or 128k, depending on the config, as RAM.
 * Plus 64k expansion RAM (8096 or SuperPET) if necessary. Also there
 * is the 1/2k video RAM as "VRAM".
 * In this prototype we save the full ram......
 */

static const char module_ram_name[] = "PETMEM";
#define PETMEM_DUMP_VER_MAJOR   1
#define PETMEM_DUMP_VER_MINOR   2

/*
 * UBYTE        CONFIG          Bits 0-3: 0 = 40 col PET without CRTC
 *                                        1 = 40 col PET with CRTC
 *                                        2 = 80 col PET (with CRTC)
 *                                        3 = SuperPET
 *                                        4 = 8096
 *                                        5 = 8296
 *                              Bit 6: 1= RAM at $9***
 *                              Bit 7: 1= RAM at $A***
 *
 * UBYTE        KEYBOARD        0 = UK business
 *                              1 = graphics
 *
 * UBYTE        MEMSIZE         memory size of low 32k in k (4,8,16,32)
 *
 * UBYTE        CONF8X96        8x96 configuration register
 * UBYTE        SUPERPET        SuperPET config:
 *                              Bit 0: spet_ramen,  1= RAM enabled
 *                                  1: spet_ramwp,  1= RAM write protected
 *                                  2: spet_ctrlwp, 1= CTRL reg write prot.
 *                                  3: spet_diag,   0= diag active
 *                                  4-7: spet_bank, RAM block in use
 *
 * ARRAY        RAM             4-32k RAM (not 8296, dep. on MEMSIZE)
 * ARRAY        VRAM            2/4k RAM (not 8296, dep in CONFIG)
 * ARRAY        EXTRAM          64k (SuperPET and 8096 only)
 * ARRAY        RAM             128k RAM (8296 only)
 *
 *                              Added in format V1.1, should be part of
 *                              KEYBOARD in later versions.
 *
 * BYTE         POSITIONAL      bit 0=0 = symbolic keyboard mapping
 *                                   =1 = positional keyboard mapping
 *
 *                              Added in format V1.2
 * BYTE         EOIBLANK        bit 0=0: EOI does not blank screen
 *                                   =1: EOI does blank screen
 */

static int mem_write_ram_snapshot_module(snapshot_t *p)
{
    snapshot_module_t *m;
    BYTE config, rconf, memsize, conf8x96, superpet;
    int kbdindex;

    memsize = petres.ramSize;
    if (memsize > 32) {
        memsize = 32;
    }

    if (!petres.crtc) {
        config = 0;
    } else {
        config = petres.videoSize == 0x400 ? 1 : 2;
    }

    if (petres.map) {
        config = petres.map + 3;
    } else {
    if (petres.superpet)
        config = 3;
    }

    rconf = (petres.mem9 ? 0x40 : 0)
                | (petres.memA ? 0x80 : 0) ;

    conf8x96 = map_reg;

    superpet = (spet_ramen ? 1 : 0)
                | (spet_ramwp ? 2 : 0)
                | (spet_ctrlwp ? 4 : 0)
                | (spet_diag ? 8 : 0)
                | ((spet_bank << 4) & 0xf0) ;

    m = snapshot_module_create(p, module_ram_name,
                               PETMEM_DUMP_VER_MAJOR, PETMEM_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;
    snapshot_module_write_byte(m, (BYTE)(config | rconf));

    resources_get_value("KeymapIndex", (resource_value_t*) &kbdindex);
    snapshot_module_write_byte(m, (BYTE)(kbdindex >> 1));

    snapshot_module_write_byte(m, memsize);
    snapshot_module_write_byte(m, conf8x96);
    snapshot_module_write_byte(m, superpet);

    if (config != 5) {
        snapshot_module_write_byte_array(m, ram, memsize << 10);

        snapshot_module_write_byte_array(m, ram + 0x8000,
                                         (config < 2) ? 0x400 : 0x800);

        if (config == 3 || config == 4) {
            snapshot_module_write_byte_array(m, ram + 0x10000, 0x10000);
        }
    } else {    /* 8296 */
        snapshot_module_write_byte_array(m, ram, 0x20000);
    }

    snapshot_module_write_byte(m, (BYTE)(kbdindex & 1));
    snapshot_module_write_byte(m, (BYTE)(petres.eoiblank ? 1 : 0));

    snapshot_module_close(m);

    return 0;
}

static int mem_read_ram_snapshot_module(snapshot_t *p)
{
    BYTE vmajor, vminor;
    snapshot_module_t *m;
    BYTE config, rconf, byte, memsize, conf8x96, superpet;
    petinfo_t peti = { 32, 0x0800, 1, 80, 0, 0, 0, 0, 0, 0, 0,
                       NULL, NULL, NULL, NULL, NULL, NULL };

    m = snapshot_module_open(p, module_ram_name, &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (vmajor != PETMEM_DUMP_VER_MAJOR) {
        log_error(pet_snapshot_log,
                  "Cannot load PET RAM module with major version %d",
                  vmajor);
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_read_byte(m, &config);

    snapshot_module_read_byte(m, &byte);
    peti.kbd_type = byte;

    snapshot_module_read_byte(m, &memsize);
    snapshot_module_read_byte(m, &conf8x96);
    snapshot_module_read_byte(m, &superpet);

    rconf = config & 0xc0;
    config &= 0x0f;

    peti.ramSize = memsize;
    peti.crtc = 1;
    peti.IOSize = 0x800;
    peti.video = 80;
    peti.superpet = 0;

    switch (config) {
      case 0:           /* 40 cols w/o CRTC */
        peti.crtc = 0;
        peti.video = 40;
        break;
      case 1:           /* 40 cols w/ CRTC */
        peti.video = 40;
        break;
      case 2:           /* 80 cols (w/ CRTC) */
        break;
      case 3:           /* SuperPET */
        spet_ramen = superpet & 1;
        spet_ramwp = superpet & 2;
        spet_ctrlwp= superpet & 4;
        spet_diag  = superpet & 8;
        spet_bank  = (superpet >> 4) & 0x0f;
        peti.superpet = 1;
        break;
      case 4:           /* 8096 */
        peti.ramSize = 96;
        break;
      case 5:           /* 8296 */
        peti.ramSize = 128;
        break;
    };

    peti.mem9 = (rconf & 0x40) ? 1 : 0;
    peti.memA = (rconf & 0x80) ? 1 : 0;

    pet_set_conf_info(&peti);  /* set resources and config accordingly */
    map_reg = conf8x96;

    mem_initialize_memory();

    pet_crtc_set_screen();

    if (config != 5) {
        snapshot_module_read_byte_array(m, ram, memsize << 10);

        snapshot_module_read_byte_array(m, ram + 0x8000,
                                        (config < 2) ? 0x400 : 0x800);

        if (config == 3 || config == 4) {
            snapshot_module_read_byte_array(m, ram + 0x10000, 0x10000);
        }
    } else {    /* 8296 */
        snapshot_module_read_byte_array(m, ram, 0x20000);
    }

    if (vminor > 0) {
        int kindex;
        snapshot_module_read_byte(m, &byte);
        resources_get_value("KeymapIndex",
                            (resource_value_t *)&kindex);
        resources_set_value("KeymapIndex",
                            (resource_value_t)((kindex & ~1) | (byte & 1)));
    }
    if (vminor > 1) {
        snapshot_module_read_byte(m, &byte);
        resources_set_value("EoiBlank",
                            (resource_value_t)(byte & 1));
    }

    snapshot_module_close(m);

    return 0;
}

static const char module_rom_name[] = "PETROM";
#define PETROM_DUMP_VER_MAJOR   1
#define PETROM_DUMP_VER_MINOR   0

/*
 * UBYTE        CONFIG          Bit 0: 1= $9*** ROM included
 *                                  1: 1= $a*** ROM included
 *                                  2: 1= $b*** ROM included
 *                                  3: 1= $e900-$efff ROM included
 *
 * ARRAY        KERNAL          4k KERNAL ROM image $f000-$ffff
 * ARRAY        EDITOR          2k EDITOR ROM image $e000-$e800
 * ARRAY        CHARGEN         2k CHARGEN ROM image
 * ARRAY        ROM9            4k $9*** ROM (if CONFIG & 1)
 * ARRAY        ROMA            4k $A*** ROM (if CONFIG & 2)
 * ARRAY        ROMB            4k $B*** ROM (if CONFIG & 4)
 * ARRAY        ROMC            4k $C*** ROM
 * ARRAY        ROMD            4k $D*** ROM
 * ARRAY        ROME9           7 blocks $e900-$efff ROM (if CONFIG & 8)
 *
 */

static int mem_write_rom_snapshot_module(snapshot_t *p, int save_roms)
{
    snapshot_module_t *m;
    BYTE config;
    int i, trapfl;

    if (!save_roms)
        return 0;

    m = snapshot_module_create(p, module_rom_name,
                               PETROM_DUMP_VER_MAJOR, PETROM_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    /* disable traps before saving the ROM */
    resources_get_value("VirtualDevices", (resource_value_t*) &trapfl);
    resources_set_value("VirtualDevices", (resource_value_t) 1);
    petmem_unpatch_2001();

    config = (rom_9_loaded ? 1 : 0)
             | (rom_A_loaded ? 2 : 0)
             | (rom_B_loaded ? 4 : 0)
             | ((petres.ramSize == 128) ? 8 : 0);

    snapshot_module_write_byte(m, config);

    {
        snapshot_module_write_byte_array(m, rom + 0x7000, 0x1000);
        snapshot_module_write_byte_array(m, rom + 0x6000, 0x0800);

        /* pick relevant data from chargen ROM */
        for (i = 0; i < 128; i++) {
            snapshot_module_write_byte_array(m, chargen_rom + i * 16, 8);
        }
        for (i = 0; i < 128; i++) {
            snapshot_module_write_byte_array(m, chargen_rom + 0x1000 + i * 16,
                                             8);
        }

        if (config & 1) {
            snapshot_module_write_byte_array(m, rom + 0x1000, 0x1000);
        }
        if (config & 2) {
            snapshot_module_write_byte_array(m, rom + 0x2000, 0x1000);
        }
        if (config & 4) {
            snapshot_module_write_byte_array(m, rom + 0x3000, 0x1000);
        }

        snapshot_module_write_byte_array(m, rom + 0x4000, 0x2000);

        if (config & 8) {
           snapshot_module_write_byte_array(m, rom + 0x6900, 0x0700);
        }
    }

    /* enable traps again when necessary */
    resources_set_value("VirtualDevices", (resource_value_t) trapfl);
    petmem_patch_2001();

    snapshot_module_close(m);

    return 0;
}

static int mem_read_rom_snapshot_module(snapshot_t *p)
{
    BYTE vmajor, vminor;
    snapshot_module_t *m;
    BYTE config;
    int trapfl, new_iosize;

    m = snapshot_module_open(p, module_rom_name, &vmajor, &vminor);
    if (m == NULL)
        return 0;       /* optional */

    if (vmajor != PETROM_DUMP_VER_MAJOR) {
        log_error(pet_snapshot_log,
                "Cannot load PET ROM module with major version %d",
                vmajor);
        snapshot_module_close(m);
        return -1;
    }

    /* disable traps before loading the ROM */
    resources_get_value("VirtualDevices", (resource_value_t*)&trapfl);
    resources_set_value("VirtualDevices", (resource_value_t)1);
    petmem_unpatch_2001();

    config = (rom_9_loaded ? 1 : 0)
             | (rom_A_loaded ? 2 : 0)
             | (rom_B_loaded ? 4 : 0)
             | ((petres.pet2k || petres.ramSize == 128) ? 8 : 0);

    snapshot_module_read_byte(m, &config);

    /* De-initialize kbd-buf, autostart and tape stuff here before
       loading the new ROMs. These depend on addresses defined in the
       rom - they might be different in the loaded ROM. */
    kbd_buf_init(0, 0, 0, 0);
    autostart_init(0, 0, 0, 0, 0, 0);
    tape_deinstall();

    rom_9_loaded = config & 1;
    rom_A_loaded = config & 2;
    rom_B_loaded = config & 4;

    if (config & 8) {
        new_iosize = 0x100;
    } else {
        new_iosize = 0x800;
    }
    if (new_iosize != petres.IOSize) {
        petres.IOSize = new_iosize;
        mem_initialize_memory();
    }

    {
        /* kernal $f000-$ffff */
        snapshot_module_read_byte_array(m, rom + 0x7000, 0x1000);
        /* editor $e000-$e7ff */
        snapshot_module_read_byte_array(m, rom + 0x6000, 0x0800);

        /* chargen ROM */
        resources_set_value("Basic1Chars", (resource_value_t) 0);
        snapshot_module_read_byte_array(m, chargen_rom, 0x0800);
        petmem_convert_chargen(chargen_rom);

        /* $9000-$9fff */
        if (config & 1) {
            snapshot_module_read_byte_array(m, rom + 0x1000, 0x1000);
        }
        /* $a000-$afff */
        if (config & 2) {
            snapshot_module_read_byte_array(m, rom + 0x2000, 0x1000);
        }
        /* $b000-$bfff */
        if (config & 4) {
            snapshot_module_read_byte_array(m, rom + 0x3000, 0x1000);
        }

        /* $c000-$dfff */
        snapshot_module_read_byte_array(m, rom + 0x4000, 0x2000);

        /* $e900-$efff editor extension */
        if (config & 8) {
            snapshot_module_read_byte_array(m, rom + 0x6900, 0x0700);
        }
    }

    log_warning(pet_snapshot_log,"Dumped Romset files and saved settings will "
                "represent\nthe state before loading the snapshot!");

    petres.rompatch = 0;

    petmem_get_kernal_checksum();
    petmem_get_editor_checksum();
    petmem_checksum();

    petmem_patch_2001();

    /* enable traps again when necessary */
    resources_set_value("VirtualDevices", (resource_value_t) trapfl);

    snapshot_module_close(m);

    return 0;
}

int pet_snapshot_write_module(snapshot_t *m, int save_roms) {
    if (mem_write_ram_snapshot_module(m) < 0
        || mem_write_rom_snapshot_module(m, save_roms) < 0 )
        return -1;
    return 0;
}

int pet_snapshot_read_module(snapshot_t *m) {
    if (mem_read_ram_snapshot_module(m) < 0
        || mem_read_rom_snapshot_module(m) < 0 )
        return -1;
    return 0;
}

