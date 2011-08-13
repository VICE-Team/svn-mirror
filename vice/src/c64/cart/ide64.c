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
#define CARTRIDGE_INCLUDE_SLOTMAIN_API
#include "c64cartsystem.h"
#undef CARTRIDGE_INCLUDE_SLOTMAIN_API
#include "c64export.h"
#include "cartio.h"
#include "cartridge.h"
#include "cmdline.h"
#include "ds1202_1302.h"
#include "ide64.h"
#include "log.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "snapshot.h"
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
static rtc_ds1202_1302_t *ds1302_context = NULL;

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
    FILE *ide_disk;
    unsigned int settings_cylinders, settings_heads, settings_sectors, settings_size;
    unsigned int actual_cylinders, actual_heads, actual_sectors;
    int settings_autodetect_size;
};

static struct drive_t drives[4], *cdrive = NULL;
static int idrive = -1;

/* communication latch */
static WORD out_d030, in_d030;

/* config ram */
static char ide64_DS1302[65];

static char *ide64_configuration_string = NULL;

static int settings_version4, rtc_offset_res;
static time_t rtc_offset;

static BYTE hdd_identify[128] = {
    0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x30, 0x32, 0x31, 0x31,
    0x38, 0x30, 0x36, 0x30, 0x20, 0x20, 0x20, 0x20,

    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x34,
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
static void ide64_io1_store(WORD addr, BYTE value);
static BYTE ide64_io1_read(WORD addr);
static BYTE ide64_io1_peek(WORD addr);

static io_source_t ide64_device = {
    CARTRIDGE_NAME_IDE64,
    IO_DETACH_CART,
    NULL,
    0xde20, 0xdeff, 0xff,
    0,
    ide64_io1_store,
    ide64_io1_read,
    ide64_io1_peek,
    NULL, /* TODO: dump */
    CARTRIDGE_IDE64,
    0,
    0
};

static io_source_list_t *ide64_list_item = NULL;

static const c64export_resource_t export_res = {
    CARTRIDGE_NAME_IDE64, 1, 1, &ide64_device, NULL, CARTRIDGE_IDE64
};

/* ---------------------------------------------------------------------*/
/* drive reset response */
static void ide64_drive_reset(struct drive_t *cdrive)
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
    cdrive->actual_sectors = cdrive->settings_sectors;
    cdrive->actual_heads = cdrive->settings_heads;
    cdrive->actual_cylinders = cdrive->settings_cylinders;
}

