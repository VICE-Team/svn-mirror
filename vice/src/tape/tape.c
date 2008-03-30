/*
 * tape.c - Tape unit emulation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *
 * Based on older code by
 *  Jouko Valta <jopi@stekt.oulu.fi>
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

#include "log.h"
#include "maincpu.h"
#include "mem.h"
#include "mos6510.h"
#include "serial.h"
#include "t64.h"
#include "tap.h"
#include "tape.h"
#include "tapeimage.h"
#include "traps.h"
#include "types.h"
#include "utils.h"

/* #define DEBUG_TAPE */

/* Cassette Format Constants */
#define CAS_TYPE_OFFSET 0
#define CAS_STAD_OFFSET 1       /* start address */
#define CAS_ENAD_OFFSET 3       /* end address */
#define CAS_NAME_OFFSET 5       /* filename */

#define CAS_TYPE_PRG    1       /* Binary Program */
#define CAS_TYPE_BAS    3       /* Relocatable Program */
#define CAS_TYPE_DATA   4       /* Data Record */
#define CAS_TYPE_EOF    5       /* End of Tape marker */

/* CPU addresses for tape routine variables.  */
static ADDRESS buffer_pointer_addr;
static ADDRESS st_addr;
static ADDRESS verify_flag_addr;
static ADDRESS stal_addr;
static ADDRESS eal_addr;
static ADDRESS kbd_buf_addr;
static ADDRESS kbd_buf_pending_addr;
static int irqval;
static ADDRESS irqtmp;

/* Flag: has tape been initialized?  */
static int tape_is_initialized = 0;

/* Tape traps to be installed.  */
static const trap_t *tape_traps;

/* Logging goes here.  */
static log_t tape_log = LOG_ERR;

/* The tape image for device 1. */
tape_image_t *tape_image_dev1 = NULL;

/* ------------------------------------------------------------------------- */

static inline void set_st(BYTE b)
{
    mem_store(st_addr, (BYTE)(mem_read(st_addr) | b));
}

/* ------------------------------------------------------------------------- */

void tape_traps_install(void)
{
    const trap_t *p;

    if (tape_traps != NULL) {
        for (p = tape_traps; p->func != NULL; p++)
            traps_add(p);
    }
}

void tape_traps_deinstall(void)
{
    const trap_t *p;

    if (tape_traps != NULL) {
        for (p = tape_traps; p->func != NULL; p++)
            traps_remove(p);
    }
}

