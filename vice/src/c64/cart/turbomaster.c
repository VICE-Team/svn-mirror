/*
 * turbomaster.c - Emulation of the Schneider 4Mhz 65c02 based Turbo Master
 *
 * Written by Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "alarm.h"
#include "c64cia.h"
#include "c64mem.h"
#include "cartio.h"
#include "clkguard.h"
#include "cmdline.h"
#include "debug.h"
#include "interrupt.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "monitor.h"
#include "r65c02.h"
#include "resources.h"
#include "snapshot.h"
#include "sysfile.h"
#include "translate.h"
#include "traps.h"
#include "turbomaster.h"
#include "types.h"
#include "util.h"

/* ------------------------------------------------------------------------- */

/* MACHINE_STUFF should define/undef

 - NEED_REG_PC
 - TRACE

 The following are optional:

 - PAGE_ZERO
 - PAGE_ONE
 - STORE_IND
 - LOAD_IND
 - DMA_FUNC
 - DMA_ON_RESET
 - CHECK_AND_RUN_ALTERNATE_CPU

*/

#define NEED_REG_PC

#ifdef NEED_REG_PC
unsigned int reg_pc;
#endif

#define JUMP(addr)                                                                         \
    do {                                                                                   \
        reg_pc = (unsigned int)(addr);                                                     \
    } while (0)

#define cpu_reset turbomaster_reset

#ifndef DMA_ON_RESET
#define DMA_ON_RESET
#endif

#ifndef CPU_ADDITIONAL_RESET
#define CPU_ADDITIONAL_RESET()
#endif

#ifndef CPU_ADDITIONAL_INIT
#define CPU_ADDITIONAL_INIT()
#endif

#ifndef DMA_FUNC
static void turbomaster_generic_dma(void)
{
    /* Generic DMA hosts can be implemented here.
       For example a very accurate REU emulation. */
}
#define DMA_FUNC turbomaster_generic_dma()
#endif

/* Information about the last executed opcode.  This is used to know the
   number of write cycles in the last executed opcode and to delay interrupts
   by one more cycle if necessary, as happens with conditional branch opcodes
   when the branch is taken.  */
unsigned int last_opcode_info;

/* Address of the last executed opcode. This is used by watchpoints. */
unsigned int last_opcode_addr;

/* Return nonzero if a pending NMI should be dispatched now.  This takes
   account for the internal delays of the 65c02, but does not actually check
   the status of the NMI line.  */
inline static int interrupt_check_nmi_delay(interrupt_cpu_status_t *cs,
                                            CLOCK cpu_clk)
{
    unsigned int delay_cycles = INTERRUPT_DELAY;

    /* BRK (0x00) delays the NMI by one opcode.  */
    /* TODO DO_INTERRUPT sets last opcode to 0: can NMI occur right after IRQ? */
    if (OPINFO_NUMBER(*cs->last_opcode_info_ptr) == 0x00) {
        return 0;
    }

    /* Branch instructions delay IRQs and NMI by one cycle if branch
       is taken with no page boundary crossing.  */
    if (OPINFO_DELAYS_INTERRUPT(*cs->last_opcode_info_ptr)) {
        delay_cycles++;
    }

    if (cs->nmi_delay_cycles >= delay_cycles) {
        return 1;
    }

    return 0;
}

/* Return nonzero if a pending IRQ should be dispatched now.  This takes
   account for the internal delays of the 6510, but does not actually check
   the status of the IRQ line.  */
inline static int interrupt_check_irq_delay(interrupt_cpu_status_t *cs,
                                            CLOCK cpu_clk)
{
    CLOCK irq_clk = cs->irq_clk + INTERRUPT_DELAY;

    /* Branch instructions delay IRQs and NMI by one cycle if branch
       is taken with no page boundary crossing.  */
    if (OPINFO_DELAYS_INTERRUPT(*cs->last_opcode_info_ptr)) {
        irq_clk++;
    }

    /* If an opcode changes the I flag from 1 to 0, the 6510 needs
       one more opcode before it triggers the IRQ routine.  */
    if (cpu_clk >= irq_clk) {
        if (!OPINFO_ENABLES_IRQ(*cs->last_opcode_info_ptr)) {
            return 1;
        } else {
            cs->global_pending_int |= IK_IRQPEND;
        }
    }
    return 0;
}

