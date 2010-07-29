/*
 * easyflash.c - Cartridge handling of the easyflash cart.
 *
 * Written by
 *  ALeX Kazik <alx@kazik.de>
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

#include <stdio.h>
#include <string.h>

#include "archdep.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "cartridge.h"
#include "cmdline.h"
#include "crt.h"
#include "easyflash.h"
#include "flash040.h"
#include "lib.h"
#include "log.h"
#include "maincpu.h"
#include "mem.h"
#include "resources.h"
#include "translate.h"

/* the 27F040B statemachine */
static flash040_context_t *easyflash_state_low = NULL;
static flash040_context_t *easyflash_state_high = NULL;

/* the jumper */
static int easyflash_jumper;

/* writing back to crt enabled */
static int easyflash_crt_write;

/* backup of the registers */
static BYTE easyflash_register_00, easyflash_register_02;

/* decoding table of the modes */
static const BYTE easyflash_memconfig[] = {
       /* bit3 = jumper, bit2 = mode, bit1 = !exrom, bit0 = game */

       /* jumper off, mode 0, trough 00,01,10,11 in game/exrom bits */
    3, /* exrom high, game low, jumper off */
    3, /* Reserved, don't use this */
    1, /* exrom low, game low, jumper off */
    1, /* Reserved, don't use this */

       /* jumper off, mode 1, trough 00,01,10,11 in game/exrom bits */
    2, 3, 0, 1,

       /* jumper on, mode 0, trough 00,01,10,11 in game/exrom bits */
    2, /* exrom high, game low, jumper on */
    3, /* Reserved, don't use this */
    0, /* exrom low, game low, jumper on */
    1, /* Reserved, don't use this */

       /* jumper on, mode 1, trough 00,01,10,11 in game/exrom bits */
    2, 3, 0, 1,
};

/* extra RAM */
static BYTE easyflash_ram[256];

/* filename when attached */
static char *easyflash_crt_filename = NULL;

static const char STRING_EASYFLASH[] = "EasyFlash Cartridge";

/* ---------------------------------------------------------------------*/

static void REGPARM2 easyflash_io1_store(WORD addr, BYTE value)
{
    BYTE mem_mode;

    switch (addr & 2) {
        case 0:
            /* bank register */
            easyflash_register_00 = value & 0x3f; /* we only remember 6 bits */
            break;
        default:
            /* mode register */
            easyflash_register_02 = value & 0x87; /* we only remember led, mode, exrom, game */
            mem_mode = easyflash_memconfig[(easyflash_jumper << 3) | (easyflash_register_02 & 0x07)];
            cartridge_config_changed(mem_mode, mem_mode, CMODE_READ);
            /* TODO: change led */
            /* (value & 0x80) -> led on if true, led off if false */
    }
    cartridge_romhbank_set(easyflash_register_00);
    cartridge_romlbank_set(easyflash_register_00);
    mem_pla_config_changed();
}

static BYTE REGPARM1 easyflash_io2_read(WORD addr)
{
    return easyflash_ram[addr & 0xff];
}

static void REGPARM2 easyflash_io2_store(WORD addr, BYTE value)
{
    easyflash_ram[addr & 0xff] = value;
}

/* ---------------------------------------------------------------------*/

static io_source_t easyflash_io1_device = {
    "Easy Flash",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0,
    easyflash_io1_store,
    NULL,
    NULL, /* TODO: peek */
    NULL, /* TODO: dump */
    CARTRIDGE_EASYFLASH
};

static io_source_t easyflash_io2_device = {
    "Easy Flash",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    1, /* read is always valid */
    easyflash_io2_store,
    easyflash_io2_read,
    NULL, /* TODO: peek */
    NULL, /* TODO: dump */
    CARTRIDGE_EASYFLASH
};

static io_source_list_t *easyflash_io1_list_item = NULL;
static io_source_list_t *easyflash_io2_list_item = NULL;

