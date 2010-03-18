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

static io_source_list_t c64io1_head = { NULL, NULL, NULL };
static io_source_list_t c64io2_head = { NULL, NULL, NULL };

static void io_source_detach(int detach_id, char *resource_name)
{
    switch (detach_id) {
        case IO_DETACH_CART:
            cartridge_detach_image();
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

    while (1) {
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
        if (current->next == NULL) {
            break;
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
    io_source_list_t *prev = device->previous;

    prev->next = device->next;

    if (device->next) {
        device->next->previous = prev;
    }

    lib_free(device);
}

/* ---------------------------------------------------------------------------------------------------------- */

BYTE REGPARM1 c64io1_read(WORD addr)
{
    io_source_list_t *current = &c64io1_head;
    int io_source_counter = 0;
    BYTE real_retval;
    BYTE retval = 0;

    vicii_handle_pending_alarms_external(0);

    while (current->next) {
        current = current->next;
        if (current->device->read != NULL) {
            if (addr >= current->device->start_address && addr <= current->device->end_address) {
                retval = current->device->read((WORD)(addr & current->device->address_mask));
                if (current->device->io_source_valid) {
                    real_retval = retval;
                    io_source_counter++;
                }
            } else {
                current->device->io_source_valid = 0;
            }
        }
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

void REGPARM2 c64io1_store(WORD addr, BYTE value)
{
    io_source_list_t *current = &c64io1_head;

    vicii_handle_pending_alarms_external_write();

    while (current->next) {
        current = current->next;
        if (current->device->store != NULL) {
            if (addr >= current->device->start_address && addr <= current->device->end_address) {
                current->device->store((WORD)(addr & current->device->address_mask), value);
            }
        }
    }
}

BYTE REGPARM1 c64io2_read(WORD addr)
{
    io_source_list_t *current = &c64io2_head;
    int io_source_counter = 0;
    BYTE real_retval;
    BYTE retval = 0;

    vicii_handle_pending_alarms_external(0);

    while (current->next) {
        current = current->next;
        if (current->device->read != NULL) {
            if (addr >= current->device->start_address && addr <= current->device->end_address) {
                retval = current->device->read((WORD)(addr & current->device->address_mask));
                if (current->device->io_source_valid) {
                    real_retval = retval;
                    io_source_counter++;
                }
            } else {
                current->device->io_source_valid = 0;
            }
        }
    }

    if (io_source_counter == 0) {
        return vicii_read_phi1();
    }

    if (io_source_counter == 1) {
        return retval;
    }

    io_source_msg_detach(addr, io_source_counter, &c64io2_head);

    return vicii_read_phi1();
}

void REGPARM2 c64io2_store(WORD addr, BYTE value)
{
    io_source_list_t *current = &c64io2_head;

    vicii_handle_pending_alarms_external_write();

    while (current->next) {
        current = current->next;
        if (current->device->store != NULL) {
            if (addr >= current->device->start_address && addr <= current->device->end_address) {
                current->device->store((WORD)(addr & current->device->address_mask), value);
            }
        }
    }
}

/* ---------------------------------------------------------------------------------------------------------- */

void c64io_ioreg_add_list(struct mem_ioreg_list_s **mem_ioreg_list)
{
    io_source_list_t *current = &c64io1_head;

    if (current->next != NULL) {
        current = current->next;
        while (1) {
            mon_ioreg_add_list(mem_ioreg_list, current->device->name, current->device->start_address, current->device->end_address);
            if (current->next == NULL) {
                break;
            }
            current = current->next;
        }
    }

    current = &c64io2_head;
    
    if (current->next != NULL) {
        current = current->next;
        while (1) {
            mon_ioreg_add_list(mem_ioreg_list, current->device->name, current->device->start_address, current->device->end_address);
            if (current->next == NULL) {
                break;
            }
            current = current->next;
        }
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
