/*
 * drive.c - Hardware-level Commodore disk drive emulation.
 *
 * Written by
 *  Daniel Sladic (sladic@eecg.toronto.edu)
 *  Andreas Boose (boose@unixserv.rz.fh-hannover.de)
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

/* TODO:
	- more accurate emulation of disk rotation.
	- different speeds within one track.
	- support for .d64 images with attached error code.
	- check for byte ready *within* `BVC', `BVS' and `PHP'.
	- serial bus handling might be faster.  */

#define __1541__

#include "vice.h"

#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "drive.h"
#include "iecdrive.h"
#include "gcr.h"
#include "interrupt.h"
#include "vmachine.h"
#include "serial.h"
#include "vdrive.h"
#include "warn.h"
#include "mem.h"
#include "resources.h"
#include "cmdline.h"
#include "memutils.h"
#include "viad.h"
#include "via.h"
#include "cia.h"
#include "utils.h"
#include "ui.h"

/* ------------------------------------------------------------------------- */

/* Drive specific variables.  */
drive_t drive[2];

/* Prototypes of functions called by resource management.  */
static int drive_enable(int dnr);
static void drive_disable(int dnr);
static int drive_set_disk_drive_type(int drive_type, int dnr);
static void drive_set_sync_factor(unsigned int factor);
static void drive_set_ntsc_sync_factor(void);
static void drive_set_pal_sync_factor(void);

/* Flag: Is the true 1541 emulation turned on?  */
int drive_enabled[2];

/* What drive type we have to emulate?  */
/*static int drive_type[2];*/

/* Pointer to the IEC bus structure.  */
static iec_info_t *iec_info;

/* Flag: Do we emulate a SpeedDOS-compatible parallel cable?  */
int drive_parallel_cable_enabled;

/* What extension policy?  (See `DRIVE_EXTEND_*' in `drive.h'.)  */
static int extend_image_policy;

/* What idling method?  (See `DRIVE_IDLE_*' in `drive.h'.)  */
static int idling_method[2];

/* Original ROM code is saved here.  */
static BYTE drive_rom_idle_trap[2];

/* What sync factor between the CPU and the 1541?  If equal to
   `DRIVE_SYNC_PAL', the same as PAL machines.  If equal to
   `DRIVE_SYNC_NTSC', the same as NTSC machines.  The sync factor is
   calculated as

   65536 * clk_1541 / clk_[c64|vic20]

   where `clk_1541' is fixed to 1 MHz, while `clk_[c64|vic20]' depends on the
   video timing (PAL or NTSC).  The pre-calculated values for PAL and NTSC
   are in `pal_sync_factor' and `ntsc_sync_factor'.  */
static int sync_factor;

/* Name of the DOS ROMs.  */
static char *dos_rom_name_1541;
static char *dos_rom_name_1571;
static char *dos_rom_name_1581;

static int set_drive0_enabled(resource_value_t v)
{
    if ((int) v)
	drive_enable(0);
    else
	drive_disable(0);
    return 0;
}

static int set_drive1_enabled(resource_value_t v)
{
    if ((int) v)
	drive_enable(1);
    else
	drive_disable(1);
    return 0;
}

static int set_drive0_type(resource_value_t v)
{
    switch ((int) v) {
      case DRIVE_TYPE_1541:
      case DRIVE_TYPE_1571:
      case DRIVE_TYPE_1581:
        return drive_set_disk_drive_type((int) v, 0);
      default:
        return -1;
    }
}

static int set_drive1_type(resource_value_t v)
{
    switch ((int) v) {
      case DRIVE_TYPE_1541:
      case DRIVE_TYPE_1571:
      case DRIVE_TYPE_1581:
        return drive_set_disk_drive_type((int) v, 1);
      default:
        return -1;
    }
}

static int set_drive_parallel_cable_enabled(resource_value_t v)
{
    drive_parallel_cable_enabled = (int) v;
    return 0;
}

static int set_extend_image_policy(resource_value_t v)
{
    switch ((int) v) {
      case DRIVE_EXTEND_NEVER:
      case DRIVE_EXTEND_ASK:
      case DRIVE_EXTEND_ACCESS:
        extend_image_policy = (int) v;
        return 0;
      default:
        return -1;
    }
}

static int set_drive0_idling_method(resource_value_t v)
{
    /* FIXME: Maybe we should call `drive[01]_cpu_execute()' here?  */
    if ((int) v != DRIVE_IDLE_SKIP_CYCLES
        && (int) v != DRIVE_IDLE_TRAP_IDLE
        && (int) v != DRIVE_IDLE_NO_IDLE)
        return -1;

    if (rom_loaded && drive[0].type == DRIVE_TYPE_1541)
        drive[0].rom[0xec9b - 0x8000] = 
            (idling_method[0] != DRIVE_IDLE_TRAP_IDLE)
            ? 0x00 : drive_rom_idle_trap[0];
    idling_method[0] = (int) v;
    return 0;
}
static int set_drive1_idling_method(resource_value_t v)
{
    /* FIXME: Maybe we should call `drive[01]_cpu_execute()' here?  */
    if ((int) v != DRIVE_IDLE_SKIP_CYCLES
        && (int) v != DRIVE_IDLE_TRAP_IDLE
        && (int) v != DRIVE_IDLE_NO_IDLE)
        return -1;

    if (rom_loaded && drive[1].type == DRIVE_TYPE_1541)
        drive[1].rom[0xec9b - 0x8000] =
            (idling_method[1] != DRIVE_IDLE_TRAP_IDLE)
            ? 0x00 : drive_rom_idle_trap[1];
    idling_method[1] = (int) v;
    return 0;
}

static int set_sync_factor(resource_value_t v)
{
    switch ((int) v) {
      case DRIVE_SYNC_PAL:
        sync_factor = (int) v;
        drive_set_pal_sync_factor();
        break;
      case DRIVE_SYNC_NTSC:
        sync_factor = (int) v;
        drive_set_ntsc_sync_factor();
        break;
      default:
        if ((int) v > 0) {
            sync_factor = (int) v;
            drive_set_sync_factor((int) v);
        } else {
            return -1;
        }
    }

    return 0;
}

static int set_dos_rom_name_1541(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_1541 == NULL)
        dos_rom_name_1541 = stralloc(name);
    else {
        dos_rom_name_1541 = xrealloc(dos_rom_name_1541, strlen(name) + 1);
        strcpy(dos_rom_name_1541, name);
    }
    return 0;
}

static int set_dos_rom_name_1571(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_1571 == NULL)
        dos_rom_name_1571 = stralloc(name);
    else {
        dos_rom_name_1571 = xrealloc(dos_rom_name_1571, strlen(name) + 1);
        strcpy(dos_rom_name_1571, name);
    }
    return 0;
}

static int set_dos_rom_name_1581(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_1581 == NULL)
        dos_rom_name_1581 = stralloc(name);
    else {
        dos_rom_name_1581 = xrealloc(dos_rom_name_1581, strlen(name) + 1);
        strcpy(dos_rom_name_1581, name);
    }
    return 0;
}

