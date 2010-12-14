/*
 * ide64.c - Cartridge handling, IDE64 cart.
 *
 * Written by
 *  Kajtar Zsolt <soci@c64.rulez.org>
 *
 * Real-Time-Clock patches by
 *  Greg King <greg.king4@verizon.net>
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
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "archdep.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "cartridge.h"
#include "cmdline.h"
#include "ds1302.h"
#include "ide64.h"
#include "log.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "translate.h"
#include "types.h"
#include "util.h"
#include "vicii-phi1.h"

#define IDE_BSY  0x80
#define IDE_DRDY 0x40
#define IDE_DF   0x20
#define IDE_DSC  0x10
#define IDE_DRQ  0x08
#define IDE_CORR 0x04
#define IDE_IDX  0x02
#define IDE_ERR  0x01

#define IDE_UNC  0x40
#define IDE_MC   0x20
#define IDE_IDNF 0x10
#define IDE_MCR  0x08
#define IDE_ABRT 0x04
#define IDE_TK0N 0x02
#define IDE_AMNF 0x01

/* Flag: cartridge enabled */
static int ide64_enabled;

/* Current IDE64 bank */
static unsigned int current_bank;

/* Current memory config */
static unsigned int current_cfg;

/* ds1302 context */
static rtc_ds1302_t *ds1302_context = NULL;

/*  */
static BYTE kill_port;

/* IDE registers */
struct drive_t {
    BYTE ide_error;
    BYTE ide_features;
    BYTE ide_sector_count, ide_sector_count_internal;
    BYTE ide_sector;
    BYTE ide_cylinder_low;
    BYTE ide_cylinder_high;
    BYTE ide_head;
    BYTE ide_status;
    BYTE ide_control;
    BYTE ide_cmd;
    unsigned int ide_bufp;
    char *ide64_image_file;
    BYTE buffer[512];
    BYTE ide_identify[128];
    FILE *ide_disk;
    unsigned int settings_cylinders, settings_heads, settings_sectors;
};

static struct drive_t drives[4], *cdrive = NULL;
static int idrive = -1;

/* communication latch */
static WORD out_d030, in_d030;

/* config ram */
static char ide64_DS1302[65];

static char *ide64_configuration_string = NULL;

static int settings_autodetect_size, settings_version4, rtc_offset;

