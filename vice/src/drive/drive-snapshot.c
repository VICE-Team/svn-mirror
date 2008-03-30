/*
 * drive-snapshot.c - Hardware-level Commodore disk drive emulation,
 *                    snapshot module.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "archdep.h"
#include "attach.h"
#include "diskconstants.h"
#include "diskimage.h"
#include "drive-snapshot.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivemem.h"
#include "driverom.h"
#include "gcr.h"
#include "iecdrive.h"
#include "lib.h"
#include "log.h"
#include "machine-drive.h"
#include "resources.h"
#include "rotation.h"
#include "snapshot.h"
#include "types.h"
#include "vdrive-bam.h"
#include "vdrive-snapshot.h"
#include "zfile.h"


/* Logging.  */
static log_t drive_snapshot_log = LOG_ERR;

static int drive_snapshot_write_image_module(snapshot_t *s, unsigned int dnr);
static int drive_snapshot_write_gcrimage_module(snapshot_t *s,
                                                unsigned int dnr);
static int drive_snapshot_read_image_module(snapshot_t *s, unsigned int dnr);
static int drive_snapshot_read_gcrimage_module(snapshot_t *s, unsigned int dnr);
static int drive_snapshot_write_rom_module(snapshot_t *s, unsigned int dnr);
static int drive_snapshot_read_rom_module(snapshot_t *s, unsigned int dnr);

/*
This is the format of the DRIVE snapshot module.

Name                 Type   Size   Description

SyncFactor           DWORD  1      sync factor main cpu <-> drive cpu

Accum                DWORD  2
AttachClk            CLOCK  2      write protect handling on attach
BitsMoved            DWORD  2      number of bits moved since last access
ByteReady            BYTE   2      flag: Byte ready
ClockFrequency       BYTE   2      current clock frequency
CurrentHalfTrack     WORD   2      current half track of the r/w head
DetachClk            CLOCK  2      write protect handling on detach
DiskID1              BYTE   2      disk ID1
DiskID2              BYTE   2      disk ID2
ExtendImagePolicy    BYTE   2      Is extending the disk image allowed
FinishByte           BYTE   2      flag: Mode changed, finish byte
GCRHeadOffset        DWORD  2      offset from the begin of the track
GCRRead              BYTE   2      next value to read from disk
GCRWriteValue        BYTE   2      next value to write to disk
IdlingMethod         BYTE   2      What idle methode do we use
LastMode             BYTE   2      flag: Was the last mode read or write
ParallelCableEnabled BYTE   2      flag: Is the parallel cable enabed
ReadOnly             BYTE   2      flag: This disk is read only
RotationLastClk      CLOCK  2
RotationTablePtr     DWORD  2      pointer to the rotation table
                                   (offset to the rotation table is saved)
Type                 DWORD  2      drive type

*/

#define DRIVE_SNAP_MAJOR 1
#define DRIVE_SNAP_MINOR 0