static resource_t resources[] = {
    { "Drive8", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &drive_enabled[0], set_drive0_enabled },
    { "Drive9", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &drive_enabled[1], set_drive1_enabled },
    { "Drive8Type", RES_INTEGER, (resource_value_t) DRIVE_TYPE_1541,
      (resource_value_t *) &(drive[0].type), set_drive0_type },
    { "Drive9Type", RES_INTEGER, (resource_value_t) DRIVE_TYPE_1541,
      (resource_value_t *) &(drive[1].type), set_drive1_type },
    { "DriveParallelCable", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &drive_parallel_cable_enabled, set_drive_parallel_cable_enabled },
    { "DriveExtendImagePolicy", RES_INTEGER, (resource_value_t) DRIVE_EXTEND_NEVER,
      (resource_value_t *) &extend_image_policy, set_extend_image_policy },
    { "Drive8IdleMethod", RES_INTEGER, (resource_value_t) DRIVE_IDLE_TRAP_IDLE,
      (resource_value_t *) &idling_method[0], set_drive0_idling_method },
    { "Drive9IdleMethod", RES_INTEGER, (resource_value_t) DRIVE_IDLE_TRAP_IDLE,
      (resource_value_t *) &idling_method[1], set_drive1_idling_method },
    { "DriveSyncFactor", RES_INTEGER, (resource_value_t) DRIVE_SYNC_PAL,
      (resource_value_t *) &sync_factor, set_sync_factor },
    { "DosName1541", RES_STRING, (resource_value_t) "dos1541",
      (resource_value_t *) &dos_rom_name_1541, set_dos_rom_name_1541 },
    { "DosName1571", RES_STRING, (resource_value_t) "dos1571",
      (resource_value_t *) &dos_rom_name_1571, set_dos_rom_name_1571 },
    { "DosName1581", RES_STRING, (resource_value_t) "dos1581",
      (resource_value_t *) &dos_rom_name_1581, set_dos_rom_name_1581 },
    { NULL }
};

int drive_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static cmdline_option_t cmdline_options[] = {
    { "-drive8", SET_RESOURCE, 0, NULL, NULL, "Drive8", (resource_value_t) 1,
      NULL, "Enable hardware-level emulation of drive #8" },
    { "+drive8", SET_RESOURCE, 0, NULL, NULL, "Drive8", (resource_value_t) 0,
      NULL, "Disable hardware-level emulation of drive #8" },
    { "-parallel", SET_RESOURCE, 0, NULL, NULL, "DriveParallelCable",
      (resource_value_t) 1,
      NULL, "Enable SpeedDOS-compatible parallel cable" },
    { "+parallel", SET_RESOURCE, 0, NULL, NULL, "DriveParallelCable",
      (resource_value_t) 0,
      NULL, "Disable SpeedDOS-compatible parallel cable" },
    { "-drive8idle", SET_RESOURCE, 2, NULL, NULL, "Drive8IdleMethod",
      NULL, "<method>",
      "Set drive idling method (0: no traps, 1: skip cycles, 2: trap idle)" },
    { "-drive9idle", SET_RESOURCE, 2, NULL, NULL, "Drive9IdleMethod",
      NULL, "<method>",
      "Set drive idling method (0: no traps, 1: skip cycles, 2: trap idle)" },
    { "-drivesync", SET_RESOURCE, 1, NULL, NULL, "DriveSyncFactor",
      NULL, "<value>", "Set 1541 sync factor to <value>" },
    { "-paldrive", SET_RESOURCE, 0, NULL, NULL, "DriveSyncFactor",
      (resource_value_t) DRIVE_SYNC_PAL,
      NULL, "Use PAL 1541 sync factor" },
    { "-ntscdrive", SET_RESOURCE, 0, NULL, NULL, "DriveSyncFactor",
      (resource_value_t) DRIVE_SYNC_NTSC,
      NULL, "Use NTSC 1541 sync factor" },
    { "-dos1541", SET_RESOURCE, 1, NULL, NULL, "DosName1541", NULL,
      "<name>", "Specify name of 1541 DOS ROM image name" },
    { "-dos1571", SET_RESOURCE, 1, NULL, NULL, "DosName1571", NULL,
      "<name>", "Specify name of 1571 DOS ROM image name" },
    { "-dos1581", SET_RESOURCE, 1, NULL, NULL, "DosName1581", NULL,
      "<name>", "Specify name of 1581 DOS ROM image name" },
    { NULL }
};

int drive_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* RAM/ROM.  */
static BYTE drive_rom1541[DRIVE_ROM1541_SIZE];
static BYTE drive_rom1571[DRIVE_ROM1571_SIZE];
static BYTE drive_rom1581[DRIVE_ROM1581_SIZE];

BYTE drive0_ram[DRIVE_RAM_SIZE];
BYTE drive1_ram[DRIVE_RAM_SIZE];

/* If nonzero, at least one vaild drive ROM has already been loaded.  */
static int rom_loaded = 0;

/* If nonzero, the ROM image has been loaded.  */
static int rom1541_loaded = 0;
static int rom1571_loaded = 0;
static int rom1581_loaded = 0;

/* Map of the sector sizes.  */
extern char sector_map_1541[43];
extern char sector_map_1571[71];

/* Speed zone of each track of the 1541 disk drive.  */
extern int speed_map_1541[42];

/* Speed zone of each track of the 1571 disk drive.  */
extern int speed_map_1571[70];

/* Speed (in bps) of the disk in the 4 disk areas.  */
static int rot_speed_bps[2][4] = { { 250000, 266667, 285714, 307692 },
                                   { 125000, 133333, 142857, 153846 } };

/* Number of bytes per track size.  */
static int raw_track_size[4] = { 6250, 6666, 7142, 7692 };

/* If the user does not want to extend the disk image and `ask mode' is
   selected this flag gets cleared.  */
static int ask_extend_disk_image;

/* Clock speed of the PAL and NTSC versions of the connected computer.  */
static CLOCK pal_cycles_per_sec;
static CLOCK ntsc_cycles_per_sec;

/* Warnings.  */
enum drive_warnings { WARN_GCRWRITE };
#define DRIVE_NUM_WARNINGS (WARN_GCRWRITE + 1)
static warn_t *drive_warn;

#define GCR_OFFSET(track, sector)  ((track - 1) * NUM_MAX_BYTES_TRACK \
				    + sector * NUM_BYTES_SECTOR_GCR)

static void GCR_data_writeback(int dnr);
static void initialize_rotation(int freq, int dnr);
static void drive_extend_disk_image(int dnr);
static void drive_set_half_track(int num, int dnr);
static void drive_update_ui_status(void);
static int drive_sync_found(int dnr);
static int drive_write_protect_sense(int dnr);
static int drive_load_rom_images(void);
static void drive_setup_rom_image(int dnr);
static void drive_initialize_rom_traps(int dnr);

/* ------------------------------------------------------------------------- */

/* Disk image handling. */

