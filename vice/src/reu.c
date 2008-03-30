/*
 * reu.h - REU 1750 emulation.
 *
 * Written by
 *  Jouko Valta (jopi@stekt.oulu.fi)
 *  Richard Hable (K3027E7@edvz.uni-linz.ac.at)
 *
 * Fixes by
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

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "reu.h"
#include "maincpu.h"
#include "mem.h"
#include "utils.h"

/* #define REU_DEBUG */

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

/* ------------------------------------------------------------------------- */

int    reset_reu(int size)
{
    int i;

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
	printf("REU: %dKB unit installed.\n", REUSIZE);
	if (load_file(reu_file_name, reuram, ReuSize) == 0) {
	    printf ("REU: image `%s' loaded successfully.\n", reu_file_name);
	} else {
	    printf ("REU: (no image loaded).\n");
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
	printf("REU: image `%s' saved successfully.\n", reu_file_name);
    else
	fprintf(stderr,"REU: cannot save image `%s'.\n", reu_file_name);
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
	reu[0] &= ~0xe0;	/* Bits 7-5 are cleared when register is
				   read. */
	break;

      case 0x6:
	/* wrong address of bank register corrected - RH */
	retval = reu[6] | 0xf8;
	break;

      case 0x9:
	retval = reu[6] | 0x3f;
	break;

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
    printf("REU: read [$%02X] => $%02X\n", addr, retval);
#endif
    return retval;
}


void REGPARM2 store_reu(ADDRESS addr, BYTE byte)
{
    if (reuram == NULL)
	reset_reu(0);

    reu[addr] = byte;

#ifdef REU_DEBUG
    printf("REU: store [$%02X] <= $%02X\n", addr, (int) byte);
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
    host_addr = (ADDRESS)(reu[2]) | ( (ADDRESS)( reu[3] ) << 8 );
    reu_addr  = (int)(reu[4]) | ((int)(reu[5]) << 8) | ((int)(reu[6] & 7) << 16);
    if (( len = (int)(reu[7]) | ( (int)(reu[8]) << 8)) == 0)
	len = 0x10000;

    /* Fixed addresses implemented -- [EP] 04-16-97. */
    host_step = reu[0xA] & 0x80 ? 0 : 1;
    reu_step = reu[0xA] & 0x40 ? 0 : 1;

    /* clk += len; */

    switch (reu[1] & 0x03) {
      case 0: /* C64 -> REU */
#ifdef REU_DEBUG
	printf("REU: copy ext $%05X %s<= main $%04X%s, $%04X (%d) bytes.\n",
	       reu_addr, reu_step ? "" : "(fixed) ", host_addr,
	       host_step ? "" : " (fixed)", len, len);
#endif
	for (; len--; host_addr = (host_addr + host_step) & 0xffff, reu_addr += reu_step) {
	    BYTE value = LOAD(host_addr);
	    reuram[reu_addr % ReuSize] = value;
	}
	break;

      case 1: /* REU -> C64 */
#ifdef REU_DEBUG
	printf("REU: copy ext $%05X %s=> main $%04X%s, $%04X (%d) bytes.\n",
	       reu_addr, reu_step ? "" : "(fixed) ", host_addr,
	       host_step ? "" : " (fixed)", len, len);
#endif
	for (; len--; host_addr += host_step, reu_addr += reu_step )
	    STORE((host_addr & 0xffff), reuram[reu_addr % ReuSize]);
	break;

      case 2: /* swap */
        /* for-loop corrected - RH */
	/* clk += len; */	/* [EP] 04-16-97. */
#ifdef REU_DEBUG
	printf("REU: swap ext $%05X %s<=> main $%04X%s, $%04X (%d) bytes.\n",
	       reu_addr, reu_step ? "" : "(fixed) ", host_addr,
	       host_step ? "" : " (fixed)", len, len);
#endif
	for (; len--; host_addr += host_step, reu_addr += reu_step ) {
	    c = reuram[reu_addr % ReuSize];
	    reuram[reu_addr % ReuSize] = LOAD(host_addr & 0xffff);
	    STORE((host_addr & 0xffff), c);
	}
	break;

      case 3: /* compare */
#ifdef REU_DEBUG
	printf("REU: compare ext $%05X %s<=> main $%04X%s, $%04X (%d) bytes.\n",
	       reu_addr, reu_step ? "" : "(fixed) ", host_addr,
	       host_step ? "" : " (fixed)", len, len);
#endif
	while (len--) {
	    if (reuram[reu_addr % ReuSize] != LOAD(host_addr & 0xffff)) {
		reu[0] |= 0x20; /* FAULT */
		break;
	    }
	    host_addr += host_step; reu_addr += reu_step;
	}
	break;
    }

    if (!(reu[1] & 0x20)) {
	/* not autoload
         * incr. of addr. disabled, as already pointing to correct addr.
	 * address changes only if not fixed, correct reu base registers  -RH
	 */
#ifdef REU_DEBUG
	printf("No autoload\n");
#endif
        if ( !(reu[0xA] & 0x80)) {
	    reu[2] = host_addr & 0xff;
	    reu[3] = (host_addr >> 8) & 0xff;
	}
        if ( !(reu[0xA] & 0x40)) {
	    reu[4] = reu_addr & 0xff;
	    reu[5] = (reu_addr >> 8) & 0xff;
	    reu[6] = (reu_addr>>16);
	}

	reu[7] = 1;
	reu[8] = 0;
    }

    /* [EP] 04-16-97. */
    reu[0] |= 0x40;
    reu[1] &= 0x7f;
}

