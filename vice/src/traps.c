/*
 * traps.c - Allow VICE to replace ROM code with C function calls.
 *
 * Written by
 *   Teemu Rantanen (tvr@cs.hut.fi)
 *   Jarkko Sonninen (sonninen@lut.fi)
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
#include <memory.h>

#include "traps.h"
#include "maincpu.h"
#include "mem.h"
#include "interrupt.h"

/* #define DEBUG_TRAPS */

typedef struct _traplist_t {
    struct _traplist_t *next;
    const trap_t *trap;
} traplist_t;

static traplist_t *traplist;

#ifdef IDLE_TRAP
static void trap_idle_E5D4();

static trap_t idle_trap =
{
    "Idle trap",
    0xE5D4,
    {0xF0, 0xF7, 0x78},
    trap_idle_E5D4,
};
#endif

void    initialize_traps()
{
    traplist = NULL;

#ifdef IDLE_TRAP
    set_trap(&idle_trap);
#endif
}


int     set_trap(const trap_t *t)
{
    int     i;
    traplist_t *p;

#ifdef DEBUG_TRAPS
    printf("setting up trap %s at 0x%x\n", t -> name, t -> address);
#endif

    for (i = 0; i < 3; i++) {
	if (read_rom(t -> address + i) != t -> check[i]) {
	    printf("incorrect checkbyte for trap %s. not installed\n",
		t -> name);
	    return 1;
	}
    }

    /*
     * BRK (0x00) is trap-opcode
     */
    store_rom(t -> address, 0x00);

    p = (traplist_t *) malloc (sizeof (traplist_t));
    p->next = traplist;
    p->trap = t;
    traplist = p;

    return 0;
}


int     remove_trap(const trap_t *t)
{
    traplist_t *p = traplist, *prev = NULL;

#ifdef DEBUG_TRAPS
    printf("removing trap %s at 0x%x\n", t -> name, t -> address);
#endif

    if (read_rom(t -> address) != 0x00)
	printf("incorrect checkbyte for trap %s.\n", t -> name);

    store_rom(t -> address, t->check[0]);


    while (p) {
	if (p -> trap -> address == t -> address)
	    break;
	prev = p;
	p = p -> next;
    }

    if (!p) {
	printf("cannot remove trap %s.\n", t -> name);
	return -1;
    }

    if (prev)
	prev -> next = p -> next;
    else
	traplist = p -> next;

    free ((char *)p);
    return 0;
}

int     trap_handler(void)
{
    traplist_t *p = traplist;

    while (p) {
	if (p -> trap -> address == maincpu_regs.pc) {
	    (*p -> trap -> func) ();
            maincpu_regs.pc = p->trap->resume_address;
	    return 0;
	}
	p = p -> next;
    }

    return 1;
}


/* FIXME: this is obsolete... */
#ifdef IDLE_TRAP
static void trap_idle_E5D4()
{
    if (!IF_ZERO())
    {
	maincpu_regs.pc = 0xe5d6;
	clk += 2;
    }
    else {
	clk = next_alarm_clk(&maincpu_int_status);
	if (LOAD(0xc6))
	    maincpu_regs.pc = 0xe5cd;
    }
}

#endif  /* IDLE_TRAP */
