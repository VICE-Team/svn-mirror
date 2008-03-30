/*
 * autostart.c - Automatic image loading and starting.
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <viceteam@t-online.de>
 *  Thomas Bretz <tbretz@ph.tum.de>
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
#include <string.h>

#include "archdep.h"
#include "autostart.h"
#include "attach.h"
#include "charset.h"
#include "datasette.h"
#include "drive.h"
#include "fsdevice.h"
#include "imagecontents.h"
#include "interrupt.h"
#include "kbdbuf.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "p00.h"
#include "resources.h"
#include "serial.h"
#include "snapshot.h"
#include "tape.h"
#include "types.h"
#include "ui.h"
#include "utils.h"
#include "vdrive.h"
#include "vdrive-bam.h"
#include "zfile.h"

/* Kernal addresses.  Set by `autostart_init()'.  */

static ADDRESS blnsw;           /* Cursor Blink enable: 0 = Flash Cursor */
static int pnt;                 /* Pointer: Current Screen Line Address */
static int pntr;                /* Cursor Column on Current Line */
static int lnmx;                /* Physical Screen Line Length */

/* Current state of the autostart routine.  */
static enum {
    AUTOSTART_NONE,
    AUTOSTART_ERROR,
    AUTOSTART_HASTAPE,
    AUTOSTART_PRESSPLAYONTAPE,
    AUTOSTART_LOADINGTAPE,
    AUTOSTART_HASDISK,
    AUTOSTART_LOADINGDISK,
    AUTOSTART_HASSNAPSHOT,
    AUTOSTART_DONE
} autostartmode = AUTOSTART_NONE;

#define AUTOSTART_WAIT_BLINK   0
#define AUTOSTART_NOWAIT_BLINK 1

/* Log descriptor.  */
static log_t autostart_log = LOG_ERR;

/* Flag: was true drive emulation turned on when we started booting the disk
   image?  */
static int orig_drive_true_emulation_state = 0;

/* PETSCII name of the program to load. NULL if default */
static BYTE *autostart_program_name = NULL;

/* Minimum number of cycles before we feed BASIC with commands.  */
static CLOCK min_cycles;

/* Flag: Do we want to switch true drive emulation on/off during autostart?  */
static int handle_drive_true_emulation;

/* Flag: autostart is initialized.  */
static int autostart_enabled = 0;

/* Flag: Autostart the file or just load it?  */
static unsigned int autostart_run_mode;

/* Flag: maincpu_clk isn't resetted yet */
static int autostart_wait_for_reset;
/* ------------------------------------------------------------------------- */

/* Deallocate program name if we have one */
static void deallocate_program_name(void)
{
    if (autostart_program_name) {
        free(autostart_program_name);
        autostart_program_name = NULL;
    }
}

static enum { YES, NO, NOT_YET } check(const char *s, unsigned int blink_mode)
{
    int screen_addr, line_length, cursor_column, addr, i;

    screen_addr = (int)(mem_read((ADDRESS)(pnt))
                  | (mem_read((ADDRESS)(pnt + 1)) << 8));
    cursor_column = (int)mem_read((ADDRESS)(pntr));

    line_length = (int)(lnmx < 0 ? -lnmx : mem_read((ADDRESS)(lnmx)) + 1);

    if (!kbd_buf_is_empty())
        return NOT_YET;

    if (blink_mode == AUTOSTART_WAIT_BLINK && cursor_column != 0)
        return NOT_YET;

    if (blink_mode == AUTOSTART_WAIT_BLINK && blnsw != 0
        && mem_read(blnsw) != 0)
        return NOT_YET;

    if (blink_mode == AUTOSTART_WAIT_BLINK)
        addr = screen_addr - line_length;
    else
        addr = screen_addr;

    for (i = 0; s[i] != '\0'; i++) {

        if (mem_read((ADDRESS)(addr + i)) != s[i] % 64) {
            if (mem_read((ADDRESS)(addr + i)) != (BYTE)32)
                return NO;
            return NOT_YET;
        }
    }

    return YES;
}

static void set_true_drive_emulation_mode(int on)
{
    resources_set_value("DriveTrueEmulation", (resource_value_t)on);
    ui_update_menus();
}