/* Initialize the tape emulation, using the traps in `trap_list'.  */
/* FIXME: This should be passed through a struct.  */
int tape_init(tape_init_t *init)
{
    tape_log = log_open("Tape");

    tape_image_init();

    tape_image_dev1 = (tape_image_t *)xcalloc(1, sizeof(tape_image_t));

    tap_init(init);

    /* Set addresses of tape routine variables.  */
    st_addr = init->st_addr;
    buffer_pointer_addr = init->buffer_pointer_addr;
    verify_flag_addr = init->verify_flag_addr;
    irqtmp = init->irqtmp;
    irqval = init->irqval;
    stal_addr = init->stal_addr;
    eal_addr = init->eal_addr;

    kbd_buf_addr = init->kbd_buf_addr;
    kbd_buf_pending_addr = init->kbd_buf_pending_addr;

    tape_traps = init->trap_list;
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

    if (tape_image_dev1->name != NULL &&
        tape_image_dev1->type == TAPE_TYPE_T64)
        tape_image_detach(1);

    tape_traps_deinstall();

    tape_traps = NULL;

    tape_is_initialized = 0;

    return 0;
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

    cassette_buffer = mem_ram + (mem_read(buffer_pointer_addr)
                      | (mem_read((ADDRESS)(buffer_pointer_addr + 1)) << 8));

    if (tape_image_dev1->name == NULL
        || tape_image_dev1->type != TAPE_TYPE_T64) {
        err = 1;
    } else {
        t64_t *t64;
        t64_file_record_t *rec;

        t64 = (t64_t *)tape_image_dev1->data;
        rec = NULL;

        err = 0;
        do {
            if (t64_seek_to_next_file(t64, 1) < 0) {
                err = 1;
                break;
            }

            rec = t64_get_current_file_record(t64);
        } while (rec->entry_type != T64_FILE_RECORD_NORMAL);

        if (!err) {
            cassette_buffer[CAS_TYPE_OFFSET] = CAS_TYPE_BAS;
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

    mem_store(st_addr, 0);      /* Clear the STATUS word.  */
    mem_store(verify_flag_addr, 0);

    if (irqtmp) {
        mem_store(irqtmp, (BYTE)(irqval & 0xff));
        mem_store((ADDRESS)(irqtmp + 1), (BYTE)((irqval >> 8) & 0xff));
    }

    /* Check if STOP has been pressed.  */
    {
        int i, n = mem_read(kbd_buf_pending_addr);

        MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
        for (i = 0; i < n; i++) {
            if (mem_read((ADDRESS)(kbd_buf_addr + i)) == 0x3) {
                MOS6510_REGS_SET_CARRY(&maincpu_regs, 1);
                break;
            }
        }
    }

    MOS6510_REGS_SET_ZERO(&maincpu_regs, 1);
}

void tape_find_header_trap_plus4(void)
{
    int err;
    BYTE *cassette_buffer;

    cassette_buffer = mem_ram + buffer_pointer_addr;

    if (tape_image_dev1->name == NULL
        || tape_image_dev1->type != TAPE_TYPE_T64) {
        err = 1;
    } else {
        t64_t *t64;
        t64_file_record_t *rec;

        t64 = (t64_t *)tape_image_dev1->data;
        rec = NULL;

        err = 0;
        do {
            if (t64_seek_to_next_file(t64, 1) < 0) {
                err = 1;
                break;
            }

            rec = t64_get_current_file_record(t64);
        } while (rec->entry_type != T64_FILE_RECORD_NORMAL);

        if (!err) {
            mem_store(0xF8, CAS_TYPE_BAS);
            cassette_buffer[CAS_STAD_OFFSET - 1] = rec->start_addr & 0xff;
            cassette_buffer[CAS_STAD_OFFSET] = rec->start_addr >> 8;
            cassette_buffer[CAS_ENAD_OFFSET - 1] = rec->end_addr & 0xff;
            cassette_buffer[CAS_ENAD_OFFSET] = rec->end_addr >> 8;
            memcpy(cassette_buffer + CAS_NAME_OFFSET - 1,
                   rec->cbm_name, T64_REC_CBMNAME_LEN);
        }
    }

    if (err)
        mem_store(0xF8, CAS_TYPE_EOF);

    mem_store(0xb6,0x33);
    mem_store(0xb7,0x03);

    mem_store(st_addr, 0);      /* Clear the STATUS word.  */
    mem_store(verify_flag_addr, 0);

    /* Check if STOP has been pressed.  */
    {
        int i, n = mem_read(kbd_buf_pending_addr);

        MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
        for (i = 0; i < n; i++) {
            if (mem_read((ADDRESS)(kbd_buf_addr + i)) == 0x3) {
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

    start = (mem_read(stal_addr) | (mem_read((ADDRESS)(stal_addr + 1)) << 8));
    end = (mem_read(eal_addr) | (mem_read((ADDRESS)(eal_addr + 1)) << 8));

    switch (MOS6510_REGS_GET_X(&maincpu_regs)) {
      case 0x0e:
        {
            /* Read block.  */
            len = end - start;

            if (t64_read((t64_t *)tape_image_dev1->data,
                         mem_ram + (int)start, (int)len) == (int)len) {
                st = 0x40;      /* EOF */
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
        mem_store(irqtmp, (BYTE)(irqval & 0xff));
        mem_store((ADDRESS)(irqtmp + 1), (BYTE)((irqval >> 8) & 0xff));
    }

    set_st(st);                 /* EOF and possible errors */

    MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
}

void tape_receive_trap_plus4(void)
{
    WORD start, end, len;
    BYTE st;

    start = (mem_read(stal_addr) | (mem_read((ADDRESS)(stal_addr + 1)) << 8));
    end = (mem_read(eal_addr) | (mem_read((ADDRESS)(eal_addr + 1)) << 8));

    /* Read block.  */
    len = end - start;

    if (t64_read((t64_t *)tape_image_dev1->data,
                 mem_ram + (int) start, (int)len) == (int) len) {
        st = 0x40;      /* EOF */
    } else {
        st = 0x10;

        log_warning(tape_log,
                    "Unexpected end of tape: file may be truncated.");
    }

    /* Set registers and flags like the Kernal routine does.  */


    set_st(st);                 /* EOF and possible errors */
}

const char *tape_get_file_name(void)
{
    return tape_image_dev1->name;
}

int tape_tap_attched(void)
{
    if (tape_image_dev1->name != NULL
        && tape_image_dev1->type == TAPE_TYPE_TAP)
        return 1;

    return 0;
}

/* ------------------------------------------------------------------------- */

void tape_get_header(tape_image_t *tape_image, BYTE *name)
{
    switch (tape_image->type) {
      case TAPE_TYPE_T64:
        t64_get_header((t64_t *)tape_image->data, name);
        break;
      case TAPE_TYPE_TAP:
        tap_get_header((tap_t *)tape_image->data, name);
        break;
    }
}

tape_file_record_t *tape_get_current_file_record(tape_image_t *tape_image)
{
    static tape_file_record_t rec;

    memset(rec.name, 0, 17);

    switch (tape_image->type) {
      case TAPE_TYPE_T64:
        {
            t64_file_record_t *t64_rec;

            t64_rec = t64_get_current_file_record((t64_t *)tape_image->data);
            memcpy(rec.name, t64_rec->cbm_name, 16);
            rec.type = (t64_rec->entry_type == T64_FILE_RECORD_FREE) ? 0 : 1;
            rec.start_addr = t64_rec->start_addr;
            rec.end_addr = t64_rec->end_addr;
            break;
        }
      case TAPE_TYPE_TAP:
        {
            tape_file_record_t *tape_rec;

            tape_rec = tap_get_current_file_record((tap_t *)tape_image->data);
            memcpy(rec.name, tape_rec->name, 16);
            rec.type = 1;
            rec.start_addr = tape_rec->start_addr;
            rec.end_addr = tape_rec->end_addr;
            break;
        }
    }
    return &rec;
}

int tape_seek_start(tape_image_t *tape_image)
{
    switch (tape_image->type) {
      case TAPE_TYPE_T64:
        return t64_seek_start((t64_t *)tape_image->data);
      case TAPE_TYPE_TAP:
        return tap_seek_start((tap_t *)tape_image->data);
    }
    return -1;
}

int tape_seek_to_file(tape_image_t *tape_image, unsigned int file_number)
{
    switch (tape_image->type) {
      case TAPE_TYPE_T64:
        return t64_seek_to_file((t64_t *)tape_image->data, file_number);
      case TAPE_TYPE_TAP:
        return tap_seek_to_file((tap_t *)tape_image->data, file_number);
    }
    return -1;
}

int tape_seek_to_next_file(tape_image_t *tape_image, unsigned int allow_rewind)
{
    switch (tape_image->type) {
      case TAPE_TYPE_T64:
        return t64_seek_to_next_file((t64_t *)tape_image->data, allow_rewind);
      case TAPE_TYPE_TAP:
        return tap_seek_to_next_file((tap_t *)tape_image->data, allow_rewind);
    }
    return -1;
}

/* ------------------------------------------------------------------------- */

int tape_internal_close_tape_image(tape_image_t *tape_image)
{
    if (tape_image_close(tape_image) < 0)
        return -1;

    free(tape_image);

    return 0;
}

tape_image_t *tape_internal_open_tape_image(const char *name,
                                            unsigned int read_only)
{
    tape_image_t *image;

    image = (tape_image_t *)xmalloc(sizeof(tape_image_t));
    image->name = stralloc(name);
    image->read_only = read_only;

    if (tape_image_open(image) < 0) {
        free(image->name);
        free(image);
        log_error(tape_log, "Cannot open file `%s'", name);
        return NULL;
    }

    return image;
}