static void drive_read_image_d64_d71(int dnr)
{
    BYTE buffer[260], *ptr, chksum;
    int rc, i;
    int track, sector;

    if (!drive[dnr].drive_floppy)
	return;

    buffer[0] = 0x07;
    buffer[258] = buffer[259] = 0;

    /* Since the D64/D71 format does not provide the actual track sizes or
       speed zones, we set them to standard values.  */
    if (drive[dnr].drive_floppy->ImageFormat == 1541) {
        for (track = 0; track < MAX_TRACKS_1541; track++) {
            drive[dnr].GCR_track_size[track] = 
                raw_track_size[speed_map_1541[track]];
            memset(drive[dnr].GCR_speed_zone, speed_map_1541[track], 
                NUM_MAX_BYTES_TRACK);
        }
    }
    if (drive[dnr].drive_floppy->ImageFormat == 1571) {
        for (track = 0; track < MAX_TRACKS_1571; track++) {
            drive[dnr].GCR_track_size[track] =
                raw_track_size[speed_map_1571[track]];
            memset(drive[dnr].GCR_speed_zone, speed_map_1571[track],
                NUM_MAX_BYTES_TRACK);
        }
    }

    drive_set_half_track(drive[dnr].current_half_track, dnr);

    for (track = 1; track <= drive[dnr].drive_floppy->NumTracks; track++) {
    int max_sector;

	ptr = drive[dnr].GCR_data + GCR_OFFSET(track, 0);
    if (drive[dnr].drive_floppy->ImageFormat == 1541)
        max_sector = sector_map_1541[track];
    if (drive[dnr].drive_floppy->ImageFormat == 1571)
        max_sector = sector_map_1571[track];
 
	/* Clear track to avoid read errors.  */
	memset(ptr, 0xff, NUM_MAX_BYTES_TRACK);

	for (sector = 0; sector < max_sector; sector++) {
	    ptr = drive[dnr].GCR_data + GCR_OFFSET(track, sector);

	    rc = floppy_read_block(drive[dnr].drive_floppy->ActiveFd,
				   drive[dnr].drive_floppy->ImageFormat,
				   buffer + 1, track, sector,
				   drive[dnr].drive_floppy->D64_Header);
	    if (rc < 0) {
		printf("DRIVE#%i: Error reading T:%d S:%d from the disk image\n",
		       dnr + 8, track, sector);
		/* FIXME: could be handled better. */
	    } else {
		chksum = buffer[1];
		for (i = 2; i < 257; i++)
		    chksum ^= buffer[i];
		buffer[257] = chksum;
		convert_sector_to_GCR(buffer, ptr, track, sector,
            drive[dnr].diskID1, drive[dnr].diskID2);
	    }
	}
    }
}

static int drive_read_image_gcr(int dnr)
{
    int track, track_len, zone_len, i, NumTracks;
    BYTE len[2], comp_speed[NUM_MAX_BYTES_TRACK / 4];
    BYTE *track_data, *zone_data;
    DWORD gcr_track_p[MAX_TRACKS_1541 * 2];
    DWORD gcr_speed_p[MAX_TRACKS_1541 * 2];
    off_t offset;

    NumTracks = drive[dnr].drive_floppy->NumTracks;

    lseek(drive[dnr].drive_floppy->ActiveFd, 12, SEEK_SET);
    if (read_dword(drive[dnr].drive_floppy->ActiveFd, gcr_track_p,
        NumTracks * 8) < 0) {
	fprintf(stderr, "1541: Could not read GCR disk image.\n");
	return 0;
    }

    lseek(drive[dnr].drive_floppy->ActiveFd, 12 + NumTracks * 8, SEEK_SET);
    if (read_dword(drive[dnr].drive_floppy->ActiveFd, gcr_speed_p,
        NumTracks * 8) < 0) {
	fprintf(stderr, "1541: Could not read GCR disk image.\n");
	return 0;
    }

    for (track = 0; track < MAX_TRACKS_1541; track++) {

	track_data = drive[dnr].GCR_data + track * NUM_MAX_BYTES_TRACK;
	zone_data = drive[dnr].GCR_speed_zone + track * NUM_MAX_BYTES_TRACK;
	memset(track_data, 0xff, NUM_MAX_BYTES_TRACK);
	memset(zone_data, 0x00, NUM_MAX_BYTES_TRACK / 4);
	drive[dnr].GCR_track_size[track] = 6250;

	if (track <= NumTracks && gcr_track_p[track * 2] != 0) {

	    offset = gcr_track_p[track * 2];

	    lseek(drive[dnr].drive_floppy->ActiveFd, offset, SEEK_SET);
	    if (read(drive[dnr].drive_floppy->ActiveFd, (char *)len, 2) < 2) {
		fprintf(stderr, "1541: Could not read GCR disk image.\n");
		return 0;
	    }

	    track_len = len[0] + len[1] * 256;

	    if (track_len < 5000 || track_len > 7928) {
		fprintf(stderr, "1541: Track field length %i is not supported.\n",
            track_len);
		return 0;
	    }

	    drive[dnr].GCR_track_size[track] = track_len;

	    lseek(drive[dnr].drive_floppy->ActiveFd, offset + 2, SEEK_SET);
	    if (read(drive[dnr].drive_floppy->ActiveFd, (char *)track_data,
            track_len)
			< track_len) {
		fprintf(stderr, "1541: Could not read GCR disk image.\n");
		return 0;
	    }

	    zone_len = (track_len + 3) / 4;

	    if (gcr_speed_p[track * 2] > 3) {

		offset = gcr_speed_p[track * 2];

		lseek(drive[dnr].drive_floppy->ActiveFd, offset, SEEK_SET);
		if (read(drive[dnr].drive_floppy->ActiveFd, (char *)comp_speed,
			zone_len) < zone_len) {
		    fprintf(stderr, "1541: Could not read GCR disk image.\n");
		    return 0;
		}

		for (i = 0; i < zone_len; i++) {
		    zone_data[i * 4] = comp_speed[i] & 3;
		    zone_data[i * 4 + 1] = (comp_speed[i] >> 2) & 3;
		    zone_data[i * 4 + 2] = (comp_speed[i] >> 4) & 3;
		    zone_data[i * 4 + 3] = (comp_speed[i] >> 6) & 3;
		}
	    } else {
		memset(zone_data, gcr_speed_p[track * 2], NUM_MAX_BYTES_TRACK);
	    }
	}
    }
    return 1;
}