static int get_true_drive_emulation_state(void)
{
    int value;

    if (resources_get_value("DriveTrueEmulation",
                            (resource_value_t *)&value) < 0)
        return 0;

    return value;
}

/* ------------------------------------------------------------------------- */

static void load_snapshot_trap(ADDRESS unused_addr, void *unused_data)
{
    if (autostart_program_name
        && machine_read_snapshot((char *)autostart_program_name, 0) < 0)
        ui_error("Cannot load snapshot file.");
    ui_update_menus();
}

/* ------------------------------------------------------------------------- */

/* Initialize autostart.  */
int autostart_init(CLOCK _min_cycles, int _handle_drive_true_emulation,
                   int _blnsw, int _pnt, int _pntr, int _lnmx)
{
    blnsw = (ADDRESS)(_blnsw);
    pnt = _pnt;
    pntr = _pntr;
    lnmx = _lnmx;

    if (autostart_log == LOG_ERR) {
        autostart_log = log_open("AUTOSTART");
        if (autostart_log == LOG_ERR)
            return -1;
    }

    min_cycles = _min_cycles;
    handle_drive_true_emulation = _handle_drive_true_emulation;

    if (_min_cycles)
        autostart_enabled = 1;
    else
        autostart_enabled = 0;

    return 0;
}

void autostart_disable(void)
{
    if (!autostart_enabled)
        return;

    autostartmode = AUTOSTART_ERROR;
    deallocate_program_name();
    log_error(autostart_log, "Turned off.");
}

/* ------------------------------------------------------------------------- */

/* This function is called by the `serialreceivebyte()' trap as soon as EOF
   is reached.  */
static void disk_eof_callback(void)
{
    if (handle_drive_true_emulation) {
        BYTE id[2], *buffer;
        unsigned int track, sector;

        if (orig_drive_true_emulation_state) {
            log_message(autostart_log, "Turning true drive emulation on.");
            vdrive_bam_get_disk_id(8, id);
            vdrive_get_last_read(&track, &sector, &buffer);
        }
        set_true_drive_emulation_mode(orig_drive_true_emulation_state);
        if (orig_drive_true_emulation_state) {
            drive_set_disk_memory(0, id, track, sector);
            drive_set_last_read(0, track, sector, buffer);
        }
    }

    if (autostartmode != AUTOSTART_NONE) {
        if (autostart_run_mode == AUTOSTART_MODE_RUN)
            log_message(autostart_log, "Starting program.");
        else
            log_message(autostart_log, "Program loaded.");
    }

    autostartmode = AUTOSTART_DONE;

    serial_set_eof_callback(NULL);
}

/* This function is called by the `serialattention()' trap before
   returning.  */
static void disk_attention_callback(void)
{
    if (autostart_run_mode == AUTOSTART_MODE_RUN)
        kbd_buf_feed("RUN\r");

    serial_set_attention_callback(NULL);

    /* Next step is waiting for end of loading, to turn true drive emulation
       on.  */
    serial_set_eof_callback(disk_eof_callback);
}

/* ------------------------------------------------------------------------- */

static void advance_hastape(void)
{
    char *tmp;

    switch (check("READY.", AUTOSTART_WAIT_BLINK)) {
      case YES:
        log_message(autostart_log, "Loading file.");
        if (autostart_program_name) {
            tmp = concat("LOAD\"", autostart_program_name, "\"\r", NULL);
            kbd_buf_feed(tmp);
            free(tmp);
        } else {
            kbd_buf_feed("LOAD\r");
        }
        if (tape_tap_attched()) {
            autostartmode = AUTOSTART_PRESSPLAYONTAPE;
        } else {
            autostartmode = AUTOSTART_LOADINGTAPE;
        }
        deallocate_program_name();
        break;
      case NO:
        autostart_disable();
        break;
      case NOT_YET:
        break;
    }
}

static void advance_pressplayontape(void)
{
    switch (check("PRESS PLAY ON TAPE", AUTOSTART_NOWAIT_BLINK)) {
      case YES:
        autostartmode = AUTOSTART_LOADINGTAPE;
        datasette_control(DATASETTE_CONTROL_START);
        break;
      case NO:
        autostart_disable();
        break;
      case NOT_YET:
        break;
    }
}