static void WDC_STP(void)
{
}

static void WDC_WAI(void)
{
}

#if !defined WORDS_BIGENDIAN && defined ALLOW_UNALIGNED_ACCESS

#define opcode_t DWORD

#define FETCH_OPCODE(o)                        \
    do {                                       \
        o = LOAD(reg_pc);                      \
        if (fetch_tab[o & 0xff]) {             \
            o |= LOAD(reg_pc + 1) << 8;        \
            if (fetch_tab[o & 0xff] - 1) {     \
                o |= (LOAD(reg_pc + 2) << 16); \
            }                                  \
        }                                      \
    } while (0)

#define p0 (opcode & 0xff)
#define p1 ((opcode >> 8) & 0xff)
#define p2 (opcode >> 8)

#else /* WORDS_BIGENDIAN || !ALLOW_UNALIGNED_ACCESS */

#define opcode_t         \
    struct {             \
        BYTE ins;        \
        union {          \
            BYTE op8[2]; \
            WORD op16;   \
        } op;            \
    }

#define FETCH_OPCODE(o)                                 \
    do {                                                \
        (o).ins = LOAD(reg_pc);                         \
        if (fetch_tab[(o).ins]) {                       \
            (o).op.op16 = LOAD(reg_pc + 1);             \
            if (fetch_tab[(o).ins - 1]) {               \
                (o).op.op16 |= (LOAD(reg_pc + 2) << 8); \
            }                                           \
        }                                               \
    } while (0)

#define p0 (opcode.ins)
#define p2 (opcode.op.op16)

#ifdef WORDS_BIGENDIAN
#  define p1 (opcode.op.op8[1])
#else
#  define p1 (opcode.op.op8[0])
#endif

#endif /* !WORDS_BIGENDIAN */

/* ------------------------------------------------------------------------- */

static BYTE turbomaster_ram[0x10000];
static BYTE turbomaster_basic1[0x2000];
static BYTE turbomaster_basic2[0x2000];
static BYTE turbomaster_kernal1[0x2000];
static BYTE turbomaster_kernal2[0x2000];
static BYTE turbomaster_cpu_port0 = 0;
static BYTE turbomaster_cpu_port1 = 0xff;

static int turbomaster_bank_config = 7;
static int softmode_speed = 4;

/* switch 1, 65c02 enable */
static int turbomaster_s1_cpu_started = 0;

/* switch 2, rom1/rom2 switch */
static int turbomaster_s2_rom = 1;

/* switch 3, speed control, 0: soft / 1: manual */
static int turbomaster_s3_speed = 0;

/* switch 4, MHZ, 1: 1Mhz / 4: 4Mhz */
static int turbomaster_s4_mhz = 4;

static int turbomaster_enabled = 0;
static int turbomaster_4Mhz_cycle = 0;

static char *turbomaster_rom1_filename = NULL;
static char *turbomaster_rom2_filename = NULL;

/* ------------------------------------------------------------------------- */

#define CLK_ADD(clock, amount)

#define LOAD(addr) (turbomaster_mem_read_tab[(addr) >> 8][turbomaster_bank_config])((WORD)(addr))
#define LOAD_ZERO(addr) (turbomaster_mem_read_tab[0][turbomaster_bank_config])((WORD)(addr))

#define STORE(addr, value) (turbomaster_mem_write_tab[(addr) >> 8][turbomaster_bank_config])((WORD)(addr), (BYTE)(value))
#define STORE_ZERO(addr, value) (turbomaster_mem_write_tab[0][turbomaster_bank_config])((WORD)(addr), (BYTE)(value))

