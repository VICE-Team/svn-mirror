/*
 * ata.c - ATA(PI) device emulation
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
#include <string.h>

#include "archdep.h"
#include "log.h"
#include "ata.h"
#include "snapshot.h"
#include "types.h"
#include "util.h"
#include "lib.h"
#include "alarm.h"
#include "maincpu.h"

#define ATA_UNC  0x40
#define ATA_IDNF 0x10
#define ATA_ABRT 0x04
#define ATA_WP 0x40
#define ATA_DRDY 0x40
#define ATA_DRQ 0x08
#define ATA_ERR 0x01
#define ATA_LBA 0x40
#define ATA_COPYRIGHT "KAJTAR ZSOLT (SOCI/SINGULAR)"
#define ATA_SERIAL_NUMBER &"$Date::                      $"[8]
#define ATA_REVISION &"$Revision::          $"[12]

#ifdef ATA_DEBUG
#define debug(args...) log_message(drv->log, args)
#else
#define debug(args...) {}
#endif
#define putw(a,b) {result[(a)*2]=(b) & 0xff;result[(a)*2+1]=(b) >> 8;}
#define setb(a,b,c) {result[(a)*2+(b)/8]|=(c) ? (1 << ((b) & 7)) : 0;}

static const BYTE identify[128] = {
    0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x30, 0x32, 0x31, 0x31,
    0x38, 0x30, 0x30, 0x32, 0x20, 0x20, 0x20, 0x20,

    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x36,
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

static void ident_update_string(BYTE *b, char *s, int n) { 	 
    int i; 	 
    for (i=0;i<n;i+=2) { 	 
        b[i | 1] = *s ? *s++ : 0x20; 	 
        b[i] = *s ? *s++ : 0x20; 	 
    } 	 
} 	 

static void ata_change_power_mode(struct ata_drive_t *drv, BYTE value) {
    if (drv->power == 0x00 && value != 0x00) {
        drv->busy = 1;
        alarm_set(drv->bsy_alarm, maincpu_clk + drv->spinup_time);
    }
    if (drv->power != 0x00 && value == 0x00) {
        drv->busy = 1;
        alarm_set(drv->bsy_alarm, maincpu_clk + drv->spindown_time);
    }
    drv->power = value;
}

static void drive_diag(struct ata_drive_t *drv)
{
    drv->error = 1;
    drv->sector_count = 1;
    drv->sector = 1;
    drv->cylinder = drv->atapi ? 0xeb14 : 0x0000;
    drv->head = drv->slave ? 0x10 : 0x00;
    drv->bufp = drv->sector_size;
    drv->cmd = 0x08;
}

static int seek_sector(struct ata_drive_t *drv)
{
    int lba;

    drv->bufp = drv->sector_size;
    drv->error = 0;

    if (drv->atapi) {
        lba = (drv->packet[2] << 24) | (drv->packet[3] << 16) | (drv->packet[4] << 8) | drv->packet[5];
    } else if (drv->lba && (drv->head & ATA_LBA)) {
        lba = ((drv->head & 0x0f) << 24) | (drv->cylinder << 8) | drv->sector;
    } else {
        if (drv->sector == 0 || drv->sector > drv->sectors || (drv->head & 0xf) >= drv->heads ||
            drv->cylinder >= drv->cylinders) {
            lba = -1;
        }
        lba = (drv->cylinder * drv->heads + (drv->head & 0xf)) * drv->sectors + drv->sector - 1;
    }

    if (lba >= drv->size) {
        lba = -1;
    }

    if (!drv->file) {
        drv->error = drv->atapi ? 0x24 : ATA_ABRT;
    } else if (lba < 0 || fseek(drv->file, (off_t)lba * drv->sector_size, SEEK_SET)) {
        drv->error = drv->atapi ? 0x54 : ATA_IDNF;
    } else {
        drv->busy = 1;
        alarm_set(drv->bsy_alarm, maincpu_clk + (CLOCK)(abs(drv->pos - lba) * drv->seek_time / drv->size));
        drv->pos = lba;
    }
    drv->cmd = 0x00;
    return drv->error;
}

static int read_sector(struct ata_drive_t *drv)
{
    drv->bufp = drv->sector_size;
    drv->error = 0;

    if (drv->attention && drv->atapi) {
        drv->attention = 0;
        drv->error = 0x64;
        drv->cmd = 0x00;
        return drv->error;
    }

    if (!drv->file) {
        drv->error = drv->atapi ? 0x24: ATA_ABRT;
        drv->cmd = 0x00;
        return drv->error;
    }

    memset(drv->buffer, 0, drv->sector_size);
    clearerr(drv->file);
    ata_change_power_mode(drv, 0xff);
    fread(drv->buffer, 1, drv->sector_size, drv->file);

    if (ferror(drv->file)) {
        drv->error = drv->atapi ? 0x54: (ATA_UNC | ATA_ABRT);
        drv->cmd = 0x00;
    } else {
        drv->pos++;
        drv->bufp = 0;
    }
    return drv->error;
}

static int write_sector(struct ata_drive_t *drv)
{
    drv->bufp = drv->sector_size;
    drv->error = 0;

    if (drv->attention && drv->atapi) {
        drv->attention = 0;
        drv->error = 0x64;
        drv->cmd = 0x00;
        return drv->error;
    }

    if (!drv->file) {
        drv->error = drv->atapi ? 0x24: ATA_ABRT;
        drv->cmd = 0x00;
        return drv->error;
    }

    if (drv->readonly) {
        drv->error = drv->atapi ? 0x74 : (ATA_WP | ATA_ABRT);
        drv->cmd = 0x00;
        return drv->error;
    }

    ata_change_power_mode(drv, 0xff);
    if (fwrite(drv->buffer, 1, drv->sector_size, drv->file) != drv->sector_size) {
        drv->error = drv->atapi ? 0x54 : (ATA_UNC | ATA_ABRT);
        drv->cmd = 0x00;
    } else {
        drv->pos++;
    }

    if (!drv->wcache) {
        if (fflush(drv->file)) {
            drv->error = drv->atapi ? 0x54 : (ATA_UNC | ATA_ABRT);
            drv->cmd = 0x00;
        }
    }
    return drv->error;
}

void ata_reset(struct ata_drive_t *drv)
{
    BYTE oldcmd = drv->cmd;

    drive_diag(drv);

    if (oldcmd != 0xe6) {
        drv->head = 0;
        drv->sectors = drv->default_sectors;
        drv->heads = drv->default_heads;
        drv->cylinders = drv->default_cylinders;
    }
}

static void ata_poweron(struct ata_drive_t *drv)
{
    drv->wcache = 0;
    drv->lookahead = 0;
    drv->power = 0x00;
    drv->attention = 1;
    drv->cmd = 0x00;

    ata_reset(drv);
    ata_change_power_mode(drv, 0xff);
}

static void ata_bsy_alarm_handler(CLOCK offset, void *data) {
    struct ata_drive_t *drv = (struct ata_drive_t *)data;

    drv->busy = 0;
    alarm_unset(drv->bsy_alarm);
}

void ata_init(struct ata_drive_t *drv, int drive)
{
    char *name;
    drv->myname = lib_msprintf("ATA%d", drive);
    drv->log = log_open(drv->myname);
    drv->file = NULL;
    drv->filename = NULL;
    drv->update_needed = 0;
    drv->type = ATA_DRIVE_NONE;
    drv->buffer = lib_malloc(2048);
    drv->slave = drive & 1;
    drv->settings_cylinders = 0;
    drv->settings_heads = 0;
    drv->settings_sectors = 0;
    drv->auto_cylinders = 0;
    drv->auto_heads = 0;
    drv->auto_sectors = 0;
    drv->auto_size = 0;
    name = lib_msprintf("%sBSY", drv->myname);
    drv->bsy_alarm = alarm_new(maincpu_alarm_context, name, ata_bsy_alarm_handler, drv);
    lib_free(name);
}

void ata_shutdown(struct ata_drive_t *drv) {
    if (drv->filename) {
        lib_free(drv->filename);
        drv->filename = NULL;
    }
    log_close(drv->log);
    lib_free(drv->myname);
    lib_free(drv->buffer);
}

static void ata_execute_command(struct ata_drive_t *drv, BYTE value) {
    if (((drv->head >> 4) & 1) != drv->slave && value != 0x90) {
        return;
    }
    if (drv->cmd == 0xe6) {
        return;
    }
    drv->bufp = drv->sector_size;
    drv->error = 0;
    drv->cmd = 0x00;
    switch (value) {
        case 0x00:
            debug("NOP");
            drv->error = ATA_ABRT;
            return;
        case 0x20:
        case 0x21:
            if (drv->lba && (drv->head & ATA_LBA)) {
                debug("READ SECTORS (%d)*%d", ((drv->head & 0xf) << 24) | (drv->cylinder << 8) | drv->sector, drv->sector_count);
            } else {
                debug("READ SECTORS (%d/%d/%d)*%d", drv->cylinder, drv->head & 0xf, drv->sector, drv->sector_count);
            }
            drv->sector_count_internal = drv->sector_count;
            if (seek_sector(drv)) {
                return;
            }
            drv->cmd = 0x20;
            read_sector(drv);
            return;
        case 0x30:
        case 0x31:
            if (drv->lba && (drv->head & ATA_LBA)) {
                debug("WRITE SECTORS (%d)*%d", ((drv->head & 0xf) << 24) | (drv->cylinder << 8) | drv->sector, drv->sector_count);
            } else {
                debug("WRITE SECTORS (%d/%d/%d)*%d", drv->cylinder, drv->head & 0xf, drv->sector, drv->sector_count);
            }
            drv->sector_count_internal = drv->sector_count;
            if (seek_sector(drv)) {
                return;
            }
            if (drv->readonly) {
                drv->error = ATA_WP | ATA_ABRT;
                return;
            }
            drv->bufp = 0;
            drv->cmd = 0x30;
            return;
        case 0x90:
            debug("EXECUTE DEVICE DIAGNOSTIC");
            drive_diag(drv);
            return;
        case 0x91:
            drv->heads = (drv->head & 0xf) + 1;
            drv->sectors = drv->sector_count;
            if (drv->sectors < 1 || drv->sectors > 63) {
                drv->cylinders = 0;
            } else {
                int size = drv->size;
                if (size > 16514064) size = 16514064;
                size /= drv->heads * drv->sectors;
                drv->cylinders = (size > 65535) ? 65535 : size;
            }
            debug("INITIALIZE DEVICE PARAMETERS (%d/%d/%d)", drv->cylinders, drv->heads, drv->sectors);
            if (drv->cylinders == 0) {
                drv->heads = 0;
                drv->sectors = 0;
                drv->error = ATA_ABRT;
                return;
            } 
            return;
        case 0x94:
        case 0xe0:
            debug("STANDBY IMMEDIATE");
            ata_change_power_mode(drv, 0x00);
            return;
        case 0x95:
        case 0xe1:
            debug("IDLE IMMEDIATE");
            ata_change_power_mode(drv, 0x80);
            return;
        case 0x96:
        case 0xe2:
            debug("STANDBY %02x", drv->sector_count);
            ata_change_power_mode(drv, 0x00);
            return;
        case 0x97:
        case 0xe3:
            debug("IDLE %02x", drv->sector_count);
            ata_change_power_mode(drv, 0x80);
            return;
        case 0xe4:
            debug("READ BUFFER");
            drv->sector_count_internal = 1;
            drv->bufp = 0;
            drv->cmd = 0xe4;
            return;
        case 0x98:
        case 0xe5:
            debug("CHECK POWER MODE");
            drv->sector_count = drv->power;
            drv->bufp = drv->sector_size;
            drv->cmd = 0xe5;
            return;
        case 0x99:
        case 0xe6:
            debug("SLEEP");
            drv->bufp = drv->sector_size;
            drv->cmd = 0xe6;
            ata_change_power_mode(drv, 0x00);
            return;
        case 0xe8:
            debug("WRITE BUFFER");
            drv->sector_count_internal = 1;
            drv->bufp = 0;
            drv->cmd = 0xe8;
            return;
        case 0xec:
            {
                BYTE result[512];
                memset(result, 0, sizeof(result));
                int size;
                debug("IDENTIFY DEVICE");
                putw(0, (drv->type == ATA_DRIVE_HDD) ? 0x0040 : 0x848a);
                putw(1, drv->default_cylinders);
                putw(3, drv->default_heads);
                if (drv->type != ATA_DRIVE_CF) {
                    putw(4, drv->sector_size * drv->default_sectors);
                    putw(5, drv->sector_size);
                }
                putw(6, drv->default_sectors);
                if (drv->type == ATA_DRIVE_CF) {
                    putw(7, drv->size >> 16);
                    putw(8, drv->size & 0xffff);
                }
                ident_update_string(result + 20, ATA_SERIAL_NUMBER, 20);
                putw(21, BUFSIZ / drv->sector_size);
                ident_update_string(result + 46, ATA_REVISION, 8);
                if (drv->type == ATA_DRIVE_HDD) {
                    ident_update_string(result + 54, "ATA-HDD " ATA_COPYRIGHT, 40);
                } else {
                    ident_update_string(result + 54, "ATA-CFA " ATA_COPYRIGHT, 40);
                }
                setb(49, 13, 1); /* standard timers */
                setb(49, 9, drv->lba); /* LBA support */
                setb(53, 0, drv->sectors > 0);
                putw(54, drv->cylinders);
                putw(55, drv->heads);
                putw(56, drv->sectors);
                size = drv->cylinders * drv->heads * drv->sectors;
                if (size > drv->size) size = drv->size;
                putw(57, size & 0xffff);
                putw(58, size >> 16);
                if (drv->lba) {
                    putw(60, drv->size & 0xffff);
                    putw(61, drv->size >> 16);
                }
                setb(82, 3, 1); /* pm command set */
                setb(82, 4, drv->atapi); /* packet */
                setb(82, 5, 1); /* write cache */
                setb(82, 6, 1); /* look-ahead */
                setb(82, 12, 1); /* write buffer */
                setb(82, 13, 1); /* read buffer */
                setb(83, 14, 1);
                setb(84, 14, 1);
                setb(85, 3, 1); /* pm command set */
                setb(85, 4, drv->atapi); /* packet */
                setb(85, 5, drv->wcache);
                setb(85, 6, drv->lookahead);
                setb(85, 12, 1); /* write buffer */
                setb(85, 13, 1); /* read buffer */
                setb(87, 14, 1);

                drv->sector_count_internal = 1;
                memcpy(drv->buffer + drv->sector_size - sizeof(result), result, sizeof(result));
                drv->bufp = drv->sector_size - sizeof(result);
                drv->cmd = 0xec;
            }
            return;
        case 0xef:
            switch (drv->features) {
            case 0x02:
                debug("SET ENABLE WRITE CACHE");
                drv->wcache = 1;
                return;
            case 0x03:
                debug("SET TRANSFER MODE %02x", drv->sector_count);
                if (drv->sector_count > 1 && drv->sector_count != 8) {
                    drv->error = ATA_ABRT;
                }
                return;
            case 0x33:
                debug("SET DISABLE RETRY");
                return;
            case 0x55:
                debug("SET DISABLE LOOK-AHEAD");
                drv->lookahead = 0;
                return;
            case 0x82:
                debug("SET DISABLE WRITE CACHE");
                drv->wcache = 0;
                if (drv->file) {
                    fflush(drv->file);
                }
                return;
            case 0x99:
                debug("SET ENABLE RETRY");
                return;
            case 0xaa:
                debug("SET ENABLE LOOK-AHEAD");
                drv->lookahead = 1;
                return;
            default:
                debug("SET FEATURES %02x", drv->features);
                drv->error = ATA_ABRT;
            }
            return;
    }
    debug("COMMAND %02x", value & 0xff);
    drv->error = ATA_ABRT;
    return;
}

