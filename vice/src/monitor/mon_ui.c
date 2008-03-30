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

#include "mon.h"
#include "montypes.h" /* TODO: @SRT to be removed! */
#include "mon_register.h"
#include "mon_ui.h"
#include "mon_util.h"
#include "utils.h"

#include <malloc.h>
#include <string.h>


#if 1
/* @@@SRT: TODO: This is just a quick hack! */

#include "asm.h"
extern monitor_cpu_type_t monitor_cpu_type;

#define mon_get_reg_val(_a_,_b_) \
   ((monitor_cpu_type.mon_register_get_val)(_a_, _b_))

#endif /* #if 1 */


struct mon_disassembly_private
{
	MEMSPACE memspace;
    ADDRESS  StartAddress;
    ADDRESS  EndAddress;
    ADDRESS  CurrentAddress;
    int      Lines;
};

struct mon_disassembly_private *mon_disassembly_init( void )
{
    struct mon_disassembly_private *ret = xmalloc(sizeof(struct mon_disassembly_private));

    ret->memspace       = e_comp_space;
    ret->StartAddress   = -1;
    ret->EndAddress     = 0;
    ret->CurrentAddress = 0;

    mon_disassembly_update( ret );

    return ret;
}

void mon_disassembly_deinit( struct mon_disassembly_private *pmdp )
{
    free(pmdp);
}

void mon_disassembly_update( struct mon_disassembly_private *pmdp )
{
    pmdp->CurrentAddress = mon_get_reg_val(pmdp->memspace,e_PC);

    if ((pmdp->CurrentAddress < pmdp->StartAddress) || (pmdp->CurrentAddress > pmdp->EndAddress))
    {
        pmdp->StartAddress = pmdp->CurrentAddress;
        pmdp->EndAddress   = 0;
    }
}

void mon_disassembly_set_memspace( struct mon_disassembly_private *pmdp, MEMSPACE memspace )
{
    pmdp->memspace = memspace;
}

MEMSPACE mon_disassembly_get_memspace( struct mon_disassembly_private *pmdp )
{
    return pmdp->memspace;
}


struct mon_disassembly *mon_disassembly_get_lines( struct mon_disassembly_private *pmdp, int lines )
{
    ADDRESS loc;
    unsigned int size;
    int  i;
    int  have_label = 0; /* this *must* be initialized with zero! */
    struct mon_disassembly *contents;
    struct mon_disassembly *ret;

    loc = pmdp->StartAddress;
    ret = NULL;

    pmdp->Lines = lines;

    for (i=0; i<lines; i++ )
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
        contents->flags.is_breakpoint     = (loc == 0xA486) || (loc == 0xA488); /* @SRT: just for testing! */
        contents->flags.breakpoint_active = loc == 0xA488;                      /* @SRT: just for testing! */

		contents->content = 
            mon_disassemble_with_label(pmdp->memspace, loc, 1, &size, &have_label );

        contents->length  = strlen(contents->content);

        pmdp->EndAddress = loc;

        loc += size;
    }

    return ret;
}

static
ADDRESS scroll_down( struct mon_disassembly_private *pmdp, ADDRESS loc )
{
    unsigned int size;
    int  have_label = 1; /* with 1, we prevent processing of labels! */
    char *content;

	content = 
        mon_disassemble_with_label(pmdp->memspace, loc, 1, &size, &have_label );

    free(content);

    return loc + size;
}

static
ADDRESS scroll_down_page( struct mon_disassembly_private *pmdp, ADDRESS loc )
{
    unsigned int size;
    int  have_label = 0; /* this has to be initialized with zero for correct processing */
    int  i;

    for (i=2; i<pmdp->Lines; i++)
    {
        char *content;

	    content = 
            mon_disassemble_with_label(pmdp->memspace, loc, 1, &size, &have_label );

        free(content);

        loc += size;
    }

    return loc;
}

static
ADDRESS scroll_up( struct mon_disassembly_private *pmdp, ADDRESS loc )
{
    /* @SRT TODO: HOW TO DO THIS??? */
    return loc - 1;
}

static
ADDRESS scroll_up_page( struct mon_disassembly_private *pmdp, ADDRESS loc )
{
    /* @SRT TODO: HOW TO DO THIS??? */
    return loc - 30;
}

ADDRESS mon_scroll ( struct mon_disassembly_private *pmdp, MON_SCROLL_TYPE ScrollType )
{
    switch (ScrollType)
    {
        case MON_SCROLL_NOTHING:
            break;

        case MON_SCROLL_DOWN:
            pmdp->StartAddress = scroll_down( pmdp, pmdp->StartAddress );
            break;

        case MON_SCROLL_UP:
            pmdp->StartAddress = scroll_up( pmdp, pmdp->StartAddress );
            break;

        case MON_SCROLL_PAGE_DOWN:
            pmdp->StartAddress = scroll_down_page( pmdp, pmdp->StartAddress );
            break;

        case MON_SCROLL_PAGE_UP:
            pmdp->StartAddress = scroll_up_page( pmdp, pmdp->StartAddress );
            break;
    }
    return pmdp->StartAddress;
}

ADDRESS mon_scroll_to( struct mon_disassembly_private *pmdp, ADDRESS addr )
{
    pmdp->StartAddress = addr;
    return pmdp->StartAddress;
}