#define LOAD_ADDR(addr) \
    ((LOAD((addr) + 1) << 8) | LOAD(addr))

#define LOAD_ZERO_ADDR(addr) \
    ((LOAD_ZERO((addr) + 1) << 8) | LOAD_ZERO(addr))

#define LOAD_ZERO_ADDR_X(addr) \
    ((LOAD_ZERO(addr)), (LOAD_ZERO((addr) + reg_x)) | (LOAD_ZERO((addr) + reg_x + 1) << 8))

#ifndef STORE_IND
#define STORE_IND(addr, value) STORE((addr), (value))
#endif

#ifndef LOAD_IND
#define LOAD_IND(addr) LOAD((addr))
#endif

#define PUSH(val) (LOAD(reg_pc + 1)), (turbomaster_ram_store((WORD)(reg_sp-- | 0x100), (BYTE)(val)))
#define PULL() (LOAD(reg_pc + 1)), (LOAD(reg_sp | 0x100)), (turbomaster_ram_read((WORD)(++reg_sp | 0x100)))

#define LOAD_IND_X(addr) (LOAD(LOAD_ZERO_ADDR_X((addr) + reg_x)))

#define LOAD_ZERO_X(addr) ((LOAD((addr))), (LOAD_ZERO((addr) + reg_x)))

#define LOAD_ZERO_Y(addr) ((LOAD((addr))), (LOAD_ZERO((addr) + reg_y)))

#define LOAD_IND_Y(addr)                                      \
    ((tmp_address = (LOAD_ZERO_ADDR((addr)) & 0xff) + reg_y), \
      (tmp_address > 0xff ?                                   \
       (LOAD(reg_pc + 1),                                     \
        LOAD(tmp_address))                                    \
      : (LOAD(tmp_address))))

#define LOAD_INDIRECT(addr) (LOAD(LOAD_ZERO_ADDR((addr))))

#define LOAD_ABS_Y(addr)              \
    ((((addr) & 0xff) + reg_y) > 0xff \
     ? (LOAD(reg_pc + 2),             \
        LOAD((addr) + reg_y))         \
     : LOAD((addr) + reg_y))

#define LOAD_ABS_X(addr)              \
    ((((addr) & 0xff) + reg_x) > 0xff \
     ? (LOAD(reg_pc + 2),             \
        LOAD((addr) + reg_x))         \
     : LOAD((addr) + reg_x))

#define LOAD_IND_Y_BANK(addr) LOAD_IND_Y((addr))

#define LOAD_ABS_X_RMW(addr) \
    (LOAD(reg_pc + 2),       \
     LOAD((addr) + reg_x))

/* ------------------------------------------------------------------------- */

#define CLK_INCREMENT ++maincpu_clk

static int turbomaster_running_speed = 4;

static void turbomaster_check_4Mhz_mode(void)
{
    if (softmode_speed == 4 && !turbomaster_s3_speed) {
        turbomaster_running_speed = 4;
    } else if (turbomaster_s3_speed && turbomaster_s4_mhz == 4) {
        turbomaster_running_speed = 4;
    } else {
        turbomaster_running_speed = 1;
    }
}

static void turbomaster_clk_inc(void)
{
    if (turbomaster_running_speed == 4) {
        ++turbomaster_4Mhz_cycle;
        if (turbomaster_4Mhz_cycle == 4) {
            CLK_INCREMENT;
            turbomaster_4Mhz_cycle = 0;
        }
    } else {
        CLK_INCREMENT;
    }
}

static void turbomaster_clk_inc_stretch(void)
{
    CLK_INCREMENT;
    if (turbomaster_running_speed == 4) {
        CLK_INCREMENT;
        if (turbomaster_4Mhz_cycle) {
            turbomaster_4Mhz_cycle = 0;
        }
    }
}

/* ------------------------------------------------------------------------- */