static void atapi_execute_command(struct ata_drive_t *drv, BYTE value) {
    if (((drv->head >> 4) & 1) != drv->slave && value != 0x90) {
        return;
    }
    if (drv->cmd == 0xe6 && value != 0x08) {
        return;
    }
    drv->bufp = drv->sector_size;
    drv->error = 0;
    drv->cmd = 0x00;
    switch (value) {
        case 0x00:
            ata_execute_command(drv, value);
            return;
        case 0x08:
            debug("DEVICE RESET");
            drive_diag(drv);
            return;
        case 0x90:
            ata_execute_command(drv, value);
            return;
        case 0xa0:
            drv->sector_count_internal = 1;
            drv->bufp = drv->sector_size - sizeof(drv->packet);
            drv->cmd = 0xa0;
            return;
        case 0xa1:
            {
                BYTE result[512];
                memset(result, 0, sizeof(result));
                debug("IDENTIFY PACKET DEVICE");
                putw(0, (drv->type == ATA_DRIVE_FDD) ? 0x8180: 0x8580);
                ident_update_string(result + 20, ATA_SERIAL_NUMBER, 20);
                putw(21, BUFSIZ / drv->sector_size);
                ident_update_string(result + 46, ATA_REVISION, 8);
                if (drv->type == ATA_DRIVE_FDD) {
                    ident_update_string(result + 54, "ATA-FDD " ATA_COPYRIGHT, 40);
                } else {
                    ident_update_string(result + 54, "ATA-DVD " ATA_COPYRIGHT, 40);
                }
                setb(49, 9, drv->lba); /* LBA support */
                setb(82, 3, 1); /* pm command set */
                setb(82, 4, drv->atapi); /* packet */
                setb(82, 5, 1); /* write cache */
                setb(82, 6, 1); /* look-ahead */
                setb(82, 9, 1); /* device reset */
                setb(83, 14, 1);
                setb(84, 14, 1);
                setb(85, 3, 1); /* pm command set */
                setb(85, 4, drv->atapi); /* packet */
                setb(85, 5, drv->wcache);
                setb(85, 6, drv->lookahead);
                setb(87, 14, 1);

                drv->sector_count_internal = 1;
                memcpy(drv->buffer + drv->sector_size - sizeof(result), result, sizeof(result));
                drv->bufp = drv->sector_size - sizeof(result);
                drv->cmd = 0xa1;
            }
            return;
        case 0xe0:
        case 0xe1:
        case 0xe2:
        case 0xe3:
        case 0xe5:
        case 0xe6:
        case 0xef:
            ata_execute_command(drv, value);
            return;
    }
    debug("COMMAND %02x", value & 0xff);
    drv->error = ATA_ABRT;
    return;
}