static void write_track_gcr(int track, int dnr)
{
    int gap, i, NumTracks;
    BYTE len[2];
    DWORD gcr_track_p[MAX_TRACKS_1541 * 2];
    DWORD gcr_speed_p[MAX_TRACKS_1541 * 2];
    off_t offset;

    NumTracks = drive[dnr].drive_floppy->NumTracks;

    lseek(drive[dnr].drive_floppy->ActiveFd, 12, SEEK_SET);
    if (read_dword(drive[dnr].drive_floppy->ActiveFd, gcr_track_p,
        NumTracks * 8) < 0) {
	fprintf(stderr, "1541: Could not read GCR disk image header.\n");
	return;
    }

    lseek(drive[dnr].drive_floppy->ActiveFd, 12 + NumTracks * 8, SEEK_SET);
    if (read_dword(drive[dnr].drive_floppy->ActiveFd, gcr_speed_p,
        NumTracks * 8) < 0) {
	fprintf(stderr, "1541: Could not read GCR disk image header.\n");
	return;
    }

    if (gcr_track_p[(track - 1) * 2] == 0) {
	offset = lseek(drive[dnr].drive_floppy->ActiveFd, 0, SEEK_END);
	if (offset < 0) {
	    fprintf(stderr, "1541: Could not extend GCR disk image.\n");
	    return;
	}
	gcr_track_p[(track - 1) * 2] = offset;
    }

    offset = gcr_track_p[(track - 1) * 2];

    len[0] = drive[dnr].GCR_track_size[track - 1] % 256;
    len[1] = drive[dnr].GCR_track_size[track - 1] / 256;

    if (lseek(drive[dnr].drive_floppy->ActiveFd, offset, SEEK_SET) < 0
        || write(drive[dnr].drive_floppy->ActiveFd, (char *)len, 2) < 0) {
	fprintf(stderr, "1541: Could not write GCR disk image.\n");
	return;
    }

    /* Clear gap between the end of the actual track and the start of
       the next track.  */
    gap = NUM_MAX_BYTES_TRACK - drive[dnr].GCR_track_size[track - 1];
    if (gap > 0)
	memset(drive[dnr].GCR_track_start_ptr + drive[dnr].GCR_track_size[track - 1], 0, gap);

    if (lseek(drive[dnr].drive_floppy->ActiveFd, offset + 2, SEEK_SET) < 0
        || write(drive[dnr].drive_floppy->ActiveFd,
        (char *)drive[dnr].GCR_track_start_ptr, NUM_MAX_BYTES_TRACK) < 0) {
	fprintf(stderr, "1541: Could not write GCR disk image.\n");
	return;
    }

    for (i = 0; (drive[dnr].GCR_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK]
	    == drive[dnr].GCR_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK + i])
	    && i < NUM_MAX_BYTES_TRACK; i++);

    if (i < drive[dnr].GCR_track_size[track - 1]) {
	/* This will change soon.  */
	fprintf(stderr,
                "1541: Saving different speed zones is not supported yet.\n");
	return;
    }

    if (gcr_speed_p[(track - 1) * 2] >= 4) {
	/* This will change soon.  */
	fprintf(stderr,
                "1541: Adding new speed zones is not supported yet.\n");
	return;
    }

    offset = 12 + NumTracks * 8 + (track - 1) * 8;
    if (lseek(drive[dnr].drive_floppy->ActiveFd, offset, SEEK_SET) < 0
        || write_dword(drive[dnr].drive_floppy->ActiveFd,
           &gcr_speed_p[(track - 1) * 2], 4) < 0) {
    fprintf(stderr, "1541: Could not write GCR disk image.\n");
    return;
    }

#if 0  /* We do not support writing different speeds yet.  */
    for (i = 0; i < (NUM_MAX_BYTES_TRACK / 4); i++)
    zone_len = (drive[dnr].GCR_track_size[track - 1] + 3) / 4;
    zone_data = drive[dnr].GCR_speed_zone + (track - 1) * NUM_MAX_BYTES_TRACK;

    if (gap > 0)
	memset(zone_data + drive[dnr].GCR_track_size[track - 1], 0, gap);

    for (i = 0; i < (NUM_MAX_BYTES_TRACK / 4); i++)
	comp_speed[i] = (zone_data[i * 4]
                         | (zone_data[i * 4 + 1] << 2)
                         | (zone_data[i * 4 + 2] << 4)
                         | (zone_data[i * 4 + 3] << 6));

    if (lseek(drive[dnr].drive_floppy->ActiveFd, offset, SEEK_SET) < 0
        || write(drive[dnr].drive_floppy->ActiveFd, (char *)comp_speed,
                 NUM_MAX_BYTES_TRACK / 4) < 0) {
        fprintf(stderr, "1541: Could not write GCR disk image");
        return;
    }
#endif
}

static int setID(int dnr)
{
    BYTE buffer[256];
    int rc;

    if (!drive[dnr].drive_floppy)
	return -1;

    rc = floppy_read_block(drive[dnr].drive_floppy->ActiveFd,
			   drive[dnr].drive_floppy->ImageFormat,
			   buffer, 18, 0, drive[dnr].drive_floppy->D64_Header);
    if (rc >= 0) {
	drive[dnr].diskID1 = buffer[0xa2];
	drive[dnr].diskID2 = buffer[0xa3];
    }

    return rc;
}

static BYTE *GCR_find_sector_header(int track, int sector, int dnr)
{
    BYTE *offset = drive[dnr].GCR_track_start_ptr;
    BYTE *GCR_track_end = drive[dnr].GCR_track_start_ptr + drive[dnr].GCR_current_track_size;
    char GCR_header[5], header_data[4];
    int i, sync_count = 0, wrap_over = 0;

    while ((offset < GCR_track_end) && !wrap_over) {
	while (*offset != 0xff)	{
	    offset++;
	    if (offset >= GCR_track_end)
		return NULL;
	}

	while (*offset == 0xff)	{
	    offset++;
	    if (offset == GCR_track_end) {
		offset = drive[dnr].GCR_track_start_ptr;
		wrap_over = 1;
	    }
	    /* Check for killer tracks.  */
	    if((++sync_count) >= drive[dnr].GCR_current_track_size)
		return NULL;
	}

	for (i=0; i < 5; i++) {
	    GCR_header[i] = *(offset++);
	    if (offset >= GCR_track_end) {
		offset = drive[dnr].GCR_track_start_ptr;
		wrap_over = 1;
	    }
	}

	convert_GCR_to_4bytes(GCR_header, header_data);

	if (header_data[0] == 0x08) {
	    /* FIXME: Add some sanity checks here.  */
	    if (header_data[2] == sector && header_data[3] == track)
		return offset;
	}
    }
    return NULL;
}

static BYTE *GCR_find_sector_data(BYTE *offset, int dnr)
{
    BYTE *GCR_track_end = drive[dnr].GCR_track_start_ptr + drive[dnr].GCR_current_track_size;
    int header = 0;

    while (*offset != 0xff) {
	offset++;
	if (offset >= GCR_track_end)
	    offset = drive[dnr].GCR_track_start_ptr;
	header++;
	if (header >= 500)
	    return NULL;
    }

    while (*offset == 0xff) {
	offset++;
	if (offset == GCR_track_end)
	    offset = drive[dnr].GCR_track_start_ptr;
    }
    return offset;
}

/* ------------------------------------------------------------------------- */

/* Global clock counters.  */
CLOCK drive_clk[2];

/* Initialize the hardware-level 1541 emulation (should be called at least once
   before anything else).  Return 0 on success, -1 on error.  */
