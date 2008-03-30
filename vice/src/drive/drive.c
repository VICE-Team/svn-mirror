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

#include "vice.h"

#ifdef STDC_HEADERS
#ifdef __riscos
#include "ROlib.h"
#else
#include <fcntl.h>
#endif
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if defined OS2 || defined _MSC_VER
#include <sys/types.h>
#endif

#include "attach.h"
#include "ciad.h"
#include "clkguard.h"
#include "cmdline.h"
#include "drive.h"
#include "fdc.h"
#include "gcr.h"
#include "iecdrive.h"
#include "log.h"
#include "resources.h"
#include "riotd.h"
#include "serial.h"
#include "snapshot.h"
#include "sysfile.h"
#include "ui.h"
#include "utils.h"
#include "vdrive.h"
#include "viad.h"
#include "wd1770.h"
#include "zfile.h"

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
static int set_drive0_idling_method(resource_value_t v);
static int set_drive1_idling_method(resource_value_t v);
static void drive_initialize_rom_traps(int dnr);
static int drive_check_image_format(int format, int dnr);

static int drive_load_1541(void);
static int drive_load_1541ii(void);
static int drive_load_1571(void);
static int drive_load_1581(void);
static int drive_load_2031(void);
static int drive_load_1001(void);

/* Generic drive logging goes here.  */
static log_t drive_log = LOG_ERR;

/* Is true drive emulation switched on?  */
static int drive_true_emulation;

/* Pointer to the IEC bus structure.  */
static iec_info_t *iec_info;

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
static char *dos_rom_name_1541 = 0;
static char default_dos_rom_name_1541[] = "dos1541";
static char *dos_rom_name_1541ii = 0;
static char *dos_rom_name_1571 = 0;
static char *dos_rom_name_1581 = 0;
static char *dos_rom_name_2031 = 0;
static char *dos_rom_name_1001 = 0;

/* If nonzero, at least one vaild drive ROM has already been loaded.  */
static int rom_loaded = 0;

/* If nonzero, we are far enough in init that we can load ROMs */
static int drive_rom_load_ok = 0;

static int set_drive_true_emulation(resource_value_t v)
{
    drive_true_emulation = (int) v;
    if ((int) v) {
        if (drive[0].type != DRIVE_TYPE_NONE) {
            drive[0].enable = 1;
            drive0_cpu_reset_clk();
        }
        if (drive[1].type != DRIVE_TYPE_NONE) {
            drive[1].enable = 1;
            drive1_cpu_reset_clk();
        }
        drive_enable(0);
        drive_enable(1);
        iec_calculate_callback_index();
    } else {
        drive_disable(0);
        drive_disable(1);

	/* update BAM after true drive emulation having probably
	   changed the BAM on disk (14May1999) */
	if (drive[0].drive_floppy != NULL) {
	    vdrive_bam_read_bam(drive[0].drive_floppy);
	}
	if (drive[1].drive_floppy != NULL) {
	    vdrive_bam_read_bam(drive[1].drive_floppy);
	}
    }
    return 0;
}

static int set_drive0_type(resource_value_t v)
{
    int type = (int) v;
    int busses = iec_available_busses();

    /* if bus for drive type is not allowed, set to default value for bus */
    if (!drive_match_bus(type, 0, busses)) {
	if (busses & IEC_BUS_IEC) {
	    type = DRIVE_TYPE_1541;
	} else
	if (busses & IEC_BUS_IEEE) {
	    type = DRIVE_TYPE_2031;
	} else
	    type = DRIVE_TYPE_NONE;
    }

    switch (type) {
      case DRIVE_TYPE_1541:
      case DRIVE_TYPE_1541II:
      case DRIVE_TYPE_1571:
      case DRIVE_TYPE_1581:
      case DRIVE_TYPE_2031:
      case DRIVE_TYPE_1001:
        if (drive[0].type != type) {
	    drive[0].current_half_track = 2 * ((type == DRIVE_TYPE_1001)
								? 38 : 18);
        }
        drive[0].type = type;
        if (drive_true_emulation) {
            drive[0].enable = 1;
            drive_enable(0);
            iec_calculate_callback_index();
        }
        drive_set_disk_drive_type(type, 0);
        drive_initialize_rom_traps(0);
        set_drive0_idling_method((resource_value_t) drive[0].idling_method);
        return 0;
      case DRIVE_TYPE_NONE:
        drive[0].type = type;
        drive_disable(0);
        return 0;
      default:
        return -1;
    }
}

static int set_drive1_type(resource_value_t v)
{
    int type = (int) v;
    int busses = iec_available_busses();

    /* if bus for drive type is not allowed, set to default value for bus */
    if (!drive_match_bus(type, 1, busses)) {
	if (busses & IEC_BUS_IEC) {
	    type = DRIVE_TYPE_1541;
	} else
	if (busses & IEC_BUS_IEEE) {
	    type = DRIVE_TYPE_2031;
	} else
	    type = DRIVE_TYPE_NONE;
    }

    switch (type) {
      case DRIVE_TYPE_1541:
      case DRIVE_TYPE_1541II:
      case DRIVE_TYPE_1571:
      case DRIVE_TYPE_1581:
      case DRIVE_TYPE_2031:
      case DRIVE_TYPE_1001:
        if (drive[1].type != type) {
	    drive[1].current_half_track = 2 * ((type == DRIVE_TYPE_1001)
								? 38 : 18);
        }
        drive[1].type = type;
        if (drive_true_emulation) {
            drive[1].enable = 1;
            drive_enable(1);
            iec_calculate_callback_index();
        }
        drive_set_disk_drive_type(type, 1);
        drive_initialize_rom_traps(1);
        set_drive1_idling_method((resource_value_t) drive[1].idling_method);
        return 0;
      case DRIVE_TYPE_NONE:
        drive[1].type = type;
        drive_disable(1);
        return 0;
      default:
        return -1;
    }
}

static int set_drive0_parallel_cable_enabled(resource_value_t v)
{
    drive[0].parallel_cable_enabled = (int) v;
    return 0;
}

static int set_drive1_parallel_cable_enabled(resource_value_t v)
{
    drive[1].parallel_cable_enabled = (int) v;
    return 0;
}

static int set_drive0_extend_image_policy(resource_value_t v)
{
    switch ((int) v) {
      case DRIVE_EXTEND_NEVER:
      case DRIVE_EXTEND_ASK:
      case DRIVE_EXTEND_ACCESS:
        drive[0].extend_image_policy = (int) v;
        return 0;
      default:
        return -1;
    }
}

static int set_drive1_extend_image_policy(resource_value_t v)
{
    switch ((int) v) {
      case DRIVE_EXTEND_NEVER:
      case DRIVE_EXTEND_ASK:
      case DRIVE_EXTEND_ACCESS:
        drive[1].extend_image_policy = (int) v;
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

    drive[0].idling_method = (int) v;

    if (rom_loaded && drive[0].type == DRIVE_TYPE_1541) {
        if (drive[0].idling_method == DRIVE_IDLE_TRAP_IDLE) {
            drive[0].rom[0xeae4 - 0x8000] = 0xea;
            drive[0].rom[0xeae5 - 0x8000] = 0xea;
            drive[0].rom[0xeae8 - 0x8000] = 0xea;
            drive[0].rom[0xeae9 - 0x8000] = 0xea;
            drive[0].rom[0xec9b - 0x8000] = 0x00;
        } else {
            drive[0].rom[0xeae4 - 0x8000] = drive[0].rom_checksum[0];
            drive[0].rom[0xeae5 - 0x8000] = drive[0].rom_checksum[1];
            drive[0].rom[0xeae8 - 0x8000] = drive[0].rom_checksum[2];
            drive[0].rom[0xeae9 - 0x8000] = drive[0].rom_checksum[3];
            drive[0].rom[0xec9b - 0x8000] = drive[0].rom_idle_trap;
        }
    }
    return 0;
}
static int set_drive1_idling_method(resource_value_t v)
{
    /* FIXME: Maybe we should call `drive[01]_cpu_execute()' here?  */
    if ((int) v != DRIVE_IDLE_SKIP_CYCLES
        && (int) v != DRIVE_IDLE_TRAP_IDLE
        && (int) v != DRIVE_IDLE_NO_IDLE)
        return -1;

    drive[1].idling_method = (int) v;

    if (rom_loaded && drive[1].type == DRIVE_TYPE_1541) {
        if (drive[1].idling_method == DRIVE_IDLE_TRAP_IDLE) {
            drive[1].rom[0xeae4 - 0x8000] = 0xea;
            drive[1].rom[0xeae5 - 0x8000] = 0xea;
            drive[1].rom[0xeae8 - 0x8000] = 0xea;
            drive[1].rom[0xeae9 - 0x8000] = 0xea;
            drive[1].rom[0xec9b - 0x8000] = 0x00;
        } else {
            drive[1].rom[0xeae4 - 0x8000] = drive[1].rom_checksum[0];
            drive[1].rom[0xeae5 - 0x8000] = drive[1].rom_checksum[1];
            drive[1].rom[0xeae8 - 0x8000] = drive[1].rom_checksum[2];
            drive[1].rom[0xeae9 - 0x8000] = drive[1].rom_checksum[3];
            drive[1].rom[0xec9b - 0x8000] = drive[1].rom_idle_trap;
        }
    }
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

static int set_dos_rom_name_1001(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_1001 != NULL && name != NULL
        && strcmp(name, dos_rom_name_1001) == 0)
        return 0;

    string_set(&dos_rom_name_1001, name);

    return drive_load_1001();
}

static int set_dos_rom_name_2031(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_2031 != NULL && name != NULL
        && strcmp(name, dos_rom_name_2031) == 0)
        return 0;

    string_set(&dos_rom_name_2031, name);

    return drive_load_2031();
}

