/*
 * c64tpi.c - IEEE488 interface for the C64.
 *
 * Written by
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <viceteam@t-online.de>
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

#include "archdep.h"
#include "c64.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "c64tpi.h"
#include "cartridge.h"
#include "drivecpu.h"
#include "lib.h"
#include "log.h"
#include "parallel.h"
#include "maincpu.h"
#include "resources.h"
#include "tpi.h"
#include "types.h"
#include "util.h"

/*
    IEEE488 interface for c64 and c128

    - the hardware uses a TPI at $DF00-$DF07 (mirrored through $DF08-$DFFF)

    TODO: register description

*/

/* #define DEBUGTPI */

#ifdef DEBUGTPI
#define DBG(x) printf x
#else
#define DBG(x)
#endif

#define mytpi_init tpi_init
#define mytpi_set_int tpi_set_int

static int ieee488_enabled = 0;

int tpi_cart_enabled(void)
{
    return ieee488_enabled;
}

/* ---------------------------------------------------------------------*/

static void REGPARM2 tpi_store(WORD addr, BYTE data)
{
    tpicore_store(machine_context.tpi1, addr, data);
}

static BYTE REGPARM1 tpi_read(WORD addr)
{
    return tpicore_read(machine_context.tpi1, addr);
}

BYTE REGPARM1 tpi_peek(WORD addr)
{
    return tpicore_peek(machine_context.tpi1, addr);
}

/* ---------------------------------------------------------------------*/

static io_source_t tpi_device = {
    "IEEE488",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0x07,
    1, /* read is always valid */
    tpi_store,
    tpi_read,
    NULL, /* FIXME: peek */
    NULL, /* FIXME: dump */
    CARTRIDGE_IEEE488
};

static io_source_list_t *tpi_list_item = NULL;

static const c64export_resource_t export_res = {
    "IEEE488", 0, 0, NULL, &tpi_device, CARTRIDGE_IEEE488
};

/* ---------------------------------------------------------------------*/

static void set_int(unsigned int int_num, int value)
{
}

static void restore_int(unsigned int int_num, int value)
{
}

static void set_ca(tpi_context_t *tpi_context, int a)
{
}

static void set_cb(tpi_context_t *tpi_context, int a)
{
}

static int ieee_is_dev = 1;
static BYTE ieee_is_out = 1;

static void reset(tpi_context_t *tpi_context)
{
    /* assuming input after reset */
    parallel_cpu_set_atn(0);
    parallel_cpu_set_ndac(0);
    parallel_cpu_set_nrfd(0);
    parallel_cpu_set_dav(0);
    parallel_cpu_set_eoi(0);
    parallel_cpu_set_bus(0xff);

    ieee_is_dev = 1;
    ieee_is_out = 1;
}

static void store_pa(tpi_context_t *tpi_context, BYTE byte)
{
    if (byte != tpi_context->oldpa) {
        BYTE tmp = ~byte;

        ieee_is_dev = byte & 0x01;
        ieee_is_out = byte & 0x02;

        parallel_cpu_set_bus((BYTE)(ieee_is_out ? tpi_context->oldpb : 0xff));

        if (ieee_is_out) {
            parallel_cpu_set_ndac(0);
            parallel_cpu_set_nrfd(0);
            parallel_cpu_set_dav((BYTE)(tmp & 0x10));
            parallel_cpu_set_eoi((BYTE)(tmp & 0x20));
        } else {
            parallel_cpu_set_nrfd((BYTE)(tmp & 0x80));
            parallel_cpu_set_ndac((BYTE)(tmp & 0x40));
            parallel_cpu_set_dav(0);
            parallel_cpu_set_eoi(0);
        }
        if (ieee_is_dev) {
            parallel_cpu_set_atn(0);
        } else {
            parallel_cpu_set_atn((BYTE)(tmp & 0x08));
        }
    }
}

static void store_pb(tpi_context_t *tpi_context, BYTE byte)
{
    parallel_cpu_set_bus((BYTE)(ieee_is_out ? byte : 0xff));
}