static int ide64_disk_attach(struct drive_t *cdrive)
{
    int res;

    if (!ide64_enabled) {
        return 0;
    }


    if (cdrive->ide_disk != NULL) {
        fclose(cdrive->ide_disk);
    }

    cdrive->ide_disk = NULL;

    if (!cdrive->ide64_image_file[0]) {
        ide64_drive_reset(cdrive);
        return 0;
    }

    machine_trigger_reset(MACHINE_RESET_MODE_HARD);

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

    if (cdrive->settings_autodetect_size && cdrive->ide_disk) {
        /* try to get drive geometry */
        unsigned char idebuf[24];
        unsigned long size = 0;

        /* read header */
        res = (int)fread(idebuf, 1, 24, cdrive->ide_disk);
        if (res < 24) {
            memset(&res, 0, sizeof(res));
        }
        /* check signature */

        for (;;) {

            if (memcmp(idebuf,"C64-IDE V", 9) == 0) { /* old filesystem always CHS */
                cdrive->settings_cylinders = ((idebuf[0x10] << 8) | idebuf[0x11]) + 1;
                cdrive->settings_heads = (idebuf[0x12] & 0x0f) + 1;
                cdrive->settings_sectors = idebuf[0x13];
                cdrive->settings_size = cdrive->settings_cylinders * cdrive->settings_heads * cdrive->settings_sectors;
                break;  /* OK */
            }

            if (memcmp(idebuf + 8, "C64 CFS V", 9) == 0) {
                if (idebuf[0x04] & 0x40) { /* LBA */
                    cdrive->settings_cylinders = 0;
                    cdrive->settings_heads = 0;
                    cdrive->settings_sectors = 0;
                    cdrive->settings_size = ((idebuf[0x04] & 0x0f) << 24) | (idebuf[0x05] << 16) | (idebuf[0x06] << 8) | idebuf[0x07];
                } else { /* CHS */
                    cdrive->settings_cylinders = ((idebuf[0x05] << 8) | idebuf[0x06]) + 1;
                    cdrive->settings_heads = (idebuf[0x04] & 0x0f) + 1;
                    cdrive->settings_sectors = idebuf[0x07];
                    cdrive->settings_size = cdrive->settings_cylinders * cdrive->settings_heads * cdrive->settings_sectors;
                }
                break;  /* OK */
            }

            log_message(LOG_DEFAULT, "IDE64: Disk image signature not found, guessing size.");

            size = 0;
            if (fseek(cdrive->ide_disk, 0, SEEK_END) == 0) {
                size = ftell(cdrive->ide_disk);
                if (size < 0) size = 0;
            }
            cdrive->settings_cylinders = 0;
            cdrive->settings_heads = 0;
            cdrive->settings_sectors = 0;
            cdrive->settings_size = size >> 9;
            break;
        }
    }

    if (cdrive->settings_size < 1) {
        cdrive->settings_cylinders = hdd_identify[2] | (hdd_identify[3] << 8);
        cdrive->settings_heads = hdd_identify[6];
        cdrive->settings_sectors = hdd_identify[12];
        if (hdd_identify[120] || hdd_identify[121] || hdd_identify[122] || hdd_identify[123]) {
            cdrive->settings_size = hdd_identify[120];
            cdrive->settings_size |= hdd_identify[121] << 8;
            cdrive->settings_size |= hdd_identify[122] << 16;
            cdrive->settings_size |= hdd_identify[123] << 24;
        } else {
            cdrive->settings_size = cdrive->settings_cylinders * cdrive->settings_heads * cdrive->settings_sectors;
        }
        log_message(LOG_DEFAULT, "IDE64: Image size invalid, using default %d MiB.", cdrive->settings_size >> 11);
    }

    if (cdrive->settings_sectors < 1 || cdrive->settings_sectors > 63 || cdrive->settings_cylinders > 65535 || (cdrive->settings_sectors * cdrive->settings_heads * cdrive->settings_cylinders) > 16514064) {
        unsigned int size = cdrive->settings_size;
        int i, c, h, s;

        if (size > 16514064) size = 16514064;
        h = 1; s = 1; i = 63; c = size;
        while (i > 1 && c > 1) {
            if ((c % i) == 0) {
                if (s * i <= 63) {
                    s *= i; c /= i;
                    continue;
                }
                if (h * i <= 16) {
                    h *= i; c /= i;
                    continue;
                }
            }
            i--;
        }
        for (;;) {
            if (size <= 1032192) {
                if (c <= 1024) break;
            } else {
                if (h < 5 && c < 65536) break;
                if (h < 9 && c < 32768) break;
                if (c < 16384) break;
            }
            if (s == 63 && h < 16) h++;
            if (s < 63) s++;
            c = size / (h * s);
        }
        cdrive->settings_cylinders = c;
        cdrive->settings_heads = h;
        cdrive->settings_sectors = s;
    }

    log_message(LOG_DEFAULT, "IDE64: %i/%i/%i CHS geometry, %u sectors total.", cdrive->settings_cylinders, cdrive->settings_heads, cdrive->settings_sectors, cdrive->settings_size);

    ide64_drive_reset(cdrive); /* update actual geometry */

    return 0;
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
    /* FIXME: perhaps a reset should be triggered when config changes ? */
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
    int i = vice_ptr_to_int(param);

    if (cylinders > 65535 || cylinders < 1) {
        return -1;
    }

    drives[i].settings_cylinders = cylinders;
    drives[i].settings_size = drives[i].settings_cylinders * drives[i].settings_heads * drives[i].settings_sectors;

    return 0;
}

static int set_heads(int val, void *param)
{
    unsigned int heads = (unsigned int)val;
    int i = vice_ptr_to_int(param);

    if (heads > 16 || heads < 1) {
        return -1;
    }

    drives[i].settings_heads = heads;
    drives[i].settings_size = drives[i].settings_cylinders * drives[i].settings_heads * drives[i].settings_sectors;

    return 0;
}

static int set_sectors(int val, void *param)
{
    unsigned int sectors = (unsigned int)val;
    int i = vice_ptr_to_int(param);

    if (sectors > 63 || sectors < 1) {
        return -1;
    }

    drives[i].settings_sectors = sectors;
    drives[i].settings_size = drives[i].settings_cylinders * drives[i].settings_heads * drives[i].settings_sectors;

    return 0;
}

