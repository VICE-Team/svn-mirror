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
	- support for more than 35 tracks.
	- support for .d64 images with attached error code.
	- support for GCR encoded image files.
	- check for byte ready *within* `BVC', `BVS' and `PHP'.
	- serial bus handling might be faster. */

#define __1541__

#include "vice.h"

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "true1541.h"
#include "interrupt.h"
#include "vmachine.h"
#include "serial.h"
#include "drive.h"
#include "warn.h"
#include "mem.h"
#include "resources.h"
#include "memutils.h"
#include "viad.h"
#include "via.h"
#include "cia.h"
#include "ui.h"

/* ------------------------------------------------------------------------- */

/* If this is defined, allow exact GCR operation when writing to the
   disk too.  */
#define TRUE_GCR_WRITE

/* This enables faster handling of the IEC bus.  It does not work on VIC20
   yet.  */
#if !defined(VIC20)
#define FAST_BUS
#endif

#define NOT(x) ((x)^1)
#define NUM_BYTES_SECTOR_GCR 360
#define NUM_MAX_BYTES_TRACK 7693

extern void set_atn(BYTE state);

/* RAM/ROM.  */
BYTE true1541_rom[TRUE1541_ROM_SIZE];
BYTE true1541_ram[TRUE1541_RAM_SIZE];

/* LED status (zero = off).  */
int true1541_led_status;

/* Current half track on which the R/W head is positioned.  */
int true1541_current_half_track = 36;

/* If nonzero, the 1541 ROM has already been loaded.  */
static int true1541_rom_loaded = 0;

#ifndef FAST_BUS
static BYTE cpu_data = 0;
static BYTE cpu_clock = 0;
static BYTE cpu_atn = 0;
static BYTE drive_data = 0;
static BYTE drive_clock = 0;
static BYTE drive_atna = 0;
static BYTE drive_data_modifier = 0;
static BYTE cpu_bus_val = 0;
static BYTE drive_bus_val = 0;
static BYTE bus_data = 0;
static BYTE bus_clock = 0;
static BYTE bus_atn = 0;
#else
static BYTE drive_bus, drive_data, cpu_bus; /* FIXME: ugly name `drive_data'. */
/* This is the IEC line status as seen by the CIA and VIA ports.  */
static BYTE drive_port, cpu_port;
#endif

#ifdef CBM64
static BYTE parallel_cable_cpu_value = 0xff;
static BYTE parallel_cable_drive_value = 0xff;
#endif

static int init_complete = 0;
static int byte_ready = 1;

/* Pointer to the attached disk image.  */
static DRIVE *true1541_floppy;

/* Disk ID.  */
static BYTE diskID1, diskID2;

/* Map of the sector sizes.  */
extern char sector_map[43];

/* Flag: does the current need to be written out to disk?  */
static int GCR_dirty_track = 0;

/* GCR value being written to the disk.  */
static BYTE GCR_write_value = 0x55;

/* Raw GCR image of the disk.  */
static BYTE GCR_data[MAX_TRACKS_1541 * NUM_MAX_BYTES_TRACK];

/* Pointer to the start of the GCR data for this track.  */
static BYTE *GCR_track_start_ptr = GCR_data;

/* Pointer to the size of the GCR data for this track.  */
static int GCR_track_size;

/* Offset of the R/W head on the current track.  */
static int GCR_head_offset;

/* Speed (in bps) of the disk in the 4 disk areas.  */
static int rot_speed_bps[4] = { 250000, 266667, 285714, 307692 };

/* Speed zone of each track.  This should be removed in the future.  */
static int speed_map[35] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                             3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1,
                             1, 1, 0, 0, 0, 0, 0 };

/* Number of bytes per track size.  */
static int raw_track_size[4] = { 6250, 6666, 7142, 7692 };

static int read_write_mode;
static int byte_ready_active;

/* Tick when the disk image was attached.  */
static CLOCK attach_clk = (CLOCK)0;

/* Warnings.  */
enum true1541_warnings { WARN_GCRWRITE };
#define TRUE1541_NUM_WARNINGS (WARN_GCRWRITE + 1)
static warn_t *true1541_warn;

#define GCR_OFFSET(track, sector)  ((track - 1) * NUM_MAX_BYTES_TRACK \
				    + sector * NUM_BYTES_SECTOR_GCR)

