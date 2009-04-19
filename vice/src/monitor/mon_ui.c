/*
 * mon_ui.c - Monitor user interface functions.
 *
 * Written by
 *  Spiro Trikaliotis <Spiro.Trikaliotis@gmx.de>
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
#include <string.h>

#include "lib.h"
#include "mon_breakpoint.h"
#include "mon_memory.h"
#include "mon_register.h"
#include "mon_ui.h"
#include "mon_util.h"
#include "monitor.h"
#include "montypes.h"
#include "resources.h"


void mon_disassembly_init(mon_disassembly_private_t * pmdp)
{
    pmdp->memspace = e_comp_space;
    pmdp->StartAddress = -1;
    pmdp->EndAddress = 0;
    pmdp->CurrentAddress = 0;
    pmdp->have_label = 0;

    mon_disassembly_goto_pc(pmdp);
}

static
void mon_disassembly_check_if_in_range(mon_disassembly_private_t *pmdp)
{
    if ((pmdp->CurrentAddress < pmdp->StartAddress)
        || (pmdp->CurrentAddress > pmdp->EndAddress)) {
        pmdp->StartAddress = pmdp->CurrentAddress;
        pmdp->EndAddress = 0;
    }
}

void mon_disassembly_update(mon_disassembly_private_t *pmdp)
{
    mon_disassembly_goto_pc(pmdp);
}

void mon_disassembly_set_memspace(mon_disassembly_private_t *pmdp,
                                  MEMSPACE memspace)
{
    pmdp->memspace = memspace;
}

MEMSPACE mon_disassembly_get_memspace(mon_disassembly_private_t *pmdp)
{
    return pmdp->memspace;
}


mon_disassembly_t * mon_disassembly_get_lines(
    mon_disassembly_private_t *pmdp, int lines_visible,
    int lines_full_visible)
{
    WORD loc;
    unsigned int size;
    int  i;
    unsigned int  have_label = pmdp->have_label;
    mon_disassembly_t *contents = NULL;
    mon_disassembly_t *ret;

    loc = pmdp->StartAddress;
    ret = NULL;

    pmdp->Lines = lines_full_visible;

    for (i = 0; i < lines_visible; i++ ) {
        mon_disassembly_t *newcont;
        mon_breakpoint_type_t bptype;

        newcont = lib_malloc(sizeof * newcont);

        if (ret == NULL) {
            ret      =
            contents = newcont;
        } else {
            contents = contents->next = newcont;
        }

        contents->next = NULL;
        contents->flags.active_line = loc == pmdp->CurrentAddress ? 1 : 0;

        /* determine type of breakpoint */
        bptype = mon_breakpoint_is(new_addr(pmdp->memspace, loc));

        contents->flags.is_breakpoint = bptype != BP_NONE;
        contents->flags.breakpoint_active = bptype == BP_ACTIVE;

        contents->content =
            mon_disassemble_with_label(pmdp->memspace, loc, 1, 
            &size, &have_label);

        contents->length  = strlen(contents->content);

        pmdp->EndAddress = loc;

        loc += size;
    }

    return ret;
}

static
WORD determine_address_of_line(mon_disassembly_private_t *pmdp, 
                               WORD loc, int line )
{
    unsigned int size;
    int  i;
    unsigned int have_label = pmdp->have_label;

    /* it's one less than visible, so there will be one line visible left! */
    for (i = 0; i < line; i++) {
        char *content;

        content = mon_disassemble_with_label(pmdp->memspace, loc, 1, &size,
                                             &have_label);

        lib_free(content);

        loc += size;
    }

    return loc;
}

static
WORD scroll_down(mon_disassembly_private_t *pmdp, WORD loc)
{
    return determine_address_of_line(pmdp, loc, 1);
}

static
WORD scroll_down_page(mon_disassembly_private_t *pmdp, WORD loc)
{
    /* the count is one less than visible,
       so there will be one visible line left on the screen! */
    return determine_address_of_line( pmdp, loc, pmdp->Lines - 1 );
}

