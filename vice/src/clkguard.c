/*
 * clkguard.c - Handle clock counter overflows.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "clkguard.h"

#include "utils.h"

clk_guard_t *clk_guard_new (CLOCK *_clk_ptr, CLOCK _clk_max_value)
{
    clk_guard_t *new;

    new = xmalloc(sizeof(clk_guard_t));
    clk_guard_init(new, _clk_ptr, _clk_max_value);

    return new;
}

void clk_guard_init(clk_guard_t *guard, CLOCK *_clk_ptr, CLOCK _clk_max_value)
{
    guard->clk_ptr = _clk_ptr;
    guard->clk_base = (CLOCK) 0;
    guard->clk_max_value = _clk_max_value;

    guard->callback_list = NULL;
}

void clk_guard_set_clk_base(clk_guard_t *guard, CLOCK _clk_base)
{
    guard->clk_base = _clk_base;
}

CLOCK clk_guard_get_clk_base(clk_guard_t *guard)
{
    return guard->clk_base;
}

void clk_guard_add_callback (clk_guard_t *guard,
                             clk_guard_callback_t function,
                             void *data)
{
    clk_guard_callback_list_t *new;

    new = xmalloc(sizeof(clk_guard_callback_list_t));
    new->function = function;
    new->data = data;

    /* Add to the head of the list.  Order is supposed not to matter at all
       for this purpose.  */
    new->next = guard->callback_list;
    guard->callback_list = new;
}

void clk_guard_destroy (clk_guard_t *guard)
{
    clk_guard_callback_list_t *lp;

    lp = guard->callback_list;
    while (lp != NULL) {
        clk_guard_callback_list_t *lp_next = lp->next;

        free(lp);
        lp = lp_next;
    }

    free(guard);
}

CLOCK clk_guard_prevent_overflow (clk_guard_t *guard)
{
    if (*guard->clk_ptr < guard->clk_max_value) {
        return (CLOCK) 0;
    } else {
        clk_guard_callback_list_t *lp;
        CLOCK sub;

        /* Make sure we have at least 0xfffff cycles for doing our jobs.  */
        sub = guard->clk_max_value - 0xfffff;

        /* Make sure we subtract a multiple of the `clk_base'.  */
        if (guard->clk_base)
            sub = (sub / guard->clk_base) * guard->clk_base;

        /* Update clock counter.  */
        *guard->clk_ptr -= sub;

        /* Execute the callbacks. */
        for (lp = guard->callback_list; lp != NULL; lp = lp->next)
            lp->function (sub, lp->data);

        return sub;
    }
}
