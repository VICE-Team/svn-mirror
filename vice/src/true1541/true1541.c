/*
 * true1541.c - Hardware-level Commodore 1541 disk drive emulation.
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
	- support for GCR encoded image files.
	- check for byte ready *within* `BVC', `BVS' and `PHP'.
	- serial bus handling might be faster.  */

#define __1541__

#include "vice.h"

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "true1541.h"
#include "gcr.h"
#include "interrupt.h"
#include "vmachine.h"
#include "serial.h"
#include "drive.h"
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

/* Flag: Is the true 1541 emulation turned on?  */
int true1541_enabled;

/* Flag: Do we emulate a SpeedDOS-compatible parallel cable?  */
int true1541_parallel_cable_enabled;

/* What extension policy?  (See `TRUE1541_EXTEND_*' in `true1541.h'.)  */
static int extend_image_policy;

/* What idling method?  (See `TRUE1541_IDLE_*' in `true1541.h'.)  */
static int idling_method;

/* Original ROM code is saved here.  */
static BYTE true1541_rom_idle_trap;

/* What sync factor between the CPU and the 1541?  If equal to
   `TRUE1541_SYNC_PAL', the same as PAL machines.  If equal to
   `TRUE1541_SYNC_NTSC', the same as NTSC machines.  The sync factor is
   calculated as

   65536 * clk_1541 / clk_[c64|vic20]

   where `clk_1541' is fixed to 1 MHz, while `clk_[c64|vic20]' depends on the
   video timing (PAL or NTSC).  The pre-calculated values for PAL and NTSC
   are in `pal_sync_factor' and `ntsc_sync_factor'.  */
static int sync_factor;

/* Name of the DOS ROM.  */
static char *dos_rom_name;

static int set_true1541_enabled(resource_value_t v)
{
    if ((int) v)
        true1541_enable();
    else
        true1541_disable();
    return 0;
}

static int set_true1541_parallel_cable_enabled(resource_value_t v)
{
    true1541_parallel_cable_enabled = (int) v;
    return 0;
}

static int set_extend_image_policy(resource_value_t v)
{
    switch ((int) v) {
      case TRUE1541_EXTEND_NEVER:
      case TRUE1541_EXTEND_ASK:
      case TRUE1541_EXTEND_ACCESS:
        extend_image_policy = (int) v;
        return 0;
      default:
        return -1;
    }
}

static int set_idling_method(resource_value_t v)
{
    /* FIXME: Maybe we should call `true1541_cpu_execute()' here?  */
    if ((int) v != TRUE1541_IDLE_SKIP_CYCLES
        && (int) v != TRUE1541_IDLE_TRAP_IDLE
        && (int) v != TRUE1541_IDLE_NO_IDLE)
        return -1;

    if (rom_loaded)
	true1541_rom[0xec9b - 0xc000] = 
	    (idling_method != TRUE1541_IDLE_TRAP_IDLE)
	    ? 0x00 : true1541_rom_idle_trap;
    idling_method = (int) v;
    return 0;
}

static int set_sync_factor(resource_value_t v)
{
    switch ((int) v) {
      case TRUE1541_SYNC_PAL:
        sync_factor = (int) v;
        true1541_set_pal_sync_factor();
        break;
      case TRUE1541_SYNC_NTSC:
        sync_factor = (int) v;
        true1541_set_ntsc_sync_factor();
        break;
      default:
        if ((int) v > 0) {
            sync_factor = (int) v;
            true1541_set_sync_factor((int) v);
        } else {
            return -1;
        }
    }

    return 0;
}

static int set_dos_rom_name(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name == NULL)
        dos_rom_name = stralloc(name);
    else {
        dos_rom_name = xrealloc(dos_rom_name, strlen(name) + 1);
        strcpy(dos_rom_name, name);
    }
    return 0;
}

static resource_t resources[] = {
    { "True1541", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &true1541_enabled, set_true1541_enabled },
    { "True1541ParallelCable", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &true1541_parallel_cable_enabled, set_true1541_parallel_cable_enabled },
    { "True1541ExtendImagePolicy", RES_INTEGER, (resource_value_t) TRUE1541_EXTEND_NEVER,
      (resource_value_t *) &extend_image_policy, set_extend_image_policy },
    { "True1541IdleMethod", RES_INTEGER, (resource_value_t) TRUE1541_IDLE_TRAP_IDLE,
      (resource_value_t *) &idling_method, set_idling_method },
    { "True1541SyncFactor", RES_INTEGER, (resource_value_t) TRUE1541_SYNC_PAL,
      (resource_value_t *) &sync_factor, set_sync_factor },
    { "DosName", RES_STRING, (resource_value_t) "dos1541",
      (resource_value_t *) &dos_rom_name, set_dos_rom_name },
    { NULL }
};