static read_func_ptr_t turbomaster_io_read_tab[0x11] = {
    c64io_d000_read,
    c64io_d100_read,
    c64io_d200_read,
    c64io_d300_read,
    c64io_d400_read,
    c64io_d500_read,
    c64io_d600_read,
    c64io_d700_read,
    colorram_read,
    colorram_read,
    colorram_read,
    colorram_read,
    cia1_read,
    cia2_read,
    c64io_de00_read,
    c64io_df00_read,
    NULL
};

static store_func_ptr_t turbomaster_io_write_tab[0x11] = {
    c64io_d000_store,
    c64io_d100_store,
    c64io_d200_store,
    c64io_d300_store,
    c64io_d400_store,
    c64io_d500_store,
    c64io_d600_store,
    c64io_d700_store,
    colorram_store,
    colorram_store,
    colorram_store,
    colorram_store,
    cia1_store,
    cia2_store,
    c64io_de00_store,
    c64io_df00_store,
    NULL
};

static BYTE turbomaster_zeroram_read(WORD addr)
{
    turbomaster_clk_inc();
    if (addr > 1) {
        return turbomaster_ram[addr];
    }
    if (addr == 0) {
        return turbomaster_cpu_port0;
    }
    return turbomaster_cpu_port1;
}

static void turbomaster_zeroram_store(WORD addr, BYTE val)
{
    if (addr > 1) {
        turbomaster_clk_inc_stretch();
        turbomaster_ram[addr] = val;
        ram_store(addr, val);
    } else if (addr == 0) {
        turbomaster_clk_inc();
        turbomaster_cpu_port0 = val;
        if (val & 0x80) {
            softmode_speed = 1;
        } else {
            softmode_speed = 4;
        }
        turbomaster_check_4Mhz_mode();
    } else {
        turbomaster_clk_inc();
        turbomaster_cpu_port1 = val;
        turbomaster_bank_config = val & 7;
    }
}

static BYTE turbomaster_ram_read(WORD addr)
{
    turbomaster_clk_inc();
    return turbomaster_ram[addr];
}

static void turbomaster_ram_store(WORD addr, BYTE val)
{
    turbomaster_clk_inc_stretch();
    turbomaster_ram[addr] = val;
    ram_store(addr, val);
}

static BYTE turbomaster_basic_read(WORD addr)
{
    BYTE *current_basic = (turbomaster_s2_rom == 1) ? turbomaster_basic1 : turbomaster_basic2;

    turbomaster_clk_inc();
    return current_basic[addr & 0x1fff];
}

static void turbomaster_dxxx_ram_store(WORD addr, BYTE val)
{
    turbomaster_clk_inc();
    turbomaster_ram[addr] = val;
}

static BYTE turbomaster_chargen_read(WORD addr)
{
    turbomaster_clk_inc();
    return mem_chargen_rom[addr & (C64_CHARGEN_ROM_SIZE - 1)];
}

static BYTE turbomaster_io_read(WORD addr)
{
    int index = (addr >> 8) & 0xf;

    turbomaster_clk_inc_stretch();
    return turbomaster_io_read_tab[index](addr);
}

static void turbomaster_io_store(WORD addr, BYTE val)
{
    int index = (addr >> 8) & 0xf;

    turbomaster_clk_inc_stretch();
    turbomaster_io_write_tab[index](addr, val);
}

static BYTE turbomaster_kernal_read(WORD addr)
{
    BYTE *current_kernal = (turbomaster_s2_rom == 1) ? turbomaster_kernal1 : turbomaster_kernal2;

    turbomaster_clk_inc();
    return current_kernal[addr & 0x1fff];
}

/* ------------------------------------------------------------------------- */

static read_func_ptr_t turbomaster_mem_read_tab[0x101][8];
static store_func_ptr_t turbomaster_mem_write_tab[0x101][8];

static void set_read_item(int index, int config, BYTE (*func)(WORD addr))
{
    turbomaster_mem_read_tab[index][config] = func;
}