static void GCR_data_writeback(void);
static void initialize_rotation(void);

/* ------------------------------------------------------------------------- */

/* GCR handling. */

static BYTE GCR_conv_data[16] = { 0x0a, 0x0b, 0x12, 0x13,
				  0x0e, 0x0f, 0x16, 0x17,
				  0x09, 0x19, 0x1a, 0x1b,
				  0x0d, 0x1d, 0x1e, 0x15 };

static BYTE From_GCR_conv_data[32] = { 0, 0, 0, 0, 0, 0, 0, 0,
				       0, 8, 0, 1, 0,12, 4, 5,
				       0, 0, 2, 3, 0,15, 6, 7,
				       0, 9,10,11, 0,13,14, 0 };

static void convert_4bytes_to_GCR(BYTE *buffer, BYTE *ptr)
{
    *ptr = GCR_conv_data[(*buffer) >> 4] << 3;
    *ptr |= GCR_conv_data[(*buffer) & 0x0f] >> 2;
    ptr++;

    *ptr = GCR_conv_data[(*buffer) & 0x0f] << 6;
    buffer++;
    *ptr |= GCR_conv_data[(*buffer) >> 4] << 1;
    *ptr |= GCR_conv_data[(*buffer) & 0x0f] >> 4;
    ptr++;

    *ptr = GCR_conv_data[(*buffer) & 0x0f] << 4;
    buffer++;
    *ptr |= GCR_conv_data[(*buffer) >> 4] >> 1;
    ptr++;

    *ptr = GCR_conv_data[(*buffer) >> 4] << 7;
    *ptr |= GCR_conv_data[(*buffer) & 0x0f] << 2;
    buffer++;
    *ptr |= GCR_conv_data[(*buffer) >> 4] >> 3;
    ptr++;

    *ptr = GCR_conv_data[(*buffer) >> 4] << 5;
    *ptr |= GCR_conv_data[(*buffer) & 0x0f];
}

static void convert_GCR_to_4bytes(BYTE *buffer, BYTE *ptr)
{
    BYTE gcr_bytes[8];
    int i;

    gcr_bytes[0] =  (*buffer) >> 3;
    gcr_bytes[1] =  ((*buffer) & 0x07) << 2;
    buffer++;
    gcr_bytes[1] |= (*buffer) >> 6;
    gcr_bytes[2] =  ((*buffer) & 0x3e) >> 1;
    gcr_bytes[3] =  ((*buffer) & 0x01) << 4;
    buffer++;
    gcr_bytes[3] |= ((*buffer) & 0xf0) >> 4;
    gcr_bytes[4] =  ((*buffer) & 0x0f) << 1;
    buffer++;
    gcr_bytes[4] |= (*buffer) >> 7;
    gcr_bytes[5] =  ((*buffer) & 0x7c) >> 2;
    gcr_bytes[6] =  ((*buffer) & 0x03) << 3;
    buffer++;
    gcr_bytes[6] |= ((*buffer) & 0xe0) >> 5;
    gcr_bytes[7] = (*buffer) & 0x1f;

    for (i = 0; i < 4; i++, ptr++) {
        *ptr = From_GCR_conv_data[gcr_bytes[2 * i]] << 4;
        *ptr |= From_GCR_conv_data[gcr_bytes[2 * i + 1]];
    }
}

static void convert_sector_to_GCR(BYTE *buffer, BYTE *ptr, int track,
				  int sector)
{
    int i;
    BYTE buf[4];

    memset(ptr, 0xff, 5);	/* Sync */
    ptr += 5;

    buf[0] = 0x08;		/* Header identifier */
    buf[1] = sector ^ track ^ diskID2 ^ diskID1;
    buf[2] = sector;
    buf[3] = track;
    convert_4bytes_to_GCR(buf, ptr);
    ptr += 5;

    buf[0] = diskID2;
    buf[1] = diskID1;
    buf[2] = buf[3] = 0x0f;
    convert_4bytes_to_GCR(buf, ptr);
    ptr += 5;

    memset(ptr, 0x55, 9);	/* Header Gap */
    ptr += 9;

    memset(ptr, 0xff, 5);	/* Sync */
    ptr += 5;

    for (i = 0; i < 65; i++) {
	convert_4bytes_to_GCR(buffer, ptr);
	buffer += 4;
	ptr += 5;
    }
    /* FIXME: This is approximated.  */
    memset(ptr, 0x55, 6);	/* Gap before next sector.  */
    ptr += 6;

}

