/*
 * mon_registerz80.c - The VICE built-in monitor Z80 register functions.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include "uimon.h"
#include "z80regs.h"

static unsigned int mon_register_get_val(int mem, int reg_id)
{
    z80_regs_t *reg_ptr;

    if (mem == e_disk8_space) {
        if (!check_drive_emu_level_ok(8))
            return 0;
    }
    if (mem == e_disk9_space) {
        if (!check_drive_emu_level_ok(9))
            return 0;
    }

    reg_ptr = mon_interfaces[mem]->z80_cpu_regs;

    switch(reg_id) {
      case e_AF:
        return Z80_REGS_GET_AF(reg_ptr);
      case e_BC:
        return Z80_REGS_GET_BC(reg_ptr);
      case e_DE:
        return Z80_REGS_GET_DE(reg_ptr);
      case e_HL:
        return Z80_REGS_GET_HL(reg_ptr);
      case e_IX:
        return Z80_REGS_GET_IX(reg_ptr);
      case e_IY:
        return Z80_REGS_GET_IY(reg_ptr);
      case e_SP:
        return Z80_REGS_GET_SP(reg_ptr);
      case e_PC:
        return Z80_REGS_GET_PC(reg_ptr);
      case e_I:
        return Z80_REGS_GET_I(reg_ptr);
      case e_R:
        return Z80_REGS_GET_R(reg_ptr);
      case e_AF2:
        return Z80_REGS_GET_AF2(reg_ptr);
      case e_BC2:
        return Z80_REGS_GET_BC2(reg_ptr);
      case e_DE2:
        return Z80_REGS_GET_DE2(reg_ptr);
      case e_HL2:
        return Z80_REGS_GET_HL2(reg_ptr);
      default:
        log_error(LOG_ERR, "Unknown register!");
    }
    return 0;
}

static void mon_register_set_val(int mem, int reg_id, WORD val)
{
    z80_regs_t *reg_ptr;

    if (mem == e_disk8_space) {
        if (!check_drive_emu_level_ok(8))
            return;
    }
    if (mem == e_disk9_space) {
        if (!check_drive_emu_level_ok(9))
            return;
    }

    reg_ptr = mon_interfaces[mem]->z80_cpu_regs;

    switch(reg_id) {
      case e_AF:
        Z80_REGS_SET_AF(reg_ptr, val);
        break;
        Z80_REGS_SET_BC(reg_ptr, val);
        break;
        Z80_REGS_SET_DE(reg_ptr, val);
        break;
        Z80_REGS_SET_HL(reg_ptr, val);
        break;
        Z80_REGS_SET_IX(reg_ptr, val);
        break;
        Z80_REGS_SET_IY(reg_ptr, val);
        break;
        Z80_REGS_SET_SP(reg_ptr, val);
        break;
        Z80_REGS_SET_PC(reg_ptr, val);
        break;
        Z80_REGS_SET_I(reg_ptr, val);
        break;
        Z80_REGS_SET_R(reg_ptr, val);
        break;
        Z80_REGS_SET_AF2(reg_ptr, val);
        break;
        Z80_REGS_SET_BC2(reg_ptr, val);
        break;
        Z80_REGS_SET_DE2(reg_ptr, val);
        break;
        Z80_REGS_SET_HL2(reg_ptr, val);
        break;
      default:
        log_error(LOG_ERR, "Unknown register!");
        return;
    }
    force_array[mem] = 1;
}

static void mon_register_print(int mem)
{
    z80_regs_t *regs;

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
    regs = mon_interfaces[mem]->z80_cpu_regs;

    uimon_out("  ADDR AF   BC   DE   HL   IX   IY   SP   I  R  AF'  BC'  DE'  HL'\n");
    uimon_out(".;%04x %04x %04x %04x %04x %04x %04x %04x %02x %02x %04x %04x %04x %04x\n",
              mon_register_get_val(mem, e_PC),
              mon_register_get_val(mem, e_AF),
              mon_register_get_val(mem, e_BC),
              mon_register_get_val(mem, e_DE),
              mon_register_get_val(mem, e_HL),
              mon_register_get_val(mem, e_IX),
              mon_register_get_val(mem, e_IY),
              mon_register_get_val(mem, e_SP),
              mon_register_get_val(mem, e_I),
              mon_register_get_val(mem, e_R),
              mon_register_get_val(mem, e_AF2),
              mon_register_get_val(mem, e_BC2),
              mon_register_get_val(mem, e_DE2),
              mon_register_get_val(mem, e_HL2));
}

void mon_registerz80_init(monitor_cpu_type_t *monitor_cpu_type)
{
    monitor_cpu_type->mon_register_get_val = mon_register_get_val;
    monitor_cpu_type->mon_register_set_val = mon_register_set_val;
    monitor_cpu_type->mon_register_print = mon_register_print;
}