static void set_write_item(int index, int config, void (*func)(WORD addr, BYTE val))
{
    turbomaster_mem_write_tab[index][config] = func;
}

/* 
config charen hiram loram $00-$9f $a0-$bf    $c0-cf $d0-$df   $e0-$ff
  0      0      0     0     RAM     RAM        RAM    RAM       RAM
  1      0      0     1     RAM     RAM        RAM  CHAR ROM    RAM
  2      0      1     0     RAM     RAM        RAM  CHAR ROM  KERNAL ROM
  3      0      1     1     RAM   BASIC ROM    RAM  CHAR ROM  KERNAL ROM
  4      1      0     0     RAM     RAM        RAM    RAM       RAM
  5      1      0     1     RAM     RAM        RAM    I/O       RAM
  6      1      1     0     RAM     RAM        RAM    I/O     KERNAL ROM
  7      1      1     1     RAM   BASIC ROM    RAM    I/O     KERNAL ROM
*/

static void turbomaster_mem_init(void)
{
    int i;
    int j;

    /* $0000-$00ff, all zero page RAM */
    for (j = 0; j < 8; ++j) {
        set_read_item(0, j, turbomaster_zeroram_read);
        set_write_item(0, j, turbomaster_zeroram_store);
    }

    /* $0100-$9fff, all RAM */
    for (j = 0; j < 8; ++j) {
        for (i = 1; i < 0xa0; ++i) {
            set_read_item(i, j, turbomaster_ram_read);
            set_write_item(i, j, turbomaster_ram_store);
        }
    }

    /* $a000-$bfff, BASIC ROM in configs 3&7, in all others RAM */
    for (j = 0; j < 8; ++j) {
        for (i = 0xa0; i < 0xc0; ++i) {
            if (j == 3 || j == 7) {
                set_read_item(i, j, turbomaster_basic_read);
            } else {
                set_read_item(i, j, turbomaster_ram_read);
            }
            set_write_item(i, j, turbomaster_ram_store);
        }
    }

    /* $c000-$cfff, all RAM */
    for (j = 0; j < 8; ++j) {
        for (i = 0xc0; i < 0xd0; ++i) {
            set_read_item(i, j, turbomaster_ram_read);
            set_write_item(i, j, turbomaster_ram_store);
        }
    }

    /* $d000-$dfff, config 1/2/3 CHAR ROM, 0&4 RAM and 5/6/7 KERNAL ROM */
    for (j = 0; j < 8; ++j) {
        for (i = 0xd0; i < 0xe0; ++i) {
            switch (j) {
                case 0:
                case 4:
                    set_read_item(i, j, turbomaster_ram_read);
                    set_write_item(i, j, turbomaster_dxxx_ram_store);
                    break;
                case 1:
                case 2:
                case 3:
                    set_read_item(i, j, turbomaster_chargen_read);
                    set_write_item(i, j, turbomaster_dxxx_ram_store);
                    break;
                default:
                    set_read_item(i, j, turbomaster_io_read);
                    set_write_item(i, j, turbomaster_io_store);
                    break;
            }
        }
    }

    /* $e000-$ffff, configs 0/1/4/5 RAM, 2/3/6/7 KERNAL ROM */
    for (j = 0; j < 8; ++j) {
        for (i = 0xe0; i < 0x100; ++i) {
            if (j == 0 || j == 1 || j == 4 || j == 5) {
                set_read_item(i, j, turbomaster_ram_read);
                set_write_item(i, j, turbomaster_ram_store);
            } else {
                set_read_item(i, j, turbomaster_kernal_read);
                set_write_item(i, j, turbomaster_ram_store);
            }
        }
    }
}

/* ------------------------------------------------------------------------- */

