/*
 * ide64.c - Cartridge handling, IDE64 cart.
 *
 * Written by
 *  Kajtar Zsolt <soci@c64.rulez.org>
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

#include "archdep.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "cmdline.h"
#include "ide64.h"
#include "log.h"
#include "resources.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
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


static const c64export_resource_t export_res = {
    "IDE64", 1, 1
};

/* Current IDE64 bank */
static unsigned int current_bank;

/* Current memory config */
static unsigned int current_cfg;

/*  */
static BYTE kill_port;

/*  */
static BYTE clock_data;

/*  */
static unsigned int clock_address;

/*  */
static unsigned int clock_tick;

/*  */
static unsigned int clock_burst;

/* IDE registers */
static BYTE ide_error;
static BYTE ide_features;
static BYTE ide_sector_count, ide_sector_count_internal;
static BYTE ide_sector;
static BYTE ide_cylinder_low;
static BYTE ide_cylinder_high;
static BYTE ide_head;
static BYTE ide_status;
static BYTE ide_control;

/* communication latch */
static WORD out_d030, in_d030;

/* buffer pointer */
static unsigned int ide_bufp;

/* active command */
static BYTE ide_cmd;

/* image file */
static FILE *ide_disk;
char *ide64_image_file = NULL;

/* config ram */
static char ide64_DS1302[65];

static char *ide64_configuration_string = NULL;

static unsigned int settings_cylinders, settings_heads, settings_sectors;
static unsigned int settings_autodetect_size;