static BYTE hdd_identify[128] = {
    0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x30, 0x32, 0x30, 0x31,
    0x37, 0x30, 0x32, 0x30, 0x20, 0x20, 0x20, 0x20,

    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x33,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x49, 0x56,
    0x45, 0x43, 0x48, 0x2d, 0x44, 0x44, 0x20, 0x20,

    0x20, 0x20, 0x41, 0x4b, 0x54, 0x4a, 0x52, 0x41,
    0x5a, 0x20, 0x4f, 0x53, 0x54, 0x4c, 0x28, 0x20,
    0x4f, 0x53, 0x49, 0x43, 0x53, 0x2f, 0x4e, 0x49,
    0x55, 0x47, 0x41, 0x4c, 0x29, 0x52, 0x01, 0x00,

    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x04, 0x00,
    0x10, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* ---------------------------------------------------------------------*/

/* some prototypes are needed */
static void REGPARM2 ide64_io1_store(WORD addr, BYTE value);
static BYTE REGPARM1 ide64_io1_read(WORD addr);

static io_source_t ide64_device = {
    CARTRIDGE_NAME_IDE64,
    IO_DETACH_CART,
    NULL,
    0xde20, 0xdeff, 0xff,
    0,
    ide64_io1_store,
    ide64_io1_read,
    NULL, /* TODO: peek */
    NULL, /* TODO: dump */
    CARTRIDGE_IDE64
};

static io_source_list_t *ide64_list_item = NULL;

static const c64export_resource_t export_res = {
    CARTRIDGE_NAME_IDE64, 1, 1, &ide64_device, NULL, CARTRIDGE_IDE64
};

/* ---------------------------------------------------------------------*/
/* drive reset response */
static void ide64_reset(struct drive_t *cdrive)
{
    cdrive->ide_error = 1;
    cdrive->ide_sector_count = 1;
    cdrive->ide_sector = 1;
    cdrive->ide_cylinder_low = 0;
    cdrive->ide_cylinder_high = 0;
    cdrive->ide_head = 0;
    cdrive->ide_status = IDE_DRDY | IDE_DSC;
    cdrive->ide_bufp = 510;
    cdrive->ide_cmd = 0x00;
}

static int ide64_disk_attach(struct drive_t *cdrive)
{
    if (!ide64_enabled) {
        return 0;
    }

    ide64_reset(cdrive);

    if (cdrive->ide_disk != NULL) {
        fclose(cdrive->ide_disk);
    }

    cdrive->ide_disk = NULL;

    if (!cdrive->ide64_image_file[0]) {
        return 0;
    }

    cdrive->ide_disk = fopen(cdrive->ide64_image_file, MODE_READ_WRITE);

    if (!cdrive->ide_disk) {
        cdrive->ide_disk = fopen(cdrive->ide64_image_file, MODE_APPEND);
    }

    if (!cdrive->ide_disk) {
        cdrive->ide_disk = fopen(cdrive->ide64_image_file, MODE_READ);
    }

    if (cdrive->ide_disk) {
        log_message(LOG_DEFAULT, "IDE64: Using imagefile `%s'.", cdrive->ide64_image_file);
    } else {
        log_message(LOG_DEFAULT, "IDE64: Cannot use image file `%s'. NO DRIVE EMULATION!", cdrive->ide64_image_file);
    }

    memcpy(cdrive->ide_identify, hdd_identify, sizeof(hdd_identify));

    if (!settings_autodetect_size) {
        return 0;
    }

    if (cdrive->ide_disk) {
        /* try to get drive geometry */
        unsigned char idebuf[24];
        int  heads, sectors, cyll, cylh, cyl, res;
        unsigned long size = 0;
        int is_chs;

        /* read header */
        res = (int)fread(idebuf, 1, 24, cdrive->ide_disk);
        if (res < 24) {
            log_message(LOG_DEFAULT, "IDE64: Couldn't read disk geometry from image, using default 8 MiB.");
            return 0;
        }
        /* check signature */

        for (;;) {

            res = memcmp(idebuf,"C64-IDE V", 9);

            if (res == 0) { /* old filesystem always CHS */
                cyl = (idebuf[0x10] << 8) | idebuf[0x11];
                heads = idebuf[0x12] & 0x0f;
                sectors = idebuf[0x13];
                is_chs = 1;
                break;  /* OK */
            }

            res = memcmp(idebuf + 8, "C64 CFS V", 9);

            if (res == 0) {
                if (idebuf[0x04] & 0x40) { /* LBA */
                    size = ((idebuf[0x04] & 0x0f) << 24) | (idebuf[0x05] << 16) | (idebuf[0x06] << 8) | idebuf[0x07];
                    cyl = heads = sectors = 1; /* fake */
                    is_chs = 0;
                } else { /* CHS */
                    cyl = (idebuf[0x05] << 8) | idebuf[0x06];
                    heads = idebuf[0x04] & 0x0f;
                    sectors = idebuf[0x07];
                    is_chs = 1;
                }
                break;  /* OK */
            }

            log_message(LOG_DEFAULT, "IDE64: Disk is not formatted, using default 8 MiB.");
            return 0;
        }

        if (is_chs) {
            cyl++;
            heads++;
            size = cyl * heads * sectors;
            log_message(LOG_DEFAULT, "IDE64: using %i/%i/%i CHS geometry, %lu sectors total.", cyl, heads, sectors, size);
        } else {
            log_message(LOG_DEFAULT, "IDE64: LBA geometry, %lu sectors total.", size);
        }


        cdrive->settings_cylinders = cyl;
        cdrive->settings_heads = heads;
        cdrive->settings_sectors = sectors;

        cyll = cyl & 0xff;
        cylh = cyl >> 8;
        cdrive->ide_identify[0x02] = cyll;
        cdrive->ide_identify[108] = cyll;
        cdrive->ide_identify[0x03] = cylh;
        cdrive->ide_identify[109] = cylh;
        cdrive->ide_identify[0x06] = heads;
        cdrive->ide_identify[110] = heads;
        cdrive->ide_identify[0x0c] = sectors;
        cdrive->ide_identify[112] = sectors;

        cdrive->ide_identify[114] = (BYTE)(size & 0xff);
        size >>= 8;
        cdrive->ide_identify[115] = (BYTE)(size & 0xff);
        size >>= 8;
        cdrive->ide_identify[116] = (BYTE)(size & 0xff);
        size >>= 8;
        cdrive->ide_identify[117] = (BYTE)(size & 0xff);

        memcpy(cdrive->ide_identify + 120, cdrive->ide_identify + 114, 4);
    }

    return 0;
}

static void geometry_update(struct drive_t *cdrive)
{
    cdrive->ide_identify[108] = cdrive->settings_cylinders & 255;
    cdrive->ide_identify[109] = cdrive->settings_cylinders >> 8;
    cdrive->ide_identify[110] = cdrive->settings_heads;
    cdrive->ide_identify[112] = cdrive->settings_sectors;
}

static int set_ide64_config(const char *cfg, void *param)
{
    int i;

    ide64_DS1302[64] = 0;
    memset(ide64_DS1302, 0x40, 64);

    if (cfg) {
        for (i = 0; cfg[i] && i < 64; i++) {
            ide64_DS1302[i] = cfg[i];
        }
    }
    util_string_set(&ide64_configuration_string, ide64_DS1302);
    try_cartridge_init(16);
    return 0;
}

static int set_ide64_image_file(const char *name, void *param)
{
    int i = vice_ptr_to_int(param);

    util_string_set(&drives[i].ide64_image_file, name);

    return ide64_disk_attach(&drives[i]);
}

static int set_cylinders(int val, void *param)
{
    unsigned int cylinders = (unsigned int)val;

    if (cylinders > 1024) {
        return -1;
    }

    drives[0].settings_cylinders = cylinders;
    geometry_update(&drives[0]);

    return 0;
}

static int set_heads(int val, void *param)
{
    unsigned int heads = (unsigned int)val;

    if (heads > 16) {
        return -1;
    }

    drives[0].settings_heads = heads;
    geometry_update(&drives[0]);

    return 0;
}

static int set_sectors(int val, void *param)
{
    unsigned int sectors = (unsigned int)val;

    if (sectors > 63) {
        return -1;
    }

    drives[0].settings_sectors = sectors;
    geometry_update(&drives[0]);

    return 0;
}

static int set_autodetect_size(int val, void *param)
{
    settings_autodetect_size = val;

    return 0;
}

static int set_version4(int val, void *param)
{
    if (settings_version4 != val) {
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    }

    settings_version4 = val;

    return 0;
}

static int set_rtc_offset(int val, void *param)
{
    rtc_offset = val;

    return 0;
}

static const resource_string_t resources_string[] = {
    { "IDE64Image1", "ide.hdd", RES_EVENT_NO, NULL,
      &drives[0].ide64_image_file, set_ide64_image_file, (void *)0 },
    { "IDE64Image2", "", RES_EVENT_NO, NULL,
      &drives[1].ide64_image_file, set_ide64_image_file, (void *)1 },
    { "IDE64Image3", "", RES_EVENT_NO, NULL,
      &drives[2].ide64_image_file, set_ide64_image_file, (void *)2 },
    { "IDE64Image4", "", RES_EVENT_NO, NULL,
      &drives[3].ide64_image_file, set_ide64_image_file, (void *)3 },
    { "IDE64Config", "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@", RES_EVENT_NO, NULL,
      &ide64_configuration_string, set_ide64_config, NULL },
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "IDE64Cylinders", 256,
      RES_EVENT_NO, NULL,
      (int *)&drives[0].settings_cylinders, set_cylinders, NULL },
    { "IDE64Heads", 4,
      RES_EVENT_NO, NULL,
      (int *)&drives[0].settings_heads, set_heads, NULL },
    { "IDE64Sectors", 16,
      RES_EVENT_NO, NULL,
      (int *)&drives[0].settings_sectors, set_sectors, NULL },
    { "IDE64AutodetectSize", 1,
      RES_EVENT_NO, NULL,
      &settings_autodetect_size, set_autodetect_size, NULL },
    { "IDE64version4", 0,
      RES_EVENT_NO, NULL,
      &settings_version4, set_version4, NULL },
    { "IDE64RTCOffset", 0,
      RES_EVENT_NO, NULL,
      (int *)&rtc_offset, set_rtc_offset, NULL },
    { NULL }
};