static int turbomaster_load_rom(const char *rom_name, int rom_nr)
{
    int size;
    BYTE turbomaster_rom[0x4000];

    /* Load turbomaster ROM.  */
    size = sysfile_load(rom_name, turbomaster_rom, 0x4000, 0x4000);
    if (size < 0 || (size & (size - 1))) {
        log_error(LOG_DEFAULT, "Couldn't load TurboMaster ROM `%s'.", rom_name);
        return -1;
    }
    if (rom_nr == 1) {
        memcpy(turbomaster_basic1, turbomaster_rom, 0x2000);
        memcpy(turbomaster_kernal1, turbomaster_rom + 0x2000, 0x2000);
    } else {
        memcpy(turbomaster_basic2, turbomaster_rom, 0x2000);
        memcpy(turbomaster_kernal2, turbomaster_rom + 0x2000, 0x2000);
    }
    return 0;
}

/* ------------------------------------------------------------------------- */

int turbomaster_cart_enabled(void)
{
    return turbomaster_enabled;
}

static int set_turbomaster_enabled(int value, void *param)
{
    int val = value ? 1 : 0;

    if (turbomaster_enabled != val) {
        if (val) {
            turbomaster_enabled = 1;
            turbomaster_reset();
        } else {
            turbomaster_enabled = 0;
            turbomaster_s1_cpu_started = 0;
        }
    }
    return 0;
}

static int set_turbomaster_cpu_started(int value, void *param)
{
    int val = (value) ? 1 : 0;

    if (val == turbomaster_s1_cpu_started) {
        return 0;
    }

    turbomaster_s1_cpu_started = value;

    return 0;
}

static int set_turbomaster_rom(int value, void *param)
{
    if (value != 1 && value != 2) {
        return -1;
    }

    turbomaster_s2_rom = value;

    return 0;
}

static int set_turbomaster_speed_control(int value, void *param)
{
    int val = (value) ? 1 : 0;

    if (turbomaster_s3_speed != val) {
        turbomaster_s3_speed = val;
        turbomaster_check_4Mhz_mode();
    }
    return 0;
}

static int set_turbomaster_mhz(int value, void *param)
{
    if (value != 1 && value != 4) {
        return -1;
    }

    if (turbomaster_s4_mhz != value) {
        turbomaster_s4_mhz = value;
        turbomaster_check_4Mhz_mode();
    }
    return 0;
}

static int set_turbomaster_rom1_filename(const char *name, void *param)
{
    if (turbomaster_rom1_filename != NULL && name != NULL && strcmp(name, turbomaster_rom1_filename) == 0) {
        return 0;
    }

    if (name != NULL && *name != '\0') {
        if (util_check_filename_access(name) < 0) {
            return -1;
        }
        turbomaster_load_rom(name, 1);
    } else {
        memset(turbomaster_basic1, 0, 0x2000);
        memset(turbomaster_kernal1, 0, 0x2000);
    }

    util_string_set(&turbomaster_rom1_filename, name);
    if (turbomaster_s1_cpu_started && turbomaster_s2_rom == 1) {
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    }

    return 0;
}

static int set_turbomaster_rom2_filename(const char *name, void *param)
{
    if (turbomaster_rom2_filename != NULL && name != NULL && strcmp(name, turbomaster_rom2_filename) == 0) {
        return 0;
    }

    if (name != NULL && *name != '\0') {
        if (util_check_filename_access(name) < 0) {
            return -1;
        }
        turbomaster_load_rom(name, 2);
    } else {
        memset(turbomaster_basic2, 0, 0x2000);
        memset(turbomaster_kernal2, 0, 0x2000);
    }

    util_string_set(&turbomaster_rom2_filename, name);
    if (turbomaster_s1_cpu_started && turbomaster_s2_rom == 2) {
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    }

    return 0;
}

