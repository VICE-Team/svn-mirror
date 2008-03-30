/*
 * reu.c - REU 1750 emulation.
 *
 * Written by
 *  Jouko Valta (jopi@stekt.oulu.fi)
 *  Richard Hable (K3027E7@edvz.uni-linz.ac.at)
 *  Ettore Perazzoli (ettore@comm2000.it) [EP]
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
#include <string.h>
#endif

#include "reu.h"

#include "log.h"
#include "maincpu.h"
#include "mem.h"
#include "utils.h"

/* #define REU_DEBUG */

#define REUSIZE 512

/*
 * Status and Command Registers
 * bit	7	6	5	4	3	2	1	0
 * 00	Int	EOB	Fault	RamSize	________ Version ________
 * 01	Exec	0	Load	Delayed	0	0	   Mode
 */


/* global */

static int     ReuSize = REUSIZE << 10;
static BYTE    reu[16];        /* REC registers */
static BYTE   *reuram = 0;
static char   *reu_file_name;

static log_t   reu_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

/* FIXME: This does not really handle different sizes.  */
int    reset_reu(int size)
{
    int i;

    if (reu_log == LOG_ERR)
        reu_log = log_open("REU");

    if (size > 0)
	ReuSize = size;

    for (i=0; i < 16; i++)
	reu[i] = 0;

    if (ReuSize >= 256)
	reu[0] = 0x50;
    else
	reu[0] = 0x40;

    reu[1] = 0x4A;

    if (reuram == NULL) {
	reuram = xmalloc(ReuSize);
	log_message(reu_log, "%dKB unit installed.", REUSIZE);
	if (load_file(reu_file_name, reuram, ReuSize) == 0) {
	    log_message(reu_log, "Image `%s' loaded successfully.",
                        reu_file_name);
	} else {
	    log_message(reu_log, "(No image loaded).");
	}
    }

    return 0;
}

void activate_reu(void)
{
    if (reuram == NULL)
	reset_reu(0);
}

void close_reu(void)
{
    if (reuram == NULL || reu_file_name == NULL)
	return;

    if (save_file(reu_file_name, reuram, ReuSize) == 0)
	log_message(reu_log, "image `%s' saved successfully.", reu_file_name);
    else
	log_error(reu_log, "cannot save image `%s'.", reu_file_name);
}


BYTE REGPARM1 read_reu(ADDRESS addr)
{
    BYTE retval;

    if (reuram == NULL)
	reset_reu(0);

    switch (addr) {
      case 0x0:
	/* fixed by [EP], 04-16-97. */
	retval = (reu[0] & 0x60) | (((ReuSize >> 10) >= 256) ? 0x10 : 0x00);

        /* Bits 7-5 are cleared when register is read, and pending IRQs are
           removed. */
	reu[0] &= ~0xe0;
        maincpu_set_irq(I_REU, 0);
	break;

      case 0x6:
	/* wrong address of bank register corrected - RH */
	retval = reu[6] | 0xf8;
	break;

      case 0x9:
	retval = reu[9] | 0x1f;
	break;

      case 0xa:
        retval = reu[0xa] | 0x3f;

      case 0xb:
      case 0xc:
      case 0xd:
      case 0xe:
      case 0xf:
	retval = 0xff;
	break;

      default:
	retval = reu[addr];
    }

#ifdef REU_DEBUG
    log_message(reu_log, "read [$%02X] => $%02X.", addr, retval);
#endif
    return retval;
}


void REGPARM2 store_reu(ADDRESS addr, BYTE byte)
{
    if (reuram == NULL)
	reset_reu(0);

    reu[addr] = byte;

#ifdef REU_DEBUG
    log_message(reu_log, "store [$%02X] <= $%02X.", addr, (int) byte);
#endif

    /* write REC command register
     * DMA only if execution bit (7) set  - RH */
    if ((addr == 0x1) && (byte & 0x80))
	reu_dma(byte & 0x10);
}

/* This function is called when write to REC command register or memory
 * location FF00 is detected.
 *
 * If host address exceeds ffff transfer contiues at 0000.
 * If reu address exceeds 7ffff transfer continues at 00000.
 * If address is fixed the same value is used during the whole transfer.
 */
/* Added correct handling of fixed addresses with transfer length 1  - RH */
/* Added fixed address support - [EP] */

