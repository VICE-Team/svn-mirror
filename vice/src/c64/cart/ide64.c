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
#include "log.h"

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
static BYTE ide_sector_count;
static BYTE ide_sector;
static BYTE ide_cylinder_low;
static BYTE ide_cylinder_high;
static BYTE ide_head;
static BYTE ide_status;
static BYTE ide_control;

/* communication buffer */
static WORD outd030;

/* buffer pointer */
static unsigned int ide_bufp;

/* active command */
static BYTE ide_cmd;

/* image file */
FILE *ide_disk;
char *ide64_image_file = NULL;

/* config ram */
char ide64_DS1302[65];

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

/* drive reset response */
void ide64_reset(void)
{
    ide_error=1;
    ide_sector_count=1;
    ide_sector=1;
    ide_cylinder_low=0;
    ide_cylinder_high=0;
    ide_head=0;
    ide_status=IDE_DRDY | IDE_DSC;
    ide_bufp=510;
    ide_cmd=0x00;
}

/* seek to a sector */
int ide_seek_sector(void)
{
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

    switch (addr & 0xff) {
    case 0x20:
	switch (ide_cmd) {
	case 0x20:
	case 0xec:
	    outd030=export_ram0[ide_bufp | 0x200] | (export_ram0[ide_bufp | 0x201] << 8);
    	    if (ide_bufp<510) ide_bufp+=2; else {
	        ide_status=ide_status & (~IDE_DRQ);
	        ide_cmd=0x00;
	    }
	    break;
	default:outd030=(WORD)rand();
	}
        break;
    case 0x21:outd030=ide_error;break;
    case 0x22:outd030=ide_sector_count;break;
    case 0x23:outd030=ide_sector;break;
    case 0x24:outd030=ide_cylinder_low;break;
    case 0x25:outd030=ide_cylinder_high;break;
    case 0x26:outd030=ide_head;break;
    case 0x27:/* primary device only */
    case 0x2e:outd030=(ide_head & 0x10)?0:ide_status;break;
    case 0x28:
    case 0x29:
    case 0x2a:
    case 0x2b:
    case 0x2c:
    case 0x2d:
    case 0x2f:outd030=(WORD)rand();break;
    case 0x30:return outd030;
    case 0x31:return outd030 >> 8;
    case 0x5f:
            if ((kill_port ^ 0x02) & 0x02) return 1;

            if (clock_tick<17) clock_tick++;

            if (clock_tick==16) {

                if (clock_address & 0x01) {

                    if (clock_address & 0x40) {
			    i=(clock_burst & 0x1f)*2;
			    clock_data=(ide64_DS1302[i] << 4) | (ide64_DS1302[i+1] & 0xf);/* data */
			}
                        else clock_data=export_ram0[clock_burst & 0x1f]; /* clock */

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
    int i;
/*    log_debug("IDE64 write %02x:%02x", addr, value);*/

    if (kill_port & 1) return;
    switch (addr & 0xff) {
    case 0x20:
        switch (ide_cmd) {
        case 0x30:
    	    export_ram0[ide_bufp | 0x200]=outd030 & 0xff;
    	    export_ram0[ide_bufp | 0x201]=outd030 >> 8;
    	    if (ide_bufp<510) ide_bufp+=2; else {
            	    if (fwrite(&export_ram0[0x200],1,512,ide_disk)!=512) {
			ide_error=IDE_UNC|IDE_ABRT;goto aborted_command;
		    }
		ide_status=ide_status & (~IDE_DRQ);
        	ide_cmd=0x00;
	    }
	    break;
        }
        break;
    case 0x21:ide_features=outd030 & 0xff;break;
    case 0x22:ide_sector_count=outd030 & 0xff;break;
    case 0x23:ide_sector=outd030 & 0xff;break;
    case 0x24:ide_cylinder_low=outd030 & 0xff;break;
    case 0x25:ide_cylinder_high=outd030 & 0xff;break;
    case 0x26:ide_head=outd030 & 0xff;break;
    case 0x27:
	if (ide_head & 0x10) break; /* primary device only */
        if (!ide_disk) break; /* if image file exists? */
        switch (outd030 & 0xff) {
        case 0x20:
        case 0x21:
	    ide_status=(ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ) & (~IDE_ERR)) | IDE_DRDY;
#ifdef IDE64_DEBUG
            log_debug("IDE64 READ (%d/%d/%d)",ide_cylinder_low|(ide_cylinder_high << 8), ide_head & 0xf, ide_sector);
#endif
            if (ide_seek_sector()) {ide_error=IDE_IDNF;goto aborted_command;}
            memset(&export_ram0[0x200],0,512);
            if (fread(&export_ram0[0x200],1,512,ide_disk)!=512) {ide_error=IDE_UNC|IDE_ABRT;goto aborted_command;}
	    ide_bufp=0,ide_status=ide_status | IDE_DRQ;
            ide_cmd=0x20;
            break;
        case 0x30:
        case 0x31:
	    ide_status=(ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ) & (~IDE_ERR)) | IDE_DRDY;
#ifdef IDE64_DEBUG
            log_debug("IDE64 WRITE (%d/%d/%d)",ide_cylinder_low|(ide_cylinder_high << 8), ide_head & 0xf, ide_sector);
#endif
            if (ide_seek_sector()) {ide_error=IDE_IDNF;goto aborted_command;}
            ide_bufp=0,ide_status=ide_status | IDE_DRQ;
            ide_cmd=0x30;
            break;
        case 0x91:
	    ide_status=(ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ) & (~IDE_ERR)) | IDE_DRDY;
            {
                unsigned long size;
#ifdef IDE64_DEBUG
                log_debug("IDE64 SETMAX (%d/%d)", (ide_head & 0xf)+1, ide_sector_count);
#endif
                size=(ide_identify[109]*256+ide_identify[108])*((ide_head & 0xf)+1)*ide_sector_count;
		if (size==0 || size>16384) {ide_error=IDE_ABRT;goto aborted_command;}
                ide_identify[110]=(ide_head & 0xf)+1;
                ide_identify[112]=ide_sector_count;
                ide_identify[114]=size & 0xff;size>>=8;
                ide_identify[115]=size & 0xff;size>>=8;
                ide_identify[116]=size & 0xff;size>>=8;
                ide_identify[117]=size & 0xff;
                break;
            }
        case 0xec:
	    ide_status=(ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_ERR)) | IDE_DRDY | IDE_DRQ;
#ifdef IDE64_DEBUG
            log_debug("IDE64 IDENTIFY");
#endif
            ide_bufp=0;
            memcpy(&export_ram0[0x200],&ide_identify,128);
            memset(&export_ram0[0x280],0,512-128);
            ide_cmd=0xec;
            break;
        default:
#ifdef IDE64_DEBUG
    	    switch (outd030 & 0xff) {
    		case 0x00:log_debug("IDE64 NOP");break;
    		case 0x08:log_debug("IDE64 ATAPI RESET");break;
    		case 0x94:
    		case 0xe0:log_debug("IDE64 STANDBY IMMEDIATE");break;
    		case 0x97:
    		case 0xe3:log_debug("IDE64 IDLE");break;
    		case 0xef:log_debug("IDE64 SET FEATURES");break;
		case 0xa0:log_debug("IDE64 PACKET");break;
		case 0x95:
		case 0xe1:log_debug("IDE64 IDLE IMMEDIATE");break;
    		case 0xa1:log_debug("IDE64 IDENTIFY PACKET DEVICE");break;
		default:log_debug("IDE64 COMMAND %02x",outd030 & 0xff);
	    }
#endif
	    ide_error=IDE_ABRT;
aborted_command:
            ide_status=(ide_status & (~IDE_BSY) & (~IDE_DF) & (~IDE_DRQ)) | IDE_DRDY | IDE_ERR;
            ide_bufp=510;
            ide_cmd=0x00;
            break;
        }
        break;
    case 0x2e:
	if ((ide_control & 0x04) && ((value ^ 0x04) & 0x04)) {
            ide64_reset();
#ifdef IDE64_DEBUG
            log_debug("IDE64 RESET");
#endif
        }
        ide_control=value;
        break;
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

                clock_burst=(clock_address >> 1) & 0x1f;
                if (clock_burst==0x1f) clock_burst=0x20;

                if (clock_address & 0x01) clock_tick=15;

            } else

            if (clock_tick==16) {
                if (clock_address & 0x01) break;

                if (clock_address & 0x40) {
		    i=(clock_burst & 0x1f)*2;

		    ide64_DS1302[i]=(clock_data >> 4) | 0x40;/* data */
		    ide64_DS1302[i+1]=(clock_data & 0xf) | 0x40;/* data */
		    }
                    else export_ram0[clock_burst & 0x1f]=clock_data; /* clock */

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
    ide64_reset();

    export_ram0[0x0000]=0x80;
    export_ram0[0x0001]=0x00;
    export_ram0[0x0002]=0x00;
    export_ram0[0x0003]=0x19;
    export_ram0[0x0004]=0x05;
    export_ram0[0x0005]=0x01;
    export_ram0[0x0006]=0x02;
}

void ide64_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x10000);
    memcpy(romh_banks, rawcart, 0x10000);
    cartridge_config_changed(1, 0, CMODE_READ);
}

void ide64_detach(void)
{
    if (ide_disk) fclose(ide_disk);
#ifdef IDE64_DEBUG
    log_debug("IDE64 detached");
#endif
}

int ide64_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x10000,
        UTIL_FILE_LOAD_SKIP_ADDRESS | UTIL_FILE_LOAD_FILL) < 0)
        return -1;

    ide_disk=fopen(ide64_image_file,MODE_READ_WRITE);
    if (!ide_disk) ide_disk=fopen(ide64_image_file,MODE_APPEND);
    if (!ide_disk) ide_disk=fopen(ide64_image_file,MODE_READ);

#ifdef IDE64_DEBUG
    log_debug("IDE64 attached");
#endif

    if (ide_disk) log_message(LOG_DEFAULT, _("IDE64: Using imagefile `%s'."), ide64_image_file);
	else log_message(LOG_DEFAULT, _("IDE64: Cannot use image file `%s'. NO DRIVE EMULATION!"), ide64_image_file);

    return 0;
}