static BYTE ide_identify[128] = {
    0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x30, 0x32, 0x33, 0x30,
    0x38, 0x30, 0x35, 0x31, 0x20, 0x20, 0x20, 0x20,

    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x32,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x49, 0x56,
    0x45, 0x43, 0x49, 0x20, 0x45, 0x44, 0x34, 0x36,

    0x20, 0x3a, 0x41, 0x4b, 0x54, 0x4a, 0x52, 0x41,
    0x5a, 0x20, 0x4f, 0x53, 0x54, 0x4c, 0x28, 0x20,
    0x4f, 0x53, 0x49, 0x43, 0x53, 0x2f, 0x4e, 0x49,
    0x55, 0x47, 0x41, 0x4c, 0x29, 0x52, 0x01, 0x00,

    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x04, 0x00,
    0x10, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static void geometry_update(void)
{
    ide_identify[108] = settings_cylinders & 255;
    ide_identify[109] = settings_cylinders >> 8;
    ide_identify[110] = settings_heads;
    ide_identify[112] = settings_sectors;
}

static int set_ide64_config(resource_value_t v, void *param)
{
    const char *cfg = (const char *)v;
    int i;

    ide64_DS1302[64] = 0;
    memset(ide64_DS1302, 0x40, 64);
    ide64_configuration_string = ide64_DS1302;

    if (cfg)
        for (i = 0; cfg[i] && i < 64; i++)
            ide64_DS1302[i] = cfg[i];

    return try_cartridge_init(16);
}

static int set_ide64_image_file(resource_value_t v, void *param)
{
    const char *name = (const char *)v;

    util_string_set(&ide64_image_file, name);

    return try_cartridge_init(32);
}

static int set_cylinders(resource_value_t v, void *param)
{
    unsigned int cylinders = (unsigned int)v;

    if (cylinders > 1024)
        return -1;

    settings_cylinders = cylinders;
    geometry_update();

    return 0;
}

static int set_heads(resource_value_t v, void *param)
{
    unsigned int heads = (unsigned int)v;

    if (heads > 16)
        return -1;

    settings_heads = heads;
    geometry_update();

    return 0;
}

static int set_sectors(resource_value_t v, void *param)
{
    unsigned int sectors = (unsigned int)v;

    if (sectors > 63)
        return -1;

    settings_sectors = sectors;
    geometry_update();

    return 0;
}

static int set_autodetect_size(resource_value_t v, void *param)
{
    settings_autodetect_size = (unsigned int)v;

    return 0;
}

static const resource_t resources[] = {
    { "IDE64Image", RES_STRING, (resource_value_t)"ide.hdd",
      RES_EVENT_NO, NULL,
      (void *)&ide64_image_file, set_ide64_image_file, NULL },
    { "IDE64Config", RES_STRING, (resource_value_t)"",
      RES_EVENT_NO, NULL,
      (void *)&ide64_configuration_string, set_ide64_config, NULL },
    { "IDE64Cylinders", RES_INTEGER, (resource_value_t)256,
      RES_EVENT_NO, NULL,
      (void *)&settings_cylinders, set_cylinders, NULL },
    { "IDE64Heads", RES_INTEGER, (resource_value_t)4,
      RES_EVENT_NO, NULL,
      (void *)&settings_heads, set_heads, NULL },
    { "IDE64Sectors", RES_INTEGER, (resource_value_t)16,
      RES_EVENT_NO, NULL,
      (void *)&settings_sectors, set_sectors, NULL },
    { "IDE64AutodetectSize", RES_INTEGER, (resource_value_t)1,
      RES_EVENT_NO, NULL,
      (void *)&settings_autodetect_size, set_autodetect_size, NULL },
    { NULL }
};

int ide64_resources_init(void)
{
    return resources_register(resources);
}

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] = {
    { "-IDE64image", SET_RESOURCE, 1, NULL, NULL, "IDE64Image", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_IDE64_NAME },
    { "-IDE64cyl", SET_RESOURCE, 1, NULL, NULL, "IDE64Cylinders", NULL,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_CYLINDERS_IDE64 },
    { "-IDE64hds", SET_RESOURCE, 1, NULL, NULL, "IDE64Heads", NULL,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_HEADS_IDE64 },
    { "-IDE64sec", SET_RESOURCE, 1, NULL, NULL, "IDE64Sectors", NULL,
      IDCLS_P_VALUE, IDCLS_SET_AMOUNT_SECTORS_IDE64 },
    { "-IDE64autosize", SET_RESOURCE, 0, NULL, NULL,
      "IDE64AutodetectSize", (void *)1,
      0, IDCLS_AUTODETECT_IDE64_GEOMETRY },
    { "+IDE64autosize", SET_RESOURCE, 0, NULL, NULL,
      "IDE64AutodetectSize", (void *)0,
      0, IDCLS_NO_AUTODETECT_IDE64_GEOMETRY },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] = {
    { "-IDE64image", SET_RESOURCE, 1, NULL, NULL, "IDE64Image", NULL,
      N_("<name>"), N_("Specify name of IDE64 image file") },
    { "-IDE64cyl", SET_RESOURCE, 1, NULL, NULL, "IDE64Cylinders", NULL,
      N_("<value>"), N_("Set number of cylinders for the IDE64 emulation") },
    { "-IDE64hds", SET_RESOURCE, 1, NULL, NULL, "IDE64Heads", NULL,
      N_("<value>"), N_("Set number of heads for the IDE64 emulation") },
    { "-IDE64sec", SET_RESOURCE, 1, NULL, NULL, "IDE64Sectors", NULL,
      N_("<value>"), N_("Set number of sectors for the IDE64 emulation") },
    { "-IDE64autosize", SET_RESOURCE, 0, NULL, NULL,
      "IDE64AutodetectSize", (void *)1,
      NULL, N_("Autodetect geometry of formatted images") },
    { "+IDE64autosize", SET_RESOURCE, 0, NULL, NULL,
      "IDE64AutodetectSize", (void *)0,
      NULL, N_("Do not autodetect geometry of formatted images") },
    { NULL }
};
#endif

int ide64_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}


/* drive reset response */
static void ide64_reset(void)
{
    ide_error = 1;
    ide_sector_count = 1;
    ide_sector = 1;
    ide_cylinder_low = 0;
    ide_cylinder_high = 0;
    ide_head = 0;
    ide_status = IDE_DRDY | IDE_DSC;
    ide_bufp = 510;
    ide_cmd = 0x00;
}

