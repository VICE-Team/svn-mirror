/*
 * vsidstubs.c - dummies for unneeded/unused functions
 *
 * Written by
 *  groepaz <groepaz@gmx.net> 
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "c64.h"
#include "c64cart.h"
#include "c64fastiec.h"
#include "c64iec.h"
#include "c64mem.h"
#include "c64-cmdline-options.h"
#include "cartridge.h"
#include "drive.h"
#include "gfxoutput.h"   /* FIXME: this include can be removed once the structs defined in this include are no longer used in stubs */
#include "midi.h"
#include "machine-printer.h"
#include "vicii-phi1.h"

/*******************************************************************************
    Memory related
*******************************************************************************/

int c64_256k_enabled = 0; /* always 0, needed from c64gluelogic.c */

/* needed from c64gluelogic.c */
void c64_256k_cia_set_vbank(int ciabank)
{
}

/*******************************************************************************
    Drive related
*******************************************************************************/

int machine_drive_resources_init(void)
{
    return 0;
}

void machine_drive_resources_shutdown(void)
{
}

int machine_drive_cmdline_options_init(void)
{
    return 0;
}

void machine_drive_init(struct drive_context_s *drv)
{
}

void machine_drive_shutdown(struct drive_context_s *drv)
{
}

void machine_drive_reset(struct drive_context_s *drv)
{
}

void machine_drive_mem_init(struct drive_context_s *drv, unsigned int type)
{
}

void machine_drive_setup_context(struct drive_context_s *drv)
{
}

void machine_drive_idling_method(unsigned int dnr)
{
}

void machine_drive_vsync_hook(void)
{
}

void machine_drive_rom_load(void)
{
}

void machine_drive_rom_setup_image(unsigned int dnr)
{
}

int machine_drive_rom_read(unsigned int type, WORD addr, BYTE *data)
{
    return -1;
}

int machine_drive_rom_check_loaded(unsigned int type)
{
    return -1;
}

void machine_drive_rom_do_checksum(unsigned int dnr)
{
}

int machine_drive_snapshot_read(struct drive_context_s *ctxptr, struct snapshot_s *s)
{
    return 0;
}

int machine_drive_snapshot_write(struct drive_context_s *ctxptr, struct snapshot_s *s)
{
    return 0;
}

int machine_drive_image_attach(struct disk_image_s *image, unsigned int unit)
{
    return -1;
}

int machine_drive_image_detach(struct disk_image_s *image, unsigned int unit)
{
    return -1;
}

void machine_drive_port_default(struct drive_context_s *drv)
{
}

void machine_drive_flush(void)
{
}

void machine_drive_stub(void)
{
}

/*******************************************************************************
    parallel cable
*******************************************************************************/

void parallel_cable_drive_write(BYTE data, int handshake, unsigned int dnr)
{
}

BYTE parallel_cable_drive_read(int handshake)
{
    return 0xff;
}

void parallel_cable_cpu_execute(void)
{
}

void parallel_cable_cpu_write(BYTE data)
{
}

BYTE parallel_cable_cpu_read(void)
{
    return 0xff;
}

void parallel_cable_cpu_pulse(void)
{
}

void parallel_cable_cpu_undump(BYTE data)
{
}

void parallel_bus_enable(int enable)
{
}

void parallel_trap_eof_callback_set(void (*func)(void))
{
}

void parallel_trap_attention_callback_set(void (*func)(void))
{
}

/*******************************************************************************
    Cartridge system
*******************************************************************************/

/* Expansion port signals.  */
export_t export = { 0, 0, 0, 0 }; /* c64 export */

/* Type of the cartridge attached. ("Main Slot") */
int mem_cartridge_type = CARTRIDGE_NONE;

int cartridge_resources_init(void)
{
    return 0;
}

void cartridge_resources_shutdown(void)
{
}

int cartridge_cmdline_options_init(void)
{
    return 0;
}

/*
    get filename of cart with given type
*/
const char *cartridge_get_file_name(int type)
{
    return "";
}

