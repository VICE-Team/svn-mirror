/*
 * c64io.c - C64 io handling ($DE00-$DFFF).
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include <string.h>

#include "c64io.h"
#include "cartridge.h"
#include "lib.h"
#include "monitor.h"
#include "resources.h"
#include "translate.h"
#include "types.h"
#include "uiapi.h"
#include "util.h"
#include "vicii-phi1.h"
#include "vicii.h"

/* #define IODEBUG */

#ifdef IODEBUG
#define DBG(x) printf x
#else
#define DBG(x)
#endif

static io_source_list_t c64io1_head = { NULL, NULL, NULL };
static io_source_list_t c64io2_head = { NULL, NULL, NULL };

static void io_source_detach(int detach_id, char *resource_name)
{
    switch (detach_id) {
        case IO_DETACH_CART:
            cartridge_detach_image(-1); /* FIXME ! - pass cart id that is associated with io source */
            break;
        case IO_DETACH_RESOURCE:
            resources_set_int(resource_name, 0);
            break;
    }
    ui_update_menus();
}

static void io_source_msg_detach(WORD addr, int amount, io_source_list_t *start)
{
    io_source_detach_t *detach_list = lib_malloc(sizeof(io_source_detach_t) * amount);
    io_source_list_t *current = start;
    char *old_msg = NULL;
    char *new_msg = NULL;
    int i = 0;

    current = current->next;

    while (current) {
        if (current->device->io_source_valid) {
            detach_list[i].det_id = current->device->detach_id;
            detach_list[i].det_name = current->device->resource_name;
            if (i == 0) {
                old_msg = lib_stralloc(translate_text(IDGS_IO_READ_COLL_AT_X_FROM));
                new_msg = util_concat(old_msg, current->device->name, NULL);
                lib_free(old_msg);
            }
            if (i != amount - 1 && i != 0) {
                old_msg = new_msg;
                new_msg = util_concat(old_msg, ", ", current->device->name, NULL);
                lib_free(old_msg);
            }
            if (i == amount - 1) {
                old_msg = new_msg;
                new_msg = util_concat(old_msg, translate_text(IDGS_AND), current->device->name, translate_text(IDGS_ALL_DEVICES_DETACHED), NULL);
                lib_free(old_msg);
            }
            i++;
            if (i == amount) {
                break;
            }
        }
        current = current->next;
    }

    ui_error(new_msg, addr);
    lib_free(new_msg);

    for (i = 0; i < amount; i++) {
        io_source_detach(detach_list[i].det_id, detach_list[i].det_name);
    }
    lib_free(detach_list);
}

static inline BYTE io_read(io_source_list_t *list, WORD addr)
{
    io_source_list_t *current = list->next;
    int io_source_counter = 0;
    BYTE retval = 0;

    vicii_handle_pending_alarms_external(0);

    while (current) {
        if (current->device->read != NULL) {
            if (addr >= current->device->start_address && addr <= current->device->end_address) {
                retval = current->device->read((WORD)(addr & current->device->address_mask));
                if (current->device->io_source_valid) {
                    io_source_counter++;
                }
            } else {
                current->device->io_source_valid = 0;
            }
        }
        current = current->next;
    }

    if (io_source_counter == 0) {
        return vicii_read_phi1();
    }

    if (io_source_counter == 1) {
        return retval;
    }

    io_source_msg_detach(addr, io_source_counter, &c64io1_head);

    return vicii_read_phi1();
}

/* peek from i/o area with no side-effects */
static inline BYTE io_peek(io_source_list_t *list, WORD addr)
{
    io_source_list_t *current = list->next;
    int io_source_counter = 0;
    BYTE retval = 0;

    while (current) {
        if (current->device->read != NULL) {
            if (addr >= current->device->start_address && addr <= current->device->end_address) {
                if (current->device->peek) {
                    retval = current->device->peek((WORD)(addr & current->device->address_mask));
                } else {
                    retval = current->device->read((WORD)(addr & current->device->address_mask));
                }
                if (current->device->io_source_valid) {
                    io_source_counter++;
                }
            } else {
                current->device->io_source_valid = 0;
            }
        }
        current = current->next;
    }

    if (io_source_counter == 0) {
        return vicii_read_phi1();
    }

    if (io_source_counter == 1) {
        return retval;
    }

    io_source_msg_detach(addr, io_source_counter, &c64io1_head);

    return vicii_read_phi1();
}

