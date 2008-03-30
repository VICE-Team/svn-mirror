/*
 * c610mem.c - CBM-II memory handling.
 *
 * Written by
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef C610
#define C610                     /* for mkdep */
#endif

#include "vice.h"

#include <stdio.h>

#include "types.h"
#include "memutils.h"
#include "crtc.h"
#include "kbd.h"
#include "kbdbuf.h"
#include "autostart.h"
#include "resources.h"
#include "cmdline.h"
#include "interrupt.h"
#include "vmachine.h"
#include "maincpu.h"
#include "emuid.h"
#include "utils.h"
#include "c610mem.h"
#include "c610tpi.h"
#include "c610acia.h"
#include "c610cia.h"
#include "sid.h"
#include "tapeunit.h"

/* ------------------------------------------------------------------------- */

/* The CBM-II memory. */

BYTE ram[C610_RAM_SIZE];  		/* 256k, banks 0-3 */
BYTE rom[C610_ROM_SIZE];		/* complete bank 15 */
BYTE chargen_rom[C610_CHARGEN_ROM_SIZE];

BYTE *page_zero;
BYTE *page_one;

int ram_size = C610_RAM_SIZE;		/* FIXME: referenced in mon.c */

/* selected banks for normal access and indirect accesses */
static int bank_exec = -1;
static int bank_ind = -1;

/* Memory read and write tables - banked. */
static read_func_ptr_t _mem_read_tab[16][0x101];
static store_func_ptr_t _mem_write_tab[16][0x101];
static BYTE *_mem_read_base_tab[16][0x101];

/* watch tables are fixed */
read_func_ptr_t _mem_read_tab_watch[0x101];
read_func_ptr_t _mem_read_ind_tab_watch[0x101];
store_func_ptr_t _mem_write_tab_watch[0x101];
store_func_ptr_t _mem_write_ind_tab_watch[0x101];

read_func_ptr_t *_mem_read_tab_ptr;
read_func_ptr_t *_mem_read_ind_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
store_func_ptr_t *_mem_write_ind_tab_ptr;
BYTE **_mem_read_base_tab_ptr;
BYTE **_mem_read_ind_base_tab_ptr;

/* Flag: nonzero if the ROM has been loaded. */
int rom_loaded = 0;

/* CRTC register pointer. */
static BYTE crtc_ptr = 0;

#define IS_NULL(s)  (s == NULL || *s == '\0')

/* prototype */
void set_screen(void);

/* ------------------------------------------------------------------------- */

static int ramsize;

static int set_ramsize(resource_value_t v)
{
    int rs = (int) v;
    if(rs==128 || rs==256 || rs==512 || rs==1024) {
	ramsize = rs;
	initialize_memory();
        return 0;
    }
    return -1;
}

/* ------------------------------------------------------------------------- */

/* Flag: Do we enable the Emulator ID?  */
static int emu_id_enabled;

static int set_emu_id_enabled(resource_value_t v)
{
    emu_id_enabled = (int)v;
    return 0;
}

/* Enable/disable the Emulator ID.  */
void mem_toggle_emu_id(int flag)
{
    emu_id_enabled = flag;
}

/* ------------------------------------------------------------------------- */

/* ROM names */
static char *kernal_rom_name = NULL;
static char *chargen_name = NULL;
static char *basic_rom_name = NULL;
static char *cart_2_name = NULL;
static char *cart_4_name = NULL;
static char *cart_6_name = NULL;
static int cart1_ram = 0;
static int cart2_ram = 0;
static int cart4_ram = 0;
static int cart6_ram = 0;

/* FIXME: Should load the new character ROM.  */
static int set_chargen_rom_name(resource_value_t v)
{
    const char *name = (const char *) v;

    if (chargen_name != NULL && name != NULL
        && strcmp(name, chargen_name) == 0)
        return 0;

    string_set(&chargen_name, name);
    return 0;
}

/* FIXME: Should load the new Kernal ROM.  */
static int set_kernal_rom_name(resource_value_t v)
{
    const char *name = (const char *) v;

    if (kernal_rom_name != NULL && name != NULL
        && strcmp(name, kernal_rom_name) == 0)
        return 0;

    string_set(&kernal_rom_name, name);
    return 0;
}

/* FIXME: Should load the new BASIC ROM.  */
static int set_basic_rom_name(resource_value_t v)
{
    const char *name = (const char *) v;

    if (basic_rom_name != NULL
        && name != NULL
        && strcmp(name, basic_rom_name) == 0)
        return 0;

    string_set(&basic_rom_name, name);
    return 0;
}