static void advance_loadingtape(void)
{
    switch (check("READY.", AUTOSTART_WAIT_BLINK)) {
      case YES:
        log_message(autostart_log, "Starting program.");
        kbd_buf_feed("RUN\r");
        autostartmode = AUTOSTART_DONE;
        break;
      case NO:
        autostart_disable();
        break;
      case NOT_YET:
        break;
    }
}

static void advance_hasdisk(void)
{
    char *tmp;
    int traps;

    switch (check("READY.", AUTOSTART_WAIT_BLINK)) {
      case YES:
        if (autostart_program_name)
            log_message(autostart_log, "Loading program '%s'",
                        autostart_program_name);
        else
            log_message(autostart_log, "Loading program '*'");
        orig_drive_true_emulation_state = get_true_drive_emulation_state();
        if (handle_drive_true_emulation) {
            resources_get_value("VirtualDevices",
                                (resource_value_t *)&traps);
            if (traps) {
                if (orig_drive_true_emulation_state)
                    log_message(autostart_log,
                                "Switching true drive emulation off.");
                set_true_drive_emulation_mode(0);
            } else {
                if (!orig_drive_true_emulation_state)
                    log_message(autostart_log,
                                "Switching true drive emulation on.");
                set_true_drive_emulation_mode(1);
            }
        } else {
            traps = 1;
        }
        if (autostart_program_name)
            tmp = xmsprintf("LOAD\"%s\",8,1\r", autostart_program_name);
        else
            tmp = stralloc("LOAD\"*\",8,1\r");
        kbd_buf_feed(tmp);
        free(tmp);

        if (!traps) {
            if (autostart_run_mode == AUTOSTART_MODE_RUN)
                kbd_buf_feed("RUN\r");
            autostartmode = AUTOSTART_DONE;
        } else {
            autostartmode = AUTOSTART_LOADINGDISK;
            serial_set_attention_callback(disk_attention_callback);
        }
        deallocate_program_name();
        break;
      case NO:
        orig_drive_true_emulation_state = get_true_drive_emulation_state();
        autostart_disable();
        break;
      case NOT_YET:
        break;
    }
}

static void advance_hassnapshot(void)
{
    switch (check("READY.", AUTOSTART_WAIT_BLINK)) {
      case YES:
        log_message(autostart_log, "Restoring snapshot.");
        interrupt_maincpu_trigger_trap(load_snapshot_trap, (void*)0);
        autostartmode = AUTOSTART_DONE;
        break;
      case NO:
        autostart_disable();
        break;
      case NOT_YET:
        break;
    }
}

/* Execute the actions for the current `autostartmode', advancing to the next
   mode if necessary.  */
void autostart_advance(void)
{
    if (!autostart_enabled)
        return;

    if (maincpu_clk < min_cycles)
    {
        autostart_wait_for_reset = 0;
        return;
    }

    if (autostart_wait_for_reset)
        return;


    switch (autostartmode) {
      case AUTOSTART_HASTAPE:
        advance_hastape();
        break;
      case AUTOSTART_PRESSPLAYONTAPE:
        advance_pressplayontape();
        break;
      case AUTOSTART_LOADINGTAPE:
        advance_loadingtape();
        break;
      case AUTOSTART_HASDISK:
        advance_hasdisk();
        break;
      case AUTOSTART_HASSNAPSHOT:
        advance_hassnapshot();
        break;
      default:
        return;
    }

    if (autostartmode == AUTOSTART_ERROR && handle_drive_true_emulation) {
        log_message(autostart_log, "Now turning true drive emulation %s.",
                    orig_drive_true_emulation_state ? "on" : "off");
        set_true_drive_emulation_mode(orig_drive_true_emulation_state);
    }
}

int autostart_ignore_reset = 0;

/* Clean memory and reboot for autostart.  */
static void reboot_for_autostart(const char *program_name, unsigned int mode,
                                 unsigned int runmode)
{
    if (!autostart_enabled)
        return;

    log_message(autostart_log, "Resetting the machine to autostart '%s'",
                program_name ? program_name : "*");
    mem_powerup();
    autostart_ignore_reset = 1;
    deallocate_program_name();
    if (program_name)
        autostart_program_name = (BYTE *)stralloc(program_name);
    maincpu_trigger_reset();
    /* The autostartmode must be set AFTER the shutdown to make the autostart
       threadsafe for OS/2 */
    autostartmode = mode;
    autostart_run_mode = runmode;
    autostart_wait_for_reset = 1;
}