static const resource_int_t resources_int[] = {
    { "TurboMaster", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &turbomaster_enabled, set_turbomaster_enabled, NULL },
    { "TurboMasterCPU", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &turbomaster_s1_cpu_started, set_turbomaster_cpu_started, NULL },
    { "TurboMasterROM", 1, RES_EVENT_STRICT, (resource_value_t)0,
      &turbomaster_s2_rom, set_turbomaster_rom, NULL },
    { "TurboMasterSpeedControl", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &turbomaster_s3_speed, set_turbomaster_speed_control, NULL },
    { "TurboMasterMhz", 4, RES_EVENT_STRICT, (resource_value_t)0,
      &turbomaster_s4_mhz, set_turbomaster_speed_control, NULL },
    { NULL }
};

static const resource_string_t resources_string[] = {
    { "TurboMasterROM1Name", "", RES_EVENT_NO, NULL,
      &turbomaster_rom1_filename, set_turbomaster_rom1_filename, NULL },
    { "TurboMasterROM2Name", "", RES_EVENT_NO, NULL,
      &turbomaster_rom2_filename, set_turbomaster_rom2_filename, NULL },
    { NULL }
};

int turbomaster_resources_init(void)
{
    turbomaster_mem_init();

    if (resources_register_int(resources_int) < 0) {
        return -1;
    }

    return resources_register_string(resources_string);
}

void turbomaster_resources_shutdown(void)
{
    lib_free(turbomaster_rom1_filename);
    lib_free(turbomaster_rom2_filename);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-tm", SET_RESOURCE, 0,
      NULL, NULL, "TurboMaster", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_TURBOMASTER,
      NULL, NULL },
    { "+tm", SET_RESOURCE, 0,
      NULL, NULL, "TurboMaster", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_TURBOMASTER,
      NULL, NULL },
    { "-tmswitchrom1", SET_RESOURCE, 0,
      NULL, NULL, "TurboMasterROM", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_TURBOMASTER_SWITCH_ROM_1,
      NULL, NULL },
    { "-tmswitchrom2", SET_RESOURCE, 0,
      NULL, NULL, "TurboMasterROM", (resource_value_t)2,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_TURBOMASTER_SWITCH_ROM_2,
      NULL, NULL },
    { "-tmswitchspeedsoft", SET_RESOURCE, 0,
      NULL, NULL, "TurboMasterSpeedControl", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_TURBOMASTER_SWITCH_SPEED_SOFT,
      NULL, NULL },
    { "-tmswitchspeedmanual", SET_RESOURCE, 0,
      NULL, NULL, "TurboMasterSpeedControl", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_TURBOMASTER_SWITCH_SPEED_MANUAL,
      NULL, NULL },
    { "-tmswitch4mhz", SET_RESOURCE, 0,
      NULL, NULL, "TurboMasterMhz", (resource_value_t)4,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_TURBOMASTER_SWITCH_4_MHZ,
      NULL, NULL },
    { "-tmswitch1mhz", SET_RESOURCE, 0,
      NULL, NULL, "TurboMasterMhz", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_TURBOMASTER_SWITCH_1_MHZ,
      NULL, NULL },
    { "-tmrom1image", SET_RESOURCE, 1,
      NULL, NULL, "TurboMasterROM1Name", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_TURBOMASTER_ROM1_NAME,
      NULL, NULL },
    { "-tmrom2image", SET_RESOURCE, 1,
      NULL, NULL, "TurboMasterROM2Name", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_TURBOMASTER_ROM2_NAME,
      NULL, NULL },
    { NULL }
};

int turbomaster_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

static BYTE reg_a = 0;
static BYTE reg_x = 0;
static BYTE reg_y = 0;
static BYTE reg_p = 0;
static BYTE reg_sp = 0;
static BYTE flag_n = 0;
static BYTE flag_z = 0;

R65C02_regs_t turbomaster_regs;

static void import_registers(void)
{
    reg_a = turbomaster_regs.a;
    reg_x = turbomaster_regs.x;
    reg_y = turbomaster_regs.y;
    reg_p = turbomaster_regs.p;
    reg_sp = turbomaster_regs.sp;
    flag_n = turbomaster_regs.n;
    flag_z = turbomaster_regs.z;
    reg_pc = turbomaster_regs.pc;
}