/* FIXME: Should load the new Cartridge ROM.  */
static int set_cart2_rom_name(resource_value_t v)
{
    const char *name = (const char *) v;

    if (cart_2_name != NULL
        && name != NULL
        && strcmp(name, cart_2_name) == 0)
        return 0;

    string_set(&cart_2_name, name);
    return 0;
}

/* FIXME: Should load the new Cartridge ROM.  */
static int set_cart4_rom_name(resource_value_t v)
{
    const char *name = (const char *) v;

    if (cart_4_name != NULL
        && name != NULL
        && strcmp(name, cart_4_name) == 0)
        return 0;

    string_set(&cart_4_name, name);
    return 0;
}

/* FIXME: Should load the new Cartridge ROM.  */
static int set_cart6_rom_name(resource_value_t v)
{
    const char *name = (const char *) v;

    if (cart_6_name != NULL
        && name != NULL
        && strcmp(name, cart_6_name) == 0)
        return 0;

    string_set(&cart_6_name, name);
    return 0;
}

static int set_cart1_ram(resource_value_t v)
{
    cart1_ram = (int) v;
    initialize_memory();
    return 0;
}

static int set_cart2_ram(resource_value_t v)
{
    cart2_ram = (int) v;
    initialize_memory();
    return 0;
}

static int set_cart4_ram(resource_value_t v)
{
    cart4_ram = (int) v;
    initialize_memory();
    return 0;
}

static int set_cart6_ram(resource_value_t v)
{
    cart6_ram = (int) v;
    initialize_memory();
    return 0;
}

/* ------------------------------------------------------------------------- */

static resource_t resources[] = {
    {"RamSize", RES_INTEGER, (resource_value_t) 128,
     (resource_value_t *) & ramsize, set_ramsize},
    { "ChargenName", RES_STRING, (resource_value_t) "chargen",
     (resource_value_t *) &chargen_name, set_chargen_rom_name },
    { "KernalName", RES_STRING, (resource_value_t) "kernal",
     (resource_value_t *) &kernal_rom_name, set_kernal_rom_name },
    { "BasicName", RES_STRING, (resource_value_t) "basic.b128",
     (resource_value_t *) &basic_rom_name, set_basic_rom_name },
    { "Cart2Name", RES_STRING, (resource_value_t) NULL,
     (resource_value_t *) &cart_2_name, set_cart2_rom_name },
    { "Cart4Name", RES_STRING, (resource_value_t) NULL,
     (resource_value_t *) &cart_4_name, set_cart4_rom_name },
    { "Cart6Name", RES_STRING, (resource_value_t) NULL,
     (resource_value_t *) &cart_6_name, set_cart6_rom_name },

    { "Ram1", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &cart1_ram, set_cart1_ram },
    { "Ram2", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &cart2_ram, set_cart2_ram },
    { "Ram4", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &cart4_ram, set_cart4_ram },
    { "Ram6", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &cart6_ram, set_cart6_ram },

    { "EmuID", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &emu_id_enabled, set_emu_id_enabled },
    { NULL }
};