int drive_snapshot_write_module(snapshot_t *s, int save_disks, int save_roms)
{
    int i;
    char snap_module_name[] = "DRIVE";
    snapshot_module_t *m;
    DWORD rotation_table_ptr[2];
    BYTE GCR_image[2];
    int drive_true_emulation;
    DWORD sync_factor;

    resources_get_value("DriveTrueEmulation", (void *)&drive_true_emulation);

    if (vdrive_snapshot_module_write(s, drive_true_emulation ? 10 : 8) < 0)
        return -1;

    if (!drive_true_emulation)
        return 0;

    /* Save changes to disk before taking a snapshot.  */
    drive_gcr_data_writeback(0);
    drive_gcr_data_writeback(1);

    rotation_table_get(rotation_table_ptr);

    for (i = 0; i < 2; i++) {
        GCR_image[i] = (drive[i].GCR_image_loaded == 0
                       || !save_disks) ? 0 : 1;
    }

    m = snapshot_module_create(s, snap_module_name, DRIVE_SNAP_MAJOR,
                               DRIVE_SNAP_MINOR);
    if (m == NULL)
        return -1;

    resources_get_value("MachineVideoStandard", (void *)&sync_factor);

    if (SMW_DW(m, (DWORD)sync_factor) < 0) {
        if (m != NULL)
            snapshot_module_close(m);
        return -1;
    }

    for (i = 0; i < 2; i++) {
        if (0
            || SMW_DW(m, (DWORD)drive[i].snap_accum) < 0
            || SMW_DW(m, (DWORD)drive[i].attach_clk) < 0
            || SMW_DW(m, (DWORD)drive[i].snap_bits_moved) < 0
            || SMW_B(m, (BYTE)drive[i].byte_ready_level) < 0
            || SMW_B(m, (BYTE)drive[i].clock_frequency) < 0
            || SMW_W(m, (WORD)drive[i].current_half_track) < 0
            || SMW_DW(m, (DWORD)drive[i].detach_clk) < 0
            || SMW_B(m, (BYTE)drive[i].diskID1) < 0
            || SMW_B(m, (BYTE)drive[i].diskID2) < 0
            || SMW_B(m, (BYTE)drive[i].extend_image_policy) < 0
            || SMW_B(m, (BYTE)drive[i].snap_finish_byte) < 0
            || SMW_DW(m, (DWORD)drive[i].GCR_head_offset) < 0
            || SMW_B(m, (BYTE)drive[i].GCR_read) < 0
            || SMW_B(m, (BYTE)drive[i].GCR_write_value) < 0
            || SMW_B(m, (BYTE)drive[i].idling_method) < 0
            || SMW_B(m, (BYTE)drive[i].snap_last_mode) < 0
            || SMW_B(m, (BYTE)drive[i].parallel_cable_enabled) < 0
            || SMW_B(m, (BYTE)drive[i].read_only) < 0
            || SMW_DW(m, (DWORD)drive[i].snap_rotation_last_clk) < 0
            || SMW_DW(m, (DWORD)(rotation_table_ptr[i])) < 0
            || SMW_DW(m, (DWORD)drive[i].type) < 0
        ) {
            if (m != NULL)
                snapshot_module_close(m);
            return -1;
          }
    }

    if (snapshot_module_close(m) < 0)
        return -1;

    for (i = 0; i < 2; i++) {
        if (drive[i].enable) {
            struct drive_context_s *ctxptr =
                (i == 0) ? &drive0_context : &drive1_context;

            if (drive_cpu_snapshot_write_module(ctxptr, s) < 0)
                return -1;
            if (machine_drive_snapshot_write(ctxptr, s) < 0)
                return -1;
        }
    }

    if (save_disks) {
        if (GCR_image[0] > 0) {
            if (drive_snapshot_write_gcrimage_module(s, 0) < 0)
                return -1;
        } else {
            if (drive_snapshot_write_image_module(s, 0) < 0)
                return -1;
        }
        if (GCR_image[1] > 0) {
            if (drive_snapshot_write_gcrimage_module(s, 1) < 0)
                return -1;
        } else {
            if (drive_snapshot_write_image_module(s, 1) < 0)
                return -1;
        }
    }
    if (save_roms && drive[0].enable)
        if (drive_snapshot_write_rom_module(s, 0) < 0)
            return -1;
    if (save_roms && drive[1].enable)
        if (drive_snapshot_write_rom_module(s, 1) < 0)
            return -1;
    return 0;
}

