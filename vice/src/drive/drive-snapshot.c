/*
 * drive-snapshot.c - Hardware-level Commodore disk drive emulation,
 *                    snapshot module.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "ciad.h"
#include "drive.h"
#include "fdc.h"
#include "iecdrive.h"
#include "log.h"
#include "resources.h"
#include "riotd.h"
#include "snapshot.h"
#include "types.h"
#include "utils.h"
#include "vdrive-bam.h"
#include "vdrive.h"
#include "viad.h"
#include "wd1770.h"
#include "zfile.h"

/* Logging.  */
static log_t drive_snapshot_log = LOG_ERR;

static int drive_write_image_snapshot_module(snapshot_t *s, int dnr);
static int drive_write_gcrimage_snapshot_module(snapshot_t *s, int dnr);
static int drive_read_image_snapshot_module(snapshot_t *s, int dnr);
static int drive_read_gcrimage_snapshot_module(snapshot_t *s, int dnr);
static int drive_write_rom_snapshot_module(snapshot_t *s, int dnr);
static int drive_read_rom_snapshot_module(snapshot_t *s, int dnr);

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

int drive_write_snapshot_module(snapshot_t *s, int save_disks, int save_roms)
{
    int i;
    char snap_module_name[] = "DRIVE";
    snapshot_module_t *m;
    DWORD rotation_table_ptr[2];
    BYTE GCR_image[2];
    int drive_true_emulation;
    DWORD sync_factor;

    resources_get_value("DriveTrueEmulation",
                        (resource_value_t *)&drive_true_emulation);

    if (vdrive_write_snapshot_module(s, drive_true_emulation ? 10 : 8) < 0)
        return -1;

    if (!drive_true_emulation)
        return 0;

    /* Save changes to disk before taking a snapshot.  */
    drive_GCR_data_writeback(0);
    drive_GCR_data_writeback(1);

    for (i = 0; i < 2; i++) {
        rotation_table_ptr[i] = (DWORD) (drive[i].rotation_table_ptr
                                         - drive[i].rotation_table[0]);
        GCR_image[i] = (drive[i].GCR_image_loaded == 0
                         || !save_disks) ? 0 : 1;
    }

    m = snapshot_module_create(s, snap_module_name, DRIVE_SNAP_MAJOR,
                               DRIVE_SNAP_MINOR);
    if (m == NULL)
        return -1;

    resources_get_value("VideoStandard", (resource_value_t *)&sync_factor);

    if (snapshot_module_write_dword(m, (DWORD) sync_factor) < 0) {
        if (m != NULL)
            snapshot_module_close(m);
        return -1;
    }

    for (i = 0; i < 2; i++) {
        if (0
            || snapshot_module_write_dword(m, (DWORD) drive[i].accum) < 0
            || snapshot_module_write_dword(m, (DWORD) drive[i].attach_clk) < 0
            || snapshot_module_write_dword(m, (DWORD) drive[i].bits_moved) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].byte_ready) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].clock_frequency) < 0
            || snapshot_module_write_word(m, (WORD) drive[i].current_half_track) < 0
            || snapshot_module_write_dword(m, (DWORD) drive[i].detach_clk) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].diskID1) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].diskID2) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].extend_image_policy) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].finish_byte) < 0
            || snapshot_module_write_dword(m, (DWORD) drive[i].GCR_head_offset) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].GCR_read) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].GCR_write_value) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].idling_method) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].last_mode) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].parallel_cable_enabled) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].read_only) < 0
            || snapshot_module_write_dword(m, (DWORD) drive[i].rotation_last_clk) < 0
            || snapshot_module_write_dword(m, (DWORD) (rotation_table_ptr[i])) < 0
            || snapshot_module_write_dword(m, (DWORD) drive[i].type) < 0
        ) {
            if (m != NULL)
                snapshot_module_close(m);
            return -1;
          }
    }

    if (snapshot_module_close(m) < 0)
        return -1;

    if (drive[0].enable) {
        if (drive_cpu_write_snapshot_module(&drive0_context, s) < 0)
            return -1;
        if (drive[0].type == DRIVE_TYPE_1541
            || drive[0].type == DRIVE_TYPE_1541II
            || drive[0].type == DRIVE_TYPE_2031) {
            if (via1d_write_snapshot_module(&drive0_context, s) < 0
                || via2d_write_snapshot_module(&drive0_context, s) < 0)
                return -1;
        }
        if (drive[0].type == DRIVE_TYPE_1571) {
            if (via1d_write_snapshot_module(&drive0_context, s) < 0
                || via2d_write_snapshot_module(&drive0_context, s) < 0
                || cia1571d0_write_snapshot_module(s) < 0)
                return -1;
        }
        if (drive[0].type == DRIVE_TYPE_1581) {
            if (cia1581d0_write_snapshot_module(s) < 0)
                return -1;
        }
	if ((drive[0].type == DRIVE_TYPE_1001)
	    || (drive[0].type == DRIVE_TYPE_8050)
	    || (drive[0].type == DRIVE_TYPE_8250)
	    ) {
	    if (riot1d0_write_snapshot_module(s) < 0
		|| riot2d0_write_snapshot_module(s) < 0
		|| fdc_write_snapshot_module(s, 0) < 0)
		return -1;
	}
    }
    if (drive[1].enable) {
        if (drive_cpu_write_snapshot_module(&drive1_context, s) < 0)
            return -1;
        if (drive[1].type == DRIVE_TYPE_1541
            || drive[1].type == DRIVE_TYPE_1541II
            || drive[1].type == DRIVE_TYPE_2031) {
            if (via1d_write_snapshot_module(&drive1_context, s) < 0
                || via2d_write_snapshot_module(&drive1_context, s) < 0)
                return -1;
        }
        if (drive[1].type == DRIVE_TYPE_1571) {
            if (via1d_write_snapshot_module(&drive1_context, s) < 0
                || via2d_write_snapshot_module(&drive1_context, s) < 0
                || cia1571d1_write_snapshot_module(s) < 0)
                return -1;
        }
        if (drive[1].type == DRIVE_TYPE_1581) {
            if (cia1581d1_write_snapshot_module(s) < 0)
                return -1;
        }
	if ((drive[1].type == DRIVE_TYPE_1001)
	    || (drive[1].type == DRIVE_TYPE_8050)
	    || (drive[1].type == DRIVE_TYPE_8250)
	    ) {
	    if (riot1d1_write_snapshot_module(s) < 0
		|| riot2d1_write_snapshot_module(s) < 0
		|| fdc_write_snapshot_module(s, 1) < 0)
		return -1;
	}
    }

    if (save_disks) {
	if (GCR_image[0] > 0) {
            if (drive_write_gcrimage_snapshot_module(s, 0) < 0)
                return -1;
	} else {
            if (drive_write_image_snapshot_module(s, 0) < 0)
		return -1;
	}
        if (GCR_image[1] > 0) {
            if (drive_write_gcrimage_snapshot_module(s, 1) < 0)
                return -1;
	} else {
            if (drive_write_image_snapshot_module(s, 1) < 0)
		return -1;
	}
    }
    if (save_roms && drive[0].enable)
        if (drive_write_rom_snapshot_module(s, 0) < 0)
            return -1;
    if (save_roms && drive[1].enable)
        if (drive_write_rom_snapshot_module(s, 1) < 0)
            return -1;
    return 0;
}

