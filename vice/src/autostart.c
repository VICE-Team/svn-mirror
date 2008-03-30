/*
 * autostart.c - automatic image loading and starting
 *
 * Written by
 *  Teemu Rantanen      (tvr@cs.hut.fi)
 *  Ettore Perazzoli    (ettore@comm2000.it)
 *
 * Patches by
 *  André Fachat        (a.fachat@physik.tu-chemnitz.de)
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

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#endif

#ifdef OS2
#include <stdlib.h>
#endif

#include "autostart.h"

#include "archdep.h"
#include "attach.h"
#include "charsets.h"
#include "datasette.h"
#include "fsdevice.h"
#include "interrupt.h"
#include "kbdbuf.h"
#include "machine.h"
#include "mem.h"
#include "p00.h"
#include "resources.h"
#include "serial.h"
#include "snapshot.h"
#include "tape.h"
#include "ui.h"
#include "utils.h"
#include "vdrive.h"
#include "zfile.h"

/* Kernal addresses.  Set by `autostart_init()'.  */

static int blnsw;		/* Cursor Blink enable: 0 = Flash Cursor */
static int pnt;			/* Pointer: Current Screen Line Address */
static int pntr;		/* Cursor Column on Current Line */
static int lnmx;		/* Physical Screen Line Length */

/* Current state of the autostart routine.  */
static enum {
    AUTOSTART_NONE,
    AUTOSTART_ERROR,
    AUTOSTART_HASTAPE,
    AUTOSTART_LOADINGTAPE,
    AUTOSTART_HASDISK,
    AUTOSTART_LOADINGDISK,
    AUTOSTART_HASSNAPSHOT,
    AUTOSTART_DONE
} autostartmode = AUTOSTART_NONE;

/* Log descriptor.  */
static log_t autostart_log = LOG_ERR;

/* Flag: was true 1541 emulation turned on when we started booting the disk
   image?  */
static int orig_true1541_state = 0;

/* PETSCII name of the program to load. NULL if default */
static BYTE *autostart_program_name = NULL;

/* Minimum number of cycles before we feed BASIC with commands.  */
static CLOCK min_cycles;

/* Flag: Do we want to switch true 1541 emulation on/off during autostart?  */
static int handle_true1541;

/* Flag: autostart is initialized.  */
static int autostart_enabled = 0;

/* ------------------------------------------------------------------------- */

/* Deallocate program name if we have one */
static void deallocate_program_name(void)
{
    if (autostart_program_name) {
	free(autostart_program_name);
	autostart_program_name = NULL;
    }
}

static enum { YES, NO, NOT_YET } check(const char *s)
{
    int screen_addr = mem_read(pnt) | (mem_read(pnt + 1) << 8);
    int line_length = lnmx < 0 ? -lnmx : mem_read(lnmx) + 1;
    int cursor_column = mem_read(pntr);
    int addr, i;

    if (!kbd_buf_is_empty() || cursor_column != 0 || mem_read(blnsw) != 0)
	return NOT_YET;

    addr = screen_addr - line_length;
    for (i = 0; s[i] != '\0'; i++) {
	if (mem_read((ADDRESS) (addr + i)) != s[i] % 64) {
	    if (mem_read((ADDRESS) (addr + i)) != (BYTE) 32)
		return NO;
	    return NOT_YET;
	}
    }

    return YES;
}

static void set_true1541_mode(int on)
{
    resources_set_value("DriveTrueEmulation", (resource_value_t) on);
    ui_update_menus();
}

static int get_true1541_state(void)
{
    int value;

    if (resources_get_value("DriveTrueEmulation",
                            (resource_value_t *) & value) < 0)
	return 0;

    return value;
}

/* ------------------------------------------------------------------------- */

static void load_snapshot_trap(ADDRESS unused_addr, void *unused_data)
{
    if (autostart_program_name
        && machine_read_snapshot((char *)autostart_program_name) < 0)
        ui_error("Cannot load snapshot file.");
    ui_update_menus();
}

/* ------------------------------------------------------------------------- */

/* Initialize autostart.  */
int autostart_init(CLOCK _min_cycles, int _handle_true1541,
                   int _blnsw, int _pnt, int _pntr, int _lnmx)
{
    blnsw = _blnsw;
    pnt = _pnt;
    pntr = _pntr;
    lnmx = _lnmx;

    if (autostart_log == LOG_ERR) {
        autostart_log = log_open("AUTOSTART");
        if (autostart_log == LOG_ERR)
            return -1;
    }

    min_cycles = _min_cycles;
    handle_true1541 = _handle_true1541;

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
    if (handle_true1541) {
        if (orig_true1541_state)
            log_message(autostart_log, "Turning true drive emulation on.");
        set_true1541_mode(orig_true1541_state);
    }

    log_message(autostart_log, "Starting program.");

    autostartmode = AUTOSTART_DONE;

    serial_set_eof_callback(NULL);
}