int drive_snapshot_read_module(snapshot_t *s)
{
    BYTE major_version, minor_version;
    int i;
    snapshot_module_t *m;
    char snap_module_name[] = "DRIVE";
    DWORD rotation_table_ptr[2];
    int drive_true_emulation;
    int sync_factor;

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);
    if (m == NULL) {
        /* If this module is not found true emulation is off.  */
        resources_set_value("DriveTrueEmulation", (resource_value_t)0);
        return 0;
    }

    if (major_version > DRIVE_SNAP_MAJOR || minor_version > DRIVE_SNAP_MINOR) {
        log_error(drive_snapshot_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  DRIVE_SNAP_MAJOR, DRIVE_SNAP_MINOR);
    }

    /* If this module exists true emulation is enabled.  */
    /* XXX drive_true_emulation = 1 */
    resources_set_value("DriveTrueEmulation", (resource_value_t)1);

    if (SMR_DW_INT(m, &sync_factor) < 0) {
        if (m != NULL)
            snapshot_module_close(m);
        return -1;
    }

    for (i = 0; i < 2; i++) {
        if (0
            || SMR_DW_UL(m, &drive[i].snap_accum) < 0
            || SMR_DW(m, &drive[i].attach_clk) < 0
            || SMR_DW_UL(m, &drive[i].snap_bits_moved) < 0
            || SMR_B_INT(m, (int*)&drive[i].byte_ready_level) < 0
            || SMR_B_INT(m, &drive[i].clock_frequency) < 0
            || SMR_W_INT(m, &drive[i].current_half_track) < 0
            || SMR_DW(m, &drive[i].detach_clk) < 0
            || SMR_B(m, &drive[i].diskID1) < 0
            || SMR_B(m, &drive[i].diskID2) < 0
            || SMR_B_INT(m, &drive[i].extend_image_policy) < 0
            || SMR_B_INT(m, &drive[i].snap_finish_byte) < 0
            || SMR_DW_INT(m, (int*)&drive[i].GCR_head_offset) < 0
            || SMR_B(m, &drive[i].GCR_read) < 0
            || SMR_B(m, &drive[i].GCR_write_value) < 0
            || SMR_B_INT(m, &drive[i].idling_method) < 0
            || SMR_B_INT(m, &drive[i].snap_last_mode) < 0
            || SMR_B_INT(m, &drive[i].parallel_cable_enabled) < 0
            || SMR_B_INT(m, &drive[i].read_only) < 0
            || SMR_DW(m, &drive[i].snap_rotation_last_clk) < 0
            || SMR_DW(m, &rotation_table_ptr[i]) < 0
            || SMR_DW_INT(m, (int*)&drive[i].type) < 0
        ) {
            if (m != NULL)
                snapshot_module_close(m);
            return -1;
        }
    }
    snapshot_module_close(m);
    m = NULL;

    rotation_table_set(rotation_table_ptr);

    for (i = 0; i < 2; i++) {
        drive[i].GCR_track_start_ptr = (drive[i].gcr->data
                           + ((drive[i].current_half_track / 2 - 1)
                              * NUM_MAX_BYTES_TRACK));
        if (drive[i].type != DRIVE_TYPE_1571) {
            if (drive[i].type == DRIVE_TYPE_1581) {
                rotation_init_table(1, i);
                resources_set_value("MachineVideoStandard",
                                    (resource_value_t)sync_factor);
            } else {
                drive[i].side = 0;
                rotation_init_table(0, i);
                resources_set_value("MachineVideoStandard",
                                    (resource_value_t)sync_factor);
            }
        }
    }

    switch (drive[0].type) {
      case DRIVE_TYPE_1541:
      case DRIVE_TYPE_1541II:
      case DRIVE_TYPE_1571:
      case DRIVE_TYPE_1581:
      case DRIVE_TYPE_2031:
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_2040:
      case DRIVE_TYPE_3040:
      case DRIVE_TYPE_4040:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        drive[0].enable = 1;
        machine_drive_rom_setup_image(0);
        drive_mem_init(&drive0_context, drive[0].type);
        resources_set_value("Drive8IdleMethod",
                            (resource_value_t)drive[0].idling_method);
        drive_rom_initialize_traps(0);
        drive_set_active_led_color(drive[0].type, 0);
        break;
      case DRIVE_TYPE_NONE:
        drive_disable(0);
        break;
      default:
        return -1;
    }

    switch (drive[1].type) {
      case DRIVE_TYPE_1541:
      case DRIVE_TYPE_1541II:
      case DRIVE_TYPE_1571:
      case DRIVE_TYPE_1581:
      case DRIVE_TYPE_2031:
      case DRIVE_TYPE_1001:
        /* drive 1 does not allow dual disk drive */
        drive[1].enable = 1;
        machine_drive_rom_setup_image(1);
        drive_mem_init(&drive1_context, drive[1].type);
        resources_set_value("Drive9IdleMethod",
                            (resource_value_t) drive[1].idling_method);
        drive_rom_initialize_traps(1);
        drive_set_active_led_color(drive[1].type, 1);
        break;
      case DRIVE_TYPE_NONE:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        drive_disable(1);
        break;
      default:
        return -1;
    }

    /* Clear parallel cable before undumping parallel port values.  */
    parallel_cable_drive0_write(0xff, 0);
    parallel_cable_drive1_write(0xff, 0);

    for (i = 0; i < 2; i++) {
        if (drive[i].enable) {
            struct drive_context_s *ctxptr =
                (i == 0) ? &drive0_context : &drive1_context;

            if (drive_cpu_snapshot_read_module(ctxptr, s) < 0)
                return -1;
            if (machine_drive_snapshot_read(ctxptr, s) < 0)
                return 1;
        }
    }

    if (drive_snapshot_read_image_module(s, 0) < 0
        || drive_snapshot_read_gcrimage_module(s, 0) < 0)
        return -1;
    if (drive_snapshot_read_image_module(s, 1) < 0
        || drive_snapshot_read_gcrimage_module(s, 1) < 0)
        return -1;
    if (drive_snapshot_read_rom_module(s, 0) < 0)
        return -1;
    if (drive_snapshot_read_rom_module(s, 1) < 0)
        return -1;

    if (drive[0].type != DRIVE_TYPE_NONE)
        drive_enable(0);
    if (drive[1].type != DRIVE_TYPE_NONE)
        drive_enable(1);

    iec_calculate_callback_index();
    iec_update_ports_embedded();
    drive_update_ui_status();

    resources_get_value("DriveTrueEmulation", (void *)&drive_true_emulation);

    if (vdrive_snapshot_module_read(s, drive_true_emulation ? 10 : 8) < 0)
        return -1;

    return 0;
}