static inline void io_store(io_source_list_t *list, WORD addr, BYTE value)
{
    io_source_list_t *current = list->next;

    vicii_handle_pending_alarms_external_write();

    while (current) {
        if (current->device->store != NULL) {
            if (addr >= current->device->start_address && addr <= current->device->end_address) {
                current->device->store((WORD)(addr & current->device->address_mask), value);
            }
        }
        current = current->next;
    }
}

/* ---------------------------------------------------------------------------------------------------------- */

io_source_list_t *c64io_register(io_source_t *device)
{
    io_source_list_t *current;
    io_source_list_t *retval = lib_malloc(sizeof(io_source_list_t));

    if ((device->start_address & 0xff00) == 0xde00) {
        current = &c64io1_head;
    } else {
        current = &c64io2_head;
    }

    while (current->next != NULL) {
        current = current->next;
    }
    current->next = retval;
    retval->previous = current;
    retval->device = device;
    retval->next = NULL;

    return retval;
}

void c64io_unregister(io_source_list_t *device)
{
    io_source_list_t *prev;
    if (device) {
        prev = device->previous;
        prev->next = device->next;

        if (device->next) {
            device->next->previous = prev;
        }

        lib_free(device);
    }
}

/* ---------------------------------------------------------------------------------------------------------- */

BYTE REGPARM1 c64io1_read(WORD addr)
{
    DBG(("io1 r %04x\n", addr));
    return io_read(&c64io1_head, addr);
}

BYTE REGPARM1 c64io1_peek(WORD addr)
{
    DBG(("io1 p %04x\n", addr));
    return io_peek(&c64io1_head, addr);
}

void REGPARM2 c64io1_store(WORD addr, BYTE value)
{
    DBG(("io1 w %04x %02x\n", addr, value));
    io_store(&c64io1_head, addr, value);
}

BYTE REGPARM1 c64io2_read(WORD addr)
{
    DBG(("io2 r %04x\n", addr));
    return io_read(&c64io2_head, addr);
}

BYTE REGPARM1 c64io2_peek(WORD addr)
{
    DBG(("io2 p %04x\n", addr));
    return io_peek(&c64io2_head, addr);
}

void REGPARM2 c64io2_store(WORD addr, BYTE value)
{
    DBG(("io2 w %04x %02x\n", addr, value));
    io_store(&c64io2_head, addr, value);
}

/* ---------------------------------------------------------------------------------------------------------- */

void c64io_ioreg_add_list(struct mem_ioreg_list_s **mem_ioreg_list)
{
    io_source_list_t *current = c64io1_head.next;

    while (current) {
        mon_ioreg_add_list(mem_ioreg_list, current->device->name, current->device->start_address, current->device->end_address, current->device->dump);
        current = current->next;
    }

    current = c64io2_head.next;

    while (current) {
        mon_ioreg_add_list(mem_ioreg_list, current->device->name, current->device->start_address, current->device->end_address, current->device->dump);
        current = current->next;
    }
}

/* ---------------------------------------------------------------------------------------------------------- */

static int cpu_lines;
static char *cpu_lines_lock_name;

int get_cpu_lines_lock(void)
{
    return cpu_lines;
}

void set_cpu_lines_lock(int device, char *name)
{
    cpu_lines = device;
    cpu_lines_lock_name = name;
}

void remove_cpu_lines_lock(void)
{
    cpu_lines = 0;
    cpu_lines_lock_name = NULL;
}

char *get_cpu_lines_lock_name(void)
{
    return cpu_lines_lock_name;
}
