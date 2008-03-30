/*
 * drive.h - Hardware-level Commodore disk drive emulation.
 *
 * Written by
 *  Daniel Sladic (sladic@eecg.toronto.edu)
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Andreas Boose (boose@linux.rz.fh-hannover.de)
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

#ifndef _DRIVE_H
#define _DRIVE_H

#include "types.h"
#include "vdrive.h"		/* DRIVE */
#include "drivecpu.h"
#include "log.h"

/* VIA 1 drive 0 alarms.  */
#define	A_VIA1D0T1	0
#define	A_VIA1D0T2	1
/* VIA 2 drive 0 alarms.  */
#define	A_VIA2D0T1	2
#define	A_VIA2D0T2	3
/* CIA 1571 drive 0 alarms.  */
#define A_CIA1571D0TOD	4
#define A_CIA1571D0TA	5
#define A_CIA1571D0TB	6
/* CIA 1581 drive 0 alarms.  */
#define A_CIA1581D0TOD	7
#define A_CIA1581D0TA	8
#define A_CIA1581D0TB	9

/* VIA 1 drive 1 alarms.  */
#define A_VIA1D1T1	0
#define A_VIA1D1T2	1
/* VIA 2 drive 1 alarms.  */
#define A_VIA2D1T1	2
#define A_VIA2D1T2	3
/* CIA 1571 drive 1 alarms.  */
#define A_CIA1571D1TOD	4
#define A_CIA1571D1TA	5
#define A_CIA1571D1TB	6
/* CIA 1581 drive 1 alarms.  */
#define A_CIA1581D1TOD	7
#define A_CIA1581D1TA	8
#define A_CIA1581D1TB	9

#define DRIVE_NUMOFALRM 10

/* VIA 1 drive 0 interrupts.  */
#define	I_VIA1D0T1	0
#define	I_VIA1D0T2	1
#define	I_VIA1D0SR	2
#define	I_VIA1D0FL	3
#define I_VIA1D0CB1	4
#define I_VIA1D0CB2	5
#define I_VIA1D0CA1	6
#define I_VIA1D0CA2	7
/* VIA 2 drive 0 interrupts.  */
#define	I_VIA2D0T1	8
#define	I_VIA2D0T2	9
#define	I_VIA2D0SR	10
#define	I_VIA2D0FL	11
#define I_VIA2D0CB1	12
#define I_VIA2D0CB2	13
#define I_VIA2D0CA1	14
#define I_VIA2D0CA2	15
/* CIA 1571 drive 0 interrupts.  */
#define I_CIA1571D0FL	16
#define I_CIA1571D0TOD	17
#define I_CIA1571D0SP	18
#define I_CIA1571D0TA	19
#define I_CIA1571D0TB	20
/* CIA 1581 drive 0 interrupts.  */
#define I_CIA1581D0FL   21
#define I_CIA1581D0TOD  22
#define I_CIA1581D0SP   23
#define I_CIA1581D0TA   24
#define I_CIA1581D0TB   25

/* VIA 1 drive 1 interrupts. */
#define I_VIA1D1T1  0
#define I_VIA1D1T2  1
#define I_VIA1D1SR  2
#define I_VIA1D1FL  3
#define I_VIA1D1CB1 4
#define I_VIA1D1CB2 5
#define I_VIA1D1CA1 6
#define I_VIA1D1CA2 7
/* VIA 2 drive 1 interrupts. */
#define I_VIA2D1T1  8
#define I_VIA2D1T2  9
#define I_VIA2D1SR  10
#define I_VIA2D1FL  11
#define I_VIA2D1CB1 12
#define I_VIA2D1CB2 13
#define I_VIA2D1CA1 14
#define I_VIA2D1CA2 15
/* CIA 1571 drive 1 interrupts.  */
#define I_CIA1571D1FL	16
#define I_CIA1571D1TOD	17
#define I_CIA1571D1SP	18
#define I_CIA1571D1TA	19
#define I_CIA1571D1TB	20
/* CIA 1581 drive 1 interrupts.  */
#define I_CIA1581D1FL   21
#define I_CIA1581D1TOD  22
#define I_CIA1581D1SP   23
#define I_CIA1581D1TA   24
#define I_CIA1581D1TB   25

#define DRIVE_NUMOFINT 26

#define DRIVE_ROM1541_SIZE	0x4000
#define DRIVE_ROM1571_SIZE	0x8000
#define DRIVE_ROM1581_SIZE	0x8000
#define DRIVE_ROM2031_SIZE	0x4000
#define DRIVE_ROM_SIZE		0x8000
#define DRIVE_RAM_SIZE		0x2000
#define DRIVE_ROM1541_CHECKSUM	1976666

/* Sync factors.  */
#define DRIVE_SYNC_PAL               -1
#define DRIVE_SYNC_NTSC              -2

/* Extended disk image handling.  */
#define DRIVE_EXTEND_NEVER		0
#define DRIVE_EXTEND_ASK		1
#define DRIVE_EXTEND_ACCESS		2

/* Drive idling methods.  */
#define DRIVE_IDLE_NO_IDLE		0
#define DRIVE_IDLE_SKIP_CYCLES	1
#define DRIVE_IDLE_TRAP_IDLE		2

/* Drive type.  */
#define DRIVE_TYPE_NONE 0
#define DRIVE_TYPE_1541	1541
#define DRIVE_TYPE_1571	1571
#define DRIVE_TYPE_1581 1581
#define DRIVE_TYPE_2031 2031

/* Number of cycles before an attached disk becomes visible to the R/W head.
   This is mostly to make routines that auto-detect disk changes happy.  */