/* -------------------------------------------------------------------- */
/* read/write "normal" disk image snapshot module */

#define IMAGE_SNAP_MAJOR 1
#define IMAGE_SNAP_MINOR 0

/*
 * This image format is pretty simple:
 *
 * WORD Type            Disk image type (1581, 8050, 8250)
 * 256 * blocks(disk image type) BYTE
 *                      disk image
 *
 */

static int drive_snapshot_write_image_module(snapshot_t *s, unsigned int dnr)
{
    char snap_module_name[10];
    snapshot_module_t *m;
    BYTE sector_data[0x100];
    WORD word;
    int track, sector;
    int rc;

    if (drive[dnr].image == NULL)
        return 0;

    sprintf(snap_module_name, "IMAGE%i", dnr);

    m = snapshot_module_create(s, snap_module_name, IMAGE_SNAP_MAJOR,
                               IMAGE_SNAP_MINOR);
    if (m == NULL)
       return -1;

    word = drive[dnr].image->type;
    SMW_W(m, word);

    /* we use the return code to step through the tracks. So we do not
       need any geometry info. */
    for (track = 1; ; track++) {
        rc = 0;
        for (sector = 0; ; sector++) {
            rc = disk_image_read_sector(drive[dnr].image, sector_data, track,
                                        sector);
            if (rc == 0) {
                SMW_BA(m, sector_data, 0x100);
            } else {
                break;
            }
        }
        if (sector == 0) {
            break;
        }
    }

    if (snapshot_module_close(m) < 0)
        return -1;
    return 0;
}