static
WORD scroll_up_count(mon_disassembly_private_t *pmdp, WORD loc,
                        unsigned int count)
{
    unsigned int size;
    /* this has to be initialized with zero for correct processing */
    unsigned int have_label = 0;

    WORD testloc = loc - 3 * count - 3;

    unsigned int *disp = lib_malloc(sizeof(unsigned int) * count);
    unsigned int storepos = 0;

    while (testloc < loc) {
        char *content;

        disp[storepos++] = loc - testloc;
        if (storepos == count)
            storepos = 0;

	    content = mon_disassemble_with_label(pmdp->memspace, testloc, 1,
                                                 &size, &have_label );

        lib_free(content);
        testloc += size;
    }

    loc -= disp[storepos];

    lib_free(disp);

    return loc;
}

static
WORD scroll_up(mon_disassembly_private_t *pmdp, WORD loc)
{
    return scroll_up_count( pmdp, loc, 1 );
}

static
WORD scroll_up_page(mon_disassembly_private_t *pmdp, WORD loc)
{
    /* the count is one less than visible,
       so there will be one visible line left on the screen! */
    return scroll_up_count(pmdp, loc, pmdp->Lines - 1);
}

WORD mon_disassembly_scroll(mon_disassembly_private_t *pmdp, 
                            MON_SCROLL_TYPE ScrollType)
{
    switch (ScrollType) {
      case MON_SCROLL_NOTHING:
        break;

      case MON_SCROLL_DOWN:
        pmdp->StartAddress = scroll_down(pmdp, pmdp->StartAddress);
        break;

      case MON_SCROLL_UP:
        pmdp->StartAddress = scroll_up(pmdp, pmdp->StartAddress);
        break;

      case MON_SCROLL_PAGE_DOWN:
        pmdp->StartAddress = scroll_down_page(pmdp, pmdp->StartAddress);
        break;

      case MON_SCROLL_PAGE_UP:
        pmdp->StartAddress = scroll_up_page(pmdp, pmdp->StartAddress);
        break;
    }
    return pmdp->StartAddress;
}

WORD mon_disassembly_scroll_to(mon_disassembly_private_t *pmdp, 
                               WORD addr)
{
    pmdp->StartAddress = addr;
    return pmdp->StartAddress;
}

void mon_disassembly_set_breakpoint(mon_disassembly_private_t *pmdp)
{
    mon_breakpoint_set(pmdp->AddrClicked);
}

void mon_disassembly_unset_breakpoint(mon_disassembly_private_t *pmdp)
{
    mon_breakpoint_unset(pmdp->AddrClicked);
}

void mon_disassembly_enable_breakpoint(mon_disassembly_private_t *pmdp)
{
    mon_breakpoint_enable(pmdp->AddrClicked);
}

void mon_disassembly_disable_breakpoint(mon_disassembly_private_t *pmdp)
{
    mon_breakpoint_disable(pmdp->AddrClicked);
}

void mon_disassembly_goto_address(mon_disassembly_private_t *pmdp,
                                  WORD addr)
{
    pmdp->CurrentAddress = addr;
    mon_disassembly_check_if_in_range(pmdp);
}

void mon_disassembly_goto_pc(mon_disassembly_private_t *pmdp)
{
    mon_disassembly_goto_address(pmdp, 
        (WORD)(monitor_cpu_for_memspace[pmdp->memspace]->mon_register_get_val(pmdp->memspace, e_PC)));
}

void mon_disassembly_set_next_instruction(mon_disassembly_private_t *pmdp)
{
    monitor_cpu_for_memspace[pmdp->memspace]->mon_register_set_val(pmdp->memspace, e_PC, (WORD) addr_location(pmdp->AddrClicked));
}

void mon_disassembly_goto_string(mon_disassembly_private_t * pmdp, char *addr)
{
    unsigned long address;
    char * remain;

    address = strtoul(addr, &remain, 16);

    if (*remain == 0) {
        mon_disassembly_goto_address(pmdp, (WORD) address);
    }
}


