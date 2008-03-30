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

#ifdef STDC_HEADERS
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#endif

#include "drive.h"
#include "iecdrive.h"
#include "gcr.h"
#include "serial.h"
#include "vdrive.h"
#include "warn.h"
#include "mem.h"
#include "resources.h"
#include "cmdline.h"
#include "memutils.h"
#include "viad.h"
#include "via.h"
#include "ciad.h"
#include "cia.h"
#include "utils.h"
#include "ui.h"
#include "snapshot.h"

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

/* Is true drive emulation switched on?  */
static int drive_true_emulation;

/* Pointer to the IEC bus structure.  */
static iec_info_t *iec_info;

/* Flag: Do we emulate a SpeedDOS-compatible parallel cable?  */
/* FIXME: This will be moved to struct drive_t soon.  */
int drive_parallel_cable_enabled;

/* What extension policy?  (See `DRIVE_EXTEND_*' in `drive.h'.)  */
/* FIXME: This will be moved to struct drive_t soon.  */
static int extend_image_policy;

/* What sync factor between the CPU and the drive?  If equal to
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
static char *dos_rom_name_2031;

static int set_drive_true_emulation(resource_value_t v)
{
    drive_true_emulation = (int) v;
    if ((int) v) {
        if (drive[0].type != DRIVE_TYPE_NONE) {
            drive[0].enable = 1;
            drive_enable(0);
        }
        if (drive[1].type != DRIVE_TYPE_NONE) {
            drive[1].enable = 1;
            drive_enable(1);
        }
    } else {
            drive_disable(0);
            drive_disable(1);
    }
    return 0;
}

static int set_drive0_type(resource_value_t v)
{
    switch ((int) v) {
      case DRIVE_TYPE_1541:
      case DRIVE_TYPE_1571:
      case DRIVE_TYPE_1581:
      case DRIVE_TYPE_2031:
        drive[0].type = (int) v;
        if (drive_true_emulation) {
        drive[0].enable = 1;
        drive_enable(0);
        }
        drive_set_disk_drive_type((int) v, 0);
        return 0;
      case DRIVE_TYPE_NONE:
        drive[0].type = (int) v;
        drive_disable(0);
        return 0;
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
      case DRIVE_TYPE_2031:
        drive[1].type = (int) v;
        if (drive_true_emulation) {
        drive[1].enable = 1;
        drive_enable(1);
        }
        drive_set_disk_drive_type((int) v, 1);
        return 0;
      case DRIVE_TYPE_NONE:
        drive[1].type = (int) v;
        drive_disable(1);
        return 0;
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

    if (rom_loaded && drive[0].type == DRIVE_TYPE_1541) {
/*
        drive[0].rom[0xec9b - 0x8000] =
            (drive[0].idling_method != DRIVE_IDLE_TRAP_IDLE)
            ? 0x00 : drive[0].rom_idle_trap;
*/
    }
    drive[0].idling_method = (int) v;
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
            (drive[1].idling_method != DRIVE_IDLE_TRAP_IDLE)
            ? 0x00 : drive[1].rom_idle_trap;
    drive[1].idling_method = (int) v;
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

static int set_dos_rom_name_2031(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_2031 == NULL)
        dos_rom_name_2031 = stralloc(name);
    else {
        dos_rom_name_2031 = xrealloc(dos_rom_name_2031, strlen(name) + 1);
        strcpy(dos_rom_name_2031, name);
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
    { "DriveTrueEmulation", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &drive_true_emulation, set_drive_true_emulation },
    { "Drive8Type", RES_INTEGER, (resource_value_t) DRIVE_TYPE_1541,
      (resource_value_t *) &(drive[0].type), set_drive0_type },
    { "Drive9Type", RES_INTEGER, (resource_value_t) DRIVE_TYPE_NONE,
      (resource_value_t *) &(drive[1].type), set_drive1_type },
    { "DriveParallelCable", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &drive_parallel_cable_enabled, set_drive_parallel_cable_enabled },
    { "DriveExtendImagePolicy", RES_INTEGER, (resource_value_t) DRIVE_EXTEND_NEVER,
      (resource_value_t *) &extend_image_policy, set_extend_image_policy },
    { "Drive8IdleMethod", RES_INTEGER, (resource_value_t) DRIVE_IDLE_TRAP_IDLE,
      (resource_value_t *) &(drive[0].idling_method), set_drive0_idling_method },
    { "Drive9IdleMethod", RES_INTEGER, (resource_value_t) DRIVE_IDLE_TRAP_IDLE,
      (resource_value_t *) &(drive[1].idling_method), set_drive1_idling_method },
    { "DriveSyncFactor", RES_INTEGER, (resource_value_t) DRIVE_SYNC_PAL,
      (resource_value_t *) &sync_factor, set_sync_factor },
    { "DosName2031", RES_STRING, (resource_value_t) "dos2031",
      (resource_value_t *) &dos_rom_name_2031, set_dos_rom_name_2031 },
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
    { "-truedrive", SET_RESOURCE, 0, NULL, NULL, "DriveTrueEmulation",
      (resource_value_t) 1,
      NULL, "Enable hardware-level emulation of disk drives" },
    { "+truedrive", SET_RESOURCE, 0, NULL, NULL, "DriveTrueEmulation",
      (resource_value_t) 0,
      NULL, "Disable hardware-level emulation of disk drives" },
    { "-drive8type", SET_RESOURCE, DRIVE_TYPE_1541, NULL, NULL, "Drive8Type",
      NULL, "<type>", "Set drive type (0: no drive)" },
    { "-drive9type", SET_RESOURCE, DRIVE_TYPE_NONE, NULL, NULL, "Drive9Type",
      NULL, "<type>", "Set drive type (0: no drive)" },
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
      NULL, "<value>", "Set drive sync factor to <value>" },
    { "-paldrive", SET_RESOURCE, 0, NULL, NULL, "DriveSyncFactor",
      (resource_value_t) DRIVE_SYNC_PAL,
      NULL, "Use PAL drive sync factor" },
    { "-ntscdrive", SET_RESOURCE, 0, NULL, NULL, "DriveSyncFactor",
      (resource_value_t) DRIVE_SYNC_NTSC,
      NULL, "Use NTSC drive sync factor" },
    { "-dos1541", SET_RESOURCE, 1, NULL, NULL, "DosName1541", NULL,
      "<name>", "Specify name of 1541 DOS ROM image name" },
    { "-dos1571", SET_RESOURCE, 1, NULL, NULL, "DosName1571", NULL,
      "<name>", "Specify name of 1571 DOS ROM image name" },
    { "-dos1581", SET_RESOURCE, 1, NULL, NULL, "DosName1581", NULL,
      "<name>", "Specify name of 1581 DOS ROM image name" },
    { "-dos2031", SET_RESOURCE, 1, NULL, NULL, "DosName2031", NULL,
      "<name>", "Specify name of 2031 DOS ROM image name" },
    { NULL }
};