static int drive_snapshot_read_image_module(snapshot_t *s, unsigned int dnr)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;
    char snap_module_name[10];
    WORD word;
    char *filename;
    char request_str[100];
    int len = 0;
    FILE *fp;
    BYTE sector_data[0x100];
    int track, sector;
    int rc;

    sprintf(snap_module_name, "IMAGE%i", dnr);

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);
    if (m == NULL)
        return 0;

    if (major_version > IMAGE_SNAP_MAJOR || minor_version > IMAGE_SNAP_MINOR) {
        log_error(drive_snapshot_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  IMAGE_SNAP_MAJOR, IMAGE_SNAP_MINOR);
    }

    if (SMR_W(m, &word) < 0) {
        snapshot_module_close(m);
        return -1;
    }

    switch(word) {
      case 1581:
        len = D81_FILE_SIZE;
        break;
      case 8050:
        len = D80_FILE_SIZE;
        break;
      case 8250:
        len = D82_FILE_SIZE;
        break;
      default:
        log_error(drive_snapshot_log,
                  "Snapshot of disk image unknown (type %d)",
                  (int)word);
        snapshot_module_close(m);
        return -1;
    }

    /* create temporary file of the right size */
    filename = archdep_tmpnam();

    if (filename == NULL) {
        log_error(drive_snapshot_log, "Could not create temporary filename");
        snapshot_module_close(m);
        return -1;
    }

    fp = fopen(filename, MODE_WRITE);

    if (fp == NULL) {
        log_error(drive_snapshot_log, "Could not create temporary file");
        log_error(drive_snapshot_log, "filename=%s", filename);
        lib_free(filename);
        snapshot_module_close(m);
        return -1;
    }

    /* blow up the file to needed size */
    if (fseek(fp, len - 1, SEEK_SET) < 0
        || (fputc(0, fp) == EOF)) {
        log_error(drive_snapshot_log, "Could not create large temporary file");
        fclose(fp);
        lib_free(filename);
        snapshot_module_close(m);
        return -1;
    }

    fclose(fp);
    lib_free(filename);

    if (file_system_attach_disk(dnr + 8, filename) < 0) {
        log_error(drive_snapshot_log, "Invalid Disk Image");
        lib_free(filename);
        snapshot_module_close(m);
        return -1;
    }

    sprintf(request_str, "Disk image unit #%d imported from snapshot", dnr + 8);
    zfile_close_action(filename, ZFILE_REQUEST, request_str);

    /* we use the return code to step through the tracks. So we do not
       need any geometry info. */
    SMR_BA(m, sector_data, 0x100);
    for (track = 1; ; track++) {
        rc = 0;
        for (sector = 0; ; sector++) {
            rc = disk_image_write_sector(drive[dnr].image, sector_data, track,
                                         sector);
            if (rc == 0) {
                SMR_BA(m, sector_data, 0x100);
            } else {
                break;
            }
        }
        if (sector == 0) {
            break;
        }
    }

    vdrive_bam_reread_bam(dnr + 8);

    snapshot_module_close(m);
    m = NULL;

    return 0;
}

/* -------------------------------------------------------------------- */
/* read/write GCR disk image snapshot module */

#define GCRIMAGE_SNAP_MAJOR 1
#define GCRIMAGE_SNAP_MINOR 0