int drive_init(CLOCK pal_hz, CLOCK ntsc_hz)
{
    int track, i;

    pal_cycles_per_sec = pal_hz;
    ntsc_cycles_per_sec = ntsc_hz;

    if (rom_loaded)
	return 0;

    drive_clk[0] = 0L;
    drive_clk[1] = 0L;

    drive_warn = warn_init("1541", DRIVE_NUM_WARNINGS);

    if (drive_load_rom_images() < 0)
	return -1;

    iec_info = iec_get_drive_port();
    /* Set IEC lines of disabled drives to `1'.  */
    if (iec_info != NULL) {
	iec_info->drive_bus = 0xff;
	iec_info->drive_data = 0xff;
	iec_info->drive2_bus = 0xff;
	iec_info->drive2_data = 0xff;
    }

    printf("Drive: Finished loading ROM images.\n");
    rom_loaded = 1;

    drive_setup_rom_image(0);
    drive_setup_rom_image(1);

    for (i = 0; i < 2; i++) {
	drive[i].byte_ready = 1;
	drive[i].GCR_dirty_track = 0;
	drive[i].GCR_write_value = 0x55;
	drive[i].GCR_track_start_ptr = drive[i].GCR_data;
	drive[i].attach_clk = (CLOCK)0;
	drive[i].detach_clk = (CLOCK)0;
	drive[i].bits_moved = drive[i].accum = 0;
	drive[i].finish_byte = 0;
	drive[i].last_mode = 1;
	drive[i].rotation_last_clk = 0L;
	drive[i].have_new_disk = 0;
	drive[i].rotation_table_ptr = drive[i].rotation_table[0];
	drive[i].old_led_status = 0;
	drive[i].old_half_track = 0;
	drive[i].side = 0;
	for (track = 0; track < MAX_TRACKS_1541; track++)
	    drive[i].GCR_track_size[track] = raw_track_size[speed_map_1541[track]];
	/* Position the R/W head on the directory track.  */
	drive_set_half_track(36, i);
    }

    drive_initialize_rom_traps(0);
    drive_initialize_rom_traps(1);

    initialize_rotation(0, 0);
    initialize_rotation(0, 1);

    drive0_cpu_init(drive[0].type);
    drive1_cpu_init(drive[1].type);

    /* Make sure the sync factor is acknowledged correctly.  */
    set_sync_factor((resource_value_t) sync_factor);

    /* Make sure the traps are moved as needed.  */
    if (drive_enabled[0])
	drive_enable(0);
    if (drive_enabled[1])
	drive_enable(1);

    return 0;
}

static int drive_set_disk_drive_type(int type, int dnr)
{
    switch (type) {
      case DRIVE_TYPE_1541:
        if (rom1541_loaded < 1 && rom_loaded)
            return -1;
        drive_rotate_disk(0, dnr);
        drive[dnr].clock_frequency = 1;
        break;
      case DRIVE_TYPE_1571:
        if (rom1571_loaded < 1 && rom_loaded)
            return -1;
        drive_rotate_disk(0, dnr);
        drive[dnr].clock_frequency = 1;
        break;
      case DRIVE_TYPE_1581:
        if (rom1581_loaded < 1 && rom_loaded)
            return -1;
        drive_rotate_disk(0, dnr);
        drive[dnr].clock_frequency = 2;
        break;
      default:
        return -1;
    }

    initialize_rotation(0, dnr);
    drive[dnr].type = type;
    drive[dnr].side = 0;
    drive_setup_rom_image(dnr);
    set_sync_factor((resource_value_t) sync_factor);

    if (dnr == 0)
        drive0_cpu_init(type);
    if (dnr == 1)
        drive1_cpu_init(type);
    return 0;
}

static int drive_load_rom_images(void)
{
    unsigned long s;
    int i;

    /* Load the ROMs. */
    if (mem_load_sys_file(dos_rom_name_1541, drive_rom1541, DRIVE_ROM1541_SIZE,
                          DRIVE_ROM1541_SIZE) < 0) {
        fprintf(stderr,
                "Drive: Warning: 1541 ROM image not found.\n"
                "Drive: Hardware-level 1541 emulation is not available.\n");
    } else {
        rom1541_loaded = 1;

        /* Calculate ROM checksum.  */
        for (i = 0, s = 0; i < DRIVE_ROM1541_SIZE; i++)
            s += drive_rom1541[i];

        if (s != DRIVE_ROM1541_CHECKSUM)
            fprintf(stderr,
                    "Drive: Warning: unknown 1541 ROM image.  Sum: %lu\n", s);
    } 

    if (mem_load_sys_file(dos_rom_name_1571, drive_rom1571, DRIVE_ROM1571_SIZE,
                          DRIVE_ROM1571_SIZE) < 0) {
        fprintf(stderr,
                "Drive: Warning: 1571 ROM image not found.\n"
                "Drive: Hardware-level 1571 emulation is not available.\n");
    } else
        rom1571_loaded = 1;

    if (mem_load_sys_file(dos_rom_name_1581, drive_rom1581, DRIVE_ROM1581_SIZE,
                          DRIVE_ROM1581_SIZE) < 0) {
        fprintf(stderr,
                "Drive: Warning: 1581 ROM image not found.\n"
                "Drive: Hardware-level 1581 emulation is not available.\n");
    } else
        rom1581_loaded = 1;

    /* FIXME: Drive type radio button should be made insensitive here
       if a ROM image is not loaded. */

    if ((rom1541_loaded | rom1571_loaded | rom1581_loaded) < 1) {
        fprintf(stderr, "Drive: No ROM image found at all!\n"
                        "Drive: Hardware-level emulation is not available.\n");
        return -1;
    }

    return 0;
}

static void drive_setup_rom_image(int dnr)
{
    if (rom_loaded) {
        switch (drive[dnr].type) {
          case DRIVE_TYPE_1541:
            memcpy(&(drive[dnr].rom[0x4000]), drive_rom1541, DRIVE_ROM1541_SIZE);
            break;
          case DRIVE_TYPE_1571:
            memcpy(drive[dnr].rom, drive_rom1571, DRIVE_ROM1571_SIZE);
            break;
          case DRIVE_TYPE_1581:
            memcpy(drive[dnr].rom, drive_rom1581, DRIVE_ROM1581_SIZE);
            break;
        }
    }
}

static void drive_initialize_rom_traps(int dnr)
{
    if (drive[dnr].type == DRIVE_TYPE_1541) {
        /* Remove the ROM check.  */
        drive[dnr].rom[0xeae4 - 0x8000] = 0xea;
        drive[dnr].rom[0xeae5 - 0x8000] = 0xea;
        drive[dnr].rom[0xeae8 - 0x8000] = 0xea;
        drive[dnr].rom[0xeae9 - 0x8000] = 0xea;

        /* Trap the idle loop.  */
        drive_rom_idle_trap[dnr] = drive[dnr].rom[0xec9b - 0x8000];
        if (idling_method[dnr] == DRIVE_IDLE_TRAP_IDLE)
        drive[dnr].rom[0xec9b - 0x8000] = 0x00;
   }
}

/* Activate full 1541 emulation. */
static int drive_enable(int dnr)
{
    /* This must come first, because this might be called before the drive
       initialization.  */
    drive_enabled[dnr] = 1;

    if (!rom_loaded)
        return -1;
    /* Always disable kernal traps. */
    serial_remove_traps();

    if (drive[0].drive_floppy != NULL)
	drive_attach_floppy(drive[0].drive_floppy);
    if (drive[1].drive_floppy != NULL)
	drive_attach_floppy(drive[1].drive_floppy);

    if (dnr == 0) 
	drive0_cpu_wake_up();
    if (dnr == 1)
	drive1_cpu_wake_up();

    ui_toggle_drive_status(1);
    return 0;
}

