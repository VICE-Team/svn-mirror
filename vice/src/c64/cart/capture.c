/*
 * capture.c - Cartridge handling, Capture cart.
 *
 * Written by
 *  Groepaz <groepaz@gmx.net>
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

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "c64meminit.h"
#include "c64memrom.h"
#include "c64tpi.h"
#include "capture.h"
#include "cartridge.h"
#include "machine.h"
#include "maincpu.h"
#include "types.h"
#include "vicii-mem.h"
#include "vicii-phi1.h"

/*
    define 1 for alternative config that does not force ultimax mode all the time.
    for some reason this way does not work properly, hence doing it the hard way :)
*/
#define NOFORCEULTIMAX1 0
#define NOFORCEULTIMAX2 0

static const c64export_resource_t export_res = {
    "Capture", 1, 1
};

static unsigned int cart_enabled = 0;
static unsigned int freeze_pressed = 0;
static unsigned int register_enabled = 0;

/*
mode & 3 = 0 : GAME=0 EXROM=0^1 roml
mode & 3 = 1 : GAME=1 EXROM=0^1 roml & romh
mode & 3 = 2 : GAME=0 EXROM=1^1 ram
mode & 3 = 3 : GAME=1 EXROM=1^1 ultimax
*/
static void capture_config_changed(BYTE mode_phi1, BYTE mode_phi2, unsigned int wflag)
{
    if (wflag == CMODE_WRITE) {
        machine_handle_pending_alarms(maincpu_rmw_flag + 1);
    } else {
        machine_handle_pending_alarms(0);
    }

    export.game = mode_phi2 & 1;
    export.exrom = ((mode_phi2 >> 1) & 1) ^ 1;

    mem_pla_config_changed();
    if (mode_phi2 & 0x80) {
        cartridge_release_freeze();
    }
    cart_ultimax_phi1 = (mode_phi1 & 1) & ((mode_phi1 >> 1) & 1);
    cart_ultimax_phi2 = export.game & (export.exrom ^ 1) & ((~mode_phi1 >> 2) & 1);
    machine_update_memory_ptrs();
}

/*
    the rest of the callbacks should map in cartridge memory or open i/o when 
    cart_enabled is 1, and wrap to the *_without_ultimax functions if the
    cartridge is disabled
*/

BYTE REGPARM1 capture_roml_read(WORD addr)
{
    if (cart_enabled == 0) {
        return mem_read_without_ultimax(addr);
    }
    return vicii_read_phi1();
}

void REGPARM2 capture_roml_store(WORD addr, BYTE value)
{
    if (cart_enabled == 0) {
        mem_store_without_ultimax(addr, value);
    }
}

/*
    7474  - 2 D-Flipflops (cart_enabled, register_enabled)
    74125 - 4 Tri-State Buffers
    74133 - NAND with 13 Inputs (adress decoder)

    the cartridge is disabled after a reset. 

    when the freeze button is pressed the following happens:
    - an NMI is generated
    - as soon as the current adress is in bank 0xfe the cart switches 
      to ultimax mode. the cart rom then contains one page full of
      "jmp $eaea", which ultimatively calls the freezer code.
    - the fff7/fff8 "register" logic is enabled

    after that any access (read or write) to $fff7 will turn the cart_enabled
    off (leave ultimax mode), and an access to $fff8 will turn the cart back
    on (enter ultimax mode). the "register logic" that causes this can only
    be disabled again by a hardware reset.
*/

