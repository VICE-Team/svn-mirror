/*
 * cartridge_banks.c -- Registry of linear cartridge memory regions.
 *
 * Written by
 *  Jesper Balman Gravgaard <jesper@balmangravgaard.dk>
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

#include <assert.h>
#include <stddef.h>

#include "cartridge_banks.h"

static cart_bank_info_t *cart_bank_head = NULL;
static unsigned int cart_bank_gen = 0;

void cartridge_bank_register(cart_bank_info_t *info)
{
    cart_bank_info_t **link;

    assert(info != NULL);
    assert(info->read != NULL);

    for (link = &cart_bank_head; *link != NULL; link = &(*link)->next) {
        if (*link == info) {
            cart_bank_gen++;
            return;
        }
    }

    info->next = NULL;
    *link = info;
    cart_bank_gen++;
}

void cartridge_bank_unregister(cart_bank_info_t *info)
{
    cart_bank_info_t *cur;
    cart_bank_info_t *prev = NULL;

    assert(info != NULL);

    for (cur = cart_bank_head; cur != NULL; cur = cur->next) {
        if (cur == info) {
            if (prev != NULL) {
                prev->next = cur->next;
            } else {
                cart_bank_head = cur->next;
            }
            cur->next = NULL;
            cart_bank_gen++;
            return;
        }
        prev = cur;
    }
}

cart_bank_info_t *cartridge_bank_list(void)
{
    return cart_bank_head;
}

unsigned int cartridge_bank_generation(void)
{
    return cart_bank_gen;
}
