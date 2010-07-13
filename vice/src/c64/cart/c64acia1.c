/*
 * c64acia1.c - Definitions for a 6551 ACIA interface
 *
 * Written by
 *  Andre' Fachat <fachat@physik.tu-chemnitz.de>
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

#define mycpu           maincpu
#define myclk           maincpu_clk
#define mycpu_rmw_flag  maincpu_rmw_flag
#define mycpu_clk_guard maincpu_clk_guard

#define myacia acia1

/* resource defaults */
#define MYACIA          "Acia1"
#define MyDevice        0
#define MyIrq           IK_IRQ

#define myaciadev       acia1dev

#define myacia_init acia1_init
#define myacia_init_cmdline_options acia1_cmdline_options_init
#define myacia_init_resources acia1_resources_init
#define myacia_snapshot_read_module acia1_snapshot_read_module
#define myacia_snapshot_write_module acia1_snapshot_write_module
#define myacia_peek acia1_peek
#define myacia_reset acia1_reset
#define myacia_store acia1_store

#include "maincpu.h"
#include "cartridge.h"
#include "c64export.h"
#include "c64io.h"

#define mycpu_alarm_context maincpu_alarm_context
#define mycpu_set_irq maincpu_set_irq
#define mycpu_set_nmi maincpu_set_nmi
#define mycpu_set_int_noclk maincpu_set_int

#include "aciacore.c"

#ifdef HAVE_RS232
/* Flag: Do we enable the $DE** ACIA RS232 interface emulation?  */
static int acia_de_enabled = 0;
#endif

/* ------------------------------------------------------------------------- */

#ifdef HAVE_RS232
/* a prototype is needed */
static BYTE REGPARM1 aciacart_read(WORD addr);

/* FIXME: indiviudal IDs should be used to reflect individual carts (unless 100% identical) */

static io_source_t acia_de_device = {
    "ACIA/SWIFTLINK/TURBO232",
    IO_DETACH_RESOURCE,
    "Acia1Enable",
    0xde00, 0xde07, 0x07,
    0,
    acia1_store,
    aciacart_read,
    NULL, /* FIXME: peek */
    NULL, /* FIXME: dump */
    CARTRIDGE_TURBO232
};

static io_source_list_t *acia_de_list_item = NULL;

static const c64export_resource_t export_res = {
    "Turbo232", 0, 0, &acia_de_device, NULL, CARTRIDGE_TURBO232
};
#endif

/* ------------------------------------------------------------------------- */

int aciacart_cart_enabled(void) {
#ifdef HAVE_RS232
    return acia_de_enabled;
#else
    return 0;
#endif 
}

#ifdef HAVE_RS232
static int acia1_enable(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }
    acia_de_list_item = c64io_register(&acia_de_device);
    return 0;
}

static void acia1_disable(void)
{
    c64export_remove(&export_res);
    c64io_unregister(acia_de_list_item);
    acia_de_list_item = NULL;
}

static int set_acia_de_enabled(int val, void *param)
{
    if ((val) && (!acia_de_enabled)) {
        if (acia1_enable() < 0) {
            return -1;
        }
        acia_de_enabled = 1;
    } else if ((!val) && (acia_de_enabled)) {
        acia1_disable();
        acia_de_enabled = 0;
    }
    return 0;
}
#endif

/* ------------------------------------------------------------------------- */

#ifdef HAVE_RS232
static const resource_int_t resources_i[] = {
    { "Acia1Enable", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &acia_de_enabled, set_acia_de_enabled, NULL },
    { NULL }
};
#endif

int aciacart_resources_init(void)
{
#ifdef HAVE_RS232
    if (acia1_resources_init() < 0) {
        return -1;
    }
    return resources_register_int(resources_i);
#else
    return 0;
#endif
}

void aciacart_resources_shutdown(void)
{

}

/* ------------------------------------------------------------------------- */

#ifdef HAVE_RS232
static BYTE REGPARM1 aciacart_read(WORD addr)
{
    acia_de_device.io_source_valid = 0;
    if (acia.mode == 2 && (addr & 7 )> 3 && (addr & 7) != 7) {
        return 0;
    }
    acia_de_device.io_source_valid = 1;
    return myacia_read(addr);
}
#endif

void aciacart_reset(void)
{
    acia1_reset();
}
void aciacart_init(void)
{
    acia1_init();
}

int aciacart_cmdline_options_init(void)
{
    return acia1_cmdline_options_init();
}

void aciacart_detach(void)
{
#ifdef HAVE_RS232
    set_acia_de_enabled(0, NULL);
#endif
}

int aciacart_enable(void)
{
#ifdef HAVE_RS232
    return set_acia_de_enabled(1, NULL);
#else
    return 0;
#endif
}

/* ------------------------------------------------------------------------- */

int aciacart_snapshot_write_module(struct snapshot_s *p)
{
#ifdef HAVE_RS232
    if (acia1_snapshot_write_module(p) < 0) {
        return -1;
    }
#endif
    return 0;
}
int aciacart_snapshot_read_module(struct snapshot_s *p)
{
#ifdef HAVE_RS232
    if (acia1_snapshot_read_module(p) < 0) {
        acia_de_enabled = 0;
        return -1;
    }
    /* FIXME: Why do we need to do so???  */
    if (acia1_enable() == 0) {
        aciacart_reset();          /* Clear interrupts.  */
        acia_de_enabled = 1;
    }
#endif
    return 0;
}
