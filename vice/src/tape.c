/*
 * tape.c - (Guess what?) Tape unit emulation.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * Based on older code by
 *  Jouko Valta (jopi@stekt.oulu.fi)
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

/* tapeunit.c -- Cassette drive interface.  The cassette interface consists
   of traps in tape access routines Find, Write Header, Send Data, and Get
   Data, so that actual operation can be controlled by C routines.  To
   support turboloaders, it would be necessary to emulate the tape encoding
   used. That is much slower though. :( */

/* FIXME: This should be splitted into a T64 and a tape device part.  */

#include "vice.h"

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef __riscos
#include "ROlib.h"
#else
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <errno.h>
#endif
#endif

#include "tape.h"

#include "datasette.h"
#include "log.h"
#include "mem.h"
#include "maincpu.h"
#include "serial.h"
#include "t64.h"
#include "tap.h"
#include "traps.h"
#include "types.h"
#include "utils.h"
#include "zfile.h"

/* #define DEBUG_TAPE */

/* Cassette Format Constants */
#define CAS_TYPE_OFFSET	0
#define CAS_STAD_OFFSET	1	/* start address */
#define CAS_ENAD_OFFSET	3	/* end address */
#define CAS_NAME_OFFSET	5	/* filename */

#define CAS_TYPE_PRG	1	/* Binary Program */
#define CAS_TYPE_BAS	3	/* Relocatable Program */
#define CAS_TYPE_DATA	4	/* Data Record */
#define CAS_TYPE_EOF	5	/* End of Tape marker */

/* CPU addresses for tape routine variables.  */
static int buffer_pointer_addr;
static int st_addr;
static int verify_flag_addr;
static int stal_addr;
static int eal_addr;
static int kbd_buf_addr;
static int kbd_buf_pending_addr;
static int irqval;
static int irqtmp;

/* Flag: has tape been initialized?  */
static int tape_is_initialized = 0;

/* Tape traps to be installed.  */
static const trap_t *tape_traps;

/* T64 tape currently attached.  */
static t64_t *attached_t64_tape = NULL;

/* TAP tape currently attached.  */
static tap_t *attached_tap_tape = NULL;

/* Logging goes here.  */
static log_t tape_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

static inline void set_st(BYTE b)
{
    mem_store(st_addr, (mem_read(st_addr) | b));
}

/* ------------------------------------------------------------------------- */

static void tape_traps_install(void)
{
    const trap_t *p;

    if (tape_traps != NULL) {
    for (p = tape_traps; p->func != NULL; p++)
        traps_add(p);
    }
}

static void tape_traps_deinstall(void)
{
    const trap_t *p;

    if (tape_traps != NULL) {
    for (p = tape_traps; p->func != NULL; p++)
        traps_remove(p);
    }
}

/* Initialize the tape emulation, using the traps in `trap_list'.  */
/* FIXME: This should be passed through a struct.  */
int tape_init(int _buffer_pointer_addr,
              int _st_addr,
              int _verify_flag_addr,
              int _irqtmp,
              int _irqval,
              int _stal_addr,
              int _eal_addr,
              int _kbd_buf_addr,
              int _kbd_buf_pending_addr,
              const trap_t *trap_list)
{
    if (tape_log == LOG_ERR)
        tape_log = log_open("Tape");

    /* Set addresses of tape routine variables.  */
    st_addr = _st_addr;
    buffer_pointer_addr = _buffer_pointer_addr;
    verify_flag_addr = _verify_flag_addr;
    irqtmp = _irqtmp;
    irqval = _irqval;
    stal_addr = _stal_addr;
    eal_addr = _eal_addr;

    kbd_buf_addr = _kbd_buf_addr;
    kbd_buf_pending_addr = _kbd_buf_pending_addr;

    tape_traps = trap_list;
    tape_traps_install();

    if (tape_is_initialized)
        return 0;
    tape_is_initialized = 1;

    return 0;
}

int tape_deinstall(void)
{
    if (!tape_is_initialized)
	return -1;   

    if (attached_t64_tape != NULL) {
	tape_detach_image();
    }

    tape_traps_deinstall();

    tape_traps = NULL;

    tape_is_initialized = 0;

    return 0;
}

/* ------------------------------------------------------------------------- */

/* Functions to attach and detach tape image files.  */

/* Detach.  */
int tape_detach_image(void)
{
    int retval;

    if (attached_t64_tape != NULL) {
        log_message(tape_log,
                    "Detaching T64 image `%s'.", attached_t64_tape->file_name);
        
        /* Gone.  */
        retval = t64_close(attached_t64_tape);
        attached_t64_tape = NULL;

        /* Tape detached: release play button.  */
        datasette_set_tape_sense(0);

        return retval;
    }

    if (attached_tap_tape != NULL) {
        log_message(tape_log,
                    "Detaching TAP image `%s'.", attached_tap_tape->file_name);

        /* Gone.  */
        retval = tap_close(attached_tap_tape);
        attached_tap_tape = NULL;
        datasette_set_tape_image(NULL);

        tape_traps_install();

        return retval;
    }

    return 0;
}

