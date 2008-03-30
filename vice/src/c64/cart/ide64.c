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

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archdep.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "ide64.h"
#include "types.h"
#include "utils.h"
#include "vicii-phi1.h"


/* Current IDE64 bank */
static unsigned int current_bank;

/* Current memory config */
static unsigned int current_cfg;

/*  */
static unsigned int kill_port;

/*  */
static unsigned int clock_data;

/*  */
static unsigned int clock_address;

/*  */
static unsigned int clock_tick;

/*  */
static unsigned int clock_burst;

static unsigned int ide_error;
static unsigned int ide_sector_count;
static unsigned int ide_sector;
static unsigned int ide_cylinder_low;
static unsigned int ide_cylinder_high;
static unsigned int ide_head;
static unsigned int ide_status_cmd;

static unsigned int outd030;

static unsigned int ide_bufp;

static unsigned int ide_cmd;

FILE *ide_disk;

static BYTE ide_identify[128]=
{  
    0x40,0x00,0x00,0x01,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x30,0x32,0x32,0x30,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x31,
    0x20,0x20,0x20,0x20,0x20,0x20,0x49,0x56,0x45,0x43,0x49,0x20,0x45,0x44,0x34,0x36,
    0x20,0x3a,0x41,0x4b,0x54,0x4a,0x52,0x41,0x5a,0x20,0x4f,0x53,0x54,0x4c,0x28,0x20,
    0x4f,0x53,0x49,0x43,0x53,0x2f,0x4e,0x49,0x55,0x47,0x41,0x4c,0x29,0x52,0x01,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x01,0x04,0x00,
    0x10,0x00,0x00,0x40,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

int ide_seek_sector(void)
{
    ide_status_cmd=IDE_DSC;
    if (ide_sector==0 ||
        ide_sector>ide_identify[112] ||
        (ide_head & 0xf)>=ide_identify[110] ||
        (ide_cylinder_low | (ide_cylinder_high << 8))>=(ide_identify[108] | (ide_identify[109] << 8))) return 1;
    return fseek(ide_disk,(((ide_cylinder_low | (ide_cylinder_high << 8))*ide_identify[110]+(ide_head & 0xf))*ide_identify[112]+ide_sector-1) << 9,SEEK_SET);
}

BYTE REGPARM1 ide64_io1_read(ADDRESS addr)
{
    int i;

    if (kill_port & 1) return vicii_read_phi1();
    if (addr>=0xde60) return roml_banks[(addr & 0x3fff) | (current_bank << 14)];
/*    log_debug("IDE64 read %02x", addr); */
    switch (addr & 0xff) {
    case 0x20:outd030=export_ram0[ide_bufp | 0x200] | (export_ram0[ide_bufp | 0x201] << 8);
        if (ide_bufp<510) ide_bufp+=2; else ide_status_cmd=(ide_status_cmd & (~IDE_DRQ)) | IDE_DRDY;
        break;
    case 0x21:outd030=ide_error;break;
    case 0x22:outd030=ide_sector_count;break;
    case 0x23:outd030=ide_sector;break;
    case 0x24:outd030=ide_cylinder_low;break;
    case 0x25:outd030=ide_cylinder_high;break;
    case 0x26:outd030=(ide_head | 0xa0);break;
    case 0x27:outd030=ide_status_cmd;break;
    case 0x2e:outd030=ide_status_cmd;break;
    case 0x2f:outd030=(unsigned int)rand();break;
    case 0x30:return outd030;
    case 0x31:return outd030 >> 8;
    case 0x5f:
            if ((kill_port ^ 0x02) & 0x02) return 1;

            if (clock_tick<17) clock_tick++;

            if (clock_tick==16) {

                if (clock_address & 0x01) {

                    if (clock_address & 0x40) clock_data=export_ram0[clock_burst & 0x1f];/* data */
                        else clock_data=export_ram0[(clock_burst & 0x1f) | 0x100]; /* clock */

                    if (clock_burst & 0x20) clock_burst++;

                }
                clock_tick=8;
            }

            i=clock_data;clock_data>>=1;
            return i;

    }
    return vicii_read_phi1();
}

void REGPARM2 ide64_io1_store(ADDRESS addr, BYTE value)
{
/*    log_debug("IDE64 write %02x:%02x", addr, value); */

    if (kill_port & 1) return;
    switch (addr & 0xff) {
    case 0x20:
        export_ram0[ide_bufp | 0x200]=outd030 & 0xff;
        export_ram0[ide_bufp | 0x201]=outd030 >> 8;
        if (ide_bufp<510) ide_bufp+=2; else 
        {
            switch (ide_cmd) {
            case 0x30:
                fseek(ide_disk,(((ide_cylinder_low | (ide_cylinder_high << 8))*ide_identify[6]+(ide_head & 0xf))*ide_identify[12]+ide_sector-1) << 9,SEEK_SET);
                fwrite(&export_ram0[0x200],1,512,ide_disk);
                ide_status_cmd=(ide_status_cmd & (~IDE_DRQ)) | IDE_DRDY;
                break;
            }
            ide_cmd=0;
        }
        break;
    case 0x21:ide_error=outd030 & 0xff;break;
    case 0x22:ide_sector_count=outd030 & 0xff;break;
    case 0x23:ide_sector=outd030 & 0xff;break;
    case 0x24:ide_cylinder_low=outd030 & 0xff;break;
    case 0x25:ide_cylinder_high=outd030 & 0xff;break;
    case 0x26:
        ide_head=outd030 & 0xff;
        ide_status_cmd=IDE_DRDY|IDE_DSC;
        break;
    case 0x27:
        if ((ide_head ^ 0x10) & 0x10) /* primary device only */
        switch (outd030 & 0xff) {
        case 0x20:
#ifdef IDE64_DEBUG
            log_debug("IDE64 READ (%d/%d/%d)",ide_cylinder_low|(ide_cylinder_high << 8), ide_head & 0xf, ide_sector);
#endif
            if (ide_seek_sector()) break;
            ide_bufp=0;
            memset(&export_ram0[0x200],0,512);
            fread(&export_ram0[0x200],1,512,ide_disk);
            ide_status_cmd=IDE_DSC|IDE_DRQ;
            break;
        case 0x30:
#ifdef IDE64_DEBUG
            log_debug("IDE64 WRITE (%d/%d/%d)",ide_cylinder_low|(ide_cylinder_high << 8), ide_head & 0xf, ide_sector);
#endif
            if (ide_seek_sector()) break;
            ide_bufp=0;
            ide_status_cmd=IDE_DSC|IDE_DRQ;
            ide_cmd=0x30;
            break;
        case 0x91:
            {
                unsigned long size;
#ifdef IDE64_DEBUG
                log_debug("IDE64 SETMAX (%d/%d)", (ide_head+1) & 0xf, ide_sector_count);
#endif
                ide_identify[110]=(ide_head+1) & 0xf;
                ide_identify[112]=ide_sector_count;
                size=(ide_identify[109]*256+ide_identify[108])*((ide_head+1) & 0xf)*ide_sector_count;
                ide_identify[114]=size & 0xff;size>>=8;
                ide_identify[115]=size & 0xff;size>>=8;
                ide_identify[116]=size & 0xff;size>>=8;
                ide_identify[117]=size & 0xff;
                ide_status_cmd=IDE_DRDY|IDE_DSC;
                break;
            }
#ifdef IDE64_DEBUG
        case 0x00:
            log_debug("IDE64 NOP");
            ide_status_cmd=IDE_DRDY|IDE_DSC|IDE_ERR;
            ide_error=IDE_ABRT;
            break;
        case 0x08:
            log_debug("IDE64 ATAPI RESET");
            ide_status_cmd=IDE_DRDY|IDE_DSC;
            ide_status_cmd=IDE_DRDY|IDE_DSC|IDE_ERR;
            ide_error=IDE_ABRT;
            break;
        case 0x94:
        case 0xe0:
            log_debug("IDE64 STANDBY IMMEDIATE");
            ide_status_cmd=IDE_DRDY|IDE_DSC|IDE_ERR;
            ide_error=IDE_ABRT;
            break;
        case 0x97:
        case 0xe3:
            log_debug("IDE64 POWER MANAGEMENT");
            ide_status_cmd=IDE_DRDY|IDE_DSC|IDE_ERR;
            ide_error=IDE_ABRT;
            break;
        case 0xef:
            log_debug("IDE64 SET FEATURES");
            ide_status_cmd=IDE_DRDY|IDE_DSC|IDE_ERR;
            ide_error=IDE_ABRT;
            break;
#endif
        case 0xec:
#ifdef IDE64_DEBUG
            log_debug("IDE64 IDENTIFY");
#endif
            ide_bufp=0;
            ide_status_cmd=IDE_DSC|IDE_DRQ;
            memset(&export_ram0[0x200],0,512);
            memcpy(&export_ram0[0x200],&ide_identify,128);
            break;
        default:
#ifdef IDE64_DEBUG
            log_debug("IDE64 COMMAND %02x",outd030 & 0xff);
#endif
            ide_status_cmd=IDE_DRDY|IDE_DSC|IDE_ERR;
            ide_error=IDE_ABRT;
            ide_bufp=510;
            ide_cmd=0x00;
            break;
        }
        break;
    case 0x2e:break;
    case 0x2f:break;
    case 0x30:outd030=(outd030 & 0xff00) | value;break;
    case 0x31:outd030=(outd030 & 0x00ff) | (value << 8);break;
    case 0x32:current_bank=0;break;
    case 0x33:current_bank=1;break;
    case 0x34:current_bank=2;break;
    case 0x35:current_bank=3;break;
    case 0x5f:
            if ((kill_port ^ 0x02) & 0x02) break;
            clock_data=(clock_data >> 1) | ((value & 1) << 7);
            if (clock_tick<17) clock_tick++;
            if (clock_tick==8) {
                clock_address=clock_data;

/*                log_debug("IDE64 DS1302 command %02x", clock_address); */

                clock_burst=(clock_address >> 1) & 0x1f;
                if (clock_burst==0x1f) clock_burst=0x20;

                if (clock_address & 0x01) clock_tick=15;

            } else

            if (clock_tick==16) {
                if (clock_address & 0x01) break;

                if (clock_address & 0x40) export_ram0[clock_burst & 0x1f]=clock_data;/* data */
                    else export_ram0[(clock_burst & 0x1f) | 0x100]=clock_data; /* clock */

/*                log_debug("IDE64 DS1302 write %02x:%02x", clock_burst, clock_data); */

                if (clock_burst & 0x20) clock_burst++;

                clock_tick=8;
            }
        break;
    case 0xfb:if (((kill_port ^ 0x02) & 0x02) && (value & 0x02)) clock_tick=0;
        kill_port=value;
        break;
    case 0xfc:current_cfg=1;break;
    case 0xfd:current_cfg=0;break;
    case 0xfe:current_cfg=3;break;
    case 0xff:current_cfg=2;break;
    default:return;
    }
    cartridge_config_changed(1, current_cfg | (current_bank << 3), CMODE_READ);
}

void ide64_config_init(void)
{
    cartridge_config_changed(1, 0, CMODE_READ);
    current_bank=0;
    current_cfg=0;
    kill_port=0;
    clock_data=0;
    ide_bufp=0;
    ide_status_cmd=IDE_DSC;
    ide_cmd=0;
    export_ram0[0x0000]=0x08;
    export_ram0[0x0001]=0x0c;
    export_ram0[0x0002]=0x0d;
    export_ram0[0x0003]=0x0e;
    export_ram0[0x0004]=0x06;
    export_ram0[0x0005]=0x0e;
    export_ram0[0x0006]=0x0e;
    export_ram0[0x0007]=0x80;
    export_ram0[0x0008]=0xff;
    export_ram0[0x0009]=0x00;
    export_ram0[0x000a]=0x7e;

    export_ram0[0x0100]=0x80;
    export_ram0[0x0101]=0x00;
    export_ram0[0x0102]=0x00;
    export_ram0[0x0103]=0x19;
    export_ram0[0x0104]=0x05;
    export_ram0[0x0105]=0x01;
    export_ram0[0x0106]=0x02;
}

void ide64_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x10000);
    memcpy(romh_banks, rawcart, 0x10000);
    cartridge_config_changed(1, 0, CMODE_READ);
}

void ide64_detach(void)
{
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

    ide_disk=fopen("ide.hdd",MODE_READ_WRITE);
    if (!ide_disk) ide_disk=fopen("ide.hdd",MODE_APPEND);
    if (!ide_disk) return -1;

#ifdef IDE64_DEBUG
    log_debug("IDE64 attached");
#endif
    return 0;
}