static void undump_pa(tpi_context_t *tpi_context, BYTE byte)
{
    BYTE tmp = ~byte;
    ieee_is_dev = byte & 0x01;
    ieee_is_out = byte & 0x02;

    parallel_cpu_set_bus((BYTE)(ieee_is_out ? tpi_context->oldpb : 0xff));

    if (ieee_is_out) {
        parallel_cpu_set_ndac(0);
        parallel_cpu_set_nrfd(0);
        parallel_cpu_set_dav((BYTE)(tmp & 0x10));
        parallel_cpu_set_eoi((BYTE)(tmp & 0x20));
    } else {
        parallel_cpu_set_nrfd((BYTE)(tmp & 0x80));
        parallel_cpu_set_ndac((BYTE)(tmp & 0x40));
        parallel_cpu_set_dav(0);
        parallel_cpu_set_eoi(0);
    }
    if (ieee_is_dev) {
        parallel_cpu_restore_atn(0);
    } else {
        parallel_cpu_restore_atn((BYTE)(tmp & 0x08));
    }
}

static void undump_pb(tpi_context_t *tpi_context, BYTE byte)
{
    parallel_cpu_set_bus((BYTE)(ieee_is_out ? byte : 0xff));
}

static void store_pc(tpi_context_t *tpi_context, BYTE byte)
{
    int exrom = ((byte & 8) ? 0 : 1); /* 1 = active */
    /* FIXME: passthrough support */
    cartridge_config_changed(exrom << 1, exrom << 1, CMODE_READ);
}

static void undump_pc(tpi_context_t *tpi_context, BYTE byte)
{
}

static BYTE read_pa(tpi_context_t *tpi_context)
{
    BYTE byte;

    drivecpu_execute_all(maincpu_clk);

    byte = 0xff;
    if (ieee_is_out) {
        if (parallel_nrfd) {
            byte &= 0x7f;
        }
        if (parallel_ndac) {
            byte &= 0xbf;
        }
    } else {
        if (parallel_dav) {
            byte &= 0xef;
        }
        if (parallel_eoi) {
            byte &= 0xdf;
        }
    }
    if (ieee_is_dev) {
        if (parallel_atn) {
            byte &= 0xf7;
        }
    }

    byte = (byte & ~(tpi_context->c_tpi)[TPI_DDPA]) | (tpi_context->c_tpi[TPI_PA] & tpi_context->c_tpi[TPI_DDPA]);

    return byte;
}

static BYTE read_pb(tpi_context_t *tpi_context)
{
    BYTE byte;

    drivecpu_execute_all(maincpu_clk);

    byte = ieee_is_out ? 0xff : parallel_bus;
    byte = (byte & ~(tpi_context->c_tpi)[TPI_DDPB]) | (tpi_context->c_tpi[TPI_PB] & tpi_context->c_tpi[TPI_DDPB]);

    return byte;
}

static BYTE read_pc(tpi_context_t *tpi_context)
{
    BYTE byte = (0xff & ~(tpi_context->c_tpi)[TPI_DDPC]) | (tpi_context->c_tpi[TPI_PC] & tpi_context->c_tpi[TPI_DDPC]);

    return byte;
}

/* ---------------------------------------------------------------------*/

void tpi_init(tpi_context_t *tpi_context)
{
    tpi_context->log = log_open(tpi_context->myname);
}

void tpi_setup_context(machine_context_t *machine_context)
{
    tpi_context_t *tpi_context;

    machine_context->tpi1 = lib_malloc(sizeof(tpi_context_t));
    tpi_context = machine_context->tpi1;

    tpi_context->prv = NULL;

    tpi_context->context = (void *)machine_context;

    tpi_context->rmw_flag = &maincpu_rmw_flag;
    tpi_context->clk_ptr = &maincpu_clk;

    tpi_context->myname = lib_msprintf("TPI");

    tpicore_setup_context(tpi_context);

    tpi_context->store_pa = store_pa;
    tpi_context->store_pb = store_pb;
    tpi_context->store_pc = store_pc;
    tpi_context->read_pa = read_pa;
    tpi_context->read_pb = read_pb;
    tpi_context->read_pc = read_pc;
    tpi_context->undump_pa = undump_pa;
    tpi_context->undump_pb = undump_pb;
    tpi_context->undump_pc = undump_pc;
    tpi_context->reset = reset;
    tpi_context->set_ca = set_ca;
    tpi_context->set_cb = set_cb;
    tpi_context->set_int = set_int;
    tpi_context->restore_int = restore_int;
}

/* ---------------------------------------------------------------------*/

static char *ieee488_filename = NULL;