/* ------------------------------------------------------------------------- */

/* Autostart snapshot file `file_name'.  */
int autostart_snapshot(const char *file_name, const char *program_name)
{
    BYTE vmajor, vminor;
    snapshot_t *snap;

    if (file_name == NULL || !autostart_enabled)
        return -1;

    deallocate_program_name();  /* not needed at all */

    if (!(snap = snapshot_open(file_name, &vmajor, &vminor, machine_name)) ) {
        autostartmode = AUTOSTART_ERROR;
        return -1;
    }

    log_message(autostart_log, "Loading snapshot file `%s'.", file_name);
    snapshot_close(snap);

    /*autostart_program_name = (BYTE *)stralloc(file_name);
    interrupt_maincpu_trigger_trap(load_snapshot_trap, (void*)0);*/
    /* use for snapshot */
    reboot_for_autostart(file_name, AUTOSTART_HASSNAPSHOT, AUTOSTART_MODE_RUN);

    return 0;
}

/* Autostart tape image `file_name'.  */
int autostart_tape(const char *file_name, const char *program_name,
                   unsigned int program_number, unsigned int runmode)
{
    char *name = NULL;

    if (!file_name || !autostart_enabled)
        return -1;

    /* Get program name first to avoid more than one file handle open on
       image.  */
    if (!program_name && program_number > 0)
        name = image_contents_filename_by_number(IMAGE_CONTENTS_TAPE,
                                                 file_name, 0, program_number);
    else
        name = stralloc(program_name ? program_name : "");

    if (!(tape_image_attach(1, file_name) < 0)) {
        log_message(autostart_log,
                    "Attached file `%s' as a tape image.", file_name);
        if (tape_tap_attched()) {
            if (program_number > 0) {
                free(name);
                name = NULL;
                tape_seek_to_file(tape_image_dev1, program_number);
            } else {
                tape_seek_start(tape_image_dev1);
            }
        }
        reboot_for_autostart(name, AUTOSTART_HASTAPE, runmode);
        free(name);

        return 0;
    }

    autostartmode = AUTOSTART_ERROR;
    deallocate_program_name();

    if (name)
        free(name);

    return -1;
}

/* Cope with 0xa0 padded file names.  */
static void autostart_disk_cook_name(char **name)
{
    unsigned int pos;

    pos = 0;

    while((*name)[pos] != '\0') {
        if (((unsigned char)((*name)[pos])) == 0xa0) {
            char *ptr;

            ptr = xmalloc(pos + 1);
            memcpy(ptr, *name, pos);
            ptr[pos] = '\0';
            free(*name);
            *name = ptr;
            break;
        }
        pos++;
    }
}

/* Autostart disk image `file_name'.  */
int autostart_disk(const char *file_name, const char *program_name,
                   unsigned int program_number, unsigned int runmode)
{
    char *name = NULL;

    if (!file_name || !autostart_enabled)
        return -1;

    /* Get program name first to avoid more than one file handle open on
       image.  */
    if (!program_name && program_number > 0)
        name = image_contents_filename_by_number(IMAGE_CONTENTS_DISK,
                                                 file_name, 0, program_number);
    else
        name = stralloc(program_name ? program_name : "*");

    if (name) {
        autostart_disk_cook_name(&name);
        if (!(file_system_attach_disk(8, file_name) < 0)) {
            log_message(autostart_log,
                        "Attached file `%s' as a disk image.", file_name);
            reboot_for_autostart(name, AUTOSTART_HASDISK, runmode);
            free(name);

            return 0;
        }
    }

    autostartmode = AUTOSTART_ERROR;
    deallocate_program_name();
    if (name)
        free(name);

    return -1;
}

/* Autostart PRG file `file_name'.  The PRG file can either be a raw CBM file
   or a P00 file, and the FS-based drive emulation is set up so that its
   directory becomes the current one on unit #8.  */