static int read_byte_into_int(snapshot_module_t *m, int *value_return)
{
    BYTE b;

    if (snapshot_module_read_byte(m, &b) < 0)
        return -1;
    *value_return = (int) b;
    return 0;
}

static int read_word_into_int(snapshot_module_t *m, int *value_return)
{
    WORD b;

    if (snapshot_module_read_word(m, &b) < 0)
        return -1;
    *value_return = (int) b;
    return 0;
}

static int read_dword_into_unsigned_long(snapshot_module_t *m,
                                         unsigned long *value_return)
{
    DWORD b;

    if (snapshot_module_read_dword(m, &b) < 0)
        return -1;
    *value_return = (unsigned long) b;
    return 0;
}

static int read_dword_into_int(snapshot_module_t *m, int *value_return)
{
    DWORD b;

    if (snapshot_module_read_dword(m, &b) < 0)
        return -1;
    *value_return = (int) b;
    return 0;
}

int drive_read_snapshot_module(snapshot_t *s)
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

    if (read_dword_into_int(m, &sync_factor) < 0) {
        if (m != NULL)
            snapshot_module_close(m);
        return -1;
    }

    for (i = 0; i < 2; i++) {
        if (0
            || read_dword_into_unsigned_long(m, &drive[i].accum) < 0
            || snapshot_module_read_dword(m, &drive[i].attach_clk) < 0
            || read_dword_into_unsigned_long(m, &drive[i].bits_moved) < 0
            || read_byte_into_int(m, &drive[i].byte_ready) < 0
            || read_byte_into_int(m, &drive[i].clock_frequency) < 0
            || read_word_into_int(m, &drive[i].current_half_track) < 0
            || snapshot_module_read_dword(m, &drive[i].detach_clk) < 0
            || snapshot_module_read_byte(m, &drive[i].diskID1) < 0
            || snapshot_module_read_byte(m, &drive[i].diskID2) < 0
            || read_byte_into_int(m, &drive[i].extend_image_policy) < 0
            || read_byte_into_int(m, &drive[i].finish_byte) < 0
            || read_dword_into_int(m, &drive[i].GCR_head_offset) < 0
            || snapshot_module_read_byte(m, &drive[i].GCR_read) < 0
            || snapshot_module_read_byte(m, &drive[i].GCR_write_value) < 0
            || read_byte_into_int(m, &drive[i].idling_method) < 0
            || read_byte_into_int(m, &drive[i].last_mode) < 0
            || read_byte_into_int(m, &drive[i].parallel_cable_enabled) < 0
            || read_byte_into_int(m, &drive[i].read_only) < 0
            || snapshot_module_read_dword(m, &drive[i].rotation_last_clk) < 0
            || snapshot_module_read_dword(m, &rotation_table_ptr[i]) < 0
            || read_dword_into_int(m, &drive[i].type) < 0
        ) {
            if (m != NULL)
                snapshot_module_close(m);
            return -1;
        }
    }
    snapshot_module_close(m);
    m = NULL;

    for (i = 0; i < 2; i++) {
        drive[i].shifter = drive[i].bits_moved;
        drive[i].rotation_table_ptr = drive[i].rotation_table[0]
            + rotation_table_ptr[i];
        drive[i].GCR_track_start_ptr = (drive[i].gcr->data
                           + ((drive[i].current_half_track / 2 - 1)
                              * NUM_MAX_BYTES_TRACK));
        if (drive[i].type != DRIVE_TYPE_1571) {
            if (drive[i].type == DRIVE_TYPE_1581) {
                drive_initialize_rotation_table(1, i);
                resources_set_value("VideoStandard",
                                    (resource_value_t)sync_factor);
            } else {
                drive[i].side = 0;
                drive_initialize_rotation_table(0, i);
                resources_set_value("VideoStandard",
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
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        drive[0].enable = 1;
        drive_setup_rom_image(0);
        drive_mem_init(&drive0_context, drive[0].type);
        resources_set_value("Drive8IdleMethod",
                            (resource_value_t)drive[0].idling_method);
        drive_initialize_rom_traps(0);
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
        drive_setup_rom_image(1);
        drive_mem_init(&drive1_context, drive[1].type);
        resources_set_value("Drive9IdleMethod",
                            (resource_value_t) drive[1].idling_method);
        drive_initialize_rom_traps(1);
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

    if (drive[0].enable) {
        if (drive_cpu_read_snapshot_module(&drive0_context, s) < 0)
            return -1;
        if (drive[0].type == DRIVE_TYPE_1541
            || drive[0].type == DRIVE_TYPE_1541II
            || drive[0].type == DRIVE_TYPE_2031) {
            if (via1d_read_snapshot_module(&drive0_context, s) < 0
                || via2d_read_snapshot_module(&drive0_context, s) < 0)
                return -1;
        }
        if (drive[0].type == DRIVE_TYPE_1571) {
            if (via1d_read_snapshot_module(&drive0_context, s) < 0
                || via2d_read_snapshot_module(&drive0_context, s) < 0
                || cia1571d0_read_snapshot_module(s) < 0)
                return -1;
        }
        if (drive[0].type == DRIVE_TYPE_1581) {
            if (cia1581d0_read_snapshot_module(s) < 0)
                return -1;
        }
	if ((drive[0].type == DRIVE_TYPE_1001)
	    || (drive[0].type == DRIVE_TYPE_8050)
	    || (drive[0].type == DRIVE_TYPE_8250)
	    ) {
	    if (riot1d0_read_snapshot_module(s) < 0
		|| riot2d0_read_snapshot_module(s) < 0
		|| fdc_read_snapshot_module(s, 0) < 0)
		return -1;
	}
    }

    if (drive[1].enable) {
        if (drive_cpu_read_snapshot_module(&drive1_context, s) < 0)
            return -1;
        if (drive[1].type == DRIVE_TYPE_1541
            || drive[1].type == DRIVE_TYPE_1541II
            || drive[1].type == DRIVE_TYPE_2031) {
            if (via1d_read_snapshot_module(&drive1_context, s) < 0
                || via2d_read_snapshot_module(&drive1_context, s) < 0)
                return -1;
        }
        if (drive[1].type == DRIVE_TYPE_1571) {
            if (via1d_read_snapshot_module(&drive1_context, s) < 0
                || via2d_read_snapshot_module(&drive1_context, s) < 0
                || cia1571d1_read_snapshot_module(s) < 0)
                return -1;
        }
        if (drive[1].type == DRIVE_TYPE_1581) {
            if (cia1581d1_read_snapshot_module(s) < 0)
                return -1;
        }
	if ((drive[1].type == DRIVE_TYPE_1001)
	    || (drive[1].type == DRIVE_TYPE_8050)
	    || (drive[1].type == DRIVE_TYPE_8250)
	    ) {
	    if (riot1d1_read_snapshot_module(s) < 0
		|| riot2d1_read_snapshot_module(s) < 0
		|| fdc_read_snapshot_module(s, 1) < 0)
		return -1;
	}
    }
    if (drive_read_image_snapshot_module(s, 0) < 0
	|| drive_read_gcrimage_snapshot_module(s, 0) < 0)
        return -1;
    if (drive_read_image_snapshot_module(s, 1) < 0
	|| drive_read_gcrimage_snapshot_module(s, 1) < 0)
        return -1;
    if (drive_read_rom_snapshot_module(s, 0) < 0)
        return -1;
    if (drive_read_rom_snapshot_module(s, 1) < 0)
        return -1;

    if (drive[0].type != DRIVE_TYPE_NONE)
        drive_enable(0);
    if (drive[1].type != DRIVE_TYPE_NONE)
        drive_enable(1);

    iec_calculate_callback_index();
    iec_update_ports_embedded();
    drive_update_ui_status();

    resources_get_value("DriveTrueEmulation",
                        (resource_value_t *)&drive_true_emulation);

    if (vdrive_read_snapshot_module(s, drive_true_emulation ? 10 : 8) < 0)
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
 * WORD Type		Disk image type (1581, 8050, 8250)
 * 256 * blocks(disk image type) BYTE
 *			disk image
 *
 */

static int drive_write_image_snapshot_module(snapshot_t *s, int dnr)
{
    char snap_module_name[10];
    snapshot_module_t *m;
    BYTE sector_data[0x100];
    WORD word;
    int track, sector;
    int rc;

    if (drive[dnr].image == NULL) {
        return 0;
    }

    sprintf(snap_module_name, "IMAGE%i", dnr);

    m = snapshot_module_create(s, snap_module_name, IMAGE_SNAP_MAJOR,
                               IMAGE_SNAP_MINOR);
    if (m == NULL)
       return -1;

    word = drive[dnr].image->type;
    snapshot_module_write_word(m, word);

    /* we use the return code to step through the tracks. So we do not
       need any geometry info. */
    for (track = 1; ; track++) {
        rc = 0;
        for (sector = 0; ; sector++) {
            rc = disk_image_read_sector(drive[dnr].image, sector_data, track,
                                        sector);
            if (rc == 0) {
                snapshot_module_write_byte_array(m, sector_data, 0x100);
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

static int drive_read_image_snapshot_module(snapshot_t *s, int dnr)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;
    char snap_module_name[10];
    WORD word;
    char *p, filename[L_tmpnam];
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

    if (snapshot_module_read_word(m, &word) < 0) {
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
    p = tmpnam(filename);
    if (!p) {
	log_error(drive_snapshot_log, "Could not create temporary filename");
        snapshot_module_close(m);
	return -1;
    }
    fp = fopen(filename, "w+b");
    if (!fp) {
	log_error(drive_snapshot_log, "Could not create temporary file");
	log_error(drive_snapshot_log, "filename=%s", filename);
        snapshot_module_close(m);
	return -1;
    }
    /* blow up the file to needed size */
    if (fseek(fp, len - 1, SEEK_SET) < 0
	|| (fputc(0, fp) == EOF)) {
	log_error(drive_snapshot_log, "Could not create large temporary file");
	fclose(fp);
        snapshot_module_close(m);
	return -1;
    }
    fclose(fp);
    if (file_system_attach_disk(dnr + 8, filename) < 0) {
        log_error(drive_snapshot_log, "Invalid Disk Image");
        snapshot_module_close(m);
	return -1;
    }

    sprintf(request_str, "Disk image unit #%d imported from snapshot", dnr + 8);
    zfile_close_action(filename, ZFILE_REQUEST, request_str);

    /* we use the return code to step through the tracks. So we do not
       need any geometry info. */
    snapshot_module_read_byte_array(m, sector_data, 0x100);
    for (track = 1; ; track++) {
        rc = 0;
        for (sector = 0; ; sector++) {
            rc = disk_image_write_sector(drive[dnr].image, sector_data, track,
                                         sector);
            if (rc == 0) {
                snapshot_module_read_byte_array(m, sector_data, 0x100);
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

static int drive_write_gcrimage_snapshot_module(snapshot_t *s, int dnr)
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

    tmpbuf = (char*)xmalloc(MAX_TRACKS_1571 * 4);

    for (i = 0; i < MAX_TRACKS_1571; i++) {
        tmpbuf[i * 4] = drive[dnr].gcr->track_size[i] & 0xff;
        tmpbuf[i * 4 + 1] = (drive[dnr].gcr->track_size[i] >> 8) & 0xff;
        tmpbuf[i * 4 + 2] = (drive[dnr].gcr->track_size[i] >> 16) & 0xff;
        tmpbuf[i * 4 + 3] = (drive[dnr].gcr->track_size[i] >> 24) & 0xff;
    }

    if (0
        || snapshot_module_write_byte_array(m, drive[dnr].gcr->data,
            sizeof(drive[dnr].gcr->data)) < 0
        || snapshot_module_write_byte_array(m, drive[dnr].gcr->speed_zone,
            sizeof(drive[dnr].gcr->speed_zone)) < 0
        || snapshot_module_write_byte_array(m, tmpbuf, MAX_TRACKS_1571 * 4) < 0
        ) {
        if (m != NULL)
            snapshot_module_close(m);
        free(tmpbuf);
        return -1;
    }
    free(tmpbuf);
    if (snapshot_module_close(m) < 0)
        return -1;
    return 0;
}

static int drive_read_gcrimage_snapshot_module(snapshot_t *s, int dnr)
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

    tmpbuf = (char*)xmalloc(MAX_TRACKS_1571 * 4);

    if (0
        || snapshot_module_read_byte_array(m, drive[dnr].gcr->data,
            sizeof(drive[dnr].gcr->data)) < 0
        || snapshot_module_read_byte_array(m, drive[dnr].gcr->speed_zone,
            sizeof(drive[dnr].gcr->speed_zone)) < 0
        || snapshot_module_read_byte_array(m, tmpbuf, MAX_TRACKS_1571 * 4) < 0
        ) {
        if (m != NULL)
            snapshot_module_close(m);
        free(tmpbuf);
        return -1;
    }
    snapshot_module_close(m);
    m = NULL;

    for (i = 0; i < MAX_TRACKS_1571; i++)
        drive[dnr].gcr->track_size[i] = tmpbuf[i * 4] + (tmpbuf[i * 4 + 1] << 8)
            + (tmpbuf[i * 4 + 2] << 16) + (tmpbuf[i * 4 + 3] << 24);

    free(tmpbuf);
    drive[dnr].GCR_image_loaded = 1;
    drive[dnr].image = NULL;
    return 0;
}

/* -------------------------------------------------------------------- */

#define ROM_SNAP_MAJOR 1
#define ROM_SNAP_MINOR 0

static int drive_write_rom_snapshot_module(snapshot_t *s, int dnr)
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
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        base = &(drive[dnr].rom[0x4000]);
        len = DRIVE_ROM1001_SIZE;
        break;
      default:
        return -1;
    }

    if (snapshot_module_write_byte_array(m, base, len) < 0) {
        if (m != NULL)
            snapshot_module_close(m);
        return -1;
    }
    if (snapshot_module_close(m) < 0)
        return -1;
    return 0;
}

static int drive_read_rom_snapshot_module(snapshot_t *s, int dnr)
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
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        base = &(drive[dnr].rom[0x4000]);
        len = DRIVE_ROM1001_SIZE;
        break;
      default:
        return -1;
    }

    if (snapshot_module_read_byte_array(m, base, len) < 0) {
        if (m != NULL)
            snapshot_module_close(m);
        return -1;
    }

    if (drive[dnr].type == DRIVE_TYPE_1541) {
	drive_do_1541_checksum();
    }

    snapshot_module_close(m);
    return 0;
}