/* Disable full 1541 emulation.  */
static void drive_disable(int dnr)
{
    /* This must come first, because this might be called before the true
       1541 initialization.  */
    drive_enabled[dnr] = 0;

    if (rom_loaded && drive_enabled[0] == 0 && drive_enabled[1] == 0) 
	serial_install_traps();

    if (rom_loaded){
  	if (dnr == 0)
	    drive0_cpu_sleep();
	if (dnr == 1)
	    drive1_cpu_sleep();
	/* Set IEC lines of disabled drives to `1'.  */
	if (dnr == 0 && iec_info != NULL) {
	    iec_info->drive_bus = 0xff;
	    iec_info->drive_data = 0xff;
	}
	if (dnr == 1 && iec_info != NULL) {
	    iec_info->drive2_bus = 0xff;
	    iec_info->drive2_data = 0xff;
	}
    if (dnr == 0)
        GCR_data_writeback(0);
    if (dnr == 1)
        GCR_data_writeback(1);
    }
    ui_toggle_drive_status(0);
}

void drive_reset(void)
{
    drive0_cpu_reset();
    drive1_cpu_reset();
    warn_reset(drive_warn);
}

/* ------------------------------------------------------------------------- */

/* Attach a disk image to the true 1541 emulation. */
int drive_attach_floppy(DRIVE *floppy)
{
    int dnr;

    if (floppy->ImageFormat != 1541 && floppy->ImageFormat != 1571)
	return -1;

    if (floppy->unit != 8 && floppy->unit != 9)
	return -1;

    dnr = floppy->unit - 8;

    drive[dnr].drive_floppy = floppy;
    drive[dnr].have_new_disk = 1;
    drive[dnr].attach_clk = drive_clk[dnr];
    ask_extend_disk_image = 1;

    if (drive[dnr].drive_floppy->GCR_Header != 0) {
        if (!drive_read_image_gcr(dnr))
            return -1;
    } else {
	if (setID(dnr) >= 0) {
	    drive_read_image_d64_d71(dnr);
	    return 0;
	} else {
	    return -1;
	}
    }
    return 0;
}

/* Detach a disk image from the true 1541 emulation. */
int drive_detach_floppy(DRIVE *floppy)
{
    int dnr;

    if (floppy->unit != 8 && floppy->unit != 9)
    return -1;

    dnr = floppy->unit - 8;

    if (floppy != drive[dnr].drive_floppy) {
        /* Shouldn't happen.  */
        fprintf(stderr, "Whaaat?  Attempt for bogus drive detachment!\n");
        return -1;
    } else if (drive[dnr].drive_floppy != NULL) {
	GCR_data_writeback(dnr);
	drive[dnr].detach_clk = drive_clk[dnr];
	drive[dnr].drive_floppy = NULL;
	memset(drive[dnr].GCR_data, 0, sizeof(drive[dnr].GCR_data));
    }
    return 0;
}

/* ------------------------------------------------------------------------- */

/* Initialization.  */
static void initialize_rotation(int freq, int dnr)
{
    int i, j;

    for (i = 0; i < 4; i++) {
        int speed = rot_speed_bps[freq][i];

        for (j = 0; j < ROTATION_TABLE_SIZE; j++) {
            double bits = (double)j * (double)speed / 1000000.0;

            drive[dnr].rotation_table[i][j].bits = (unsigned long)bits;
            drive[dnr].rotation_table[i][j].accum = ((bits -
                                          (unsigned long)bits) * ACCUM_MAX);
        }
    }

    drive[dnr].bits_moved = drive[dnr].accum = 0;
}

/* Set the `byte ready' bit.  */
inline void drive_set_byte_ready(int val, int dnr)
{
    drive[dnr].byte_ready = val;
}

/* Rotate the disk according to the current value of `drive_clk[]'.  If
   `mode_change' is non-zero, there has been a Read -> Write mode switch.  */
void drive_rotate_disk(int mode_change, int dnr)
{
    unsigned long new_bits;

    if (mode_change) {
	drive[dnr].finish_byte = 1;
	return;
    }

    /* If the drive's motor is off or byte ready is disabled do nothing.  */
    if (drive[dnr].byte_ready_active != 0x06)
	return;

    /* Calculate the number of bits that have passed under the R/W head since
       the last time.  */
    {
        CLOCK delta = drive_clk[dnr] - drive[dnr].rotation_last_clk;

        new_bits = 0;
        while (delta > 0) {
            if (delta >= ROTATION_TABLE_SIZE) {
                struct _rotation_table *p = (drive[dnr].rotation_table_ptr
                                             + ROTATION_TABLE_SIZE - 1);
                new_bits += p->bits;
                drive[dnr].accum += p->accum;
                delta -= ROTATION_TABLE_SIZE - 1;
            } else {
                struct _rotation_table *p = drive[dnr].rotation_table_ptr
                                              + delta;
                new_bits += p->bits;
                drive[dnr].accum += p->accum;
                delta = 0;
            }
            if (drive[dnr].accum >= ACCUM_MAX) {
                drive[dnr].accum -= ACCUM_MAX;
                new_bits++;
            }
        }
    }

    if (drive[dnr].bits_moved + new_bits >= 8) {

	drive[dnr].bits_moved += new_bits;
	drive[dnr].rotation_last_clk = drive_clk[dnr];

	if (drive[dnr].finish_byte) {
	    if (drive[dnr].last_mode == 0) { /* write */
		drive[dnr].GCR_dirty_track = 1;
		if (drive[dnr].bits_moved >= 8) {
		    drive[dnr].GCR_track_start_ptr[drive[dnr].GCR_head_offset]
		        = drive[dnr].GCR_write_value;
		    drive[dnr].GCR_head_offset = ((drive[dnr].GCR_head_offset + 1) %
                                       drive[dnr].GCR_current_track_size);
		    drive[dnr].bits_moved -= 8;
		}
	    } else {		/* read */
		if (drive[dnr].bits_moved >= 8) {
		    drive[dnr].GCR_head_offset = ((drive[dnr].GCR_head_offset + 1) %
                                       drive[dnr].GCR_current_track_size);
		    drive[dnr].bits_moved -= 8;
		    drive[dnr].GCR_read = drive[dnr].GCR_track_start_ptr[drive[dnr].GCR_head_offset];
		}
	    }

	    drive[dnr].finish_byte = 0;
	    drive[dnr].last_mode = drive[dnr].read_write_mode;
	}

	if (drive[dnr].last_mode == 0) {	/* write */
	    drive[dnr].GCR_dirty_track = 1;
	    while (drive[dnr].bits_moved >= 8) {
		drive[dnr].GCR_track_start_ptr[drive[dnr].GCR_head_offset]
		    = drive[dnr].GCR_write_value;
		drive[dnr].GCR_head_offset = ((drive[dnr].GCR_head_offset + 1)
                                   % drive[dnr].GCR_current_track_size);
		drive[dnr].bits_moved -= 8;
	    }
	} else {		/* read */
	    drive[dnr].GCR_head_offset = ((drive[dnr].GCR_head_offset
	                                   + drive[dnr].bits_moved / 8)
			       % drive[dnr].GCR_current_track_size);
	    drive[dnr].bits_moved %= 8;
	    drive[dnr].GCR_read = drive[dnr].GCR_track_start_ptr[drive[dnr].GCR_head_offset];

	}

	if (!drive_sync_found(dnr))
	    drive_set_byte_ready(1, dnr);
    } /* if (drive[dnr].bits_moved + new_bits >= 8) */
}