int ide64_resources_init(void)
{
    int i;

    if (resources_register_string(resources_string) < 0) {
        return -1;
    }
    if (resources_register_int(resources_int) < 0) {
        return -1;
    }

    for (i = 0; i < 4; i++) {
        drives[i].ide_disk = NULL;
    }

    ide64_enabled = 0;

    return 0;
}

int ide64_resources_shutdown(void)
{
    int i;

    lib_free(ide64_configuration_string);

    for (i = 0; i < 4; i++) {
        lib_free(drives[i].ide64_image_file);
        drives[i].ide64_image_file = NULL;
    }
    ide64_configuration_string = NULL;
    return 0;
}

static const cmdline_option_t cmdline_options[] = {
    { "-IDE64image1", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Image1", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_IDE64_NAME,
      NULL, NULL },
    { "-IDE64image2", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Image2", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_IDE64_NAME,
      NULL, NULL },
    { "-IDE64image3", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Image3", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_IDE64_NAME,
      NULL, NULL },
    { "-IDE64image4", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Image4", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_IDE64_NAME,
      NULL, NULL },
    { "-IDE64cyl", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Cylinders", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_CYLINDERS_IDE64,
      NULL, NULL },
    { "-IDE64hds", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Heads", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_HEADS_IDE64,
      NULL, NULL },
    { "-IDE64sec", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Sectors", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_SECTORS_IDE64,
      NULL, NULL },
    { "-IDE64autosize", SET_RESOURCE, 0,
      NULL, NULL, "IDE64AutodetectSize", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_AUTODETECT_IDE64_GEOMETRY,
      NULL, NULL },
    { "+IDE64autosize", SET_RESOURCE, 0,
      NULL, NULL, "IDE64AutodetectSize", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_NO_AUTODETECT_IDE64_GEOMETRY,
      NULL, NULL },
    { "-IDE64version4", SET_RESOURCE, 0,
      NULL, NULL, "IDE64version4", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, T_("Emulate version 4 hardware") },
    { "+IDE64version4", SET_RESOURCE, 0,
      NULL, NULL, "IDE64version4", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, T_("Emulate pre version 4 hardware") },
    { NULL }
};

