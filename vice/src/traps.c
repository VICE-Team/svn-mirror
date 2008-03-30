/*
 * traps.c - Allow VICE to replace ROM code with C function calls.
 *
 * Written by
 *   Teemu Rantanen (tvr@cs.hut.fi)
 *   Jarkko Sonninen (sonninen@lut.fi)
 *   Ettore Perazzoli (ettore@comm2000.it)
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

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#endif

#include "traps.h"
#include "maincpu.h"
#include "mem.h"
#include "interrupt.h"
#include "resources.h"
#include "cmdline.h"
#include "utils.h"

/* #define DEBUG_TRAPS */

#ifdef DEBUG_TRAPS
#define DEBUG(x)        printf x
#else
#define DEBUG(x)
#endif

typedef struct _traplist_t {
    struct _traplist_t *next;
    const trap_t *trap;
} traplist_t;

static traplist_t *traplist;

static int install_trap(const trap_t *t);
static int remove_trap(const trap_t *t);

/* ------------------------------------------------------------------------- */

/* Trap-related resources.  */

/* Flag: Should we avoid installing traps at all?  */
static int no_traps_enabled;

static int set_no_traps_enabled(resource_value_t v)
{
    int new_value = (int) v;

    if ((!no_traps_enabled && new_value) || (no_traps_enabled && !new_value)) {
        if (new_value) {
            /* Traps have been disabled.  */
            traplist_t *p;

            for (p = traplist; p != NULL; p = p->next)
                remove_trap(p->trap);
        } else {
            /* Traps have been enabled.  */
            traplist_t *p;

            for (p = traplist; p != NULL; p = p->next)
                install_trap(p->trap);
        }
    }

    no_traps_enabled = new_value;
    return 0;
}

static resource_t resources[] = {
    { "NoTraps", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t) &no_traps_enabled, set_no_traps_enabled },
    { NULL }
};

int traps_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* Trap-related command-line options.  */

static cmdline_option_t cmdline_options[] = {
    { "-traps", SET_RESOURCE, 0, NULL, NULL, "NoTraps", (resource_value_t) 0,
      NULL, "Enable Kernal serial traps for fast serial/tape emulation" },
    { "+traps", SET_RESOURCE, 0, NULL, NULL, "NoTraps", (resource_value_t) 1,
      NULL, "Do not install any Kernal traps" },
    { NULL }
};

int traps_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

void traps_init(void)
{
    traplist = NULL;
}

static int install_trap(const trap_t *t)
{
    int i;

    DEBUG(("TRAPS: Patching ROM for trap `%s' at $%04X\n",
           t->name, t->address));

    for (i = 0; i < 3; i++) {
	if (read_rom(t->address + i) != t->check[i]) {
	    printf("TRAPS: Incorrect checkbyte for trap `%s'.  Not installed.\n",
                   t->name);
	    return -1;
	}
    }

    /* BRK (0x00) is trap-opcode.  */
    store_rom(t->address, 0x00);

    return 0;
}

int traps_add(const trap_t *t)
{
    traplist_t *p;

    DEBUG(("TRAPS: Adding trap `%s' at $%04X (enabled=%d).\n", t->name, t->address, !no_traps_enabled));

    p = (traplist_t *) xmalloc (sizeof (traplist_t));
    p->next = traplist;
    p->trap = t;
    traplist = p;

    if (!no_traps_enabled)
        install_trap(t);

    return 0;
}

static int remove_trap(const trap_t *t)
{
    DEBUG(("TRAPS: Removing trap `%s' at $%04X.\n", t->name, t->address));

    if (read_rom(t->address) != 0x00) {
	printf("TRAPS: No trap `%s' installed?\n", t->name);
        return -1;
    }

    store_rom(t->address, t->check[0]);
    return 0;
}

int traps_remove(const trap_t *t)
{
    traplist_t *p = traplist, *prev = NULL;

    while (p) {
	if (p->trap->address == t->address)
	    break;
	prev = p;
	p = p->next;
    }

    if (!p) {
	printf("TRAPS: Trap `%s' not found.\n", t->name);
	return -1;
    }

    if (prev)
	prev->next = p->next;
    else
	traplist = p ->next;

    free(p);

    if (!no_traps_enabled)
        remove_trap(t);

    return 0;
}

int traps_handler(void)
{
    traplist_t *p = traplist;
    unsigned int pc = MOS6510_REGS_GET_PC(&maincpu_regs);

    DEBUG(("TRAPS: Checking for trap at PC=$%04X, p=%p.\n", maincpu_regs.pc,p));

    while (p) {
        DEBUG(("TRAPS: check Address %04X\n", p->trap->address));
	if (p->trap->address == pc) {
            /* This allows the trap function to remove traps.  */
            ADDRESS resume_address = p->trap->resume_address;

            DEBUG(("TRAPS: Found %s\n", p->trap->name));
	    (*p->trap->func)();
            /* XXX ALERT!  `p' might not be valid anymore here, because
               `p->trap->func()' might have removed all the traps.  */
            MOS6510_REGS_SET_PC(&maincpu_regs, resume_address);
            return 0;
	}
	p = p->next;
    }

    return -1;
}