static const c64export_resource_t export_res = {
    "Easy Flash", 1, 1, &easyflash_io1_device, &easyflash_io2_device, CARTRIDGE_EASYFLASH
};

/* ---------------------------------------------------------------------*/

static int easyflash_check_empty(BYTE *data)
{
    int i;

    for (i = 0; i < 0x2000; i++) {
        if (data[i] != 0xff) {
            return 0;
        }
    }
    return 1;
}

static int set_easyflash_jumper(int val, void *param)
{
    easyflash_jumper = val;
    return 0;
}

static int set_easyflash_crt_write(int val, void *param)
{
    easyflash_crt_write = val;
    return 0;
}

/* ---------------------------------------------------------------------*/

static const resource_int_t resources_int[] = {
    { "EasyFlashJumper", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &easyflash_jumper, set_easyflash_jumper, NULL },
    { "EasyFlashWriteCRT", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &easyflash_crt_write, set_easyflash_crt_write, NULL },
    { NULL }
};

int easyflash_resources_init(void)
{
    return resources_register_int(resources_int);
}

/* ---------------------------------------------------------------------*/

static const cmdline_option_t cmdline_options[] =
{
    { "-easyflashjumper", SET_RESOURCE, 0,
      NULL, NULL, "EasyFlashJumper", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_EASYFLASH_JUMPER,
      NULL, NULL },
    { "+easyflashjumper", SET_RESOURCE, 0,
      NULL, NULL, "EasyFlashJumper", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_EASYFLASH_JUMPER,
      NULL, NULL },
    { "-easyflashcrtwrite", SET_RESOURCE, 0,
      NULL, NULL, "EasyFlashWriteCRT", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_EASYFLASH_CRT_WRITING,
      NULL, NULL },
    { "+easyflashcrtwrite", SET_RESOURCE, 0,
      NULL, NULL, "EasyFlashWriteCRT", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_EASYFLASH_CRT_WRITING,
      NULL, NULL },
    { NULL }
};

int easyflash_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 easyflash_roml_read(WORD addr)
{
    return flash040core_read(easyflash_state_low, (easyflash_register_00 * 0x2000) + (addr & 0x1fff));
}

void REGPARM2 easyflash_roml_store(WORD addr, BYTE value)
{
    flash040core_store(easyflash_state_low, (easyflash_register_00 * 0x2000) + (addr & 0x1fff), value);
}

BYTE REGPARM1 easyflash_romh_read(WORD addr)
{
    return flash040core_read(easyflash_state_high, (easyflash_register_00 * 0x2000) + (addr & 0x1fff));
}

void REGPARM2 easyflash_romh_store(WORD addr, BYTE value)
{
    flash040core_store(easyflash_state_high, (easyflash_register_00 * 0x2000) + (addr & 0x1fff), value);
}

/* ---------------------------------------------------------------------*/

void easyflash_config_init(void)
{
    easyflash_io1_store((WORD)0xde00, 0);
    easyflash_io1_store((WORD)0xde02, 0);
}

void easyflash_config_setup(BYTE *rawcart)
{
    easyflash_state_low = lib_malloc(sizeof(flash040_context_t));
    easyflash_state_high = lib_malloc(sizeof(flash040_context_t));

    flash040core_init(easyflash_state_low, maincpu_alarm_context, FLASH040_TYPE_B, roml_banks);
    memcpy(easyflash_state_low->flash_data, rawcart, 0x80000);

    flash040core_init(easyflash_state_high, maincpu_alarm_context, FLASH040_TYPE_B, romh_banks);
    memcpy(easyflash_state_high->flash_data, rawcart + 0x80000, 0x80000);
}

/* ---------------------------------------------------------------------*/
static int easyflash_common_attach(const char *filename)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    easyflash_io1_list_item = c64io_register(&easyflash_io1_device);
    easyflash_io2_list_item = c64io_register(&easyflash_io2_device);

    easyflash_crt_filename = lib_stralloc(filename);

    return 0;
}