int true1541_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* FIXME: Maybe DosName should not be changed by `-dosname'?  */

static cmdline_option_t cmdline_options[] = {
    { "-1541", SET_RESOURCE, 0, NULL, NULL, "True1541", (resource_value_t) 1,
      NULL, "Enable hardware-level 1541 emulation" },
    { "+1541", SET_RESOURCE, 0, NULL, NULL, "True1541", (resource_value_t) 0,
      NULL, "Disable hardware-level 1541 emulation" },
    { "-parallel", SET_RESOURCE, 0, NULL, NULL, "True1541ParallelCable",
      (resource_value_t) 1,
      NULL, "Enable SpeedDOS-compatible parallel cable" },
    { "+parallel", SET_RESOURCE, 0, NULL, NULL, "True1541ParallelCable",
      (resource_value_t) 0,
      NULL, "Disable SpeedDOS-compatible parallel cable" },
    { "-driveidle", SET_RESOURCE, 2, NULL, NULL, "True1541IdleMethod",
      NULL, "<method>",
      "Set 1541 idling method (0: no traps, 1: skip cycles, 2: trap idle)" },
    { "-drivesync", SET_RESOURCE, 1, NULL, NULL, "True1541SyncFactor",
      NULL, "<value>", "Set 1541 sync factor to <value>" },
    { "-paldrive", SET_RESOURCE, 0, NULL, NULL, "True1541SyncFactor",
      (resource_value_t) TRUE1541_SYNC_PAL,
      NULL, "Use PAL 1541 sync factor" },
    { "-ntscdrive", SET_RESOURCE, 0, NULL, NULL, "True1541SyncFactor",
      (resource_value_t) TRUE1541_SYNC_NTSC,
      NULL, "Use NTSC 1541 sync factor" },
    { "-dosname", SET_RESOURCE, 1, NULL, NULL, "DosName", NULL,
      "<name>", "Specify name of 1541 DOS ROM image name" },
    { NULL }
};

int true1541_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

#define NUM_BYTES_SECTOR_GCR 360
#define NUM_MAX_BYTES_TRACK 7928

/* RAM/ROM.  */
BYTE true1541_rom[TRUE1541_ROM_SIZE];
BYTE true1541_ram[TRUE1541_RAM_SIZE];

/* LED status (zero = off).  */
int true1541_led_status;

/* Current half track on which the R/W head is positioned.  */
int true1541_current_half_track = 36;

/* If nonzero, the 1541 ROM has already been loaded.  */
static int rom_loaded = 0;

static int init_complete = 0;
static int byte_ready = 1;

/* Pointer to the attached disk image.  */
static DRIVE *true1541_floppy;

/* Disk ID.  */
static BYTE diskID1, diskID2;

/* Map of the sector sizes.  */
extern char sector_map[43];

/* Speed zone of each track.  */
extern int speed_map[42];

/* Flag: does the current need to be written out to disk?  */
static int GCR_dirty_track = 0;

/* GCR value being written to the disk.  */
static BYTE GCR_write_value = 0x55;

/* Raw GCR image of the disk.  */
static BYTE GCR_data[MAX_TRACKS_1541 * NUM_MAX_BYTES_TRACK];

/* Speed zone image of the disk.  */
static BYTE GCR_speed_zone[MAX_TRACKS_1541 * NUM_MAX_BYTES_TRACK];

/* Pointer to the start of the GCR data for this track.  */
static BYTE *GCR_track_start_ptr = GCR_data;

/* Size of the GCR data for the current track.  */
static int GCR_current_track_size;

/* Size of the GCR data of each track.  */
static int GCR_track_size[MAX_TRACKS_1541];

/* Offset of the R/W head on the current track.  */
static int GCR_head_offset;

/* Speed (in bps) of the disk in the 4 disk areas.  */
static int rot_speed_bps[4] = { 250000, 266667, 285714, 307692 };

/* Number of bytes per track size.  */
static int raw_track_size[4] = { 6250, 6666, 7142, 7692 };

static int read_write_mode;
static int byte_ready_active;

/* If the user does not want to extend the disk image and `ask mode' is
   selected this flag gets cleared.  */
static int ask_extend_disk_image;

/* Tick when the disk image was attached.  */
static CLOCK attach_clk = (CLOCK)0;

/* Tick when the disk image was detached.  */
static CLOCK detach_clk = (CLOCK)0;

/* Clock speed of the PAL and NTSC versions of the connected computer.  */
static CLOCK pal_cycles_per_sec;
static CLOCK ntsc_cycles_per_sec;

/* Warnings.  */
enum true1541_warnings { WARN_GCRWRITE };
#define TRUE1541_NUM_WARNINGS (WARN_GCRWRITE + 1)
static warn_t *true1541_warn;

#define GCR_OFFSET(track, sector)  ((track - 1) * NUM_MAX_BYTES_TRACK \
				    + sector * NUM_BYTES_SECTOR_GCR)

