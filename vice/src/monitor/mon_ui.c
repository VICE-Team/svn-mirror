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
#include "mon_register.h"
#include "mon_ui.h"
#include "mon_util.h"
#include "utils.h"


struct mon_disassembly_private
{
    MEMSPACE memspace;
    ADDRESS  StartAddress;
    ADDRESS  EndAddress;
    ADDRESS  CurrentAddress;
    int      have_label;
    int      Lines;
};

struct mon_disassembly_private *mon_disassembly_init( void )
{
    struct mon_disassembly_private *ret = xmalloc(sizeof(struct mon_disassembly_private));

    ret->memspace       = e_comp_space;
    ret->StartAddress   = -1;
    ret->EndAddress     = 0;
    ret->CurrentAddress = 0;
    ret->have_label     = 0;

    mon_disassembly_update( ret );

    return ret;
}

void mon_disassembly_deinit(struct mon_disassembly_private *pmdp)
{
    free(pmdp);
}

void mon_disassembly_update(struct mon_disassembly_private *pmdp)
{
    pmdp->CurrentAddress
        = monitor_cpu_type.mon_register_get_val(pmdp->memspace,e_PC);

    if ((pmdp->CurrentAddress < pmdp->StartAddress)
        || (pmdp->CurrentAddress > pmdp->EndAddress)) {
        pmdp->StartAddress = pmdp->CurrentAddress;
        pmdp->EndAddress   = 0;
    }
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
    ADDRESS loc;
    unsigned int size;
    int  i;
    int  have_label = pmdp->have_label;
    struct mon_disassembly *contents = NULL;
    struct mon_disassembly *ret;

    loc = pmdp->StartAddress;
    ret = NULL;

    pmdp->Lines = lines_full_visible;

    for (i = 0; i < lines_visible; i++ )
    {
        struct mon_disassembly *newcont;
        newcont = xmalloc(sizeof(struct mon_disassembly));

        if (ret == NULL)
        {
            ret      = 
            contents = newcont;
        }
        else
        {
            contents       =
            contents->next = newcont;
        }

        contents->next                    = NULL;
        contents->flags.active_line       = loc==pmdp->CurrentAddress ? 1 : 0;
        /* @SRT: just for testing! */
        contents->flags.is_breakpoint     = (loc == 0xA47B) || (loc == 0xA47D);
        /* @SRT: just for testing! */
        contents->flags.breakpoint_active = loc == 0xA47B;

	contents->content = 
        mon_disassemble_with_label(pmdp->memspace, loc, 1, &size, &have_label);

        contents->length  = strlen(contents->content);

        pmdp->EndAddress = loc;

        loc += size;
    }

    return ret;
}

static
ADDRESS scroll_down(struct mon_disassembly_private *pmdp, ADDRESS loc)
{
    unsigned int size;
    char *content;

	content = mon_disassemble_with_label(pmdp->memspace, loc, 1, &size,
                                             &pmdp->have_label );

    free(content);

    return loc + size;
}

static
ADDRESS scroll_down_page(struct mon_disassembly_private *pmdp, ADDRESS loc)
{
    unsigned int size;
    int  i;

    /* it's one less than visible, so there will be one line visible left! */
    for (i = 1; i < pmdp->Lines; i++) {
        char *content;

        content = mon_disassemble_with_label(pmdp->memspace, loc, 1, &size,
                                             &pmdp->have_label );

        free(content);

        loc += size;
    }

    return loc;
}

static
ADDRESS scroll_up_count(struct mon_disassembly_private *pmdp, ADDRESS loc,
                        unsigned int count)
{
    unsigned int size;
    /* this has to be initialized with zero for correct processing */
    int  have_label = 0;
    /* @SRT: TODO: adjust: is this enough? */
    ADDRESS testloc = loc - 3 * count - 3;

    unsigned int *disp = xmalloc( sizeof(unsigned int)*count );
    unsigned int storepos = 0;

    while (testloc < loc)
    {
        char *content;

        disp[storepos++] = loc - testloc;
        if (storepos==count)
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
ADDRESS scroll_up(struct mon_disassembly_private *pmdp, ADDRESS loc)
{
    return scroll_up_count( pmdp, loc, 1 );
}

static
ADDRESS scroll_up_page(struct mon_disassembly_private *pmdp, ADDRESS loc)
{
    /* the count is one less than visible,
       so there will be one line visible left! */
    return scroll_up_count( pmdp, loc, pmdp->Lines - 1);
}

ADDRESS mon_scroll(struct mon_disassembly_private *pmdp,
                   MON_SCROLL_TYPE ScrollType )
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

ADDRESS mon_scroll_to(struct mon_disassembly_private *pmdp, ADDRESS addr)
{
    pmdp->StartAddress = addr;
    return pmdp->StartAddress;
}

void mon_ui_init(void)
{
}