/*
    returns 1 if the cartridge of the given type is enabled

    - used by c64iec.c:iec_available_busses
*/
int cartridge_type_enabled(int type)
{
    return -1;
}

/*
    attach cartridge image

    type == -1  NONE
    type ==  0  CRT format

    returns -1 on error, 0 on success
*/
int cartridge_attach_image(int type, const char *filename)
{
    return -1;
}

void cartridge_detach_image(int type)
{
}

void cartridge_set_default(void)
{
}

int cartridge_save_image(int type, const char *filename)
{
    return -1;
}

/* trigger a freeze, but don't trigger the cartridge logic (which might release it). used by monitor */
void cartridge_trigger_freeze_nmi_only(void)
{
}

/* called by individual carts */
void cartridge_release_freeze(void)
{
}

/*
   called by the UI when the freeze button is pressed
*/
void cartridge_trigger_freeze(void)
{
}

/* called by c64.c:machine_specific_init */
void cartridge_init(void)
{
}

/* ROML read - mapped to 8000 in 8k,16k,ultimax */
BYTE roml_read(WORD addr)
{
    return vicii_read_phi1();
}

/* ROML store - mapped to 8000 in ultimax mode */
void roml_store(WORD addr, BYTE value)
{
}

/* ROMH read - mapped to A000 in 16k, to E000 in ultimax */
BYTE romh_read(WORD addr)
{
    return vicii_read_phi1();
}

/* ROMH read if hirom is selected - mapped to E000 in ultimax */
BYTE ultimax_romh_read_hirom(WORD addr)
{
    return vicii_read_phi1();
}

/* ROMH store - mapped to E000 in ultimax mode */
void romh_store(WORD addr, BYTE value)
{
}

/* ROMH store - A000-BFFF in 16kGame

   normally writes to ROM area would go to RAM an not generate
   a write select. some carts however map RAM here and also
   accept writes in this mode.
*/
void romh_no_ultimax_store(WORD addr, BYTE value)
{
    /* store to c64 ram */
    mem_store_without_romlh(addr, value);
}

/* ROML store - mapped to 8000-9fff in 8kGame, 16kGame

   normally writes to ROM area would go to RAM and not generate
   a write select. some carts however map ram here and also
   accept writes in this mode.
*/
void roml_no_ultimax_store(WORD addr, BYTE value)
{
    /* store to c64 ram */
    ram_store(addr, value);
}

/* RAML store (ROML _NOT_ selected) - mapped to 8000-9fff in 8kGame, 16kGame */
void raml_no_ultimax_store(WORD addr, BYTE value)
{
    /* store to c64 ram */
    ram_store(addr, value);
}

/* ultimax read - 1000 to 7fff */
BYTE ultimax_1000_7fff_read(WORD addr)
{
    /* default; no cart, open bus */
    return vicii_read_phi1();
}

/* ultimax store - 1000 to 7fff */
void ultimax_1000_7fff_store(WORD addr, BYTE value)
{
    /* default; no cart, open bus */
}

/* ultimax read - a000 to bfff */
BYTE ultimax_a000_bfff_read(WORD addr)
{
    /* default; no cart, open bus */
    return vicii_read_phi1();
}

/* ultimax store - a000 to bfff */
void ultimax_a000_bfff_store(WORD addr, BYTE value)
{
    /* default; no cart, open bus */
}

/* ultimax read - c000 to cfff */
BYTE ultimax_c000_cfff_read(WORD addr)
{
    /* default; no cart, open bus */
    return vicii_read_phi1();
}

/* ultimax store - c000 to cfff */
void ultimax_c000_cfff_store(WORD addr, BYTE value)
{
    /* default; no cart, open bus */
}

/* ultimax read - d000 to dfff */
BYTE ultimax_d000_dfff_read(WORD addr)
{
    /* default; no cart, c64 i/o */
    return read_bank_io(addr);
}

/* ultimax store - d000 to dfff */
void ultimax_d000_dfff_store(WORD addr, BYTE value)
{
    /* default;no cart, c64 i/o */
    store_bank_io(addr, value);
}