static int set_dos_rom_name_1541(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_1541 != NULL && name != NULL
        && strcmp(name, dos_rom_name_1541) == 0)
        return 0;

    string_set(&dos_rom_name_1541, name);

    return drive_load_1541();
}

static int set_dos_rom_name_1541ii(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_1541ii != NULL && name != NULL
        && strcmp(name, dos_rom_name_1541ii) == 0)
        return 0;

    string_set(&dos_rom_name_1541ii, name);

    return drive_load_1541ii();
}

static int set_dos_rom_name_1571(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_1571 != NULL && name != NULL
        && strcmp(name, dos_rom_name_1571) == 0)
        return 0;

    string_set(&dos_rom_name_1571, name);

    return drive_load_1571();
}

static int set_dos_rom_name_1581(resource_value_t v)
{
    const char *name = (const char *) v;

    if (dos_rom_name_1581 != NULL && name != NULL
        && strcmp(name, dos_rom_name_1581) == 0)
        return 0;

    string_set(&dos_rom_name_1581, name);

    return drive_load_1581();
}


static resource_t resources[] = {
    { "DriveTrueEmulation", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &drive_true_emulation, set_drive_true_emulation },
    { "Drive8Type", RES_INTEGER, (resource_value_t) DRIVE_TYPE_1541,
      (resource_value_t *) &(drive[0].type), set_drive0_type },
    { "Drive9Type", RES_INTEGER, (resource_value_t) DRIVE_TYPE_NONE,
      (resource_value_t *) &(drive[1].type), set_drive1_type },
    { "Drive8ParallelCable", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[0].parallel_cable_enabled),
       set_drive0_parallel_cable_enabled },
    { "Drive9ParallelCable", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &(drive[1].parallel_cable_enabled),
       set_drive1_parallel_cable_enabled },
    { "Drive8ExtendImagePolicy", RES_INTEGER,
      (resource_value_t) DRIVE_EXTEND_NEVER, (resource_value_t *)
      &(drive[0].extend_image_policy), set_drive0_extend_image_policy },
    { "Drive9ExtendImagePolicy", RES_INTEGER,
      (resource_value_t) DRIVE_EXTEND_NEVER, (resource_value_t *)
      &(drive[1].extend_image_policy), set_drive1_extend_image_policy },
    { "Drive8IdleMethod", RES_INTEGER, (resource_value_t) DRIVE_IDLE_TRAP_IDLE,
      (resource_value_t *) &(drive[0].idling_method), set_drive0_idling_method },
    { "Drive9IdleMethod", RES_INTEGER, (resource_value_t) DRIVE_IDLE_TRAP_IDLE,
      (resource_value_t *) &(drive[1].idling_method), set_drive1_idling_method },
    { "DriveSyncFactor", RES_INTEGER, (resource_value_t) DRIVE_SYNC_PAL,
      (resource_value_t *) &sync_factor, set_sync_factor },
    { "DosName1541", RES_STRING, (resource_value_t) "dos1541",
      (resource_value_t *) &dos_rom_name_1541, set_dos_rom_name_1541 },
    { "DosName1541ii", RES_STRING, (resource_value_t) "d1541II",
      (resource_value_t *) &dos_rom_name_1541ii, set_dos_rom_name_1541ii },
    { "DosName1571", RES_STRING, (resource_value_t) "dos1571",
      (resource_value_t *) &dos_rom_name_1571, set_dos_rom_name_1571 },
    { "DosName1581", RES_STRING, (resource_value_t) "dos1581",
      (resource_value_t *) &dos_rom_name_1581, set_dos_rom_name_1581 },
    { "DosName2031", RES_STRING, (resource_value_t) "dos2031",
      (resource_value_t *) &dos_rom_name_2031, set_dos_rom_name_2031 },
    { "DosName1001", RES_STRING, (resource_value_t) "dos1001",
      (resource_value_t *) &dos_rom_name_1001, set_dos_rom_name_1001 },
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
    { "-drive8type", SET_RESOURCE, 1, NULL, NULL, "Drive8Type",
      (resource_value_t) DRIVE_TYPE_1541, "<type>",
      "Set drive type (0: no drive)" },
    { "-drive9type", SET_RESOURCE, 1, NULL, NULL, "Drive9Type",
      (resource_value_t) DRIVE_TYPE_NONE, "<type>",
      "Set drive type (0: no drive)" },
    { "-parallel8", SET_RESOURCE, 0, NULL, NULL, "Drive8ParallelCable",
      (resource_value_t) 1,
      NULL, "Enable SpeedDOS-compatible parallel cable" },
    { "+parallel8", SET_RESOURCE, 0, NULL, NULL, "Drive8ParallelCable",
      (resource_value_t) 0,
      NULL, "Disable SpeedDOS-compatible parallel cable" },
    { "-parallel9", SET_RESOURCE, 0, NULL, NULL, "Drive9ParallelCable",
      (resource_value_t) 1,
      NULL, "Enable SpeedDOS-compatible parallel cable" },
    { "+parallel9", SET_RESOURCE, 0, NULL, NULL, "Drive9ParallelCable",
      (resource_value_t) 0,
      NULL, "Disable SpeedDOS-compatible parallel cable" },
    { "-drive8idle", SET_RESOURCE, 1, NULL, NULL, "Drive8IdleMethod",
      (resource_value_t) DRIVE_IDLE_TRAP_IDLE, "<method>",
      "Set drive idling method (0: no traps, 1: skip cycles, 2: trap idle)" },
    { "-drive9idle", SET_RESOURCE, 1, NULL, NULL, "Drive9IdleMethod",
      (resource_value_t) DRIVE_IDLE_TRAP_IDLE, "<method>",
      "Set drive idling method (0: no traps, 1: skip cycles, 2: trap idle)" },
    { "-drive8extend", SET_RESOURCE, 1, NULL, NULL, "Drive8ExtendImagePolicy",
      (resource_value_t) DRIVE_EXTEND_NEVER, "<method>",
      "Set drive 8 40 track extension policy (0: never, 1: ask, 2: on access)"},
    { "-drive9extend", SET_RESOURCE, 1, NULL, NULL, "Drive9ExtendImagePolicy",
      (resource_value_t) DRIVE_EXTEND_NEVER, "<method>",
      "Set drive 9 40 track extension policy (0: never, 1: ask, 2: on access)"},
    { "-drivesync", SET_RESOURCE, 1, NULL, NULL, "DriveSyncFactor",
      (resource_value_t) DRIVE_SYNC_PAL, "<value>",
      "Set drive sync factor to <value>" },
    { "-paldrive", SET_RESOURCE, 0, NULL, NULL, "DriveSyncFactor",
      (resource_value_t) DRIVE_SYNC_PAL,
      NULL, "Use PAL drive sync factor" },
    { "-ntscdrive", SET_RESOURCE, 0, NULL, NULL, "DriveSyncFactor",
      (resource_value_t) DRIVE_SYNC_NTSC,
      NULL, "Use NTSC drive sync factor" },
    { "-dos1541", SET_RESOURCE, 1, NULL, NULL, "DosName1541", "dos1541",
      "<name>", "Specify name of 1541 DOS ROM image name" },
    { "-dos1541II", SET_RESOURCE, 1, NULL, NULL, "DosName1541II", "dos1541II",
      "<name>", "Specify name of 1541-II DOS ROM image name" },
    { "-dos1571", SET_RESOURCE, 1, NULL, NULL, "DosName1571", "dos1571",
      "<name>", "Specify name of 1571 DOS ROM image name" },
    { "-dos1581", SET_RESOURCE, 1, NULL, NULL, "DosName1581", "dos1581",
      "<name>", "Specify name of 1581 DOS ROM image name" },
    { "-dos2031", SET_RESOURCE, 1, NULL, NULL, "DosName2031", "dos2031",
      "<name>", "Specify name of 2031 DOS ROM image name" },
    { "-dos1001", SET_RESOURCE, 1, NULL, NULL, "DosName1001", "dos1001",
      "<name>", "Specify name of 1001/8050/8250 DOS ROM image name" },
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
static BYTE *drive_rom1541ii;
static BYTE *drive_rom1571;
static BYTE *drive_rom1581;
static BYTE *drive_rom2031;
static BYTE *drive_rom1001;
#else
static BYTE drive_rom1541[DRIVE_ROM1541_SIZE];
static BYTE drive_rom1541ii[DRIVE_ROM1541II_SIZE];
static BYTE drive_rom1571[DRIVE_ROM1571_SIZE];
static BYTE drive_rom1581[DRIVE_ROM1581_SIZE];
static BYTE drive_rom2031[DRIVE_ROM2031_SIZE];
static BYTE drive_rom1001[DRIVE_ROM1001_SIZE];
#endif

