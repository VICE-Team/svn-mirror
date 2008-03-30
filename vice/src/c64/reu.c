/*
 * reu.c - REU emulation.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 * 
 * Based on old code by
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Richard Hable <K3027E7@edvz.uni-linz.ac.at>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "c64cart.h"
#include "cartridge.h"
#include "cmdline.h"
#include "interrupt.h"
#include "log.h"
#include "maincpu.h"
#include "mem.h"
#include "resources.h"
#include "reu.h"
#include "snapshot.h"
#include "types.h"
#include "utils.h"

/* #define REU_DEBUG */

/*
 * Status and Command Registers
 * bit  7       6       5       4       3       2       1       0
 * 00   Int     EOB     Fault   RamSize ________ Version ________
 * 01   Exec    0       Load    Delayed 0       0          Mode
 */

/* REU registers */
static BYTE reu[16];

/* REU image.  */
static BYTE *reu_ram = NULL;

static log_t reu_log = LOG_ERR;

static int reu_activate(void);
static int reu_deactivate(void);

/* ------------------------------------------------------------------------- */

/* Flag: Do we enable the external REU?  */
int reu_enabled;

/* Size of the REU.  */
DWORD reu_size = 0;

/* Size of the REU in KB.  */
DWORD reu_size_kb = 0;

/* Filename of the REU image.  */
char *reu_filename = NULL;

/*
 * Some cartridges can coexist with the REU.
 * This list might not be complete, but atleast these are known
 * to work. Feel free to add more coexisting cartridges to this list.
 */
static int reu_coexist_cartridge(void)
{
    int result;

    switch (mem_cartridge_type) {
      case CARTRIDGE_NONE:
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
      case CARTRIDGE_EXPERT:
      case CARTRIDGE_RETRO_REPLAY:
        result = 1;
        break;
      default:
        result = 0;
    }
    return result;
}

static int set_reu_enabled(resource_value_t v, void *param)
{
    if (!(int)v) {
        if (reu_enabled) {
            if (reu_deactivate() < 0) {
                return -1;
            }
        }
        reu_enabled = 0;
        return 0;
    } else { 
        if (reu_coexist_cartridge() == 1) {
            if (!reu_enabled) {
                if (reu_activate() < 0) {
                    return -1;
                }
            }
            reu_enabled = 1;
            return 0;
        } else {
            /* The REU and the IEEE488 interface share the same address
               space, so they cannot be enabled at the same time.  */
            return -1;
        }
    }
}

static int set_reu_size(resource_value_t v, void *param)
{
    if ((DWORD)v == reu_size_kb)
        return 0;

    switch ((DWORD)v) {
      case 128:
      case 256:
      case 512:
      case 1024:
      case 2048:
      case 4096:
      case 8192:
      case 16384:
        break;
      default:
        log_message(reu_log, "Unknown REU size %ld.", (long)v);
        return -1;
    }

    if (reu_enabled) {
        reu_deactivate();
        reu_size_kb = (DWORD)v;
        reu_size = reu_size_kb << 10;
        reu_activate();
    } else {
        reu_size_kb = (DWORD)v;
        reu_size = reu_size_kb << 10;
    }

    return 0;
}

static int set_reu_filename(resource_value_t v, void *param)
{
    const char *name = (const char *)v;

    if (reu_filename != NULL && name != NULL
        && strcmp(name, reu_filename) == 0)
        return 0;

    if (reu_enabled) {
        reu_deactivate();
        util_string_set(&reu_filename, name);
        reu_activate();
    } else {
        util_string_set(&reu_filename, name);
    }

    return 0;
}

static resource_t resources[] = {
    { "REU", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&reu_enabled, set_reu_enabled, NULL },
    { "REUsize", RES_INTEGER, (resource_value_t)512,
      (resource_value_t *)&reu_size_kb, set_reu_size, NULL },
    { "REUfilename", RES_STRING, (resource_value_t)"",
      (resource_value_t *)&reu_filename, set_reu_filename, NULL },
    { NULL }
};

