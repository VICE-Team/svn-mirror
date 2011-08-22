/*
 * ata.h - ATA(PI) device emulation
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

#ifndef VICE_ATA
#define VICE_ATA

#include "types.h"
#include "log.h"
#include "alarm.h"

#define ATA_DRIVE_NONE 0
#define ATA_DRIVE_HDD 1
#define ATA_DRIVE_FDD 2
#define ATA_DRIVE_CD 3
#define ATA_DRIVE_CF 4

struct ata_drive_t {
    BYTE error;
    BYTE features;
    BYTE sector_count, sector_count_internal;
    BYTE sector;
    WORD cylinder;
    BYTE head;
    BYTE control;
    BYTE cmd;
    BYTE power;
    int bufp;
    BYTE *buffer;
    FILE *file;
    char *filename;
    char *myname;
    int default_cylinders, default_heads, default_sectors, size;
    int cylinders, heads, sectors;
    int settings_cylinders, settings_heads, settings_sectors;
    int auto_cylinders, auto_heads, auto_sectors, auto_size;
    int settings_autodetect_size, settings_type;
    int slave;
    int update_needed;
    int readonly;
    int attention;
    int wcache;
    int lookahead;
    int type;
    int busy;
    int pos;
    alarm_t *bsy_alarm;
    log_t log;
    int sector_size;
    int atapi;
    CLOCK seek_time;
    CLOCK spinup_time, spindown_time;
};

extern void ata_init(struct ata_drive_t *drv, int drive);
extern void ata_shutdown(struct ata_drive_t *drv);
extern void ata_register_store(struct ata_drive_t *cdrive, BYTE addr, WORD value);
extern WORD ata_register_read(struct ata_drive_t *cdrive, BYTE addr);
extern WORD ata_register_peek(struct ata_drive_t *cdrive, BYTE addr);
extern void ata_image_attach(struct ata_drive_t *cdrive);
extern void ata_image_detach(struct ata_drive_t *cdrive);
extern void ata_image_change(struct ata_drive_t *cdrive);
extern void ata_reset(struct ata_drive_t *cdrive);

struct snapshot_s;
extern int ata_snapshot_read_module(struct ata_drive_t *drv, struct snapshot_s *s);
extern int ata_snapshot_write_module(struct ata_drive_t *drv, struct snapshot_s *s);

#endif