/* ------------------------------------------------------------------------- */

/* This prevents the CLOCK counters `rotation_last_clk', `attach_clk'
   and `detach_clk' from overflowing.  */
void drive_prevent_clk_overflow(CLOCK sub, int dnr)
{
    if (dnr == 0)
	sub = drive0_cpu_prevent_clk_overflow(sub);
    if (dnr == 1)
	sub = drive1_cpu_prevent_clk_overflow(sub);

    if (sub > 0) {
	drive_rotate_disk(0, dnr);
	drive[dnr].rotation_last_clk -= sub;
	if (drive[dnr].attach_clk > (CLOCK) 0)
	    drive[dnr].attach_clk -= sub;
	if (drive[dnr].detach_clk > (CLOCK) 0)
	    drive[dnr].detach_clk -= sub;
    }
}

/* Read a GCR byte from the disk. */
BYTE drive_read_disk_byte(int dnr)
{
    BYTE val;

    if (drive[dnr].attach_clk != (CLOCK)0) {
        if (drive_clk[dnr] - drive[dnr].attach_clk < DRIVE_ATTACH_DELAY)
            return 0;
        drive[dnr].attach_clk = (CLOCK)0;
    }

    drive_rotate_disk(0, dnr);
    val = drive[dnr].GCR_read;

    return val;
}

int drive_byte_ready(int dnr)
{
   if(drive[dnr].byte_ready_active) {
       drive_rotate_disk(0, dnr);
       return drive[dnr].byte_ready;
   } else {
       return 0;
   }
}


/* Return non-zero if the Sync mark is found.  It is required to
   call drive_rotate_disk() to update drive[].GCR_head_offset first.  */
static int drive_sync_found(int dnr)
{
    BYTE val = drive[dnr].GCR_track_start_ptr[drive[dnr].GCR_head_offset];

    if (val != 0xff || drive[dnr].last_mode == 0) {
        return 0;
    } else {
	int next_head_offset = (drive[dnr].GCR_head_offset > 0
				? drive[dnr].GCR_head_offset - 1
				: drive[dnr].GCR_current_track_size - 1);

	if (drive[dnr].GCR_track_start_ptr[next_head_offset] != 0xff)
	    return 0;

	/* As the current rotation code cannot cope with non byte aligned
	   writes, do not change `drive[].bits_moved'!  */
	/* drive[].bits_moved = 0; */
	return 1;
    }
}

/* Move the head to half track `num'.  */
static void drive_set_half_track(int num, int dnr)
{
    if (drive[dnr].type == DRIVE_TYPE_1541 && num > 84)
        num = 84;
    if (drive[dnr].type == DRIVE_TYPE_1571 && num > 140)
        num = 140;
    if (num < 2)
	num = 2;

    drive[dnr].current_half_track = num;
    drive[dnr].GCR_track_start_ptr = (drive[dnr].GCR_data
			   + ((drive[dnr].current_half_track / 2 - 1)
			      * NUM_MAX_BYTES_TRACK));

    drive[dnr].GCR_current_track_size = 
        drive[dnr].GCR_track_size[drive[dnr].current_half_track / 2 - 1];
    drive[dnr].GCR_head_offset = 0;
}

/* Increment the head position by `step' half-tracks. Valid values
   for `step' are `+1' and `-1'.  */
void drive_move_head(int step, int dnr)
{
    GCR_data_writeback(dnr);
    if (drive[dnr].type == DRIVE_TYPE_1571) {
        if (drive[dnr].current_half_track + step == 71)
            return;
    }
    drive_set_half_track(drive[dnr].current_half_track + step, dnr);
    /* FIXME: We are using one track display for both drives.  */
    ui_display_drive_track((double)drive[dnr].current_half_track / 2.0);
}

/* Write one GCR byte to the disk. */
void drive_write_gcr(BYTE val, int dnr)
{
    if (drive[dnr].drive_floppy == NULL)
	return;

    drive_rotate_disk(0, dnr);
    drive[dnr].GCR_write_value = val;
}

/* Return the write protect sense status. */
static int drive_write_protect_sense(int dnr)
{
    /* Toggle the write protection bit if the disk was detached.  */
    if (drive[dnr].detach_clk != (CLOCK)0) {
	if (drive_clk[dnr] - drive[dnr].detach_clk < DRIVE_DETACH_DELAY)
	    return 0;
	drive[dnr].detach_clk = (CLOCK)0;
    }
    if ((drive[dnr].attach_clk != (CLOCK)0) &&
	(drive_clk[dnr] - drive[dnr].attach_clk < DRIVE_ATTACH_DELAY))
	return 0;
    if (drive[dnr].drive_floppy == NULL) {
	/* No disk in drive, write protection is on. */
	return 1;
    } else if (drive[dnr].have_new_disk) {
	/* Disk has changed, make sure the drive sees at least one change in
	   the write protect status. */
	drive[dnr].have_new_disk = 0;
	return !drive[dnr].drive_floppy->ReadOnly;
    } else {
	return drive[dnr].drive_floppy->ReadOnly;
    }
}

static void GCR_data_writeback(int dnr)
{
    int rc, extend, track, sector, max_sector;
    BYTE buffer[260], *offset;

    track = drive[dnr].current_half_track / 2;

    if (!drive[dnr].GCR_dirty_track)
	return;

    if (drive[dnr].drive_floppy->GCR_Header != 0) {
	write_track_gcr(track, dnr);
	drive[dnr].GCR_dirty_track = 0;
	return;
    }

    if (drive[dnr].drive_floppy->ImageFormat == 1541) {
        if (track > EXT_TRACKS_1541)
            return;
        max_sector = sector_map_1541[track];
        if (track > drive[dnr].drive_floppy->NumTracks) {
            switch (extend_image_policy) {
              case DRIVE_EXTEND_NEVER:
                ask_extend_disk_image = 1;
                return;
              case DRIVE_EXTEND_ASK:
                if (ask_extend_disk_image == 1) {
                    extend = ui_extend_image_dialog();
                    if (extend == 0) {
                        ask_extend_disk_image = 0;
                        return;
                    } else {
                        drive_extend_disk_image(dnr);
                    }
                } else {
                    return;
                }
                break;
              case DRIVE_EXTEND_ACCESS:
                ask_extend_disk_image = 1;
                drive_extend_disk_image(dnr);
                break;
            }
        }
    }

    if (drive[dnr].drive_floppy->ImageFormat == 1571) {
        if (track > MAX_TRACKS_1571)
            return;
        max_sector = sector_map_1571[track];
    }

    drive[dnr].GCR_dirty_track = 0;

    for (sector = 0; sector < max_sector; sector++) {

	offset = GCR_find_sector_header(track, sector, dnr);
	if (offset == NULL)
	    fprintf(stderr,
                    "DRIVE#%i: Could not find header of T:%d S:%d.\n",
                    dnr + 8, track, sector);
	else {

	    offset = GCR_find_sector_data(offset, dnr);
	    if (offset == NULL)
		fprintf(stderr,
		"DRIVE#%i: Could not find data sync of T:%d S:%d.\n",
		dnr + 8, track, sector);
	    else {

		convert_GCR_to_sector(buffer, offset, 
		    drive[dnr].GCR_track_start_ptr,
		    drive[dnr].GCR_current_track_size);
		if (buffer[0] != 0x7)
		    fprintf(stderr,
			"DRIVE#%i: Could not find data block id of T:%d S:%d.\n",
			dnr + 8, track, sector);
		else {
		    rc = floppy_write_block(drive[dnr].drive_floppy->ActiveFd,
                                drive[dnr].drive_floppy->ImageFormat,
                                buffer + 1, track, sector,
                                drive[dnr].drive_floppy->D64_Header);
		    if (rc < 0)
			fprintf(stderr,
			    "DRIVE#%i: Could not update T:%d S:%d.\n",
			    dnr + 8, track, sector);
		}
	    }
	}
    }
}