static void atapi_packet_execute_command(struct ata_drive_t *drv) {
    drv->bufp = drv->sector_size;
    drv->error = 0x00;
    drv->cmd = 0x00;
    if (drv->attention) {
        drv->attention = 0;
        drv->error = 0x64;
        return;
    }
    switch (drv->packet[0]) {
    case 0x00:
        debug("TEST UNIT READY");
        return;
    case 0x1b:
        debug("START STOP %d", drv->packet[4] & 3);
        return;
    case 0x23:
        {
            BYTE result[12];
            int len;
            memset(result, 0, sizeof(result));
            debug("READ FORMAT CAPACITIES");
            result[3]=8;
            result[4]=drv->size >> 24;
            result[5]=drv->size >> 16;
            result[6]=drv->size >> 8;
            result[7]=drv->size;
            result[8]=drv->file ? 2 : 3;
            result[10]=drv->sector_size >> 8;
            result[11]=drv->sector_size;

            len = (drv->packet[8] < sizeof(result) && !drv->packet[7]) ? ((drv->packet[8] + 1) & 0xfe) : sizeof(result);
            drv->sector_count_internal = 1;
            memcpy(drv->buffer + drv->sector_size - len, result, len);
            if (len) {
                drv->bufp = drv->sector_size - len;
                drv->cmd = 0x23;
            }
        }
        return;
    case 0x28:
        debug("READ 10 (%d)*%d", (drv->packet[2] << 24) | (drv->packet[3] << 16) | (drv->packet[4] << 8) | drv->packet[5], drv->packet[8]);
        drv->sector_count_internal = drv->packet[8];
        if (seek_sector(drv)) {
            return;
        }
        drv->cmd = 0x28;
        read_sector(drv);
        return;
    case 0x2a:
        debug("WRITE 10 (%d)*%d", (drv->packet[2] << 24) | (drv->packet[3] << 16) | (drv->packet[4] << 8) | drv->packet[5], drv->packet[8]);
        drv->sector_count_internal = drv->packet[8];
        if (seek_sector(drv)) {
            return;
        }
        if (drv->readonly) {
            drv->error = 0x54;
            return;
        }
        drv->bufp = 0;
        drv->cmd = 0x2a;
        return;
    case 0xbb:
        debug("SET CD SPEED %d/%d", drv->packet[2] | (drv->packet[3] << 8), drv->packet[4] | (drv->packet[5] << 8));
        if (drv->type != ATA_DRIVE_CD) {
            drv->error = 0xB4;
        }
        return;
    }
    debug("PACKET COMMAND %02x", drv->packet[0]);
    drv->error = 0xB4;
    return;
}