static int drive_snapshot_write_gcrimage_module(snapshot_t *s, unsigned int dnr)
{
    char snap_module_name[10];
    snapshot_module_t *m;
    BYTE *tmpbuf;
    int i;

    sprintf(snap_module_name, "GCRIMAGE%i", dnr);

    m = snapshot_module_create(s, snap_module_name, GCRIMAGE_SNAP_MAJOR,
                               GCRIMAGE_SNAP_MINOR);
    if (m == NULL)
        return -1;

    tmpbuf = (BYTE *)lib_malloc(MAX_TRACKS_1571 * 4);

    for (i = 0; i < MAX_TRACKS_1571; i++) {
        tmpbuf[i * 4] = drive[dnr].gcr->track_size[i] & 0xff;
        tmpbuf[i * 4 + 1] = (drive[dnr].gcr->track_size[i] >> 8) & 0xff;
        tmpbuf[i * 4 + 2] = (drive[dnr].gcr->track_size[i] >> 16) & 0xff;
        tmpbuf[i * 4 + 3] = (drive[dnr].gcr->track_size[i] >> 24) & 0xff;
    }

    if (0
        || SMW_BA(m, drive[dnr].gcr->data,
            sizeof(drive[dnr].gcr->data)) < 0
        || SMW_BA(m, drive[dnr].gcr->speed_zone,
            sizeof(drive[dnr].gcr->speed_zone)) < 0
        || SMW_BA(m, tmpbuf, MAX_TRACKS_1571 * 4) < 0) {
        if (m != NULL)
            snapshot_module_close(m);
        lib_free(tmpbuf);
        return -1;
    }

    lib_free(tmpbuf);

    if (snapshot_module_close(m) < 0)
        return -1;

    return 0;
}

static int drive_snapshot_read_gcrimage_module(snapshot_t *s, unsigned int dnr)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;
    char snap_module_name[10];
    BYTE *tmpbuf;
    int i;

    sprintf(snap_module_name, "GCRIMAGE%i", dnr);

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);
    if (m == NULL)
        return 0;

    if (major_version > GCRIMAGE_SNAP_MAJOR
        || minor_version > GCRIMAGE_SNAP_MINOR) {
        log_error(drive_snapshot_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  GCRIMAGE_SNAP_MAJOR, GCRIMAGE_SNAP_MINOR);
    }

    tmpbuf = (BYTE *)lib_malloc(MAX_TRACKS_1571 * 4);

    if (0
        || SMR_BA(m, drive[dnr].gcr->data,
            sizeof(drive[dnr].gcr->data)) < 0
        || SMR_BA(m, drive[dnr].gcr->speed_zone,
            sizeof(drive[dnr].gcr->speed_zone)) < 0
        || SMR_BA(m, tmpbuf, MAX_TRACKS_1571 * 4) < 0) {

        if (m != NULL)
            snapshot_module_close(m);
        lib_free(tmpbuf);
        return -1;
    }

    snapshot_module_close(m);
    m = NULL;

    for (i = 0; i < MAX_TRACKS_1571; i++)
        drive[dnr].gcr->track_size[i] = tmpbuf[i * 4] + (tmpbuf[i * 4 + 1] << 8)
            + (tmpbuf[i * 4 + 2] << 16) + (tmpbuf[i * 4 + 3] << 24);

    lib_free(tmpbuf);

    drive[dnr].GCR_image_loaded = 1;
    drive[dnr].image = NULL;

    return 0;
}

/* -------------------------------------------------------------------- */

#define ROM_SNAP_MAJOR 1
#define ROM_SNAP_MINOR 0