/* Attach.  */
int tape_attach_image(const char *name)
{
    t64_t *new_t64_tape;
    tap_t *new_tap_tape;

    if (!name || !*name)
	return -1;

    new_t64_tape = t64_open(name);
    if (new_t64_tape != NULL)
    {
        tape_detach_image();
        attached_t64_tape = new_t64_tape;

        log_message(tape_log, "T64 image '%s' attached.", name);

        /* Tape attached: press play button.  */
        datasette_set_tape_sense(1);

        return 0;
    }

    new_tap_tape = tap_open(name);
    if (new_tap_tape != NULL)
    {
        tape_detach_image();
        attached_tap_tape = new_tap_tape;

        datasette_set_tape_image(new_tap_tape);

        log_message(tape_log, "TAP image '%s' attached.", name);

        tape_traps_deinstall();

        return 0;
    }

    return -1;
}

/* ------------------------------------------------------------------------- */

/* Tape traps.  These functions implement the standard kernal replacements
   for the tape functions.  Every emulator can either use these traps, or
   install its own ones, by passing an appropriate `trap_list' to
   `tape_init()'.  */

/* Find the next Tape Header and load it onto the Tape Buffer.  */
void tape_find_header_trap(void)
{
    int err;
    BYTE *cassette_buffer;

    cassette_buffer = ram + (mem_read(buffer_pointer_addr)
                             | (mem_read(buffer_pointer_addr + 1) << 8));

    if (attached_t64_tape == NULL) {
        err = 1;
    } else {
        t64_file_record_t *rec = NULL;

        err = 0;
        do {
            if (t64_seek_to_next_file(attached_t64_tape, 1) < 0) {
                err = 1;
                break;
            }

            rec = t64_get_current_file_record(attached_t64_tape);
        } while (rec->entry_type != T64_FILE_RECORD_NORMAL);

        if (!err) {
            cassette_buffer[CAS_TYPE_OFFSET] = CAS_TYPE_PRG;
            cassette_buffer[CAS_STAD_OFFSET] = rec->start_addr & 0xff;
            cassette_buffer[CAS_STAD_OFFSET + 1] = rec->start_addr >> 8;
            cassette_buffer[CAS_ENAD_OFFSET] = rec->end_addr & 0xff;
            cassette_buffer[CAS_ENAD_OFFSET + 1] = rec->end_addr >> 8;
            memcpy(cassette_buffer + CAS_NAME_OFFSET,
                   rec->cbm_name, T64_REC_CBMNAME_LEN);
        }
    }

    if (err)
	cassette_buffer[CAS_TYPE_OFFSET] = CAS_TYPE_EOF;

    mem_store(st_addr, 0);	/* Clear the STATUS word.  */
    mem_store(verify_flag_addr, 0);

    if (irqtmp) {
	mem_store(irqtmp, irqval & 0xff);
	mem_store(irqtmp + 1, (irqval >> 8) & 0xff);
    }

    /* Check if STOP has been pressed.  */
    {
	int i, n = mem_read(kbd_buf_pending_addr);

        MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
	for (i = 0; i < n; i++) {
	    if (mem_read(kbd_buf_addr + i) == 0x3) {
                MOS6510_REGS_SET_CARRY(&maincpu_regs, 1);
		break;
	    }
        }
    }

    MOS6510_REGS_SET_ZERO(&maincpu_regs, 1);
}

/* Cassette Data transfer trap.

   XR flags the function to be performed on IRQ:

   08   Write tape
   0a   Write tape leader
   0c   Normal keyscan
   0e   Read tape

   Luckily enough, these values are valid for all the machines.  */
void tape_receive_trap(void)
{
    WORD start, end, len;
    BYTE st;

    start = (mem_read(stal_addr) | (mem_read(stal_addr + 1) << 8));
    end = (mem_read(eal_addr) | (mem_read(eal_addr + 1) << 8));

    switch (MOS6510_REGS_GET_X(&maincpu_regs)) {
      case 0x0e:
        {
            /* Read block.  */
            len = end - start;

            if (t64_read(attached_t64_tape,
                         ram + (int) start, (int) len) == (int) len) {
                st = 0x40;	/* EOF */
            } else {
                st = 0x10;

                log_warning(tape_log,
                            "Unexpected end of tape: file may be truncated.");
            }
        }
        break;
      default:
        log_error(tape_log, "Kernal command %x not supported.",
                  MOS6510_REGS_GET_X(&maincpu_regs));
        st = 0x40;
        break;
    }

    /* Set registers and flags like the Kernal routine does.  */

    if (irqtmp) {
	mem_store(irqtmp, irqval & 0xff);
	mem_store(irqtmp + 1, (irqval >> 8) & 0xff);
    }

    set_st(st);			/* EOF and possible errors */

    MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
}

char *tape_get_file_name(void)
{
    if (attached_t64_tape)
        return attached_t64_tape->file_name;
    if (attached_tap_tape)
        return attached_tap_tape->file_name;
    return NULL;
}