WORD ata_register_read(struct ata_drive_t *drv, BYTE addr)
{
    WORD res;

    if (drv->type == ATA_DRIVE_NONE) {
        return 0;
    }
    if (((drv->head >> 4) & 1) != drv->slave) {
        return 0;
    }
    if (drv->cmd == 0xe6) {
        return 0;
    }
    switch (addr) {
    case 0:
        switch (drv->cmd) {
        case 0x20:
        case 0x23:
        case 0x28:
        case 0xec:
        case 0xe4:
        case 0xa1:
            res = drv->buffer[drv->bufp] | (drv->buffer[drv->bufp | 1] << 8);
            drv->bufp += 2;
            if (drv->bufp >= drv->sector_size) {
                drv->sector_count_internal--;
                if (!drv->sector_count_internal) {
                    drv->cmd = 0x00;
                } 
                switch (drv->cmd) {
                case 0x20:
                case 0x28:
                    read_sector(drv);
                    break;
                default:
                    drv->bufp = drv->sector_size;
                    drv->cmd = 0x00;
                    break;
                }
            }
            return res;
        }
        return 0;
    case 1:
        return (WORD)drv->error;
    case 2:
        if (drv->atapi) {
            switch (drv->cmd) {
            case 0xa0:
            case 0x08:
                return 0x01;
            case 0xa1:
            case 0x28:
            case 0x23:
                return 0x02;
            case 0x2a:
                return 0x00;
            case 0xe5:
                break;
            default:
                return 0x03;
            }
        }
        return (WORD)drv->sector_count;
    case 3:
        return (WORD)drv->sector;
    case 4:
        return (WORD)drv->cylinder & 0xff;
    case 5:
        return (WORD)drv->cylinder >> 8;
    case 6:
        return (WORD)drv->head | 0xa0;
    case 7:
    case 14:
        return (drv->busy ? 0x80 : 0) | ((drv->atapi && drv->cmd == 0x08) ? 0: ATA_DRDY) | ((drv->bufp < drv->sector_size) ? ATA_DRQ : 0) | ((drv->error & 0xfe) ? ATA_ERR : 0);
    case 15:
        return (WORD)((drv->slave ? 0xc1 : 0xc2) | (((drv->head ^ 15) & 15) << 2));
    default:
        return 0;
    }
}