static void drive_extend_disk_image(int dnr)
{
    int rc, track, sector;
    BYTE buffer[256];

    drive[dnr].drive_floppy->NumTracks = EXT_TRACKS_1541;
    drive[dnr].drive_floppy->NumBlocks = EXT_BLOCKS_1541;
    memset(buffer, 0, 256);
    for (track = NUM_TRACKS_1541 + 1; track <= EXT_TRACKS_1541; track++) {
	for (sector = 0; sector < sector_map_1541[track]; sector++) {
	    rc = floppy_write_block(drive[dnr].drive_floppy->ActiveFd,
                            drive[dnr].drive_floppy->ImageFormat,
                            buffer, track, sector,
                            drive[dnr].drive_floppy->D64_Header);
	if (rc < 0)
	    fprintf(stderr,
	    "1541: Could not update T:%d S:%d.\n", track, sector);
	}
    }
}

void drive_update_zone_bits(int zone, int dnr)
{
    drive[dnr].rotation_table_ptr = drive[dnr].rotation_table[zone];
}

void drive_update_viad2_pcr(int pcrval, int dnr)
{
    drive[dnr].read_write_mode = pcrval & 0x20;
    drive[dnr].byte_ready_active = (drive[dnr].byte_ready_active & ~0x02)
                                     | (pcrval & 0x02);
}

void drive_motor_control(int flag, int dnr)
{
    drive[dnr].byte_ready_active = (drive[dnr].byte_ready_active & ~0x04)
                                     | (flag & 0x04);
}

BYTE drive_read_viad2_prb(int dnr)
{
    drive_rotate_disk(0, dnr);
    return (drive_sync_found(dnr) ? 0 : 0x80)
        | (drive_write_protect_sense(dnr) ? 0 : 0x10);
}

void drive_set_1571_side(int side, int dnr)
{
    int num = drive[dnr].current_half_track;
    drive_rotate_disk(0, dnr);
    drive[dnr].side = side;
    if (num > 70)
        num -= 70;
    num += side * 70;
printf("NUM: %i SIDE: %i\n",num,side);
    drive_set_half_track(num, dnr);
}

/* ------------------------------------------------------------------------- */

/* Handle a ROM trap. */
int drive0_trap_handler(void)
{
    if (MOS6510_REGS_GET_PC(&drive0_cpu_regs) == 0xec9b) {
	/* Idle loop */
	MOS6510_REGS_SET_PC(&drive0_cpu_regs, 0xebff);
	if (idling_method[0] == DRIVE_IDLE_TRAP_IDLE)
	    drive_clk[0] = next_alarm_clk(&drive0_int_status);
	} else
	    return 1;

    return 0;
}

int drive1_trap_handler(void)
{
    if (MOS6510_REGS_GET_PC(&drive1_cpu_regs) == 0xec9b) {
	/* Idle loop */
	MOS6510_REGS_SET_PC(&drive1_cpu_regs, 0xebff);
	if (idling_method[1] == DRIVE_IDLE_TRAP_IDLE)
	    drive_clk[1] = next_alarm_clk(&drive1_int_status);
	} else
	    return 1;

    return 0;
}

/* ------------------------------------------------------------------------- */

/* Set the sync factor between the computer and the 1541.  */

static void drive_set_sync_factor(unsigned int factor)
{
    drive0_cpu_set_sync_factor(drive[0].clock_frequency * factor);
    drive1_cpu_set_sync_factor(drive[1].clock_frequency * factor);
printf("Factor: %i\n",drive[0].clock_frequency);
}

static void drive_set_pal_sync_factor(void)
{
    if (pal_cycles_per_sec != 0.0) {
        int sync_factor = (int) floor(65536.0 * (1000000.0 /
                                         ((double)pal_cycles_per_sec)));
        drive_set_sync_factor(sync_factor);
    }
}

static void drive_set_ntsc_sync_factor(void)
{
    if (ntsc_cycles_per_sec != 0.0) {
        int sync_factor = (int) floor(65536.0 * (1000000.0 /
                                         ((double)ntsc_cycles_per_sec)));

        drive_set_sync_factor(sync_factor);
    }
}

void drive_set_1571_sync_factor(int sync, int dnr)
{
    if (rom_loaded) {
        drive_rotate_disk(0, dnr);
        initialize_rotation(sync ? 1 : 0, dnr);
        drive[dnr].clock_frequency = (sync) ? 2 : 1;
        set_sync_factor((resource_value_t) sync_factor);
    }
}

/* ------------------------------------------------------------------------- */

/* Update the status bar in the UI.  */
/* FIXME: We have only one drive LED.  */
static void drive_update_ui_status(void)
{
    int my_led_status;
    if (!drive_enabled[0]) {
        if (drive[0].old_led_status >= 0) {
            drive[0].old_led_status = drive[0].old_half_track = -1;
            ui_toggle_drive_status(0);
        }
        return;
    }

    /* Actually update the LED status only if the `trap idle' idling method
       is being used, as the LED status could be incorrect otherwise.  */
    if (idling_method[0] != DRIVE_IDLE_SKIP_CYCLES)
	/* FIXME: Hack to use one LED for both drives.  */
	my_led_status = (drive[0].led_status | drive[1].led_status) ? 1 : 0;
    else
	my_led_status = 0;

    if (my_led_status != drive[0].old_led_status) {
        ui_display_drive_led(my_led_status);
	drive[0].old_led_status = my_led_status;
    }

    /* FIXME: Only the track of drive #8 is shown.  */
    if (drive[0].current_half_track != drive[0].old_half_track) {
	drive[0].old_half_track = drive[0].current_half_track;
	ui_display_drive_track((float) drive[0].current_half_track / 2.0);
    }
}

/* This is called at every vsync.  */
void drive_vsync_hook(void)
{
    drive_update_ui_status();

    if (idling_method[0] != DRIVE_IDLE_SKIP_CYCLES && drive_enabled[0])
	drive0_cpu_execute();
    if (idling_method[1] != DRIVE_IDLE_SKIP_CYCLES && drive_enabled[1])
	drive1_cpu_execute();
}
