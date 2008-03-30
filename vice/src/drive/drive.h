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
#include "diskimage.h"
#include "drivecpu.h"
#include "gcr.h"
#include "log.h"

/* VIA 1 drive 0 interrupts.  */
#define	I_VIA1D0FL	0

/* VIA 2 drive 0 interrupts.  */
#define	I_VIA2D0FL	1

/* CIA 1571 drive 0 interrupts.  */
#define I_CIA1571D0FL	2

/* CIA 1581 drive 0 interrupts.  */
#define I_CIA1581D0FL   3

/* RIOT 1001 drive 0 interrupts. */
#define	I_RIOTD0FL	4


/* VIA 1 drive 1 interrupts. */
#define I_VIA1D1FL  	0

/* VIA 2 drive 1 interrupts. */
#define I_VIA2D1FL  	1

/* CIA 1571 drive 1 interrupts.  */
#define I_CIA1571D1FL	2

/* CIA 1581 drive 1 interrupts.  */
#define I_CIA1581D1FL   3

/* RIOT 1001 drive 1 interrupts. */
#define	I_RIOTD1FL	4

#define DRIVE_NUMOFINT 	5


#define DRIVE_ROM1541_SIZE   0x4000
#define DRIVE_ROM1541II_SIZE 0x4000
#define DRIVE_ROM1571_SIZE   0x8000
#define DRIVE_ROM1581_SIZE   0x8000
#define DRIVE_ROM2031_SIZE   0x4000
#define DRIVE_ROM1001_SIZE   0x4000	/* same as ROM8050 and ROM8250 !*/
#define DRIVE_ROM_SIZE       0x8000
#define DRIVE_RAM_SIZE       0x2000
#define DRIVE_ROM1541_CHECKSUM	1976666

/* Sync factors.  */
#define DRIVE_SYNC_PAL               -1
#define DRIVE_SYNC_NTSC              -2

/* Extended disk image handling.  */
#define DRIVE_EXTEND_NEVER		0
#define DRIVE_EXTEND_ASK		1
#define DRIVE_EXTEND_ACCESS		2

/* Drive idling methods.  */
#define DRIVE_IDLE_NO_IDLE     0
#define DRIVE_IDLE_SKIP_CYCLES 1
#define DRIVE_IDLE_TRAP_IDLE   2

/* Drive type.  */
#define DRIVE_TYPE_NONE      0
#define DRIVE_TYPE_1541   1541
#define DRIVE_TYPE_1541II 1542
#define DRIVE_TYPE_1571   1571
#define DRIVE_TYPE_1581   1581
#define DRIVE_TYPE_2031   2031
#define DRIVE_TYPE_1001   1001	/* DOS 2.7 single floppy drive, 1M/disk */

/* Possible colors of the drive active LED.  */
#define DRIVE_ACTIVE_RED     0
#define DRIVE_ACTIVE_GREEN   1

/* Number of cycles before an attached disk becomes visible to the R/W head.
   This is mostly to make routines that auto-detect disk changes happy.  */
#define DRIVE_ATTACH_DELAY           (2*600000)

/* Number of cycles the write protection is activated on detach.  */
#define DRIVE_DETACH_DELAY           (2*200000)

/* Number of cycles the after a disk can be inserted after a disk has been
   detached.  */
#define DRIVE_ATTACH_DETACH_DELAY    (2*400000)

/* Number of bytes in one raw sector.  */
#define NUM_BYTES_SECTOR_GCR 360

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

    /* Pointer to the start of the GCR data of this track.  */
    BYTE *GCR_track_start_ptr;

    /* Size of the GCR data for the current track.  */
    int GCR_current_track_size;

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

    /* Tick when the disk image was attached, but an old image was just
       detached.  */
    CLOCK attach_detach_clk;

    /* Byte to read from r/w head.  */
    BYTE GCR_read;

    unsigned long bits_moved;
    unsigned long accum;
    unsigned long shifter;
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

    /* Pointer to the attached disk image.  */
    disk_image_t *image;

    /* Pointer to the gcr image.  */
    gcr_t *gcr;
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
extern int drive_attach_image(disk_image_t *image, int unit);
extern int drive_detach_image(disk_image_t *image, int unit);
extern void drive_update_viad2_pcr(int pcrval, drive_t *dptr);
extern BYTE drive_read_viad2_prb(drive_t *dptr);
extern CLOCK drive_prevent_clk_overflow(CLOCK sub, int dnr);
extern void drive_vsync_hook(void);
extern void drive_set_1571_sync_factor(int new_sync, int dnr);
extern void drive_set_1571_side(int side, int dnr);
extern void drive_update_ui_status(void);
extern void drive_cpu_execute(CLOCK clk_value);
extern void drive_GCR_data_writeback(int dnr);
extern int drive_read_block(int track, int sector, BYTE *readdata, int dnr);
extern int drive_write_block(int track, int sector, BYTE *writedata, int dnr);

extern int reload_rom_1541(char *name);

extern int drive_write_snapshot_module(snapshot_t *s, int save_disks, 
                                       int save_roms);
extern int drive_read_snapshot_module(snapshot_t *s);

#ifdef AVOID_STATIC_ARRAYS
extern BYTE *drive_rom;
#else
extern BYTE drive_rom[DRIVE_ROM_SIZE];
#endif

extern void drive0_parallel_set_atn(int);
extern void drive1_parallel_set_atn(int);
extern void drive0_via_set_atn(int);
extern void drive1_via_set_atn(int);
extern void drive0_riot_set_atn(int);
extern void drive1_riot_set_atn(int);

extern int drive_match_bus(int drive_type, int drv, int bus_map);

extern int drive_num_leds(int drv);

#endif /* !_DRIVE_H */