int drive_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* RAM/ROM.  */
#ifdef AVOID_STATIC_ARRAYS
static BYTE *drive_rom1541;
static BYTE *drive_rom1571;
static BYTE *drive_rom1581;
static BYTE *drive_rom2031;

BYTE *drive0_ram;
BYTE *drive1_ram;
#else
static BYTE drive_rom1541[DRIVE_ROM1541_SIZE];
static BYTE drive_rom1571[DRIVE_ROM1571_SIZE];
static BYTE drive_rom1581[DRIVE_ROM1581_SIZE];
static BYTE drive_rom2031[DRIVE_ROM2031_SIZE];

BYTE drive0_ram[DRIVE_RAM_SIZE];
BYTE drive1_ram[DRIVE_RAM_SIZE];
#endif

/* If nonzero, at least one vaild drive ROM has already been loaded.  */
static int rom_loaded = 0;

/* If nonzero, the ROM image has been loaded.  */
static int rom1541_loaded = 0;
static int rom1571_loaded = 0;
static int rom1581_loaded = 0;
static int rom2031_loaded = 0;

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
inline static BYTE drive_sync_found(int dnr);
inline static BYTE drive_write_protect_sense(int dnr);
static int drive_load_rom_images(void);
static void drive_setup_rom_image(int dnr);
static void drive_initialize_rom_traps(int dnr);
static int drive_write_image_snapshot_module(snapshot_t *s, int dnr);
static int drive_read_image_snapshot_module(snapshot_t *s, int dnr);

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
    if (drive[dnr].drive_floppy->ImageFormat == 1541
        && (drive[dnr].type == DRIVE_TYPE_1541
        || drive[dnr].type == DRIVE_TYPE_2031)) {
        for (track = 0; track < MAX_TRACKS_1541; track++) {
            drive[dnr].GCR_track_size[track] =
                raw_track_size[speed_map_1541[track]];
            memset(drive[dnr].GCR_speed_zone, speed_map_1541[track],
                NUM_MAX_BYTES_TRACK);
        }
    }
    if (drive[dnr].drive_floppy->ImageFormat == 1571
        || drive[dnr].type == DRIVE_TYPE_1571
        || drive[dnr].type == DRIVE_TYPE_2031) {
        for (track = 0; track < MAX_TRACKS_1571; track++) {
            drive[dnr].GCR_track_size[track] =
                raw_track_size[speed_map_1571[track]];
            memset(drive[dnr].GCR_speed_zone, speed_map_1571[track],
                NUM_MAX_BYTES_TRACK);
        }
    }

    drive_set_half_track(drive[dnr].current_half_track, dnr);

    for (track = 1; track <= drive[dnr].drive_floppy->NumTracks; track++) {
	int max_sector = 0;

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
	fprintf(stderr, "DRIVE#%i: Could not read GCR disk image.\n", dnr + 8);
	return 0;
    }

    lseek(drive[dnr].drive_floppy->ActiveFd, 12 + NumTracks * 8, SEEK_SET);
    if (read_dword(drive[dnr].drive_floppy->ActiveFd, gcr_speed_p,
        NumTracks * 8) < 0) {
	fprintf(stderr, "DRIVE#%i: Could not read GCR disk image.\n", dnr + 8);
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
		fprintf(stderr, "DRIVE#%i: Could not read GCR disk image.\n", dnr + 8);
		return 0;
	    }

	    track_len = len[0] + len[1] * 256;

	    if (track_len < 5000 || track_len > 7928) {
		fprintf(stderr, "DRIVE#%i: Track field length %i is not supported.\n",
            dnr + 8, track_len);
		return 0;
	    }

	    drive[dnr].GCR_track_size[track] = track_len;

	    lseek(drive[dnr].drive_floppy->ActiveFd, offset + 2, SEEK_SET);
	    if (read(drive[dnr].drive_floppy->ActiveFd, (char *)track_data,
            track_len)
			< track_len) {
		fprintf(stderr, "DRIVE#%i: Could not read GCR disk image.\n", dnr + 8);
		return 0;
	    }

	    zone_len = (track_len + 3) / 4;

	    if (gcr_speed_p[track * 2] > 3) {

		offset = gcr_speed_p[track * 2];

		lseek(drive[dnr].drive_floppy->ActiveFd, offset, SEEK_SET);
		if (read(drive[dnr].drive_floppy->ActiveFd, (char *)comp_speed,
			zone_len) < zone_len) {
		    fprintf(stderr, "DRIVE#%i: Could not read GCR disk image.\n",
		        dnr + 8);
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
	fprintf(stderr, "DRIVE#%i: Could not read GCR disk image header.\n",
	    dnr + 8);
	return;
    }

    lseek(drive[dnr].drive_floppy->ActiveFd, 12 + NumTracks * 8, SEEK_SET);
    if (read_dword(drive[dnr].drive_floppy->ActiveFd, gcr_speed_p,
        NumTracks * 8) < 0) {
	fprintf(stderr, "DRIVE#%i: Could not read GCR disk image header.\n",
	    dnr + 8);
	return;
    }

    if (gcr_track_p[(track - 1) * 2] == 0) {
	offset = lseek(drive[dnr].drive_floppy->ActiveFd, 0, SEEK_END);
	if (offset < 0) {
	    fprintf(stderr, "DRIVE#8%i: Could not extend GCR disk image.\n",
	        dnr + 8);
	    return;
	}
	gcr_track_p[(track - 1) * 2] = offset;
    }

    offset = gcr_track_p[(track - 1) * 2];

    len[0] = drive[dnr].GCR_track_size[track - 1] % 256;
    len[1] = drive[dnr].GCR_track_size[track - 1] / 256;

    if (lseek(drive[dnr].drive_floppy->ActiveFd, offset, SEEK_SET) < 0
        || write(drive[dnr].drive_floppy->ActiveFd, (char *)len, 2) < 0) {
	fprintf(stderr, "DRIVE%i: Could not write GCR disk image.\n",
	    dnr + 8);
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
	fprintf(stderr, "DRIVE#%i: Could not write GCR disk image.\n",
	    dnr + 8);
	return;
    }

    for (i = 0; (drive[dnr].GCR_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK]
	    == drive[dnr].GCR_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK + i])
	    && i < NUM_MAX_BYTES_TRACK; i++);

    if (i < drive[dnr].GCR_track_size[track - 1]) {
	/* This will change soon.  */
	fprintf(stderr, "DRIVE#%i: Saving different speed zones is not "
                "supported yet.\n", dnr + 8);
	return;
    }

    if (gcr_speed_p[(track - 1) * 2] >= 4) {
	/* This will change soon.  */
	fprintf(stderr, "DRIVE#%i: Adding new speed zones is not "
                "supported yet.\n", dnr + 8);
	return;
    }

    offset = 12 + NumTracks * 8 + (track - 1) * 8;
    if (lseek(drive[dnr].drive_floppy->ActiveFd, offset, SEEK_SET) < 0
        || write_dword(drive[dnr].drive_floppy->ActiveFd,
           &gcr_speed_p[(track - 1) * 2], 4) < 0) {
    fprintf(stderr, "DRIVE#%i: Could not write GCR disk image.\n", dnr + 8);
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
        fprintf(stderr, "DRIVE#%i: Could not write GCR disk image", dnr + 8);
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

/* Initialize the hardware-level drive emulation (should be called at least
   once before anything else).  Return 0 on success, -1 on error.  */
int drive_init(CLOCK pal_hz, CLOCK ntsc_hz)
{
    int track, i;

    pal_cycles_per_sec = pal_hz;
    ntsc_cycles_per_sec = ntsc_hz;

    if (rom_loaded)
	return 0;

#ifdef AVOID_STATIC_ARRAYS
    drive_rom1541 = xmalloc(DRIVE_ROM1541_SIZE);
    drive_rom1571 = xmalloc(DRIVE_ROM1571_SIZE);
    drive_rom1581 = xmalloc(DRIVE_ROM1581_SIZE);
    drive_rom2031 = xmalloc(DRIVE_ROM2031_SIZE);

    drive0_ram = xmalloc(DRIVE_RAM_SIZE);
    drive1_ram = xmalloc(DRIVE_RAM_SIZE);
#endif

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
	drive[i].GCR_image_loaded = 0;
	drive[i].read_only = 0;
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
    if (drive[0].enable)
	drive_enable(0);
    if (drive[1].enable)
	drive_enable(1);

    return 0;
}

static int drive_set_disk_drive_type(int type, int dnr)
{
    switch (type) {
      case DRIVE_TYPE_1541:
        if (rom1541_loaded < 1 && rom_loaded)
            return -1;
        if (drive[dnr].byte_ready_active == 0x06)
            drive_rotate_disk(dnr);
        drive[dnr].clock_frequency = 1;
        break;
      case DRIVE_TYPE_1571:
        if (rom1571_loaded < 1 && rom_loaded)
            return -1;
        if (drive[dnr].byte_ready_active == 0x06)
            drive_rotate_disk(dnr);
        drive[dnr].clock_frequency = 1;
        break;
      case DRIVE_TYPE_1581:
        if (rom1581_loaded < 1 && rom_loaded)
            return -1;
        if (drive[dnr].byte_ready_active == 0x06)
            drive_rotate_disk(dnr);
        drive[dnr].clock_frequency = 2;
        break;
      case DRIVE_TYPE_2031:
        if (rom2031_loaded < 1 && rom_loaded)
            return -1;
        if (drive[dnr].byte_ready_active == 0x06)
            drive_rotate_disk(dnr);
        drive[dnr].clock_frequency = 1;
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

    if (mem_load_sys_file(dos_rom_name_2031, drive_rom2031, DRIVE_ROM2031_SIZE,
                          DRIVE_ROM2031_SIZE) < 0) {
        fprintf(stderr,
                "Drive: Warning: 2031 ROM image not found.\n"
                "Drive: Hardware-level 2031 emulation is not available.\n");
    } else
        rom2031_loaded = 1;

    /* FIXME: Drive type radio button should be made insensitive here
       if a ROM image is not loaded. */

    if ((rom1541_loaded | rom1571_loaded | rom1581_loaded 
						| rom2031_loaded) < 1) {
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
          case DRIVE_TYPE_2031:
            memcpy(&(drive[dnr].rom[0x4000]), drive_rom2031, DRIVE_ROM2031_SIZE);
            break;
        }
    }
}

static void drive_initialize_rom_traps(int dnr)
{
    if (drive[dnr].type == DRIVE_TYPE_1541) {
        /* Remove the ROM check.  */
/*
        drive[dnr].rom[0xeae4 - 0x8000] = 0xea;
        drive[dnr].rom[0xeae5 - 0x8000] = 0xea;
        drive[dnr].rom[0xeae8 - 0x8000] = 0xea;
        drive[dnr].rom[0xeae9 - 0x8000] = 0xea;
*/
        /* Trap the idle loop.  */
/*
        drive[dnr].rom_idle_trap = drive[dnr].rom[0xec9b - 0x8000];
        if (drive[dnr].idling_method == DRIVE_IDLE_TRAP_IDLE)
        drive[dnr].rom[0xec9b - 0x8000] = 0x00;
*/
   }
}

/* Activate full drive emulation. */
static int drive_enable(int dnr)
{
    int i;

    /* This must come first, because this might be called before the drive
       initialization.  */
    if (!rom_loaded)
        return -1;

    /* Always disable kernal traps. */
    if (drive_true_emulation)
        serial_remove_traps();
    else
        return 0;

    if (drive[dnr].type == DRIVE_TYPE_NONE)
        return 0;

    if (drive[dnr].drive_floppy != NULL)
	drive_attach_floppy(drive[dnr].drive_floppy);

    if (dnr == 0)
	drive0_cpu_wake_up();
    if (dnr == 1)
	drive1_cpu_wake_up();

    /* Make sure the UI is updated.  */
    for (i = 0; i < 2; i++) {
        if (drive[i].enable) {
            drive[i].old_led_status = -1;
            drive[i].old_half_track = -1;
        }
    }

    ui_enable_drive_status((drive[0].enable ? UI_DRIVE_ENABLE_0 : 0)
                           | (drive[1].enable ? UI_DRIVE_ENABLE_1 : 0));

    return 0;
}

/* Disable full drive emulation.  */
static void drive_disable(int dnr)
{
    int i;

    /* This must come first, because this might be called before the true
       drive initialization.  */
    drive[dnr].enable = 0;

    if (rom_loaded && !drive_true_emulation)
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

    /* Make sure the UI is updated.  */
    for (i = 0; i < 2; i++) {
        if (drive[i].enable) {
            drive[i].old_led_status = -1;
            drive[i].old_half_track = -1;
        }
    }

    ui_enable_drive_status((drive[0].enable ? UI_DRIVE_ENABLE_0 : 0)
                           | (drive[1].enable ? UI_DRIVE_ENABLE_1 : 0));
}

void drive_reset(void)
{
    drive0_cpu_reset();
    drive1_cpu_reset();
    warn_reset(drive_warn);
}

/* ------------------------------------------------------------------------- */

/* Attach a disk image to the true drive emulation. */
int drive_attach_floppy(DRIVE *floppy)
{
    int dnr;

    if (floppy->ImageFormat != 1541 && floppy->ImageFormat != 1571)
	return -1;

    if (floppy->unit != 8 && floppy->unit != 9)
	return -1;

    dnr = floppy->unit - 8;

    drive[dnr].drive_floppy = floppy;
    drive[dnr].read_only = drive[dnr].drive_floppy->ReadOnly;
    drive[dnr].have_new_disk = 1;
    drive[dnr].attach_clk = drive_clk[dnr];
    ask_extend_disk_image = 1;

    if (drive[dnr].drive_floppy->GCR_Header != 0) {
        if (!drive_read_image_gcr(dnr))
            return -1;
    } else {
	if (setID(dnr) >= 0) {
	    drive_read_image_d64_d71(dnr);
	    drive[dnr].GCR_image_loaded = 1;
	    return 0;
	} else {
	    return -1;
	}
    }
    drive[dnr].GCR_image_loaded = 1;
    return 0;
}

/* Detach a disk image from the true drive emulation. */
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
	drive[dnr].GCR_image_loaded = 0;
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

/* Rotate the disk according to the current value of `drive_clk[]'.  If
   `mode_change' is non-zero, there has been a Read -> Write mode switch.  */
void drive_rotate_disk(int dnr)
{
    unsigned long new_bits;

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
/*printf("RD: %x\t%i\t%i\n",drive[dnr].GCR_read,drive[dnr].GCR_head_offset,drive_clk[0]);*/
	}

    /* The byte ready line is only set when no sync is found.  */
	if (drive_sync_found(dnr))
	    drive[dnr].byte_ready = 1;
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
	if (drive[dnr].byte_ready_active == 0x06)
	    drive_rotate_disk(dnr);
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
    if (drive[dnr].attach_clk != (CLOCK)0) {
        if (drive_clk[dnr] - drive[dnr].attach_clk < DRIVE_ATTACH_DELAY)
            return 0;
        drive[dnr].attach_clk = (CLOCK)0;
    }

    if (drive[dnr].byte_ready_active == 0x06)
        drive_rotate_disk(dnr);
    return drive[dnr].GCR_read;
}