WORD ata_register_peek(struct ata_drive_t *drv, BYTE addr)
{
    if (addr == 0) {
        return 0;
    }
    return ata_register_read(drv, addr);
}


void ata_register_store(struct ata_drive_t *drv, BYTE addr, WORD value)
{
    if (drv->type == ATA_DRIVE_NONE) {
        return;
    }
    switch (addr) {
        case 0:
            switch (drv->cmd) {
            case 0x30:
            case 0x2a:
            case 0xe8:
            case 0xa0:
                drv->buffer[drv->bufp] = value & 0xff;
                drv->buffer[drv->bufp | 1] = value >> 8;
                drv->bufp += 2;
                if (drv->bufp >= drv->sector_size) {
                    switch (drv->cmd) {
                    case 0x2a:
                    case 0x30:
                        if (write_sector(drv)) {
                            return;
                        }
                        if (--drv->sector_count_internal) {
                            drv->bufp = 0;
                            return;
                        }
                        if (!drv->file || fflush(drv->file)) {
                            drv->error = drv->atapi ? 0x54 : (ATA_UNC | ATA_ABRT);
                            break;
                        }
                        break;
                    case 0xe8:
                        break;
                    case 0xa0:
                        memcpy(drv->packet, drv->buffer + drv->sector_size - sizeof(drv->packet), sizeof(drv->packet));
                        atapi_packet_execute_command(drv);
                        return;
                    }
                    drv->bufp = drv->sector_size;
                    drv->cmd = 0x00;
                }
                break;
            }
            return;
        case 1:
            drv->features = (BYTE)value;
            return;
        case 2:
            drv->sector_count =  (BYTE)value;
            return;
        case 3:
            drv->sector = (BYTE)value;
            return;
        case 4:
            drv->cylinder = (drv->cylinder & 0xff00) | (value & 0xff);
            return;
        case 5:
            drv->cylinder = (drv->cylinder & 0xff) | (value << 8);
            return;
        case 6:
            drv->head = (BYTE)value;
            return;
        case 7:
            if (drv->atapi) {
                atapi_execute_command(drv, (BYTE)value);
            } else {
                ata_execute_command(drv, (BYTE)value);
            }
            return;
        case 14:
            if ((drv->control & 0x04) && ((value ^ 0x04) & 0x04)) {
                ata_reset(drv);
                debug("SOFTWARE RESET");
            }
            drv->control = (BYTE)value;
            return;
    }
    return;
}