int ide64_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* seek to a sector */
static int ide_seek_sector(void)
{
    unsigned int lba;

    if (cdrive->ide_head & 0x40) {
        lba = ((cdrive->ide_head & 0x0f) << 24) | (cdrive->ide_cylinder_high << 16) | (cdrive->ide_cylinder_low << 8) | cdrive->ide_sector;
        if (lba > (unsigned int)((cdrive->ide_identify[117] << 24) | (cdrive->ide_identify[116] << 16) | (cdrive->ide_identify[115] << 8) | cdrive->ide_identify[114])) {
            return 1;
        }
    } else {
        if (cdrive->ide_sector == 0 || cdrive->ide_sector > cdrive->ide_identify[112] || (cdrive->ide_head & 0xf) >= cdrive->ide_identify[110] ||
            (cdrive->ide_cylinder_low | (cdrive->ide_cylinder_high << 8)) >= (cdrive->ide_identify[108] | (cdrive->ide_identify[109] << 8))) {
            return 1;
        }
        lba = ((cdrive->ide_cylinder_low | (cdrive->ide_cylinder_high << 8)) * cdrive->ide_identify[110] + (cdrive->ide_head & 0xf)) * cdrive->ide_identify[112] + cdrive->ide_sector - 1;
    }
    return fseek(cdrive->ide_disk, lba << 9, SEEK_SET);
}

static BYTE REGPARM1 ide64_io1_read(WORD addr)
{
    int i;

    ide64_device.io_source_valid = 1;

    if (kill_port & 1) {
        if ((addr & 0xff) >= 0x5f) {
            ide64_device.io_source_valid = 0;
            return vicii_read_phi1();
        }
    }

    if ((addr & 0xff) >= 0x60) {
        return roml_banks[(addr & 0xff) | 0x1e00 | (current_bank << 14)];
    }

    switch (addr & 0xff) {
        case 0x20:
            switch (cdrive->ide_cmd) {
                case 0x20:
                case 0xec:
                case 0xe4:
                    in_d030 = cdrive->buffer[cdrive->ide_bufp] | (cdrive->buffer[cdrive->ide_bufp | 1] << 8);
                    if (cdrive->ide_bufp < 510) {
                        cdrive->ide_bufp += 2;
                    } else {
                        cdrive->ide_sector_count_internal--;
                        if (!cdrive->ide_sector_count_internal) {
                            cdrive->ide_status = cdrive->ide_status & (~IDE_DRQ);
                            cdrive->ide_cmd = 0x00;
                        } else {
                            memset(cdrive->buffer, 0, 512);
                            if (fread(cdrive->buffer, 1, 512, cdrive->ide_disk) != 512) {
                                cdrive->ide_error = IDE_UNC | IDE_ABRT;
                                cdrive->ide_status = (cdrive->ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ)) | IDE_DRDY | IDE_ERR;
                                cdrive->ide_bufp = 510;
                                cdrive->ide_cmd = 0x00;
                            }
                            cdrive->ide_bufp = 0;
                            cdrive->ide_status = cdrive->ide_status | IDE_DRQ;
                        }
                    }
                    break;
                default:
                    in_d030 = (WORD)vicii_read_phi1();
            }
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x21:
            in_d030 = cdrive->ide_error;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x22:
            in_d030 = cdrive->ide_sector_count;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x23:
            in_d030 = cdrive->ide_sector;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x24:
            in_d030 = cdrive->ide_cylinder_low;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x25:
            in_d030 = cdrive->ide_cylinder_high;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x26:
            in_d030 = cdrive->ide_head;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x27:
        case 0x2e:
            in_d030 = cdrive->ide_status;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2f:
            in_d030 = (WORD)vicii_read_phi1();
            break;
        case 0x30:
            if (settings_version4) {
                break;
            }
            return (unsigned char)in_d030;
        case 0x31:
            return in_d030 >> 8;
        case 0x32:
            return (settings_version4 ? 0x20 : 0x10) | (current_bank << 2) | (((current_cfg & 1) ^ 1) << 1) | (current_cfg >> 1);
        case 0x5f:
            i = vicii_read_phi1();
            if ((kill_port & 2) == 0) {
                return i;
            }
            i &= ~1;
            ds1302_set_lines(ds1302_context, 1u, 0u, 1u);
            i |= ds1302_read_data_line(ds1302_context);
            ds1302_set_lines(ds1302_context, 1u, 1u, 1u);
            return i;
    }
    ide64_device.io_source_valid = 0;
    return vicii_read_phi1();
}

