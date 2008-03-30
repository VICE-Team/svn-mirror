/*
 * c64io.c - C64 io handling ($DE00-$DFFF).
 *
 * Written by
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

#include <string.h>

#include "c64-resources.h"
#include "c64_256k.h"
#include "c64acia.h"
#include "c64cart.h"
#include "c64io.h"
#include "cartridge.h"
#include "emuid.h"
#include "lib.h"
#include "monitor.h"
#include "reu.h"
#include "georam.h"
#include "ramcart.h"
#include "resources.h"
#include "sid-resources.h"
#include "sid.h"
#include "types.h"
#include "ui.h"
#include "util.h"
#include "vicii-phi1.h"

#ifdef HAVE_TFE
#include "tfe.h"
#endif

struct io_source_s {
    int id;
    char *name;
    int detach_id;
    char *resource_name;
};
typedef struct io_source_s io_source_t;

static void io_source_detach(int detach_id, char *resource_name)
{
  switch (detach_id)
  {
    case IO_DETACH_CART:
      cartridge_detach_image();
      break;
    case IO_DETACH_RESOURCE:
      resources_set_value(resource_name, (resource_value_t)0);
      break;
  }
}

static io_source_t io_source_table[] = {
    {IO_SOURCE_NONE, "NONE", 0, NULL},
    {IO_SOURCE_ACIA, "ACIA/SWIFTLINK/TURBO232", IO_DETACH_RESOURCE, "Acia1Enable"},
    {IO_SOURCE_C64_256K, "C64 256K", IO_DETACH_RESOURCE, "C64_256K"},
    {IO_SOURCE_GEORAM, "GEORAM", IO_DETACH_RESOURCE, "GEORAM"},
    {IO_SOURCE_RAMCART, "RAMCART", IO_DETACH_RESOURCE, "RAMCART"},
    {IO_SOURCE_REU, "REU", IO_DETACH_RESOURCE, "REU"},
    {IO_SOURCE_TFE_RR_NET, "ETHERNET CART", IO_DETACH_RESOURCE, "ETHERNET_ACTIVE"},
    {IO_SOURCE_STEREO_SID, "STEREO SID", IO_DETACH_RESOURCE, "SidStereo"},
    {IO_SOURCE_ACTION_REPLAY, "ACTION_REPLAY", IO_DETACH_CART, NULL},
    {IO_SOURCE_ATOMIC_POWER, "ATOMIC POWER", IO_DETACH_CART, NULL},
    {IO_SOURCE_EPYX_FASTLOAD, "EPYX FASTLOAD", IO_DETACH_CART, NULL},
    {IO_SOURCE_FINAL1, "FINAL I", IO_DETACH_CART, NULL},
    {IO_SOURCE_FINAL3, "FINAL III", IO_DETACH_CART, NULL},
    {IO_SOURCE_IDE64, "IDE64", IO_DETACH_CART, NULL},
    {IO_SOURCE_IEEE488, "IEEE488", IO_DETACH_CART, NULL},
    {IO_SOURCE_KCS, "KCS POWER", IO_DETACH_CART, NULL},
    {IO_SOURCE_MAGIC_FORMEL, "MAGIC FORMEL", IO_DETACH_CART, NULL},
    {IO_SOURCE_RR, "RETRO REPLAY", IO_DETACH_CART, NULL},
    {IO_SOURCE_SS4, "SUPER SNAPSHOT 4", IO_DETACH_CART, NULL},
    {IO_SOURCE_SS5, "SUPER SNAPSHOT 5", IO_DETACH_CART, NULL},
    {IO_SOURCE_WARPSPEED, "WARPSPEED", IO_DETACH_CART, NULL},
    {IO_SOURCE_EMUID, "EMU ID", IO_DETACH_RESOURCE, "EmuID"},
    {-1,NULL,0,NULL}
};

int io_source;

static char *get_io_source_name(int id)
{
  int i=0;

  while (io_source_table[i].id!=-1)
  {
    if (io_source_table[i].id==id)
      return io_source_table[i].name;
    i++;
  }
  return "Unknown";
}

static int get_io_source_index(int id)
{
  int i=0;

  while (io_source_table[i].id!=-1)
  {
    if (io_source_table[i].id==id)
      return i;
    i++;
  }
  return 0;
}

#define MAX_IO1_RETURNS 7
#define MAX_IO2_RETURNS 8

#if MAX_IO1_RETURNS>MAX_IO2_RETURNS
static int io_source_return[MAX_IO1_RETURNS];
#else
static int io_source_return[MAX_IO2_RETURNS];
#endif

static BYTE real_return_value;
static BYTE return_value;
static int returned;
static int io_source_start;
static int io_source_end;

static void io_source_check(int counter)
{
    if (io_source!=0)
    {
        returned++;
        real_return_value=return_value;
        io_source_return[counter]=io_source;
        io_source=0;
        if (io_source_start==-1)
          io_source_start=counter;
        io_source_end=counter;
    }
}

static void io_source_msg_detach(int addr)
{
    char *old_msg=NULL;
    char *new_msg=NULL;
    int i;
    int index;

    for (i=io_source_start; i<io_source_end+1; i++)
    {
        if (io_source_return[i]!=0)
        {
            if (i==io_source_start)
            {
                old_msg=strdup("I/O read collision at %X from ");
                new_msg=util_concat(old_msg,get_io_source_name(io_source_return[i]),NULL);
                lib_free(old_msg);
            }
            if (i>io_source_start && i<io_source_end)
            {
                old_msg=new_msg;
                new_msg=util_concat(old_msg,", ",get_io_source_name(io_source_return[i]),NULL);
                lib_free(old_msg);
            }
            if (i==io_source_end)
            {
                old_msg=new_msg;
                new_msg=util_concat(old_msg," and ",get_io_source_name(io_source_return[i]),
                                    ".\nAll the named devices will be detached",NULL);
                lib_free(old_msg);
            }
        }
    }
    ui_error(new_msg,addr);
    lib_free(new_msg);

    for (i=io_source_start; i<io_source_end+1; i++)
    {
        if (io_source_return[i]!=0)
        {
            index=get_io_source_index(io_source_return[i]);
            io_source_detach(io_source_table[index].detach_id,io_source_table[index].resource_name);
        }
    }
}

BYTE REGPARM1 io1_read(WORD addr)
{
    int io_source_counter=0;

    memset(io_source_return,0,sizeof(io_source_return));
    returned=0;
    io_source_start=-1;
    io_source_end=-1;

    if (sid_stereo
        && addr >= sid_stereo_address_start
        && addr < sid_stereo_address_end)
    {
        return_value=sid2_read(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }

    if (c64_256k_enabled && addr>=c64_256k_start && addr<=c64_256k_start+0x7f)
    {
        return_value=c64_256k_read((WORD)(addr & 0x03));
        io_source_check(io_source_counter);
        io_source_counter++;
    }

    if (georam_enabled)
    {
        return_value=georam_window_read((WORD)(addr & 0xff));
        io_source_check(io_source_counter);
        io_source_counter++;
    }

    if (ramcart_enabled)
    {
        return_value=ramcart_reg_read((WORD)(addr & 1));
        io_source_check(io_source_counter);
        io_source_counter++;
    }

#ifdef HAVE_TFE
    if (tfe_enabled)
    {
        if ((tfe_as_rr_net && addr<0xde10) || !tfe_as_rr_net)

        return_value=tfe_read((WORD)(addr & 0x0f));
        io_source_check(io_source_counter);
        io_source_counter++;
    }
#endif

    if (mem_cartridge_type != CARTRIDGE_NONE)
    {
        return_value=cartridge_read_io1(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }

#ifdef HAVE_RS232
    if (acia_de_enabled)
    {
        return_value=acia1_read((WORD)(addr & 0x07));
        io_source_check(io_source_counter);
        io_source_counter++;
    }
#endif

    if (returned==0)
      return vicii_read_phi1();

    if (returned==1)
      return real_return_value;

    io_source_msg_detach(addr);

    return vicii_read_phi1();
}

void REGPARM2 io1_store(WORD addr, BYTE value)
{
    if (sid_stereo
        && addr >= sid_stereo_address_start
        && addr < sid_stereo_address_end)
        sid2_store(addr, value);
    if (c64_256k_enabled && addr>=c64_256k_start && addr<=c64_256k_start+0x7f) {
        c64_256k_store((WORD)(addr & 0x03), value);
    }
    if (georam_enabled) {
        georam_window_store((WORD)(addr & 0xff), value);
    }
    if (ramcart_enabled) {
        ramcart_reg_store((WORD)(addr&1), value);
    }
#ifdef HAVE_TFE
    if (tfe_enabled)
        tfe_store((WORD)(addr & 0x0f), value);
#endif
    if (mem_cartridge_type != CARTRIDGE_NONE)
        cartridge_store_io1(addr, value);
#ifdef HAVE_RS232
    if (acia_de_enabled)
        acia1_store((WORD)(addr & 0x07), value);
#endif
    return;
}

BYTE REGPARM1 io2_read(WORD addr)
{
    int io_source_counter=0;

    memset(io_source_return,0,sizeof(io_source_return));
    returned=0;
    io_source_start=-1;
    io_source_end=-1;

    if (sid_stereo
        && addr >= sid_stereo_address_start
        && addr < sid_stereo_address_end)
    {
        return_value=sid2_read(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (c64_256k_enabled && addr>=c64_256k_start && addr<=c64_256k_start+0x7f)
    {
        return_value=c64_256k_read((WORD)(addr & 0x03));
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (ramcart_enabled)
    {
        return_value=ramcart_window_read(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (mem_cartridge_type == CARTRIDGE_RETRO_REPLAY)
    {
        return_value=cartridge_read_io2(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (reu_enabled)
    {
        return_value=reu_read((WORD)(addr & 0x0f));
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (georam_enabled && addr>=0xdf80)
    {
        return georam_reg_read((WORD)(addr & 1));
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (mem_cartridge_type != CARTRIDGE_NONE && mem_cartridge_type != CARTRIDGE_RETRO_REPLAY)
    {
        return_value=cartridge_read_io2(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (emu_id_enabled && addr >= 0xdfa0)
    {
        return_value=emuid_read((WORD)(addr - 0xdfa0));
        io_source=IO_SOURCE_EMUID;
        io_source_check(io_source_counter);
        io_source_counter++;
    }

    if (returned==0)
      return vicii_read_phi1();

    if (returned==1)
      return real_return_value;

    io_source_msg_detach(addr);

    return vicii_read_phi1();
}

void REGPARM2 io2_store(WORD addr, BYTE value)
{
    if (sid_stereo
        && addr >= sid_stereo_address_start
        && addr < sid_stereo_address_end)
        sid2_store(addr, value);
    if (mem_cartridge_type == CARTRIDGE_RETRO_REPLAY) {
        cartridge_store_io2(addr, value);
    }
    if (c64_256k_enabled && addr>=c64_256k_start && addr<=c64_256k_start+0x7f) {
        c64_256k_store((WORD)(addr & 0x03), value);
    }
    if (reu_enabled) {
        reu_store((WORD)(addr & 0x0f), value);
    }
    if (georam_enabled && addr>=0xdf80) {
        georam_reg_store((WORD)(addr & 1), value);
    }
    if (ramcart_enabled) {
        ramcart_window_store(addr, value);
    }
    if (mem_cartridge_type != CARTRIDGE_NONE) {
        cartridge_store_io2(addr, value);
    }
    return;
}

void c64io_ioreg_add_list(struct mem_ioreg_list_s **mem_ioreg_list)
{
    if (reu_enabled)
        mon_ioreg_add_list(mem_ioreg_list, "REU", 0xdf00, 0xdf0f);
    if (georam_enabled)
    {
        mon_ioreg_add_list(mem_ioreg_list, "GEORAM", 0xde00, 0xdeff);
        mon_ioreg_add_list(mem_ioreg_list, "GEORAM", 0xdffe, 0xdfff);
    }
    if (ramcart_enabled)
    {
        mon_ioreg_add_list(mem_ioreg_list, "RAMCART", 0xde00, 0xde01);
        mon_ioreg_add_list(mem_ioreg_list, "RAMCART", 0xdf00, 0xdfff);
    }
    if (c64_256k_enabled && c64_256k_start==0xde00)
        mon_ioreg_add_list(mem_ioreg_list, "C64_256K", 0xde00, 0xde7f);
    if (c64_256k_enabled && c64_256k_start==0xde80)
        mon_ioreg_add_list(mem_ioreg_list, "C64_256K", 0xde80, 0xdeff);
    if (c64_256k_enabled && c64_256k_start==0xdf00)
        mon_ioreg_add_list(mem_ioreg_list, "C64_256K", 0xdf00, 0xdf7f);
    if (c64_256k_enabled && c64_256k_start==0xdf80)
        mon_ioreg_add_list(mem_ioreg_list, "C64_256K", 0xdf80, 0xdfff);

#ifdef HAVE_TFE
    if (tfe_enabled)
        mon_ioreg_add_list(mem_ioreg_list, "TFE", 0xde00, 0xde0f);
#endif
}
