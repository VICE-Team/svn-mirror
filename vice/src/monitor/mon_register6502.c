/*
 * mon_register6502.c - The VICE built-in monitor 6502 register functions.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Daniel Sladic <sladic@eecg.toronto.edu>
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

#include "asm.h"
#include "log.h"
#include "mon.h"
#include "mos6510.h"
#include "uimon.h"

#define TEST(x) ((x)!=0)

static unsigned int mon_register_get_val(int mem, int reg_id)
{
    mos6510_regs_t *reg_ptr = NULL;

    if (mem == e_disk8_space) {
        if (!check_drive_emu_level_ok(8))
            return 0;
    }
    if (mem == e_disk9_space) {
        if (!check_drive_emu_level_ok(9))
            return 0;
    }

    reg_ptr = mon_interfaces[mem]->cpu_regs;

    switch(reg_id) {
      case e_A:
        return MOS6510_REGS_GET_A(reg_ptr);
      case e_X:
        return MOS6510_REGS_GET_X(reg_ptr);
      case e_Y:
        return MOS6510_REGS_GET_Y(reg_ptr);
      case e_PC:
        return MOS6510_REGS_GET_PC(reg_ptr);
      case e_SP:
        return MOS6510_REGS_GET_SP(reg_ptr);
      case e_FLAGS:
          return MOS6510_REGS_GET_FLAGS(reg_ptr)|
              MOS6510_REGS_GET_SIGN(reg_ptr)|
              MOS6510_REGS_GET_ZERO(reg_ptr)<<1;
      default:
        log_error(LOG_ERR, "Unknown register!");
    }
    return 0;
}

static void mon_register_set_val(int mem, int reg_id, WORD val)
{
    mos6510_regs_t *reg_ptr = NULL;

    if (mem == e_disk8_space) {
        if (!check_drive_emu_level_ok(8))
            return;
    }
    if (mem == e_disk9_space) {
        if (!check_drive_emu_level_ok(9))
            return;
    }

    reg_ptr = mon_interfaces[mem]->cpu_regs;

    switch(reg_id) {
      case e_A:
        MOS6510_REGS_SET_A(reg_ptr, val);
        break;
      case e_X:
        MOS6510_REGS_SET_X(reg_ptr, val);
        break;
      case e_Y:
        MOS6510_REGS_SET_Y(reg_ptr, val);
        break;
      case e_PC:
        MOS6510_REGS_SET_PC(reg_ptr, val);
        if (mem == e_disk8_space)
            mon_interfaces[mem]->set_bank_base();
        if (mem == e_disk9_space)
            mon_interfaces[mem]->set_bank_base();
        break;
      case e_SP:
        MOS6510_REGS_SET_SP(reg_ptr, val);
        break;
      default:
        log_error(LOG_ERR, "Unknown register!");
        return;
    }
    force_array[mem] = 1;
}

static void mon_register_print(int mem)
{
    mos6510_regs_t *regs;

    if (mem == e_disk8_space) {
        if (!check_drive_emu_level_ok(8))
            return;
    } else if (mem == e_disk9_space) {
        if (!check_drive_emu_level_ok(9))
            return;
    } else if (mem != e_comp_space) {
        log_error(LOG_ERR, "Unknown memory space!");
        return;
    }
    regs = mon_interfaces[mem]->cpu_regs;
    uimon_out("  ADDR AC XR YR SP 01 NV-BDIZC\n");
    uimon_out(".;%04x %02x %02x %02x %02x %02x %d%d%c%d%d%d%d%d\n",
              mon_register_get_val(mem, e_PC),
              mon_register_get_val(mem, e_A),
              mon_register_get_val(mem, e_X),
              mon_register_get_val(mem, e_Y),
              mon_register_get_val(mem, e_SP),
              mon_register_get_val(mem, 1),
              TEST(MOS6510_REGS_GET_SIGN(regs)),
              TEST(MOS6510_REGS_GET_OVERFLOW(regs)),
              '1',
              TEST(MOS6510_REGS_GET_BREAK(regs)),
              TEST(MOS6510_REGS_GET_DECIMAL(regs)),
              TEST(MOS6510_REGS_GET_INTERRUPT(regs)),
              TEST(MOS6510_REGS_GET_ZERO(regs)),
              TEST(MOS6510_REGS_GET_CARRY(regs)));
}

void mon_register6502_init(monitor_cpu_type_t *monitor_cpu_type)
{
    monitor_cpu_type->mon_register_get_val = mon_register_get_val;
    monitor_cpu_type->mon_register_set_val = mon_register_set_val;
    monitor_cpu_type->mon_register_print = mon_register_print;
}