int autostart_prg(const char *file_name, unsigned int runmode)
{
    FILE *f;
    char *cbm_name;
    char p00_header_file_name[20]; /* FIXME */
    int p00_type;
    char *directory;
    char *file;

    f = fopen(file_name, MODE_READ);
    if (f == NULL) {
        log_error(autostart_log, "Cannot open `%s'.", file_name);
        return -1;
    }

    p00_type = p00_check_name(file_name);
    if (p00_type >= 0) {
        if (p00_type == FT_PRG &&
            p00_read_header(f, (BYTE *)p00_header_file_name, NULL)) {
            p00_type = -1;
        } else {
            if (p00_type != FT_PRG) {
                fclose(f);
                return -1;
            }
        }
    }

    /* Extract the directory path to allow FS-based drive emulation to
       work.  */
    util_fname_split(file_name, &directory, &file);

    if (archdep_path_is_relative(directory)) {
        char *tmp;
        archdep_expand_path(&tmp, directory);
        free(directory);
        directory = tmp;

        /* FIXME: We should actually eat `.'s and `..'s from `directory'
           instead.  */
    }

    /* Prepare the CBM file name.  */
    if (p00_type != FT_PRG) {
        /* Then it must be a raw file.  */
        cbm_name = stralloc(file);
        charset_petconvstring(cbm_name, 0);
    } else {
        cbm_name = stralloc(p00_header_file_name);
    }

    /* Setup FS-based drive emulation.  */
    fsdevice_set_directory(directory ? directory : ".", 8);
    set_true_drive_emulation_mode(0);
    resources_set_value("VirtualDevices", (resource_value_t)1);
    resources_set_value("FSDevice8ConvertP00", (resource_value_t)1);
    file_system_detach_disk(8);
    ui_update_menus();

    /* Now it's the same as autostarting a disk image.  */
    reboot_for_autostart(cbm_name, AUTOSTART_HASDISK, runmode);

    free(directory);
    free(file);
    free(cbm_name);
    fclose(f);

    log_message(autostart_log, "Preparing to load PRG file `%s'.",
                file_name);

    return 0;
}

/* ------------------------------------------------------------------------- */

/* Autostart `file_name', trying to auto-detect its type.  */
int autostart_autodetect(const char *file_name, const char *program_name,
                         unsigned int program_number, unsigned int runmode)
{
    if (file_name == NULL)
        return -1;

    if (!autostart_enabled) {
        log_error(autostart_log,
                  "Autostart is not available on this setup.");
        return -1;
    }

    log_message(autostart_log, "Autodetecting image type of `%s'.", file_name);

    if (autostart_disk(file_name, program_name, program_number, runmode) == 0) {
        log_message(autostart_log, "`%s' recognized as disk image.", file_name);
        return 0;
    }
    if (autostart_tape(file_name, program_name, program_number, runmode) == 0) {
        log_message(autostart_log, "`%s' recognized as tape image.", file_name);
        return 0;
    }
    if (autostart_snapshot(file_name, program_name) == 0) {
        log_message(autostart_log, "`%s' recognized as snapshot image.",
                    file_name);
        return 0;
    }
    if (autostart_prg(file_name, runmode) == 0) {
        log_message(autostart_log, "`%s' recognized as program/p00 file.",
                    file_name);
        return 0;
    }

    log_error(autostart_log, "`%s' is not a valid file.", file_name);
    return -1;
}

/* Autostart the image attached to device `num'.  */
int autostart_device(int num)
{
    if (!autostart_enabled)
        return -1;

    switch (num) {
      case 8:
        reboot_for_autostart(NULL, AUTOSTART_HASDISK, AUTOSTART_MODE_RUN);
        return 0;
      case 1:
        reboot_for_autostart(NULL, AUTOSTART_HASTAPE, AUTOSTART_MODE_RUN);
        return 0;
    }
    return -1;
}

/* Disable autostart on reset.  */
void autostart_reset(void)
{
    int oldmode;

    if (!autostart_enabled)
        return;

    if (!autostart_ignore_reset
        && autostartmode != AUTOSTART_NONE
        && autostartmode != AUTOSTART_ERROR) {
        oldmode = autostartmode;
        autostartmode = AUTOSTART_NONE;
        if (oldmode != AUTOSTART_DONE)
            disk_eof_callback();
        autostartmode = AUTOSTART_NONE;
        deallocate_program_name();
        log_message(autostart_log, "Turned off.");
    }
    autostart_ignore_reset = 0;
}