void ata_image_attach(struct ata_drive_t *drv)
{
    int typechange = 0;

    if (drv->file != NULL) {
        fclose(drv->file);
        drv->file = NULL;
    }

    if (drv->type != drv->settings_type) {
        typechange = 1;
        drv->type = drv->settings_type;
        switch (drv->type) {
        case ATA_DRIVE_FDD:
            drv->atapi = 1;
            drv->lba = 1;
            drv->sector_size = 512;
            drv->readonly = 0;
            drv->seek_time = (CLOCK)90000;
            drv->spinup_time = (CLOCK)8000000;
            drv->spindown_time = (CLOCK)5000000;
            break;
        case ATA_DRIVE_CD:
            drv->atapi = 1;
            drv->lba = 1;
            drv->sector_size = 2048;
            drv->readonly = 1;
            drv->seek_time = (CLOCK)190000;
            drv->spinup_time = (CLOCK)2800000;
            drv->spindown_time = (CLOCK)2000000;
            break;
        case ATA_DRIVE_HDD:
            drv->atapi = 0;
            drv->lba = 1;
            drv->sector_size = 512;
            drv->readonly = 0;
            drv->seek_time = (CLOCK)16000;
            drv->spinup_time = (CLOCK)3000000;
            drv->spindown_time = (CLOCK)2000000;
            break;
        case ATA_DRIVE_CF:
            drv->atapi = 0;
            drv->lba = 1;
            drv->sector_size = 512;
            drv->readonly = 0;
            drv->seek_time = (CLOCK)10;
            drv->spinup_time = (CLOCK)300000;
            drv->spindown_time = (CLOCK)2000;
            break;
        default:
            drv->atapi = 0;
            drv->lba = 1;
            drv->sector_size = 512;
            drv->readonly = 1;
            drv->seek_time = (CLOCK)0;
            drv->spinup_time = (CLOCK)0;
            drv->spindown_time = (CLOCK)0;
            drv->type = ATA_DRIVE_NONE;
            break;
        }
    }

    if (drv->type != ATA_DRIVE_NONE && drv->filename && drv->filename[0]) {
        if (!drv->readonly) {
            drv->file = fopen(drv->filename, MODE_READ_WRITE);
        }
        if (!drv->file) {
            drv->readonly = 1;
            drv->file = fopen(drv->filename, MODE_READ);
        }
    }

    if ((drv->settings_autodetect_size || drv->atapi) && drv->file) {
        drv->default_cylinders = drv->auto_cylinders;
        drv->default_heads = drv->auto_heads;
        drv->default_sectors = drv->auto_sectors;
        drv->size = drv->auto_size;

        if (drv->size < 1) {
            off_t size = 0;
            if (fseek(drv->file, 0, SEEK_END) == 0) {
                size = ftell(drv->file);
                if (size < 0) size = 0;
            }
            drv->default_cylinders = 0;
            drv->default_heads = 0;
            drv->default_sectors = 0;
            drv->size = size / drv->sector_size;
        }
    } else {
        drv->default_cylinders = drv->settings_cylinders;
        drv->default_heads = drv->settings_heads;
        drv->default_sectors = drv->settings_sectors;
        drv->size = drv->settings_sectors * drv->settings_heads * drv->settings_cylinders;
    }

    if (drv->size < 1) {
        drv->default_cylinders = identify[2] | (identify[3] << 8);
        drv->default_heads = identify[6];
        drv->default_sectors = identify[12];
        drv->size = drv->default_cylinders * drv->default_heads * drv->default_sectors;
        if ((identify[99] & 0x02) && (identify[120] || identify[121] || identify[122] || identify[123])) {
            drv->size = identify[120];
            drv->size |= identify[121] << 8;
            drv->size |= identify[122] << 16;
            drv->size |= identify[123] << 24;
            drv->lba = 1;
        } else {
            drv->lba = 0;
        }
        log_warning(drv->log, "Image size invalid, using default %d MiB.", drv->size / (1048576 / drv->sector_size));
    }

    while (drv->default_sectors < 1 || drv->default_sectors > 63 || drv->default_cylinders > 65535 ||
            (drv->default_sectors * drv->default_heads * drv->default_cylinders) > 16514064) {
        int size = drv->size;
        int i, c, h, s;

        if (drv->atapi) break;

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
        drv->default_cylinders = c;
        drv->default_heads = h;
        drv->default_sectors = s;
        break;
    }

    if (drv->file) {
        if (drv->atapi) {
            log_message(drv->log, "Attached `%s' %u sectors total.", drv->filename, drv->size);
        } else {
            log_message(drv->log, "Attached `%s' %i/%i/%i CHS geometry, %u sectors total.", drv->filename, drv->default_cylinders, drv->default_heads, drv->default_sectors, drv->size);
        }
    } else {
        if (drv->filename && drv->filename[0]) {
            log_warning(drv->log, "Cannot use image file `%s', drive disabled.", drv->filename);
        }
    }

    if (!drv->atapi || typechange) {
        ata_poweron(drv); /* update actual geometry */
    } else {
        drv->attention = 1; /* disk change only */
    }
    return;
}