BYTE ide64_get_killport(void)
{
    return kill_port;
}

static void REGPARM2 ide64_io1_store(WORD addr, BYTE value)
{
    if (kill_port & 1) {
        if ((addr & 0xff) >= 0x5f) {
            return;
        }
    }

    switch (addr & 0xff) {
        case 0x20:
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            switch (cdrive->ide_cmd) {
                case 0x30:
                case 0xe8:
                    cdrive->buffer[cdrive->ide_bufp] = out_d030 & 0xff;
                    cdrive->buffer[cdrive->ide_bufp | 1] = out_d030 >> 8;
                    if (cdrive->ide_bufp < 510) {
                        cdrive->ide_bufp += 2;
                    } else {
                        if (cdrive->ide_cmd != 0xe8) {
                            if (fwrite(cdrive->buffer, 1, 512, cdrive->ide_disk) != 512) {
                                cdrive->ide_error = IDE_UNC | IDE_ABRT;
                                goto aborted_command;
                            }
                        }
                        cdrive->ide_sector_count_internal--;
                        if (!cdrive->ide_sector_count_internal) {
                            if (cdrive->ide_cmd != 0xe8) fflush(cdrive->ide_disk);
                            cdrive->ide_status = (cdrive->ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ) & (~IDE_ERR)) | IDE_DRDY;
                            cdrive->ide_cmd = 0x00;
                        } else {
                            cdrive->ide_bufp = 0;
                            cdrive->ide_status |= IDE_DRQ;
                        }
                    }
                    break;
            }
            return;
        case 0x21:
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            drives[idrive & ~1].ide_features = drives[idrive | 1].ide_features = out_d030 & 0xff;
            return;
        case 0x22:
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            drives[idrive & ~1].ide_sector_count = drives[idrive | 1].ide_sector_count = out_d030 & 0xff;
            return;
        case 0x23:
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            drives[idrive & ~1].ide_sector = drives[idrive | 1].ide_sector = out_d030 & 0xff;
            return;
        case 0x24:
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            drives[idrive & ~1].ide_cylinder_low = drives[idrive | 1].ide_cylinder_low = out_d030 & 0xff;
            return;
        case 0x25:
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            drives[idrive & ~1].ide_cylinder_high = drives[idrive | 1].ide_cylinder_high = out_d030 & 0xff;
            return;
        case 0x26:
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            drives[idrive & ~1].ide_head = drives[idrive | 1].ide_head = out_d030 & 0xff;
            idrive = (idrive & ~1) | ((out_d030 >> 4) & 1);
            cdrive = &drives[idrive];
            return;
        case 0x27:
            if (!cdrive->ide_disk) {
                return; /* if image file exists? */
            }
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            switch (out_d030 & 0xff) {
                case 0x20:
                case 0x21:
                    cdrive->ide_status = (cdrive->ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ) & (~IDE_ERR)) | IDE_DRDY;
#ifdef IDE64_DEBUG
                    if (cdrive->ide_head & 0x40) {
                        log_debug("IDE64 %d READ (%d)*%d", idrive, (cdrive->ide_cylinder_low << 8) | (cdrive->ide_cylinder_high << 16) |
                                  ((cdrive->ide_head & 0xf) << 24) | cdrive->ide_sector, cdrive->ide_sector_count);
                    } else {
                        log_debug("IDE64 %d READ (%d/%d/%d)*%d", idrive, cdrive->ide_cylinder_low | (cdrive->ide_cylinder_high << 8),
                                  cdrive->ide_head & 0xf, cdrive->ide_sector, cdrive->ide_sector_count);
                    }
#endif
                    if (ide_seek_sector()) {
                        cdrive->ide_error = IDE_IDNF;
                        goto aborted_command;
                    }
                    memset(cdrive->buffer, 0, 512);
                    if (fread(cdrive->buffer, 1, 512, cdrive->ide_disk) != 512) {
                        cdrive->ide_error = IDE_UNC | IDE_ABRT;
                        goto aborted_command;
                    }
                    cdrive->ide_bufp = 0; cdrive->ide_status |= IDE_DRQ;
                    cdrive->ide_sector_count_internal = cdrive->ide_sector_count;
                    cdrive->ide_cmd = 0x20;
                    break;
                case 0xe4:
                    cdrive->ide_status = (cdrive->ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ) & (~IDE_ERR)) | IDE_DRDY;
