/*
 * ata.c - ATA device emulation
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

#define ATA_UNC  0x40
#define ATA_IDNF 0x10
#define ATA_ABRT 0x04
#define ATA_DRDY 0x40
#define ATA_DRQ 0x08
#define ATA_ERR 0x01
#define ATA_LBA 0x40

static const BYTE hdd_identify[128] = {
    0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x30, 0x32, 0x31, 0x31,
    0x38, 0x30, 0x30, 0x32, 0x20, 0x20, 0x20, 0x20,

    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x35,
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

static void update_string(BYTE *b, char *s, int n) {
    int i;
    for (i=0;i<n;i+=2) {
        b[i | 1] = *s ? *s++ : 0x20;
        b[i] = *s ? *s++ : 0x20;
    }
}

static void drive_diag(struct ata_drive_t *drv)
{
    drv->error = 1;
    drv->sector_count = 1;
    drv->sector = 1;
    drv->cylinder = 0;
    drv->head = 0;
    drv->bufp = 512;
    drv->cmd = 0x00;
}

static int seek_sector(struct ata_drive_t *drv)
{
    int lba;

    if (drv->head & ATA_LBA) {
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
    if (lba < 0 || fseek(drv->file, (off_t)lba << 9, SEEK_SET)) {
        drv->error = ATA_IDNF;
    } else {
        drv->error = 0;
    }
    drv->bufp = 512;
    drv->cmd = 0x00;
    return drv->error;
}

static void read_sector(struct ata_drive_t *drv)
{
    memset(drv->buffer, 0, 512);
    clearerr(drv->file);
    fread(drv->buffer, 1, 512, drv->file);
    if (ferror(drv->file)) {
        drv->error = ATA_UNC | ATA_ABRT;
        drv->bufp = 512;
        drv->cmd = 0x00;
    } else {
        drv->error = 0;
        drv->bufp = 0;
    }
}

void ata_reset(struct ata_drive_t *drv)
{
    drive_diag(drv);
    drv->sectors = drv->default_sectors;
    drv->heads = drv->default_heads;
    drv->cylinders = drv->default_cylinders;
}

void ata_init(struct ata_drive_t *drv, int drive)
{
    drv->myname = lib_msprintf("ATA%d", drive);
    drv->log = log_open(drv->myname);
    drv->file = NULL;
    drv->filename = NULL;
    drv->update_needed = 0;
}

void ata_shutdown(struct ata_drive_t *drv) {
    if (drv->filename) {
        lib_free(drv->filename);
        drv->filename = NULL;
    }
    log_close(drv->log);
    lib_free(drv->myname);
}

WORD ata_register_read(struct ata_drive_t *drv, BYTE addr)
{
    WORD res;

    if (!drv->file) {
        return 0;
    }
    if (((drv->head >> 4) & 1) != drv->slave) {
        return 0;
    }
    switch (addr) {
    case 0:
        switch (drv->cmd) {
            case 0x20:
            case 0xec:
            case 0xe4:
                res = drv->buffer[drv->bufp] | (drv->buffer[drv->bufp | 1] << 8);
                drv->bufp += 2;
                if (drv->bufp >= 512) {
                    drv->sector_count_internal--;
                    if (!drv->sector_count_internal) {
                        drv->cmd = 0x00;
                    } else {
                        read_sector(drv);
                    }
                }
                return res;
        }
        return 0;
    case 1:
        return (WORD)drv->error;
    case 2:
        return (WORD)drv->sector_count;
    case 3:
        return (WORD)drv->sector;
    case 4:
        return (WORD)drv->cylinder & 0xff;
    case 5:
        return (WORD)drv->cylinder >> 8;
    case 6:
        return (WORD)drv->head;
    case 7:
    case 14:
        return ATA_DRDY | ((drv->bufp < 512) ? ATA_DRQ : 0) | ((drv->error & 0xfe) ? ATA_ERR : 0);
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
    if (!drv->file) {
        return; /* if image file exists? */
    }
    switch (addr & 0xff) {
        case 0:
            switch (drv->cmd) {
                case 0x30:
                case 0xe8:
                    drv->buffer[drv->bufp] = value & 0xff;
                    drv->buffer[drv->bufp | 1] = value >> 8;
                    drv->bufp += 2;
                    if (drv->bufp >= 512) {
                        if (drv->cmd != 0xe8) {
                            if (fwrite(drv->buffer, 1, 512, drv->file) != 512) {
                                drv->error = ATA_UNC | ATA_ABRT;
                                drv->cmd = 0x00;
                                return;
                            }
                        }
                        drv->sector_count_internal--;
                        if (!drv->sector_count_internal) {
                            if (drv->cmd != 0xe8) {
                                if (fflush(drv->file)) {
                                    drv->error = ATA_UNC | ATA_ABRT;
                                    drv->cmd = 0x00;
                                    return;
                                }
                            }
                            drv->cmd = 0x00;
                        } else {
                            drv->error = 0;
                            drv->bufp = 0;
                        }
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
            if (((drv->head >> 4) & 1) != drv->slave && (value & 0xff)!= 0x90) {
                return;
            }
            switch (value & 0xff) {
                case 0x20:
                case 0x21:
#ifdef ATA_DEBUG
                    if (drv->head & ATA_LBA) {
                        log_message(drv->log, "READ SECTORS (%d)*%d", ((drv->head & 0xf) << 24) | (drv->cylinder << 8) |
                                  drv->sector, drv->sector_count);
                    } else {
                        log_message(drv->log, "READ SECTORS (%d/%d/%d)*%d", drv->cylinder, drv->head & 0xf, drv->sector, drv->sector_count);
                    }
#endif
                    drv->sector_count_internal = drv->sector_count;
                    if (seek_sector(drv)) {
                        return;
                    }
                    drv->cmd = 0x20;
                    read_sector(drv);
                    return;
                case 0x30:
                case 0x31:
#ifdef ATA_DEBUG
                    if (drv->head & ATA_LBA) {
                        log_message(drv->log, "WRITE SECTORS (%d)*%d", ((drv->head & 0xf) << 24) | (drv->cylinder << 8) |
                                  drv->sector, drv->sector_count);
                    } else {
                        log_message(drv->log, "WRITE SECTORS (%d/%d/%d)*%d", drv->cylinder, drv->head & 0xf, drv->sector, drv->sector_count);
                    }
#endif
                    drv->sector_count_internal = drv->sector_count;
                    if (seek_sector(drv)) {
                        return;
                    }
                    if (drv->readonly) {
                        drv->error = ATA_ABRT;
                        drv->bufp = 512;
                        drv->cmd = 0x00;
                        return;
                    }
                    drv->bufp = 0;
                    drv->cmd = 0x30;
                    return;
                case 0x90:
#ifdef ATA_DEBUG
                    log_message(drv->log, "EXECUTE DEVICE DIAGNOSTIC");
#endif
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
#ifdef ATA_DEBUG
                    log_message(drv->log, "INITIALIZE DEVICE PARAMETERS (%d/%d/%d)", drv->cylinders, drv->heads, drv->sectors);
#endif
                    if (drv->cylinders == 0) {
                        drv->heads = 0;
                        drv->sectors = 0;
                        drv->error = ATA_ABRT;
                    } else {
                        drv->error = 0;
                    }
                    break;
                case 0xe4:
#ifdef ATA_DEBUG
                    log_message(drv->log, "READ BUFFER");
#endif
                    drv->sector_count_internal = 1;
                    drv->error = 0;
                    drv->bufp = 0;
                    drv->cmd = 0xe4;
                    return;
                case 0xe8:
#ifdef ATA_DEBUG
                    log_message(drv->log, "WRITE BUFFER");
#endif
                    drv->sector_count_internal = 1;
                    drv->error = 0;
                    drv->bufp = 0;
                    drv->cmd = 0xe8;
                    return;
                case 0xec:
                    {
                        int size;
#ifdef ATA_DEBUG
                        log_message(drv->log, "IDENTIFY DEVICE");
#endif
                        memset(drv->buffer, 0, 512);
                        memcpy(drv->buffer, hdd_identify, 128);
                        drv->buffer[2] = drv->default_cylinders & 255;
                        drv->buffer[3] = drv->default_cylinders >> 8;
                        drv->buffer[6] = drv->default_heads;
                        drv->buffer[12] = drv->default_sectors;
                        drv->buffer[106] = drv->sectors ? 1 : 0;
                        drv->buffer[108] = drv->cylinders & 255;
                        drv->buffer[109] = drv->cylinders >> 8;
                        drv->buffer[110] = drv->heads;
                        drv->buffer[112] = drv->sectors;
                        size = drv->cylinders * drv->heads * drv->sectors;
                        if (size > drv->size) size = drv->size;
                        drv->buffer[114] = size & 0xff;
                        drv->buffer[115] = (size >> 8) & 0xff;
                        drv->buffer[116] = (size >> 16) & 0xff;
                        drv->buffer[117] = (size >> 24) & 0xff;
                        drv->buffer[120] = drv->size & 0xff;
                        drv->buffer[121] = (drv->size >> 8) & 0xff;
                        drv->buffer[122] = (drv->size >> 16) & 0xff;
                        drv->buffer[123] = (drv->size >> 24) & 0xff;

                        drv->sector_count_internal = 1;
                        drv->error = 0;
                        drv->bufp = 0;
                        drv->cmd = 0xec;
                    }
                    return;
                default:
#ifdef ATA_DEBUG
                    switch (value & 0xff) {
                        case 0x00:
                            log_message(drv->log, "NOP");
                            break;
                        case 0x08:
                            log_message(drv->log, "DEVICE RESET");
                            break;
                        case 0x94:
                        case 0xe0:
                            log_message(drv->log, "STANDBY IMMEDIATE");
                            break;
                        case 0x97:
                        case 0xe3:
                            log_message(drv->log, "IDLE %02x", drv->sector_count);
                            break;
                        case 0xef:
                            log_message(drv->log, "SET FEATURES %02x", drv->features);
                            break;
                        case 0xa0:
                            log_message(drv->log, "PACKET");
                            break;
                        case 0x95:
                        case 0xe1:
                            log_message(drv->log, "IDLE IMMEDIATE");
                            break;
                        case 0xa1:
                            log_message(drv->log, "IDENTIFY PACKET DEVICE");
                            break;
                        default:
                            log_message(drv->log, "COMMAND %02x", value & 0xff);
                    }
#endif
                    drv->error = ATA_ABRT;
                    break;
            }
            drv->bufp = 512;
            drv->cmd = 0x00;
            return;
        case 14:
            if ((drv->control & 0x04) && ((value ^ 0x04) & 0x04)) {
                ata_reset(drv);
#ifdef ATA_DEBUG
                log_message(drv->log, "SOFTWARE RESET");
#endif
            }
            drv->control = (BYTE)value;
            return;
        default:
            return;
    }
}

void ata_image_attach(struct ata_drive_t *drv, int slave)
{
    int res;

    if (drv->file != NULL) {
        fclose(drv->file);
    }

    drv->file = NULL;
    drv->slave = slave;

    if (!drv->filename || !drv->filename[0]) {
        ata_reset(drv);
        return;
    }

    drv->readonly = 0;
    drv->file = fopen(drv->filename, MODE_READ_WRITE);

    if (!drv->file) {
        drv->file = fopen(drv->filename, MODE_APPEND);
    }

    if (!drv->file) {
        drv->readonly = 1;
        drv->file = fopen(drv->filename, MODE_READ);
    }

    drv->default_cylinders = drv->settings_cylinders;
    drv->default_heads = drv->settings_heads;
    drv->default_sectors = drv->settings_sectors;
    drv->size = drv->settings_sectors * drv->settings_heads * drv->settings_cylinders;
    if (drv->settings_autodetect_size && drv->file) {
        /* try to get drive geometry */
        unsigned char header[24];
        off_t size = 0;

        /* read header */
        res = (int)fread(header, 1, 24, drv->file);
        if (res < 24) {
            memset(&header, 0, sizeof(header));
        }
        /* check signature */

        for (;;) {

            if (memcmp(header,"C64-IDE V", 9) == 0) { /* old filesystem always CHS */
                drv->default_cylinders = ((header[0x10] << 8) | header[0x11]) + 1;
                drv->default_heads = (header[0x12] & 0x0f) + 1;
                drv->default_sectors = header[0x13];
                drv->size = drv->default_cylinders * drv->default_heads * drv->default_sectors;
                break;  /* OK */
            }

            if (memcmp(header + 8, "C64 CFS V", 9) == 0) {
                if (header[0x04] & ATA_LBA) { /* LBA */
                    drv->default_cylinders = 0;
                    drv->default_heads = 0;
                    drv->default_sectors = 0;
                    drv->size = ((header[0x04] & 0x0f) << 24) | (header[0x05] << 16) | (header[0x06] << 8) | header[0x07];
                } else { /* CHS */
                    drv->default_cylinders = ((header[0x05] << 8) | header[0x06]) + 1;
                    drv->default_heads = (header[0x04] & 0x0f) + 1;
                    drv->default_sectors = header[0x07];
                    drv->size = drv->default_cylinders * drv->default_heads * drv->default_sectors;
                }
                break;  /* OK */
            }

            log_warning(drv->log, "image signature not found, guessing size.");

            size = 0;
            if (fseek(drv->file, 0, SEEK_END) == 0) {
                size = ftell(drv->file);
                if (size < 0) size = 0;
            }
            drv->default_cylinders = 0;
            drv->default_heads = 0;
            drv->default_sectors = 0;
            drv->size = size >> 9;
            break;
        }
    }

    if (drv->size < 1) {
        drv->default_cylinders = hdd_identify[2] | (hdd_identify[3] << 8);
        drv->default_heads = hdd_identify[6];
        drv->default_sectors = hdd_identify[12];
        drv->size = drv->default_cylinders * drv->default_heads * drv->default_sectors;
        if ((hdd_identify[99] & 0x02) && (hdd_identify[120] || hdd_identify[121] || hdd_identify[122] || hdd_identify[123])) {
            drv->size = hdd_identify[120];
            drv->size |= hdd_identify[121] << 8;
            drv->size |= hdd_identify[122] << 16;
            drv->size |= hdd_identify[123] << 24;
        } 
        log_warning(drv->log, "Image size invalid, using default %d MiB.", drv->size >> 11);
    }

    if (drv->default_sectors < 1 || drv->default_sectors > 63 || drv->default_cylinders > 65535 || (drv->default_sectors * drv->default_heads * drv->default_cylinders) > 16514064) {
        int size = drv->size;
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
        drv->default_cylinders = c;
        drv->default_heads = h;
        drv->default_sectors = s;
    }

    if (drv->file) {
        log_message(drv->log, "Attached `%s' %i/%i/%i CHS geometry, %u sectors total.", drv->filename, drv->default_cylinders, drv->default_heads, drv->default_sectors, drv->size);
    } else {
        log_warning(drv->log, "Cannot use image file `%s', drive disabled.", drv->filename);
    }

    ata_reset(drv); /* update actual geometry */
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

/* ---------------------------------------------------------------------*/
/*    snapshot support functions                                             */

/* Please note that after loading a snapshot the the image is in readonly
 * mode to prevent any filesystem corruption. This could be solved later by
 * checksumming the image (this might be slow!) and comparing it to the stored
 * checksum to check if there was any modification meanwhile.
 */

#define CART_DUMP_VER_MAJOR   0
#define CART_DUMP_VER_MINOR   0

int ata_snapshot_write_module(struct ata_drive_t *drv, snapshot_t *s)
{
    snapshot_module_t *m;
    int pos = 0;

    m = snapshot_module_create(s, drv->myname,
                          CART_DUMP_VER_MAJOR, CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (drv->file) {
        fflush(drv->file);
        pos = ftell(drv->file) >> 9;
    }

    SMW_STR(m, drv->filename);
    SMW_B(m, drv->error);
    SMW_B(m, drv->features);
    SMW_B(m, drv->sector_count);
    SMW_B(m, drv->sector_count_internal);
    SMW_B(m, drv->sector);
    SMW_W(m, drv->cylinder);
    SMW_B(m, drv->head);
    SMW_B(m, drv->control);
    SMW_B(m, drv->cmd);
    SMW_DW(m, drv->bufp);
    SMW_BA(m, drv->buffer, 512);
    SMW_DW(m, drv->cylinders);
    SMW_DW(m, drv->heads);
    SMW_DW(m, drv->sectors);
    SMW_DW(m, drv->default_cylinders);
    SMW_DW(m, drv->default_heads);
    SMW_DW(m, drv->default_sectors);
    SMW_DW(m, drv->size);
    SMW_DW(m, pos);

    return snapshot_module_close(m);
}

int ata_snapshot_read_module(struct ata_drive_t *drv, snapshot_t *s)
{
    BYTE vmajor, vminor;
    snapshot_module_t *m;
    char *filename = NULL;
    int pos;

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
    ata_image_attach(drv, drv->slave);
    SMR_B(m, &drv->error);
    SMR_B(m, &drv->features);
    SMR_B(m, &drv->sector_count);
    SMR_B(m, &drv->sector_count_internal);
    SMR_B(m, &drv->sector);
    SMR_W(m, &drv->cylinder);
    SMR_B(m, &drv->head);
    SMR_B(m, &drv->control);
    SMR_B(m, &drv->cmd);
    SMR_DW_INT(m, &drv->bufp);
    if (drv->bufp < 0 || drv->bufp > 512) drv->bufp = 512;
    SMR_BA(m, drv->buffer, 512);
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
    SMR_DW_INT(m, &pos);
    if (pos < 0 || pos > 268435455) pos = 0;

    if (drv->file) {
        fseek(drv->file, (off_t)pos << 9, SEEK_SET);
    }
    drv->readonly = 1; /* make sure there's no filesystem corruption */
    drv->update_needed = 1; /* temporary settings only */

    return snapshot_module_close(m);
}
