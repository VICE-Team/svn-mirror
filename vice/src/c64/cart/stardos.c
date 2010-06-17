/*
 * stardos.c - Cartridge handling, StarDOS cart.
 *
 * (w)2008 Groepaz/Hitmen
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

#include "stardos.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "c64memrom.h"
#include "c64rom.h"
#include "cartridge.h"
#include "machine.h"
#include "resources.h"
#include "types.h"
#include "util.h"

/*  the stardos hardware is kindof perverted. it has two "registers", which
    are nothing more than the IO1 and/or IO2 line connected to a capacitor.
    the caps are then connected to a flipflop. now multiple reads of one of
    the "registers" charges a capacitor, which then when its charged enough
    causes the flipflop to switch. the output of the flipflop then controls
    the GAME line, ie it switches a rom bank at $8000 on or off.

    the original stardos code reads either $de61 or $dfa1 256 times in a loop,
    the emulation somewhat replicates this behaviour (delayed charging of the
    capacitor) by using a counter that is equivalent to the "charge" of the
    caps- and we assume that 256 reads are needed to succesfully switch.

    the second rom bank contains a kernal replacement. the necessary select
    signal comes from a clip that has to be installed inside of the c64.
*/

#define DBGSTARDOS

#ifdef DBGSTARDOS
#define DBG(x) printf x
#else
#define DBG(x)
#endif

static int cnt_de61, cnt_dfa1;
static int roml_enable;

/* ---------------------------------------------------------------------*/
#define CHARGETIME      0xfe

static BYTE REGPARM1 stardos_io1_read(WORD addr)
{
    ++cnt_de61;
    if (cnt_de61 > CHARGETIME) {
        /* enable bank 0 at $8000 */
        roml_enable = 1;
        cnt_dfa1 = 0;
        DBG(("STAROS: roml enable:%d\n",roml_enable));
    }

    return 0;
} 

static BYTE REGPARM1 stardos_io2_read(WORD addr)
{
    ++cnt_dfa1;
    if (cnt_dfa1 > CHARGETIME) {
        /* disable bank 0 at $8000 */
        roml_enable = 0;
        cnt_de61 = 0;
        DBG(("STAROS: roml enable:%d\n",roml_enable));
    }
    
    return 0;
}

/* ---------------------------------------------------------------------*/

static io_source_t stardos_io1_device = {
    "StarDOS",
    IO_DETACH_CART,
    NULL,
    0xde61, 0xde61, 0x01,
    0, /* read is never valid */
    NULL,
    stardos_io1_read,
    NULL,
    NULL,
    CARTRIDGE_STARDOS
};

static io_source_t stardos_io2_device = {
    "StarDOS",
    IO_DETACH_CART,
    NULL,
    0xdfa1, 0xdfa1, 0x01,
    0, /* read is never valid */
    NULL,
    stardos_io2_read,
    NULL,
    NULL,
    CARTRIDGE_STARDOS
};

static io_source_list_t *stardos_io1_list_item = NULL;
static io_source_list_t *stardos_io2_list_item = NULL;

static const c64export_resource_t export_res = {
    "StarDOS", 1, 1, &stardos_io1_device, &stardos_io2_device, CARTRIDGE_STARDOS
};

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 stardos_roml_read(WORD addr)
{
    if (roml_enable) {
        return roml_banks[(addr & 0x1fff)];
    } else {
        return mem_read_without_ultimax(addr);
    }
}

BYTE REGPARM1 stardos_romh_read(WORD addr)
{
    return romh_banks[(addr & 0x1fff)];
}

void stardos_config_init(void)
{
    cnt_de61 = 0;
    cnt_dfa1 = 0;
    roml_enable = 1;
    cartridge_config_changed(2, 3, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

/* not sure, the original hardware doesn't work like this.
   should probably be left out unless it will cause problems */
#if 0
void stardos_reset(void)
{
    cnt_de61 = 0;
    cnt_dfa1 = 0;
}
#endif

void stardos_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, &rawcart[0], 0x2000);
    memcpy(romh_banks, &rawcart[0x2000], 0x2000);

    cartridge_config_changed(2, 3, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static int stardos_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    stardos_io1_list_item = c64io_register(&stardos_io1_device);
    stardos_io2_list_item = c64io_register(&stardos_io2_device);

    return 0;
}

int stardos_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x4000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    return stardos_common_attach();
}

int stardos_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (fread(&rawcart[0x0000], 0x2000, 1, fd) < 1) {
        return -1;
    }

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (fread(&rawcart[0x2000], 0x2000, 1, fd) < 1) {
        return -1;
    }

    return stardos_common_attach();
}

void stardos_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(stardos_io1_list_item);
    c64io_unregister(stardos_io2_list_item);
    stardos_io1_list_item = NULL;
    stardos_io2_list_item = NULL;
}