static int set_autodetect_size(int val, void *param)
{
    int i = vice_ptr_to_int(param);

    drives[i].settings_autodetect_size = val;

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
    rtc_offset_res = val;
    rtc_offset = (time_t)val;

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
    { "IDE64Cylinders1", 256,
      RES_EVENT_NO, NULL,
      (int *)&drives[0].settings_cylinders, set_cylinders, (void *)0 },
    { "IDE64Cylinders2", 256,
      RES_EVENT_NO, NULL,
      (int *)&drives[1].settings_cylinders, set_cylinders, (void *)1 },
    { "IDE64Cylinders3", 256,
      RES_EVENT_NO, NULL,
      (int *)&drives[2].settings_cylinders, set_cylinders, (void *)2 },
    { "IDE64Cylinders4", 256,
      RES_EVENT_NO, NULL,
      (int *)&drives[3].settings_cylinders, set_cylinders, (void *)3 },
    { "IDE64Heads1", 4,
      RES_EVENT_NO, NULL,
      (int *)&drives[0].settings_heads, set_heads, (void *)0 },
    { "IDE64Heads2", 4,
      RES_EVENT_NO, NULL,
      (int *)&drives[1].settings_heads, set_heads, (void *)1 },
    { "IDE64Heads3", 4,
      RES_EVENT_NO, NULL,
      (int *)&drives[2].settings_heads, set_heads, (void *)2 },
    { "IDE64Heads4", 4,
      RES_EVENT_NO, NULL,
      (int *)&drives[3].settings_heads, set_heads, (void *)3 },
    { "IDE64Sectors1", 16,
      RES_EVENT_NO, NULL,
      (int *)&drives[0].settings_sectors, set_sectors, (void *)0 },
    { "IDE64Sectors2", 16,
      RES_EVENT_NO, NULL,
      (int *)&drives[1].settings_sectors, set_sectors, (void *)1 },
    { "IDE64Sectors3", 16,
      RES_EVENT_NO, NULL,
      (int *)&drives[2].settings_sectors, set_sectors, (void *)2 },
    { "IDE64Sectors4", 16,
      RES_EVENT_NO, NULL,
      (int *)&drives[3].settings_sectors, set_sectors, (void *)3 },
    { "IDE64AutodetectSize1", 1,
      RES_EVENT_NO, NULL,
      (int *)&drives[0].settings_autodetect_size, set_autodetect_size, (void *)0 },
    { "IDE64AutodetectSize2", 1,
      RES_EVENT_NO, NULL,
      (int *)&drives[1].settings_autodetect_size, set_autodetect_size, (void *)1 },
    { "IDE64AutodetectSize3", 1,
      RES_EVENT_NO, NULL,
      (int *)&drives[2].settings_autodetect_size, set_autodetect_size, (void *)2 },
    { "IDE64AutodetectSize4", 1,
      RES_EVENT_NO, NULL,
      (int *)&drives[3].settings_autodetect_size, set_autodetect_size, (void *)3 },
    { "IDE64version4", 0,
      RES_EVENT_NO, NULL,
      &settings_version4, set_version4, NULL },
    { "IDE64RTCOffset", 0,
      RES_EVENT_NO, NULL,
      (int *)&rtc_offset_res, set_rtc_offset, NULL },
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
    { "-IDE64cyl1", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Cylinders1", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_CYLINDERS_IDE64,
      NULL, NULL },
    { "-IDE64cyl2", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Cylinders2", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_CYLINDERS_IDE64,
      NULL, NULL },
    { "-IDE64cyl3", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Cylinders3", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_CYLINDERS_IDE64,
      NULL, NULL },
    { "-IDE64cyl4", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Cylinders4", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_CYLINDERS_IDE64,
      NULL, NULL },
    { "-IDE64hds1", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Heads1", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_HEADS_IDE64,
      NULL, NULL },
    { "-IDE64hds2", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Heads2", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_HEADS_IDE64,
      NULL, NULL },
    { "-IDE64hds3", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Heads3", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_HEADS_IDE64,
      NULL, NULL },
    { "-IDE64hds4", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Heads4", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_HEADS_IDE64,
      NULL, NULL },
    { "-IDE64sec1", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Sectors1", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_SECTORS_IDE64,
      NULL, NULL },
    { "-IDE64sec2", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Sectors2", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_SECTORS_IDE64,
      NULL, NULL },
    { "-IDE64sec3", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Sectors3", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_SECTORS_IDE64,
      NULL, NULL },
    { "-IDE64sec4", SET_RESOURCE, 1,
      NULL, NULL, "IDE64Sectors4", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_SECTORS_IDE64,
      NULL, NULL },
    { "-IDE64autosize1", SET_RESOURCE, 0,
      NULL, NULL, "IDE64AutodetectSize1", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_AUTODETECT_IDE64_GEOMETRY,
      NULL, NULL },
    { "+IDE64autosize1", SET_RESOURCE, 0,
      NULL, NULL, "IDE64AutodetectSize1", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_NO_AUTODETECT_IDE64_GEOMETRY,
      NULL, NULL },
    { "-IDE64autosize2", SET_RESOURCE, 0,
      NULL, NULL, "IDE64AutodetectSize2", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_AUTODETECT_IDE64_GEOMETRY,
      NULL, NULL },
    { "+IDE64autosize2", SET_RESOURCE, 0,
      NULL, NULL, "IDE64AutodetectSize2", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_NO_AUTODETECT_IDE64_GEOMETRY,
      NULL, NULL },
    { "-IDE64autosize3", SET_RESOURCE, 0,
      NULL, NULL, "IDE64AutodetectSize3", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_AUTODETECT_IDE64_GEOMETRY,
      NULL, NULL },
    { "+IDE64autosize3", SET_RESOURCE, 0,
      NULL, NULL, "IDE64AutodetectSize3", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_NO_AUTODETECT_IDE64_GEOMETRY,
      NULL, NULL },
    { "-IDE64autosize4", SET_RESOURCE, 0,
      NULL, NULL, "IDE64AutodetectSize4", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_AUTODETECT_IDE64_GEOMETRY,
      NULL, NULL },
    { "+IDE64autosize4", SET_RESOURCE, 0,
      NULL, NULL, "IDE64AutodetectSize4", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_NO_AUTODETECT_IDE64_GEOMETRY,
      NULL, NULL },
    { "-IDE64version4", SET_RESOURCE, 0,
      NULL, NULL, "IDE64version4", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_IDE64_V4,
      NULL, NULL },
    { "+IDE64version4", SET_RESOURCE, 0,
      NULL, NULL, "IDE64version4", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_IDE64_PRE_V4,
      NULL, NULL },
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
    } else {
        if (cdrive->ide_sector == 0 || cdrive->ide_sector > cdrive->actual_sectors || (cdrive->ide_head & 0xf) >= cdrive->actual_heads ||
            (cdrive->ide_cylinder_low | (cdrive->ide_cylinder_high << 8)) >= cdrive->actual_cylinders) {
            return 1;
        }
        lba = ((cdrive->ide_cylinder_low | (cdrive->ide_cylinder_high << 8)) * cdrive->actual_heads + (cdrive->ide_head & 0xf)) * cdrive->actual_sectors + cdrive->ide_sector - 1;
    }
    if (lba >= cdrive->settings_size) {
        return 1;
    }
    return fseek(cdrive->ide_disk, lba << 9, SEEK_SET);
}