/* Return non-zero if the Sync mark is found.  It is required to
   call drive_rotate_disk() to update drive[].GCR_head_offset first.
   The return value corresponds to bit#7 of VIA2 PRB. This means 0x0
   is returned when sync is found and 0x80 is returned when no sync
   is found.  */
inline static BYTE drive_sync_found(int dnr)
{
    BYTE val = drive[dnr].GCR_track_start_ptr[drive[dnr].GCR_head_offset];

    if (val != 0xff || drive[dnr].last_mode == 0) {
        return 0x80;
    } else {
	int next_head_offset = (drive[dnr].GCR_head_offset > 0
				? drive[dnr].GCR_head_offset - 1
				: drive[dnr].GCR_current_track_size - 1);

	if (drive[dnr].GCR_track_start_ptr[next_head_offset] != 0xff)
	    return 0x80;

	/* As the current rotation code cannot cope with non byte aligned
	   writes, do not change `drive[].bits_moved'!  */
	/* drive[dnr].bits_moved = 0; */
	return 0x0;
    }
}

/* Move the head to half track `num'.  */
static void drive_set_half_track(int num, int dnr)
{
    if ((drive[dnr].type == DRIVE_TYPE_1541
        || drive[dnr].type == DRIVE_TYPE_2031) && num > 84)
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
}