void    reu_dma(int immed)
{
    static int delay = 0;
    unsigned int len;
    int reu_step, host_step;
    ADDRESS host_addr;
    int reu_addr;
    BYTE c;

    if (!immed) {
	delay++;
	return;
    }
    else {
	if (!delay && (immed < 0))
	    return;
	delay = 0;
    }

    /* wrong address of bank register & calculations corrected  - RH */
    host_addr = (ADDRESS)reu[2] | ((ADDRESS)reu[3] << 8);
    reu_addr  = ((int)reu[4] | ((int)reu[5] << 8)
                 | (((int)reu[6] & 7) << 16));
    if (( len = (int)(reu[7]) | ( (int)(reu[8]) << 8)) == 0)
	len = 0x10000;

    /* Fixed addresses implemented -- [EP] 04-16-97. */
    host_step = reu[0xA] & 0x80 ? 0 : 1;
    reu_step = reu[0xA] & 0x40 ? 0 : 1;

    /* clk += len; */

    switch (reu[1] & 0x03) {
      case 0: /* C64 -> REU */
#ifdef REU_DEBUG
	log_message(reu_log,
                    "copy ext $%05X %s<= main $%04X%s, $%04X (%d) bytes.",
                    reu_addr, reu_step ? "" : "(fixed) ", host_addr,
                    host_step ? "" : " (fixed)", len, len);
#endif
	for (; len--; host_addr = (host_addr + host_step) & 0xffff, reu_addr += reu_step) {
	    BYTE value = mem_read(host_addr);
	    reuram[reu_addr % ReuSize] = value;
	}
	break;

      case 1: /* REU -> C64 */
#ifdef REU_DEBUG
	log_message(reu_log,
                    "copy ext $%05X %s=> main $%04X%s, $%04X (%d) bytes.",
                    reu_addr, reu_step ? "" : "(fixed) ", host_addr,
                    host_step ? "" : " (fixed)", len, len);
#endif
	for (; len--; host_addr += host_step, reu_addr += reu_step )
	    mem_store((host_addr & 0xffff), reuram[reu_addr % ReuSize]);
	break;

      case 2: /* swap */
#ifdef REU_DEBUG
	log_message(reu_log,
                    "swap ext $%05X %s<=> main $%04X%s, $%04X (%d) bytes.",
                    reu_addr, reu_step ? "" : "(fixed) ", host_addr,
                    host_step ? "" : " (fixed)", len, len);
#endif
	for (; len--; host_addr += host_step, reu_addr += reu_step ) {
	    c = reuram[reu_addr % ReuSize];
	    reuram[reu_addr % ReuSize] = mem_read(host_addr & 0xffff);
	    mem_store((host_addr & 0xffff), c);
	}
	break;

      case 3: /* compare */
#ifdef REU_DEBUG
	log_message(reu_log,
                    "compare ext $%05X %s<=> main $%04X%s, $%04X (%d) bytes.",
                    reu_addr, reu_step ? "" : "(fixed) ", host_addr,
                    host_step ? "" : " (fixed)", len, len);
#endif
	while (len--) {
	    if (reuram[reu_addr % ReuSize] != mem_read(host_addr & 0xffff)) {
		reu[0] |= 0x20; /* FAULT */

                /* Bit 7: interrupt enable
                   Bit 5: interrupt on verify error */
                if (reu[9] & 0xa0) {
                    reu[0] |= 0x80;
                    maincpu_set_irq(I_REU, 1);
                }
		break;
	    }
	    host_addr += host_step; reu_addr += reu_step;
	}

        if ((reu[9] & 0xa0) == 0xa0)
            reu[0] |= 0x80;
	break;
    }

    if (!(reu[1] & 0x20)) {
	/* not autoload
         * incr. of addr. disabled, as already pointing to correct addr.
	 * address changes only if not fixed, correct reu base registers  -RH
	 */
#ifdef REU_DEBUG
	log_message(reu_log, "No autoload.");
#endif
        if ( !(reu[0xA] & 0x80)) {
	    reu[2] = host_addr & 0xff;
	    reu[3] = (host_addr >> 8) & 0xff;
	}
        if ( !(reu[0xA] & 0x40)) {
	    reu[4] = reu_addr & 0xff;
	    reu[5] = (reu_addr >> 8) & 0xff;
	    reu[6] = (reu_addr >> 16);
	}

	reu[7] = 1;
	reu[8] = 0;
    }

    /* [EP] 04-16-97. */
    reu[0] |= 0x40;
    reu[1] &= 0x7f;

    /* Bit 7: interrupt enable.  */
    /* Bit 6: interrupt on end of block */
    if ((reu[9] & 0xc0) == 0xc0) {
        reu[0] |= 0x80;
        maincpu_set_irq(I_REU, 1);
    }
}

/* ------------------------------------------------------------------------- */

static char snap_module_name[] = "REU1764";
#define SNAP_MAJOR 0
#define SNAP_MINOR 0

int reu_write_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
        return -1;

    if (snapshot_module_write_dword(m, (DWORD) (ReuSize >> 10)) < 0
        || snapshot_module_write_byte_array(m, reu, sizeof(reu)) < 0
        || snapshot_module_write_byte_array(m, reuram, ReuSize) < 0) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int reu_read_snapshot_module(snapshot_t *s)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;
    DWORD size;

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);
    if (m == NULL)
        return -1;

    if (major_version != SNAP_MAJOR) {
        log_error(reu_log, "Major version %d not valid; should be %d.",
                major_version, SNAP_MAJOR);
        goto fail;
    }

    /* Read RAM size.  */
    if (snapshot_module_read_dword(m, &size) < 0)
        goto fail;

    if (size > REUSIZE) {
        log_error(reu_log, "Size %d in snapshot not supported.", size);
        goto fail;
    }

    /* FIXME: We cannot really support sizes different from `REUSIZE'.  */

    reset_reu(ReuSize);

    if (snapshot_module_read_byte_array(m, reu, sizeof(reu)) < 0
        || snapshot_module_read_byte_array(m, reuram, ReuSize) < 0)
        goto fail;

    if (reu[0] & 0x80)
        set_irq_noclk(&maincpu_int_status, I_REU, 1);
    else
        set_irq_noclk(&maincpu_int_status, I_REU, 0);

    snapshot_module_close(m);
    return 0;

fail:
    snapshot_module_close(m);
    return -1;
}