/* seek to a sector */
static int ide_seek_sector(void)
{
    unsigned int lba;

    if (ide_head & 0x40) {
        lba = ((ide_head & 0x0f) << 24) | (ide_cylinder_high << 16)
              | (ide_cylinder_low << 8) | ide_sector;
	if (lba > (unsigned int)((ide_identify[117] << 24)
            | (ide_identify[116] << 16) | (ide_identify[115] << 8)
            | ide_identify[114]))
            return 1;
    } else {
	if (ide_sector == 0 ||
    	    ide_sector > ide_identify[112] ||
    	    (ide_head & 0xf) >= ide_identify[110] ||
    	    (ide_cylinder_low | (ide_cylinder_high << 8)) >= (ide_identify[108]
            | (ide_identify[109] << 8)))
            return 1;
        lba = ((ide_cylinder_low | (ide_cylinder_high << 8)) *
              ide_identify[110]+(ide_head & 0xf)) * ide_identify[112]
              + ide_sector - 1;
    }
    return fseek(ide_disk, lba << 9, SEEK_SET);
}

BYTE REGPARM1 ide64_io1_read(WORD addr)
{
    int i;

    if (kill_port & 1)
        if (addr >= 0xde5f)
            return vicii_read_phi1();

    if (addr >= 0xde60)
        return roml_banks[(addr & 0x3fff) | (current_bank << 14)];

    switch (addr & 0xff) {
      case 0x20:
        switch (ide_cmd) {
          case 0x20:
          case 0xec:
	    in_d030 = export_ram0[ide_bufp | 0x200]
                      | (export_ram0[ide_bufp | 0x201] << 8);
    	    if (ide_bufp < 510)
                ide_bufp += 2;
            else {
		ide_sector_count_internal--;
		if (!ide_sector_count_internal) {
	    	    ide_status = ide_status & (~IDE_DRQ);
	    	    ide_cmd = 0x00;
		} else {
        	    memset(&export_ram0[0x200], 0, 512);
	            if (fread(&export_ram0[0x200], 1, 512, ide_disk) != 512) {
			ide_error = IDE_UNC|IDE_ABRT;
        		ide_status = (ide_status & (~IDE_BSY) & (~IDE_DF)
                                     & (~IDE_DRQ)) | IDE_DRDY | IDE_ERR;
        		ide_bufp = 510;
        		ide_cmd = 0x00;
		    }
		    ide_bufp = 0, ide_status = ide_status | IDE_DRQ;
		}
	    }
	    break;
          default:
            in_d030 = (WORD)vicii_read_phi1();
	}
        break;
      case 0x21:
        in_d030 = ide_error;
        break;
      case 0x22:
        in_d030 = ide_sector_count;
        break;
      case 0x23:
        in_d030 = ide_sector;
        break;
      case 0x24:
        in_d030 = ide_cylinder_low;
        break;
      case 0x25:
        in_d030 = ide_cylinder_high;
        break;
      case 0x26:
        in_d030 = ide_head;
        break;
      case 0x27:
        /* primary device only */
      case 0x2e:
        in_d030 = (ide_head & 0x10) ? 0 : ide_status;
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
        io_source=IO_SOURCE_IDE64;
        return (unsigned char)in_d030;
      case 0x31:
        io_source=IO_SOURCE_IDE64;
        return in_d030 >> 8;
      case 0x32:
        io_source=IO_SOURCE_IDE64;
        return 0x10 | (current_bank << 2) | (((current_cfg & 1) ^ 1) << 1)
               | (current_cfg >> 1);
      case 0x5f:
        io_source=IO_SOURCE_IDE64;
        if ((kill_port ^ 0x02) & 0x02)
            return 1;

        if (clock_tick < 17)
            clock_tick++;

        if (clock_tick == 16) {

            if (clock_address & 0x01) {

                if (clock_address & 0x40) {
                    i = (clock_burst & 0x1f) * 2;
                    clock_data = (ide64_DS1302[i] << 4)
                                 | (ide64_DS1302[i + 1] & 0xf);/* data */
                } else
                    clock_data = export_ram0[clock_burst & 0x1f]; /* clock */

                if (clock_burst & 0x20)
                    clock_burst++;

            }
            clock_tick = 8;
        }

        i = clock_data;
        clock_data >>= 1;
        return i;
    }
    return vicii_read_phi1();
}