#ifdef IDE64_DEBUG
                    log_debug("IDE64 %d READ BUFFER", idrive);
#endif
                    cdrive->ide_bufp = 0; cdrive->ide_status |= IDE_DRQ;
                    cdrive->ide_sector_count_internal = 1;
                    cdrive->ide_cmd = 0xe4;
                    break;
                case 0x30:
                case 0x31:
                    cdrive->ide_status = (cdrive->ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ) & (~IDE_ERR)) | IDE_DRDY;
#ifdef IDE64_DEBUG
                    if (cdrive->ide_head & 0x40) {
                        log_debug("IDE64 %d WRITE (%d)*%d", idrive, (cdrive->ide_cylinder_low << 8) | (cdrive->ide_cylinder_high << 16) |
                                  ((cdrive->ide_head & 0xf) << 24) | cdrive->ide_sector, cdrive->ide_sector_count);
                    } else {
                        log_debug("IDE64 %d WRITE (%d/%d/%d)*%d", idrive, cdrive->ide_cylinder_low | (cdrive->ide_cylinder_high << 8),
                                  cdrive->ide_head & 0xf, cdrive->ide_sector, cdrive->ide_sector_count);
                    }
#endif
                    if (ide_seek_sector()) {
                        cdrive->ide_error = IDE_IDNF;
                        goto aborted_command;
                    }
                    cdrive->ide_bufp = 0; cdrive->ide_status |= IDE_DRQ;
                    cdrive->ide_cmd = 0x30;
                    cdrive->ide_sector_count_internal = cdrive->ide_sector_count;
                    break;
                case 0xe8:
                    cdrive->ide_status = (cdrive->ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ) & (~IDE_ERR)) | IDE_DRDY;
#ifdef IDE64_DEBUG
                    log_debug("IDE64 %d WRITE BUFFER", idrive);
#endif
                    cdrive->ide_bufp = 0; cdrive->ide_status |= IDE_DRQ;
                    cdrive->ide_cmd = 0xe8;
                    cdrive->ide_sector_count_internal = 1;
                    break;
                case 0x91:
                    cdrive->ide_status = (cdrive->ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ) & (~IDE_ERR)) | IDE_DRDY;
                    {
                        unsigned long size;
#ifdef IDE64_DEBUG
                        log_debug("IDE64 %d SETMAX (%d/%d)", idrive, (cdrive->ide_head & 0xf) + 1, cdrive->ide_sector_count);
#endif
                        size = (cdrive->ide_identify[109] * 256 + cdrive->ide_identify[108]) * ((cdrive->ide_head & 0xf) + 1) * cdrive->ide_sector_count;
                        if (size == 0 || size > (unsigned long)((cdrive->ide_identify[123] << 24) | (cdrive->ide_identify[122] << 16) |
                                                                (cdrive->ide_identify[121] << 8) | cdrive->ide_identify[120])) {
                            cdrive->ide_error = IDE_ABRT;
                            goto aborted_command;
                        }
                        cdrive->ide_identify[110] = (cdrive->ide_head & 0xf) + 1;
                        cdrive->ide_identify[112] = cdrive->ide_sector_count;
                        cdrive->ide_identify[114] = (BYTE)(size & 0xff);
                        size >>= 8;
                        cdrive->ide_identify[115] = (BYTE)(size & 0xff);
                        size >>= 8;
                        cdrive->ide_identify[116] = (BYTE)(size & 0xff);
                        size >>= 8;
                        cdrive->ide_identify[117] = (BYTE)(size & 0xff);
                        break;
                    }
                case 0xec:
                    cdrive->ide_status = (cdrive->ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_ERR)) | IDE_DRDY | IDE_DRQ;