int c610_mem_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-model", CALL_FUNCTION, 1, cbm2_set_model, NULL, NULL, NULL,
     "<modelnumber>", "Specify CBM-II model to emulate" },
    { "-ramsize", SET_RESOURCE, 1, NULL, NULL, "RamSize", NULL,
     "<ramsize>", "Specify size of RAM (128/256/512/1024 kByte)" },

    { "-kernal", SET_RESOURCE, 1, NULL, NULL, "KernalName", NULL,
      "<name>", "Specify name of Kernal ROM image" },
    { "-basic", SET_RESOURCE, 1, NULL, NULL, "BasicName", NULL,
      "<name>", "Specify name of BASIC ROM image" },
    { "-chargen", SET_RESOURCE, 1, NULL, NULL, "ChargenName", NULL,
      "<name>", "Specify name of character generator ROM image" },

    { "-cart2", SET_RESOURCE, 1, NULL, NULL, "Cart2Name", NULL,
      "<name>", "Specify name of cartridge ROM image for $2000" },
    { "-cart4", SET_RESOURCE, 1, NULL, NULL, "Cart4Name", NULL,
      "<name>", "Specify name of cartridge ROM image for $4000" },
    { "-cart6", SET_RESOURCE, 1, NULL, NULL, "Cart2Name", NULL,
      "<name>", "Specify name of cartridge ROM image for $6000" },

    { "-ram1", SET_RESOURCE, 0, NULL, NULL, "Ram1", (resource_value_t) 1, 
      NULL, "Enable RAM mapping in $1000-$1FFF" },
    { "+ram1", SET_RESOURCE, 0, NULL, NULL, "Ram1", (resource_value_t) 0, 
      NULL, "Disable RAM mapping in $1000-$1FFF" },
    { "-ram2", SET_RESOURCE, 0, NULL, NULL, "Ram2", (resource_value_t) 1, 
      NULL, "Enable RAM mapping in $2000-$3FFF" },
    { "+ram2", SET_RESOURCE, 0, NULL, NULL, "Ram2", (resource_value_t) 0, 
      NULL, "Disable RAM mapping in $2000-$3FFF" },
    { "-ram4", SET_RESOURCE, 0, NULL, NULL, "Ram4", (resource_value_t) 1, 
      NULL, "Enable RAM mapping in $4000-$5FFF" },
    { "+ram4", SET_RESOURCE, 0, NULL, NULL, "Ram4", (resource_value_t) 0, 
      NULL, "Disable RAM mapping in $4000-$5FFF" },
    { "-ram6", SET_RESOURCE, 0, NULL, NULL, "Ram6", (resource_value_t) 1, 
      NULL, "Enable RAM mapping in $6000-$7FFF" },
    { "+ram6", SET_RESOURCE, 0, NULL, NULL, "Ram6", (resource_value_t) 0, 
      NULL, "Disable RAM mapping in $6000-$7FFF" },

    { "-emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (resource_value_t) 1,
      NULL, "Enable emulator identification" },
    { "+emuid", SET_RESOURCE, 0, NULL, NULL, "EmuID", (resource_value_t) 0,
      NULL, "Disable emulator identification" },
    { NULL }
};

int c610_mem_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

static struct {
	const char *model;
	int ramsize;
	const char *basic;
    } modtab[] = {
    { "610",  128,  "basic.b128"  },
    { "620",  256,  "basic.b256"  },
    { "620+", 1024, "basic.b256"  },
    { NULL }
};

int cbm2_set_model(const char *model, void *extra)
{
    int i;
    for(i=0; modtab[i].model; i++) {
	if(!strcmp(modtab[i].model, model)) {
	    set_ramsize((resource_value_t)modtab[i].ramsize);
	    set_basic_rom_name((resource_value_t)modtab[i].basic);
	    mem_load();
	    return 0;
	}
    }
    return -1;
}

/* ------------------------------------------------------------------------- */

void set_bank_exec(int val) {
    int i;
    val &= 0x0f;
    if(val != bank_exec) {
 /* printf("set_bank_exec(%d)\n",val); */
    	bank_exec = val;
    	_mem_read_tab_ptr      = _mem_read_tab[bank_exec];
    	_mem_write_tab_ptr     = _mem_write_tab[bank_exec];
    	_mem_read_base_tab_ptr = _mem_read_base_tab[bank_exec];
    	/* set all register mirror locations */
	for(i=0;i<16;i++) {
	    ram[i<<16] = val;
	}
    	page_zero = _mem_read_base_tab_ptr[0];
    	page_one = _mem_read_base_tab_ptr[1];
    }
}

void set_bank_ind(int val) {
    int i;
    val &= 0x0f;
    if(val != bank_ind) {
/* printf("set_bank_ind(%d)\n",val); */
    	bank_ind = val;
    	_mem_read_ind_tab_ptr      = _mem_read_tab[bank_ind];
    	_mem_write_ind_tab_ptr     = _mem_write_tab[bank_ind];
    	_mem_read_ind_base_tab_ptr = _mem_read_base_tab[bank_ind];
    	/* set all register mirror locations */
	for(i=0;i<16;i++) {
	    ram[(i<<16) + 1] = val;
	}
    }
}

/* ------------------------------------------------------------------------- */

void REGPARM2 store_zero(ADDRESS addr, BYTE value)
{
    if(addr==0) set_bank_exec(value); else
    if(addr==1) set_bank_ind(value);

    _mem_write_tab_ptr[0](addr&0xff,value);
}