/* Write one GCR byte to the disk. */
void drive_write_gcr(BYTE val, int dnr)
{
    if (drive[dnr].byte_ready_active == 0x06)
        drive_rotate_disk(dnr);
    drive[dnr].GCR_write_value = val;
}

/* Return the write protect sense status. */
inline static BYTE drive_write_protect_sense(int dnr)
{
    /* Toggle the write protection bit if the disk was detached.  */
    if (drive[dnr].detach_clk != (CLOCK)0) {
	if (drive_clk[dnr] - drive[dnr].detach_clk < DRIVE_DETACH_DELAY)
	    return 0x10;
	drive[dnr].detach_clk = (CLOCK)0;
    }
    if ((drive[dnr].attach_clk != (CLOCK)0) &&
	(drive_clk[dnr] - drive[dnr].attach_clk < DRIVE_ATTACH_DELAY))
	return 0x10;
    if (drive[dnr].GCR_image_loaded == 0) {
	/* No disk in drive, write protection is on. */
	return 0x0;
    } else if (drive[dnr].have_new_disk) {
	/* Disk has changed, make sure the drive sees at least one change in
	   the write protect status. */
	drive[dnr].have_new_disk = 0;
	return drive[dnr].read_only ? 0x10 : 0x0;
    } else {
	return drive[dnr].read_only ? 0x0 : 0x10;
    }
}

