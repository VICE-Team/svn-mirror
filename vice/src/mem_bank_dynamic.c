/*
 * mem_bank_dynamic.c -- Per-machine helper that merges base bank tables
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

#include "vice.h"

#include <stddef.h>
#include <string.h>

#include "lib.h"
#include "mem.h"
#include "mem_bank_dynamic.h"

/* Instances holding merged tables, freed by mem_bank_dynamic_shutdown(). */
static mem_bank_dynamic_t *instances = NULL;

static void free_tables(mem_bank_dynamic_t *self)
{
    int i;

    if (self->banknames == NULL) {
        return;
    }

    for (i = 0; i < self->num_cart_banks; i++) {
        lib_free(self->banknames[self->num_base_banks + i]);
    }
    lib_free(self->banknames);
    lib_free(self->banknums);
    lib_free(self->bankindex);
    lib_free(self->bankflags);
    lib_free(self->cart);

    self->banknames = NULL;
    self->banknums = NULL;
    self->bankindex = NULL;
    self->bankflags = NULL;
    self->cart = NULL;
    self->num_cart_banks = 0;
}

static void link_instance(mem_bank_dynamic_t *self)
{
    mem_bank_dynamic_t *cur;

    for (cur = instances; cur != NULL; cur = cur->next) {
        if (cur == self) {
            return;
        }
    }
    self->next = instances;
    instances = self;
}

static void rebuild(mem_bank_dynamic_t *self)
{
    const mem_bank_dynamic_config_t *cfg = self->cfg;
    cart_bank_info_t *info;
    int num_base = 0;
    int num_cart = 0;
    int total;
    int i;
    int j;

    free_tables(self);

    self->generation = cartridge_bank_generation();
    self->valid = 1;

    for (info = cartridge_bank_list(); info != NULL; info = info->next) {
        num_cart += info->num_banks;
    }
    if (num_cart == 0) {
        return;
    }

    while (cfg->base_banknames[num_base] != NULL) {
        num_base++;
    }
    self->num_base_banks = num_base;

    self->cart_banks_start = 0;
    for (i = 0; i < num_base; i++) {
        if (cfg->base_banknums[i] >= self->cart_banks_start) {
            self->cart_banks_start = cfg->base_banknums[i] + 1;
        }
    }

    total = num_base + num_cart;
    self->banknames = lib_malloc((size_t)(total + 1) * sizeof(char *));
    self->banknums = lib_malloc((size_t)(total + 1) * sizeof(int));
    self->bankindex = lib_malloc((size_t)(total + 1) * sizeof(int));
    self->bankflags = lib_malloc((size_t)(total + 1) * sizeof(int));
    self->cart = lib_malloc((size_t)num_cart * sizeof(cart_bank_info_t *));
    self->num_cart_banks = num_cart;

    for (i = 0; i < num_base; i++) {
        self->banknames[i] = (char *)cfg->base_banknames[i];
        self->banknums[i] = cfg->base_banknums[i];
        self->bankindex[i] = cfg->base_bankindex[i];
        self->bankflags[i] = cfg->base_bankflags[i];
    }

    j = 0;
    for (info = cartridge_bank_list(); info != NULL; info = info->next) {
        for (i = 0; i < info->num_banks; i++) {
            int flags = MEM_BANK_ISARRAY;

            if (i == 0) {
                flags |= MEM_BANK_ISARRAYFIRST;
            }
            if (i == info->num_banks - 1) {
                flags |= MEM_BANK_ISARRAYLAST;
            }
            self->banknames[num_base + j] = lib_msprintf("%s%02x", info->prefix, (unsigned int)i);
            self->banknums[num_base + j] = self->cart_banks_start + j;
            self->bankindex[num_base + j] = i;
            self->bankflags[num_base + j] = flags;
            self->cart[j] = info;
            j++;
        }
    }

    self->banknames[total] = NULL;
    self->banknums[total] = -1;
    self->bankindex[total] = -1;
    self->bankflags[total] = -1;

    link_instance(self);
}