#ifdef IDE64_DEBUG
                    log_debug("IDE64 %d IDENTIFY", idrive);
#endif
                    cdrive->ide_bufp = 0;
                    memset(cdrive->buffer, 0, 512);
                    memcpy(cdrive->buffer, cdrive->ide_identify, 128);
                    cdrive->ide_cmd = 0xec;
                    cdrive->ide_sector_count_internal = 1;
                    break;
                default:
#ifdef IDE64_DEBUG
                    switch (out_d030 & 0xff) {
                        case 0x00:
                            log_debug("IDE64 %d NOP", idrive);
                            break;
                        case 0x08:
                            log_debug("IDE64 %d ATAPI RESET", idrive);
                            break;
                        case 0x94:
                        case 0xe0:
                            log_debug("IDE64 %d STANDBY IMMEDIATE", idrive);
                            break;
                        case 0x97:
                        case 0xe3:
                            log_debug("IDE64 %d IDLE %02x", idrive, cdrive->ide_sector_count);
                            break;
                        case 0xef:
                            log_debug("IDE64 %d SET FEATURES %02x", idrive, cdrive->ide_features);
                            break;
                        case 0xa0:
                            log_debug("IDE64 %d PACKET", idrive);
                            break;
                        case 0x95:
                        case 0xe1:
                            log_debug("IDE64 %d IDLE IMMEDIATE", idrive);
                            break;
                        case 0xa1:
                            log_debug("IDE64 %d IDENTIFY PACKET DEVICE", idrive);
                            break;
                        default:
                            log_debug("IDE64 %d COMMAND %02x", idrive, out_d030 & 0xff);
                    }
#endif
                    cdrive->ide_error = IDE_ABRT;
aborted_command:
                    cdrive->ide_status = (cdrive->ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ)) | IDE_DRDY | IDE_ERR;
                    cdrive->ide_bufp = 510;
                    cdrive->ide_cmd = 0x00;
                    break;
            }
            return;
        case 0x28:
            idrive &= ~2;
            cdrive = &drives[idrive];
            return;
        case 0x29:
            idrive |= 2;
            cdrive = &drives[idrive];
            return;
        case 0x2e:
            if ((cdrive->ide_control & 0x04) && ((value ^ 0x04) & 0x04)) {
                ide64_reset(&drives[idrive & ~1]);
                ide64_reset(&drives[idrive | 1]);
#ifdef IDE64_DEBUG
                log_debug("IDE64 RESET");
#endif
            }
            drives[idrive & ~1].ide_control = value;
            drives[idrive | 1].ide_control = value;
            return;
        case 0x30:
            if (settings_version4) {
                return;
            }
            out_d030 = (out_d030 & 0xff00) | value;
            return;
        case 0x31:
            out_d030 = (out_d030 & 0x00ff) | (value << 8);
            return;
        case 0x32:
            if (settings_version4) {
                break;
            }
            current_bank = 0;
            break;
        case 0x33:
            if (settings_version4) {
                break;
            }
            current_bank = 1;
            break;
        case 0x34:
            if (settings_version4) {
                break;
            }
            current_bank = 2;
            break;
        case 0x35:
            if (settings_version4) {
                break;
            }
            current_bank = 3;
            break;
        case 0x5f:
            if ((kill_port & 2) == 0) {
                break;
            }
            ds1302_set_lines(ds1302_context, 1u, 0u, value & 1u);
            ds1302_set_lines(ds1302_context, 1u, 1u, value & 1u);
            return;
        case 0x60:
            if (settings_version4) {
                current_bank = 0;
            }
            break;
        case 0x61:
            if (settings_version4) {
                current_bank = 1;
            }
            break;
        case 0x62:
            if (settings_version4) {
                current_bank = 2;
            }
            break;
        case 0x63:
            if (settings_version4) {
                current_bank = 3;
            }
            break;
        case 0x64:
            if (settings_version4) {
                current_bank = 4;
            }
            break;
        case 0x65:
            if (settings_version4) {
                current_bank = 5;
            }
            break;
        case 0x66:
            if (settings_version4) {
                current_bank = 6;
            }
            break;
        case 0x67:
            if (settings_version4) {
                current_bank = 7;
            }
            break;
        case 0xfb:
            if (((kill_port & 0x02) == 0) && (value & 0x02)) {
                ds1302_set_lines(ds1302_context, 0u, 1u, 1u);
            }
            kill_port = value;
            if ((kill_port & 1) == 0) {
                return;
            }
            current_cfg = 2;
            break;
        case 0xfc:
            current_cfg = 1;
            break;
        case 0xfd:
            current_cfg = 0;
            break;
        case 0xfe:
            current_cfg = 3;
            break;
        case 0xff:
            current_cfg = 2;
            break;
        default:
            return;
    }
    cartridge_config_changed(0, (BYTE)(current_cfg | (current_bank << CMODE_BANK_SHIFT)), CMODE_READ | CMODE_PHI2_RAM);
}