void ata_image_detach(struct ata_drive_t *drv)
{
    if (drv->file != NULL) {
        fclose(drv->file);
        drv->file = NULL;
        log_message(drv->log, "Detached.");
    }
    return;
}

void ata_image_change(struct ata_drive_t *drv)
{
    if (drv->type != drv->settings_type || !drv->atapi) {
        drv->update_needed = 1;
    } else {
        ata_image_attach(drv);
    }
}

/* ---------------------------------------------------------------------*/
/*    snapshot support functions                                             */

/* Please note that after loading a snapshot the the image is in readonly
 * mode to prevent any filesystem corruption. This could be solved later by
 * checksumming the image (this might be slow!) and comparing it to the stored
 * checksum to check if there was any modification meanwhile.
 */

#define CART_DUMP_VER_MAJOR   0
#define CART_DUMP_VER_MINOR   5

int ata_snapshot_write_module(struct ata_drive_t *drv, snapshot_t *s)
{
    snapshot_module_t *m;
    DWORD bsy_clk = CLOCK_MAX;

    m = snapshot_module_create(s, drv->myname,
                          CART_DUMP_VER_MAJOR, CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (drv->busy) {
        bsy_clk = drv->bsy_alarm->context->pending_alarms[drv->bsy_alarm->pending_idx].clk;
    }

    SMW_STR(m, drv->filename);
    SMW_DW(m, drv->type);
    SMW_B(m, drv->error);
    SMW_B(m, drv->features);
    SMW_B(m, drv->sector_count);
    SMW_B(m, drv->sector_count_internal);
    SMW_B(m, drv->sector);
    SMW_W(m, drv->cylinder);
    SMW_B(m, drv->head);
    SMW_B(m, drv->control);
    SMW_B(m, drv->cmd);
    SMW_B(m, drv->power);
    SMW_BA(m, drv->packet, sizeof(drv->packet));
    SMW_DW(m, drv->bufp);
    SMW_BA(m, drv->buffer, drv->sector_size);
    SMW_DW(m, drv->cylinders);
    SMW_DW(m, drv->heads);
    SMW_DW(m, drv->sectors);
    SMW_DW(m, drv->default_cylinders);
    SMW_DW(m, drv->default_heads);
    SMW_DW(m, drv->default_sectors);
    SMW_DW(m, drv->size);
    SMW_DW(m, drv->pos);
    SMW_B(m, drv->wcache);
    SMW_B(m, drv->lookahead);
    SMW_B(m, drv->busy);
    SMW_DW(m, bsy_clk);

    return snapshot_module_close(m);
}

int ata_snapshot_read_module(struct ata_drive_t *drv, snapshot_t *s)
{
    BYTE vmajor, vminor;
    snapshot_module_t *m;
    char *filename = NULL;
    DWORD bsy_clk;

    m = snapshot_module_open(s, drv->myname, &vmajor, &vminor);
    if (m == NULL) {
        return -1;
    }

    if ((vmajor != CART_DUMP_VER_MAJOR) || (vminor != CART_DUMP_VER_MINOR)) {
        snapshot_module_close(m);
        return -1;
    }

    SMR_STR(m, &filename);
    if (strcmp(filename, drv->filename)) {
        lib_free(filename);
        snapshot_module_close(m);
        return -1;
    }
    lib_free(filename);
    SMR_DW_INT(m, &drv->type);
    if (drv->type != ATA_DRIVE_HDD && drv->type != ATA_DRIVE_FDD && drv->type != ATA_DRIVE_CD) {
        drv->type = ATA_DRIVE_NONE;
    }
    ata_image_attach(drv);
    SMR_B(m, &drv->error);
    SMR_B(m, &drv->features);
    SMR_B(m, &drv->sector_count);
    SMR_B(m, &drv->sector_count_internal);
    SMR_B(m, &drv->sector);
    SMR_W(m, &drv->cylinder);
    SMR_B(m, &drv->head);
    SMR_B(m, &drv->control);
    SMR_B(m, &drv->cmd);
    SMR_B(m, &drv->power);
    SMR_BA(m, drv->packet, sizeof(drv->packet));
    if (drv->power != 0 && drv->power != 0x80) drv->power = 0xff;
    SMR_DW_INT(m, &drv->bufp);
    if (drv->bufp < 0 || drv->bufp > drv->sector_size) drv->bufp = drv->sector_size;
    SMR_BA(m, drv->buffer, drv->sector_size);
    SMR_DW_INT(m, &drv->cylinders);
    if (drv->cylinders < 1 || drv->cylinders > 65535) drv->cylinders = 1;
    SMR_DW_INT(m, &drv->heads);
    if (drv->heads < 1 || drv->heads > 16) drv->heads = 1;
    SMR_DW_INT(m, &drv->sectors);
    if (drv->sectors < 1 || drv->sectors > 16) drv->sectors = 1;
    SMR_DW_INT(m, &drv->default_cylinders);
    if (drv->default_cylinders < 1 || drv->default_cylinders > 16) drv->default_cylinders = 1;
    SMR_DW_INT(m, &drv->default_heads);
    if (drv->default_heads < 1 || drv->default_heads > 16) drv->default_heads = 1;
    SMR_DW_INT(m, &drv->default_sectors);
    if (drv->default_sectors < 1 || drv->default_sectors > 16) drv->default_sectors = 1;
    SMR_DW_INT(m, &drv->size);
    if (drv->size < 1 || drv->size > 268435455) drv->size = 1;
    SMR_DW_INT(m, &drv->pos);
    if (drv->pos < 0 || drv->pos > 268435455) drv->pos = 0;
    SMR_B_INT(m, &drv->wcache);
    if (drv->wcache) drv->wcache = 1;
    SMR_B_INT(m, &drv->lookahead);
    if (drv->lookahead) drv->lookahead = 1;
    SMR_B_INT(m, &drv->busy);
    SMR_DW(m, &bsy_clk);
    if (drv->busy) {
        drv->busy = 1;
        alarm_set(drv->bsy_alarm, bsy_clk);
    } else {
        alarm_unset(drv->bsy_alarm);
    }

    if (drv->file) {
        fseek(drv->file, (off_t)drv->pos * drv->sector_size, SEEK_SET);
    }
    if (!drv->atapi) { /* atapi supports disc change events */
        drv->readonly = 1; /* make sure for ata that there's no filesystem corruption */
        drv->update_needed = 1; /* temporary settings only */
    }

    return snapshot_module_close(m);
}
