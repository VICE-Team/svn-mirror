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

#include <stdio.h>
#include <stdlib.h>

#include "autostart.h"

#include "attach.h"
#include "interrupt.h"
#include "kbdbuf.h"
#include "mem.h"
#include "resources.h"
#include "serial.h"
#include "tape.h"
#include "ui.h"
#include "utils.h"
#include "vdrive.h"
#include "vmachine.h"
#include "warn.h"


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
    AUTOSTART_DONE
} autostartmode = AUTOSTART_NONE;

/* Warnings.  */
static warn_t *pwarn = NULL;

/* Flag: was true 1541 emulation turned on when we started booting the disk
   image?  */
static int orig_true1541_state = 0;

/* Program name to load. NULL if default */
static char *autostart_program_name = NULL;

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

/* Initialize autostart.  */
int autostart_init(CLOCK _min_cycles, int _handle_true1541,
		   int _blnsw, int _pnt, int _pntr, int _lnmx)
{
    blnsw = _blnsw;
    pnt = _pnt;
    pntr = _pntr;
    lnmx = _lnmx;

    if (!pwarn) {
	pwarn = warn_init("AUTOSTART", 32);
	if (!pwarn)
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
    warn(pwarn, -1, "disabling autostart");
}

/* ------------------------------------------------------------------------- */

/* This function is called by the `serialreceivebyte()' trap as soon as EOF
   is reached.  */
static void disk_eof_callback(void)
{
    if (handle_true1541) {
        if (orig_true1541_state)
            warn(pwarn, -1, "switching true 1541 on");
        set_true1541_mode(orig_true1541_state);
    }

    warn(pwarn, -1, "starting program");

    autostartmode = AUTOSTART_DONE;

    serial_set_eof_callback(NULL);
}

/* This function is called by the `serialattention()' trap before
   returning.  */
static void disk_attention_callback(void)
{
    kbd_buf_feed("run\r");

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
            warn(pwarn, -1, "loading tape");
            if (autostart_program_name) {
                tmp = concat("load\"", autostart_program_name,
                             "\"\r", NULL);
                kbd_buf_feed(tmp);
                free(tmp);
            } else
                kbd_buf_feed("load\r");
            autostartmode = AUTOSTART_LOADINGTAPE;
            deallocate_program_name();
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
            warn(pwarn, -1, "starting program");
            kbd_buf_feed("run\r");
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

                warn(pwarn, -1, "loading disk");
                orig_true1541_state = get_true1541_state();
                if (handle_true1541) {
                    resources_get_value("NoTraps",
					(resource_value_t *) & no_traps);
                    if (!no_traps) {
                        if (orig_true1541_state)
                            warn(pwarn, -1,
                                 "switching true 1541 emulation off");
                        set_true1541_mode(0);
                    } else {
                        if (!orig_true1541_state)
                            warn(pwarn, -1,
                                 "switching true 1541 emulation on");
                        set_true1541_mode(1);
                    }
                } else
                    no_traps = 0;

                if (autostart_program_name) {
                    tmp = alloca(strlen(autostart_program_name) + 20);
                    sprintf(tmp, "load\"%s\",8,1\r",
                            autostart_program_name);
                    kbd_buf_feed(tmp);
                } else
                    kbd_buf_feed("load\"*\",8,1\r");

                if (no_traps) {
                    kbd_buf_feed("run\r");
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
      default:
        return;
    }

    if (autostartmode == AUTOSTART_ERROR && handle_true1541) {
	warn(pwarn, -1, "now turning true 1541 emulation %s",
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

    warn(pwarn, -1, "rebooting...");
    mem_powerup();
    autostart_ignore_reset = 1;
    maincpu_trigger_reset();
    deallocate_program_name();
    if (program_name)
	autostart_program_name = stralloc(program_name);
}

/* ------------------------------------------------------------------------- */

/* Autostart tape image `file_name'.  */
int autostart_tape(const char *file_name, const char *program_name)
{
    if (file_name == NULL || !autostart_enabled)
	return -1;

    if (serial_select_file(DT_TAPE, 1, file_name) < 0) {
	warn(pwarn, -1, "cannot attach file '%s' (as a tape)", file_name);
	autostartmode = AUTOSTART_ERROR;
	deallocate_program_name();
	return -1;
    }
    warn(pwarn, -1, "attached file `%s' as a tape image", file_name);
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
	warn(pwarn, -1, "cannot attach file `%s' as a disk image", file_name);
	file_system_detach_disk(8);
	autostartmode = AUTOSTART_ERROR;
	deallocate_program_name();
	return -1;
    }
    warn(pwarn, -1, "attached file `%s' as a disk image to device #8",
	 file_name);
    autostartmode = AUTOSTART_HASDISK;
    reboot_for_autostart(program_name);

    return 0;
}

/* Autostart `file_name', trying to auto-detect its type.  */
int autostart_autodetect(const char *file_name, const char *program_name)
{
    if (file_name == NULL)
	return -1;

    if (!autostart_enabled) {
	fprintf(stderr, "Couldn't autostart - unknown kernal!");
	return -1;
    }
    if (autostart_disk(file_name, program_name) == 0)
	warn(pwarn, -1, "`%s' detected as a disk image", file_name);
    else if (autostart_tape(file_name, program_name) == 0)
	warn(pwarn, -1, "`%s' detected as a tape image", file_name);
    else {
	warn(pwarn, -1, "type of file `%s' unrecognized", file_name);
	return -1;
    }

    return 0;
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
	warn(pwarn, -1, "disabling autostart");
	autostartmode = AUTOSTART_NONE;
	deallocate_program_name();
    }
    autostart_ignore_reset = 0;
}