BYTE REGPARM1 ide64_roml_read(WORD addr)
{
    return roml_banks[(addr & 0x3fff) | (roml_bank << 14)];
}

BYTE REGPARM1 ide64_romh_read(WORD addr)
{
    return romh_banks[(addr & 0x3fff) | (romh_bank << 14)];
}

BYTE REGPARM1 ide64_1000_7fff_read(WORD addr)
{
    return export_ram0[addr & 0x7fff];
}

void REGPARM2 ide64_1000_7fff_store(WORD addr, BYTE value)
{
    export_ram0[addr & 0x7fff] = value;
}

BYTE REGPARM1 ide64_a000_bfff_read(WORD addr)
{
    return romh_banks[(addr & 0x3fff) | (romh_bank << 14)];
}

BYTE REGPARM1 ide64_c000_cfff_read(WORD addr)
{
    return export_ram0[addr & 0x7fff];
}

void REGPARM2 ide64_c000_cfff_store(WORD addr, BYTE value)
{
    export_ram0[addr & 0x7fff] = value;
}

void ide64_config_init(void)
{
    cartridge_config_changed(0, 0, CMODE_READ | CMODE_PHI2_RAM);
    current_bank = 0;
    current_cfg = 0;
    kill_port = 0;
    if (ds1302_context != NULL) {
        ds1302_set_lines(ds1302_context, 0u, 1u, 1u);
    }
}

void ide64_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x20000);
    memcpy(romh_banks, rawcart, 0x20000);
    cartridge_config_changed(0, 0, CMODE_READ | CMODE_PHI2_RAM);
}

void ide64_detach(void)
{
    int i;

    c64export_remove(&export_res);

    if (ds1302_context != NULL) {
        ds1302_destroy(ds1302_context);
        ds1302_context = NULL;
    }

    for (i = 0; i < 4; i++) {
        if (drives[i].ide_disk != NULL) {
            fclose(drives[i].ide_disk);
            drives[i].ide_disk = NULL;
        }
    }

    ide64_enabled = 0;

    c64io_unregister(ide64_list_item);
    ide64_list_item = NULL;
#ifdef IDE64_DEBUG
    log_debug("IDE64 detached");
#endif
}

static int ide64_common_attach(void)
{
    int i;

    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    ide64_enabled = 1;

    if (ds1302_context != NULL) {
        ds1302_destroy(ds1302_context);
    }
    ds1302_context = ds1302_init((BYTE *)ide64_DS1302, &rtc_offset);
    ds1302_set_lines(ds1302_context, 0u, 1u, 1u);

    for (i = 0; i < 4; i++) {
        if (idrive < 0) {
            drives[i].ide_disk = NULL;
        }
        ide64_disk_attach(&drives[i]);
    }
    idrive = 0;
    cdrive = &drives[idrive];

#ifdef IDE64_DEBUG
    log_debug("IDE64 attached");
#endif

    ide64_list_item = c64io_register(&ide64_device);

    return 0;
}

int ide64_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x20000, UTIL_FILE_LOAD_SKIP_ADDRESS | UTIL_FILE_LOAD_FILL) < 0) {
        return -1;
    }

    return ide64_common_attach();
}

int ide64_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int i;

    for (i = 0; i <= 7; i++) {
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            if (i == 4) {
                break;
            }
            return -1;
        }

        if (chipheader[0xc] != 0x80 || chipheader[0xe] != 0x40) {
            return -1;
        }

        if (chipheader[0xb] > 7) {
            return -1;
        }

        if (fread(&rawcart[chipheader[0xb] << 14], 0x4000, 1, fd) < 1) {
            return -1;
        }
    }

    return ide64_common_attach();
}