static BYTE ide64_io1_read(WORD addr)
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
                            clearerr(cdrive->ide_disk);
                            fread(cdrive->buffer, 1, 512, cdrive->ide_disk);
                            if (ferror(cdrive->ide_disk)) {
                                cdrive->ide_error = IDE_UNC | IDE_ABRT;
                                cdrive->ide_status = (cdrive->ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ)) | IDE_DRDY | IDE_ERR;
                                cdrive->ide_bufp = 510;
                                cdrive->ide_cmd = 0x00;
                                break;
                            }
                            cdrive->ide_bufp = 0;
                            cdrive->ide_status = cdrive->ide_status | IDE_DRQ;
                        }
                    }
                    break;
                default:
                    in_d030 = 0;
            }
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x21:
            in_d030 = cdrive->ide_disk ? cdrive->ide_error : 0;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x22:
            in_d030 = cdrive->ide_disk ? cdrive->ide_sector_count : 0;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x23:
            in_d030 = cdrive->ide_disk ? cdrive->ide_sector : 0;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x24:
            in_d030 = cdrive->ide_disk ? cdrive->ide_cylinder_low : 0;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x25:
            in_d030 = cdrive->ide_disk ? cdrive->ide_cylinder_high : 0;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x26:
            in_d030 = cdrive->ide_disk ? cdrive->ide_head : 0;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x27:
        case 0x2e:
            in_d030 = cdrive->ide_disk ? cdrive->ide_status : 0;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x2f:
            if (!drives[idrive].ide_disk || !drives[idrive ^ 1].ide_disk) {
                in_d030 = (cdrive->ide_disk ? ((idrive & 1) ? 0xc1 : 0xc2) : 0xff) | (((cdrive->ide_head ^ 15) & 15) << 2);
            } else {
                in_d030 = 0;
            }
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
            in_d030 = 0;
            if (settings_version4) {
                return in_d030 & 0xff;
            }
            break;
        case 0x30:
            if (settings_version4) {
                break;
            }
            return (BYTE)in_d030;
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
            ds1202_1302_set_lines(ds1302_context, 1u, 0u, 1u);
            i |= ds1202_1302_read_data_line(ds1302_context);
            ds1202_1302_set_lines(ds1302_context, 1u, 1u, 1u);
            return i;
    }
    ide64_device.io_source_valid = 0;
    return vicii_read_phi1();
}