static int drive_snapshot_write_rom_module(snapshot_t *s, unsigned int dnr)
{
    char snap_module_name[10];
    snapshot_module_t *m;
    BYTE *base;
    int len;

    sprintf(snap_module_name, "DRIVEROM%i", dnr);

    m = snapshot_module_create(s, snap_module_name, ROM_SNAP_MAJOR,
                               ROM_SNAP_MINOR);
    if (m == NULL)
       return -1;

    switch (drive[dnr].type) {
      case DRIVE_TYPE_1541:
        base = &(drive[dnr].rom[0x4000]);
        len = DRIVE_ROM1541_SIZE;
        break;
      case DRIVE_TYPE_1541II:
        base = &(drive[dnr].rom[0x4000]);
        len = DRIVE_ROM1541II_SIZE;
        break;
      case DRIVE_TYPE_1571:
        base = drive[dnr].rom;
        len = DRIVE_ROM1571_SIZE;
        break;
      case DRIVE_TYPE_1581:
        base = drive[dnr].rom;
        len = DRIVE_ROM1581_SIZE;
        break;
      case DRIVE_TYPE_2031:
        base = &(drive[dnr].rom[0x4000]);
        len = DRIVE_ROM2031_SIZE;
        break;
      case DRIVE_TYPE_2040:
        base = &(drive[dnr].rom[DRIVE_ROM_SIZE - DRIVE_ROM2040_SIZE]);
        len = DRIVE_ROM2040_SIZE;
        break;
      case DRIVE_TYPE_3040:
        base = &(drive[dnr].rom[DRIVE_ROM_SIZE - DRIVE_ROM3040_SIZE]);
        len = DRIVE_ROM3040_SIZE;
        break;
      case DRIVE_TYPE_4040:
        base = &(drive[dnr].rom[DRIVE_ROM_SIZE - DRIVE_ROM4040_SIZE]);
        len = DRIVE_ROM4040_SIZE;
        break;
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        base = &(drive[dnr].rom[0x4000]);
        len = DRIVE_ROM1001_SIZE;
        break;
      default:
        return -1;
    }

    if (SMW_BA(m, base, len) < 0) {
        if (m != NULL)
            snapshot_module_close(m);
        return -1;
    }
    if (snapshot_module_close(m) < 0)
        return -1;
    return 0;
}

static int drive_snapshot_read_rom_module(snapshot_t *s, unsigned int dnr)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;
    char snap_module_name[10];
    BYTE *base;
    int len;

    sprintf(snap_module_name, "DRIVEROM%i", dnr);

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);
    if (m == NULL)
        return 0;

    if (major_version > ROM_SNAP_MAJOR || minor_version > ROM_SNAP_MINOR) {
        log_error(drive_snapshot_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  ROM_SNAP_MAJOR, ROM_SNAP_MINOR);
    }

    switch (drive[dnr].type) {
      case DRIVE_TYPE_1541:
        base = &(drive[dnr].rom[0x4000]);
        len = DRIVE_ROM1541_SIZE;
        break;
      case DRIVE_TYPE_1541II:
        base = &(drive[dnr].rom[0x4000]);
        len = DRIVE_ROM1541II_SIZE;
        break;
      case DRIVE_TYPE_1571:
        base = drive[dnr].rom;
        len = DRIVE_ROM1571_SIZE;
        break;
      case DRIVE_TYPE_1581:
        base = drive[dnr].rom;
        len = DRIVE_ROM1581_SIZE;
        break;
      case DRIVE_TYPE_2031:
        base = &(drive[dnr].rom[0x4000]);
        len = DRIVE_ROM2031_SIZE;
        break;
      case DRIVE_TYPE_2040:
        base = &(drive[dnr].rom[DRIVE_ROM_SIZE - DRIVE_ROM2040_SIZE]);
        len = DRIVE_ROM2040_SIZE;
        break;
      case DRIVE_TYPE_3040:
        base = &(drive[dnr].rom[DRIVE_ROM_SIZE - DRIVE_ROM3040_SIZE]);
        len = DRIVE_ROM3040_SIZE;
        break;
      case DRIVE_TYPE_4040:
        base = &(drive[dnr].rom[DRIVE_ROM_SIZE - DRIVE_ROM4040_SIZE]);
        len = DRIVE_ROM4040_SIZE;
        break;
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        base = &(drive[dnr].rom[0x4000]);
        len = DRIVE_ROM1001_SIZE;
        break;
      default:
        return -1;
    }

    if (SMR_BA(m, base, len) < 0) {
        if (m != NULL)
            snapshot_module_close(m);
        return -1;
    }

    machine_drive_rom_do_checksum(dnr);

    snapshot_module_close(m);

    return 0;
}