void REGPARM2 ide64_io1_store(WORD addr, BYTE value)
{
    int i;
/*    log_debug("IDE64 write %02x:%02x", addr, value);*/

    if (kill_port & 1)
        if (addr >= 0xde5f)
            return;

    switch (addr & 0xff) {
      case 0x20:
        switch (ide_cmd) {
          case 0x30:
    	    export_ram0[ide_bufp | 0x200] = out_d030 & 0xff;
    	    export_ram0[ide_bufp | 0x201] = out_d030 >> 8;
    	    if (ide_bufp < 510)
                ide_bufp += 2;
            else {
                if (fwrite(&export_ram0[0x200], 1, 512, ide_disk) != 512) {
                    ide_error = IDE_UNC|IDE_ABRT;
                    goto aborted_command;
                }
                ide_sector_count_internal--;
                if (!ide_sector_count_internal) {
                    ide_status = ide_status & (~IDE_DRQ);
                    ide_cmd = 0x00;
                } else {
                    ide_bufp = 0, ide_status = ide_status | IDE_DRQ;
                }
            }
            break;
        }
        return;
      case 0x21:
        ide_features = out_d030 & 0xff;
        return;
      case 0x22:
        ide_sector_count = out_d030 & 0xff;
        return;
      case 0x23:
        ide_sector = out_d030 & 0xff;
        return;
      case 0x24:
        ide_cylinder_low = out_d030 & 0xff;
        return;
      case 0x25:
        ide_cylinder_high = out_d030 & 0xff;
        return;
      case 0x26:
        ide_head = out_d030 & 0xff;
        return;
      case 0x27:
	if (ide_head & 0x10)
            return; /* primary device only */
        if (!ide_disk)
            return; /* if image file exists? */
        switch (out_d030 & 0xff) {
          case 0x20:
          case 0x21:
            ide_status = (ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ)
                         & (~IDE_ERR)) | IDE_DRDY;
#ifdef IDE64_DEBUG
            if (ide_head & 0x40)
                log_debug("IDE64 READ (%d)*%d", (ide_cylinder_low << 8)
                          | (ide_cylinder_high << 16)
                          | ((ide_head & 0xf) << 24) | ide_sector,
                          ide_sector_count);
	    else
                log_debug("IDE64 READ (%d/%d/%d)*%d", ide_cylinder_low
                          | (ide_cylinder_high << 8), ide_head & 0xf,
                          ide_sector, ide_sector_count);
#endif
            if (ide_seek_sector()) {
                ide_error = IDE_IDNF;
                goto aborted_command;
            }
            memset(&export_ram0[0x200], 0, 512);
            if (fread(&export_ram0[0x200], 1, 512, ide_disk) != 512) {
                ide_error = IDE_UNC|IDE_ABRT;goto aborted_command;
            }
	    ide_bufp = 0, ide_status = ide_status | IDE_DRQ;
	    ide_sector_count_internal = ide_sector_count;
            ide_cmd = 0x20;
            break;
        case 0x30:
        case 0x31:
	    ide_status = (ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ)
                         & (~IDE_ERR)) | IDE_DRDY;
#ifdef IDE64_DEBUG
	    if (ide_head & 0x40)
                log_debug("IDE64 WRITE (%d)*%d",( ide_cylinder_low << 8)
                          | (ide_cylinder_high << 16)
                          | ((ide_head & 0xf) << 24) | ide_sector,
                          ide_sector_count);
	    else
                log_debug("IDE64 WRITE (%d/%d/%d)*%d", ide_cylinder_low
                          | (ide_cylinder_high << 8), ide_head & 0xf,
                          ide_sector, ide_sector_count);
#endif
            if (ide_seek_sector()) {
                ide_error = IDE_IDNF;
                goto aborted_command;
            }
            ide_bufp = 0, ide_status = ide_status | IDE_DRQ;
            ide_cmd = 0x30;
            ide_sector_count_internal = ide_sector_count;
            break;
          case 0x91:
	    ide_status = (ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ)
                         & (~IDE_ERR)) | IDE_DRDY;
            {
                unsigned long size;
#ifdef IDE64_DEBUG
                log_debug("IDE64 SETMAX (%d/%d)", (ide_head & 0xf) + 1,
                          ide_sector_count);
#endif
                size = (ide_identify[109] *256 + ide_identify[108]) *
                       ((ide_head & 0xf) + 1) * ide_sector_count;
		if (size == 0
                    || size > (unsigned long)((ide_identify[123] << 24)
                    | (ide_identify[122] << 16) | (ide_identify[121] << 8)
                    | ide_identify[120])) {
                    ide_error = IDE_ABRT;
                    goto aborted_command;
                }
                ide_identify[110] = (ide_head & 0xf) + 1;
                ide_identify[112] = ide_sector_count;
                ide_identify[114] = (BYTE)(size & 0xff); size >>= 8;
                ide_identify[115] = (BYTE)(size & 0xff); size >>= 8;
                ide_identify[116] = (BYTE)(size & 0xff); size >>= 8;
                ide_identify[117] = (BYTE)(size & 0xff);
                break;
            }
          case 0xec:
	    ide_status = (ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_ERR))
                         | IDE_DRDY | IDE_DRQ;