/* This function is called by the `serialattention()' trap before
   returning.  */
static void disk_attention_callback(void)
{
    kbd_buf_feed("RUN\r");

    serial_set_attention_callback(NULL);

    /* Next step is waiting for end of loading, to turn true 1541 emulation
       on.  */
    serial_set_eof_callback(disk_eof_callback);
}


/* Execute the actions for the current `autostartmode', advancing to the next
   mode if necessary.  */
void autostart_advance(void)
{
    char *tmp;

    if (clk < min_cycles || !autostart_enabled)
	return;

    switch (autostartmode) {
      case AUTOSTART_HASTAPE:
        switch (check("READY.")) {
          case YES:
            log_message(autostart_log, "Loading file.");
            if (autostart_program_name) {
                tmp = concat("LOAD\"", autostart_program_name,
                             "\"\r", NULL);
                kbd_buf_feed(tmp);
                free(tmp);
            } else
                kbd_buf_feed("LOAD\r");
            autostartmode = AUTOSTART_LOADINGTAPE;
            deallocate_program_name();
            datasette_control(DATASETTE_CONTROL_START);
            break;
          case NO:
            autostart_disable();
            break;
          case NOT_YET:
            break;
        }
        break;
      case AUTOSTART_LOADINGTAPE:
        switch (check("READY.")) {
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
        break;
      case AUTOSTART_HASDISK:
        switch (check("READY.")) {
          case YES:
            {
                int no_traps;

                log_message(autostart_log, "Loading program.");
                orig_true1541_state = get_true1541_state();
                if (handle_true1541) {
                    resources_get_value("NoTraps",
					(resource_value_t *) &no_traps);
                    if (!no_traps) {
                        if (orig_true1541_state)
                            log_message(autostart_log,
                                        "Switching true drive emulation off.");
                        set_true1541_mode(0);
                    } else {
                        if (!orig_true1541_state)
                            log_message(autostart_log,
                                        "Switching true drive emulation on.");
                        set_true1541_mode(1);
                    }
                } else
                    no_traps = 0;

                if (autostart_program_name) {
                    tmp = malloc(strlen((char *)(autostart_program_name)) + 20);
                    sprintf(tmp, "LOAD\"%s\",8,1\r",
                            autostart_program_name);
                    kbd_buf_feed(tmp);
                    free(tmp);
                } else
                    kbd_buf_feed("LOAD\"*\",8,1\r");

                if (no_traps) {
                    kbd_buf_feed("RUN\r");
                    autostartmode = AUTOSTART_DONE;
                } else {
                    autostartmode = AUTOSTART_LOADINGDISK;
                    deallocate_program_name();
                    serial_set_attention_callback(disk_attention_callback);
                }
                break;
            }
          case NO:
            autostart_disable();
            break;
          case NOT_YET:
            break;
        }
        break;
      case AUTOSTART_HASSNAPSHOT:
        log_message(autostart_log, "Restoring snapshot.");
	maincpu_trigger_trap(load_snapshot_trap,(void*)0);
        autostartmode = AUTOSTART_DONE;
	break;
      default:
        return;
    }

    if (autostartmode == AUTOSTART_ERROR && handle_true1541) {
        log_message(autostart_log, "Now turning true drive emulation %s.",
                    orig_true1541_state ? "on" : "off");
	set_true1541_mode(orig_true1541_state);
    }
}

static int autostart_ignore_reset = 0;

/* Clean memory and reboot for autostart.  */
static void reboot_for_autostart(const char *program_name)
{
    if (!autostart_enabled)
	return;

    log_message(autostart_log, "Resetting the machine...");

    mem_powerup();
    autostart_ignore_reset = 1;
    maincpu_trigger_reset();
    deallocate_program_name();
    if (program_name)
	autostart_program_name = (BYTE *)stralloc(program_name);
}

/* ------------------------------------------------------------------------- */

/* Autostart snapshot file `file_name'.  */
int autostart_snapshot(const char *file_name, const char *program_name)
{
    BYTE vmajor, vminor;
    snapshot_t *snap;

    if (file_name == NULL || !autostart_enabled)
	return -1;

    deallocate_program_name();	/* not needed at all */

    if (!(snap = snapshot_open(file_name, &vmajor, &vminor, machine_name)) ) {
	autostartmode = AUTOSTART_ERROR;
	return -1;
    }

    log_message(autostart_log, "Loading snapshot file `%s'.", file_name);
    snapshot_close(snap);
    autostartmode = AUTOSTART_HASSNAPSHOT;
    reboot_for_autostart(file_name);	/* use for snapshot */

    return 0;
}

/* Autostart tape image `file_name'.  */
int autostart_tape(const char *file_name, const char *program_name)
{
    if (file_name == NULL || !autostart_enabled)
	return -1;

    if (tape_attach_image(file_name) < 0) {
	autostartmode = AUTOSTART_ERROR;
	deallocate_program_name();
	return -1;
    }

    log_message(autostart_log, "Attached file `%s' as a tape image.",
                file_name);
    autostartmode = AUTOSTART_HASTAPE;
    reboot_for_autostart(program_name);

    return 0;
}

/* Autostart disk image `file_name'.  */
int autostart_disk(const char *file_name, const char *program_name)
{
    if (file_name == NULL || !autostart_enabled)
	return -1;

    if (file_system_attach_disk(8, file_name) < 0) {
	file_system_detach_disk(8);
	autostartmode = AUTOSTART_ERROR;
	deallocate_program_name();
	return -1;
    }

    log_message(autostart_log, "Attached file `%s' as a disk image to device #8.",
                file_name);

    autostartmode = AUTOSTART_HASDISK;
    reboot_for_autostart(program_name);

    return 0;
}

/* Autostart PRG file `file_name'.  The PRG file can either be a raw CBM file
   or a P00 file, and the FS-based drive emulation is set up so that its
   directory becomes the current one on unit #8.  */
int autostart_prg(const char *file_name)
{
    FILE *f;
    char *cbm_name;
    char p00_header_file_name[20]; /* FIXME */
    int p00_type;
    char *directory;
    char *file;

    f = fopen(file_name, "r");
    if (f == NULL) {
        log_error(autostart_log, "Cannot open `%s': %s",
                  file_name, strerror(errno));
        return -1;
    }

    /* First check if it's a P00 file.  */

    p00_type = p00_check_name(file_name);
    if (p00_type >= 0) {
        if (p00_type == FT_PRG
            && p00_read_header(f, p00_header_file_name, NULL) != 0)
            p00_type = -1;
        else if (p00_type != FT_PRG) {
            fclose(f);
            return -1;
        }
    }

    /* Extract the directory path to allow FS-based drive emulation to
       work.  */
    fname_split(file_name, &directory, &file);

    if (directory) {
        char *tmpdir;
        tmpdir = concat(directory, FSDEV_DIR_SEP_STR, NULL);
        free(directory);
        directory = tmpdir;
    }

    if (archdep_path_is_relative(directory)) {
        char *tmp, *cwd;

        cwd = get_current_dir();
        tmp = concat(cwd, "/", directory, NULL);
        free(directory);
        directory = tmp;

        /* FIXME: We should actually eat `.'s and `..'s from `directory'
           instead.  */
    }

    /* Prepare the CBM file name.  */
    if (p00_type != FT_PRG) {
        /* Then it must be a raw file.  */
        cbm_name = stralloc(file);
        petconvstring(cbm_name, 0);
    } else {
        cbm_name = stralloc(p00_header_file_name);
    }

    /* Setup FS-based drive emulation.  */
    fsdevice_set_directory(directory ? directory : ".", 8);
    set_true1541_mode(0);
    resources_set_value("NoTraps", (resource_value_t) 0);
    resources_set_value("FSDevice8ConvertP00", (resource_value_t) 1);
    ui_update_menus();

    /* Now it's the same as autostarting a disk image.  */
    autostartmode = AUTOSTART_HASDISK;
    reboot_for_autostart(cbm_name);

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
int autostart_autodetect(const char *file_name, const char *program_name)
{
    if (file_name == NULL)
        return -1;

    if (!autostart_enabled) {
        log_error(autostart_log,
                  "Autostart is not available on this setup.");
        return -1;
    }

    log_message(autostart_log, "Autodetecting image type of `%s'.", file_name);

    if (autostart_disk(file_name, program_name) == 0) {
        log_message(autostart_log, "`%s' recognized as disk image.", file_name);
        return 0;
    }
    if (autostart_tape(file_name, program_name) == 0) {
        log_message(autostart_log, "`%s' recognized as tape image.", file_name);
        return 0;
    }
    if (autostart_snapshot(file_name, program_name) == 0) {
        log_message(autostart_log, "`%s' recognized as snapshot image.",
                    file_name);
        return 0;
    }
    if (autostart_prg(file_name) == 0) {
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
        autostartmode = AUTOSTART_HASDISK;
        break;
      case 1:
        autostartmode = AUTOSTART_HASTAPE;
        break;
      default:
        return -1;
    }

    reboot_for_autostart(NULL);
    return 0;
}

/* Disable autostart on reset.  */
void autostart_reset(void)
{
    if (!autostart_enabled)
	return;

    if (!autostart_ignore_reset
        && autostartmode != AUTOSTART_NONE
        && autostartmode != AUTOSTART_ERROR) {
	autostartmode = AUTOSTART_NONE;
	deallocate_program_name();
	log_message(autostart_log, "Turned off.");
    }
    autostart_ignore_reset = 0;
}
