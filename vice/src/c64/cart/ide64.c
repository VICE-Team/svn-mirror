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
#include "ata.h"
#include "monitor.h"

#ifdef IDE64_DEBUG
#define debug(...) log_debug(__VA_ARGS__)
#else
#define debug(...) {}
#endif

/* Current IDE64 bank */
static int current_bank;

/* Current memory config */
static int current_cfg;

/* ds1302 context */
static rtc_ds1202_1302_t *ds1302_context = NULL;

/*  */
static BYTE kill_port;

static struct ata_drive_t drives[4];
static int idrive = 0;

/* communication latch */
static WORD out_d030, in_d030;

/* config ram */
static char ide64_DS1302[65];

static char *ide64_configuration_string = NULL;

static int settings_version4, rtc_offset_res;
static time_t rtc_offset;

/* ---------------------------------------------------------------------*/

/* some prototypes are needed */
static void ide64_io1_store(WORD addr, BYTE value);
static BYTE ide64_io1_read(WORD addr);
static BYTE ide64_io1_peek(WORD addr);
static int ide64_io1_dump(void);

static io_source_t ide64_device = {
    CARTRIDGE_NAME_IDE64,
    IO_DETACH_CART,
    NULL,
    0xde20, 0xdeff, 0xff,
    0,
    ide64_io1_store,
    ide64_io1_read,
    ide64_io1_peek,
    ide64_io1_dump,
    CARTRIDGE_IDE64,
    0,
    0
};

static io_source_list_t *ide64_list_item = NULL;

static const c64export_resource_t export_res = {
    CARTRIDGE_NAME_IDE64, 1, 1, &ide64_device, NULL, CARTRIDGE_IDE64
};

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
    return 0;
}

static int detect_ide64_image(struct ata_drive_t *drv)
{
    FILE *file;
    unsigned char header[24];
    int res;

    drv->auto_cylinders = 0;
    drv->auto_heads = 0;
    drv->auto_sectors = 0;
    drv->auto_size = 0;

    res = strlen(drv->filename); 

    if (!res) {
        drv->settings_type = ATA_DRIVE_NONE;
        return -1;
    }

    drv->settings_type = ATA_DRIVE_CF;
    if (res > 4) {
        if (!strcasecmp(drv->filename + res - 4, ".cfa")) {
            drv->settings_type = ATA_DRIVE_CF;
        } else if (!strcasecmp(drv->filename + res - 4, ".hdd")) {
            drv->settings_type = ATA_DRIVE_HDD;
        } else if (!strcasecmp(drv->filename + res - 4, ".fdd")) {
            drv->settings_type = ATA_DRIVE_FDD;
        } else if (!strcasecmp(drv->filename + res - 4, ".iso")) {
            drv->settings_type = ATA_DRIVE_CD;
        }
    }

    if (!drv->settings_autodetect_size) {
        return 0;
    }

    file = fopen(drv->filename, MODE_READ);

    if (!file) {
        return -1;
    }

    if (fread(header, 1, 24, file) < 24) {
        memset(&header, 0, sizeof(header));
    }

    if (memcmp(header, "C64-IDE V", 9) == 0) { /* old filesystem always CHS */
        drv->auto_cylinders = ((header[0x10] << 8) | header[0x11]) + 1;
        drv->auto_heads = (header[0x12] & 0x0f) + 1;
        drv->auto_sectors = header[0x13];
        drv->auto_size = drv->settings_cylinders * drv->settings_heads * drv->settings_sectors;
    } else if (memcmp(header + 8, "C64 CFS V", 9) == 0) {
        if (header[0x04] & 0x40) { /* LBA */
            drv->auto_size = ((header[0x04] & 0x0f) << 24) | (header[0x05] << 16) | (header[0x06] << 8) | header[0x07];
        } else { /* CHS */
            drv->auto_cylinders = ((header[0x05] << 8) | header[0x06]) + 1;
            drv->auto_heads = (header[0x04] & 0x0f) + 1;
            drv->auto_sectors = header[0x07];
            drv->auto_size = drv->settings_cylinders * drv->settings_heads * drv->settings_sectors;
        }
    }
    fclose(file);

    return 0;
}

static int set_ide64_image_file(const char *name, void *param)
{
    struct ata_drive_t *drv = &drives[vice_ptr_to_int(param)];

    util_string_set(&drv->filename, name);
    detect_ide64_image(drv);
    ata_image_change(drv);

    return 0;
}

static int set_cylinders(int cylinders, void *param)
{
    struct ata_drive_t *drv = &drives[vice_ptr_to_int(param)];

    if (cylinders > 65535 || cylinders < 1) {
        return -1;
    }

    drv->settings_cylinders = cylinders;
    ata_image_change(drv);

    return 0;
}