#ifdef IDE64_DEBUG
            log_debug("IDE64 IDENTIFY");
#endif
            ide_bufp = 0;
            memcpy(&export_ram0[0x200], &ide_identify, 128);
            memset(&export_ram0[0x280], 0, 512 - 128);
            ide_cmd = 0xec;
            ide_sector_count_internal = 1;
            break;
          default:
#ifdef IDE64_DEBUG
    	    switch (out_d030 & 0xff) {
              case 0x00:
                log_debug("IDE64 NOP");
                break;
              case 0x08:
                log_debug("IDE64 ATAPI RESET");
                break;
              case 0x94:
              case 0xe0:
                log_debug("IDE64 STANDBY IMMEDIATE");
                break;
              case 0x97:
              case 0xe3:
                log_debug("IDE64 IDLE");
                break;
              case 0xef:
                log_debug("IDE64 SET FEATURES");
                break;
              case 0xa0:
                log_debug("IDE64 PACKET");
                break;
              case 0x95:
              case 0xe1:
                log_debug("IDE64 IDLE IMMEDIATE");
                break;
              case 0xa1:
                log_debug("IDE64 IDENTIFY PACKET DEVICE");
                break;
              default:
                log_debug("IDE64 COMMAND %02x", out_d030 & 0xff);
	    }
#endif
	    ide_error = IDE_ABRT;
aborted_command:
            ide_status = (ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ))
                         | IDE_DRDY | IDE_ERR;
            ide_bufp = 510;
            ide_cmd = 0x00;
            break;
        }
        return;
      case 0x2e:
        if ((ide_control & 0x04) && ((value ^ 0x04) & 0x04)) {
            ide64_reset();
#ifdef IDE64_DEBUG
            log_debug("IDE64 RESET");
#endif
        }
        ide_control = value;
        return;
      case 0x30:
        out_d030 = (out_d030 & 0xff00) | value;
        return;
      case 0x31:
        out_d030 = (out_d030 & 0x00ff) | (value << 8);
        return;
      case 0x32:
        current_bank = 0;
        break;
      case 0x33:
        current_bank = 1;
        break;
      case 0x34:
        current_bank = 2;
        break;
      case 0x35:
        current_bank = 3;
        break;
      case 0x5f:
        if ((kill_port ^ 0x02) & 0x02)
            break;
        clock_data = (clock_data >> 1) | ((value & 1) << 7);
        if (clock_tick < 17)
            clock_tick++;
        if (clock_tick == 8) {

            clock_address=clock_data;

            clock_burst=(clock_address >> 1) & 0x1f;
            if (clock_burst == 0x1f)
                clock_burst = 0x20;

            if (clock_address & 0x01)
                clock_tick = 15;

        } else

            if (clock_tick == 16) {
                if (clock_address & 0x01)
                    break;

                if (clock_address & 0x40) {
                    i = (clock_burst & 0x1f) * 2;

                    ide64_DS1302[i] = (clock_data >> 4) | 0x40;/* data */
                    ide64_DS1302[i + 1] = (clock_data & 0xf) | 0x40;/* data */
                } else
                    export_ram0[clock_burst & 0x1f] = clock_data; /* clock */

            if (clock_burst & 0x20)
                clock_burst++;

            clock_tick = 8;
        }
        return;
      case 0xfb:
        if (((kill_port ^ 0x02) & 0x02) && (value & 0x02))
            clock_tick = 0;
        kill_port = value;
        if (kill_port & 1)
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
    cartridge_config_changed(1, (BYTE)(current_cfg | (current_bank << 3)),
                             CMODE_READ);
}