void mon_disassembly_determine_popup_commands(
                                   mon_disassembly_private_t *pmdp, 
                                   int xPos, int yPos, WORD *ulMask,
                                   WORD *ulDefault)
{
    MON_ADDR CurrentAddress;
    mon_breakpoint_type_t mbt;

    int drive_true_emulation;

    resources_get_int("DriveTrueEmulation", &drive_true_emulation);

    CurrentAddress = new_addr(pmdp->memspace, determine_address_of_line(pmdp,
                              pmdp->StartAddress, yPos));
    mbt = mon_breakpoint_is(CurrentAddress);

    /* remember values to be re-used when command is executed */
    pmdp->AddrClicked = CurrentAddress;

    switch (mbt) {
      case BP_ACTIVE:
        *ulMask = MDDPC_UNSET_BREAKPOINT | MDDPC_DISABLE_BREAKPOINT;
        *ulDefault = MDDPC_UNSET_BREAKPOINT;
        break;

      case BP_INACTIVE:
        *ulMask = MDDPC_SET_BREAKPOINT | MDDPC_UNSET_BREAKPOINT
                  | MDDPC_ENABLE_BREAKPOINT;
        *ulDefault = MDDPC_SET_BREAKPOINT;
        break;

      case BP_NONE:
        *ulMask = MDDPC_SET_BREAKPOINT;
        *ulDefault = MDDPC_SET_BREAKPOINT;
        break;
    }

    if (drive_true_emulation) {
        *ulMask |= MDDPC_SET_COMPUTER | MDDPC_SET_DRIVE8 | MDDPC_SET_DRIVE9 | MDDPC_SET_DRIVE10 | MDDPC_SET_DRIVE11;

        switch (pmdp->memspace) {
          case e_comp_space:
            *ulMask &= ~ MDDPC_SET_COMPUTER;
            break;
          case e_disk8_space:
            *ulMask &= ~ MDDPC_SET_DRIVE8;
            break;
          case e_disk9_space:
            *ulMask &= ~ MDDPC_SET_DRIVE9;
            break;
          case e_disk10_space:
            *ulMask &= ~ MDDPC_SET_DRIVE10;
            break;
          case e_disk11_space:
            *ulMask &= ~ MDDPC_SET_DRIVE11;
            break;
          case e_default_space:
          case e_invalid_space:
            break;
        }
    }
}

void mon_ui_init(void)
{
}

void mon_memory_init(mon_memory_private_t * pmmp)
{
    pmmp->memspace = e_comp_space;
    pmmp->StartAddress = 0;
    pmmp->EndAddress = 0;
    pmmp->CurrentAddress = 0;
    pmmp->have_label = 0;
}

void mon_memory_deinit(mon_memory_private_t * pmmp)
{
}

void mon_memory_update(mon_memory_private_t * pmmp)
{
}

mon_memory_t *mon_memory_get_lines(mon_memory_private_t * pmmp, int lines_visible, int lines_full_visible)
{
    WORD loc;
    unsigned int size;
    int  i;
    unsigned int  have_label = pmmp->have_label;
    mon_memory_t *contents = NULL;
    mon_memory_t *ret;

    loc = pmmp->StartAddress;
    ret = NULL;

    pmmp->Lines = lines_full_visible;

    for (i = 0; i < lines_visible; i++ ) {
        mon_memory_t *newcont;

        newcont = lib_malloc(sizeof * newcont);

        if (ret == NULL) {
            ret      =
            contents = newcont;
        } else {
            contents = contents->next = newcont;
        }

        contents->next = NULL;
        contents->flags.active_line = 0;
        contents->flags.is_breakpoint = 0;
        contents->flags.breakpoint_active = 0;

        contents->content = lib_stralloc(">C:a0e0  54 4f d0 4f  ce 57 41 49   TO.O.WAI");
        size += 8;

        contents->length  = strlen(contents->content);

        pmmp->EndAddress = loc;

        loc += size;
    }

    return ret;
}
