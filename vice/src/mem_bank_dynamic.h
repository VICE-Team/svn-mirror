/*
 * mem_bank_dynamic.h -- Per-machine helper that merges base bank tables
 *                       with the cartridge bank registry.
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

#ifndef VICE_MEM_BANK_DYNAMIC_H
#define VICE_MEM_BANK_DYNAMIC_H

#include "types.h"
#include "cartridge_banks.h"

/* Static configuration of one helper instance.
 *
 * Holds pointers to the four parallel base-bank tables.  Cart banks are
 * numbered from (max base bank number + 1).
 */
typedef struct mem_bank_dynamic_config_s {
    /* NULL-terminated array of base bank names. */
    const char *const *base_banknames;

    /* -1-terminated array of bank numbers, parallel to base_banknames. */
    const int *base_banknums;

    /* Array of bank indices, parallel to base_banknames. */
    const int *base_bankindex;

    /* Array of bank flags, parallel to base_banknames. */
    const int *base_bankflags;
} mem_bank_dynamic_config_t;

/* Per-machine helper state.
 *
 * A mem module defines one static instance per base table set and
 * initializes only cfg, e.g.
 *
 *     static mem_bank_dynamic_t bank_dyn = { .cfg = &bank_dyn_config };
 *
 * All other members are managed by mem_bank_dynamic.c.
 */
typedef struct mem_bank_dynamic_s {
    const mem_bank_dynamic_config_t *cfg;

    /* Bank tables merged from the base tables and the cartridge bank
       registry.  NULL when no cart banks are registered, in which case
       the accessors return the base tables. */
    char **banknames;
    int *banknums;
    int *bankindex;
    int *bankflags;

    /* Registry entry owning each cart bank, indexed by
       (bank number - cart_banks_start). */
    cart_bank_info_t **cart;

    int num_base_banks;
    int cart_banks_start;
    int num_cart_banks;

    /* Registry generation the tables were built from. */
    unsigned int generation;
    int valid;

    /* Link in the list of instances freed by mem_bank_dynamic_shutdown(). */
    struct mem_bank_dynamic_s *next;
} mem_bank_dynamic_t;

/* Return the merged (base + cart) bank-name list.  NULL-terminated.
 * Valid until the cartridge bank registry next changes. */
const char **mem_bank_dynamic_list(mem_bank_dynamic_t *self);

/* Return the merged bank-number list, parallel to _list().
 * -1-terminated. */
const int *mem_bank_dynamic_list_nos(mem_bank_dynamic_t *self);

/* Return the bank number for `name`, or -1 if not present. */
int mem_bank_dynamic_from_name(mem_bank_dynamic_t *self, const char *name);

/* Return the bank index for `bank`, or -1 if not present. */
int mem_bank_dynamic_index_from_bank(mem_bank_dynamic_t *self, int bank);

/* Return the bank flags for `bank`, or -1 if not present. */
int mem_bank_dynamic_flags_from_bank(mem_bank_dynamic_t *self, int bank);

/* If `bank` is a registered cart bank, store the byte at `addr` in
 * `*out` and return true.  Otherwise return false. */
bool mem_bank_dynamic_try_read(mem_bank_dynamic_t *self, int bank, uint16_t addr, uint8_t *out);

/* If `bank` is a registered cart bank, dispatch the write and return
 * true (writes to a region with a NULL write pointer are dropped but
 * still report true).  Otherwise return false. */
bool mem_bank_dynamic_try_write(mem_bank_dynamic_t *self, int bank, uint16_t addr, uint8_t value);

/* Free the merged tables of all helper instances. */
void mem_bank_dynamic_shutdown(void);

#endif