static BYTE ide64_io1_peek(WORD addr)
{
    BYTE value = 0;

    if (kill_port & 1) {
        if ((addr & 0xff) >= 0x5f) {
            return 0;
        }
    }

    if ((addr & 0xff) >= 0x60) {
        return roml_banks[(addr & 0xff) | 0x1e00 | (current_bank << 14)];
    }

    switch (addr & 0xff) {
        case 0x20:
            if (settings_version4) {
                if (!cdrive->ide_disk) {
                    return 0;
                }
                return 0;
            }
            break;
        case 0x21:
            if (settings_version4) {
                if (!cdrive->ide_disk) {
                    return 0;
                }
                return cdrive->ide_error & 0xff;
            }
            break;
        case 0x22:
            if (settings_version4) {
                if (!cdrive->ide_disk) {
                    return 0;
                }
                return cdrive->ide_sector_count & 0xff;
            }
            break;
        case 0x23:
            if (settings_version4) {
                if (!cdrive->ide_disk) {
                    return 0;
                }
                return cdrive->ide_sector & 0xff;
            }
            break;
        case 0x24:
            if (settings_version4) {
                if (!cdrive->ide_disk) {
                    return 0;
                }
                return cdrive->ide_cylinder_low & 0xff;
            }
            break;
        case 0x25:
            if (settings_version4) {
                if (!cdrive->ide_disk) {
                    return 0;
                }
                return cdrive->ide_cylinder_high & 0xff;
            }
            break;
        case 0x26:
            if (settings_version4) {
                if (!cdrive->ide_disk) {
                    return 0;
                }
                return cdrive->ide_head & 0xff;
            }
            break;
        case 0x27:
        case 0x2e:
            if (settings_version4) {
                if (!cdrive->ide_disk) {
                    return 0;
                }
                return cdrive->ide_status & 0xff;
            }
            break;
        case 0x2f:
            if (settings_version4) {
                if (!drives[idrive].ide_disk || !drives[idrive ^ 1].ide_disk) {
                    return (cdrive->ide_disk ? ((idrive & 1)? 0xc1 : 0xc2 ) : 0xff) | (((cdrive->ide_head ^ 15) & 15) << 2);
                } else {
                    return 0;
                }
            }
            break;
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
            if (settings_version4) {
                if (!cdrive->ide_disk) {
                    return 0;
                }
            }
            break;
        case 0x30:
            if (settings_version4) {
                break;
            }
            return (BYTE)in_d030;
        case 0x31:
            return in_d030 >> 8;
        case 0x32:
            return (settings_version4 ? 0x20 : 0x10) | (current_bank << 2) | (((current_cfg & 1) ^ 1) << 1) | (current_cfg >> 1);
        case 0x5f:
            return 0;
    }
    return value;
}

BYTE ide64_get_killport(void)
{
    return kill_port;
}