int easyflash_crt_attach(FILE *fd, BYTE *rawcart, BYTE *header, const char *filename)
{
    BYTE chipheader[0x10];
    WORD bank, offset, length;

    memset(rawcart, 0xff, 0x100000);

    while (1) {
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            break;
        }

        bank = (chipheader[0xa] << 8) | chipheader[0xb];
        offset = (chipheader[0xc] << 8) | chipheader[0xd];
        length = (chipheader[0xe] << 8) | chipheader[0xf];

        if (length == 0x2000) {
            if (bank >= 64 || !(offset == 0x8000 || offset == 0xa000 || offset == 0xe000)) {
                return -1;
            }
            if (fread(&rawcart[(bank << 13) | (offset == 0x8000 ? 0<<19 : 1<<19)], 0x2000, 1, fd) < 1) {
                return -1;
            }
        } else if (length == 0x4000) {
            if (bank >= 64 || offset != 0x8000) {
                return -1;
            }
            if (fread(&rawcart[(bank << 13) | (0<<19)], 0x2000, 1, fd) < 1) {
                return -1;
            }
            if (fread(&rawcart[(bank << 13) | (1<<19)], 0x2000, 1, fd) < 1) {
                return -1;
            }
        } else {
            return -1;
        }
    }

    return easyflash_common_attach(filename);
}

void easyflash_detach(void)
{
    if (easyflash_crt_write) {
        easyflash_save_crt();
    }
    flash040core_shutdown(easyflash_state_low);
    flash040core_shutdown(easyflash_state_high);
    lib_free(easyflash_state_low);
    lib_free(easyflash_state_high);
    lib_free(easyflash_crt_filename);
    easyflash_crt_filename = NULL;
    c64io_unregister(easyflash_io1_list_item);
    c64io_unregister(easyflash_io2_list_item);
    easyflash_io1_list_item = NULL;
    easyflash_io2_list_item = NULL;
    c64export_remove(&export_res);
}

int easyflash_save_crt(void)
{
    FILE *fd;
    BYTE header[0x40], chipheader[0x10];
    BYTE *data;
    int i;

    if (easyflash_crt_filename == NULL) {
        return -1;
    }

    fd = fopen(easyflash_crt_filename, MODE_WRITE);

    if (fd == NULL) {
        return -1;
    }

    memset(header, 0x0, 0x40);
    memset(chipheader, 0x0, 0x10);

    strcpy((char *)header, CRT_HEADER);

    header[0x13] = 0x40;
    header[0x14] = 0x01;
    header[0x17] = CARTRIDGE_EASYFLASH;
    header[0x18] = 0x01;
    strcpy((char *)&header[0x20], STRING_EASYFLASH);
    if (fwrite(header, 1, 0x40, fd) != 0x40) {
        fclose(fd);
        return -1;
    }

    strcpy((char *)chipheader, CHIP_HEADER);
    chipheader[0x06] = 0x20;
    chipheader[0x07] = 0x10;
    chipheader[0x09] = 0x02;
    chipheader[0x0e] = 0x20;
 
    for (i = 0; i < 128; i++) {
        if (i > 63) {
            data = easyflash_state_high->flash_data + ((i - 64) * 0x2000);
        } else {
            data = easyflash_state_low->flash_data + (i * 0x2000);
        }

        if (easyflash_check_empty(data) == 0) {
            chipheader[0x0b] = (i > 63) ? i - 64 : i;
            chipheader[0x0c] = (i > 63) ? 0xa0 : 0x80;

            if (fwrite(chipheader, 1, 0x10, fd) != 0x10) {
                fclose(fd);
                return -1;
            }

            if (fwrite(data, 1, 0x2000, fd) != 0x2000) {
                fclose(fd);
                return -1;
            }
        }
    }
    fclose(fd);
    return 0;
}