static int set_heads(int heads, void *param)
{
    struct ata_drive_t *drv = &drives[vice_ptr_to_int(param)];

    if (heads > 16 || heads < 1) {
        return -1;
    }

    drv->settings_heads = heads;
    ata_image_change(drv);
    return 0;
}

static int set_sectors(int sectors, void *param)
{
    struct ata_drive_t *drv = &drives[vice_ptr_to_int(param)];

    if (sectors > 63 || sectors < 1) {
        return -1;
    }

    drv->settings_sectors = sectors;
    ata_image_change(drv);
    return 0;
}

static int set_autodetect_size(int val, void *param)
{
    int i = vice_ptr_to_int(param);

    if (drives[i].settings_autodetect_size != val) {
        drives[i].settings_autodetect_size = val;
        drives[i].update_needed = 1;
    }
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
    { "IDE64Image1", "ide.cfa", RES_EVENT_NO, NULL,
      &drives[0].filename, set_ide64_image_file, (void *)0 },
    { "IDE64Image2", "", RES_EVENT_NO, NULL,
      &drives[1].filename, set_ide64_image_file, (void *)1 },
    { "IDE64Image3", "", RES_EVENT_NO, NULL,
      &drives[2].filename, set_ide64_image_file, (void *)2 },
    { "IDE64Image4", "", RES_EVENT_NO, NULL,
      &drives[3].filename, set_ide64_image_file, (void *)3 },
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

    debug("IDE64 resource init");
    for (i = 0; i < 4; i++) {
        ata_init(&drives[i], i);
    }

    if (resources_register_string(resources_string) < 0) {
        return -1;
    }
    if (resources_register_int(resources_int) < 0) {
        return -1;
    }
    
    return 0;
}

int ide64_resources_shutdown(void)
{
    int i;

    debug("IDE64 resource shutdown");
    lib_free(ide64_configuration_string);

    for (i = 0; i < 4; i++) {
        ata_shutdown(&drives[i]);
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
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
            in_d030 = ata_register_read(&drives[idrive], addr - 0x20) | ata_register_read(&drives[idrive ^ 1], addr - 0x20);
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
        case 0x5d:
            if (settings_version4) {
                return 0xff;
            }
            break;
        case 0x5e:
            if (settings_version4) {
                return 0xc0 | vicii_read_phi1();
            }
            break;
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
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
            if (settings_version4) {
                return ata_register_peek(&drives[idrive], addr - 0x20) | ata_register_peek(&drives[idrive ^ 1], addr - 0x20);
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
        case 0x5d:
            if (settings_version4) {
                return 0xff;
            }
            break;
        case 0x5e:
            if (settings_version4) {
                return 0xc0;
            }
            break;
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
        case 0x28:
        case 0x29:
            idrive = (addr & 1) << 1;
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
            if (settings_version4) {
                out_d030 = value | (out_d030 & 0xff00);
            }
            ata_register_store(&drives[idrive], addr - 0x20, out_d030);
            ata_register_store(&drives[idrive ^ 1], addr - 0x20, out_d030);
            return;
        case 0x30:
            if (!settings_version4) {
                out_d030 = (out_d030 & 0xff00) | value;
            }
            return;
        case 0x31:
            out_d030 = (out_d030 & 0x00ff) | (value << 8);
            return;
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
            if (!settings_version4) {
                current_bank = (addr - 2) & 3;
            }
            break;
        case 0x5f:
            if ((kill_port & 2) == 0) {
                break;
            }
            ds1202_1302_set_lines(ds1302_context, 1u, 0u, value & 1u);
            ds1202_1302_set_lines(ds1302_context, 1u, 1u, value & 1u);
            return;
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
            if (settings_version4) {
                current_bank = addr & 7;
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
        case 0xfd:
        case 0xfe:
        case 0xff:
            current_cfg = (addr ^ 1) & 3;
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
    int i;

    debug("IDE64 init");
    cart_config_changed_slotmain(0, 0, CMODE_READ | CMODE_PHI2_RAM);
    current_bank = 0;
    current_cfg = 0;
    kill_port = 0;
    idrive = 0;
    ds1202_1302_set_lines(ds1302_context, 0u, 1u, 1u);

    for (i = 0; i < 4; i++) {
        if (drives[i].update_needed) {
            drives[i].update_needed = 0;
            ata_image_attach(&drives[i]);
            memset(export_ram0, 0, 0x8000);
        } else {
            ata_reset(&drives[i]);
        }
    }
}

void ide64_config_setup(BYTE *rawcart)
{
    debug("IDE64 setup");
    memcpy(roml_banks, rawcart, 0x20000);
    memcpy(romh_banks, rawcart, 0x20000);
    memset(export_ram0, 0, 0x8000);
}

void ide64_detach(void)
{
    int i;

    c64export_remove(&export_res);

    ds1202_1302_destroy(ds1302_context);

    for (i = 0; i < 4; i++) {
        ata_image_detach(&drives[i]);
    }

    io_source_unregister(ide64_list_item);
    ide64_list_item = NULL;
    debug("IDE64 detached");
}

static int ide64_common_attach(BYTE *rawcart, int detect)
{
    int i;

    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    ds1302_context = ds1202_1302_init((BYTE *)ide64_DS1302, &rtc_offset, 1302);

    if (detect) {
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
    }

    ide64_list_item = io_source_register(&ide64_device);
    debug("IDE64 attached");

    return 0;
}

int ide64_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x20000, UTIL_FILE_LOAD_SKIP_ADDRESS | UTIL_FILE_LOAD_FILL) < 0) {
        return -1;
    }

    return ide64_common_attach(rawcart, 1);
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

    return ide64_common_attach(rawcart, 1);
}