static void ide64_io1_store(WORD addr, BYTE value)
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
            drives[idrive].ide_features = drives[idrive ^ 1].ide_features = out_d030 & 0xff;
            return;
        case 0x22:
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            drives[idrive].ide_sector_count = drives[idrive ^ 1].ide_sector_count = out_d030 & 0xff;
            return;
        case 0x23:
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            drives[idrive].ide_sector = drives[idrive ^ 1].ide_sector = out_d030 & 0xff;
            return;
        case 0x24:
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            drives[idrive].ide_cylinder_low = drives[idrive ^ 1].ide_cylinder_low = out_d030 & 0xff;
            return;
        case 0x25:
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            drives[idrive].ide_cylinder_high = drives[idrive ^ 1].ide_cylinder_high = out_d030 & 0xff;
            return;
        case 0x26:
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            drives[idrive].ide_head = drives[idrive ^ 1].ide_head = out_d030 & 0xff;
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
                    clearerr(cdrive->ide_disk);
                    fread(cdrive->buffer, 1, 512, cdrive->ide_disk);
                    if (ferror(cdrive->ide_disk)) {
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
                    cdrive->actual_heads = (cdrive->ide_head & 0xf) + 1;
                    cdrive->actual_sectors = cdrive->ide_sector_count;
                    if (cdrive->actual_sectors < 1 || cdrive->actual_sectors > 63) {
                        cdrive->actual_cylinders = 0;
                    } else {
                        unsigned int size = cdrive->settings_size;
                        if (size > 16514064) size = 16514064;
                        size /= cdrive->actual_heads * cdrive->actual_sectors;
                        cdrive->actual_cylinders = (size > 65535) ? 65535 : size;
                    }
#ifdef IDE64_DEBUG
                    log_debug("IDE64 %d INIT DEVICE PARAM (%d/%d/%d)", idrive, cdrive->actual_cylinders, cdrive->actual_heads, cdrive->actual_sectors);
#endif
                    if (cdrive->actual_cylinders == 0) {
                        cdrive->actual_heads = 0;
                        cdrive->actual_sectors = 0;
                        cdrive->ide_error = IDE_ABRT;
                        goto aborted_command;
                    }
                    break;
                case 0xec:
                    cdrive->ide_status = (cdrive->ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_ERR)) | IDE_DRDY | IDE_DRQ;
                    {
                        unsigned int size;
#ifdef IDE64_DEBUG
                        log_debug("IDE64 %d IDENTIFY", idrive);
#endif
                        cdrive->ide_bufp = 0;
                        memset(cdrive->buffer, 0, 512);
                        memcpy(cdrive->buffer, hdd_identify, 128);
                        cdrive->buffer[2] = cdrive->settings_cylinders & 255;
                        cdrive->buffer[3] = cdrive->settings_cylinders >> 8;
                        cdrive->buffer[6] = cdrive->settings_heads;
                        cdrive->buffer[12] = cdrive->settings_sectors;
                        cdrive->buffer[106] = cdrive->actual_sectors ? 1 : 0;
                        cdrive->buffer[108] = cdrive->actual_cylinders & 255;
                        cdrive->buffer[109] = cdrive->actual_cylinders >> 8;
                        cdrive->buffer[110] = cdrive->actual_heads;
                        cdrive->buffer[112] = cdrive->actual_sectors;
                        size = cdrive->actual_cylinders * cdrive->actual_heads * cdrive->actual_sectors;
                        if (size > cdrive->settings_size) size = cdrive->settings_size;
                        cdrive->buffer[114] = size & 0xff;
                        cdrive->buffer[115] = (size >> 8) & 0xff;
                        cdrive->buffer[116] = (size >> 16) & 0xff;
                        cdrive->buffer[117] = (size >> 24) & 0xff;
                        cdrive->buffer[120] = cdrive->settings_size & 0xff;
                        cdrive->buffer[121] = (cdrive->settings_size >> 8) & 0xff;
                        cdrive->buffer[122] = (cdrive->settings_size >> 16) & 0xff;
                        cdrive->buffer[123] = (cdrive->settings_size >> 24) & 0xff;
                        cdrive->ide_cmd = 0xec;
                        cdrive->ide_sector_count_internal = 1;
                        break;
                    }
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
            idrive = ((drives[idrive & ~2].ide_head >> 4) & 1) & ~2;
            cdrive = &drives[idrive];
            return;
        case 0x29:
            idrive = ((drives[idrive | 2].ide_head >> 4) & 1) | 2;
            cdrive = &drives[idrive];
            return;
        case 0x2e:
            if ((cdrive->ide_control & 0x04) && ((value ^ 0x04) & 0x04)) {
                ide64_drive_reset(&drives[idrive]);
                ide64_drive_reset(&drives[idrive ^ 1]);
                idrive &= ~1;
                cdrive = &drives[idrive];
#ifdef IDE64_DEBUG
                log_debug("IDE64 RESET");
#endif
            }
            drives[idrive].ide_control = drives[idrive ^ 1].ide_control = value;
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
            ds1202_1302_set_lines(ds1302_context, 1u, 0u, value & 1u);
            ds1202_1302_set_lines(ds1302_context, 1u, 1u, value & 1u);
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
                ds1202_1302_set_lines(ds1302_context, 0u, 1u, 1u);
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
    cart_config_changed_slotmain(0, (BYTE)(current_cfg | (current_bank << CMODE_BANK_SHIFT)), CMODE_READ | CMODE_PHI2_RAM);
}