static void GCR_data_writeback(void);
static void initialize_rotation(void);
static void true1541_extend_disk_image(void);

/* ------------------------------------------------------------------------- */

/* Disk image handling. */

static void read_image_d64(void)
{
    BYTE buffer[260], *ptr, chksum;
    int rc, i;
    int track, sector;

    if (!true1541_floppy)
	return;

    buffer[0] = 0x07;
    buffer[258] = buffer[259] = 0;

    /* Since the D64 format does not provide the actual track sizes or
       speed zones, we set them to standard values.  */
    for (track = 0; track < MAX_TRACKS_1541; track++) {
	GCR_track_size[track] = raw_track_size[speed_map[track]];
	memset(GCR_speed_zone, speed_map[track], NUM_MAX_BYTES_TRACK);
    }

    true1541_set_half_track(true1541_current_half_track);

    for (track = 1; track <= true1541_floppy->NumTracks; track++) {
	ptr = GCR_data + GCR_OFFSET(track, 0);

	/* Clear track to avoid read errors.  */
	memset(ptr, 0xff, NUM_MAX_BYTES_TRACK);

	for (sector = 0; sector < sector_map[track]; sector++) {
	    ptr = GCR_data + GCR_OFFSET(track, sector);

	    rc = floppy_read_block(true1541_floppy->ActiveFd,
				   true1541_floppy->ImageFormat,
				   buffer + 1, track, sector,
				   true1541_floppy->D64_Header);
	    if (rc < 0) {
		printf("1541: error reading T:%d S:%d from the disk image\n",
		       track, sector);
		/* FIXME: could be handled better. */
	    } else {
		chksum = buffer[1];
		for (i = 2; i < 257; i++)
		    chksum ^= buffer[i];
		buffer[257] = chksum;
		convert_sector_to_GCR(buffer, ptr, track, sector, diskID1, diskID2);
	    }
	}
    }
}