static int set_ieee488_enabled(int val, void *param)
{
    DBG(("IEEE: set_enabled: (%p) '%s' %d to %d\n", param, ieee488_filename, ieee488_enabled, val));
    if (ieee488_enabled && !val) {
#ifdef DEBUGTPI
        if (tpi_list_item == NULL) {
            DBG(("IEEE: BUG: ieee488_enabled == 1 and tpi_list_item == NULL ?!\n"));
        }
#endif
        c64export_remove(&export_res);
        c64io_unregister(tpi_list_item);
        tpi_list_item = NULL;
        ieee488_enabled = 0;
        DBG(("IEEE: set_enabled unregistered\n"));
    } else if (!ieee488_enabled && val) {
        if (param) {
            /* if the param is != NULL, then we should load the default image file */
            if (ieee488_filename) {
                if (*ieee488_filename) {
                    DBG(("IEEE: attach default image\n"));
                    if (cartridge_attach_image(CARTRIDGE_IEEE488, ieee488_filename) < 0) {
                        DBG(("IEEE: set_enabled did not register\n"));
                        return -1;
                    }
                    /* ieee488_enabled = 1; */ /* cartridge_attach_image will end up calling set_ieee488_enabled again */
                    return 0;
                }
            }
        } else {
            /* if the param is == NULL, then we should actually set the resource */
            if (c64export_add(&export_res) < 0) {
                DBG(("IEEE: set_enabled did not register\n"));
                return -1;
            } else {
                DBG(("IEEE: set_enabled registered\n"));
                tpi_list_item = c64io_register(&tpi_device);
                ieee488_enabled = 1;
            }
        }
    }

    DBG(("IEEE: set_enabled done: '%s' %d : %d\n",ieee488_filename , val, ieee488_enabled));
    return 0;
}

static int set_ieee488_filename(const char *name, void *param)
{
    int enabled;

    if (name != NULL && *name != '\0') {
        if (util_check_filename_access(name) < 0) {
            return -1;
        }
    }
    DBG(("IEEE: set_name: %d '%s'\n",ieee488_enabled, ieee488_filename));

    util_string_set(&ieee488_filename, name);
    resources_get_int("IEEE488", &enabled);

    if (set_ieee488_enabled(enabled, (void*)1) < 0 ) {
        lib_free (ieee488_filename);
        ieee488_filename = NULL;
        DBG(("IEEE: set_name done: %d '%s'\n",ieee488_enabled, ieee488_filename));
        return -1;
    }

    DBG(("IEEE: set_name done: %d '%s'\n",ieee488_enabled, ieee488_filename));
    return 0;
}

static const resource_string_t resources_string[] = {
    { "IEEE488Image", "", RES_EVENT_NO, NULL,
      &ieee488_filename, set_ieee488_filename, NULL },
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "IEEE488", 0, RES_EVENT_SAME, NULL,
      &ieee488_enabled, set_ieee488_enabled, (void *)1 },
    { NULL }
};

int tpi_resources_init(void)
{
    if (resources_register_string(resources_string) < 0) {
        return -1;
    }
    return resources_register_int(resources_int);
}

void tpi_resources_shutdown(void)
{
    lib_free(ieee488_filename);
    ieee488_filename = NULL;
}

/* ---------------------------------------------------------------------*/

void tpi_config_init(void)
{
    cartridge_config_changed(0, 0, CMODE_READ);
}

static int tpi_common_attach(void)
{
    return set_ieee488_enabled(1, NULL);
}

int tpi_bin_attach(const char *filename, BYTE *rawcart)
{
    FILE *fd;

    fd = fopen(filename, MODE_READ);
    if (!fd) {
        return -1;
    }
    if (fread(rawcart, 0x1000, 1, fd) < 1) {
        fclose(fd);
        return -1;
    }
    fclose(fd);

    return tpi_common_attach();
}

int tpi_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (fread(&rawcart[0x0000], 0x1000, 1, fd) < 1) {
        return -1;
    }

    return tpi_common_attach();
}

void tpi_detach(void)
{
    set_ieee488_enabled(0, NULL);
}

/* ---------------------------------------------------------------------*/

int tpi_snapshot_read_module(struct snapshot_s *s)
{
    if (tpicore_snapshot_read_module(machine_context.tpi1, s) < 0) {
        ieee488_enabled = 0;
        return -1;
    } else {
        ieee488_enabled = 1;
    }
    return 0;
}

int tpi_snapshot_write_module(struct snapshot_s *s)
{
    if (tpicore_snapshot_write_module(machine_context.tpi1, s) < 0) {
        return -1;
    }
    return 0;
}