static void export_registers(void)
{
    turbomaster_regs.a = reg_a;
    turbomaster_regs.x = reg_x;
    turbomaster_regs.y = reg_y;
    turbomaster_regs.p = reg_p;
    turbomaster_regs.sp = reg_sp;
    turbomaster_regs.n = flag_n;
    turbomaster_regs.z = flag_z;
    turbomaster_regs.pc = reg_pc;
}

void turbomaster_reset(void)
{
    turbomaster_bank_config = 7;
    turbomaster_cpu_port0 = 0;
    turbomaster_cpu_port1 = 0xff;
    softmode_speed = 4;
    turbomaster_check_4Mhz_mode();
    reg_pc = LOAD_ADDR(0xfffc);
    turbomaster_regs.pc = reg_pc;
}

/* ------------------------------------------------------------------------- */

static CLOCK turbomaster_clk_limit = 0L;

void turbomaster_mainloop(interrupt_cpu_status_t *cpu_int_status, alarm_context_t *cpu_alarm_context)
{
    /* Notice that using a struct for these would make it a lot slower (at
       least, on gcc 2.7.2.x).  */
    BYTE *bank_base = NULL;
    WORD tmp_address;
    int bank_start = 0;
    int bank_limit = -1;
    int cpu_type = CPU_R65C02;

    import_registers();

    while (turbomaster_s1_cpu_started) {
#define CLK maincpu_clk
#define RMW_FLAG maincpu_rmw_flag
#define LAST_OPCODE_ADDR last_opcode_addr
#define TRACEFLG debug.maincpu_traceflg

#define CPU_INT_STATUS cpu_int_status

#define ALARM_CONTEXT cpu_alarm_context

#define CHECK_PENDING_ALARM() (clk >= next_alarm_clk(cpu_int_status))

#define CHECK_PENDING_INTERRUPT() check_pending_interrupt(cpu_int_status)

#define TRAP(addr) maincpu_int_status->trap_func(addr);

#define ROM_TRAP_HANDLER() traps_handler()

#define JAM()                                                         \
    do {                                                              \
        unsigned int tmp;                                             \
                                                                      \
        EXPORT_REGISTERS();                                           \
        tmp = machine_jam("   " CPU_STR ": JAM at $%04X   ", reg_pc); \
        switch (tmp) {                                                \
            case JAM_RESET:                                           \
                DO_INTERRUPT(IK_RESET);                               \
                break;                                                \
            case JAM_HARD_RESET:                                      \
                mem_powerup();                                        \
                DO_INTERRUPT(IK_RESET);                               \
                break;                                                \
            case JAM_MONITOR:                                         \
                monitor_startup(e_comp_space);                        \
                IMPORT_REGISTERS();                                   \
                break;                                                \
            default:                                                  \
                CLK++;                                                \
        }                                                             \
    } while (0)

#define CALLER e_comp_space

#define ROM_TRAP_ALLOWED() mem_rom_trap_allowed((WORD)reg_pc)

#define GLOBAL_REGS turbomaster_regs

#include "65c02core.c"

        cpu_int_status->num_dma_per_opcode = 0;

        if (turbomaster_clk_limit && (maincpu_clk > turbomaster_clk_limit)) {
            log_error(LOG_DEFAULT, "cycle limit reached.");
            exit(EXIT_FAILURE);
        }
#if 0
        if (CLK > 246171754) {
            debug.maincpu_traceflg = 1;
        }
#endif
    }
    export_registers();
}

/* ------------------------------------------------------------------------- */

void turbomaster_check_and_run_65c02(interrupt_cpu_status_t *cpu_int_status, alarm_context_t *cpu_alarm_context)
{
    if (turbomaster_s1_cpu_started) {
        turbomaster_mainloop(cpu_int_status, cpu_alarm_context);
    }
}