static int read_image_gcr(void)
{
    int track, track_len, zone_len, i, NumTracks;
    BYTE len[2], comp_speed[NUM_MAX_BYTES_TRACK / 4];
    BYTE *track_data, *zone_data;
    DWORD gcr_track_p[MAX_TRACKS_1541 * 2];
    DWORD gcr_speed_p[MAX_TRACKS_1541 * 2];
    off_t offset;

    NumTracks = true1541_floppy->NumTracks;

    lseek(true1541_floppy->ActiveFd, 12, SEEK_SET);
    if (read_dword(true1541_floppy->ActiveFd, gcr_track_p, NumTracks * 8) < 0) {
	fprintf(stderr, "1541: Could not read GCR disk image.\n");
	return 0;
    }

    lseek(true1541_floppy->ActiveFd, 12 + NumTracks * 8, SEEK_SET);
    if (read_dword(true1541_floppy->ActiveFd, gcr_speed_p, NumTracks * 8) < 0) {
	fprintf(stderr, "1541: Could not read GCR disk image.\n");
	return 0;
    }

    for (track = 0; track < MAX_TRACKS_1541; track++) {

	track_data = GCR_data + track * NUM_MAX_BYTES_TRACK;
	zone_data = GCR_speed_zone + track * NUM_MAX_BYTES_TRACK;
	memset(track_data, 0xff, NUM_MAX_BYTES_TRACK);
	memset(zone_data, 0x00, NUM_MAX_BYTES_TRACK / 4);
	GCR_track_size[track] = 6250;

	if (track <= NumTracks && gcr_track_p[track * 2] != 0) {

	    offset = gcr_track_p[track * 2];

	    lseek(true1541_floppy->ActiveFd, offset, SEEK_SET);
	    if (read(true1541_floppy->ActiveFd, (char *)len, 2) < 2) {
		fprintf(stderr, "1541: Could not read GCR disk image.\n");
		return 0;
	    }

	    track_len = len[0] + len[1] * 256;

	    if (track_len < 5000 || track_len > 7928) {
		fprintf(stderr, "1541: Track field length %i is not supported.\n", track_len);
		return 0;
	    }

	    GCR_track_size[track] = track_len;

	    lseek(true1541_floppy->ActiveFd, offset + 2, SEEK_SET);
	    if (read(true1541_floppy->ActiveFd, (char *)track_data, track_len)
			< track_len) {
		fprintf(stderr, "1541: Could not read GCR disk image.\n");
		return 0;
	    }

	    zone_len = (track_len + 3) / 4;

	    if (gcr_speed_p[track * 2] > 3) {

		offset = gcr_speed_p[track * 2];

		lseek(true1541_floppy->ActiveFd, offset, SEEK_SET);
		if (read(true1541_floppy->ActiveFd, (char *)comp_speed,
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

static void write_track_gcr(int track)
{
    int gap, i, NumTracks;
    BYTE len[2];
    DWORD gcr_track_p[MAX_TRACKS_1541 * 2];
    DWORD gcr_speed_p[MAX_TRACKS_1541 * 2];
    off_t offset;

    NumTracks = true1541_floppy->NumTracks;

    lseek(true1541_floppy->ActiveFd, 12, SEEK_SET);
    if (read_dword(true1541_floppy->ActiveFd, gcr_track_p, NumTracks * 8) < 0) {
	fprintf(stderr, "1541: Could not read GCR disk image header.\n");
	return;
    }

    lseek(true1541_floppy->ActiveFd, 12 + NumTracks * 8, SEEK_SET);
    if (read_dword(true1541_floppy->ActiveFd, gcr_speed_p, NumTracks * 8) < 0) {
	fprintf(stderr, "1541: Could not read GCR disk image header.\n");
	return;
    }

    if (gcr_track_p[(track - 1) * 2] == 0) {
	offset = lseek(true1541_floppy->ActiveFd, 0, SEEK_END);
	if (offset < 0) {
	    fprintf(stderr, "1541: Could not extend GCR disk image.\n");
	    return;
	}
	gcr_track_p[(track - 1) * 2] = offset;
    }

    offset = gcr_track_p[(track - 1) * 2];

    len[0] = GCR_track_size[track - 1] % 256;
    len[1] = GCR_track_size[track - 1] / 256;

    if (lseek(true1541_floppy->ActiveFd, offset, SEEK_SET) < 0
        || write(true1541_floppy->ActiveFd, (char *)len, 2) < 0) {
	fprintf(stderr, "1541: Could not write GCR disk image.\n");
	return;
    }

    /* Clear gap between the end of the actual track and the start of
       the next track.  */
    gap = NUM_MAX_BYTES_TRACK - GCR_track_size[track - 1];
    if (gap > 0)
	memset(GCR_track_start_ptr + GCR_track_size[track - 1], 0, gap);

    if (lseek(true1541_floppy->ActiveFd, offset + 2, SEEK_SET) < 0
        || write(true1541_floppy->ActiveFd, (char *)GCR_track_start_ptr,
                 NUM_MAX_BYTES_TRACK) < 0) {
	fprintf(stderr, "1541: Could not write GCR disk image.\n");
	return;
    }

    for (i = 0; (GCR_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK]
	    == GCR_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK + i])
	    && i < NUM_MAX_BYTES_TRACK; i++);

    if (i < GCR_track_size[track - 1]) {
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
    if (lseek(true1541_floppy->ActiveFd, offset, SEEK_SET) < 0
        || write_dword(true1541_floppy->ActiveFd,
           &gcr_speed_p[(track - 1) * 2], 4) < 0) {
    fprintf(stderr, "1541: Could not write GCR disk image.\n");
    return;
    }

#if 0  /* We do not support writing different speeds yet.  */
    for (i = 0; i < (NUM_MAX_BYTES_TRACK / 4); i++)
    zone_len = (GCR_track_size[track - 1] + 3) / 4;
    zone_data = GCR_speed_zone + (track - 1) * NUM_MAX_BYTES_TRACK;

    if (gap > 0)
	memset(zone_data + GCR_track_size[track - 1], 0, gap);

    for (i = 0; i < (NUM_MAX_BYTES_TRACK / 4); i++)
	comp_speed[i] = (zone_data[i * 4]
                         | (zone_data[i * 4 + 1] << 2)
                         | (zone_data[i * 4 + 2] << 4)
                         | (zone_data[i * 4 + 3] << 6));

    if (lseek(true1541_floppy->ActiveFd, offset, SEEK_SET) < 0
        || write(true1541_floppy->ActiveFd, (char *)comp_speed,
                 NUM_MAX_BYTES_TRACK / 4) < 0) {
        fprintf(stderr, "1541: Could not write GCR disk image");
        return;
    }
#endif
}

static int setID(void)
{
    BYTE buffer[256];
    int rc;

    if (!true1541_floppy)
	return -1;

    rc = floppy_read_block(true1541_floppy->ActiveFd,
			   true1541_floppy->ImageFormat,
			   buffer, 18, 0, true1541_floppy->D64_Header);
    if (rc >= 0) {
	diskID1 = buffer[0xa2];
	diskID2 = buffer[0xa3];
    }

    return rc;
}

static BYTE *GCR_find_sector_header(int track, int sector)
{
    BYTE *offset = GCR_track_start_ptr;
    BYTE *GCR_track_end = GCR_track_start_ptr + GCR_current_track_size;
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
		offset = GCR_track_start_ptr;
		wrap_over = 1;
	    }
	    /* Check for killer tracks.  */
	    if((++sync_count) >= GCR_current_track_size)
		return NULL;
	}

	for (i=0; i < 5; i++) {
	    GCR_header[i] = *(offset++);
	    if (offset >= GCR_track_end) {
		offset = GCR_track_start_ptr;
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

static BYTE *GCR_find_sector_data(BYTE *offset)
{
    BYTE *GCR_track_end = GCR_track_start_ptr + GCR_current_track_size;
    int header = 0;

    while (*offset != 0xff) {
	offset++;
	if (offset >= GCR_track_end)
	    offset = GCR_track_start_ptr;
	header++;
	if (header >= 500)
	    return NULL;
    }

    while (*offset == 0xff) {
	offset++;
	if (offset == GCR_track_end)
	    offset = GCR_track_start_ptr;
    }
    return offset;
}

/* ------------------------------------------------------------------------- */

/* Initialize the hardware-level 1541 emulation (should be called at least once
   before anything else).  Return 0 on success, -1 on error.  */
int true1541_init(CLOCK pal_hz, CLOCK ntsc_hz)
{
    int track;

    pal_cycles_per_sec = pal_hz;
    ntsc_cycles_per_sec = ntsc_hz;

    if (rom_loaded)
	return 0;

    true1541_warn = warn_init("1541", TRUE1541_NUM_WARNINGS);

    /* Load the ROMs. */
    if (mem_load_sys_file(dos_rom_name, true1541_rom, TRUE1541_ROM_SIZE,
                          TRUE1541_ROM_SIZE) < 0) {
	fprintf(stderr,
		"1541: Warning: ROM image not loaded; hardware-level "
		"emulation is not available.\n");
	true1541_enabled = 0;
	return -1;
    }

    /* Calculate ROM checksum. */
    {
	unsigned long s;
	int i;

	for (i = 0, s = 0; i < TRUE1541_ROM_SIZE; i++)
	    s += true1541_rom[i];

	if (s != TRUE1541_ROM_CHECKSUM)
	    fprintf(stderr,
		    "1541: Warning: unknown ROM image.  Sum: %lu\n", s);
    }

    printf("1541: ROM loaded successfully.\n");
    rom_loaded = 1;

    /* Remove the ROM check. */
    true1541_rom[0xeae4 - 0xc000] = 0xea;
    true1541_rom[0xeae5 - 0xc000] = 0xea;
    true1541_rom[0xeae8 - 0xc000] = 0xea;
    true1541_rom[0xeae9 - 0xc000] = 0xea;

    /* Trap the idle loop. */
    true1541_rom_idle_trap = true1541_rom[0xec9b - 0xc000];
    if (idling_method == TRUE1541_IDLE_TRAP_IDLE)
	true1541_rom[0xec9b - 0xc000] = 0x00;

    for (track = 0; track < MAX_TRACKS_1541; track++)
	GCR_track_size[track] = raw_track_size[speed_map[track]];

    /* Position the R/W head on the directory track.  */
    true1541_set_half_track(36);

    initialize_rotation();

    true1541_cpu_init();

    /* Make sure the sync factor is acknowledged correctly.  */
    set_sync_factor((resource_value_t) sync_factor);

    /* Make sure the traps are moved as needed.  */
    if (true1541_enabled)
	true1541_enable();

    return 0;
}

/* Activate full 1541 emulation. */
int true1541_enable(void)
{
    /* This must come first, because this might be called before the true
       1541 initialization.  */
    true1541_enabled = 1;

    if (!rom_loaded)
        return -1;

    /* Always disable kernal traps. */
    serial_remove_traps();

    if (true1541_floppy != NULL)
        true1541_attach_floppy(true1541_floppy);

    true1541_cpu_wake_up();

    ui_toggle_drive_status(1);
    return 0;
}

/* Disable full 1541 emulation.  */
void true1541_disable(void)
{
    /* This must come first, because this might be called before the true
       1541 initialization.  */
    true1541_enabled = 0;

    if (rom_loaded)
        serial_install_traps();

    true1541_cpu_sleep();

    GCR_data_writeback();

    ui_toggle_drive_status(0);
}

void true1541_reset(void)
{
    true1541_cpu_reset();
    warn_reset(true1541_warn);
}

/* ------------------------------------------------------------------------- */

static int have_new_disk = 0;	/* used for disk change detection */

/* Attach a disk image to the true 1541 emulation. */
int true1541_attach_floppy(DRIVE *floppy)
{
    if (floppy->ImageFormat != 1541)
	return -1;

    true1541_floppy = floppy;
    have_new_disk = 1;
    attach_clk = true1541_clk;
    ask_extend_disk_image = 1;

    if (true1541_floppy->GCR_Header != 0) {
        if (!read_image_gcr())
            return -1;
    } else {
	if (setID() >= 0) {
	    read_image_d64();
	    return 0;
	} else {
	    return -1;
	}
    }
    return 0;
}

/* Detach a disk image from the true 1541 emulation. */
int true1541_detach_floppy(DRIVE *floppy)
{
    if (floppy != true1541_floppy) {
        /* Shouldn't happen.  */
        fprintf(stderr, "Whaaat?  Attempt for bogus true1541 detachment!\n");
        return -1;
    } else if (true1541_floppy != NULL) {
	GCR_data_writeback();
	detach_clk = true1541_clk;
	true1541_floppy = NULL;
	memset(GCR_data, 0, sizeof(GCR_data));
    }
    return 0;
}

/* ------------------------------------------------------------------------- */

static BYTE GCR_read;
static unsigned long bits_moved = 0, accum = 0;
static int finish_byte = 0, last_mode = 1;
static CLOCK rotation_last_clk = 0L;

#define ROTATION_TABLE_SIZE      0x1000
#define ACCUM_MAX                0x10000

struct _rotation_table {
    unsigned long bits;
    unsigned long accum;
};

struct _rotation_table rotation_table[4][ROTATION_TABLE_SIZE];
struct _rotation_table *rotation_table_ptr = rotation_table[0];

/* Initialization.  */
static void initialize_rotation(void)
{
    int i, j;

    for (i = 0; i < 4; i++) {
        int speed = rot_speed_bps[i];

        for (j = 0; j < ROTATION_TABLE_SIZE; j++) {
            double bits = (double)j * (double)speed / 1000000.0;

            rotation_table[i][j].bits = (unsigned long)bits;
            rotation_table[i][j].accum = ((bits - (unsigned long)bits)
                                          * ACCUM_MAX);
        }
    }

    bits_moved = accum = 0;
}

/* Set the `byte ready' bit.  */
inline void true1541_set_byte_ready(int val)
{
    byte_ready = val;
}

/* Rotate the disk according to the current value of `true1541_clk'.  If
   `mode_change' is non-zero, there has been a Read -> Write mode switch.  */
void true1541_rotate_disk(int mode_change)
{
    unsigned long new_bits;

    if (mode_change) {
	finish_byte = 1;
	return;
    }

    /* If the drive's motor is off or byte ready is disabled do nothing.  */
    if (byte_ready_active != 0x06)
	return;

    /* Calculate the number of bits that have passed under the R/W head since
       the last time.  */
    {
        CLOCK delta = true1541_clk - rotation_last_clk;

        new_bits = 0;
        while (delta > 0) {
            if (delta >= ROTATION_TABLE_SIZE) {
                struct _rotation_table *p = (rotation_table_ptr
                                             + ROTATION_TABLE_SIZE - 1);
                new_bits += p->bits;
                accum += p->accum;
                delta -= ROTATION_TABLE_SIZE - 1;
            } else {
                struct _rotation_table *p = rotation_table_ptr + delta;
                new_bits += p->bits;
                accum += p->accum;
                delta = 0;
            }
            if (accum >= ACCUM_MAX) {
                accum -= ACCUM_MAX;
                new_bits++;
            }
        }
    }

    if (bits_moved + new_bits >= 8) {

	bits_moved += new_bits;
	rotation_last_clk = true1541_clk;

	if (finish_byte) {
	    if (last_mode == 0) { /* write */
		GCR_dirty_track = 1;
		if (bits_moved >= 8) {
		    GCR_track_start_ptr[GCR_head_offset] = GCR_write_value;
		    GCR_head_offset = ((GCR_head_offset + 1) %
                                       GCR_current_track_size);
		    bits_moved -= 8;
		}
	    } else {		/* read */
		if (bits_moved >= 8) {
		    GCR_head_offset = ((GCR_head_offset + 1) %
                                       GCR_current_track_size);
		    bits_moved -= 8;
		    GCR_read = GCR_track_start_ptr[GCR_head_offset];
		}
	    }

	    finish_byte = 0;
	    last_mode = read_write_mode;
	}

	if (last_mode == 0) {	/* write */
	    GCR_dirty_track = 1;
	    while (bits_moved >= 8) {
		GCR_track_start_ptr[GCR_head_offset] = GCR_write_value;
		GCR_head_offset = ((GCR_head_offset + 1)
                                   % GCR_current_track_size);
		bits_moved -= 8;
	    }
	} else {		/* read */
	    GCR_head_offset = ((GCR_head_offset + bits_moved / 8)
			       % GCR_current_track_size);
	    bits_moved %= 8;
	    GCR_read = GCR_track_start_ptr[GCR_head_offset];
	}

	if (!true1541_sync_found())
	    true1541_set_byte_ready(1);
    } /* if (bits_moved + new_bits >= 8) */
}

/* ------------------------------------------------------------------------- */

/* This prevents the CLOCK counters `rotation_last_clk', `attach_clk'
   and `detach_clk' from overflowing.  */
void true1541_prevent_clk_overflow(CLOCK sub)
{
    sub = true1541_cpu_prevent_clk_overflow(sub);

    if (sub > 0) {
	true1541_rotate_disk(0);
	rotation_last_clk -= sub;
	if (attach_clk > (CLOCK) 0)
	    attach_clk -= sub;
	if (detach_clk > (CLOCK) 0)
	    detach_clk -= sub;
    }
}

/* Read a GCR byte from the disk. */
BYTE true1541_read_disk_byte(void)
{
    BYTE val;

    if (attach_clk != (CLOCK)0) {
        if (true1541_clk - attach_clk < TRUE1541_ATTACH_DELAY)
            return 0;
        attach_clk = (CLOCK)0;
    }

    true1541_rotate_disk(0);
    val = GCR_read;

    return val;
}

int true1541_byte_ready(void)
{
   if(byte_ready_active) {
       true1541_rotate_disk(0);
       return byte_ready;
   } else {
       return 0;
   }
}


/* Return non-zero if the Sync mark is found.  It is required to
   call true1541_rotate_disk() to update GCR_head_offset first.  */
int true1541_sync_found(void)
{
    BYTE val = GCR_track_start_ptr[GCR_head_offset];

    if (val != 0xff || last_mode == 0) {
        return 0;
    } else {
	int next_head_offset = (GCR_head_offset > 0
				? GCR_head_offset - 1
				: GCR_current_track_size - 1);

	if (GCR_track_start_ptr[next_head_offset] != 0xff)
	    return 0;

	/* As the current rotation code cannot cope with non byte aligned
	   writes, do not change `bits_moved'!  */
	/* bits_moved = 0; */
	return 1;
    }
}

/* Move the head to half track `num'.  */
void true1541_set_half_track(int num)
{
    if (num > 84)
	num = 84;
    else if (num < 2)
	num = 2;

    true1541_current_half_track = num;
    GCR_track_start_ptr = (GCR_data
			   + ((true1541_current_half_track / 2 - 1)
			      * NUM_MAX_BYTES_TRACK));

    GCR_current_track_size = GCR_track_size[true1541_current_half_track / 2
                                            - 1];
    GCR_head_offset = 0;
}

/* Increment the head position by `step' half-tracks. Valid values
   for `step' are `+1' and `-1'.  */
void true1541_move_head(int step)
{
    GCR_data_writeback();
    true1541_set_half_track(true1541_current_half_track + step);
    ui_display_drive_track((double)true1541_current_half_track / 2.0);
}

/* Write one GCR byte to the disk. */
void true1541_write_gcr(BYTE val)
{
    if (true1541_floppy == NULL)
	return;

    true1541_rotate_disk(0);
    GCR_write_value = val;
}

/* Return the write protect sense status. */
int true1541_write_protect_sense(void)
{
    /* Toggle the write protection bit if the disk was detached.  */
    if (detach_clk != (CLOCK)0) {
	if (true1541_clk - detach_clk < TRUE1541_DETACH_DELAY)
	    return 0;
	detach_clk = (CLOCK)0;
    }
    if ((attach_clk != (CLOCK)0) &&
	(true1541_clk - attach_clk < TRUE1541_ATTACH_DELAY))
	return 0;
    if (true1541_floppy == NULL) {
	/* No disk in drive, write protection is on. */
	return 1;
    } else if (have_new_disk) {
	/* Disk has changed, make sure the drive sees at least one change in
	   the write protect status. */
	have_new_disk = 0;
	return !true1541_floppy->ReadOnly;
    } else {
	return true1541_floppy->ReadOnly;
    }
}

static void GCR_data_writeback(void)
{
    int rc, extend, track, sector;
    BYTE buffer[260], *offset;

    track = true1541_current_half_track / 2;

    if (!GCR_dirty_track)
	return;

    if (true1541_floppy->GCR_Header != 0) {
	write_track_gcr(track);
	GCR_dirty_track = 0;
	return;
    }

    if (track > EXT_TRACKS_1541)
	return;

    if (track > true1541_floppy->NumTracks) {
	switch (extend_image_policy) {
	  case TRUE1541_EXTEND_NEVER:
	    ask_extend_disk_image = 1;
	    return;
	  case TRUE1541_EXTEND_ASK:
	    if (ask_extend_disk_image == 1) {
		extend = ui_extend_image_dialog();
		if (extend == 0) {
		    ask_extend_disk_image = 0;
		    return;
		} else {
		    true1541_extend_disk_image();
		}
	    } else {
		return;
	    }
	    break;
	  case TRUE1541_EXTEND_ACCESS:
	    ask_extend_disk_image = 1;
	    true1541_extend_disk_image();
	    break;
	}
    }

    GCR_dirty_track = 0;

    for (sector = 0; sector < sector_map[track]; sector++) {

	offset = GCR_find_sector_header(track, sector);
	if (offset == NULL)
	    fprintf(stderr,
                    "1541: Could not find header of T:%d S:%d.\n",
                    track, sector);
	else {

	    offset = GCR_find_sector_data(offset);
	    if (offset == NULL)
		fprintf(stderr,
		"1541: Could not find data sync of T:%d S:%d.\n",
		track, sector);
	    else {

		convert_GCR_to_sector(buffer, offset, GCR_track_start_ptr,
		    GCR_current_track_size);
		if (buffer[0] != 0x7)
		    fprintf(stderr,
			"1541: Could not find data block id of T:%d S:%d.\n",
			track, sector);
		else {
		    rc = floppy_write_block(true1541_floppy->ActiveFd,
                                true1541_floppy->ImageFormat,
                                buffer + 1, track, sector,
                                true1541_floppy->D64_Header);
		    if (rc < 0)
			fprintf(stderr,
			"1541: Could not update T:%d S:%d.\n", track, sector);
		}
	    }
	}
    }
}

void true1541_extend_disk_image(void)
{
    int rc, track, sector;
    BYTE buffer[256];

    true1541_floppy->NumTracks = EXT_TRACKS_1541;
    true1541_floppy->NumBlocks = EXT_BLOCKS_1541;
    memset(buffer, 0, 256);
    for (track = NUM_TRACKS_1541 + 1; track <= EXT_TRACKS_1541; track++) {
	for (sector = 0; sector < sector_map[track]; sector++) {
	    rc = floppy_write_block(true1541_floppy->ActiveFd,
                            true1541_floppy->ImageFormat,
                            buffer, track, sector,
                            true1541_floppy->D64_Header);
	if (rc < 0)
	    fprintf(stderr,
	    "1541: Could not update T:%d S:%d.\n", track, sector);
	}
    }
}

void true1541_update_zone_bits(int zone)
{
    rotation_table_ptr = rotation_table[zone];
}

void true1541_update_viad2_pcr(int pcrval)
{
    read_write_mode = pcrval & 0x20;
    byte_ready_active = (byte_ready_active & ~0x02) | (pcrval & 0x02);
}

void true1541_motor_control(int flag)
{
    byte_ready_active = (byte_ready_active & ~0x04) | (flag & 0x04);
}

BYTE true1541_read_viad2_prb(void)
{
    true1541_rotate_disk(0);
    return (true1541_sync_found() ? 0 : 0x80)
        | (true1541_write_protect_sense() ? 0 : 0x10);
}

/* ------------------------------------------------------------------------- */

/* Handle a ROM trap. */
int true1541_trap_handler(void)
{
    if (true1541_cpu_regs.pc == 0xec9b) {
	/* Idle loop */
	init_complete = 1;
	true1541_cpu_regs.pc = 0xebff;
	if (idling_method == TRUE1541_IDLE_TRAP_IDLE)
	    true1541_clk = next_alarm_clk(&true1541_int_status);
    } else
	return 1;

    return 0;
}

/* ------------------------------------------------------------------------- */

/* Set the sync factor between the computer and the 1541.  */

void true1541_set_sync_factor(unsigned int factor)
{
    true1541_cpu_set_sync_factor(factor);
}

void true1541_set_pal_sync_factor(void)
{
    if (pal_cycles_per_sec != 0.0) {
        int sync_factor = (int) floor(65536.0
				      * (1000000.0 /
                                         ((double)pal_cycles_per_sec)));
        true1541_set_sync_factor(sync_factor);
    }
}

void true1541_set_ntsc_sync_factor(void)
{
    if (ntsc_cycles_per_sec != 0.0) {
        int sync_factor = (int) floor(65536.0 * (1000000.0 /
                                             ((double)ntsc_cycles_per_sec)));

        true1541_cpu_set_sync_factor(sync_factor);
    }
}

/* ------------------------------------------------------------------------- */

/* Update the status bar in the UI.  */
void true1541_update_ui_status(void)
{
    static int old_led_status = 0;
    static int old_half_track = 0;
    int my_led_status;

    if (!true1541_enabled) {
        if (old_led_status >= 0) {
            old_led_status = old_half_track = -1;
            ui_toggle_drive_status(0);
        }
        return;
    }

    /* Actually update the LED status only if the `trap idle' idling method
       is being used, as the LED status could be incorrect otherwise.  */
    if (idling_method != TRUE1541_IDLE_SKIP_CYCLES)
	my_led_status = true1541_led_status ? 1 : 0;
    else
	my_led_status = 0;

    if (my_led_status != old_led_status) {
        ui_display_drive_led(my_led_status);
	old_led_status = my_led_status;
    }

    if (true1541_current_half_track != old_half_track) {
	old_half_track = true1541_current_half_track;
	ui_display_drive_track((float) true1541_current_half_track / 2.0);
    }
}

/* This is called at every vsync.  */
void true1541_vsync_hook(void)
{
    true1541_update_ui_status();

    if (!true1541_enabled)
        return;

    if (idling_method != TRUE1541_IDLE_SKIP_CYCLES)
        true1541_cpu_execute();
}