/* VIC-II reads from cart memory */
int ultimax_romh_phi1_read(WORD addr, BYTE *value)
{
    /* default; no cart, open bus */
    *value = vicii_read_phi1();
    return 1;
}

int ultimax_romh_phi2_read(WORD addr, BYTE *value)
{
    /* default; no cart, open bus */
    *value = vicii_read_phi1();
    return 1;
}

/* the following two are used by the old non cycle exact vic-ii emulation */
static BYTE mem_phi[0x1000];

BYTE *ultimax_romh_phi1_ptr(WORD addr)
{
    return mem_phi;
}

BYTE *ultimax_romh_phi2_ptr(WORD addr)
{
    return mem_phi;
}

BYTE cartridge_peek_mem(WORD addr)
{
    return 0;
}

/*
    returns 1 if the cartridge of the given type is enabled

    -  used by c64iec.c:iec_available_busses (checks for CARTRIDGE_IEEE488)
*/
int cart_type_enabled(int type)
{
    return 0;
}

/* called once by machine_setup_context */
void cartridge_setup_context(machine_context_t *machine_context)
{
}

int cartridge_enable(int type)
{
    return -1;
}

/* Initialize RAM for power-up.  */
void cartridge_ram_init(void)
{
}

/* called once by c64.c:machine_specific_shutdown at machine shutdown */
void cartridge_shutdown(void)
{
}

/*
    called from c64mem.c:mem_initialize_memory (calls XYZ_config_init)
*/
void cartridge_init_config(void)
{
}

void cartridge_reset(void)
{
}

int cartridge_flush_image(int type)
{
    return -1;
}

int cartridge_bin_save(int type, const char *filename)
{
    return -1;
}

int cartridge_crt_save(int type, const char *filename)
{
    return -1;
}

void cartridge_sound_chip_init(void)
{
}

int cartridge_snapshot_write_modules(struct snapshot_s *s)
{
    return 0;
}

int cartridge_snapshot_read_modules(struct snapshot_s *s)
{
    return 0;
}

int cart_is_slotmain(int type)
{
    return 0;
}

/*******************************************************************************
    individual carts
*******************************************************************************/

int digimax_cart_enabled = 0;
int digimax_is_userport = 0;

void digimax_sound_store(WORD addr, BYTE value)
{
}

midi_interface_t midi_interface[] = {
    { NULL }
};

void digimax_userport_store(WORD addr, BYTE value)
{
}

/*******************************************************************************
    gfxoutput drivers
*******************************************************************************/

int gfxoutput_early_init(void)
{
    return 0;
}

int gfxoutput_resources_init(void)
{
    return 0;
}

int gfxoutput_cmdline_options_init(void)
{
    return 0;
}

int gfxoutput_init(void)
{
    return 0;
}

void gfxoutput_shutdown(void)
{
}

int gfxoutput_num_drivers(void)
{
    return 0;
}

/* FIXME: this stub can be removed once all GUI's have been adapted to
          not use this call for vsid */
gfxoutputdrv_t *gfxoutput_get_driver(const char *drvname)
{
    return NULL;
}

/* FIXME: this stub can be removed once all GUI's have been adapted to
          not use this call for vsid */
gfxoutputdrv_t *gfxoutput_drivers_iter_next(void)
{
    return NULL;
}

/* FIXME: this stub can be removed once all GUI's have been adapted to
          not use this call for vsid */
gfxoutputdrv_t *gfxoutput_drivers_iter_init(void)
{
    return NULL;
}

/* FIXME: this table can be removed once all GUI's have been adapted to
          not use this table for vsid */
gfxoutputdrv_format_t ffmpegdrv_formatlist[] =
{
    { NULL, NULL, NULL }
};

/*******************************************************************************
    FIXME: this "function" is never called, the functions referenced here cause
           the linker to pull in certain other modules early (else we get linker
           errors).

           more stubs should be created so these can later be removed (hopefully)
*******************************************************************************/

void dummy(void)
{
    c64iec_init();
    c64fastiec_init();
    machine_printer_setup_context(&machine_context);
    c64_cmdline_options_init();
}