static void convert_GCR_to_sector(BYTE *buffer, BYTE *ptr)
{
    BYTE *offset = ptr;
    BYTE *GCR_track_end = GCR_track_start_ptr + GCR_track_size;
    char GCR_header[5];
    int i, j;

    for (i = 0; i < 65; i++) {
	for (j = 0; j < 5; j++) {
	    GCR_header[j] = *(offset++);
	    if (offset >= GCR_track_end)
		offset = GCR_track_start_ptr;
	}
	convert_GCR_to_4bytes(GCR_header, buffer);
	buffer += 4;
    }
}

static void read_image_GCR(void)
{
    BYTE buffer[260], *ptr, chksum;
    int rc, i;
    int track, sector;

    if (!true1541_floppy)
	return;

    buffer[0] = 0x07;
    buffer[258] = buffer[259] = 0;

    for (track = 1; track <= 35; track++) {
	ptr = GCR_data + GCR_OFFSET(track, 0);
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
		convert_sector_to_GCR(buffer, ptr, track, sector);
	    }
	}
    }
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
	true1541_ram[0x12] = diskID1;
	true1541_ram[0x13] = diskID2;
    }

    return rc;
}

static BYTE *GCR_find_sector_header(int track, int sector)
{
    BYTE *offset = GCR_track_start_ptr;
    BYTE *GCR_track_end = GCR_track_start_ptr + GCR_track_size;
    char GCR_header[5], header_data[4];
    int i, sync_count = 0, wrap_over = 0;

    while ((offset < GCR_track_end) && !wrap_over)
    {
	while (*offset != 0xff)
	{
	    offset++;
	    if (offset >= GCR_track_end)
		return NULL;
	}

	while (*offset == 0xff)
	{
	    offset++;
	    if (offset == GCR_track_end) {
		offset = GCR_track_start_ptr;
		wrap_over = 1;
	    }
	    /* Check for killer tracks.  */
	    if((++sync_count) >= GCR_track_size)
		return NULL;
	}

	for (i=0; i < 5; i++)
	{
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
    BYTE *GCR_track_end = GCR_track_start_ptr + GCR_track_size;
    int header = 0;

    while (*offset != 0xff)
    {
	offset++;
	if (offset >= GCR_track_end)
	    offset = GCR_track_start_ptr;
	header++;
	if (header >= 500)
	    return NULL;

    }

    while (*offset == 0xff)
    {
	offset++;
	if (offset == GCR_track_end)
	    offset = GCR_track_start_ptr;
    }
    return offset;
}

/* ------------------------------------------------------------------------- */

/* Initialize the hardware-level 1541 emulation (should be called at least once
   before anything else).  Return 0 on success, -1 on error.  */
int initialize_true1541(void)
{
    if (true1541_rom_loaded)
	return 1;

    true1541_warn = warn_init("1541", TRUE1541_NUM_WARNINGS);

    /* Load the ROMs. */
    if (mem_load_sys_file(app_resources.directory, app_resources.dosName,
		  true1541_rom, TRUE1541_ROM_SIZE, TRUE1541_ROM_SIZE) < 0) {
	fprintf(stderr,
		"1541: Warning: ROM image not loaded; hardware-level "
		"emulation is not available.\n");
	app_resources.true1541 = 0;
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
    true1541_rom_loaded = 1;

    /* Remove the ROM check. */
    true1541_rom[0xeae4 - 0xc000] = 0xea;
    true1541_rom[0xeae5 - 0xc000] = 0xea;
    true1541_rom[0xeae8 - 0xc000] = 0xea;
    true1541_rom[0xeae9 - 0xc000] = 0xea;

    /* Trap the idle loop. */
    true1541_rom[0xec9b - 0xc000] = 0x00;

#ifndef TRUE_GCR_WRITE
    /* Trap the write sector routine. */
    true1541_rom[0xf594 - 0xc000] = 0x20;  /* JSR $F5F2 */
    true1541_rom[0xf595 - 0xc000] = 0xf2;
    true1541_rom[0xf596 - 0xc000] = 0xf5;
    true1541_rom[0xf597 - 0xc000] = 0x00;
#endif

    /* Position the R/W head on the directory track.  */
    true1541_set_half_track(36);

    initialize_rotation();

    true1541_cpu_init();
    return 0;
}

/* Activate full 1541 emulation. */
int true1541_enable(void)
{
    /* Always disable kernal traps. */
    if (rom_loaded) {
	remove_serial_traps();
    }

    if (true1541_floppy != NULL)
        true1541_attach_floppy(true1541_floppy);

    app_resources.true1541 = 1;
    true1541_cpu_wake_up();

    UiToggleDriveStatus(1);
    return 0;
}

/* Disable full 1541 emulation.  */
void true1541_disable(void)
{
    if (rom_loaded) {
	/* Do not enable kernal traps if required. */
	if (app_resources.noTraps)
	    remove_serial_traps();
	else
	    install_serial_traps();
    }
    app_resources.true1541 = 0;
    true1541_cpu_sleep();

    GCR_data_writeback();

    UiToggleDriveStatus(0);
}

/* This is called when the true1541 resource is changed, to acknowledge the new
   value.  */
void true1541_ack_switch(void)
{
    if (app_resources.true1541)
	true1541_enable();
    else
	true1541_disable();
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

    if (setID() >= 0) {
	read_image_GCR();
	return 0;
    } else
	return -1;
}

/* Detach a disk image from the true 1541 emulation. */
int true1541_detach_floppy(void)
{
    if (true1541_floppy != NULL) {
	GCR_data_writeback();
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
		    GCR_head_offset = (GCR_head_offset + 1) % GCR_track_size;
		    bits_moved -= 8;
		}
	    } else {		/* read */
		if (bits_moved >= 8) {
		    GCR_head_offset = (GCR_head_offset + 1) % GCR_track_size;
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
		GCR_head_offset = (GCR_head_offset + 1) % GCR_track_size;
		bits_moved -= 8;
	    }
	} else {		/* read */
	    GCR_head_offset = ((GCR_head_offset + bits_moved / 8)
			       % GCR_track_size);
	    bits_moved %= 8;
	    GCR_read = GCR_track_start_ptr[GCR_head_offset];
	}

	if (!true1541_sync_found())
	    true1541_set_byte_ready(1);
    } /* if (bits_moved + new_bits >= 8) */
}

/* ------------------------------------------------------------------------- */

/* This prevents the CLOCK counters (currently only `rotation_last_clk')
   from overflowing.  */
void true1541_prevent_clk_overflow(void)
{
    if (true1541_cpu_prevent_clk_overflow()) {
	true1541_rotate_disk(0);
	rotation_last_clk -= PREVENT_CLK_OVERFLOW_SUB;
        if (attach_clk > (CLOCK) 0)
            attach_clk -= PREVENT_CLK_OVERFLOW_SUB;
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

#ifndef TRUE1541_ROTATE

    val = GCR_track_start_ptr[GCR_head_offset];
    GCR_head_offset = (GCR_head_offset + 1) % GCR_track_size;

#else  /* !TRUE1541_ROTATE */

    true1541_rotate_disk(0);
    val = GCR_read;

#endif /* !TRUE1541_ROTATE */

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

#ifndef TRUE1541_ROTATE
    if (val != 0xff)
	true1541_read_disk_byte();
    else {
	while (1) {
	    int next_head_offset = (GCR_head_offset + 1) % GCR_track_size;

	    if (GCR_track_start_ptr[next_head_offset] != 0xff)
		break;
	    GCR_head_offset = next_head_offset;
	}
    }
    return val == 0xff;
#else
    if (val != 0xff || last_mode == 0) {
        return 0;
    } else {
	int next_head_offset = (GCR_head_offset > 0
				? GCR_head_offset - 1
				: GCR_track_size - 1);

	if (GCR_track_start_ptr[next_head_offset] != 0xff)
	    return 0;

	/* As the current rotation code cannot cope with non byte aligned
	   writes, do not change `bits_moved'!  */
	/* bits_moved = 0; */
	return 1;
    }
#endif
}

/* Move the head to half track `num'.  */
void true1541_set_half_track(int num)
{
    if (num > 70)
	num = 70;
    else if (num < 2)
	num = 2;

    true1541_current_half_track = num;
    GCR_track_start_ptr = (GCR_data
			   + ((true1541_current_half_track / 2 - 1)
			      * NUM_MAX_BYTES_TRACK));

    GCR_track_size = raw_track_size[speed_map[true1541_current_half_track / 2 - 1]];

    GCR_head_offset = 0;
}

/* Increment the head position by `step' half-tracks. Valid values
   for `step' are `+1' and `-1'.  */
void true1541_move_head(int step)
{
    GCR_data_writeback();
    true1541_set_half_track(true1541_current_half_track + step);
    UiDisplayDriveTrack((double)true1541_current_half_track / 2.0);
}

/* Write one GCR byte to the disk. */
void true1541_write_gcr(BYTE val)
{
#ifndef TRUE_GCR_WRITE

    /* This is not implemented! */
    warn(true1541_warn, WARN_GCRWRITE,
	 "program tries to write raw GCR data to the disk.");

#else

    if (true1541_floppy == NULL)
	return;

#ifndef TRUE1541_ROTATE
    if (!true1541_floppy->ReadOnly) {
	GCR_track_start_ptr[GCR_head_offset] = val;
	GCR_head_offset = (GCR_head_offset + 1) % GCR_track_size;
        GCR_dirty_track = 1;
    }
#else
    true1541_rotate_disk(0);
    GCR_write_value = val;
#endif

#endif
}

/* Return the write protect sense status. */
int true1541_write_protect_sense(void)
{
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
    int rc, track, sector;
    BYTE buffer[260], *offset;

    if (!GCR_dirty_track)
        return;

    GCR_dirty_track = 0;
    track = true1541_current_half_track / 2;

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

		convert_GCR_to_sector(buffer, offset);
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

void true1541_update_zone_bits(int zone)
{
    rotation_table_ptr = rotation_table[zone];
#ifdef TRUE1541_ROTATE
    /* printf("1541: zone %d, %d bps\n", zone, rot_speed_bps[zone]); */
#endif
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

/* ------------------------------------------------------------------------- */

#ifndef TRUE_GCR_WRITE
/* DOS ROM write trap.  As handling GCR writes is complicate, we simply put a
   trap in the DOS write routine and do everything by hand. */
static void true1541_write_trap(void)
{
    int track, sector, addr;

    track = true1541_ram[0x18];
    sector = true1541_ram[0x19];
    addr = true1541_ram[0x30] | (true1541_ram[0x31] << 8);
    printf("1541: write T:%d S:%d from $%04X... ", track, sector, addr);
    if (addr <= 0x700
	&& floppy_write_block(true1541_floppy->ActiveFd,
			      true1541_floppy->ImageFormat,
			      true1541_ram + addr, track, sector,
			      true1541_floppy->D64_Header) >= 0) {
	BYTE buf[260], checksum;
	int i;

	/* FIXME: this is inefficient and ugly. */
	buf[0] = 0x7;
	buf[258] = buf[259] = 0;
	checksum = buf[1] = true1541_ram[addr];
	for (i = 2; i < 257; i++) {
	    buf[i] = true1541_ram[addr + i - 1];
	    checksum ^= buf[i];
	}
	buf[257] = checksum;
	convert_sector_to_GCR(buf, GCR_data + GCR_OFFSET(track, sector),
			      track, sector);
	printf("OK.\n");
    } else
	printf("Error!\n");
    true1541_program_counter = 0xf5dc;
}
#endif

/* Handle a ROM trap. */
int true1541_trap_handler(void)
{
    if (true1541_program_counter == 0xec9b) {
	/* Idle loop */
	init_complete = 1;
	true1541_program_counter = 0xebff;
	if (app_resources.true1541IdleMethod == TRUE1541_IDLE_TRAP_IDLE)
	    true1541_clk = next_alarm_clk(&true1541_int_status);
#ifndef TRUE_GCR_WRITE
    } else if (true1541_program_counter == 0xf597) {
	true1541_write_trap();
#endif
    } else
	return 1;

    return 0;
}

/* ------------------------------------------------------------------------- */

/* IEC bus handling.  FIXME: Total chaos here!!  */

#define BUS_DBG 0

#ifndef FAST_BUS
inline void resolve_bus_signals(void)
{
    bus_atn = NOT(cpu_atn);
    bus_clock = (NOT(cpu_clock) & NOT(drive_clock));
    bus_data = (NOT(drive_data) & NOT(drive_data_modifier) & NOT(cpu_data));

#if BUS_DBG
    printf("SB: [%ld]  data:%d clock:%d atn:%d\n",
	   true1541_clk, bus_data, bus_clock, bus_atn);
#endif
}
#endif

#ifdef FAST_BUS
inline static void update_ports(void)
{
    cpu_port = cpu_bus & drive_bus;
    drive_port = (((cpu_port >> 4) & 0x4)
		  | (cpu_port >> 7)
		  | ((cpu_bus << 3) & 0x80));
}
#endif

void serial_bus_drive_write(BYTE data)
{
#ifndef FAST_BUS
    static int last_write = 0;
#endif

    if (!app_resources.true1541)
	return;

#ifndef FAST_BUS

    data = ~data;
    drive_data = ((data & 2) >> 1);
    drive_clock = ((data & 8) >> 3);
    drive_atna = ((data & 16) >> 4);
    drive_data_modifier = (NOT(cpu_atn) ^ NOT(drive_atna));

    if (last_write != (data & 26)) {
	resolve_bus_signals();
    }
    last_write = data & 26;

#else

    drive_bus = (((data << 3) & 0x40)
		 | ((data << 6) & ((~data ^ cpu_bus) << 3) & 0x80));
    drive_data = data;
    update_ports();

#endif
}

BYTE serial_bus_drive_read(void)
{
    if (!app_resources.true1541)
	return 0;

#ifndef FAST_BUS

    drive_bus_val = bus_data | (bus_clock << 2) | (bus_atn << 7);

#if BUS_DBG
    printf("SB: drive read  data:%d clock:%d atn:%d\n",
	   (~bus_data) & 1, (~bus_clock) & 1, (cpu_atn) & 1);
#endif

    return drive_bus_val;

#else

    return drive_port;

#endif
}

/* ------------------------------------------------------------------------- */

/* C64/C128-specific IEC bus handling. */

#if defined(CBM64) || defined(C128)

/* The C64 has all bus lines in one I/O byte in a CIA.  If this byte is read or
   modified, these routines are called. */

void serial_bus_cpu_write(BYTE data)
{
#ifndef FAST_BUS
    static int last_write = 0;
#endif

    if (!app_resources.true1541)
	return;

    true1541_cpu_execute();

#ifndef FAST_BUS

    if ((cpu_atn == 0) && (data & 8))
	set_atn(1);

    if (!(data & 8))
	set_atn(0);

    cpu_data = ((data & 32) >> 5);
    cpu_clock = ((data & 16) >> 4);
    cpu_atn = ((data & 8) >> 3);
    drive_data_modifier = (NOT(cpu_atn) ^ NOT(drive_atna));

    if (last_write != (data & 56))
	resolve_bus_signals();

    last_write = data & 56;

#else

    cpu_bus = (((data << 2) & 0x80)
	       | ((data << 2) & 0x40)
	       | ((data << 1) & 0x10));

    /* FIXME: this is slow, we should avoid doing it when not necessary.  */

    set_atn(!(cpu_bus & 0x10));

    drive_bus = (((drive_data << 3) & 0x40)
		 | ((drive_data << 6)
		    & ((~drive_data ^ cpu_bus) << 3)
		    & 0x80));

    update_ports();

#endif
}

BYTE serial_bus_cpu_read(void)
{
    if (!app_resources.true1541)
	return 0;

#ifndef FAST_BUS

    true1541_cpu_execute();
    cpu_bus_val = (bus_data << 7) | (bus_clock << 6) | (bus_atn << 3);

    return cpu_bus_val;

#else

    true1541_cpu_execute();
    return cpu_port;

#endif
}

#endif

/* ------------------------------------------------------------------------- */

/* VIC20-specific IEC bus handling. */

#if defined(VIC20)

/*
   The VIC20 has a strange bus layout for the serial IEC bus.

     VIA1 CA2 CLK out
     VIA1 CB1 SRQ in
     VIA1 CB2 DATA out
     VIA2 PA0 CLK in
     VIA2 PA1 DATA in
     VIA2 PA7 ATN out

 */

extern unsigned int reg_pc;

/* These two routines are called for VIA2 Port A. */

BYTE serial_bus_pa_read(void)
{
    if (!app_resources.true1541)
	return 0;

    true1541_cpu_execute();

    cpu_bus_val = (bus_data << 1) | (bus_clock << 0) | (bus_atn << 7);

#if BUS_DBG
    if(cpu_atn)
      printf("SB: cpu PC=%04x read  data:%d clock:%d, value read=%02x\n",
	   reg_pc,
	   (~cpu_data) & (~drive_data) & 1, (~cpu_clock) & (~drive_clock) & 1,
	   cpu_bus_val);
#endif

    return cpu_bus_val;
}

void serial_bus_pa_write(BYTE data)
{
    static int last_write = 0;

    if (!app_resources.true1541)
	return;

    true1541_cpu_execute();

    if ((cpu_atn == 0) && (data & 128))
	set_atn(1);

    if (!(data & 128))
	set_atn(0);

    cpu_atn = ((data & 128) >> 7);
    drive_data_modifier = (NOT(cpu_atn) ^ NOT(drive_atna));

    if (last_write != (data & 128)) {
#if BUS_DBG
	printf("CPU PC=%04x set ATN=%d\n",reg_pc, cpu_atn);
#endif
	resolve_bus_signals();
#if BUS_DBG
	if (traceflg)
	    printf("WRITE RESET\n");
#endif
    }

    last_write = data & 128;
}

/* This routine is called for VIA1 PCR (= CA2 and CB2).
   Although Cx2 uses three bits for control, we assume the calling routine has
   set bit 5 and bit 1 to the real output value for CB2 (DATA out) and CA2 (CLK
   out) resp. (25apr1997 AF) */

void serial_bus_pcr_write(BYTE data)
{
    static int last_write = 0;

    if (!app_resources.true1541)
	return;

    true1541_cpu_execute();

    cpu_data = ((data & 32) >> 5);
    cpu_clock = ((data & 2) >> 1);
    drive_data_modifier = (NOT(cpu_atn) ^ NOT(drive_atna));

    if (last_write != (data & 34)) {
#if BUS_DBG
	printf("CPU PC=%04x set DATA=%d, CLK=%d\n",reg_pc, cpu_data, cpu_clock);
#endif
	resolve_bus_signals();
#if BUS_DBG
	if (traceflg)
	    printf("WRITE RESET\n");
#endif
    }

    last_write = data & 34;

#if  0 /*BUS_DBG*/
    printf("SB: cpu write (%02x) pcr data:%d clock:%d atn:%d\n",
	   data, cpu_data, cpu_clock, cpu_atn);
#endif
}

#endif

/* ------------------------------------------------------------------------- */

/* Set the sync factor between the computer and the 1541. */

void true1541_set_pal_sync_factor(void)
{
    true1541_set_sync_factor(TRUE1541_PAL_SYNC_FACTOR);
}

void true1541_set_ntsc_sync_factor(void)
{
    true1541_set_sync_factor(TRUE1541_NTSC_SYNC_FACTOR);
}

void true1541_ack_sync_factor(void)
{
    true1541_set_sync_factor(app_resources.true1541SyncFactor);
}

/* ------------------------------------------------------------------------- */

/* C64-specific parallel cable handling.  */

#ifdef CBM64

void parallel_cable_cpu_write(BYTE data, int handshake)
{
    true1541_cpu_execute();
    if (handshake)
	viaD1_signal(VIA_SIG_CB1, VIA_SIG_FALL);
    parallel_cable_cpu_value = data;
}

BYTE parallel_cable_cpu_read(void)
{
    true1541_cpu_execute();
    viaD1_signal(VIA_SIG_CB1, VIA_SIG_FALL);
    return parallel_cable_cpu_value & parallel_cable_drive_value;
}

void parallel_cable_drive_write(BYTE data, int handshake)
{
    if (handshake)
	cia2_set_flag();
    parallel_cable_drive_value = data;
}

BYTE parallel_cable_drive_read(int handshake)
{
    if (handshake)
	cia2_set_flag();
    return parallel_cable_cpu_value & parallel_cable_drive_value;
}

#else  /* defined CBM64 */

/* These are dummies to make the other true1541-aware machines happy.  */

void parallel_cable_drive_write(BYTE data, int handshake)
{
}

BYTE parallel_cable_drive_read(int handshake)
{
    return 0;
}

#endif /* defined CBM64 */