static void GCR_data_writeback(int dnr)
{
    int rc, extend, track, sector, max_sector = 0;
    BYTE buffer[260], *offset;

    if (drive[dnr].drive_floppy == NULL)
        return;

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
	    "DRIVE#%i: Could not update T:%d S:%d.\n", dnr + 8, track, sector);
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
    if (drive[dnr].byte_ready_active == 0x06)
        drive_rotate_disk(dnr);
    return drive_sync_found(dnr) | drive_write_protect_sense(dnr);
}

void drive_set_1571_side(int side, int dnr)
{
    int num = drive[dnr].current_half_track;
    if (drive[dnr].byte_ready_active == 0x06)
        drive_rotate_disk(dnr);
    GCR_data_writeback(dnr);
    drive[dnr].side = side;
    if (num > 70)
        num -= 70;
    num += side * 70;
    drive_set_half_track(num, dnr);
}

/* ------------------------------------------------------------------------- */

/* Handle a ROM trap. */
int drive0_trap_handler(void)
{
    if (MOS6510_REGS_GET_PC(&drive0_cpu_regs) == 0xec9b) {
	/* Idle loop */
	MOS6510_REGS_SET_PC(&drive0_cpu_regs, 0xebff);
	if (drive[0].idling_method == DRIVE_IDLE_TRAP_IDLE)
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
	if (drive[1].idling_method == DRIVE_IDLE_TRAP_IDLE)
	    drive_clk[1] = next_alarm_clk(&drive1_int_status);
	} else
	    return 1;

    return 0;
}