#define	STORE_ZERO(bank) 						\
    void REGPARM2 store_zero_##bank(ADDRESS addr, BYTE value)		\
    {									\
        addr &= 0xff;							\
									\
        if(addr==0) set_bank_exec(value); else				\
        if(addr==1) set_bank_ind(value);				\
									\
        ram[(0x##bank << 16) | addr] = value;				\
    }


#define	READ_ZERO(bank)							\
    BYTE REGPARM1 read_zero_##bank(ADDRESS addr)			\
    {									\
        return ram[(0x##bank << 16) | (addr & 0xff)];			\
    }

#define	READ_RAM(bank)							\
    BYTE REGPARM1 read_ram_##bank(ADDRESS addr)				\
    {									\
        return ram[(0x##bank << 16) | addr];				\
    }

#define	STORE_RAM(bank)							\
    void REGPARM2 store_ram_##bank(ADDRESS addr, BYTE byte)		\
    {									\
        ram[(0x##bank << 16) | addr] = byte;				\
    }

STORE_ZERO(0)
STORE_ZERO(1)
STORE_ZERO(2)
STORE_ZERO(3)
STORE_ZERO(4)
STORE_ZERO(5)
STORE_ZERO(6)
STORE_ZERO(7)
STORE_ZERO(8)
STORE_ZERO(9)
STORE_ZERO(A)
STORE_ZERO(B)
STORE_ZERO(C)
STORE_ZERO(D)
STORE_ZERO(E)
STORE_ZERO(F)

READ_ZERO(0)
READ_ZERO(1)
READ_ZERO(2)
READ_ZERO(3)
READ_ZERO(4)
READ_ZERO(5)
READ_ZERO(6)
READ_ZERO(7)
READ_ZERO(8)
READ_ZERO(9)
READ_ZERO(A)
READ_ZERO(B)
READ_ZERO(C)
READ_ZERO(D)
READ_ZERO(E)
READ_ZERO(F)

STORE_RAM(0)
STORE_RAM(1)
STORE_RAM(2)
STORE_RAM(3)
STORE_RAM(4)
STORE_RAM(5)
STORE_RAM(6)
STORE_RAM(7)
STORE_RAM(8)
STORE_RAM(9)
STORE_RAM(A)
STORE_RAM(B)
STORE_RAM(C)
STORE_RAM(D)
STORE_RAM(E)
STORE_RAM(F)

READ_RAM(0)
READ_RAM(1)
READ_RAM(2)
READ_RAM(3)
READ_RAM(4)
READ_RAM(5)
READ_RAM(6)
READ_RAM(7)
READ_RAM(8)
READ_RAM(9)
READ_RAM(A)
READ_RAM(B)
READ_RAM(C)
READ_RAM(D)
READ_RAM(E)
READ_RAM(F)

static void REGPARM2 (*store_zero_tab[16])(ADDRESS addr, BYTE value) = {
	store_zero_0, store_zero_1, store_zero_2, store_zero_3, 
	store_zero_4, store_zero_5, store_zero_6, store_zero_7,
	store_zero_8, store_zero_9, store_zero_A, store_zero_B, 
	store_zero_C, store_zero_D, store_zero_E, store_zero_F
};

static void REGPARM2 (*store_ram_tab[16])(ADDRESS addr, BYTE value) = {
	store_ram_0, store_ram_1, store_ram_2, store_ram_3, 
	store_ram_4, store_ram_5, store_ram_6, store_ram_7,
	store_ram_8, store_ram_9, store_ram_A, store_ram_B, 
	store_ram_C, store_ram_D, store_ram_E, store_ram_F
};

static BYTE REGPARM1 (*read_ram_tab[16])(ADDRESS addr) = {
	read_ram_0, read_ram_1, read_ram_2, read_ram_3, 
	read_ram_4, read_ram_5, read_ram_6, read_ram_7,
	read_ram_8, read_ram_9, read_ram_A, read_ram_B, 
	read_ram_C, read_ram_D, read_ram_E, read_ram_F
};

static BYTE REGPARM1 (*read_zero_tab[16])(ADDRESS addr) = {
	read_zero_0, read_zero_1, read_zero_2, read_zero_3, 
	read_zero_4, read_zero_5, read_zero_6, read_zero_7,
	read_zero_8, read_zero_9, read_zero_A, read_zero_B, 
	read_zero_C, read_zero_D, read_zero_E, read_zero_F
};


void REGPARM2 store_zeroX(ADDRESS addr, BYTE value)
{
    if(addr==0) set_bank_exec(value); else
    if(addr==1) set_bank_ind(value);
}

BYTE REGPARM1 read_rom(ADDRESS addr)
{
    return rom[addr];
}

void REGPARM2 store_rom(ADDRESS addr, BYTE value)
{
    rom[addr] = value;
}

static BYTE REGPARM1 read_unused(ADDRESS addr)
{
    return 0xff; /* (addr >> 8) & 0xff; */
}

static void REGPARM2 store_dummy(ADDRESS addr, BYTE value)
{
    return;
}

/* ------------------------------------------------------------------------- */

/* Functions for watchpoint memory access.  */

BYTE REGPARM1 read_watch(ADDRESS addr)
{
    mon_watch_push_load_addr(addr, e_comp_space);
    return _mem_read_tab[bank_exec][addr >> 8](addr);
}

void REGPARM2 store_watch(ADDRESS addr, BYTE value)
{
    mon_watch_push_store_addr(addr, e_comp_space);
    _mem_write_tab[bank_exec][addr >> 8](addr, value);
}

BYTE REGPARM1 read_ind_watch(ADDRESS addr)
{
    mon_watch_push_load_addr(addr, e_comp_space);
    return _mem_read_tab[bank_ind][addr >> 8](addr);
}

void REGPARM2 store_ind_watch(ADDRESS addr, BYTE value)
{
    mon_watch_push_store_addr(addr, e_comp_space);
    _mem_write_tab[bank_ind][addr >> 8](addr, value);
}

/* ------------------------------------------------------------------------- */

/* Generic memory access.  */

void REGPARM2 mem_store(ADDRESS addr, BYTE value)
{
    _mem_write_tab_ptr[addr >> 8](addr, value);
}

BYTE REGPARM1 mem_read(ADDRESS addr)
{
    return _mem_read_tab_ptr[addr >> 8](addr);
}

/* ------------------------------------------------------------------------- */

void REGPARM2 store_io(ADDRESS addr, BYTE value)
{
    switch(addr & 0xf800) {
    case 0xc000:
    case 0xc800:
	return;				/* unused */
    case 0xd000:
	store_rom(addr, value);		/* video RAM mapped here... */
	return;
    case 0xd800:
 	switch(addr & 0xff00) {
	case 0xd800:
            if (addr & 1)
                store_crtc(crtc_ptr, value);
            else
                crtc_ptr = value;
	    return;
	case 0xd900:
	    return;			/* disk units */
	case 0xda00:
	    store_sid(addr & 0xff, value);
	    return;
	case 0xdb00:
	    return; 			/* coprocessor */
	case 0xdc00:
	    store_cia1(addr & 0x0f, value);
	    return;
	case 0xdd00:
	    store_acia1(addr & 0x03, value);
	    return;
	case 0xde00:
	    store_tpi1(addr & 0x07, value);
	    return;
	case 0xdf00:
	    store_tpi2(addr & 0x07, value);
	    return;
	}
    }
}


BYTE REGPARM1 read_io(ADDRESS addr)
{
/*
    if (emu_id_enabled && addr >= 0xE8A0) {
        addr &= 0xff;
        if (addr == 0xff)
            emulator_id[addr - 0xa0] ^= 0xff;
        return emulator_id[addr - 0xa0];
    }
*/

    switch (addr & 0xf800) {
    case 0xc000:
    case 0xc800:
	return read_unused(addr);
    case 0xd000:
	return read_rom(addr);
    case 0xd800:
	switch (addr & 0xff00) {
	case 0xd800:
            if (addr & 1)
                return read_crtc(crtc_ptr);
            else
                return 0x9f;    /* Status. */
	case 0xd900:
	    return read_unused(addr);
	case 0xda00:
	    return read_sid(addr);
	case 0xdb00:
	    return read_unused(addr);
	case 0xdc00:
	    return read_cia1(addr);
	case 0xdd00:
	    return read_acia1(addr);
	case 0xde00:
	    return read_tpi1(addr & 0x07);
	case 0xdf00:
	    return read_tpi2(addr & 0x07);
	}
    }
    return read_unused(addr);
}


/* FIXME: TODO! */
void mem_toggle_watchpoints(int flag)
{
    if (flag) {
        _mem_read_tab_ptr = _mem_read_tab_watch;
        _mem_read_ind_tab_ptr = _mem_read_ind_tab_watch;
        _mem_write_tab_ptr = _mem_write_tab_watch;
        _mem_write_ind_tab_ptr = _mem_write_ind_tab_watch;
    } else {
	set_bank_exec(bank_exec);
	set_bank_ind(bank_ind);
    }
}

/* ------------------------------------------------------------------------- */

void initialize_memory(void)
{
    int i, j;

    /* first the tables that hold the predefined bank mappings */

    for (i=0;i<16;i++) {		/* 16 banks possible */
	switch (i) {
	case 1:
	case 2:
	    for (j=255;j>=0;j--) {
		_mem_read_tab[i][j] = read_ram_tab[i];
		_mem_write_tab[i][j] = store_ram_tab[i];
		_mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
	    }
	    _mem_write_tab[i][0] = store_zero_tab[i];
	    _mem_read_tab[i][0] = read_zero_tab[i];
	    break;
	case 3:
	case 4:
	    if(ramsize >= 256) {
	        for (j=255;j>=0;j--) {
	    	    _mem_read_tab[i][j] = read_ram_tab[i];
		    _mem_write_tab[i][j] = store_ram_tab[i];
		    _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
	        }
	        _mem_write_tab[i][0] = store_zero_tab[i];
	        _mem_read_tab[i][0] = read_zero_tab[i];
	        break;
	    }
	case 0:
	case 5:
	case 6:
	case 7:
	    if(ramsize >= 512) {
	        for (j=255;j>=0;j--) {
	    	    _mem_read_tab[i][j] = read_ram_tab[i];
		    _mem_write_tab[i][j] = store_ram_tab[i];
		    _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
	        }
	        _mem_write_tab[i][0] = store_zero_tab[i];
	        _mem_read_tab[i][0] = read_zero_tab[i];
	        break;
	    }
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	    if(ramsize >= 1024) {
	        for (j=255;j>=0;j--) {
	    	    _mem_read_tab[i][j] = read_ram_tab[i];
		    _mem_write_tab[i][j] = store_ram_tab[i];
		    _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
	        }
	        _mem_write_tab[i][0] = store_zero_tab[i];
	        _mem_read_tab[i][0] = read_zero_tab[i];
	        break;
	    }
	    /* fallback for ramsize < some_value */
	    for (j=255;j>=0;j--) {
		_mem_read_tab[i][j] = read_unused;
		_mem_write_tab[i][j] = store_dummy;
		_mem_read_base_tab[i][j] = NULL;
	    }
	    _mem_write_tab[i][0] = store_zeroX;
	    break;
	case 15:
	    for (j=0;j<0x10;j++) {
		_mem_read_tab[i][j] = read_ram_F;
		_mem_write_tab[i][j] = store_ram_F;
		_mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
	    }
	    for (;j<0xc0;j++) {
		_mem_read_tab[i][j] = read_rom;
		_mem_write_tab[i][j] = store_dummy;
		_mem_read_base_tab[i][j] = rom + (j << 8);
	    }
	    for (;j<0xe0;j++) {
		_mem_read_tab[i][j] = read_io;
		_mem_write_tab[i][j] = store_io;
		_mem_read_base_tab[i][j] = NULL;
	    }
	    for (;j<0x100;j++) {
		_mem_read_tab[i][j] = read_rom;
		_mem_write_tab[i][j] = store_dummy;
		_mem_read_base_tab[i][j] = rom + (j << 8);
	    }

	    if(cart1_ram) {
	        for (j=0x10;j<0x20;j++) {
		    _mem_read_tab[i][j] = read_ram_F;
		    _mem_write_tab[i][j] = store_ram_F;
		    _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
	        }
	    }
	    if(cart2_ram) {
	        for (j=0x20;j<0x40;j++) {
		    _mem_read_tab[i][j] = read_ram_F;
		    _mem_write_tab[i][j] = store_ram_F;
		    _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
	        }
	    }
	    if(cart4_ram) {
	        for (j=0x40;j<0x60;j++) {
		    _mem_read_tab[i][j] = read_ram_F;
		    _mem_write_tab[i][j] = store_ram_F;
		    _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
	        }
	    }
	    if(cart6_ram) {
	        for (j=0x60;j<0x80;j++) {
		    _mem_read_tab[i][j] = read_ram_F;
		    _mem_write_tab[i][j] = store_ram_F;
		    _mem_read_base_tab[i][j] = ram + (i << 16) + (j << 8);
	        }
	    }

	    _mem_write_tab[i][0] = store_zero_F;
	    _mem_read_tab[i][0] = read_zero_F;
	    _mem_read_base_tab[i][0] = ram + 0xf0000;
	    break;
	}
	_mem_read_tab[i][0x100] = _mem_read_tab[i][0];
	_mem_write_tab[i][0x100] = _mem_write_tab[i][0];
	_mem_read_base_tab[i][0x100] = _mem_read_base_tab[i][0];
    }

    /* set watchpoint tables */
    for (i=256;i>=0;i--) {
	_mem_read_tab_watch[i] = read_watch;
	_mem_read_ind_tab_watch[i] = read_ind_watch;
	_mem_write_tab_watch[i] = store_watch;
	_mem_write_ind_tab_watch[i] = store_ind_watch;
    }
}

void mem_powerup(void)
{
    int i;

#ifndef __MSDOS__
    printf("Initializing RAM for power-up...\n");
#endif
    for (i = 0; i < C610_RAM_SIZE; i += 0x80) {
        memset(ram + i, 0, 0x40);
        memset(ram + i + 0x40, 0xff, 0x40);
    }
    for (i = 0; i < 0x800; i += 0x80) {
        memset(rom + i, 0, 0x40);
        memset(rom + i + 0x40, 0xff, 0x40);
        memset(rom + 0x800 + i, 0, 0x40);
        memset(rom + 0x800 + i + 0x40, 0xff, 0x40);
        memset(rom + 0xd000 + i, 0, 0x40);
        memset(rom + 0xd000 + i + 0x40, 0xff, 0x40);
    }

    bank_exec = 0;
    bank_ind = 0;
    set_bank_exec(15);
    set_bank_ind(15);
}

/* Load memory image files. */
int mem_load(void)
{
    WORD sum;                   /* ROM checksum */
    int i,j,k;
    int rsize, krsize;

    /* De-initialize kbd-buf, autostart and tape stuff here before
       reloading the ROM the traps are installed in.  */
    kbd_buf_init(0, 0, 0, 0);
    autostart_init(0, 0, 0, 0, 0, 0);
    tape_init(0, 0, 0, 0, 0, 0, 0, NULL, 0, 0);

    /* Load chargen ROM
     * we load 4k or 8k of 16-byte-per-char Charrom. For now we shorten
     * it to 8 byte-per-char, then generate the inverted chars */

    memset(chargen_rom, 0, C610_CHARGEN_ROM_SIZE);

    if ((krsize=mem_load_sys_file(chargen_name, chargen_rom, 4096, 8192)) < 0) {
        fprintf(stderr, "Couldn't load character ROM.\n");
        return -1;
    }

    if(krsize < 8192) {
	memmove(chargen_rom, chargen_rom + 8192-krsize, krsize);
	memset(chargen_rom + krsize, 0, 8192-krsize);
    }

    /* shorten from 16 byte to 8 byte per char */
    for(k=0;k<4;k++) {
        for(i=0;i<128;i++) {
    	    for(j=0;j<8;j++) {
	        chargen_rom[k*2048+i*8+j] = chargen_rom[k*2048+i*16+j];
	    }
	}
    }

    /* Inverted chargen into second half. This is a hardware feature.  */
    for (i = 0; i < 1024; i++) {
        chargen_rom[i + 1024] = chargen_rom[i] ^ 0xff;
        chargen_rom[i + 3072] = chargen_rom[i + 2048] ^ 0xff;
        chargen_rom[i + 5120] = chargen_rom[i + 4096] ^ 0xff;
        chargen_rom[i + 7168] = chargen_rom[i + 6144] ^ 0xff;
    }

    /* Init Disk/Cartridge ROM with 'unused address' values.  */
    for (i = 0x1000; i < 0x8000; i++) {
        rom[i] = ((i >> 8) & 0xff);
    }

    /* Load Kernal ROM.  */
    if (!IS_NULL(kernal_rom_name)
        && ((krsize = mem_load_sys_file(kernal_rom_name,
                                        rom + 0xe000, 0x2000, 0x2000)) < 0)) {
        fprintf(stderr, "Couldn't load ROM `%s'.\n\n", kernal_rom_name);
        return -1;
    }

    /* Load BASIC ROM.  */
    if (!IS_NULL(basic_rom_name)
        && ((rsize = mem_load_sys_file(basic_rom_name,
                                   rom + 0x8000, 0x4000, 0x4000)) < 0)) {
        fprintf(stderr, "Couldn't load BASIC ROM `%s'.\n\n",
                basic_rom_name);
    }

    /* Load extension ROMs.  */
    if (!IS_NULL(cart_2_name)
        && ((rsize = mem_load_sys_file(cart_2_name,
                                   rom + 0x2000, 0x2000, 0x2000)) < 0)) {
        fprintf(stderr, "Couldn't load ROM `%s'.\n",
                cart_2_name);
    }
    if (!IS_NULL(cart_4_name)
        && ((rsize = mem_load_sys_file(cart_4_name,
                                   rom + 0x4000, 0x2000, 0x2000)) < 0)) {
        fprintf(stderr, "Couldn't load ROM `%s'.\n",
                cart_4_name);
    }
    if (!IS_NULL(cart_6_name)
         && ((rsize = mem_load_sys_file(cart_6_name,
				   rom + 0x6000, 0x2000, 0x2000)) < 0)) {
        fprintf(stderr, "Couldn't load ROM `%s'.\n",
                cart_6_name);
    }

    /* Checksum over top 8 kByte PET kernal.  */
    for (i = 0xe000, sum = 0; i < 0x10000; i++)
        sum += rom[i];

    printf("CBM-II: Loaded ROM, checksum is %d ($%04X).\n", sum, sum);

    crtc_set_screen_mode(rom + 0xd000, 0x7ff, 80, 1);

    rom_loaded = 1;

    return 0;
}

/* ------------------------------------------------------------------------- */

/* FIXME: this does not work at all */

void mem_get_basic_text(ADDRESS *start, ADDRESS *end)
{
    if (start != NULL)
        *start = ram[0x28] | (ram[0x29] << 8);
    if (end != NULL)
        *end = ram[0x2a] | (ram[0x2b] << 8);
}

void mem_set_basic_text(ADDRESS start, ADDRESS end)
{
    ram[0x28] = ram[0xc7] = start & 0xff;
    ram[0x29] = ram[0xc8] = start >> 8;
    ram[0x2a] = ram[0x2c] = ram[0x2e] = ram[0xc9] = end & 0xff;
    ram[0x2b] = ram[0x2d] = ram[0x2f] = ram[0xca] = end >> 8;
}

/* ------------------------------------------------------------------------- */

int mem_rom_trap_allowed(ADDRESS addr)
{
    return 1; 	/* (addr >= 0xf000) && !(map_reg & 0x80); */
}

void mem_set_tape_sense(int value)
{
}

/* ------------------------------------------------------------------------- */

/* Banked memory access functions for the monitor.  */

static BYTE peek_bank_io(ADDRESS addr)
{
/*
    if (emu_id_enabled && addr >= 0xE8A0) {
        addr &= 0xff;
        if (addr == 0xff)
            emulator_id[addr - 0xa0] ^= 0xff;
        return emulator_id[addr - 0xa0];
    }
*/

    switch (addr & 0xf800) {
    case 0xc000:
    case 0xc800:
        return read_unused(addr);
    case 0xd000:
        return read_rom(addr);
    case 0xd800:
        switch (addr & 0xff00) {
        case 0xd800:
            if (addr & 1)
                return read_crtc(crtc_ptr);
            else
                return 0x9f;    /* Status. */
        case 0xd900:
            return read_unused(addr);
        case 0xda00:
            return read_sid(addr);
        case 0xdb00:
            return read_unused(addr);
        case 0xdc00:
            return peek_cia1(addr);
        case 0xdd00:
            return peek_acia1(addr);
        case 0xde00:
            return peek_tpi1(addr & 0x07);
        case 0xdf00:
            return peek_tpi2(addr & 0x07);
        }
    }
    return read_unused(addr);
}

/* Exported banked memory access functions for the monitor.  */

static const char *banknames[] = {
    "default", "cpu", "ram0", "ram1", "ram2", "ram3", 
	"ram4", "ram5", "ram6", "ram7", "ram8", "ram9",
	"ramA", "ramB", "ramC", "ramD", "ramE", "ramF",
	"romio", NULL
};

static int banknums[] = {
    17, 17, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
};

const char **mem_bank_list(void)
{
    return banknames;
}

int mem_bank_from_name(const char *name)
{
    int i = 0;

    while (banknames[i]) {
        if (!strcmp(name, banknames[i])) {
            return banknums[i];
        }
        i++;
    }
    return -1;
}

BYTE mem_bank_read(int bank, ADDRESS addr)
{
    switch (bank) {
      case 17:                  /* current */
	  return mem_read(addr);
      case 16:                   /* romio */
	  if(addr>=0xc000 && addr <0xe000) return read_io(addr);
	  return _mem_read_tab[15][addr >> 8](addr);
      default:
	  if(bank >=0 && bank <15) {
	      return read_ram_tab[bank](addr);
	  }
    }
    return read_unused(addr);
}

BYTE mem_bank_peek(int bank, ADDRESS addr)
{
    if(bank==16) {
        if (addr >= 0xC000 && addr < 0xE000) {
            return peek_bank_io(addr);
        }
    }
    return mem_bank_read(bank, addr);
}

void mem_bank_write(int bank, ADDRESS addr, BYTE byte)
{
    switch (bank) {
      case 17:                   /* current */
        mem_store(addr, byte);
        return;
      case 16:
	if(addr >= 0xc000 && addr <=0xdfff) {
	    store_io(addr, byte);
	    return;
	}
	_mem_write_tab[15][addr >> 8](addr, byte);
	return;
      default:
	if(bank >=0 && bank < 16) {
	    if (addr & 0xff00) {
	        store_ram_tab[bank](addr, byte);
	    } else {
	        store_zero_tab[bank](addr, byte);
	    }
	    return;
	}
    }
    store_dummy(addr, byte);
}