void capture_mapper(WORD addr)
{
    if (freeze_pressed) {
        if ((addr & 0xff00) == 0xfe00) {
            cart_enabled = 1;
            freeze_pressed = 0;
#if NOFORCEULTIMAX1
            capture_config_changed(2, 3, CMODE_READ);
#endif
        }
    } else {
        if (register_enabled) {       
            if (cart_enabled) {
                if ((addr & 0xffff) == 0xfff7) {
                    cart_enabled = 0;
                    freeze_pressed = 0;
#if NOFORCEULTIMAX1
                    capture_config_changed(2, 2, CMODE_READ);
#endif
                }
            } else {
                if ((addr & 0xffff) == 0xfff8) {
                    cart_enabled = 1;
                    freeze_pressed = 0;
#if NOFORCEULTIMAX1
                    capture_config_changed(2, 3, CMODE_READ);
#endif
                }
            }
        }
    }
}

BYTE REGPARM1 capture_romh_read(WORD addr)
{
    capture_mapper(addr);

    if (cart_enabled) {
        return romh_banks[(addr & 0x1fff)];
    } else {
        return mem_read_without_ultimax(addr);
    }
}

void REGPARM2 capture_romh_store(WORD addr, BYTE value)
{
    capture_mapper(addr);

    if (cart_enabled == 0) {
        mem_store_without_ultimax(addr, value);
    }
}

/*
    there is Cartridge RAM at 0x6000..0x7fff
*/
BYTE REGPARM1 capture_1000_7fff_read(WORD addr)
{
    if (cart_enabled) {
        if (addr>=0x6000) {
            return export_ram0[addr-0x6000];
        } else {
            return vicii_read_phi1();
        }
    }

    return mem_read_without_ultimax(addr);
}

void REGPARM2 capture_1000_7fff_store(WORD addr, BYTE value)
{
    if (cart_enabled) {
        if (addr>=0x6000) {
            export_ram0[addr-0x6000] = value;
        }
    }

    mem_store_without_ultimax(addr, value);
}

BYTE REGPARM1 capture_a000_bfff_read(WORD addr)
{
    if (cart_enabled == 0) {
        return mem_read_without_ultimax(addr);
    }
    return vicii_read_phi1();
}

void REGPARM2 capture_a000_bfff_store(WORD addr, BYTE value)
{
    if (cart_enabled == 0) {
        mem_store_without_ultimax(addr, value);
    }
}

BYTE REGPARM1 capture_c000_cfff_read(WORD addr)
{
    if (cart_enabled == 0) {
        return mem_read_without_ultimax(addr);
    }
    return vicii_read_phi1();
}

void REGPARM2 capture_c000_cfff_store(WORD addr, BYTE value)
{
    if (cart_enabled == 0) {
        mem_store_without_ultimax(addr, value);
    }
}

/*
    the d000-dfff area is special, as chips and colorram are also 
    available in ultimax mode
*/

BYTE REGPARM1 capture_d000_dfff_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 capture_d000_dfff_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}

/******************************************************************************/

void capture_freeze(void)
{
    if (freeze_pressed == 0) {
#if NOFORCEULTIMAX2
        capture_config_changed(2, 3|0x80, CMODE_READ);
#else
        capture_config_changed(2, 3|0x80, CMODE_READ);
#endif
        freeze_pressed = 1;
        register_enabled = 1;
    }
}

void capture_config_init(void)
{
#if NOFORCEULTIMAX2
    capture_config_changed(2, 2, CMODE_READ);
#else
    capture_config_changed(2, 3, CMODE_READ);
#endif
}

void capture_reset(void)
{
    cart_enabled = 0;
    register_enabled = 0;
    freeze_pressed = 0;
#if NOFORCEULTIMAX2
    capture_config_changed(2, 2, CMODE_READ);
#else
    capture_config_changed(2, 3, CMODE_READ);
#endif

}

void capture_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000);
    memcpy(romh_banks, rawcart, 0x2000);
#if NOFORCEULTIMAX2
    capture_config_changed(2, 2, CMODE_READ);
#else
    capture_config_changed(2, 3, CMODE_READ);
#endif
}

int capture_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (fread(rawcart, 0x2000, 1, fd) < 1) {
        return -1;
    }

    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    return 0;
}

void capture_detach(void)
{
    c64export_remove(&export_res);
}