/* ------------------------------------------------------------------------- */

/* Set the sync factor between the computer and the drive.  */

static void drive_set_sync_factor(unsigned int factor)
{
    drive0_cpu_set_sync_factor(drive[0].clock_frequency * factor);
    drive1_cpu_set_sync_factor(drive[1].clock_frequency * factor);
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
        if (drive[dnr].byte_ready_active == 0x06)
            drive_rotate_disk(dnr);
        initialize_rotation(sync ? 1 : 0, dnr);
        drive[dnr].clock_frequency = (sync) ? 2 : 1;
        set_sync_factor((resource_value_t) sync_factor);
    }
}

/* ------------------------------------------------------------------------- */

/* Update the status bar in the UI.  */
void drive_update_ui_status(void)
{
    int i;

    /* Update the LEDs and the track indicators.  */
    for (i = 0; i < 2; i++) {
        if (drive[i].enable) {
            int my_led_status = 0;

            /* Actually update the LED status only if the `trap idle'
               idling method is being used, as the LED status could be
               incorrect otherwise.  */

            if (drive[i].idling_method != DRIVE_IDLE_SKIP_CYCLES)
                my_led_status = drive[i].led_status;

            if (my_led_status != drive[i].old_led_status) {
                ui_display_drive_led(i, my_led_status);
                drive[i].old_led_status = my_led_status;
            }

            if (drive[i].current_half_track != drive[i].old_half_track) {
                drive[i].old_half_track = drive[i].current_half_track;
                ui_display_drive_track(i,
                                       ((float) drive[i].current_half_track
                                        / 2.0));
            }
        }
    }
}

/* This is called at every vsync.  */
void drive_vsync_hook(void)
{
    drive_update_ui_status();

    if (drive[0].idling_method != DRIVE_IDLE_SKIP_CYCLES && drive[0].enable)
	drive0_cpu_execute();
    if (drive[1].idling_method != DRIVE_IDLE_SKIP_CYCLES && drive[1].enable)
	drive1_cpu_execute();
}

/* ------------------------------------------------------------------------- */

/*

This is the format of the DRIVE snapshot module.

Name               Type   Size   Description

Accum              DWORD  2
AttachClk          CLOCK  2      write protect handling on attach
BitsMoved          DWORD  2      number of bits moved since last access
ByteReady          BYTE   2      flag: Byte ready
ClockFrequency     BYTE   2      current clock frequency
CurrentHalfTrack   WORD   2      current half track of the r/w head
DetachClk          CLOCK  2      write protect handling on detach
DiskID1            BYTE   2      disk ID1
DiskID2            BYTE   2      disk ID2
FinishByte         BYTE   2      flag: Mode changed, finish byte
GCRHeadOffset      DWORD  2      offset from the begin of the track
GCRImage           BYTE   2      flag: gcr image attached
GCRRead            BYTE   2      next value to read from disk
GCRWriteValue      BYTE   2      next value to write to disk
HaveNewDisk        BYTE   2      flag: A new disk is inserted
LastMode           BYTE   2      flag: Was the last mode read or write
ReadOnly           BYTE   2      flag: This disk is read only
RotationLastClk    CLOCK  2
RotationTablePtr   DWORD  2      pointer to the rotation table 
                                 (offset to the rotation table is saved)
Type               DWORD  2      drive type

*/

#define SNAP_MAJOR 0
#define SNAP_MINOR 0