void ide64_config_init(void)
{
    cartridge_config_changed(1, 0, CMODE_READ);
    current_bank = 0;
    current_cfg = 0;
    kill_port = 0;
    clock_data = 0;
    ide64_reset();

    export_ram0[0x0000] = 0x80;
    export_ram0[0x0001] = 0x00;
    export_ram0[0x0002] = 0x00;
    export_ram0[0x0003] = 0x15;
    export_ram0[0x0004] = 0x08;
    export_ram0[0x0005] = 0x06;
    export_ram0[0x0006] = 0x03;
}

void ide64_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x10000);
    memcpy(romh_banks, rawcart, 0x10000);
    cartridge_config_changed(1, 0, CMODE_READ);
}

void ide64_detach(void)
{
    c64export_remove(&export_res);

    if (ide_disk)
        fclose(ide_disk);
#ifdef IDE64_DEBUG
    log_debug("IDE64 detached");
#endif
}

int ide64_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x10000,
        UTIL_FILE_LOAD_SKIP_ADDRESS | UTIL_FILE_LOAD_FILL) < 0)
        return -1;

    if (c64export_add(&export_res) < 0)
        return -1;

    ide_disk = fopen(ide64_image_file, MODE_READ_WRITE);

    if (!ide_disk)
        ide_disk = fopen(ide64_image_file, MODE_APPEND);

    if (!ide_disk)
        ide_disk = fopen(ide64_image_file, MODE_READ);

#ifdef IDE64_DEBUG
    log_debug("IDE64 attached");
#endif

    if (ide_disk)
        log_message(LOG_DEFAULT,
                    "IDE64: Using imagefile `%s'.", ide64_image_file);
    else
        log_message(LOG_DEFAULT,
                    "IDE64: Cannot use image file `%s'. NO DRIVE EMULATION!",
                    ide64_image_file);

    if (!settings_autodetect_size)
        return 0;

    if (ide_disk) {
        /* try to get drive geometry */
        unsigned char idebuf[24];
        int  heads, sectors, cyll, cylh, cyl, res;
        unsigned long size = 0;
	int is_chs;

        /* read header */
        res = fread(idebuf, 1, 24, ide_disk);
        if (res < 24) {
            log_message(LOG_DEFAULT, "IDE64: Couldn't read disk geometry from image, using default 8MB.");
            return 0;
        }
        /* check signature */
	
	for (;;) {

    	    res = memcmp(idebuf,"C64-IDE V", 9);

	    if (res == 0) { /* old filesystem always CHS */
    		cyl = (idebuf[0x10] << 8) | idebuf[0x11];
    		heads = idebuf[0x12] & 0x0f;
    		sectors = idebuf[0x13];
		is_chs=1;
		break;		/* OK */
	    }

    	    res = memcmp(idebuf+8,"C64 CFS V", 9);

	    if (res == 0) {
		if (idebuf[0x04] & 0x40) { /* LBA */
		    size = ((idebuf[0x04] & 0x0f) << 24) | (idebuf[0x05] << 16)
                           | (idebuf[0x06] << 8) | idebuf[0x07];
		    cyl = heads = sectors = 1; /* fake */
		    is_chs = 0;
		} else { /* CHS */
    		    cyl = (idebuf[0x05] << 8) | idebuf[0x06];
    		    heads = idebuf[0x04] & 0x0f;
    		    sectors = idebuf[0x07];
		    is_chs = 1;
		}
		break;		/* OK */
	    }
	    
            log_message(LOG_DEFAULT, "IDE64: Disk is not formatted, using default 8MB.");
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

        settings_cylinders = cyl;
        settings_heads = heads;
        settings_sectors = sectors;

	cyll = cyl & 0xff;
        cylh = cyl >> 8;
        ide_identify[0x02] = cyll;        ide_identify[108] = cyll;
        ide_identify[0x03] = cylh;        ide_identify[109] = cylh;
        ide_identify[0x06] = heads;       ide_identify[110] = heads;
        ide_identify[0x0c] = sectors;     ide_identify[112] = sectors;

        ide_identify[114] = (BYTE)(size & 0xff);size >>= 8;
        ide_identify[115] = (BYTE)(size & 0xff);size >>= 8;
        ide_identify[116] = (BYTE)(size & 0xff);size >>= 8;
        ide_identify[117] = (BYTE)(size & 0xff);

	memcpy(ide_identify + 120, ide_identify + 114, 4);
    }

    return 0;
}