static int ide64_io1_dump(void) {
    const char *configs[4] = {
        "8k", "16k", "stnd", "open"
    };
    mon_out("Version: %d, Mode: %s, ", settings_version4 ? 4:3, (kill_port & 1) ? "Disabled" : "Enabled");
    mon_out("ROM bank: %d, Config: %s, Interface: %d\n", current_bank, configs[current_cfg], idrive >> 1);
    return 0;
}

/* ---------------------------------------------------------------------*/
/*    snapshot support functions                                             */

#define CART_DUMP_VER_MAJOR   0
#define CART_DUMP_VER_MINOR   0
#define SNAP_MODULE_NAME "CARTIDE"

int ide64_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;
    int i;

    for (i=0;i<4;i++) {
        if (ata_snapshot_write_module(&drives[i], s)) {
            return -1;
        }
    }

    m = snapshot_module_create(s, SNAP_MODULE_NAME, CART_DUMP_VER_MAJOR, CART_DUMP_VER_MINOR);

    if (m == NULL) {
        return -1;
    }

    SMW_DW(m, settings_version4);
    SMW_BA(m, roml_banks, settings_version4 ? 0x20000: 0x10000);
    SMW_BA(m, export_ram0, 0x8000);
    SMW_DW(m, current_bank);
    SMW_DW(m, current_cfg);
    SMW_B(m, kill_port);
    SMW_DW(m, idrive);
    SMW_W(m, in_d030);
    SMW_W(m, out_d030);
    SMW_BA(m, (unsigned char *)ide64_DS1302, 64); /* TODO: RTC snapshot! */

    snapshot_module_close(m);

    return 0;
}

int ide64_snapshot_read_module(snapshot_t *s)
{
    BYTE vmajor, vminor;
    snapshot_module_t *m;
    int i;

    for (i=0;i<4;i++) {
        if (ata_snapshot_read_module(&drives[i], s)) {
            return -1;
        }
    }

    m = snapshot_module_open(s, SNAP_MODULE_NAME, &vmajor, &vminor);

    if (m == NULL) {
        return -1;
    }

    if ((vmajor != CART_DUMP_VER_MAJOR) || (vminor != CART_DUMP_VER_MINOR)) {
        snapshot_module_close(m);
        return -1;
    }

    SMR_DW_INT(m, &settings_version4);
    if (settings_version4) settings_version4 = 1;
    SMR_BA(m, roml_banks, settings_version4 ? 0x20000: 0x10000);
    memcpy(romh_banks, roml_banks, settings_version4 ? 0x20000: 0x10000);
    SMR_BA(m, export_ram0, 0x8000);
    SMR_DW_INT(m, &current_bank);
    if (current_bank < 0 || current_bank > 7) current_bank = 0;
    SMR_DW_INT(m, &current_cfg);
    if (current_cfg < 0 || current_cfg > 3) current_cfg = 0;
    SMR_B(m, &kill_port);
    SMR_DW_INT(m, &idrive);
    if (idrive) idrive = 2;
    SMR_W(m, &in_d030);
    SMR_W(m, &out_d030);
    SMR_BA(m, (unsigned char *)ide64_DS1302, 64); /* TODO: RTC snapshot! */
    ide64_DS1302[64] = 0;

    snapshot_module_close(m);

    return ide64_common_attach(roml_banks, 0);
}