int reu_resources_init(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static cmdline_option_t cmdline_options[] =
{
    { "-reu", SET_RESOURCE, 0, NULL, NULL, "REU", (resource_value_t)1,
      NULL, "Enable the RAM expansion unit" },
    { "+reu", SET_RESOURCE, 0, NULL, NULL, "REU", (resource_value_t)0,
      NULL, "Disable the RAM expansion unit" },
    { "-reuimage", SET_RESOURCE, 1, NULL, NULL, "REUfilename", NULL,
      "<name>", "Specify name of REU image" },
    { "-reusize", SET_RESOURCE, 1, NULL, NULL, "REUsize", NULL,
      "<size in KB>", "Size of the RAM expansion unit" },
    { NULL }
};

int reu_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

void reu_init(void)
{
    reu_log = log_open("REU");
}

void reu_reset(void)
{
    int i;

    for (i = 0; i < 16; i++)
        reu[i] = 0;

    if (reu_size >= (256 << 10))
        reu[0] = 0x50;
    else
        reu[0] = 0x40;

    reu[1] = 0x4A;
}

static int reu_activate(void)
{
    if (!reu_size)
        return 0;

    reu_ram = (BYTE *)xrealloc((void *)reu_ram, (size_t)reu_size);
    log_message(reu_log, "%dKB unit installed.", reu_size >> 10);

    if (!util_check_null_string(reu_filename)) {
        if (util_file_load(reu_filename, reu_ram, (size_t)reu_size,
                           UTIL_FILE_LOAD_RAW) < 0) {
            log_message(reu_log,
                        "Reading REU image %s failed.", reu_filename);
            if (util_file_save(reu_filename, reu_ram, reu_size) < 0) {
                log_message(reu_log,
                            "Creating REU image %s failed.", reu_filename);
                return -1;
            }
            log_message(reu_log, "Creating REU image %s.", reu_filename);
            return 0;
        }
        log_message(reu_log, "Reading REU image %s.", reu_filename);
    }

    reu_reset();
    return 0;
}

static int reu_deactivate(void)
{
    if (reu_ram == NULL)
        return 0;

    if (!util_check_null_string(reu_filename)) {
        if (util_file_save(reu_filename, reu_ram, reu_size) < 0) {
            log_message(reu_log,
                        "Writing REU image %s failed.", reu_filename);
            return -1;
        }
        log_message(reu_log, "Writing REU image %s.", reu_filename);
    }

    free(reu_ram);
    reu_ram = NULL;

    return 0;
}

void reu_shutdown(void)
{
    reu_deactivate();
}

BYTE REGPARM1 reu_read(ADDRESS addr)
{
    BYTE retval;

    switch (addr) {
      case 0x0:
        retval = reu[0];

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
    log_message(reu_log, "read [$%02X] => $%02X.", addr, retval);
#endif
    return retval;
}


void REGPARM2 reu_store(ADDRESS addr, BYTE byte)
{
    if (addr != 0) /* REC status register is Read Only */
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

void reu_dma(int immed)
{
    static int delay = 0;
    int len;
    int reu_step, host_step;
    ADDRESS host_addr;
    unsigned int reu_addr, reu6_mask;
    BYTE c;

    if (!reu_enabled)
        return;

    if (!immed) {
        delay = 1;
        return;
    } else {
        if (!delay && (immed < 0))
            return;
        delay = 0;
    }

    reu6_mask = (reu_size >> 16) - 1;

    /* wrong address of bank register & calculations corrected  - RH */
    host_addr = (ADDRESS)reu[2] | ((ADDRESS)reu[3] << 8);
    reu_addr  = ((unsigned int)reu[4] | ((unsigned int)reu[5] << 8)
                 | (((unsigned int)reu[6] & reu6_mask) << 16));

    len = (int)(reu[7]) | ((int)(reu[8]) << 8);

    if (len == 0)
        len = 0x10000;

    /* Fixed addresses implemented -- [EP] 04-16-97. */
    host_step = (reu[0xA] & 0x80) ? 0 : 1;
    reu_step = (reu[0xA] & 0x40) ? 0 : 1;

    switch (reu[1] & 0x03) {
      case 0: /* C64 -> REU */
#ifdef REU_DEBUG
        log_message(reu_log,
                    "copy ext $%05X %s<= main $%04X%s, $%04X (%d) bytes.",
                    reu_addr, reu_step ? "" : "(fixed) ", host_addr,
                    host_step ? "" : " (fixed)", len, len);
#endif
        for (; len--; host_addr = (host_addr + host_step) & 0xffff,
            reu_addr += reu_step) {
            BYTE value;
            value = mem_read(host_addr);
#ifdef REU_DEBUG
            log_message(reu_log,
                        "Transferring byte: %x from main $%04X to ext $%05X.",
                        value, host_addr, reu_addr);
#endif
            reu_ram[reu_addr % reu_size] = value;
        }
        len = 0x1;
        reu[0] |= 0x40;
        break;

      case 1: /* REU -> C64 */
#ifdef REU_DEBUG
        log_message(reu_log,
                    "copy ext $%05X %s=> main $%04X%s, $%04X (%d) bytes.",
                    reu_addr, reu_step ? "" : "(fixed) ", host_addr,
                    host_step ? "" : " (fixed)", len, len);
#endif
        for (; len--; host_addr += host_step, reu_addr += reu_step ) {
#ifdef REU_DEBUG
            log_message(reu_log,
                        "Transferring byte: %x from ext $%05X to main $%04X.",
                        reu_ram[reu_addr % reu_size], reu_addr, host_addr);
#endif
            mem_store((ADDRESS)(host_addr & 0xffff),
                      reu_ram[reu_addr % reu_size]);
        }
        len = 1;
        reu[0] |= 0x40;
        break;

      case 2: /* swap */
#ifdef REU_DEBUG
        log_message(reu_log,
                    "swap ext $%05X %s<=> main $%04X%s, $%04X (%d) bytes.",
                    reu_addr, reu_step ? "" : "(fixed) ", host_addr,
                    host_step ? "" : " (fixed)", len, len);
#endif
        for (; len--; host_addr += host_step, reu_addr += reu_step ) {
            c = reu_ram[reu_addr % reu_size];
            reu_ram[reu_addr % reu_size]
                = mem_read((ADDRESS)(host_addr & 0xffff));
            mem_store((ADDRESS)(host_addr & 0xffff), c);
        }
        len = 1;
        reu[0] |= 0x40;
        break;

      case 3: /* compare */
#ifdef REU_DEBUG
        log_message(reu_log,
                    "compare ext $%05X %s<=> main $%04X%s, $%04X (%d) bytes.",
                    reu_addr, reu_step ? "" : "(fixed) ", host_addr,
                    host_step ? "" : " (fixed)", len, len);
#endif

        reu[0] &= ~0x60;

        while (len--) {
            if (reu_ram[reu_addr % reu_size]
                != mem_read((ADDRESS)(host_addr & 0xffff))) {

                host_addr += host_step;
                reu_addr += reu_step;

                reu[0] |=  0x20; /* FAULT */

                /* Bit 7: interrupt enable
                   Bit 5: interrupt on verify error */
                if (reu[9] & 0xa0) {
                    reu[0] |= 0x80;
                    maincpu_set_irq(I_REU, 1);
                }
                break;
            }
            host_addr += host_step;
            reu_addr += reu_step;
        }

        if (len < 0) {
            /* all bytes are equal, mark End Of Block */
            reu[0] |= 0x40;
            len = 1;
        }

        /*
        FIXME: [SRT] this should be done for all 4 operations,
        but 17xxTester gets an error when the length is set
        with low byte != 0 after copying HOST -> REU.
        */
        reu[7] = len & 0xff;
        reu[8] = (len >> 8) & 0xff;

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
        if (!(reu[0xA] & 0x80)) {
            reu[2] = host_addr & 0xff;
            reu[3] = (host_addr >> 8) & 0xff;
        }
        if (!(reu[0xA] & 0x40)) {
            reu[4] = reu_addr & 0xff;
            reu[5] = (reu_addr >> 8) & 0xff;
            reu[6] = (reu_addr >> 16);
        }

/* FIXME: [SRT] 17xxTester doesn't like a value of 1 here
   after transferring to REU, it expects that at least the
   low byte is zero.
   According to the manual of the 1750, after a transfer,
   the length is ALWAYS 1 (except for comparison, where it contains
   the count to retest when there was a difference).
   What's going on here? Is 17xxTester wrong (I don't believe it)? *

        reu[7] = len & 0xff;
        reu[8] = (len >> 8) & 0xff;
*/
    }

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

    if (snapshot_module_write_dword(m, (reu_size >> 10)) < 0
        || snapshot_module_write_byte_array(m, reu, sizeof(reu)) < 0
        || snapshot_module_write_byte_array(m, reu_ram, reu_size) < 0) {
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

    if (size > 16384) {
        log_error(reu_log, "Size %ld in snapshot not supported.", (long)size);
        goto fail;
    }

    set_reu_size((resource_value_t)size, NULL);

    if (!reu_enabled)
        set_reu_enabled((resource_value_t)1, NULL);

    if (snapshot_module_read_byte_array(m, reu, sizeof(reu)) < 0
        || snapshot_module_read_byte_array(m, reu_ram, reu_size) < 0)
        goto fail;

    if (reu[0] & 0x80)
        interrupt_set_irq_noclk(&maincpu_int_status, I_REU, 1);
    else
        interrupt_set_irq_noclk(&maincpu_int_status, I_REU, 0);

    snapshot_module_close(m);
    return 0;

fail:
    snapshot_module_close(m);
    return -1;
}