int drive_write_snapshot_module(snapshot_t *s)
{
    int i;
    char snap_module_name[] = "DRIVE";
    snapshot_module_t *m;
    DWORD rotation_table_ptr[2];
    BYTE GCR_image[2];

    /* Save changes to disk before taking a snapshot.  */
    GCR_data_writeback(0);
    GCR_data_writeback(1);

    for (i = 0; i < 2; i++) {
        rotation_table_ptr[i] = (DWORD) (drive[i].rotation_table_ptr
            - drive[i].rotation_table[0]);
        GCR_image[i] = (drive[i].GCR_image_loaded == 0) ? 0 : 1;
    }

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
        return -1;

    if (snapshot_module_write_byte(m, (BYTE) drive_true_emulation) < 0) {
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
            || snapshot_module_write_byte(m, (BYTE) drive[i].finish_byte) < 0
            || snapshot_module_write_dword(m, (DWORD) drive[i].GCR_head_offset) < 0
            || snapshot_module_write_byte(m, (BYTE) GCR_image[i]) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].GCR_read) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].GCR_write_value) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].have_new_disk) < 0
            || snapshot_module_write_byte(m, (BYTE) drive[i].last_mode) < 0
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
        if (drive0_cpu_write_snapshot_module(s) < 0)
            return -1;
        if (drive[0].type == DRIVE_TYPE_1541
            || drive[0].type == DRIVE_TYPE_2031) {
            if (via1d0_write_snapshot_module(s) < 0
                || via2d0_write_snapshot_module(s) < 0)
                return -1;
        }
        if (drive[0].type == DRIVE_TYPE_1571) {
            if (via1d0_write_snapshot_module(s) < 0
                || via2d0_write_snapshot_module(s) < 0
                || cia1571d0_write_snapshot_module(s) < 0)
                return -1;
        }
        if (drive[0].type == DRIVE_TYPE_1581) {
            if (cia1581d0_write_snapshot_module(s) < 0)
                return -1;
        }
    }
    if (drive[1].enable) {
        if (drive1_cpu_write_snapshot_module(s) < 0)
            return -1;
        if (drive[1].type == DRIVE_TYPE_1541
            || drive[1].type == DRIVE_TYPE_2031) {
            if (via1d1_write_snapshot_module(s) < 0
                || via2d1_write_snapshot_module(s) < 0)
                return -1;
        }
        if (drive[1].type == DRIVE_TYPE_1571) {
            if (via1d1_write_snapshot_module(s) < 0
                || via2d1_write_snapshot_module(s) < 0
                || cia1571d1_write_snapshot_module(s) < 0)
                return -1;
        }
        if (drive[1].type == DRIVE_TYPE_1581) {
            if (cia1581d1_write_snapshot_module(s) < 0)
                return -1;
        }
    }
    if (GCR_image[0] > 0) {
        if (drive_write_image_snapshot_module(s, 0) < 0)
            return -1;
    }
    if (GCR_image[1] > 0) {
        if (drive_write_image_snapshot_module(s, 1) < 0)
            return -1;
    }
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

int drive_read_snapshot_module(snapshot_t *s)
{
    BYTE major_version, minor_version;
    int i;
    snapshot_module_t *m;
    char snap_module_name[] = "DRIVE";
    DWORD rotation_table_ptr[2];
    BYTE GCR_image[2];

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);
    if (m == NULL)
        return -1;

    if (major_version > SNAP_MAJOR || minor_version > SNAP_MINOR) {
        fprintf(stderr,
                "DRIVE: Snapshot module version (%d.%d) newer than %d.%d.\n",
                major_version, minor_version,
                SNAP_MAJOR, SNAP_MINOR);
    }

    if (read_byte_into_int(m, &drive_true_emulation) < 0) {
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
            || read_byte_into_int(m, &drive[i].finish_byte) < 0
            || snapshot_module_read_dword(m, &drive[i].GCR_head_offset) < 0
            || snapshot_module_read_byte(m, &GCR_image[i]) < 0
            || snapshot_module_read_byte(m, &drive[i].GCR_read) < 0
            || snapshot_module_read_byte(m, &drive[i].GCR_write_value) < 0
            || read_byte_into_int(m, &drive[i].have_new_disk) < 0
            || read_byte_into_int(m, &drive[i].last_mode) < 0
            || read_byte_into_int(m, &drive[i].read_only) < 0
            || snapshot_module_read_dword(m, &drive[i].rotation_last_clk) < 0
            || snapshot_module_read_dword(m, &rotation_table_ptr[i]) < 0
            || snapshot_module_read_dword(m, &drive[i].type) < 0
        ) {
            if (m != NULL)
                snapshot_module_close(m);
            return -1;
        }
    }
    snapshot_module_close(m);

    /* Switch drives on/off if necessary.  */
    set_drive_true_emulation((resource_value_t) drive_true_emulation);
 
    for (i = 0; i < 2; i++) {
        drive[i].rotation_table_ptr = drive[i].rotation_table[0]
            + rotation_table_ptr[i];
        drive[i].GCR_track_start_ptr = (drive[i].GCR_data
                           + ((drive[i].current_half_track / 2 - 1)
                              * NUM_MAX_BYTES_TRACK));
        if (drive[i].type != DRIVE_TYPE_1571) {
            if (drive[i].type == DRIVE_TYPE_1581)
                drive_set_1571_sync_factor(1, i);
            else {
                drive_set_1571_sync_factor(0, i);
                drive[i].side = 0;
            }
        }
        drive_initialize_rom_traps(i);
    }

    if (drive[0].enable) {
        if (drive0_cpu_read_snapshot_module(s) < 0)
            return -1;
        if (drive[0].type == DRIVE_TYPE_1541
            || drive[0].type == DRIVE_TYPE_2031) {
            if (via1d0_read_snapshot_module(s) < 0
                || via2d0_read_snapshot_module(s) < 0)
                return -1;
        }
        if (drive[0].type == DRIVE_TYPE_1571) {
            if (via1d0_read_snapshot_module(s) < 0
                || via2d0_read_snapshot_module(s) < 0
                || cia1571d0_read_snapshot_module(s) < 0)
                return -1;
        }
        if (drive[0].type == DRIVE_TYPE_1581) {
            if (cia1581d0_read_snapshot_module(s) < 0)
                return -1;
        }
    }
    if (drive[1].enable) {
        if (drive1_cpu_read_snapshot_module(s) < 0)
            return -1;
        if (drive[1].type == DRIVE_TYPE_1541
            || drive[1].type == DRIVE_TYPE_2031) {
            if (via1d1_read_snapshot_module(s) < 0
                || via2d1_read_snapshot_module(s) < 0)
                return -1;
        }
        if (drive[1].type == DRIVE_TYPE_1571) {
            if (via1d1_read_snapshot_module(s) < 0
                || via2d1_read_snapshot_module(s) < 0
                || cia1571d1_read_snapshot_module(s) < 0)
                return -1;
        }
        if (drive[1].type == DRIVE_TYPE_1581) {
            if (cia1581d1_read_snapshot_module(s) < 0)
                return -1;
        }
    }
    if (GCR_image[0] > 0) {
        if (drive_read_image_snapshot_module(s, 0) < 0)
            return -1;
    }
    if (GCR_image[1] > 0) {
        if (drive_read_image_snapshot_module(s, 1) < 0)
            return -1;
    }

    drive[0].old_half_track = 36;
    drive[0].old_led_status = 0;
    drive[1].old_half_track = 36;
    drive[1].old_led_status = 0;
    drive_update_ui_status();

    return 0;
}

