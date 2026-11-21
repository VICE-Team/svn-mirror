/*
 * cartridge_banks.h -- Registry of linear cartridge memory regions.
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

#ifndef VICE_CARTRIDGE_BANKS_H
#define VICE_CARTRIDGE_BANKS_H

#include "types.h"

/* Registry entry describing one linear cartridge memory region.
 *
 * One entry advertises num_banks banks of 64 KiB each, named
 * "<prefix>00".."<prefix>NN".  A cartridge populates a static
 * cart_bank_info_t and calls cartridge_bank_register() to add it; a
 * cartridge with multiple regions registers multiple entries, each
 * with a unique prefix.
 *
 * Reads call info->read(linear_addr) where linear_addr is
 * (bank_index << 16) | offset and bank_index ranges over
 * 0..num_banks-1.  Writes call info->write with the same address
 * layout; a NULL write pointer marks the region read-only and writes
 * through the registry are dropped.
 *
 * Banks are numbered in registration order, so registering a region
 * does not renumber the banks of regions registered before it.
 */
typedef struct cart_bank_info_s {
    /* Short lowercase prefix used to form bank names.  Bank names are
     * formed as prefix%02x, so a prefix of length N produces names of
     * length N+2.  Must be unique across all registered structs (a
     * cart with two regions uses two prefixes). */
    const char *prefix;

    /* Number of 64 KiB banks exposed. */
    int num_banks;

    /* Read one byte from a linear address within this region.  Required.
     *   addr   linear byte address: (bank_index << 16) | offset,
     *          always below num_banks << 16
     *   ret    the byte at addr */
    uint8_t (*read)(unsigned int addr);

    /* Write one byte to a linear address within this region.
     *
     * NULL marks the region read-only; writes through the registry are
     * dropped.  Implementations should index the backing buffer
     * directly (no bank-select or flash command-sequence logic).
     *
     *   addr   linear byte address: (bank_index << 16) | offset
     *   value  byte value to write */
    void (*write)(unsigned int addr, uint8_t value);

    /* Next registered entry.  Maintained by the registry; callers of
     * cartridge_bank_list() may read it but must not write it. */
    struct cart_bank_info_s *next;
} cart_bank_info_t;

/* Append a cart_bank_info_t to the registry.  If `info` is already in
 * the registry the entry is not duplicated; the generation counter is
 * bumped either way, so re-registering after a size change is enough. */
void cartridge_bank_register(cart_bank_info_t *info);

/* Remove a cart_bank_info_t from the registry.  No-op if `info` is not
 * currently in the registry. */
void cartridge_bank_unregister(cart_bank_info_t *info);

/* Return the head of the registered cart_bank_info_t list.  Iterate
 * via info->next; the list terminates with NULL. */
cart_bank_info_t *cartridge_bank_list(void);

/* Return a counter that increments on every register/unregister. */
unsigned int cartridge_bank_generation(void);

#endif
