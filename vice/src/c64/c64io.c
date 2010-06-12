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
#include <assert.h>

#include "c64io.h"
#include "cartridge.h"
#include "lib.h"
#include "log.h"
#include "monitor.h"
#include "resources.h"
#include "translate.h"
#include "types.h"
#include "uiapi.h"
#include "util.h"
#include "vicii-phi1.h"
#include "vicii.h"

/* #define IODEBUG */
/* #define IODEBUGRW */

#ifdef IODEBUG
#define DBG(x) printf x
#else
#define DBG(x)
#endif

#ifdef IODEBUGRW
#define DBGRW(x) printf x
#else
#define DBGRW(x)
#endif

static io_source_list_t c64io1_head = { NULL, NULL, NULL };
static io_source_list_t c64io2_head = { NULL, NULL, NULL };

static void io_source_detach(io_source_detach_t *source)
{
    switch (source->det_id) {
        case IO_DETACH_CART:
            if (source->det_cartid != CARTRIDGE_NONE) {
                /* FIXME: remove this check when all cart i/o device structs have been updated */
                if (source->det_cartid == 0) {
                    DBG(("IO: cart id in io struct is 0, it should be updated! name: %s\n", source->det_devname));
                    cartridge_detach_image(0); /* will detach the cartridge from main slot */
                } else {
                    DBG(("IO: io_source_detach id:%d name: %s\n", source->det_cartid, source->det_devname));
                    cartridge_detach_image(source->det_cartid);
                }
            }
            break;
        case IO_DETACH_RESOURCE:
            resources_set_int(source->det_name, 0);
            break;
    }
    ui_update_menus();
}

/*
    amount is 2 or more
*/
static void io_source_msg_detach(WORD addr, int amount, io_source_list_t *start)
{
    io_source_detach_t *detach_list = lib_malloc(sizeof(io_source_detach_t) * amount);
    io_source_list_t *current = start;
    char *old_msg = NULL;
    char *new_msg = NULL;
    int found = 0;
    int i = 0;

    current = current->next;

    DBG(("IO: check %d sources for addr %04x\n", amount, addr));
    while (current) {
        /* DBG(("IO: check '%s'\n", current->device->name)); */
        if (current->device->io_source_valid) {
            /* found a conflict */
            detach_list[i].det_id = current->device->detach_id;
            detach_list[i].det_name = current->device->resource_name;
            detach_list[i].det_devname = current->device->name;
            detach_list[i].det_cartid = current->device->cart_id;
            DBG(("IO: found '%s'\n", current->device->name));
            /* first part of the message "read collision at x from" */
            if (found == 0) {
                old_msg = lib_stralloc(translate_text(IDGS_IO_READ_COLL_AT_X_FROM));
                new_msg = util_concat(old_msg, current->device->name, NULL);
                lib_free(old_msg);
            }
            if ((found != amount - 1) && (found != 0)) {
                old_msg = new_msg;
                new_msg = util_concat(old_msg, ", ", current->device->name, NULL);
                lib_free(old_msg);
            }
            if (found == amount - 1) {
                old_msg = new_msg;
                new_msg = util_concat(old_msg, translate_text(IDGS_AND), current->device->name, translate_text(IDGS_ALL_DEVICES_DETACHED), NULL);
                lib_free(old_msg);
            }
            found++;
            if (found == amount) {
                break;
            }
        }
        current = current->next;
    }

    if (found) {
        log_message(LOG_DEFAULT, new_msg, addr);
        ui_error(new_msg, addr);
        lib_free(new_msg);

        DBG(("IO: detaching %d\n", found));
        for (i = 0; i < found; i++) {
            DBG(("IO: detach id:%d name: %s\n", detach_list[i].det_cartid, detach_list[i].det_devname));
            io_source_detach(&detach_list[i]);
        }
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

    assert(device != NULL);
    DBG(("IO: register id:%d name:%s\n", device->cart_id, device->name));

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

    assert(device != NULL);
    DBG(("IO: unregister id:%d name:%s\n", device->device->cart_id, device->device->name));

    prev = device->previous;
    prev->next = device->next;

    if (device->next) {
        device->next->previous = prev;
    }

    lib_free(device);
}

/* ---------------------------------------------------------------------------------------------------------- */

BYTE REGPARM1 c64io1_read(WORD addr)
{
    DBGRW(("IO: io1 r %04x\n", addr));
    return io_read(&c64io1_head, addr);
}

BYTE REGPARM1 c64io1_peek(WORD addr)
{
    DBGRW(("IO: io1 p %04x\n", addr));
    return io_peek(&c64io1_head, addr);
}

void REGPARM2 c64io1_store(WORD addr, BYTE value)
{
    DBGRW(("IO: io1 w %04x %02x\n", addr, value));
    io_store(&c64io1_head, addr, value);
}

BYTE REGPARM1 c64io2_read(WORD addr)
{
    DBGRW(("IO: io2 r %04x\n", addr));
    return io_read(&c64io2_head, addr);
}

BYTE REGPARM1 c64io2_peek(WORD addr)
{
    DBGRW(("IO: io2 p %04x\n", addr));
    return io_peek(&c64io2_head, addr);
}

void REGPARM2 c64io2_store(WORD addr, BYTE value)
{
    DBGRW(("IO: io2 w %04x %02x\n", addr, value));
    io_store(&c64io2_head, addr, value);
}

/* ---------------------------------------------------------------------------------------------------------- */

/* add all registered i/o devices to the list for the monitor */
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
