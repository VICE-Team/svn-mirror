/*
 * vic20io.h -- VIC20 io handling.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifndef VICE_VIC20IO_H
#define VICE_VIC20IO_H
 
#include "types.h"

#define IO_DETACH_CART     0
#define IO_DETACH_RESOURCE 1

#define IO_COLLISION_METHOD_DETACH_ALL    0
#define IO_COLLISION_METHOD_DETACH_LAST   1
#define IO_COLLISION_METHOD_AND_WIRES     2

extern BYTE vic20io2_read(WORD addr);
extern BYTE vic20io2_peek(WORD addr);
extern void vic20io2_store(WORD addr, BYTE value);
extern BYTE vic20io3_read(WORD addr);
extern BYTE vic20io3_peek(WORD addr);
extern void vic20io3_store(WORD addr, BYTE value);

struct mem_ioreg_list_s;
extern void io_source_ioreg_add_list(struct mem_ioreg_list_s **mem_ioreg_list);

typedef struct io_source_s {
    char *name; /* literal name of this i/o device */
    int detach_id;
    char *resource_name;
    WORD start_address;
    WORD end_address;
    WORD address_mask;
    int io_source_valid; /* after reading, is 1 if read was valid */
    void (*store)(WORD address, BYTE data);
    BYTE (*read)(WORD address);
    BYTE (*peek)(WORD address); /* read without side effects (used by monitor) */
    int (*dump)(void); /* print detailed state for this i/o device (used by monitor) */
    int cart_id; /* id of associated cartridge */
    int io_source_prio; /* 0: normal, 1: higher priority (no collisions), -1: lower priority (no collisions) */
    unsigned int order; /* a tag to indicate the order of insertion */
} io_source_t;

typedef struct io_source_list_s {
    struct io_source_list_s *previous;
    io_source_t *device;
    struct io_source_list_s *next;
} io_source_list_t;

typedef struct io_source_detach_s {
    int det_id;
    char *det_devname;
    char *det_name;
    int det_cartid;
    unsigned int order;
} io_source_detach_t;

extern io_source_list_t *io_source_register(io_source_t *device);
extern void io_source_unregister(io_source_list_t *device);

extern void vic20io_shutdown(void);

extern int vic20io_resources_init(void);
extern int vic20io_cmdline_options_init(void);

extern void vic20io_set_highest_order(unsigned int nr);

#endif