BYTE ide64_roml_read(WORD addr)
{
    return roml_banks[(addr & 0x3fff) | (roml_bank << 14)];
}

BYTE ide64_romh_read(WORD addr)
{
    return romh_banks[(addr & 0x3fff) | (romh_bank << 14)];
}

BYTE ide64_1000_7fff_read(WORD addr)
{
    return export_ram0[addr & 0x7fff];
}

void ide64_1000_7fff_store(WORD addr, BYTE value)
{
    export_ram0[addr & 0x7fff] = value;
}

BYTE ide64_a000_bfff_read(WORD addr)
{
    return romh_banks[(addr & 0x3fff) | (romh_bank << 14)];
}

BYTE ide64_c000_cfff_read(WORD addr)
{
    return export_ram0[addr & 0x7fff];
}

void ide64_c000_cfff_store(WORD addr, BYTE value)
{
    export_ram0[addr & 0x7fff] = value;
}

void ide64_config_init(void)
{
    cart_config_changed_slotmain(0, 0, CMODE_READ | CMODE_PHI2_RAM);
    current_bank = 0;
    current_cfg = 0;
    kill_port = 0;
    if (ds1302_context != NULL) {
        ds1202_1302_set_lines(ds1302_context, 0u, 1u, 1u);
    }
}

void ide64_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x20000);
    memcpy(romh_banks, rawcart, 0x20000);
    cart_config_changed_slotmain(0, 0, CMODE_READ | CMODE_PHI2_RAM);
}

void ide64_detach(void)
{
    int i;

    c64export_remove(&export_res);

    if (ds1302_context != NULL) {
        ds1202_1302_destroy(ds1302_context);
        ds1302_context = NULL;
    }

    for (i = 0; i < 4; i++) {
        if (drives[i].ide_disk != NULL) {
            fclose(drives[i].ide_disk);
            drives[i].ide_disk = NULL;
        }
    }

    ide64_enabled = 0;

    io_source_unregister(ide64_list_item);
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
        ds1202_1302_destroy(ds1302_context);
    }
    ds1302_context = ds1202_1302_init((BYTE *)ide64_DS1302, &rtc_offset, 1302);
    ds1202_1302_set_lines(ds1302_context, 0u, 1u, 1u);

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

    ide64_list_item = io_source_register(&ide64_device);

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

    for (i = 0x1e60; i < 0x1efd; i++) {
        if (rawcart[i] == 0x8d && ((rawcart[i + 1] - 2) & 0xfc) == 0x30 && rawcart[i + 2] == 0xde) {
            settings_version4 = 0; /* V3 emulation required */
            break;
        }
        if (rawcart[i] == 0x8d && (rawcart[i + 1] & 0xf8) == 0x60 && rawcart[i + 2] == 0xde) {
            settings_version4 = 1; /* V4 emulation required */
            break;
        }
    }

    return ide64_common_attach();
}

/* ---------------------------------------------------------------------*/
/*    snapshot support functions                                             */


/* Snapshot is dangerous without including all the disk images
   and can lead to filesystem corruption as the IDEDOS buffers
   could get out of sync from the images if used incorrectly.
   Images are huge and IDE64 is not that useful without them,
   so the snapshot functions are not implemented now.
 */
int ide64_snapshot_write_module(snapshot_t *s)
{
    return -1;
}

int ide64_snapshot_read_module(snapshot_t *s)
{
    return -1;
}