/* If nonzero, the ROM image has been loaded.  */
static int rom1541_loaded = 0;
static int rom1541ii_loaded = 0;
static int rom1571_loaded = 0;
static int rom1581_loaded = 0;
static int rom2031_loaded = 0;
static int rom1001_loaded = 0;

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

/* Clock speed of the PAL and NTSC versions of the connected computer.  */
static CLOCK pal_cycles_per_sec;
static CLOCK ntsc_cycles_per_sec;

static int drive_led_color[2];

#define GCR_OFFSET(track, sector)  ((track - 1) * NUM_MAX_BYTES_TRACK \
				    + sector * NUM_BYTES_SECTOR_GCR)

static void initialize_rotation(int freq, int dnr);
static void initialize_rotation_table(int freq, int dnr);
static void drive_extend_disk_image(int dnr);
static void drive_set_half_track(int num, drive_t *dptr);
inline static BYTE drive_sync_found(drive_t *dptr);
inline static BYTE drive_write_protect_sense(drive_t *dptr);
static int drive_load_rom_images(void);
static void drive_setup_rom_image(int dnr);
static int drive_write_image_snapshot_module(snapshot_t *s, int dnr);
static int drive_write_gcrimage_snapshot_module(snapshot_t *s, int dnr);
static int drive_read_image_snapshot_module(snapshot_t *s, int dnr);
static int drive_read_gcrimage_snapshot_module(snapshot_t *s, int dnr);
static int drive_write_rom_snapshot_module(snapshot_t *s, int dnr);
static int drive_read_rom_snapshot_module(snapshot_t *s, int dnr);
static void drive_clk_overflow_callback(CLOCK sub, void *data);
static void drive_set_clock_frequency(int type, int dnr);

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
        || drive[dnr].type == DRIVE_TYPE_1541II
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

    drive_set_half_track(drive[dnr].current_half_track, &drive[dnr]);

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
				   drive[dnr].drive_floppy->D64_Header,
                                   drive[dnr].drive_floppy->GCR_Header,
                                   drive[dnr].drive_floppy->unit);
	    if (rc < 0) {
		log_error(drive[dnr].log,
                          "Cannot read T:%d S:%d from disk image.",
                          track, sector);
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

    fseek(drive[dnr].drive_floppy->ActiveFd, 12, SEEK_SET);
    if (read_dword(drive[dnr].drive_floppy->ActiveFd, gcr_track_p,
                   NumTracks * 8) < 0) {
	log_error(drive[dnr].log, "Could not read GCR disk image.");
	return 0;
    }

    fseek(drive[dnr].drive_floppy->ActiveFd, 12 + NumTracks * 8, SEEK_SET);
    if (read_dword(drive[dnr].drive_floppy->ActiveFd, gcr_speed_p,
                   NumTracks * 8) < 0) {
	log_error(drive[dnr].log, "Could not read GCR disk image.");
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

	    fseek(drive[dnr].drive_floppy->ActiveFd, offset, SEEK_SET);
	    if (fread((char *)len, 2, 1, drive[dnr].drive_floppy->ActiveFd) < 1) {
		log_error(drive[dnr].log, "Could not read GCR disk image.");
		return 0;
	    }

	    track_len = len[0] + len[1] * 256;

	    if (track_len < 5000 || track_len > 7928) {
		log_error(drive[dnr].log,
                          "Track field length %i is not supported.",
                          track_len);
		return 0;
	    }

	    drive[dnr].GCR_track_size[track] = track_len;

	    fseek(drive[dnr].drive_floppy->ActiveFd, offset + 2, SEEK_SET);
	    if (fread((char *)track_data, track_len, 1,
                drive[dnr].drive_floppy->ActiveFd) < 1) {
		log_error(drive[dnr].log, "Could not read GCR disk image.");
		return 0;
	    }

	    zone_len = (track_len + 3) / 4;

	    if (gcr_speed_p[track * 2] > 3) {

		offset = gcr_speed_p[track * 2];

		fseek(drive[dnr].drive_floppy->ActiveFd, offset, SEEK_SET);
		if (fread((char *)comp_speed, zone_len, 1,
                drive[dnr].drive_floppy->ActiveFd) < 1) {
		    log_error(drive[dnr].log,
                              "Could not read GCR disk image.");
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

    fseek(drive[dnr].drive_floppy->ActiveFd, 12, SEEK_SET);
    if (read_dword(drive[dnr].drive_floppy->ActiveFd, gcr_track_p,
                   NumTracks * 8) < 0) {
	log_error(drive[dnr].log, "Could not read GCR disk image header.");
	return;
    }

    fseek(drive[dnr].drive_floppy->ActiveFd, 12 + NumTracks * 8, SEEK_SET);
    if (read_dword(drive[dnr].drive_floppy->ActiveFd, gcr_speed_p,
                   NumTracks * 8) < 0) {
	log_error(drive[dnr].log, "Could not read GCR disk image header.");
	return;
    }

    if (gcr_track_p[(track - 1) * 2] == 0) {
	offset = fseek(drive[dnr].drive_floppy->ActiveFd, 0, SEEK_END);
	if (offset < 0) {
	    log_error(drive[dnr].log, "Could not extend GCR disk image.");
	    return;
	}
	gcr_track_p[(track - 1) * 2] = offset;
    }

    offset = gcr_track_p[(track - 1) * 2];

    len[0] = drive[dnr].GCR_track_size[track - 1] % 256;
    len[1] = drive[dnr].GCR_track_size[track - 1] / 256;

    if (fseek(drive[dnr].drive_floppy->ActiveFd, offset, SEEK_SET) < 0
        || fwrite((char *)len, 2, 1, drive[dnr].drive_floppy->ActiveFd) < 1) {
	log_error(drive[dnr].log, "Could not write GCR disk image.");
	return;
    }

    /* Clear gap between the end of the actual track and the start of
       the next track.  */
    gap = NUM_MAX_BYTES_TRACK - drive[dnr].GCR_track_size[track - 1];
    if (gap > 0)
	memset(drive[dnr].GCR_track_start_ptr + drive[dnr].GCR_track_size[track - 1], 0, gap);

    if (fseek(drive[dnr].drive_floppy->ActiveFd, offset + 2, SEEK_SET) < 0
        || fwrite((char *)drive[dnr].GCR_track_start_ptr, NUM_MAX_BYTES_TRACK,
        1, drive[dnr].drive_floppy->ActiveFd) < 1) {
	log_error(drive[dnr].log, "Could not write GCR disk image.");
	return;
    }

    for (i = 0; (drive[dnr].GCR_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK]
                 == drive[dnr].GCR_speed_zone[(track - 1) * NUM_MAX_BYTES_TRACK + i])
             && i < NUM_MAX_BYTES_TRACK; i++);

    if (i < drive[dnr].GCR_track_size[track - 1]) {
	/* This will change soon.  */
	log_error(drive[dnr].log,
                  "Saving different speed zones is not supported yet.");
	return;
    }

    if (gcr_speed_p[(track - 1) * 2] >= 4) {
	/* This will change soon.  */
	log_error(drive[dnr].log,
                  "Adding new speed zones is not supported yet.");
	return;
    }

    offset = 12 + NumTracks * 8 + (track - 1) * 8;
    if (fseek(drive[dnr].drive_floppy->ActiveFd, offset, SEEK_SET) < 0
        || write_dword(drive[dnr].drive_floppy->ActiveFd,
                       &gcr_speed_p[(track - 1) * 2], 4) < 0) {
        log_error(drive[dnr].log, "Could not write GCR disk image.");
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

    if (fseek(drive[dnr].drive_floppy->ActiveFd, offset, SEEK_SET) < 0
        || fwrite((char *)comp_speed, NUM_MAX_BYTES_TRACK / 4, 1
        drive[dnr].drive_floppy->ActiveFd) < 1) {
        log_error(drive[dnr].log, "Could not write GCR disk image");
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
			   buffer, 18, 0, drive[dnr].drive_floppy->D64_Header,
               drive[dnr].drive_floppy->GCR_Header,
               drive[dnr].drive_floppy->unit);
    if (rc >= 0) {
	drive[dnr].diskID1 = buffer[0xa2];
	drive[dnr].diskID2 = buffer[0xa3];
    }

    return rc;
}

static BYTE *GCR_find_sector_header(int track, int sector, int dnr,
                                    BYTE *GCR_track_start_ptr,
                                    int GCR_current_track_size)
{
    BYTE *offset = GCR_track_start_ptr;
    BYTE *GCR_track_end = GCR_track_start_ptr + GCR_current_track_size;
    BYTE GCR_header[5], header_data[4];
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
	    if((++sync_count) >= GCR_current_track_size)
		return NULL;
	}

	for (i = 0; i < 5; i++) {
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

static BYTE *GCR_find_sector_data(BYTE *offset, int dnr,
                                  BYTE *GCR_track_start_ptr,
                                  int GCR_current_track_size)
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

/* Global clock counters.  */
CLOCK drive_clk[2];

/* Initialize the hardware-level drive emulation (should be called at least
   once before anything else).  Return 0 on success, -1 on error.  */
int drive_init(CLOCK pal_hz, CLOCK ntsc_hz)
{
    int track, i;

    if (rom_loaded)
        return 0;

    pal_cycles_per_sec = pal_hz;
    ntsc_cycles_per_sec = ntsc_hz;

    drive[0].log = log_open("Drive 8");
    drive[1].log = log_open("Drive 9");
    drive_log = log_open("Drive");
    if (drive[0].log == LOG_ERR || drive[1].log == LOG_ERR
        || drive_log == LOG_ERR)
        return -1;

#ifdef AVOID_STATIC_ARRAYS
    drive_rom1541 = xmalloc(DRIVE_ROM1541_SIZE);
    drive_rom1541ii = xmalloc(DRIVE_ROM1541II_SIZE);
    drive_rom1571 = xmalloc(DRIVE_ROM1571_SIZE);
    drive_rom1581 = xmalloc(DRIVE_ROM1581_SIZE);
    drive_rom2031 = xmalloc(DRIVE_ROM2031_SIZE);
    drive_rom1001 = xmalloc(DRIVE_ROM1001_SIZE);
#endif

    drive_clk[0] = 0L;
    drive_clk[1] = 0L;
    drive[0].clk = &drive_clk[0];
    drive[1].clk = &drive_clk[1];

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

    log_message(drive_log, "Finished loading ROM images.");
    rom_loaded = 1;

    drive_setup_rom_image(0);
    drive_setup_rom_image(1);

    clk_guard_add_callback(&drive0_clk_guard, drive_clk_overflow_callback,
                           (void *) 0);
    clk_guard_add_callback(&drive1_clk_guard, drive_clk_overflow_callback,
                           (void *) 1);

    for (i = 0; i < 2; i++) {
        drive[i].byte_ready = 1;
        drive[i].GCR_dirty_track = 0;
        drive[i].GCR_write_value = 0x55;
        drive[i].GCR_track_start_ptr = drive[i].GCR_data;
        drive[i].attach_clk = (CLOCK)0;
        drive[i].detach_clk = (CLOCK)0;
        drive[i].attach_detach_clk = (CLOCK)0;
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
        drive[i].clock_frequency = 1;
        for (track = 0; track < MAX_TRACKS_1541; track++)
            drive[i].GCR_track_size[track] =
                raw_track_size[speed_map_1541[track]];
        /* Position the R/W head on the directory track.  */
        drive_set_half_track(36, &drive[i]);
        drive_led_color[i] = DRIVE_ACTIVE_RED;
    }

    drive_initialize_rom_traps(0);
    drive_initialize_rom_traps(1);

    drive_set_clock_frequency(drive[0].type, 0);
    drive_set_clock_frequency(drive[1].type, 1);

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

static void drive_set_active_led_color(int type, int dnr)
{
    switch (type) {
      case DRIVE_TYPE_1541:
        drive_led_color[dnr] = DRIVE_ACTIVE_RED;
        break;
      case DRIVE_TYPE_1541II:
        drive_led_color[dnr] = DRIVE_ACTIVE_GREEN;
        break;
      case DRIVE_TYPE_1571:
        drive_led_color[dnr] = DRIVE_ACTIVE_RED;
        break;
      case DRIVE_TYPE_1581:
        drive_led_color[dnr] = DRIVE_ACTIVE_GREEN;
        break;
      case DRIVE_TYPE_2031:
        drive_led_color[dnr] = DRIVE_ACTIVE_RED;
        break;
      case DRIVE_TYPE_1001:
        drive_led_color[dnr] = DRIVE_ACTIVE_RED;
        break;
      default:
        drive_led_color[dnr] = DRIVE_ACTIVE_RED;
    }
}

static void drive_set_clock_frequency(int type, int dnr)
{
    switch (type) {
      case DRIVE_TYPE_1541:
        drive[dnr].clock_frequency = 1;
        break;
      case DRIVE_TYPE_1541II:
        drive[dnr].clock_frequency = 1;
        break;
      case DRIVE_TYPE_1571:
        drive[dnr].clock_frequency = 1;
        break;
      case DRIVE_TYPE_1581:
        drive[dnr].clock_frequency = 2;
        break;
      case DRIVE_TYPE_2031:
        drive[dnr].clock_frequency = 1;
        break;
      case DRIVE_TYPE_1001:
        drive[dnr].clock_frequency = 1;
        break;
      default:
        drive[dnr].clock_frequency = 1;
    }
}
static int drive_set_disk_drive_type(int type, int dnr)
{
    switch (type) {
      case DRIVE_TYPE_1541:
        if (rom1541_loaded < 1 && rom_loaded)
            return -1;
        if (drive[dnr].byte_ready_active == 0x06)
            drive_rotate_disk(&drive[dnr]);
        break;
      case DRIVE_TYPE_1541II:
        if (rom1541ii_loaded < 1 && rom_loaded)
            return -1;
        if (drive[dnr].byte_ready_active == 0x06)
            drive_rotate_disk(&drive[dnr]);
        break;
      case DRIVE_TYPE_1571:
        if (rom1571_loaded < 1 && rom_loaded)
            return -1;
        if (drive[dnr].byte_ready_active == 0x06)
            drive_rotate_disk(&drive[dnr]);
        break;
      case DRIVE_TYPE_1581:
        if (rom1581_loaded < 1 && rom_loaded)
            return -1;
        if (drive[dnr].byte_ready_active == 0x06)
            drive_rotate_disk(&drive[dnr]);
        break;
      case DRIVE_TYPE_2031:
        if (rom2031_loaded < 1 && rom_loaded)
            return -1;
        if (drive[dnr].byte_ready_active == 0x06)
            drive_rotate_disk(&drive[dnr]);
        break;
      case DRIVE_TYPE_1001:
        if (rom1001_loaded < 1 && rom_loaded)
            return -1;
        if (drive[dnr].byte_ready_active == 0x06)
            drive_rotate_disk(&drive[dnr]);
        break;
      default:
        return -1;
    }

    drive_set_clock_frequency(type, dnr);

    initialize_rotation(0, dnr);
    drive[dnr].type = type;
    drive[dnr].side = 0;
    drive_setup_rom_image(dnr);
    set_sync_factor((resource_value_t) sync_factor);
    drive_set_active_led_color(type, dnr);

    if (dnr == 0)
        drive0_cpu_init(type);
    if (dnr == 1)
        drive1_cpu_init(type);

    return 0;
}

static int drive_do_1541_checksum(void)
{
    int i;
    unsigned long s;

    /* Calculate ROM checksum.  */
    for (i = 0, s = 0; i < DRIVE_ROM1541_SIZE; i++)
        s += drive_rom1541[i];

    if (s != DRIVE_ROM1541_CHECKSUM)
        log_warning(drive_log, "Unknown 1541 ROM image.  Sum: %lu.", s);

    return 0;
}

static int drive_load_1541(void)
{
    if (!drive_rom_load_ok) return 0;

    /* Load the ROMs. */
    if (sysfile_load(dos_rom_name_1541, drive_rom1541, DRIVE_ROM1541_SIZE,
        DRIVE_ROM1541_SIZE) < 0) {
        log_error(drive_log,
                  "1541 ROM image not found.  "
                  "Hardware-level 1541 emulation is not available.");
    } else {
        rom1541_loaded = 1;
        return drive_do_1541_checksum();
    }
    return -1;
}

static int drive_load_1541ii(void)
{
    if (!drive_rom_load_ok) return 0;

    if (sysfile_load(dos_rom_name_1541ii, drive_rom1541ii,
        DRIVE_ROM1541II_SIZE, DRIVE_ROM1541II_SIZE) < 0) {
        log_error(drive_log,
                  "1541-II ROM image not found.  "
                  "Hardware-level 1541-II emulation is not available.");
    } else {
        rom1541ii_loaded = 1;
	return 0;
    }
    return -1;
}

static int drive_load_1571(void)
{
    if (!drive_rom_load_ok) return 0;

    if (sysfile_load(dos_rom_name_1571, drive_rom1571, DRIVE_ROM1571_SIZE,
        DRIVE_ROM1571_SIZE) < 0) {
        log_error(drive_log,
                  "1571 ROM image not found.  "
                  "Hardware-level 1571 emulation is not available.");
    } else {
        rom1571_loaded = 1;
	return 0;
    }
    return -1;
}

static int drive_load_1581(void)
{
    if (!drive_rom_load_ok) return 0;

    if (sysfile_load(dos_rom_name_1581, drive_rom1581, DRIVE_ROM1581_SIZE,
        DRIVE_ROM1581_SIZE) < 0) {
        log_error(drive_log,
                  "1581 ROM image not found.  "
                  "Hardware-level 1581 emulation is not available.");
    } else {
        rom1581_loaded = 1;
	return 0;
    }
    return -1;
}

static int drive_load_2031(void)
{
    if (!drive_rom_load_ok) return 0;

    if (sysfile_load(dos_rom_name_2031, drive_rom2031, DRIVE_ROM2031_SIZE,
        DRIVE_ROM2031_SIZE) < 0) {
        log_error(drive_log,
                  "2031 ROM image not found.  "
                  "Hardware-level 2031 emulation is not available.");
    } else {
        rom2031_loaded = 1;
	return 0;
    }
    return -1;
}

static int drive_load_1001(void)
{
    if (!drive_rom_load_ok) return 0;

    if (sysfile_load(dos_rom_name_1001, drive_rom1001, DRIVE_ROM1001_SIZE,
        DRIVE_ROM1001_SIZE) < 0) {
        log_error(drive_log,
                  "1001 ROM image not found.  "
                  "Hardware-level 1001 emulation is not available.");
    } else {
        rom1001_loaded = 1;
	return 0;
    }
    return -1;
}

static int drive_load_rom_images(void)
{
    drive_rom_load_ok = 1;

    drive_load_1541();

    drive_load_1541ii();

    drive_load_1571();

    drive_load_1581();

    drive_load_2031();

    drive_load_1001();

    /* FIXME: Drive type radio button should be made insensitive here
       if a ROM image is not loaded. */

    if (!rom1541_loaded
        && !rom1541ii_loaded
        && !rom1571_loaded
        && !rom1581_loaded
        && !rom2031_loaded) {
        log_error(drive_log,
                  "No ROM image found at all!  "
                  "Hardware-level emulation is not available.");
        return -1;
    }

    return 0;
}

static void drive_setup_rom_image(int dnr)
{
    if (rom_loaded) {
        switch (drive[dnr].type) {
          case DRIVE_TYPE_1541:
            memcpy(&(drive[dnr].rom[0x4000]), drive_rom1541,
                   DRIVE_ROM1541_SIZE);
            break;
          case DRIVE_TYPE_1541II:
            memcpy(&(drive[dnr].rom[0x4000]), drive_rom1541ii,
                   DRIVE_ROM1541II_SIZE);
            break;
          case DRIVE_TYPE_1571:
            memcpy(drive[dnr].rom, drive_rom1571, DRIVE_ROM1571_SIZE);
            break;
          case DRIVE_TYPE_1581:
            memcpy(drive[dnr].rom, drive_rom1581, DRIVE_ROM1581_SIZE);
            break;
          case DRIVE_TYPE_2031:
            memcpy(&(drive[dnr].rom[0x4000]), drive_rom2031,
                   DRIVE_ROM2031_SIZE);
	    break;
          case DRIVE_TYPE_1001:
            memcpy(&(drive[dnr].rom[0x4000]), drive_rom1001,
                   DRIVE_ROM1001_SIZE);
            break;
        }
    }
}

static void drive_initialize_rom_traps(int dnr)
{
    if (drive[dnr].type == DRIVE_TYPE_1541) {
        /* Save the ROM check.  */
        drive[dnr].rom_checksum[0] = drive[dnr].rom[0xeae4 - 0x8000];
        drive[dnr].rom_checksum[1] = drive[dnr].rom[0xeae5 - 0x8000];
        drive[dnr].rom_checksum[2] = drive[dnr].rom[0xeae8 - 0x8000];
        drive[dnr].rom_checksum[3] = drive[dnr].rom[0xeae9 - 0x8000];
        /* Save the idle trap.  */
        drive[dnr].rom_idle_trap = drive[dnr].rom[0xec9b - 0x8000];

        if (drive[dnr].idling_method == DRIVE_IDLE_TRAP_IDLE) {
            drive[dnr].rom[0xeae4 - 0x8000] = 0xea;
            drive[dnr].rom[0xeae5 - 0x8000] = 0xea;
            drive[dnr].rom[0xeae8 - 0x8000] = 0xea;
            drive[dnr].rom[0xeae9 - 0x8000] = 0xea;
            drive[dnr].rom[0xec9b - 0x8000] = 0x00;
        }
    }

    if (drive[dnr].type == DRIVE_TYPE_1581) {
        /* Save the ROM check.  */
        /*drive[dnr].rom_checksum[0] = drive[dnr].rom[0xeae4 - 0x8000];
        drive[dnr].rom_checksum[1] = drive[dnr].rom[0xeae5 - 0x8000];
        drive[dnr].rom_checksum[2] = drive[dnr].rom[0xeae8 - 0x8000];
        drive[dnr].rom_checksum[3] = drive[dnr].rom[0xeae9 - 0x8000];*/
        /* Save the idle trap.  */
        /*drive[dnr].rom_idle_trap = drive[dnr].rom[0xec9b - 0x8000];*/
        drive[dnr].rom[0xaf6f - 0x8000] = 0x4c;
        drive[dnr].rom[0xaf70 - 0x8000] = 0xca;
        drive[dnr].rom[0xaf71 - 0x8000] = 0xaf;
        drive[dnr].rom[0xc0be - 0x8000] = 0x00;
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

    drive_set_active_led_color(drive[dnr].type, dnr);
    ui_enable_drive_status((drive[0].enable ? UI_DRIVE_ENABLE_0 : 0)
                           | (drive[1].enable ? UI_DRIVE_ENABLE_1 : 0),
                           drive_led_color);

    return 0;
}

/* Disable full drive emulation.  */
static void drive_disable(int dnr)
{
    int i;

    /* This must come first, because this might be called before the true
       drive initialization.  */
    drive[dnr].enable = 0;
    iec_calculate_callback_index();

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
        drive_GCR_data_writeback(0);
    if (dnr == 1)
        drive_GCR_data_writeback(1);
    }

    /* Make sure the UI is updated.  */
    for (i = 0; i < 2; i++) {
        if (drive[i].enable) {
            drive[i].old_led_status = -1;
            drive[i].old_half_track = -1;
        }
    }

    ui_enable_drive_status((drive[0].enable ? UI_DRIVE_ENABLE_0 : 0)
                           | (drive[1].enable ? UI_DRIVE_ENABLE_1 : 0),
                           drive_led_color);
}

void drive_reset(void)
{
    drive0_cpu_reset();
    drive1_cpu_reset();
}

/* ------------------------------------------------------------------------- */
/* Check if the drive type matches the disk image type.  */
static int drive_check_image_format(int format, int dnr)
{
    switch (format) {
      case 1541:
        if (drive[dnr].type != DRIVE_TYPE_1541
            && drive[dnr].type != DRIVE_TYPE_1541II
            && drive[dnr].type != DRIVE_TYPE_1571
            && drive[dnr].type != DRIVE_TYPE_2031)
            return -1;
        break;
      case 1571:
        if (drive[dnr].type != DRIVE_TYPE_1571)
            return -1;
        break;
      case 1581:
        if (drive[dnr].type != DRIVE_TYPE_1581)
            return -1;
        break;
      case 8050:
      case 8250:
        if (drive[dnr].type != DRIVE_TYPE_1001)
            return -1;
	break;
      default:
        return -1;
    }
    return 0;
}

/* Attach a disk image to the true drive emulation. */
int drive_attach_floppy(DRIVE *floppy)
{
    int dnr;

    if (floppy->unit != 8 && floppy->unit != 9)
        return -1;

    dnr = floppy->unit - 8;

    drive[dnr].drive_floppy = floppy;

    if (drive_check_image_format(floppy->ImageFormat, dnr) < 0)
        return 0;

    drive[dnr].read_only = drive[dnr].drive_floppy->ReadOnly;
    drive[dnr].have_new_disk = 1;
    drive[dnr].attach_clk = drive_clk[dnr];
    if (drive[dnr].detach_clk > (CLOCK) 0)
        drive[dnr].attach_detach_clk = drive_clk[dnr];
    drive[dnr].ask_extend_disk_image = 1;

    if (floppy->ImageFormat != 1581
	&& floppy->ImageFormat != 8050
	&& floppy->ImageFormat != 8250) {
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
    }
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
        log_error(drive_log, "Whaaat?  Attempt for bogus drive detachment!");
        return -1;
    } else {
        if (drive[dnr].drive_floppy != NULL) {
            if (floppy->ImageFormat == 1541 || floppy->ImageFormat == 1571) {
                drive_GCR_data_writeback(dnr);
                memset(drive[dnr].GCR_data, 0, sizeof(drive[dnr].GCR_data));
            }
            drive[dnr].detach_clk = drive_clk[dnr];
            drive[dnr].drive_floppy = NULL;
            drive[dnr].GCR_image_loaded = 0;
        }
    }
    return 0;
}

/* ------------------------------------------------------------------------- */

/* Initialization.  */

static void initialize_rotation(int freq, int dnr)
{
    initialize_rotation_table(freq, dnr);
    drive[dnr].bits_moved = drive[dnr].accum = 0;
}

static void initialize_rotation_table(int freq, int dnr)
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
}
/* ------------------------------------------------------------------------- */

/* Clock overflow handing.  */

static void drive_clk_overflow_callback(CLOCK sub, void *data)
{
    unsigned int drive_num;
    drive_t *d;

    log_warning(drive_log, "drive_clk_overflow_callback");

    drive_num = (unsigned int) data;
    d = &drive[drive_num];

    if (d->byte_ready_active == 0x06)
        drive_rotate_disk(&drive[drive_num]);
    d->rotation_last_clk -= sub;
    if (d->attach_clk > (CLOCK) 0)
        d->attach_clk -= sub;
    if (d->detach_clk > (CLOCK) 0)
        d->detach_clk -= sub;
    if (d->attach_detach_clk > (CLOCK) 0)
        d->attach_detach_clk -= sub;

    /* FIXME: Having to do this by hand sucks *big time*!  These should be in
       `drive_t'.  */
    switch (drive_num) {
      case 0:
        alarm_context_time_warp(&drive0_alarm_context, sub, -1);
        cpu_int_status_time_warp(&drive0_int_status, sub, -1);
        break;
      case 1:
        alarm_context_time_warp(&drive1_alarm_context, sub, -1);
        cpu_int_status_time_warp(&drive1_int_status, sub, -1);
        break;
      default:
        log_error(drive_log,
                  "Unexpected drive number %d in drive_clk_overflow_callback",
                  drive_num);
    }
}

CLOCK drive_prevent_clk_overflow(CLOCK sub, int dnr)
{
    /* FIXME: Having to do this by hand sucks *big time*!  */
    switch (dnr) {
      case 0:
        return drive0_cpu_prevent_clk_overflow(sub);
      case 1:
        return drive1_cpu_prevent_clk_overflow(sub);
      default:
        log_error(drive_log,
                  "Unexpected drive number %d in `drive_prevent_clk_overflow()'\n",
                  dnr);
        return 0;
    }
}

/*-------------------------------------------------------------------------- */

/* The following functions are time critical.  */

/* Return non-zero if the Sync mark is found.  It is required to
   call drive_rotate_disk() to update drive[].GCR_head_offset first.
   The return value corresponds to bit#7 of VIA2 PRB. This means 0x0
   is returned when sync is found and 0x80 is returned when no sync
   is found.  */
inline static BYTE drive_sync_found(drive_t *dptr)
{
    BYTE val = dptr->GCR_track_start_ptr[dptr->GCR_head_offset];

    if (val != 0xff || dptr->last_mode == 0) {
        return 0x80;
    } else {
        int previous_head_offset = (dptr->GCR_head_offset > 0
            ? dptr->GCR_head_offset - 1
            : dptr->GCR_current_track_size - 1);

        if (dptr->GCR_track_start_ptr[previous_head_offset] != 0xff) {
            if (dptr->shifter >= 2) {
                int next_head_offset = ((dptr->GCR_head_offset
                    < (dptr->GCR_current_track_size - 1))
                    ? dptr->GCR_head_offset + 1 : 0);
                if ((dptr->GCR_track_start_ptr[next_head_offset] & 0xc0)
                    == 0xc0)
                    return 0;
            }
            return 0x80;
        }
        /* As the current rotation code cannot cope with non byte aligned
           writes, do not change `drive[].bits_moved'!  */
        /* dptr->bits_moved = 0; */
        return 0;
    }
}

/* Rotate the disk according to the current value of `drive_clk[]'.  If
   `mode_change' is non-zero, there has been a Read -> Write mode switch.  */
void drive_rotate_disk(drive_t *dptr)
{
    unsigned long new_bits;

    /* Calculate the number of bits that have passed under the R/W head since
       the last time.  */

    CLOCK delta = *(dptr->clk) - dptr->rotation_last_clk;

    new_bits = 0;
    while (delta > 0) {
        if (delta >= ROTATION_TABLE_SIZE) {
            struct _rotation_table *p = (dptr->rotation_table_ptr
                                         + ROTATION_TABLE_SIZE - 1);
            new_bits += p->bits;
            dptr->accum += p->accum;
            delta -= ROTATION_TABLE_SIZE - 1;
        } else {
            struct _rotation_table *p = dptr->rotation_table_ptr + delta;
            new_bits += p->bits;
            dptr->accum += p->accum;
            delta = 0;
        }
        if (dptr->accum >= ACCUM_MAX) {
            dptr->accum -= ACCUM_MAX;
            new_bits++;
        }
    }

    dptr->shifter = dptr->bits_moved + new_bits;

    if (dptr->shifter >= 8) {

	dptr->bits_moved += new_bits;
	dptr->rotation_last_clk = *(dptr->clk);

	if (dptr->finish_byte) {
	    if (dptr->last_mode == 0) { /* write */
		dptr->GCR_dirty_track = 1;
		if (dptr->bits_moved >= 8) {
		    dptr->GCR_track_start_ptr[dptr->GCR_head_offset]
		        = dptr->GCR_write_value;
		    dptr->GCR_head_offset = ((dptr->GCR_head_offset + 1) %
                                             dptr->GCR_current_track_size);
		    dptr->bits_moved -= 8;
		}
	    } else {		/* read */
		if (dptr->bits_moved >= 8) {
		    dptr->GCR_head_offset = ((dptr->GCR_head_offset + 1) %
                                             dptr->GCR_current_track_size);
		    dptr->bits_moved -= 8;
		    dptr->GCR_read = dptr->GCR_track_start_ptr[dptr->GCR_head_offset];
		}
	    }

	    dptr->finish_byte = 0;
	    dptr->last_mode = dptr->read_write_mode;
	}

	if (dptr->last_mode == 0) {	/* write */
	    dptr->GCR_dirty_track = 1;
	    while (dptr->bits_moved >= 8) {
		dptr->GCR_track_start_ptr[dptr->GCR_head_offset]
		    = dptr->GCR_write_value;
		dptr->GCR_head_offset = ((dptr->GCR_head_offset + 1)
                                         % dptr->GCR_current_track_size);
		dptr->bits_moved -= 8;
	    }
	} else {		/* read */
	    dptr->GCR_head_offset = ((dptr->GCR_head_offset
                                      + dptr->bits_moved / 8)
                                     % dptr->GCR_current_track_size);
	    dptr->bits_moved %= 8;
	    dptr->GCR_read = dptr->GCR_track_start_ptr[dptr->GCR_head_offset];
	}

        dptr->shifter = dptr->bits_moved;

        /* The byte ready line is only set when no sync is found.  */
	if (drive_sync_found(dptr))
	    dptr->byte_ready = 1;
    } /* if (dptr->shifter >= 8) */
}


/* Move the head to half track `num'.  */
static void drive_set_half_track(int num, drive_t *dptr)
{
    if ((dptr->type == DRIVE_TYPE_1541 || dptr->type == DRIVE_TYPE_1541II
        || dptr->type == DRIVE_TYPE_2031) && num > 84)
        num = 84;
    if (dptr->type == DRIVE_TYPE_1571 && num > 140)
        num = 140;
    if (num < 2)
        num = 2;

    dptr->current_half_track = num;
    dptr->GCR_track_start_ptr = (dptr->GCR_data
			   + ((dptr->current_half_track / 2 - 1) * NUM_MAX_BYTES_TRACK));

    dptr->GCR_current_track_size =
        dptr->GCR_track_size[dptr->current_half_track / 2 - 1];
    dptr->GCR_head_offset = 0;
}

/* Return the write protect sense status. */
inline static BYTE drive_write_protect_sense(drive_t *dptr)
{
    /* Set the write protection bit for the time the disk is pulled out on
       detach.  */
    if (dptr->detach_clk != (CLOCK)0) {
        if ((*(dptr->clk)) - dptr->detach_clk < DRIVE_DETACH_DELAY)
            return 0x10;
        dptr->detach_clk = (CLOCK)0;
    }
    /* Clear the write protection bit for the minimum time until a new disk
       can be inserted.  */
    if (dptr->attach_detach_clk != (CLOCK)0) {
        if ((*(dptr->clk)) - dptr->attach_detach_clk
            < DRIVE_ATTACH_DETACH_DELAY)
            return 0x0;
        dptr->attach_detach_clk = (CLOCK)0;
    }
    /* Set the write protection bit for the time the disk is put in on
       attach.  */
    if (dptr->attach_clk != (CLOCK)0) {
        if (((*(dptr->clk)) - dptr->attach_clk < DRIVE_ATTACH_DELAY))
            return 0x10;
        dptr->attach_clk = (CLOCK)0;
    }

    if (dptr->GCR_image_loaded == 0) {
        /* No disk in drive, write protection is on. */
        return 0x0;
    } else if (dptr->have_new_disk) {
        /* Disk has changed, make sure the drive sees at least one change in
           the write protect status. */
        dptr->have_new_disk = 0;
        return dptr->read_only ? 0x10 : 0x0;
    } else {
        return dptr->read_only ? 0x0 : 0x10;
    }
}

void drive_update_viad2_pcr(int pcrval, drive_t *dptr)
{
    dptr->read_write_mode = pcrval & 0x20;
    dptr->byte_ready_active = (dptr->byte_ready_active & ~0x02)
                              | (pcrval & 0x02);
}

BYTE drive_read_viad2_prb(drive_t *dptr)
{
    if (dptr->byte_ready_active == 0x06)
        drive_rotate_disk(dptr);
    return drive_sync_found(dptr) | drive_write_protect_sense(dptr);
}

/* End of time critical functions.  */
/*-------------------------------------------------------------------------- */

void drive_set_1571_side(int side, int dnr)
{
    int num = drive[dnr].current_half_track;
    if (drive[dnr].byte_ready_active == 0x06)
        drive_rotate_disk(&drive[dnr]);
    drive_GCR_data_writeback(dnr);
    drive[dnr].side = side;
    if (num > 70)
        num -= 70;
    num += side * 70;
    drive_set_half_track(num, &drive[dnr]);
}

/* Increment the head position by `step' half-tracks. Valid values
   for `step' are `+1' and `-1'.  */
void drive_move_head(int step, int dnr)
{
    drive_GCR_data_writeback(dnr);
    if (drive[dnr].type == DRIVE_TYPE_1571) {
        if (drive[dnr].current_half_track + step == 71)
            return;
    }
    drive_set_half_track(drive[dnr].current_half_track + step, &drive[dnr]);
}

/* Hack... otherwise you get internal compiler errors when optimizing on gcc2.7.2 on RISC OS */
static void GCR_data_writeback2(BYTE *buffer, BYTE *offset, int dnr, int track, int sector)
{
    int rc;

    convert_GCR_to_sector(buffer, offset,
                          drive[dnr].GCR_track_start_ptr,
                          drive[dnr].GCR_current_track_size);
    if (buffer[0] != 0x7) {
        log_error(drive[dnr].log,
                  "Could not find data block id of T:%d S:%d.",
                  track, sector);
    } else {
        rc = floppy_write_block(drive[dnr].drive_floppy->ActiveFd,
                                drive[dnr].drive_floppy->ImageFormat,
                                buffer + 1, track, sector,
                                drive[dnr].drive_floppy->D64_Header,
                                drive[dnr].drive_floppy->GCR_Header,
                                drive[dnr].drive_floppy->unit);
        if (rc < 0)
            log_error(drive[dnr].log,
                      "Could not update T:%d S:%d.", track, sector);
    }
}

void drive_GCR_data_writeback(int dnr)
{
    int extend, track, sector, max_sector = 0;
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
            switch (drive[dnr].extend_image_policy) {
              case DRIVE_EXTEND_NEVER:
                drive[dnr].ask_extend_disk_image = 1;
                return;
              case DRIVE_EXTEND_ASK:
                if (drive[dnr].ask_extend_disk_image == 1) {
                    extend = ui_extend_image_dialog();
                    if (extend == 0) {
                        drive[dnr].ask_extend_disk_image = 0;
                        return;
                    } else {
                        drive_extend_disk_image(dnr);
                    }
                } else {
                    return;
                }
                break;
              case DRIVE_EXTEND_ACCESS:
                drive[dnr].ask_extend_disk_image = 1;
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

        offset = GCR_find_sector_header(track, sector, dnr,
                                        drive[dnr].GCR_track_start_ptr,
                                        drive[dnr].GCR_current_track_size);
        if (offset == NULL) {
            log_error(drive[dnr].log,
                      "Could not find header of T:%d S:%d.",
                      track, sector);
        } else {
            offset = GCR_find_sector_data(offset, dnr,
                                          drive[dnr].GCR_track_start_ptr,
                                          drive[dnr].GCR_current_track_size);
            if (offset == NULL) {
                log_error(drive[dnr].log,
                          "Could not find data sync of T:%d S:%d.",
                          track, sector);
            } else {
                GCR_data_writeback2(buffer, offset, dnr, track, sector);
            }
        }
    }
}

int drive_read_block(int track, int sector, BYTE *readdata, int dnr)
{
    BYTE buffer[260], *offset;
    BYTE *GCR_track_start_ptr;
    int GCR_current_track_size;

    if (track > drive[dnr].drive_floppy->NumTracks)
        return -1;

    /* Make sure data is flushed to the file before reading.  */
    drive_GCR_data_writeback(dnr);

    GCR_track_start_ptr = drive[dnr].GCR_data
               + ((track - 1) * NUM_MAX_BYTES_TRACK);
    GCR_current_track_size = drive[dnr].GCR_track_size[track - 1];

    offset = GCR_find_sector_header(track, sector, dnr,
                                    GCR_track_start_ptr,
                                    GCR_current_track_size);
    if (offset == NULL)
        return -1;
    offset = GCR_find_sector_data(offset, dnr, GCR_track_start_ptr,
                                  GCR_current_track_size);
    if (offset == NULL)
        return -1;

    convert_GCR_to_sector(buffer, offset, GCR_track_start_ptr,
                          GCR_current_track_size);
    if (buffer[0] != 0x7)
        return -1;

    memcpy(readdata, &buffer[1], 256);
    return 0;
}

int drive_write_block(int track, int sector, BYTE *writedata, int dnr)
{
    BYTE buffer[260], gcr_buffer[325], *offset, *buf, *gcr;
    BYTE chksum;
    BYTE *GCR_track_start_ptr;
    int GCR_current_track_size;
    int i;

    if (track > drive[dnr].drive_floppy->NumTracks)
        return -1;

    GCR_track_start_ptr = drive[dnr].GCR_data
               + ((track - 1) * NUM_MAX_BYTES_TRACK);
    GCR_current_track_size = drive[dnr].GCR_track_size[track - 1];

    offset = GCR_find_sector_header(track, sector, dnr,
                                    GCR_track_start_ptr,
                                    GCR_current_track_size);
    if (offset == NULL)
        return -1;
    offset = GCR_find_sector_data(offset, dnr, GCR_track_start_ptr,
                                  GCR_current_track_size);
    if (offset == NULL)
        return -1;

    buffer[0] = 0x7;
    memcpy(&buffer[1], writedata, 256);
    chksum = buffer[1];
    for (i = 2; i < 257; i++)
        chksum ^= buffer[i];
    buffer[257] = chksum;
    buffer[258] = buffer[259] = 0;

    buf = buffer;
    gcr = gcr_buffer;

    for (i = 0; i < 65; i++) {
        convert_4bytes_to_GCR(buf, gcr);
        buf += 4;
        gcr += 5;
    }

    for (i = 0; i < 325; i++) {
        *offset = gcr_buffer[i];
        offset++;
        if (offset == GCR_track_start_ptr + GCR_current_track_size)
            offset = GCR_track_start_ptr;
    }

    write_track_gcr(track, dnr);
    return 0;
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
                            drive[dnr].drive_floppy->D64_Header,
                            drive[dnr].drive_floppy->GCR_Header,
                            drive[dnr].drive_floppy->unit);
	if (rc < 0)
	    log_error(drive[dnr].log,
                      "Could not update T:%d S:%d.", track, sector);
	}
    }
}