#define SNAP_MAJOR 0
#define SNAP_MINOR 0

static int drive_write_image_snapshot_module(snapshot_t *s, int dnr)
{
    char snap_module_name[10];
    snapshot_module_t *m;
    BYTE *tmpbuf;
    int i;

    sprintf(snap_module_name, "GCRIMAGE%i", dnr);

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
       return -1;

    tmpbuf = xmalloc(MAX_TRACKS_1571 * 4);

    for (i = 0; i < MAX_TRACKS_1571; i++) {
        tmpbuf[i * 4] = drive[dnr].GCR_track_size[i] & 0xff;
        tmpbuf[i * 4 + 1] = (drive[dnr].GCR_track_size[i] >> 8) & 0xff;
        tmpbuf[i * 4 + 2] = (drive[dnr].GCR_track_size[i] >> 16) & 0xff;
        tmpbuf[i * 4 + 3] = (drive[dnr].GCR_track_size[i] >> 24) & 0xff;
    }

    if (0
        || snapshot_module_write_byte_array(m, drive[dnr].GCR_data,
            sizeof(drive[dnr].GCR_data)) < 0
        || snapshot_module_write_byte_array(m, drive[dnr].GCR_speed_zone,
            sizeof(drive[dnr].GCR_speed_zone)) < 0
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

static int drive_read_image_snapshot_module(snapshot_t *s, int dnr)
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
        return -1;

    if (major_version > SNAP_MAJOR || minor_version > SNAP_MINOR) {
        fprintf(stderr,
                "DRIVE: Snapshot module version (%d.%d) newer than %d.%d.\n",
                major_version, minor_version,
                SNAP_MAJOR, SNAP_MINOR);
    }

    tmpbuf = xmalloc(MAX_TRACKS_1571 * 4);

    if (0
        || snapshot_module_read_byte_array(m, drive[dnr].GCR_data,
            sizeof(drive[dnr].GCR_data)) < 0
        || snapshot_module_read_byte_array(m, drive[dnr].GCR_speed_zone,
            sizeof(drive[dnr].GCR_speed_zone)) < 0
        || snapshot_module_read_byte_array(m, tmpbuf, MAX_TRACKS_1571 * 4) < 0
        ) {
        if (m != NULL)
            snapshot_module_close(m);
        free(tmpbuf);
        return -1;
    } 
    snapshot_module_close(m);

    for (i = 0; i < MAX_TRACKS_1571; i++)
        drive[dnr].GCR_track_size[i] = tmpbuf[i * 4] + (tmpbuf[i * 4 + 1] << 8)
            + (tmpbuf[i * 4 + 2] << 16) + (tmpbuf[i * 4 + 3] << 24);

    free(tmpbuf);
    drive[dnr].GCR_image_loaded = 1;
    drive[dnr].drive_floppy = NULL;
    return 0;
}