#define DRIVE_ATTACH_DELAY           500000

/* Number of cycles the write protection is activated on detach.  */
#define DRIVE_DETACH_DELAY           300000

/* Number of bytes in one raw sector.  */
#define NUM_BYTES_SECTOR_GCR 360

/* Number of bytes in one raw track.  */
#define NUM_MAX_BYTES_TRACK 7928

/* ------------------------------------------------------------------------- */

#define ROTATION_TABLE_SIZE      0x1000
#define ACCUM_MAX                0x10000

struct _rotation_table {
    unsigned long bits;
    unsigned long accum;
};

typedef struct drive_s {

    int led_status;

    /* Current half track on which the R/W head is positioned.  */
    int current_half_track;

    /* Pointer to the attached disk image.  */
    DRIVE *drive_floppy;

    /* Current ROM image.  */
    BYTE rom[DRIVE_ROM_SIZE];

    /* Is this drive enabled?  */
    int enable;

    /* What drive type we have to emulate?  */
    int type;

    /* Disk side.  */
    int side;

    /* What idling method?  (See `DRIVE_IDLE_*')  */
    int idling_method;

    /* Original ROM code is saved here.  */
    BYTE rom_idle_trap;

    /* Original ROM code of the checksum routine is saved here.  */
    BYTE rom_checksum[4];

    /* Byte ready line.  */
    int byte_ready;

    /* Disk ID.  */
    BYTE diskID1, diskID2;

    /* Flag: does the current need to be written out to disk?  */
    int GCR_dirty_track;

    /* GCR value being written to the disk.  */
    BYTE GCR_write_value;

    /* Raw GCR image of the disk.  */
    BYTE GCR_data[MAX_TRACKS_1571 * NUM_MAX_BYTES_TRACK];

    /* Pointer to the start of the GCR data of this track.  */
    BYTE *GCR_track_start_ptr;

    /* Speed zone image of the disk.  */
    BYTE GCR_speed_zone[MAX_TRACKS_1571 * NUM_MAX_BYTES_TRACK];

    /* Size of the GCR data for the current track.  */
    int GCR_current_track_size;

    /* Size of the GCR data of each track.  */
    int GCR_track_size[MAX_TRACKS_1571];

    /* Offset of the R/W head on the current track.  */
    int GCR_head_offset;

    /* Are we in read or write mode?  */
    int read_write_mode;

    /* Activates the byte ready line.  */
    int byte_ready_active;

    /* Clock frequency of this drive in 1MHz units.  */
    int clock_frequency;

    /* Tick when the disk image was attached.  */
    CLOCK attach_clk;

    /* Tick when the disk image was detached.  */
    CLOCK detach_clk;

    /* Byte to read from r/w head.  */
    BYTE GCR_read;

    unsigned long bits_moved;
    unsigned long accum;
    int finish_byte;
    int last_mode;
    CLOCK rotation_last_clk;

    /* Used for disk change detection.  */
    int have_new_disk;

    struct _rotation_table *rotation_table_ptr;
    struct _rotation_table rotation_table[4][ROTATION_TABLE_SIZE];

    /* UI stuff.  */
    int old_led_status;
    int old_half_track;

    /* Is a GCR image loaded?  */
    int GCR_image_loaded;

    /* is this disk read only?  */
    int read_only;

    /* What extension policy?  */
    int extend_image_policy;

    /* Flag: Do we emulate a SpeedDOS-compatible parallel cable?  */
    int parallel_cable_enabled;

    /* If the user does not want to extend the disk image and `ask mode' is
    selected this flag gets cleared.  */
    int ask_extend_disk_image;

    /* Pointer to the drive clock.  */
    CLOCK *clk;

    /* Drive-specific logging goes here.  */
    log_t log;
} drive_t;

extern drive_t drive[2];

extern int drive_init_resources(void);
extern int drive_init_cmdline_options(void);
extern int drive_init(CLOCK pal_hz, CLOCK ntsc_hz);
extern void serial_bus_drive_write(BYTE data);
extern BYTE serial_bus_drive_read(void);
extern void drive0_mem_init(int type);
extern void drive1_mem_init(int type);
extern void drive_move_head(int step, int dnr);
extern void drive_rotate_disk(drive_t *dptr);
extern void drive_reset(void);
extern int drive_attach_floppy(DRIVE *floppy);
extern int drive_detach_floppy(DRIVE *floppy);
extern void drive_update_viad2_pcr(int pcrval, drive_t *dptr);
extern BYTE drive_read_viad2_prb(drive_t *dptr);
extern void drive_prevent_clk_overflow(CLOCK sub, int dnr);
extern void drive_vsync_hook(void);
extern void drive_set_1571_sync_factor(int sync, int dnr);
extern void drive_set_1571_side(int side, int dnr);
extern void drive_update_ui_status(void);
extern void drive_cpu_execute(void);

extern int drive_write_snapshot_module(snapshot_t *s, int save_disks, 
                                       int save_roms);
extern int drive_read_snapshot_module(snapshot_t *s);

#ifdef AVOID_STATIC_ARRAYS
extern BYTE *drive_rom;
extern BYTE *drive0_ram;
extern BYTE *drive1_ram;
#else
extern BYTE drive_rom[DRIVE_ROM_SIZE];
extern BYTE drive0_ram[DRIVE_RAM_SIZE];
extern BYTE drive1_ram[DRIVE_RAM_SIZE];
#endif

extern void drive0_parallel_set_atn(int);
extern void drive1_parallel_set_atn(int);

extern int drive_match_bus(int drive_type, int bus_map);

#endif /* !_DRIVE_H */