void drive_cpu_execute(CLOCK clk_value)
{
    if (drive[0].enable)
        drive0_cpu_execute(clk_value);
    if (drive[1].enable)
        drive1_cpu_execute(clk_value);
}

int drive_match_bus(int drive_type, int unit, int bus_map)
{
    if ( (drive_type == DRIVE_TYPE_NONE)
      || ((drive_type == DRIVE_TYPE_2031 || drive_type == DRIVE_TYPE_1001)
	&& (bus_map & IEC_BUS_IEEE))
      || ((drive_type != DRIVE_TYPE_2031 && drive_type != DRIVE_TYPE_1001)
	&& (bus_map & IEC_BUS_IEC))
    ) {
        return 1;
    }
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
    if (pal_cycles_per_sec != 0) {
        int new_sync_factor = (int) floor(65536.0 * (1000000.0 /
                                         ((double)pal_cycles_per_sec)));
        drive_set_sync_factor(new_sync_factor);
    }
}

static void drive_set_ntsc_sync_factor(void)
{
    if (ntsc_cycles_per_sec != 0) {
        int new_sync_factor = (int) floor(65536.0 * (1000000.0 /
                                         ((double)ntsc_cycles_per_sec)));

        drive_set_sync_factor(new_sync_factor);
    }
}

