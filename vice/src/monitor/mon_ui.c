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

#include "mon.h"
#include "montypes.h"
#include "mon_breakpoint.h"
#include "mon_register.h"
#include "mon_ui.h"
#include "mon_util.h"
#include "resources.h"
#include "utils.h"


struct mon_disassembly_private
{
    MEMSPACE memspace;
    WORD StartAddress;
    WORD EndAddress;
    WORD CurrentAddress;
    int have_label;
    int Lines;
    MON_ADDR AddrClicked;
};

struct mon_disassembly_private *mon_disassembly_init(void)
{
    struct mon_disassembly_private *pmdp
        = xmalloc(sizeof(struct mon_disassembly_private));

    pmdp->memspace = e_comp_space;
    pmdp->StartAddress = -1;
    pmdp->EndAddress = 0;
    pmdp->CurrentAddress = 0;
    pmdp->have_label = 0;

    mon_disassembly_goto_pc(pmdp);

    return pmdp;
}

void mon_disassembly_deinit(struct mon_disassembly_private *pmdp)
{
    free(pmdp);
}

static
void mon_disassembly_check_if_in_range(struct mon_disassembly_private *pmdp)
{
    if ((pmdp->CurrentAddress < pmdp->StartAddress)
        || (pmdp->CurrentAddress > pmdp->EndAddress)) {
        pmdp->StartAddress = pmdp->CurrentAddress;
        pmdp->EndAddress = 0;
    }
}

void mon_disassembly_update(struct mon_disassembly_private *pmdp)
{
    mon_disassembly_goto_pc(pmdp);
}

void mon_disassembly_set_memspace(struct mon_disassembly_private *pmdp,
                                  MEMSPACE memspace)
{
    pmdp->memspace = memspace;
}

MEMSPACE mon_disassembly_get_memspace(struct mon_disassembly_private *pmdp)
{
    return pmdp->memspace;
}


struct mon_disassembly *mon_disassembly_get_lines(
    struct mon_disassembly_private *pmdp, int lines_visible,
    int lines_full_visible)
{
    WORD loc;
    unsigned int size;
    int  i;
    unsigned int  have_label = pmdp->have_label;
    struct mon_disassembly *contents = NULL;
    struct mon_disassembly *ret;

    loc = pmdp->StartAddress;
    ret = NULL;

    pmdp->Lines = lines_full_visible;

    for (i = 0; i < lines_visible; i++ ) {
        struct mon_disassembly *newcont;
        mon_breakpoint_type_t bptype;

        newcont = xmalloc(sizeof(struct mon_disassembly));

        if (ret == NULL) {
            ret      =
            contents = newcont;
        } else {
            contents = contents->next = newcont;
        }

        contents->next = NULL;
        contents->flags.active_line = loc == pmdp->CurrentAddress ? 1 : 0;

        /* determine type of breakpoint */
        bptype = mon_is_breakpoint(new_addr(pmdp->memspace, loc));

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
WORD determine_address_of_line(struct mon_disassembly_private *pmdp, 
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

        free(content);

        loc += size;
    }

    return loc;
}

static
WORD scroll_down(struct mon_disassembly_private *pmdp, WORD loc)
{
    return determine_address_of_line(pmdp, loc, 1);
}

static
WORD scroll_down_page(struct mon_disassembly_private *pmdp, WORD loc)
{
    /* the count is one less than visible,
       so there will be one visible line left on the screen! */
    return determine_address_of_line( pmdp, loc, pmdp->Lines - 1 );
}

static
WORD scroll_up_count(struct mon_disassembly_private *pmdp, WORD loc,
                        unsigned int count)
{
    unsigned int size;
    /* this has to be initialized with zero for correct processing */
    unsigned int have_label = 0;

    WORD testloc = loc - 3 * count - 3;

    unsigned int *disp = xmalloc( sizeof(unsigned int)*count );
    unsigned int storepos = 0;

    while (testloc < loc) {
        char *content;

        disp[storepos++] = loc - testloc;
        if (storepos == count)
            storepos = 0;

	    content = mon_disassemble_with_label(pmdp->memspace, testloc, 1,
                                                 &size, &have_label );

        free(content);
        testloc += size;
    }

    loc -= disp[storepos];

    free(disp);

    return loc;
}

static
WORD scroll_up(struct mon_disassembly_private *pmdp, WORD loc)
{
    return scroll_up_count( pmdp, loc, 1 );
}

static
WORD scroll_up_page(struct mon_disassembly_private *pmdp, WORD loc)
{
    /* the count is one less than visible,
       so there will be one visible line left on the screen! */
    return scroll_up_count(pmdp, loc, pmdp->Lines - 1);
}

WORD mon_disassembly_scroll(struct mon_disassembly_private *pmdp, 
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

WORD mon_disassembly_scroll_to(struct mon_disassembly_private *pmdp, 
                               WORD addr)
{
    pmdp->StartAddress = addr;
    return pmdp->StartAddress;
}

void mon_disassembly_set_breakpoint(struct mon_disassembly_private *pmdp)
{
    mon_set_breakpoint(pmdp->AddrClicked);
}

void mon_disassembly_unset_breakpoint(struct mon_disassembly_private *pmdp)
{
    mon_unset_breakpoint(pmdp->AddrClicked);
}

void mon_disassembly_enable_breakpoint(struct mon_disassembly_private *pmdp)
{
    mon_enable_breakpoint(pmdp->AddrClicked);
}

void mon_disassembly_disable_breakpoint(struct mon_disassembly_private *pmdp)
{
    mon_disable_breakpoint(pmdp->AddrClicked);
}

void mon_disassembly_goto_address(struct mon_disassembly_private *pmdp,
                                  WORD addr)
{
    pmdp->CurrentAddress = addr;
    mon_disassembly_check_if_in_range(pmdp);
}

void mon_disassembly_goto_pc(struct mon_disassembly_private *pmdp)
{
    mon_disassembly_goto_address(pmdp, 
        (WORD)(monitor_cpu_type.mon_register_get_val(pmdp->memspace, e_PC)));
}


void mon_disassembly_determine_popup_commands(
                                   struct mon_disassembly_private *pmdp, 
                                   int xPos, int yPos, WORD *ulMask,
                                   WORD *ulDefault)
{
    MON_ADDR CurrentAddress;
    mon_breakpoint_type_t mbt;

    int drive_true_emulation;

    resources_get_value("DriveTrueEmulation",
        (resource_value_t *)&drive_true_emulation);

    CurrentAddress = new_addr(pmdp->memspace, determine_address_of_line(pmdp,
                              pmdp->StartAddress, yPos));
    mbt = mon_is_breakpoint(CurrentAddress);

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
        switch (pmdp->memspace) {
          case e_comp_space:
            *ulMask |= MDDPC_SET_DRIVE8 | MDDPC_SET_DRIVE9;
            break;
          case e_disk8_space:
            *ulMask |= MDDPC_SET_COMPUTER | MDDPC_SET_DRIVE9;
            break;
          case e_disk9_space:
            *ulMask |= MDDPC_SET_COMPUTER | MDDPC_SET_DRIVE8;
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