static void check_rebuild(mem_bank_dynamic_t *self)
{
    if (!self->valid || self->generation != cartridge_bank_generation()) {
        rebuild(self);
    }
}

static const char **get_names(mem_bank_dynamic_t *self)
{
    check_rebuild(self);
    if (self->banknames != NULL) {
        return (const char **)self->banknames;
    }
    return (const char **)self->cfg->base_banknames;
}

static const int *get_nums(mem_bank_dynamic_t *self)
{
    check_rebuild(self);
    return self->banknums != NULL ? self->banknums : self->cfg->base_banknums;
}

static const int *get_index(mem_bank_dynamic_t *self)
{
    check_rebuild(self);
    return self->bankindex != NULL ? self->bankindex : self->cfg->base_bankindex;
}

static const int *get_flags(mem_bank_dynamic_t *self)
{
    check_rebuild(self);
    return self->bankflags != NULL ? self->bankflags : self->cfg->base_bankflags;
}

/* Return the cart bank offset (bank - cart_banks_start) of `bank`, or -1. */
static int cart_bank_offset(mem_bank_dynamic_t *self, int bank)
{
    check_rebuild(self);
    if (bank < self->cart_banks_start
        || bank >= self->cart_banks_start + self->num_cart_banks) {
        return -1;
    }
    return bank - self->cart_banks_start;
}

const char **mem_bank_dynamic_list(mem_bank_dynamic_t *self)
{
    return get_names(self);
}

const int *mem_bank_dynamic_list_nos(mem_bank_dynamic_t *self)
{
    return get_nums(self);
}

int mem_bank_dynamic_from_name(mem_bank_dynamic_t *self, const char *name)
{
    const char **names = get_names(self);
    const int *nums = get_nums(self);
    int i;

    for (i = 0; names[i] != NULL; i++) {
        if (strcmp(name, names[i]) == 0) {
            return nums[i];
        }
    }
    return -1;
}

int mem_bank_dynamic_index_from_bank(mem_bank_dynamic_t *self, int bank)
{
    const int *nums = get_nums(self);
    const int *idx = get_index(self);
    int i;

    for (i = 0; nums[i] > -1; i++) {
        if (nums[i] == bank) {
            return idx[i];
        }
    }
    return -1;
}

int mem_bank_dynamic_flags_from_bank(mem_bank_dynamic_t *self, int bank)
{
    const int *nums = get_nums(self);
    const int *flags = get_flags(self);
    int i;

    for (i = 0; nums[i] > -1; i++) {
        if (nums[i] == bank) {
            return flags[i];
        }
    }
    return -1;
}

/* Return the linear cart address of `addr` in cart bank `offset`. */
static unsigned int cart_linear_addr(mem_bank_dynamic_t *self, int offset, uint16_t addr)
{
    unsigned int index = (unsigned int)self->bankindex[self->num_base_banks + offset];

    return (index << 16) | addr;
}

bool mem_bank_dynamic_try_read(mem_bank_dynamic_t *self, int bank, uint16_t addr, uint8_t *out)
{
    int offset = cart_bank_offset(self, bank);

    if (offset < 0) {
        return false;
    }
    *out = self->cart[offset]->read(cart_linear_addr(self, offset, addr));
    return true;
}

bool mem_bank_dynamic_try_write(mem_bank_dynamic_t *self, int bank, uint16_t addr, uint8_t value)
{
    int offset = cart_bank_offset(self, bank);

    if (offset < 0) {
        return false;
    }
    if (self->cart[offset]->write != NULL) {
        self->cart[offset]->write(cart_linear_addr(self, offset, addr), value);
    }
    return true;
}

void mem_bank_dynamic_shutdown(void)
{
    mem_bank_dynamic_t *cur;
    mem_bank_dynamic_t *next;

    for (cur = instances; cur != NULL; cur = next) {
        next = cur->next;
        free_tables(cur);
        cur->valid = 0;
        cur->next = NULL;
    }
    instances = NULL;
}