void drive_set_1571_sync_factor(int new_sync, int dnr)
{
    if (rom_loaded) {
        if (drive[dnr].byte_ready_active == 0x06)
            drive_rotate_disk(&drive[dnr]);
        initialize_rotation(new_sync ? 1 : 0, dnr);
        drive[dnr].clock_frequency = (new_sync) ? 2 : 1;
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
#ifdef __riscos
                ui_display_drive_track_int(i, drive[i].current_half_track);
#else
                ui_display_drive_track(i,
                                       ((float) drive[i].current_half_track
                                       / 2.0));
#endif
            }
        }
    }
}

/* This is called at every vsync.  */
void drive_vsync_hook(void)
{
    drive_update_ui_status();
    if (drive[0].idling_method != DRIVE_IDLE_SKIP_CYCLES && drive[0].enable)
        drive0_cpu_execute(clk);
    if (drive[1].idling_method != DRIVE_IDLE_SKIP_CYCLES && drive[1].enable)
        drive1_cpu_execute(clk);
    wd1770_vsync_hook();
}

/* ------------------------------------------------------------------------- */

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
        if (drive0_cpu_write_snapshot_module(s) < 0)
            return -1;
        if (drive[0].type == DRIVE_TYPE_1541
            || drive[0].type == DRIVE_TYPE_1541II
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
	if (drive[0].type == DRIVE_TYPE_1001) {
	    if (riot1d0_write_snapshot_module(s) < 0
		|| riot2d0_write_snapshot_module(s) < 0
		|| fdc_write_snapshot_module(s, 0) < 0)
		return -1;
	}
    }
    if (drive[1].enable) {
        if (drive1_cpu_write_snapshot_module(s) < 0)
            return -1;
        if (drive[1].type == DRIVE_TYPE_1541
            || drive[1].type == DRIVE_TYPE_1541II
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
	if (drive[1].type == DRIVE_TYPE_1001) {
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

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);
    if (m == NULL) {
        /* If this module is not found true emulation is off.  */
        set_drive_true_emulation((resource_value_t) 0);
        return 0;
    }

    if (major_version > DRIVE_SNAP_MAJOR || minor_version > DRIVE_SNAP_MINOR) {
        log_error(drive_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  DRIVE_SNAP_MAJOR, DRIVE_SNAP_MINOR);
    }

    /* If this module exists true emulation is enabled.  */
    drive_true_emulation = 1;

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
        drive[i].GCR_track_start_ptr = (drive[i].GCR_data
                           + ((drive[i].current_half_track / 2 - 1)
                              * NUM_MAX_BYTES_TRACK));
        if (drive[i].type != DRIVE_TYPE_1571) {
            if (drive[i].type == DRIVE_TYPE_1581) {
                initialize_rotation_table(1, i);
                set_sync_factor((resource_value_t) sync_factor);
            } else {
                drive[i].side = 0;
                initialize_rotation_table(0, i);
                set_sync_factor((resource_value_t) sync_factor);
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
        drive[0].enable = 1;
        drive_setup_rom_image(0);
        drive0_mem_init(drive[0].type);
        set_drive0_idling_method((resource_value_t) drive[0].idling_method);
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
        drive[1].enable = 1;
        drive_setup_rom_image(1);
        drive1_mem_init(drive[1].type);
        set_drive1_idling_method((resource_value_t) drive[1].idling_method);
        drive_initialize_rom_traps(1);
        drive_set_active_led_color(drive[1].type, 1);
        break;
      case DRIVE_TYPE_NONE:
        drive_disable(1);
        break;
      default:
        return -1;
    }

    /* Clear parallel cable before undumping parallel port values.  */
    parallel_cable_drive0_write(0xff, 0);
    parallel_cable_drive1_write(0xff, 0);

    if (drive[0].enable) {
        if (drive0_cpu_read_snapshot_module(s) < 0)
            return -1;
        if (drive[0].type == DRIVE_TYPE_1541
            || drive[0].type == DRIVE_TYPE_1541II
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
	if (drive[0].type == DRIVE_TYPE_1001) {
	    if (riot1d0_read_snapshot_module(s) < 0
		|| riot2d0_read_snapshot_module(s) < 0
		|| fdc_read_snapshot_module(s, 0) < 0)
		return -1;
	}
    }

    if (drive[1].enable) {
        if (drive1_cpu_read_snapshot_module(s) < 0)
            return -1;
        if (drive[1].type == DRIVE_TYPE_1541
            || drive[1].type == DRIVE_TYPE_1541II
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
	if (drive[1].type == DRIVE_TYPE_1001) {
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

    if (drive[dnr].drive_floppy == NULL) {
	return 0;
    }

    sprintf(snap_module_name, "IMAGE%i", dnr);

    m = snapshot_module_create(s, snap_module_name, IMAGE_SNAP_MAJOR,
                               IMAGE_SNAP_MINOR);
    if (m == NULL)
       return -1;

    word = drive[dnr].drive_floppy->ImageFormat;
    snapshot_module_write_word(m, word);

    /* we use the return code to step through the tracks. So we do not
       need any geometry info. */
    for (track = 1; ; track++) {
	rc = 0;
	for (sector = 0; ; sector++) {
	    rc = floppy_read_block(drive[dnr].drive_floppy->ActiveFd,
				   drive[dnr].drive_floppy->ImageFormat,
				   sector_data, track, sector,
				   drive[dnr].drive_floppy->D64_Header,
                                   drive[dnr].drive_floppy->GCR_Header,
                                   drive[dnr].drive_floppy->unit);
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
        log_error(drive_log,
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
	log_error(drive_log, "Snapshot of disk image unknown (type %d)",
		(int)word);
        snapshot_module_close(m);
	return -1;
    }

    /* create temporary file of the right size */
    p = tmpnam(filename);
    if (!p) {
	log_error(drive_log, "Could not create temporary filename");
        snapshot_module_close(m);
	return -1;
    }
    fp = fopen(filename, "w+b");
    if (!fp) {
	log_error(drive_log, "Could not create temporary file (%s)",
			strerror(errno));
	log_error(drive_log, "filename=%s", filename);
        snapshot_module_close(m);
	return -1;
    }
    /* blow up the file to needed size */
    if (fseek(fp, len - 1, SEEK_SET) < 0
	|| (fputc(0, fp) == EOF)) {
	log_error(drive_log, "Could not create large temporary file");
	fclose(fp);
        snapshot_module_close(m);
	return -1;
    }
    fclose(fp);
    if (file_system_attach_disk(dnr + 8, filename) < 0) {
        log_error(drive_log, "Invalid Disk Image");
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
	    rc = floppy_write_block(drive[dnr].drive_floppy->ActiveFd,
				   drive[dnr].drive_floppy->ImageFormat,
				   sector_data, track, sector,
				   drive[dnr].drive_floppy->D64_Header,
                                   drive[dnr].drive_floppy->GCR_Header,
                                   drive[dnr].drive_floppy->unit);
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

    vdrive_bam_read_bam(drive[dnr].drive_floppy);

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
        log_error(drive_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  GCRIMAGE_SNAP_MAJOR, GCRIMAGE_SNAP_MINOR);
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
    m = NULL;

    for (i = 0; i < MAX_TRACKS_1571; i++)
        drive[dnr].GCR_track_size[i] = tmpbuf[i * 4] + (tmpbuf[i * 4 + 1] << 8)
            + (tmpbuf[i * 4 + 2] << 16) + (tmpbuf[i * 4 + 3] << 24);

    free(tmpbuf);
    drive[dnr].GCR_image_loaded = 1;
    drive[dnr].drive_floppy = NULL;
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
        log_error(drive_log,
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

/* -------------------------------------------------------------------- */

int reload_rom_1541(char *name) {
    char romsetnamebuffer[MAXPATHLEN];
    char *tmppath;

    if(dos_rom_name_1541) free(dos_rom_name_1541);
    if(name == NULL) {
        dos_rom_name_1541 = default_dos_rom_name_1541;
        drive_load_rom_images();
        return(1);
    }
    strcpy(romsetnamebuffer,"dos1541-");
    strncat(romsetnamebuffer,name,MAXPATHLEN - strlen(romsetnamebuffer) - 1);
    if ( sysfile_locate(romsetnamebuffer, &tmppath) ) {
      dos_rom_name_1541 = default_dos_rom_name_1541;
    } else {
      dos_rom_name_1541 = stralloc(romsetnamebuffer);
    }

    drive_load_rom_images();
    return(1);
}

void drive0_parallel_set_atn(int state)
{
    drive0_via_set_atn(state);
    drive0_riot_set_atn(state);
}

void drive1_parallel_set_atn(int state)
{
    drive1_via_set_atn(state);
    drive1_riot_set_atn(state);
}

int drive_num_leds(int dnr)
{
    if (drive[dnr].type == DRIVE_TYPE_1001) {
	return 2;
    }
    return 1;
}

